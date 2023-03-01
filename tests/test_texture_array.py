import moderngl


def test_create(ctx):
    size = (64, 32, 8)
    texture = ctx.texture_array(size, 4)

    # Basic attributes
    assert texture.size == size
    assert texture.width == 64
    assert texture.height == 32
    assert texture.layers == 8
    assert texture.dtype == 'f1'
    assert texture.components == 4
    assert texture.anisotropy == 0.0

    # Texture parameters
    assert texture.repeat_x is True
    assert texture.repeat_y is True
    assert texture.filter == (moderngl.LINEAR, moderngl.LINEAR)
    assert texture.swizzle == "RGBA"
    assert texture == texture

    texture.repeat_x = False
    texture.repeat_y = False
    assert texture.repeat_x is False
    assert texture.repeat_y is False

    assert texture.swizzle == "RGBA"
    texture.swizzle = "RGBA"
    assert texture.swizzle == "RGBA"

    texture.anisotropy = ctx.max_anisotropy
    assert texture.anisotropy == ctx.max_anisotropy

    assert texture.filter == (moderngl.LINEAR, moderngl.LINEAR)
    texture.filter = moderngl.NEAREST, moderngl.NEAREST
    assert texture.filter == (moderngl.NEAREST, moderngl.NEAREST)
    texture.build_mipmaps()
    assert texture.filter == (moderngl.LINEAR_MIPMAP_LINEAR, moderngl.LINEAR)


def test_texture_default_filter(ctx):
    """Ensure default filter is correct"""
    # Float types
    for dtype in ["f1", "f2", "f4"]:
        texture = ctx.texture_array((10, 10, 10), 4, dtype=dtype)
        assert texture.filter == (moderngl.LINEAR, moderngl.LINEAR)

    for dtype in ["u1", "u2", "u4", "i1", "i2", "i4"]:
        texture = ctx.texture_array((10, 10, 10), 4, dtype=dtype)
        assert texture.filter == (moderngl.NEAREST, moderngl.NEAREST)
