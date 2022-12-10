import unittest

import moderngl
from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()
        cls.vendor = cls.ctx.info['GL_VENDOR']

    def test_texture_3d_create_1(self):
        self.ctx.texture3d((8, 8, 8), 3)

    def test_texture_3d_create_2(self):
        pixels = b'\x10\x20\x30' * 8 * 8 * 8
        self.ctx.texture3d((8, 8, 8), 3, pixels)

    def test_texture_3d_create_string(self):
        pixels = 'abc' * 8 * 8
        with self.assertRaises(Exception):
            self.ctx.texture3d((8, 8, 8), 3, pixels)

    def test_texture_3d_create_wrong_size(self):
        with self.assertRaises(Exception):
            self.ctx.texture3d((8, 8), 3)

    def test_texture_3d_get_swizzle(self):
        tex = self.ctx.texture3d((8, 8, 8), 4)
        self.assertEqual(tex.swizzle, 'RGBA')

    def test_texture_3d_swizzle_1(self):
        tex = self.ctx.texture3d((8, 8, 8), 4)
        tex.swizzle = 'argb'
        self.assertEqual(tex.swizzle, 'ARGB')

    def test_texture_3d_swizzle_2(self):
        tex = self.ctx.texture3d((8, 8, 8), 1)
        tex.swizzle = 'RRRR'
        self.assertEqual(tex.swizzle, 'RRRR')

    def test_texture_3d_swizzle_3(self):
        tex = self.ctx.texture3d((8, 8, 8), 2)
        tex.swizzle = '01RG'
        self.assertEqual(tex.swizzle, '01RG')

    def test_texture_3d_read(self):
        pixels = b'\x10\x20\x30' * 8 * 8 * 8
        tex = self.ctx.texture3d((8, 8, 8), 3, pixels)
        self.assertEqual(tex.read(), pixels)

    def test_texture_3d_read_into(self):
        pixels = b'\x10\x20\x30' * 8 * 8 * 8
        tex = self.ctx.texture3d((8, 8, 8), 3, pixels)
        buf = bytearray(8 * 8 * 8 * 3)
        tex.read_into(buf)
        self.assertEqual(bytes(buf), pixels)

    def test_texture_3d_read_into_pbo(self):
        if self.vendor.startswith('Intel'):
            raise unittest.SkipTest('')

        pixels = b'\x10\x20\x30' * 8 * 8 * 8
        tex = self.ctx.texture3d((8, 8, 8), 3, pixels)
        buf = self.ctx.buffer(reserve=8 * 8 * 8 * 3)
        tex.read_into(buf)
        self.assertEqual(buf.read(), pixels)

    def test_texture_3d_write_1(self):
        pixels1 = b'\x00\x00\x00' * 8 * 8 * 8
        pixels2 = b'\xff\xff\xff' * 8 * 8 * 8

        tex = self.ctx.texture3d((8, 8, 8), 3, pixels1)
        self.assertEqual(tex.read(), pixels1)

        tex.write(pixels2)
        self.assertEqual(tex.read(), pixels2)

    def test_texture_3d_write_2(self):
        pixels1 = b'\x00\x00\x00' * 8 * 8 * 8
        pixels2 = b'\xff\xff\xff' * 6 * 6 * 6
        pixels3 = b'\x10\x20\x30'

        tex = self.ctx.texture3d((8, 8, 8), 3)

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
        self.assertEqual(tex.read(), expectation)

    def test_texture_default_filter(self):
        """Ensure default filter is correct"""
        # Float types
        for dtype in ["f1", "f2", "f4"]:
            texture = self.ctx.texture3d((10, 10, 10), 4, dtype=dtype)
            self.assertEqual(texture.filter, (moderngl.LINEAR, moderngl.LINEAR))

        for dtype in ["u1", "u2", "u4", "i1", "i2", "i4"]:
            texture = self.ctx.texture3d((10, 10, 10), 4, dtype=dtype)
            self.assertEqual(texture.filter, (moderngl.NEAREST, moderngl.NEAREST))

    def test_bind_to_image(self):
        tex = self.ctx.texture3d((4, 4, 4), 4)
        tex.bind_to_image(0, read=True, write=False)
        tex.bind_to_image(1, read=False, write=True)


if __name__ == '__main__':
    unittest.main()
