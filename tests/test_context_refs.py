"""
Test the references objects hold to the context they were created from.

Every object created from a context owns exactly one reference to the
underlying mgl.Context from its creation until it is deallocated.
release() frees the OpenGL object only, it does not give the reference back.

The reference count of ``ctx.mglo`` is used to observe this. The absolute value
depends on how many wrappers point at the context, so most tests only look at
differences to a baseline taken after the context was created.

We use ctx_new (a fresh context per test) so the baselines are not disturbed
by objects other tests left behind.
"""
import gc
import sys
import weakref

import pytest
from glcontext import egl

import moderngl

VERTEX_SHADER = """
    #version 330
    void main() {
        gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
    }
"""

COMPUTE_SHADER = """
    #version 430
    layout (local_size_x = 1, local_size_y = 1) in;
    void main() {}
"""


def refs(ctx):
    """Reference count of the mgl.Context (the argument adds a constant)"""
    gc.collect()
    return sys.getrefcount(ctx.mglo)


def refs_of(box):
    """
    Reference count of the object in a one element list.
    We cannot pass a local variable, Python 3.14 does not always count
    the references a local variable has to the argument of the call.
    """
    return sys.getrefcount(box[0])


def _framebuffer(ctx):
    rbo = ctx.renderbuffer((4, 4))
    return [rbo, ctx.framebuffer(rbo)]


def _scope(ctx):
    rbo = ctx.renderbuffer((4, 4))
    fbo = ctx.framebuffer(rbo)
    return [rbo, fbo, ctx.scope(framebuffer=fbo)]


def _vertex_array(ctx):
    prog = ctx.program(vertex_shader=VERTEX_SHADER)
    return [prog, ctx.vertex_array(prog, [])]


def _vertex_array_index_buffer(ctx):
    prog = ctx.program(vertex_shader=VERTEX_SHADER)
    ibo = ctx.buffer(reserve=16)
    return [prog, ibo, ctx.vertex_array(prog, [], index_buffer=ibo)]


def _compute_shader(ctx):
    if ctx.version_code < 430:
        pytest.skip("compute shaders not supported")
    return [ctx.compute_shader(COMPUTE_SHADER)]


# Every creation path for objects that have a release() method.
# Each factory returns the list of all wrappers it created.
RELEASABLE = {
    "buffer": lambda ctx: [ctx.buffer(reserve=16)],
    "texture": lambda ctx: [ctx.texture((4, 4), 4)],
    "texture_ms": lambda ctx: [ctx.texture((4, 4), 4, samples=4)],
    "depth_texture": lambda ctx: [ctx.depth_texture((4, 4))],
    "texture_array": lambda ctx: [ctx.texture_array((4, 4, 4), 4)],
    "texture3d": lambda ctx: [ctx.texture3d((4, 4, 4), 4)],
    "texture_cube": lambda ctx: [ctx.texture_cube((4, 4), 4)],
    "depth_texture_cube": lambda ctx: [ctx.depth_texture_cube((4, 4))],
    "renderbuffer": lambda ctx: [ctx.renderbuffer((4, 4))],
    "depth_renderbuffer": lambda ctx: [ctx.depth_renderbuffer((4, 4))],
    "framebuffer": _framebuffer,
    "empty_framebuffer": lambda ctx: [ctx.empty_framebuffer((4, 4))],
    "program": lambda ctx: [ctx.program(vertex_shader=VERTEX_SHADER)],
    "compute_shader": _compute_shader,
    "vertex_array": _vertex_array,
    "vertex_array_index_buffer": _vertex_array_index_buffer,
    "sampler": lambda ctx: [ctx.sampler()],
    "scope": _scope,
}

# Objects that have no release() and are freed by dropping the last reference
QUERIES = {
    "query": lambda ctx: [ctx.query(samples=True, time=True)],
    "query_primitives": lambda ctx: [ctx.query(primitives=True)],
}


@pytest.fixture(params=sorted(RELEASABLE))
def make(request):
    return RELEASABLE[request.param]


def test_release_keeps_context_until_dealloc(ctx_new, make):
    """
    The context reference is not given back by release(), only when the
    object is deallocated. Then the count is back to where it started.
    """
    ctx = ctx_new
    base = refs(ctx)

    objs = make(ctx)
    raw = [obj.mglo for obj in objs]
    assert refs(ctx) == base + len(objs)

    for obj in objs:
        obj.release()

    # The wrappers dropped their mglo, but we still hold the released objects
    assert refs(ctx) == base + len(raw)

    # Releasing again is a no-op
    for obj in raw:
        obj.release()
    assert refs(ctx) == base + len(raw)

    del objs, obj, raw
    assert refs(ctx) == base


def test_release_then_drop_wrapper(ctx_new, make):
    """The usual path: release the wrapper and drop it"""
    ctx = ctx_new
    base = refs(ctx)

    objs = make(ctx)
    for obj in objs:
        obj.release()
    del objs, obj

    assert refs(ctx) == base
    assert ctx.error == "GL_NO_ERROR"


def test_unreleased_objects_keep_themselves_alive(ctx_new, make):
    """
    Dropping a wrapper without release() keeps the underlying object (and so
    the context) alive until release() is called on it.
    """
    ctx = ctx_new
    base = refs(ctx)

    objs = make(ctx)
    raw = [obj.mglo for obj in objs]
    count = len(raw)
    assert refs(ctx) == base + count

    del objs
    assert refs(ctx) == base + count

    for obj in raw:
        obj.release()
    del raw, obj
    assert refs(ctx) == base


