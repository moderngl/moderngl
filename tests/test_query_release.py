"""
Queries own OpenGL query objects (up to four of them, one for each kind of
result) which have to be deleted when the query is released.

glIsQuery only reports names that were used by a query (a name that was only
generated is not a query object yet), so the queries are run once.
"""
import gc

import pytest
import OpenGL

OpenGL.ERROR_CHECKING = False  # Don't want PyOpenGL to raise any exceptions
from OpenGL import GL

QUERIES = {
    "samples": dict(samples=True),
    "any_samples": dict(any_samples=True),
    "time": dict(time=True),
    "primitives": dict(primitives=True),
    "all": dict(samples=True, any_samples=True, time=True, primitives=True),
    "default": dict(),
}


def query_names():
    """The names of all query objects that exist in the current context"""
    return {name for name in range(1, 1024) if GL.glIsQuery(name)}


def run(query):
    with query:
        pass
    # Some combinations of queries cannot run at the same time (samples and any_samples)
    # which is not what is tested here
    query.ctx.clear_errors()


@pytest.mark.parametrize("name", sorted(QUERIES))
def test_release_deletes_queries(ctx_new, name):
    ctx = ctx_new
    before = query_names()

    query = ctx.query(**QUERIES[name])
    run(query)
    created = query_names() - before
    assert created

    query.release()
    assert query_names() == before

    # Releasing twice is fine
    query.release()
    assert query_names() == before
    assert ctx.error == "GL_NO_ERROR"


def test_release_through_mglo(ctx_new):
    ctx = ctx_new
    before = query_names()

    query = ctx.query(samples=True, time=True)
    raw = query.mglo
    run(query)
    assert query_names() != before

    raw.release()
    assert query_names() == before
    raw.release()


def test_release_active_query(ctx_new):
    ctx = ctx_new
    before = query_names()

    query = ctx.query(samples=True, time=True)
    query.mglo.begin()
    query.release()

    assert query_names() == before
    assert ctx.error == "GL_NO_ERROR"


def test_gc_mode_none_keeps_queries_until_released(ctx_new):
    """Like every other object, nothing is deleted behind the back of the user"""
    ctx = ctx_new
    before = query_names()

    query = ctx.query(samples=True)
    run(query)
    created = query_names() - before
    assert created

    raw = query.mglo
    del query
    gc.collect()
    assert query_names() - before == created

    raw.release()
    assert query_names() == before


def test_gc_mode_auto(ctx_new):
    ctx = ctx_new
    ctx.gc_mode = "auto"
    before = query_names()

    for _ in range(100):
        query = ctx.query(samples=True, any_samples=True, time=True, primitives=True)
        run(query)
        assert query_names() != before
        del query
        assert query_names() == before

    assert ctx.error == "GL_NO_ERROR"
    ctx.gc_mode = None


def test_gc_mode_context_gc(ctx_new):
    ctx = ctx_new
    ctx.gc_mode = "context_gc"
    before = query_names()

    query = ctx.query(samples=True, time=True)
    run(query)
    del query
    gc.collect()
    assert query_names() != before

    assert ctx.gc() == 1
    assert query_names() == before

    # An explicitly released query leaves nothing for the next gc()
    query = ctx.query(samples=True)
    run(query)
    query.release()
    del query
    gc.collect()
    assert ctx.gc() == 0
    assert query_names() == before
    ctx.gc_mode = None


def test_many_queries(ctx_new):
    """The number of query objects does not grow"""
    ctx = ctx_new
    before = query_names()

    for _ in range(500):
        query = ctx.query(samples=True, time=True)
        run(query)
        query.release()
        del query

    assert query_names() == before


def test_release_after_context_release(ctx_new):
    """The OpenGL objects are gone with the context, there is nothing to delete"""
    ctx = ctx_new
    query = ctx.query(samples=True, time=True)
    raw = query.mglo
    ctx.release()

    query.release()
    raw.release()
    del query, raw
