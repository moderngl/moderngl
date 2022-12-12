import pytest

def test_renderbuffer(ctx):
    rbo = ctx.renderbuffer((4, 4))
    assert rbo.size == (4, 4)
    assert rbo.samples == 0
    assert rbo.depth == False

def test_multisample_renderbuffer(ctx):
    if ctx.max_samples < 2:
        pytest.skip('multisampling is not supported')

    rbo = ctx.renderbuffer((4, 4), samples=2)
    assert rbo.size == (4, 4)
    assert rbo.samples == 2
    assert rbo.depth is False

def test_depth_renderbuffer(ctx):
    rbo = ctx.depth_renderbuffer((4, 4))
    assert rbo.size == (4, 4)
    assert rbo.samples == 0
    assert rbo.depth is True

def test_multisample_depth_renderbuffer(ctx):
    if ctx.max_samples < 2:
        pytest.skip('multisampling is not supported')

    rbo = ctx.depth_renderbuffer((4, 4), samples=2)
    assert rbo.size == (4, 4)
    assert rbo.samples == 2
    assert rbo.depth is True

def test_renderbuffer_invalid_samples(ctx):
    if ctx.max_samples < 2:
        pytest.skip('multisampling is not supported')

    with pytest.raises(Exception, match='samples is invalid'):
        ctx.renderbuffer((4, 4), samples=3)
