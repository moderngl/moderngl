import moderngl
import pytest


def test_properties(ctx):
    tex = ctx.texture3d((2, 4, 8), 4)
    assert tex.size == (2, 4, 8)
    assert tex.width == 2
    assert tex.height == 4
    assert tex.depth == 8
    assert tex.components == 4
    assert tex.dtype == 'f1'
    assert tex.glo > 0
    assert tex.filter == (moderngl.LINEAR, moderngl.LINEAR)
    assert tex.repeat_x is True
    assert tex.repeat_y is True
    assert tex.repeat_z is True
    assert tex == tex

    tex.repeat_x = False
    tex.repeat_y = False
    tex.repeat_z = False
    assert tex.repeat_x is False
    assert tex.repeat_y is False
    assert tex.repeat_z is False


def test_mipmaps(ctx):
    tex = ctx.texture3d((8, 8, 8), 4)
    tex.build_mipmaps(max_level=2)
    tex.filter = moderngl.LINEAR_MIPMAP_LINEAR, moderngl.LINEAR


def test_texture_3d_create_1(ctx):
    ctx.texture3d((8, 8, 8), 3)


def test_texture_3d_create_2(ctx):
    pixels = b'\x10\x20\x30' * 8 * 8 * 8
    ctx.texture3d((8, 8, 8), 3, pixels)


def test_texture_3d_create_string(ctx):
    pixels = 'abc' * 8 * 8
    with pytest.raises(Exception):
        ctx.texture3d((8, 8, 8), 3, pixels)


def test_texture_3d_create_wrong_size(ctx):
    with pytest.raises(Exception):
        ctx.texture3d((8, 8), 3)


def test_texture_3d_get_swizzle(ctx):
    tex = ctx.texture3d((8, 8, 8), 4)
    assert tex.swizzle == 'RGBA'


def test_texture_3d_swizzle_1(ctx):
    tex = ctx.texture3d((8, 8, 8), 4)
    tex.swizzle = 'argb'
    assert tex.swizzle == 'ARGB'


def test_texture_3d_swizzle_2(ctx):
    tex = ctx.texture3d((8, 8, 8), 1)
    tex.swizzle = 'RRRR'
    assert tex.swizzle == 'RRRR'


def test_texture_3d_swizzle_3(ctx):
    tex = ctx.texture3d((8, 8, 8), 2)
    tex.swizzle = '01RG'
    assert tex.swizzle == '01RG'


def test_texture_3d_read(ctx):
    pixels = b'\x10\x20\x30' * 8 * 8 * 8
    tex = ctx.texture3d((8, 8, 8), 3, pixels)
    assert tex.read() == pixels


def test_texture_3d_read_into(ctx):
    pixels = b'\x10\x20\x30' * 8 * 8 * 8
    tex = ctx.texture3d((8, 8, 8), 3, pixels)
    buf = bytearray(8 * 8 * 8 * 3)
    tex.read_into(buf)
    assert bytes(buf) == pixels


def test_texture_3d_read_into_pbo(ctx):
    if ctx.info['GL_VENDOR'].startswith('Intel'):
        raise pytest.skip('Intel drivers do not support PBO')

    pixels = b'\x10\x20\x30' * 8 * 8 * 8
    tex = ctx.texture3d((8, 8, 8), 3, pixels)
    buf = ctx.buffer(reserve=8 * 8 * 8 * 3)
    tex.read_into(buf)
    assert buf.read() == pixels


def test_texture_3d_write_1(ctx):
    pixels1 = b'\x00\x00\x00' * 8 * 8 * 8
    pixels2 = b'\xff\xff\xff' * 8 * 8 * 8

    tex = ctx.texture3d((8, 8, 8), 3, pixels1)
    assert tex.read() == pixels1

    tex.write(pixels2)
    assert tex.read() == pixels2


def test_texture_3d_write_2(ctx):
    pixels1 = b'\x00\x00\x00' * 8 * 8 * 8
    pixels2 = b'\xff\xff\xff' * 6 * 6 * 6
    pixels3 = b'\x10\x20\x30'

    tex = ctx.texture3d((8, 8, 8), 3)

    tex.write(pixels1, viewport=(0, 0, 0, 8, 8, 8))
    tex.write(pixels2, viewport=(1, 1, 1, 6, 6, 6))
    tex.write(pixels3, viewport=(2, 4, 6, 1, 1, 1))

    def pixel(x, y, z):
        if x == 2 and y == 4 and z == 6:
            return b'\x10\x20\x30'

        if x == 0 or y == 0 or z == 0 or x == 7 or y == 7 or z == 7:
            return b'\x00\x00\x00'

        return b'\xff\xff\xff'

    expectation = b''.join(pixel(x, y, z) for z in range(8) for y in range(8) for x in range(8))
    assert tex.read() == expectation


def test_texture_default_filter(ctx):
    """Ensure default filter is correct"""
    # Float types
    for dtype in ["f1", "f2", "f4"]:
        texture = ctx.texture3d((10, 10, 10), 4, dtype=dtype)
        assert texture.filter == (moderngl.LINEAR, moderngl.LINEAR)

    for dtype in ["u1", "u2", "u4", "i1", "i2", "i4"]:
        texture = ctx.texture3d((10, 10, 10), 4, dtype=dtype)
        assert texture.filter == (moderngl.NEAREST, moderngl.NEAREST)


def test_bind_to_image(ctx):
    if ctx.version_code < 430:
        pytest.skip('compute shaders not supported')

    tex = ctx.texture3d((4, 4, 4), 4)
    tex.bind_to_image(0, read=True, write=False)
    tex.bind_to_image(1, read=False, write=True)
