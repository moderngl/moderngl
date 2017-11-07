import unittest

from common import get_context


class TestBuffer(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def tearDown(self):
        self.assertEqual(self.ctx.error, 'GL_NO_ERROR')

    def test_viewport(self):
        self.skipTest('1x1 pixel')
        self.ctx.viewport = (1, 2, 3, 4)
        self.assertTupleEqual(self.ctx.viewport, (1, 2, 3, 4))
        self.assertTupleEqual(self.ctx.screen.viewport, (1, 2, 3, 4))
        fbo = self.ctx.simple_framebuffer((10, 10))
        fbo.use()
        self.assertTupleEqual(self.ctx.viewport, (0, 0, 10, 10))
        self.assertTupleEqual(fbo.viewport, (0, 0, 10, 10))
        self.ctx.viewport = (2, 3, 7, 8)
        self.assertTupleEqual(fbo.viewport, (2, 3, 7, 8))
        self.assertTupleEqual(self.ctx.viewport, (2, 3, 7, 8))
        self.assertTupleEqual(self.ctx.screen.viewport, (1, 2, 3, 4))
        self.ctx.screen.viewport = (3, 3, 3, 3)
        self.assertTupleEqual(fbo.viewport, (2, 3, 7, 8))
        self.assertTupleEqual(self.ctx.viewport, (2, 3, 7, 8))
        self.assertTupleEqual(self.ctx.screen.viewport, (3, 3, 3, 3))
        self.ctx.screen.use()
        self.assertTupleEqual(self.ctx.viewport, (3, 3, 3, 3))
        fbo.viewport = (0, 0, 7, 7)
        self.assertTupleEqual(self.ctx.viewport, (3, 3, 3, 3))
        self.assertTupleEqual(self.ctx.screen.viewport, (3, 3, 3, 3))
        self.ctx.viewport = (3, 5, 7, 9)
        self.assertTupleEqual(fbo.viewport, (0, 0, 7, 7))
        self.assertTupleEqual(self.ctx.viewport, (3, 5, 7, 9))
        self.assertTupleEqual(self.ctx.screen.viewport, (3, 5, 7, 9))
        fbo.use()
        self.assertTupleEqual(self.ctx.viewport, (0, 0, 7, 7))
        self.ctx.screen.use()


if __name__ == '__main__':
    unittest.main()
