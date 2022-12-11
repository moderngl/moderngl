
def test_1(ctx):
    rbo1 = ctx.renderbuffer((4, 4), dtype='f1')
    rbo2 = ctx.renderbuffer((4, 4))
    rbo3 = ctx.renderbuffer((4, 4))
    fbo1 = ctx.framebuffer(rbo1)
    fbo2 = ctx.framebuffer(rbo2)
    fbo3 = ctx.framebuffer(rbo3)
    fbo4 = ctx.framebuffer([rbo1, rbo2, rbo3])

    fbo4.clear(0.0, 1.0, 0.0, 1.0)

    pixels1 = fbo1.read(components=4, dtype='f1')
    assert pixels1 == b'\x00\xff\x00\xff' * 16

    pixels2 = fbo2.read(components=4, dtype='f1')
    assert pixels2 == b'\x00\xff\x00\xff' * 16

    pixels3 = fbo3.read(components=4, dtype='f1')
    assert pixels3 == b'\x00\xff\x00\xff' * 16

def test_2(ctx):
    rbo1 = ctx.renderbuffer((4, 4), dtype='f1')
    rbo2 = ctx.renderbuffer((4, 4))
    rbo3 = ctx.renderbuffer((4, 4))
    fbo1 = ctx.framebuffer(rbo1)
    fbo2 = ctx.framebuffer(rbo2)
    fbo3 = ctx.framebuffer(rbo3)
    fbo4 = ctx.framebuffer([rbo1, rbo2, rbo3])

    fbo1.clear(1.0, 0.0, 0.0, 1.0)
    fbo2.clear(0.0, 1.0, 0.0, 1.0)
    fbo3.clear(0.0, 0.0, 1.0, 1.0)

    pixels1 = fbo4.read(attachment=0, components=4, dtype='f1')
    assert pixels1 == b'\xff\x00\x00\xff' * 16

    pixels2 = fbo4.read(attachment=1, components=4, dtype='f1')
    assert pixels2 == b'\x00\xff\x00\xff' * 16

    pixels3 = fbo4.read(attachment=2, components=4, dtype='f1')
    assert pixels3 == b'\x00\x00\xff\xff' * 16

def test_3(ctx):
    rbo1 = ctx.renderbuffer((4, 4), dtype='f1')
    rbo2 = ctx.renderbuffer((4, 4))
    rbo3 = ctx.renderbuffer((4, 4))
    fbo1 = ctx.framebuffer(rbo1)
    fbo2 = ctx.framebuffer(rbo2)
    fbo3 = ctx.framebuffer(rbo3)
    fbo4 = ctx.framebuffer([rbo1, rbo2, rbo3])

    fbo1.clear(1.0, 0.0, 0.0, 1.0)
    fbo2.clear(0.0, 1.0, 0.0, 1.0)
    fbo3.clear(0.0, 0.0, 1.0, 1.0)

    pixels1 = bytearray(64)
    fbo4.read_into(pixels1, attachment=0, components=4, dtype='f1')
    assert bytes(pixels1) == b'\xff\x00\x00\xff' * 16

    pixels2 = bytearray(64)
    fbo4.read_into(pixels2, attachment=1, components=4, dtype='f1')
    assert bytes(pixels2) == b'\x00\xff\x00\xff' * 16

    pixels3 = bytearray(64)
    fbo4.read_into(pixels3, attachment=2, components=4, dtype='f1')
    assert bytes(pixels3) == b'\x00\x00\xff\xff' * 16
