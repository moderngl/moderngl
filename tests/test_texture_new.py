import moderngl
import pytest

def test_texture_create_1(ctx):
    ctx.texture((16, 16), 3)

def test_texture_create_2(ctx):
    pixels = b'\x10\x20\x30' * 16 * 16
    ctx.texture((16, 16), 3, pixels)

def test_texture_create_string(ctx):
    pixels = 'abc' * 16 * 16
    with pytest.raises(Exception):
        ctx.texture((16, 16), 3, pixels)

def test_multisample_texture(ctx):
    if ctx.max_samples < 2:
        pytest.skip('multisampling is not supported')

    ctx.texture((16, 16), 3, samples=2)

def test_depth_texture(ctx):
    dt = ctx.depth_texture((16, 16))
    assert dt.compare_func == '<='
    dt.compare_func = ''
    assert dt.compare_func == '?'

def test_multisample_depth_texture(ctx):
    if ctx.max_samples < 2:
        pytest.skip('multisampling is not supported')

    ctx.depth_texture((16, 16), samples=2)

def test_texture_invalid_samples(ctx):
    if ctx.max_samples < 2:
        pytest.skip('multisampling is not supported')

    with pytest.raises(moderngl.Error, match='sample'):
        ctx.texture((16, 16), 3, samples=3)

def test_texture_get_swizzle(ctx):
    tex = ctx.texture((16, 16), 4)
    assert tex.swizzle == 'RGBA'

def test_texture_swizzle_1(ctx):
    tex = ctx.texture((16, 16), 4)
    tex.swizzle = 'argb'
    assert tex.swizzle == 'ARGB'

def test_texture_swizzle_2(ctx):
    tex = ctx.texture((16, 16), 1)
    tex.swizzle = 'RRRR'
    assert tex.swizzle == 'RRRR'

def test_texture_swizzle_3(ctx):
    tex = ctx.texture((16, 16), 2)
    tex.swizzle = '01RG'
    assert tex.swizzle, '01RG'

def test_texture_read(ctx):
    pixels = b'\x10\x20\x30' * 8 * 8
    tex = ctx.texture((8, 8), 3, pixels)
    assert tex.read(), pixels

def test_texture_read_into(ctx):
    pixels = b'\x10\x20\x30' * 8 * 8
    tex = ctx.texture((8, 8), 3, pixels)
    buf = bytearray(8 * 8 * 3)
    tex.read_into(buf)
    assert bytes(buf) == pixels

def test_texture_read_into_pbo(ctx):
    pixels = b'\x10\x20\x30' * 8 * 8
    tex = ctx.texture((8, 8), 3, pixels)
    buf = ctx.buffer(reserve=8 * 8 * 3)

    tex.read_into(buf)
    assert buf.read() == pixels

def test_texture_write_1(ctx):
    pixels1 = b'\x00\x00\x00' * 8 * 8
    pixels2 = b'\xff\xff\xff' * 8 * 8

    tex = ctx.texture((8, 8), 3, pixels1)
    assert tex.read() == pixels1

    tex.write(pixels2)
    assert tex.read() == pixels2

def test_texture_write_2(ctx):
    pixels1 = b'\x00\x00\x00' * 8 * 8
    pixels2 = b'\xff\xff\xff' * 6 * 6
    pixels3 = b'\x10\x20\x30'

    tex = ctx.texture((8, 8), 3)

    tex.write(pixels1, viewport=(0, 0, 8, 8))
    tex.write(pixels2, viewport=(1, 1, 6, 6))
    tex.write(pixels3, viewport=(2, 4, 1, 1))

    def pixel(x, y):
        if x == 2 and y == 4:
            return b'\x10\x20\x30'

        if x == 0 or y == 0 or x == 7 or y == 7:
            return b'\x00\x00\x00'

        return b'\xff\xff\xff'

    expectation = b''.join(pixel(x, y) for y in range(8) for x in range(8))
    assert tex.read() == expectation

def test_texture_alignment_1(ctx):
    tex = ctx.texture((3, 3), 3)
    assert len(tex.read(alignment=1)) == 27
    assert len(tex.read(alignment=2)) == 30
    assert len(tex.read(alignment=4)) == 36
    assert len(tex.read(alignment=8)) == 48

def test_texture_alignment_2(ctx):
    pixels = b'\x80\x90\xA0\x80\x90\xA0\x80\x90\xA0\x80\x90\xA0'
    tex = ctx.texture((2, 2), 3, pixels, alignment=1)
    assert tex.read(alignment=1) == pixels

def test_texture_alignment_3(ctx):
    pixels = b'\x80\x90\xA0\x80\x90\xA0\x00\x00\x80\x90\xA0\x80\x90\xA0\x00\x00'
    tex = ctx.texture((2, 2), 3, pixels, alignment=4)
    result = tex.read(alignment=4)
    assert result[0:6] == b'\x80\x90\xA0\x80\x90\xA0'
    assert result[8:14] == b'\x80\x90\xA0\x80\x90\xA0'

def test_texture_default_filter(ctx):
    """Ensure default filter is correct"""
    # Float types
    for dtype in ["f1", "f2", "f4"]:
        texture = ctx.texture((10, 10), 4, dtype=dtype)
        assert texture.filter == (moderngl.LINEAR, moderngl.LINEAR)

    for dtype in ["u1", "u2", "u4", "i1", "i2", "i4"]:
        texture = ctx.texture((10, 10), 4, dtype=dtype)
        assert texture.filter == (moderngl.NEAREST, moderngl.NEAREST)
