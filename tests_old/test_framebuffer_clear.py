import unittest

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_1(self):
        fbo1 = self.ctx.simple_framebuffer((4, 4))
        fbo2 = self.ctx.simple_framebuffer((4, 4))

        fbo1.clear(0.0, 0.0, 0.0, 0.0)

        self.assertEqual(fbo1.read(), b'\x00\x00\x00' * 4 * 4)

        fbo1.clear(1.0, 0.0, 0.0, 0.0, viewport=(2, 2))
        fbo1.clear(0.0, 1.0, 0.0, 0.0, viewport=(2, 0, 2, 2))
        fbo1.clear(0.0, 0.0, 1.0, 0.0, viewport=(0, 2, 2, 2))

        self.assertEqual(fbo1.read((2, 2)), b'\xff\x00\x00' * 2 * 2)
        self.assertEqual(fbo1.read((2, 0, 2, 2)), b'\x00\xff\x00' * 2 * 2)
        self.assertEqual(fbo1.read((0, 2, 2, 2)), b'\x00\x00\xff' * 2 * 2)
        self.assertEqual(fbo1.read((2, 2, 2, 2)), b'\x00\x00\x00' * 2 * 2)

        self.ctx.copy_framebuffer(fbo2, fbo1)

        self.assertEqual(fbo2.read((2, 2)), b'\xff\x00\x00' * 2 * 2)
        self.assertEqual(fbo2.read((2, 0, 2, 2)), b'\x00\xff\x00' * 2 * 2)
        self.assertEqual(fbo2.read((0, 2, 2, 2)), b'\x00\x00\xff' * 2 * 2)
        self.assertEqual(fbo2.read((2, 2, 2, 2)), b'\x00\x00\x00' * 2 * 2)


if __name__ == '__main__':
    unittest.main()
