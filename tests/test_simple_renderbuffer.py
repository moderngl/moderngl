import moderngl
import pytest

def test_renderbuffer(ctx):
    ctx.renderbuffer((64, 64))

def test_multisample_renderbuffer(ctx):
    if ctx.max_samples < 2:
        pytest.skip('multisampling is not supported')

    ctx.renderbuffer((64, 64), samples=2)

def test_depth_renderbuffer(ctx):
    ctx.depth_renderbuffer((64, 64))

def test_multisample_depth_renderbuffer(ctx):
    if ctx.max_samples < 2:
        pytest.skip('multisampling is not supported')

    ctx.depth_renderbuffer((64, 64), samples=2)

def test_renderbuffer_invalid_samples(ctx):
    with pytest.raises(moderngl.Error, match='sample'):
        ctx.renderbuffer((64, 64), samples=3)
