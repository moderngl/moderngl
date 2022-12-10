import unittest

import moderngl

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_texture_create_1(self):
        self.ctx.texture((16, 16), 3)

    def test_texture_create_2(self):
        pixels = b'\x10\x20\x30' * 16 * 16
        self.ctx.texture((16, 16), 3, pixels)

    def test_texture_create_string(self):
        pixels = 'abc' * 16 * 16
        with self.assertRaises(Exception):
            self.ctx.texture((16, 16), 3, pixels)

    def test_multisample_texture(self):
        if self.ctx.max_samples < 2:
            self.skipTest('multisampling is not supported')

        self.ctx.texture((16, 16), 3, samples=2)

    def test_depth_texture(self):
        dt = self.ctx.depth_texture((16, 16))
        assert dt.compare_func == '<='
        dt.compare_func = ''
        assert dt.compare_func == '?'

    def test_multisample_depth_texture(self):
        if self.ctx.max_samples < 2:
            self.skipTest('multisampling is not supported')

        self.ctx.depth_texture((16, 16), samples=2)

    def test_texture_invalid_samples(self):
        if self.ctx.max_samples < 2:
            self.skipTest('multisampling is not supported')

        with self.assertRaisesRegex(moderngl.Error, 'sample'):
            self.ctx.texture((16, 16), 3, samples=3)

    def test_texture_get_swizzle(self):
        tex = self.ctx.texture((16, 16), 4)
        self.assertEqual(tex.swizzle, 'RGBA')

    def test_texture_swizzle_1(self):
        tex = self.ctx.texture((16, 16), 4)
        tex.swizzle = 'argb'
        self.assertEqual(tex.swizzle, 'ARGB')

    def test_texture_swizzle_2(self):
        tex = self.ctx.texture((16, 16), 1)
        tex.swizzle = 'RRRR'
        self.assertEqual(tex.swizzle, 'RRRR')

    def test_texture_swizzle_3(self):
        tex = self.ctx.texture((16, 16), 2)
        tex.swizzle = '01RG'
        self.assertEqual(tex.swizzle, '01RG')

    def test_texture_read(self):
        pixels = b'\x10\x20\x30' * 8 * 8
        tex = self.ctx.texture((8, 8), 3, pixels)
        self.assertEqual(tex.read(), pixels)

    def test_texture_read_into(self):
        pixels = b'\x10\x20\x30' * 8 * 8
        tex = self.ctx.texture((8, 8), 3, pixels)
        buf = bytearray(8 * 8 * 3)
        tex.read_into(buf)
        self.assertEqual(bytes(buf), pixels)

    def test_texture_read_into_pbo(self):
        pixels = b'\x10\x20\x30' * 8 * 8
        tex = self.ctx.texture((8, 8), 3, pixels)
        buf = self.ctx.buffer(reserve=8 * 8 * 3)

        tex.read_into(buf)
        self.assertEqual(buf.read(), pixels)

    def test_texture_write_1(self):
        pixels1 = b'\x00\x00\x00' * 8 * 8
        pixels2 = b'\xff\xff\xff' * 8 * 8

        tex = self.ctx.texture((8, 8), 3, pixels1)
        self.assertEqual(tex.read(), pixels1)

        tex.write(pixels2)
        self.assertEqual(tex.read(), pixels2)

    def test_texture_write_2(self):
        pixels1 = b'\x00\x00\x00' * 8 * 8
        pixels2 = b'\xff\xff\xff' * 6 * 6
        pixels3 = b'\x10\x20\x30'

        tex = self.ctx.texture((8, 8), 3)

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
        self.assertEqual(tex.read(), expectation)

    def test_texture_alignment_1(self):
        tex = self.ctx.texture((3, 3), 3)
        self.assertEqual(len(tex.read(alignment=1)), 27)
        self.assertEqual(len(tex.read(alignment=2)), 30)
        self.assertEqual(len(tex.read(alignment=4)), 36)
        self.assertEqual(len(tex.read(alignment=8)), 48)

    def test_texture_alignment_2(self):
        pixels = b'\x80\x90\xA0\x80\x90\xA0\x80\x90\xA0\x80\x90\xA0'
        tex = self.ctx.texture((2, 2), 3, pixels, alignment=1)
        self.assertEqual(tex.read(alignment=1), pixels)

    def test_texture_alignment_3(self):
        pixels = b'\x80\x90\xA0\x80\x90\xA0\x00\x00\x80\x90\xA0\x80\x90\xA0\x00\x00'
        tex = self.ctx.texture((2, 2), 3, pixels, alignment=4)
        result = tex.read(alignment=4)
        self.assertEqual(result[0:6], b'\x80\x90\xA0\x80\x90\xA0')
        self.assertEqual(result[8:14], b'\x80\x90\xA0\x80\x90\xA0')

    def test_texture_default_filter(self):
        """Ensure default filter is correct"""
        # Float types
        for dtype in ["f1", "f2", "f4"]:
            texture = self.ctx.texture((10, 10), 4, dtype=dtype)
            self.assertEqual(texture.filter, (moderngl.LINEAR, moderngl.LINEAR))

        for dtype in ["u1", "u2", "u4", "i1", "i2", "i4"]:
            texture = self.ctx.texture((10, 10), 4, dtype=dtype)
            self.assertEqual(texture.filter, (moderngl.NEAREST, moderngl.NEAREST))


if __name__ == '__main__':
    unittest.main()
