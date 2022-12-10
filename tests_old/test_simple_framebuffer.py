import unittest

import moderngl

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()
        cls.max_samples = cls.ctx.info['GL_MAX_SAMPLES']

    def test_framebuffer_color_attachment(self):
        rbo = self.ctx.renderbuffer((16, 16))
        self.ctx.framebuffer(rbo)

    def test_framebuffer_get_color_attachments(self):
        rbo1 = self.ctx.renderbuffer((16, 16))
        rbo2 = self.ctx.renderbuffer((16, 16))
        rbo3 = self.ctx.renderbuffer((16, 16))

        fbo1 = self.ctx.framebuffer(rbo1)
        fbo2 = self.ctx.framebuffer([rbo2, rbo1])
        fbo3 = self.ctx.framebuffer([rbo1, rbo2, rbo3])

        self.assertEqual(len(fbo1.color_attachments), 1)
        self.assertEqual(len(fbo2.color_attachments), 2)
        self.assertEqual(len(fbo3.color_attachments), 3)

        self.assertIsInstance(fbo1.color_attachments[0], moderngl.Renderbuffer)
        self.assertIsInstance(fbo2.color_attachments[0], moderngl.Renderbuffer)
        self.assertIsInstance(fbo2.color_attachments[1], moderngl.Renderbuffer)
        self.assertIsInstance(fbo3.color_attachments[0], moderngl.Renderbuffer)
        self.assertIsInstance(fbo3.color_attachments[1], moderngl.Renderbuffer)
        self.assertIsInstance(fbo3.color_attachments[2], moderngl.Renderbuffer)

        self.assertIn(rbo1, fbo1.color_attachments)
        self.assertIn(rbo2, fbo2.color_attachments)
        self.assertIn(rbo2, fbo3.color_attachments)

        self.assertNotIn(rbo2, fbo1.color_attachments)
        self.assertNotIn(rbo3, fbo1.color_attachments)
        self.assertNotIn(rbo3, fbo2.color_attachments)

    def test_framebuffer_get_depth_attachment(self):
        rbo1 = self.ctx.renderbuffer((16, 16))
        rbo2 = self.ctx.depth_renderbuffer((16, 16))

        fbo1 = self.ctx.framebuffer(rbo1)
        fbo2 = self.ctx.framebuffer(rbo1, rbo2)

        self.assertIsNone(fbo1.depth_attachment)
        self.assertIsInstance(fbo2.depth_attachment, moderngl.Renderbuffer)

        self.assertNotEqual(fbo1.depth_attachment, rbo2)
        self.assertEqual(fbo2.depth_attachment, rbo2)

    def test_framebuffer_color_mask(self):
        fbo = self.ctx.framebuffer(self.ctx.renderbuffer((16, 16)))
        self.assertEqual(fbo.color_mask, (True, True, True, True))

    def test_framebuffer_single_channel_color_mask(self):
        fbo = self.ctx.framebuffer(self.ctx.renderbuffer((16, 16), components=1))
        self.assertEqual(fbo.color_mask, (True, False, False, False))

    def test_framebuffer_mixed_channels_color_mask(self):
        fbo = self.ctx.framebuffer([
            self.ctx.renderbuffer((16, 16), components=1),
            self.ctx.renderbuffer((16, 16), components=2),
            self.ctx.renderbuffer((16, 16), components=3),
            self.ctx.renderbuffer((16, 16), components=4),
        ])
        expected = (
            (True, False, False, False),
            (True, True, False, False),
            (True, True, True, False),
            (True, True, True, True),
        )
        self.assertEqual(fbo.color_mask, expected)

    def test_framebuffer_depth_mask_1(self):
        fbo = self.ctx.framebuffer(
            self.ctx.renderbuffer((16, 16)),
            self.ctx.depth_renderbuffer((16, 16)),
        )
        self.assertEqual(fbo.depth_mask, True)

    def test_framebuffer_depth_mask_2(self):
        fbo = self.ctx.framebuffer(
            self.ctx.renderbuffer((16, 16))
        )
        self.assertEqual(fbo.depth_mask, False)

    def test_framebuffer_color_attachments(self):
        rbo1 = self.ctx.renderbuffer((16, 16))
        rbo2 = self.ctx.renderbuffer((16, 16))
        rbo3 = self.ctx.renderbuffer((16, 16))
        self.ctx.framebuffer([rbo1, rbo2, rbo3])

    def test_framebuffer_multiple_color_masks(self):
        fbo = self.ctx.framebuffer([
            self.ctx.renderbuffer((16, 16)),
            self.ctx.renderbuffer((16, 16)),
        ])
        expected = ((True, True, True, True), (True, True, True, True))
        self.assertEqual(fbo.color_mask, expected)

    def test_framebuffer_size_mismatch(self):
        with self.assertRaisesRegex(moderngl.Error, 'size'):
            rbo1 = self.ctx.renderbuffer((16, 16))
            rbo2 = self.ctx.depth_renderbuffer((32, 32))
            self.ctx.framebuffer(rbo1, rbo2)

    def test_framebuffer_color_attachments_size_mismatch(self):
        with self.assertRaisesRegex(moderngl.Error, 'size'):
            rbo1 = self.ctx.renderbuffer((16, 16))
            rbo2 = self.ctx.renderbuffer((32, 32))
            self.ctx.framebuffer([rbo1, rbo2])

    def test_depth_framebuffer(self):
        rbo1 = self.ctx.renderbuffer((16, 16))
        rbo2 = self.ctx.depth_renderbuffer((16, 16))
        self.ctx.framebuffer(rbo1, rbo2)

    def test_framebuffer_multisample(self):
        if self.max_samples < 2:
            self.skipTest('multisampling is not supported')

        rbo1 = self.ctx.renderbuffer((16, 16), samples=2)
        self.ctx.framebuffer(rbo1)

    def test_depth_framebuffer_multisample(self):
        if self.max_samples < 2:
            self.skipTest('multisampling is not supported')

        rbo1 = self.ctx.renderbuffer((16, 16), samples=2)
        rbo2 = self.ctx.depth_renderbuffer((16, 16), samples=2)
        self.ctx.framebuffer(rbo1, rbo2)

    def test_framebuffer_multisample_sample_mismatch(self):
        if self.max_samples < 2:
            self.skipTest('multisampling is not supported')

        with self.assertRaisesRegex(moderngl.Error, 'sample'):
            rbo1 = self.ctx.renderbuffer((16, 16))
            rbo2 = self.ctx.depth_renderbuffer((16, 16), samples=2)
            self.ctx.framebuffer(rbo1, rbo2)

    def test_empty_framebuffer(self):
        with self.assertRaisesRegex(moderngl.Error, 'empty'):
            self.ctx.framebuffer([])

    def test_framebuffer_having_depth_in_colors(self):
        with self.assertRaisesRegex(moderngl.Error, r'(color|depth)'):
            self.ctx.framebuffer(self.ctx.depth_renderbuffer((16, 16)))

    def test_framebuffer_having_color_in_depth(self):
        with self.assertRaisesRegex(moderngl.Error, r'(color|depth)'):
            self.ctx.framebuffer(
                self.ctx.renderbuffer((16, 16)),
                self.ctx.renderbuffer((16, 16)),
            )


if __name__ == '__main__':
    unittest.main()
