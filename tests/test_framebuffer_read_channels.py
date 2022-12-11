
def test_framebuffer_read_channels(ctx):
    fbo = ctx.framebuffer([
        ctx.renderbuffer((4, 4)),
    ])

    fbo.clear(65 / 255, 66 / 255, 67 / 255, 68 / 255)

    assert fbo.read(components=1) == b'A' * 16
    assert fbo.read(components=2) == b'AB' * 16
    assert fbo.read(components=3) == b'ABC' * 16
    assert fbo.read(components=4) == b'ABCD' * 16
