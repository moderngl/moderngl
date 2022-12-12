import struct


def test_1(ctx):
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

    buf = ctx.buffer(reserve=64)

    fbo4.read_into(buf, attachment=0, components=4, dtype='f1')
    assert buf.read() == b'\xff\x00\x00\xff' * 16

    fbo4.read_into(buf, attachment=1, components=4, dtype='f1')
    assert buf.read() == b'\x00\xff\x00\xff' * 16

    fbo4.read_into(buf, attachment=2, components=4, dtype='f1')
    assert buf.read() == b'\x00\x00\xff\xff' * 16

def test_2(ctx):
    pixels = b'\xFF\x00\x00\xFF\x00\xFF\x00\xFF\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF'

    texture = ctx.texture((2, 2), 4)
    buf = ctx.buffer(pixels)
    texture.write(buf)

    assert texture.read() == pixels

def test_3(ctx):
    pixels = struct.pack(
        '4f4f4f4f',
        1.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0,
        0.0, 0.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
    )

    buf = ctx.buffer(pixels)
    texture = ctx.texture((2, 2), 4, dtype='f4')
    texture.write(buf)

    assert texture.read() == pixels

def test_4(ctx):
    pixels = b'\xFF\x00\x00\xFF\x00\xFF\x00\xFF\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF'

    texture = ctx.texture((2, 2), 4, pixels)
    buf = ctx.buffer(reserve=len(pixels))
    texture.read_into(buf)

    assert buf.read() == pixels

def test_5(ctx):
    pixels = struct.pack(
        '4f4f4f4f',
        1.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0,
        0.0, 0.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
    )

    texture = ctx.texture((2, 2), 4, pixels, dtype='f4')
    buf = ctx.buffer(reserve=len(pixels))
    texture.read_into(buf)

    assert buf.read() == pixels
