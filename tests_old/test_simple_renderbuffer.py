import unittest

import moderngl

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()
        cls.max_samples = cls.ctx.info['GL_MAX_SAMPLES']

    def test_renderbuffer(self):
        self.ctx.renderbuffer((64, 64))

    def test_multisample_renderbuffer(self):
        if self.max_samples < 2:
            self.skipTest('multisampling is not supported')

        self.ctx.renderbuffer((64, 64), samples=2)

    def test_depth_renderbuffer(self):
        self.ctx.depth_renderbuffer((64, 64))

    def test_multisample_depth_renderbuffer(self):
        if self.max_samples < 2:
            self.skipTest('multisampling is not supported')

        self.ctx.depth_renderbuffer((64, 64), samples=2)

    def test_renderbuffer_invalid_samples(self):
        with self.assertRaisesRegex(moderngl.Error, 'sample'):
            self.ctx.renderbuffer((64, 64), samples=3)


if __name__ == '__main__':
    unittest.main()
