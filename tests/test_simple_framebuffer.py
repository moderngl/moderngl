import moderngl
import pytest

def test_framebuffer_color_attachment(ctx):
    rbo = ctx.renderbuffer((16, 16))
    ctx.framebuffer(rbo)

def test_framebuffer_get_color_attachments(ctx):
    rbo1 = ctx.renderbuffer((16, 16))
    rbo2 = ctx.renderbuffer((16, 16))
    rbo3 = ctx.renderbuffer((16, 16))

    fbo1 = ctx.framebuffer(rbo1)
    fbo2 = ctx.framebuffer([rbo2, rbo1])
    fbo3 = ctx.framebuffer([rbo1, rbo2, rbo3])

    assert len(fbo1.color_attachments) == 1
    assert len(fbo2.color_attachments) == 2
    assert len(fbo3.color_attachments) == 3

    assert isinstance(fbo1.color_attachments[0], moderngl.Renderbuffer)
    assert isinstance(fbo2.color_attachments[0], moderngl.Renderbuffer)
    assert isinstance(fbo2.color_attachments[1], moderngl.Renderbuffer)
    assert isinstance(fbo3.color_attachments[0], moderngl.Renderbuffer)
    assert isinstance(fbo3.color_attachments[1], moderngl.Renderbuffer)
    assert isinstance(fbo3.color_attachments[2], moderngl.Renderbuffer)

    assert rbo1 in fbo1.color_attachments
    assert rbo2 in fbo2.color_attachments
    assert rbo2 in fbo3.color_attachments

    assert rbo2 not in fbo1.color_attachments
    assert rbo3 not in fbo1.color_attachments
    assert rbo3 not in fbo2.color_attachments

def test_framebuffer_get_depth_attachment(ctx):
    rbo1 = ctx.renderbuffer((16, 16))
    rbo2 = ctx.depth_renderbuffer((16, 16))

    fbo1 = ctx.framebuffer(rbo1)
    fbo2 = ctx.framebuffer(rbo1, rbo2)

    assert fbo1.depth_attachment is None
    assert isinstance(fbo2.depth_attachment, moderngl.Renderbuffer)

    assert fbo1.depth_attachment != rbo2
    assert fbo2.depth_attachment == rbo2

def test_framebuffer_color_mask(ctx):
    fbo = ctx.framebuffer(ctx.renderbuffer((16, 16)))
    assert fbo.color_mask == (True, True, True, True)

def test_framebuffer_single_channel_color_mask(ctx):
    fbo = ctx.framebuffer(ctx.renderbuffer((16, 16), components=1))
    assert fbo.color_mask == (True, False, False, False)

def test_framebuffer_mixed_channels_color_mask(ctx):
    fbo = ctx.framebuffer([
        ctx.renderbuffer((16, 16), components=1),
        ctx.renderbuffer((16, 16), components=2),
        ctx.renderbuffer((16, 16), components=3),
        ctx.renderbuffer((16, 16), components=4),
    ])
    expected = (
        (True, False, False, False),
        (True, True, False, False),
        (True, True, True, False),
        (True, True, True, True),
    )
    assert fbo.color_mask == expected

def test_framebuffer_depth_mask_1(ctx):
    fbo = ctx.framebuffer(
        ctx.renderbuffer((16, 16)),
        ctx.depth_renderbuffer((16, 16)),
    )
    assert fbo.depth_mask == True

def test_framebuffer_depth_mask_2(ctx):
    fbo = ctx.framebuffer(
        ctx.renderbuffer((16, 16))
    )
    assert fbo.depth_mask is False

def test_framebuffer_color_attachments(ctx):
    rbo1 = ctx.renderbuffer((16, 16))
    rbo2 = ctx.renderbuffer((16, 16))
    rbo3 = ctx.renderbuffer((16, 16))
    ctx.framebuffer([rbo1, rbo2, rbo3])

def test_framebuffer_multiple_color_masks(ctx):
    fbo = ctx.framebuffer([
        ctx.renderbuffer((16, 16)),
        ctx.renderbuffer((16, 16)),
    ])
    expected = ((True, True, True, True), (True, True, True, True))
    assert fbo.color_mask == expected

def test_framebuffer_size_mismatch(ctx):
    with pytest.raises(moderngl.Error, match='size'):
        rbo1 = ctx.renderbuffer((16, 16))
        rbo2 = ctx.depth_renderbuffer((32, 32))
        ctx.framebuffer(rbo1, rbo2)

def test_framebuffer_color_attachments_size_mismatch(ctx):
    with pytest.raises(moderngl.Error, match='size'):
        rbo1 = ctx.renderbuffer((16, 16))
        rbo2 = ctx.renderbuffer((32, 32))
        ctx.framebuffer([rbo1, rbo2])

def test_depth_framebuffer(ctx):
    rbo1 = ctx.renderbuffer((16, 16))
    rbo2 = ctx.depth_renderbuffer((16, 16))
    ctx.framebuffer(rbo1, rbo2)

def test_framebuffer_multisample(ctx):
    if ctx.max_samples < 2:
        pytest.skip('multisampling is not supported')

    rbo1 = ctx.renderbuffer((16, 16), samples=2)
    ctx.framebuffer(rbo1)

def test_depth_framebuffer_multisample(ctx):
    if ctx.max_samples < 2:
        pytest.skip('multisampling is not supported')

    rbo1 = ctx.renderbuffer((16, 16), samples=2)
    rbo2 = ctx.depth_renderbuffer((16, 16), samples=2)
    ctx.framebuffer(rbo1, rbo2)

def test_framebuffer_multisample_sample_mismatch(ctx):
    if ctx.max_samples < 2:
        pytest.skip('multisampling is not supported')

    with pytest.raises(moderngl.Error, match='sample'):
        rbo1 = ctx.renderbuffer((16, 16))
        rbo2 = ctx.depth_renderbuffer((16, 16), samples=2)
        ctx.framebuffer(rbo1, rbo2)

def test_empty_framebuffer(ctx):
    with pytest.raises(moderngl.Error, match='empty'):
        ctx.framebuffer([])

def test_framebuffer_having_depth_in_colors(ctx):
    with pytest.raises(moderngl.Error, match=r'(color|depth)'):
        ctx.framebuffer(ctx.depth_renderbuffer((16, 16)))

def test_framebuffer_having_color_in_depth(ctx):
    with pytest.raises(moderngl.Error, match=r'(color|depth)'):
        ctx.framebuffer(
            ctx.renderbuffer((16, 16)),
            ctx.renderbuffer((16, 16)),
        )
