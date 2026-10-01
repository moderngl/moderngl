"""
Test the references objects hold to other objects they point at.

The vertex array holds on to its program and its index buffer, the scope to its
framebuffers and the samplers it uses. Like the reference to the context
(see test_context_refs.py) these are taken when the pointer is set and given
back when the object is deallocated, not when it is released. A released object
can still be called, it must not find a pointer to something that was freed.

The counts are observed through the reference count of the mgl object,
which sits at one element of a list so the call that reads it always adds the
same amount.
"""
import gc
import sys

import pytest

import moderngl

VERTEX_SHADER = """
    #version 330
    in vec2 in_vert;
    void main() {
        gl_Position = vec4(in_vert, 0.0, 1.0);
    }
"""


def count(box):
    gc.collect()
    return sys.getrefcount(box[0])


def refs(ctx):
    gc.collect()
    return sys.getrefcount(ctx.mglo)


@pytest.fixture
def prog(ctx_new):
    return ctx_new.program(vertex_shader=VERTEX_SHADER)


def test_vertex_array_program(ctx_new, prog):
    ctx = ctx_new
    box = [prog.mglo]
    base = count(box)

    vao = ctx.vertex_array(prog, [])
    raw = vao.mglo
    assert count(box) == base + 1

    vao.release()
    assert count(box) == base + 1  # Not given back by release()

    del vao, raw
    assert count(box) == base


def test_vertex_array_keeps_program_alive(ctx_new):
    """The program is not freed while a vertex array uses it, even after the program was released"""
    ctx = ctx_new
    base = refs(ctx)

    prog = ctx.program(vertex_shader=VERTEX_SHADER)
    vao = ctx.vertex_array(prog, [])
    raw_vao = vao.mglo
    vao.release()
    prog.release()
    del vao, prog

    # vertex array and program: each one has a reference to the context
    assert refs(ctx) == base + 2

    # The program is still there for the released vertex array to look at
    for _ in range(3):
        try:
            raw_vao.render(moderngl.TRIANGLES, 3, 0, 1)
        except moderngl.Error:
            pass

    del raw_vao
    assert refs(ctx) == base


def test_vertex_array_index_buffer(ctx_new, prog):
    ctx = ctx_new
    ibo = ctx.buffer(reserve=16)
    box = [ibo.mglo]
    base = count(box)

    vao = ctx.vertex_array(prog, [], index_buffer=ibo)
    raw = vao.mglo
    assert count(box) == base + 1

    vao.release()
    assert count(box) == base + 1  # Not given back by release()

    del vao, raw
    assert count(box) == base


def test_vertex_array_keeps_index_buffer_alive(ctx_new, prog):
    ctx = ctx_new
    ibo = ctx.buffer(reserve=16)
    vao = ctx.vertex_array(prog, [], index_buffer=ibo)
    raw_vao = vao.mglo
    base = refs(ctx)

    ibo.release()
    vao.release()
    del ibo, vao
    assert refs(ctx) == base  # The buffer is still alive, it holds a reference to the context

    for _ in range(3):
        try:
            raw_vao.render(moderngl.TRIANGLES, 3, 0, 1)
        except moderngl.Error:
            pass

    del raw_vao
    assert refs(ctx) == base - 2  # The vertex array and the buffer are gone


def test_vertex_array_without_index_buffer(ctx_new, prog):
    """The index buffer is None, which is also a reference"""
    ctx = ctx_new
    box = [prog.mglo]
    base = count(box)
    vao = ctx.vertex_array(prog, [])
    vao.release()
    del vao
    assert count(box) == base


def test_vertex_array_set_index_buffer(ctx_new, prog):
    """The setter takes the new buffer before it drops the old one"""
    ctx = ctx_new
    ibo1 = ctx.buffer(reserve=16)
    ibo2 = ctx.buffer(reserve=16)
    box1 = [ibo1.mglo]
    box2 = [ibo2.mglo]
    base1 = count(box1)
    base2 = count(box2)

    vao = ctx.vertex_array(prog, [], index_buffer=ibo1)
    raw = vao.mglo
    assert count(box1) == base1 + 1

    raw.index_buffer = ibo2.mglo
    assert count(box1) == base1
    assert count(box2) == base2 + 1

    raw.index_buffer = ibo2.mglo  # Same one again
    assert count(box2) == base2 + 1

    with pytest.raises(moderngl.Error):
        raw.index_buffer = None
    assert count(box2) == base2 + 1

    vao.release()
    assert count(box2) == base2 + 1
    del vao, raw
    assert count(box1) == base1
    assert count(box2) == base2


