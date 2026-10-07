import moderngl
import pytest


def test_1(ctx):
    fbo = ctx.framebuffer(ctx.renderbuffer((4, 4)))

    fbo.clear(0.0, 0.0, 0.0, 0.0)

    assert fbo.read(components=4) == b'\x00\x00\x00\x00' * 16

    fbo.color_mask = (True, False, True, False)
    fbo.clear(0x19 / 255, 0x33 / 255, 0x4c / 255, 0x66 / 255)

    assert fbo.read(components=4) == b'\x19\x00\x4c\x00' * 16

    fbo.color_mask = (False, True, False, True)
    fbo.clear(0x7f / 255, 0x99 / 255, 0xb2 / 255, 0xcc / 255)

    assert fbo.read(components=4) == b'\x19\x99\x4c\xcc' * 16


def test_2(ctx):
    fbo = ctx.framebuffer([
        ctx.renderbuffer((4, 4)),
        ctx.renderbuffer((4, 4)),
    ])

    fbo.clear(1.0, 1.0, 1.0, 1.0)

    assert fbo.read(components=4, attachment=0) == b'\xff\xff\xff\xff' * 16
    assert fbo.read(components=4, attachment=1) == b'\xff\xff\xff\xff' * 16

    fbo.color_mask = (
        (True, False, True, False),
        (False, True, False, True),
    )

    fbo.clear(0x19 / 255, 0x33 / 255, 0x4c / 255, 0x66 / 255)

    assert fbo.read(components=4, attachment=0) == b'\x19\xff\x4c\xff' * 16
    assert fbo.read(components=4, attachment=1) == b'\xff\x33\xff\x66' * 16

    fbo.color_mask = (
        (False, True, False, True),
        (True, False, True, False),
    )

    fbo.clear(0x7f / 255, 0x99 / 255, 0xb2 / 255, 0xcc / 255)

    assert fbo.read(components=4, attachment=0) == b'\x19\x99\x4c\xcc' * 16
    assert fbo.read(components=4, attachment=1) == b'\x7f\x33\xb2\x66' * 16


def test_color_mask_too_many_masks(ctx):
    """A color mask sequence longer than the internal color mask array is rejected"""
    fbo = ctx.framebuffer([
        ctx.renderbuffer((4, 4)),
        ctx.renderbuffer((4, 4)),
    ])
    fbo.clear(0.0, 0.0, 0.0, 0.0)
    valid = (
        (True, False, True, False),
        (False, True, False, True),
    )
    fbo.color_mask = valid

    # Used to write past the end of the internal 64 element color mask array
    for count in (65, 1000):
        with pytest.raises(moderngl.Error):
            fbo.color_mask = ((True, True, True, True),) * count

    # A rejected assignment leaves the previous state intact
    assert fbo.color_mask == valid
    fbo.clear(1.0, 1.0, 1.0, 1.0)
    assert fbo.read(components=4, attachment=0) == b'\xff\x00\xff\x00' * 16
    assert fbo.read(components=4, attachment=1) == b'\x00\xff\x00\xff' * 16


def test_color_mask_invalid_entry(ctx):
    """An invalid entry is reported as an error"""
    fbo = ctx.framebuffer([
        ctx.renderbuffer((4, 4)),
        ctx.renderbuffer((4, 4)),
    ])
    with pytest.raises(moderngl.Error):
        fbo.color_mask = ((True, True, True, True), (True, True))


def test_color_mask_no_color_attachments(ctx):
    """A framebuffer without color attachments still accepts a mask"""
    fbo = ctx.framebuffer(depth_attachment=ctx.depth_renderbuffer((4, 4)))
    fbo.color_mask = ((True, False, True, False),)
