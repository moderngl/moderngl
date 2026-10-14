import pytest


def test_current_framebuffer_readback(ctx_new):
    ctx = ctx_new
    previous = ctx.simple_framebuffer((8, 8))
    target = ctx.simple_framebuffer((8, 8))
    previous.use()
    ctx.clear(0, 1, 0, 1)

    with ctx.scope(framebuffer=target):
        ctx.clear(1, 0, 0, 1)
        assert ctx.fbo.read(components=3) == b'\xff\x00\x00' * 64
        assert ctx.fbo is target

    assert ctx.fbo is previous
    assert ctx.fbo.read(components=3) == b'\x00\xff\x00' * 64


def test_nested_framebuffers(ctx_new):
    ctx = ctx_new
    previous = ctx.simple_framebuffer((8, 8))
    outer = ctx.simple_framebuffer((6, 6))
    inner = ctx.simple_framebuffer((4, 4))
    previous.use()
    with ctx.scope(framebuffer=outer):
        assert ctx.fbo is outer
        with ctx.scope(framebuffer=inner):
            ctx.clear(0, 0, 1, 1)
            assert ctx.fbo.read(components=3) == b'\x00\x00\xff' * 16
        assert ctx.fbo is outer
        assert ctx.viewport == (0, 0, 6, 6)
    assert ctx.fbo is previous
    assert ctx.viewport == (0, 0, 8, 8)


def test_scope_reuse_and_exception(ctx_new):
    ctx = ctx_new
    first = ctx.simple_framebuffer((8, 8))
    second = ctx.simple_framebuffer((6, 6))
    target = ctx.simple_framebuffer((4, 4))
    scope = ctx.scope(framebuffer=target)
    for previous in (first, second):
        previous.use()
        with pytest.raises(ValueError, match='render failed'):
            with scope:
                assert ctx.fbo is target
                raise ValueError('render failed')
        assert ctx.fbo is previous
        assert ctx.viewport == (0, 0, *previous.size)


def test_scope_without_previous_wrapper(ctx_new):
    ctx = ctx_new
    assert ctx.fbo is None
    target = ctx.simple_framebuffer((4, 4))
    with ctx.scope(framebuffer=target):
        assert ctx.fbo is target
    assert ctx.fbo is None


def test_framebuffer_change_inside_scope(ctx_new):
    ctx = ctx_new
    previous = ctx.simple_framebuffer((8, 8))
    target = ctx.simple_framebuffer((6, 6))
    other = ctx.simple_framebuffer((4, 4))
    previous.use()
    with ctx.scope(framebuffer=target):
        assert ctx.fbo is target
        other.use()
        assert ctx.fbo is other
    assert ctx.fbo is previous
    assert ctx.viewport == (0, 0, 8, 8)