def test_vertex_array_set_index_buffer_replaces_none(ctx_new, prog):
    ctx = ctx_new
    ibo = ctx.buffer(reserve=16)
    box = [ibo.mglo]
    base = count(box)

    vao = ctx.vertex_array(prog, [])
    raw = vao.mglo
    raw.index_buffer = ibo.mglo
    assert count(box) == base + 1
    vao.release()
    assert count(box) == base + 1
    del vao, raw
    assert count(box) == base


@pytest.fixture
def fbo(ctx_new):
    return ctx_new.framebuffer(ctx_new.renderbuffer((4, 4)))


def test_scope_framebuffer(ctx_new, fbo):
    ctx = ctx_new
    box = [fbo.mglo]
    base = count(box)

    scope = ctx.scope(framebuffer=fbo)
    raw = scope.mglo
    assert count(box) == base + 1

    scope.release()
    assert count(box) == base + 1  # Not given back by release()

    del scope, raw
    assert count(box) == base


def test_scope_old_framebuffer(ctx_new, fbo):
    """The scope also holds the framebuffer that was bound when it was created"""
    ctx = ctx_new
    other = ctx.framebuffer(ctx.renderbuffer((4, 4)))
    other.use()
    box = [other.mglo]
    base = count(box)

    scope = ctx.scope(framebuffer=fbo)
    raw = scope.mglo
    assert count(box) == base + 1  # The context has it as the bound one and so does the scope

    fbo.use()
    assert count(box) == base  # The context let go of it, the scope did not

    scope.release()
    assert count(box) == base
    del scope, raw
    assert count(box) == base - 1


def test_scope_keeps_framebuffers_alive(ctx_new):
    ctx = ctx_new
    base = refs(ctx)

    rbo = ctx.renderbuffer((4, 4))
    fbo = ctx.framebuffer(rbo)
    scope = ctx.scope(framebuffer=fbo)
    raw = scope.mglo
    scope.release()
    fbo.release()
    rbo.release()
    del scope, fbo, rbo

    # Only the framebuffer is left (the scope holds it), and the scope
    assert refs(ctx) == base + 2

    # The framebuffers of the released scope are still there
    for _ in range(3):
        raw.begin()
        raw.end()

    del raw
    assert refs(ctx) == base


def test_scope_samplers(ctx_new, fbo):
    ctx = ctx_new
    sampler = ctx.sampler()
    box = [sampler]
    base = count(box)

    scope = ctx.scope(framebuffer=fbo, samplers=[(sampler, 0)])
    raw = scope.mglo
    assert count(box) == base + 2  # The scope and the list the wrapper keeps

    scope.release()
    assert count(box) == base + 1  # The wrapper let go of its list, the scope did not
    del scope
    assert count(box) == base + 1

    raw.release()
    assert count(box) == base + 1
    del raw
    assert count(box) == base
    sampler.release()


def test_scope_invalid_samplers(ctx_new, fbo):
    """A scope that cannot be created is freed, and what it took with it"""
    ctx = ctx_new
    sampler = ctx.sampler()
    box = [sampler]
    base_ctx = refs(ctx)
    base = count(box)

    with pytest.raises(moderngl.Error, match="invalid samplers"):
        ctx.mglo.scope(fbo.mglo, None, (), (), (), ((sampler, 0), "wrong"))

    assert count(box) == base
    assert refs(ctx) == base_ctx
    sampler.release()


def test_scope_invalid_framebuffer_count(ctx_new, fbo):
    ctx = ctx_new
    box = [fbo.mglo]
    base = count(box)
    base_ctx = refs(ctx)

    with pytest.raises(moderngl.Error, match="invalid textures"):
        ctx.mglo.scope(fbo.mglo, None, ("wrong",), (), (), ())

    assert count(box) == base
    assert refs(ctx) == base_ctx
