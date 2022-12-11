
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
