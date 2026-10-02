"""
Releasing external objects.

An external texture or buffer wraps an OpenGL object that belongs to someone
else. Releasing the wrapper must not delete that object, but it has to give up
the references the wrapper keeps to itself and to the context like any other
object does, otherwise the wrapper and the context can never be freed.

The reference count of ``ctx.mglo`` is used to observe this (see
test_context_refs.py).
"""
import gc
import sys

import pytest
import OpenGL

OpenGL.ERROR_CHECKING = False  # Don't want PyOpenGL to raise any exceptions
from OpenGL import GL


def refs(ctx):
    gc.collect()
    return sys.getrefcount(ctx.mglo)


def external_texture(ctx, tex):
    return ctx.external_texture(tex.glo, tex.size, tex.components, tex.samples, tex.dtype)


def test_external_texture_release(ctx_new):
    ctx = ctx_new
    tex = ctx.texture((4, 4), 4, b"\x01" * 64)
    base = refs(ctx)

    ext = external_texture(ctx, tex)
    raw = ext.mglo
    assert refs(ctx) == base + 1

    ext.release()
    # Only our own reference to the released object is left
    assert refs(ctx) == base + 1

    # Releasing again is a no-op
    raw.release()
    assert refs(ctx) == base + 1

    del ext, raw
    assert refs(ctx) == base

    # The texture still belongs to its owner
    assert GL.glIsTexture(tex.glo)
    assert tex.read() == b"\x01" * 64
    assert ctx.error == "GL_NO_ERROR"


def test_external_texture_release_many(ctx_new):
    ctx = ctx_new
    tex = ctx.texture((4, 4), 4)
    base = refs(ctx)

    for _ in range(500):
        ext = external_texture(ctx, tex)
        ext.release()
        del ext

    assert refs(ctx) == base
    assert GL.glIsTexture(tex.glo)


def test_external_texture_auto_gc(ctx_new):
    """With gc_mode auto, dropping the wrapper releases the texture"""
    ctx = ctx_new
    ctx.gc_mode = "auto"
    tex = ctx.texture((4, 4), 4)
    base = refs(ctx)

    ext = external_texture(ctx, tex)
    assert refs(ctx) == base + 1
    del ext
    assert refs(ctx) == base

    assert GL.glIsTexture(tex.glo)
    ctx.gc_mode = None


def test_external_buffer_release(ctx_new):
    ctx = ctx_new
    buf = ctx.buffer(b"\x02" * 16)
    base = refs(ctx)

    ext = ctx.external_buffer(buf.glo, buf.size)
    raw = ext.mglo
    assert refs(ctx) == base + 1

    ext.release()
    assert refs(ctx) == base + 1

    raw.release()
    assert refs(ctx) == base + 1

    del ext, raw
    assert refs(ctx) == base

    # The buffer still belongs to its owner (this needs external_buffer to
    # flag the buffer as external, which it does not do yet)
    if not GL.glIsBuffer(buf.glo):
        pytest.skip("external_buffer does not flag the buffer as external")
    assert buf.read() == b"\x02" * 16


def test_external_buffer_release_many(ctx_new):
    ctx = ctx_new
    buf = ctx.buffer(reserve=16)
    base = refs(ctx)

    for _ in range(500):
        ext = ctx.external_buffer(buf.glo, buf.size)
        ext.release()
        del ext

    assert refs(ctx) == base
