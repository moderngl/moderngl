import unittest

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_renderbuffer(self):
        rbo = self.ctx.renderbuffer((4, 4))
        self.assertTupleEqual(rbo.size, (4, 4))
        self.assertEqual(rbo.samples, 0)
        self.assertEqual(rbo.depth, False)

    def test_multisample_renderbuffer(self):
        if self.ctx.max_samples < 2:
            self.skipTest('multisampling is not supported')

        rbo = self.ctx.renderbuffer((4, 4), samples=2)
        self.assertTupleEqual(rbo.size, (4, 4))
        self.assertEqual(rbo.samples, 2)
        self.assertEqual(rbo.depth, False)

    def test_depth_renderbuffer(self):
        rbo = self.ctx.depth_renderbuffer((4, 4))
        self.assertTupleEqual(rbo.size, (4, 4))
        self.assertEqual(rbo.samples, 0)
        self.assertEqual(rbo.depth, True)

    def test_multisample_depth_renderbuffer(self):
        if self.ctx.max_samples < 2:
            self.skipTest('multisampling is not supported')

        rbo = self.ctx.depth_renderbuffer((4, 4), samples=2)
        self.assertTupleEqual(rbo.size, (4, 4))
        self.assertEqual(rbo.samples, 2)
        self.assertEqual(rbo.depth, True)

    def test_renderbuffer_invalid_samples(self):
        if self.ctx.max_samples < 2:
            self.skipTest('multisampling is not supported')

        with self.assertRaisesRegex(Exception, 'sample'):
            self.ctx.renderbuffer((4, 4), samples=3)


if __name__ == '__main__':
    unittest.main()
