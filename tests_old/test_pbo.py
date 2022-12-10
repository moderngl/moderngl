import struct
import unittest

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_1(self):
        size = (4, 4)
        rbo1 = self.ctx.renderbuffer(size, dtype='f1')
        rbo2 = self.ctx.renderbuffer(size)
        rbo3 = self.ctx.renderbuffer(size)
        fbo1 = self.ctx.framebuffer(rbo1)
        fbo2 = self.ctx.framebuffer(rbo2)
        fbo3 = self.ctx.framebuffer(rbo3)
        fbo4 = self.ctx.framebuffer([rbo1, rbo2, rbo3])

        fbo1.clear(1.0, 0.0, 0.0, 1.0)
        fbo2.clear(0.0, 1.0, 0.0, 1.0)
        fbo3.clear(0.0, 0.0, 1.0, 1.0)

        buf = self.ctx.buffer(reserve=64)

        fbo4.read_into(buf, attachment=0, components=4, dtype='f1')
        self.assertEqual(buf.read(), b'\xff\x00\x00\xff' * 16)

        fbo4.read_into(buf, attachment=1, components=4, dtype='f1')
        self.assertEqual(buf.read(), b'\x00\xff\x00\xff' * 16)

        fbo4.read_into(buf, attachment=2, components=4, dtype='f1')
        self.assertEqual(buf.read(), b'\x00\x00\xff\xff' * 16)

    def test_2(self):
        pixels = b'\xFF\x00\x00\xFF\x00\xFF\x00\xFF\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF'

        texture = self.ctx.texture((2, 2), 4)
        buf = self.ctx.buffer(pixels)
        texture.write(buf)

        self.assertEqual(texture.read(), pixels)

    def test_3(self):
        pixels = struct.pack(
            '4f4f4f4f',
            1.0, 0.0, 0.0, 1.0,
            0.0, 1.0, 0.0, 1.0,
            0.0, 0.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0,
        )

        buf = self.ctx.buffer(pixels)
        texture = self.ctx.texture((2, 2), 4, dtype='f4')
        texture.write(buf)

        self.assertEqual(texture.read(), pixels)

    def test_4(self):
        pixels = b'\xFF\x00\x00\xFF\x00\xFF\x00\xFF\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF'

        texture = self.ctx.texture((2, 2), 4, pixels)
        buf = self.ctx.buffer(reserve=len(pixels))
        texture.read_into(buf)

        self.assertEqual(buf.read(), pixels)

    def test_5(self):
        pixels = struct.pack(
            '4f4f4f4f',
            1.0, 0.0, 0.0, 1.0,
            0.0, 1.0, 0.0, 1.0,
            0.0, 0.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0,
        )

        texture = self.ctx.texture((2, 2), 4, pixels, dtype='f4')
        buf = self.ctx.buffer(reserve=len(pixels))
        texture.read_into(buf)

        self.assertEqual(buf.read(), pixels)


if __name__ == '__main__':
    unittest.main()