def test_repeated_creation(ctx_new, make):
    """No drift when creating and releasing many objects"""
    ctx = ctx_new
    base = refs(ctx)

    for _ in range(50):
        for obj in make(ctx):
            obj.release()

    obj = None
    assert refs(ctx) == base


@pytest.mark.parametrize("name", sorted(QUERIES))
def test_query(ctx_new, name):
    """Queries have no release(), they are freed with the last reference"""
    ctx = ctx_new
    base = refs(ctx)

    objs = QUERIES[name](ctx)
    assert refs(ctx) == base + len(objs)

    del objs
    assert refs(ctx) == base


def test_sampler_release_many(ctx_new):
    """
    MGLSampler.release() used to touch the sampler after dropping the
    reference that kept it alive.
    """
    ctx = ctx_new
    base = refs(ctx)

    for _ in range(2000):
        sampler = ctx.sampler()
        raw = sampler.mglo
        sampler.release()
        raw.release()
        del sampler, raw

    assert refs(ctx) == base
    assert ctx.error == "GL_NO_ERROR"


def test_detect_framebuffer_default(ctx_new):
    """The default framebuffer can be released and the context keeps working"""
    ctx = ctx_new
    base = refs(ctx)

    fbo = ctx.detect_framebuffer()
    fbo.release()
    fbo.release()
    del fbo
    assert refs(ctx) == base

    # Detect it again after it was released once
    fbo = ctx.detect_framebuffer(0)
    fbo.release()
    del fbo
    assert refs(ctx) == base

    buf = ctx.buffer(b"\x01\x02\x03\x04")
    assert buf.read() == b"\x01\x02\x03\x04"
    buf.release()
    assert refs(ctx) == base
    assert ctx.error == "GL_NO_ERROR"


def test_detect_framebuffer_glo(ctx_new):
    """
    A framebuffer detected by glo is a new object. It owns a context reference
    like any other framebuffer. The old release() dropped one that was never taken.
    """
    ctx = ctx_new
    rbo = ctx.renderbuffer((4, 4))
    fbo = ctx.framebuffer(rbo)
    base = refs(ctx)

    detected = ctx.detect_framebuffer(fbo.glo)
    assert refs(ctx) == base + 1
    assert detected.size == (4, 4)
    assert detected.samples == 0  # objects are zero initialized
    detected.release()  # deletes the OpenGL framebuffer
    del detected
    assert refs(ctx) == base

    fbo.release()
    rbo.release()
    del fbo, rbo
    assert refs(ctx) == base - 2

    buf = ctx.buffer(b"\x01\x02\x03\x04")
    assert buf.read() == b"\x01\x02\x03\x04"
    buf.release()
    assert ctx.error == "GL_NO_ERROR"


def test_context_outlives_release_while_objects_exist(ctx_new):
    """
    ctx.release() destroys the OpenGL context, but the mgl.Context stays
    allocated for as long as there are objects that point at it.
    """
    ctx = ctx_new
    box = [ctx.mglo]

    buf = ctx.buffer(reserve=16)
    raw = buf.mglo
    buf.release()
    del buf

    ctx.release()
    assert refs_of(box) == 3  # box, the argument, raw

    del raw
    assert refs_of(box) == 2  # box, the argument


def test_context_outlives_release_default_framebuffer(ctx_new):
    """
    The context and its default framebuffer reference each other.
    release() breaks that so the context can go away.
    """
    ctx = ctx_new
    box = [ctx.mglo]
    fbo = ctx.detect_framebuffer()  # the default framebuffer

    ctx.release()
    # Still alive, the framebuffer owns a reference
    assert refs_of(box) == 3  # box, the argument, the framebuffer

    fbo.release()
    del fbo
    assert refs_of(box) == 2  # box, the argument


class _LoaderProxy:
    """Forwards to a real glcontext, but can be weakly referenced"""

    def __init__(self, backend):
        self._backend = backend

    def __getattr__(self, name):
        return getattr(self._backend, name)


@pytest.mark.parametrize("release_screen", [True, False])
def test_context_is_freed(release_screen):
    """
    Nothing keeps the mgl.Context alive once it is released and all objects are gone.
    The default framebuffer does not have to be released by hand.
    """
    backend = egl.create_context(glversion=330, mode="standalone")
    proxy = _LoaderProxy(backend)
    alive = weakref.ref(proxy)

    ctx = moderngl.create_context(standalone=True, context=proxy)
    del proxy

    objs = [
        ctx.buffer(reserve=16),
        ctx.texture((4, 4), 4),
        ctx.sampler(),
    ]
    raw = [obj.mglo for obj in objs]
    for obj in objs:
        obj.release()

    screen = ctx.detect_framebuffer()  # the default framebuffer
    if release_screen:
        screen.release()

    ctx.release()
    gc.collect()
    assert alive() is not None  # still referenced by the released objects

    del objs, obj, raw
    gc.collect()
    if not release_screen:
        assert alive() is not None  # and by the default framebuffer we hold

    del screen, ctx
    gc.collect()
    assert alive() is None
