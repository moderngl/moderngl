import unittest

import moderngl

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_create(self):
        size = (64, 32, 8)
        texture = self.ctx.texture_array(size, 4)

        # Basic attributes
        assert texture.size == size
        assert texture.width == 64
        assert texture.height == 32
        assert texture.layers == 8
        assert texture.dtype == 'f1'
        assert texture.components == 4

        # Texture paramters
        assert texture.repeat_x == True
        assert texture.repeat_y == True
        assert texture.filter == (moderngl.LINEAR, moderngl.LINEAR)
        assert texture.swizzle == "RGBA"
        print(texture.anisotropy)
        assert texture.anisotropy == 1.0

        texture.build_mipmaps()
        assert texture.filter == (moderngl.LINEAR_MIPMAP_LINEAR, moderngl.LINEAR)

    def test_texture_default_filter(self):
        """Ensure default filter is correct"""
        # Float types
        for dtype in ["f1", "f2", "f4"]:
            texture = self.ctx.texture_array((10, 10, 10), 4, dtype=dtype)
            self.assertEqual(texture.filter, (moderngl.LINEAR, moderngl.LINEAR))

        for dtype in ["u1", "u2", "u4", "i1", "i2", "i4"]:
            texture = self.ctx.texture_array((10, 10, 10), 4, dtype=dtype)
            self.assertEqual(texture.filter, (moderngl.NEAREST, moderngl.NEAREST))
