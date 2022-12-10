import unittest

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_framebuffer_read_channels(self):
        fbo = self.ctx.framebuffer([
            self.ctx.renderbuffer((4, 4)),
        ])

        fbo.clear(65 / 255, 66 / 255, 67 / 255, 68 / 255)

        self.assertEqual(fbo.read(components=1), b'A' * 16)
        self.assertEqual(fbo.read(components=2), b'AB' * 16)
        self.assertEqual(fbo.read(components=3), b'ABC' * 16)
        self.assertEqual(fbo.read(components=4), b'ABCD' * 16)


if __name__ == '__main__':
    unittest.main()
