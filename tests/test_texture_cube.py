import moderngl
import pytest


def test_0(ctx):
    texture = ctx.texture_cube((16, 16), 4)
    assert texture.size == (16, 16)
    assert texture.components == 4
    assert texture.filter == (moderngl.LINEAR, moderngl.LINEAR)
    assert texture.swizzle == "RGBA"
    assert texture.glo > 0
    assert texture.dtype == "f1"
    assert texture.filter == (moderngl.LINEAR, moderngl.LINEAR)
    assert texture.swizzle == "RGBA"
    assert texture == texture
    assert texture.anisotropy == 0.0

    texture.anisotropy = ctx.max_anisotropy
    assert texture.anisotropy == ctx.max_anisotropy

    texture.filter = moderngl.NEAREST, moderngl.NEAREST
    assert texture.filter == (moderngl.NEAREST, moderngl.NEAREST)
    texture.swizzle = "BGRA"
    assert texture.swizzle == "BGRA"
    texture.anisotropy = ctx.max_anisotropy
    assert texture.anisotropy == ctx.max_anisotropy


def test_1(ctx):
    faces = [
        b'\x00\x00\xff' * 4 * 4,
        b'\x00\xff\x00' * 4 * 4,
        b'\x00\xff\xff' * 4 * 4,
        b'\xff\x00\x00' * 4 * 4,
        b'\xff\x00\xff' * 4 * 4,
        b'\x00\xff\x00' * 4 * 4,
    ]
    tex = ctx.texture_cube((4, 4), 3, b''.join(faces))
    assert tex.read(0) == faces[0]
    assert tex.read(1) == faces[1]
    assert tex.read(2) == faces[2]
    assert tex.read(3) == faces[3]
    assert tex.read(4) == faces[4]
    assert tex.read(5) == faces[5]
    tex.write(0, b'\xff\xff\xff' * 4 * 4)
    assert tex.read(0) == b'\xff\xff\xff' * 4 * 4
    tex.write(2, b'\xff\xff\xff' * 4 * 4)
    assert tex.read(2) == b'\xff\xff\xff' * 4 * 4
    tex.write(5, b'\xff\xff\xff' * 4 * 4)
    assert tex.read(5) == b'\xff\xff\xff' * 4 * 4


def test_2(ctx):
    tex = ctx.texture_cube((4, 4), 3)

    with pytest.raises(Exception):
        tex.write(0, b'\xff\xff\xff' * 4 * 3)

    with pytest.raises(Exception):
        tex.write(0, b'\xff\xff\xff' * 4 * 5)

    with pytest.raises(Exception):
        tex.write(-1, b'\xff\xff\xff' * 4 * 4)

    with pytest.raises(Exception):
        tex.write(6, b'\xff\xff\xff' * 4 * 4)


def test_3(ctx):
    tex = ctx.texture_cube((4, 4), 3)

    with pytest.raises(Exception):
        tex.read(-1)

    with pytest.raises(Exception):
        tex.read(6)


def test_texture_default_filter(ctx):
    """Ensure default filter is correct"""
    # Float types
    for dtype in ["f1", "f2", "f4"]:
        texture = ctx.texture_cube((10, 10), 4, dtype=dtype)
        assert texture.filter == (moderngl.LINEAR, moderngl.LINEAR)

    for dtype in ["u1", "u2", "u4", "i1", "i2", "i4"]:
        texture = ctx.texture_cube((10, 10), 4, dtype=dtype)
        assert texture.filter == (moderngl.NEAREST, moderngl.NEAREST)
