import random
import math
import struct
import unittest

import moderngl
import numpy as np

from common import get_context


class TestScissor(unittest.TestCase):
    """
    Test blending and blend equations with float32 framebuffer.
    Emulate blend and equation values in python and compare with moderngl.
    """
    @classmethod
    def setUpClass(cls):
        """Set up program, vbo and fbo"""
        cls.ctx = get_context()
        cls.prog = cls.ctx.program(
            vertex_shader='''
                #version 330

                in vec2 in_vert;

                void main() {
                    gl_Position = vec4(in_vert, 0.0, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330

                out vec4 fragColor;
                uniform vec4 color;

                void main() {
                    fragColor = color;
                }
            ''',
        )
        quad = [
            -1.0,  1.0,
            -1.0, -1.0,
            1.0, 1.0,
            1.0, -1.0,
        ]
        vbo = cls.ctx.buffer(np.array(quad, dtype='f4').tobytes())
        cls.vao = cls.ctx.simple_vertex_array(cls.prog, vbo, 'in_vert')

    def create_fbo(self, size, components=3):
        return self.ctx.framebuffer(
            color_attachments=[self.ctx.texture(size, components)],
        )

    def test_default_value(self):
        """Ensure default value of the scissor matches the framebuffer size"""
        size = (16, 32)
        fbo = self.create_fbo(size)
        self.assertEqual(fbo.scissor, (0, 0, *size))

    def test_reset_scissor(self):
        """We should be able to reset scissor with `None`"""
        size = (16, 32)
        fbo = self.create_fbo(size)
        new_val = 2, 2, 14, 30

        fbo.scissor = new_val
        self.assertEqual(fbo.scissor, new_val)

        fbo.scissor = None
        self.assertEqual(fbo.scissor, (0, 0, *size))

    def test_render(self):
        """Render different color to the four corners of the fbo using scissor"""
        size = (4, 4)
        fbo = self.create_fbo(size)
        fbo.use()

        # upper left (red)
        fbo.scissor = 0, 2, 2, 2
        self.prog['color'].value = 1.0, 0.0, 0.0, 0.0
        self.vao.render(mode=moderngl.TRIANGLE_STRIP)

        # upper right (green)
        fbo.scissor = 2, 2, 2, 2
        self.prog['color'].value = 0.0, 1.0, 0.0, 0.0
        self.vao.render(mode=moderngl.TRIANGLE_STRIP)

        # lower left (green)
        fbo.scissor = 0, 0, 2, 2
        self.prog['color'].value = 0.0, 0.0, 1.0, 0.0
        self.vao.render(mode=moderngl.TRIANGLE_STRIP)

        # lower right (green)
        fbo.scissor = 2, 0, 2, 2
        self.prog['color'].value = 1.0, 1.0, 1.0, 1.0
        self.vao.render(mode=moderngl.TRIANGLE_STRIP)

        data = fbo.read()
        # Note that the data is flipped horizontally
        self.assertEqual(
            data,
            (b'\x00\x00\xff'b'\x00\x00\xff'b'\xff\xff\xff'b'\xff\xff\xff'
             b'\x00\x00\xff'b'\x00\x00\xff'b'\xff\xff\xff'b'\xff\xff\xff'
             b'\xff\x00\x00'b'\xff\x00\x00'b'\x00\xff\x00'b'\x00\xff\x00'
             b'\xff\x00\x00'b'\xff\x00\x00'b'\x00\xff\x00'b'\x00\xff\x00')
        )

    def test_clear(self):
        """Ensure fbo clearing works with scissoring"""
        size = (4, 4)
        fbo = self.create_fbo(size)
        fbo.use()

        # upper left (red)
        fbo.scissor = 0, 2, 2, 2
        fbo.clear(1.0, 0.0, 0.0, 0.0)

        # upper right (green)
        fbo.scissor = 2, 2, 2, 2
        fbo.clear(0.0, 1.0, 0.0, 0.0)

        # lower left (green)
        fbo.scissor = 0, 0, 2, 2
        fbo.clear(0.0, 0.0, 1.0, 0.0)

        # lower right (green)
        fbo.scissor = 2, 0, 2, 2
        fbo.clear(1.0, 1.0, 1.0, 1.0)

        data = fbo.read()
        # Note that the data is flipped horizontally
        self.assertEqual(
            data,
            (b'\x00\x00\xff'b'\x00\x00\xff'b'\xff\xff\xff'b'\xff\xff\xff'
             b'\x00\x00\xff'b'\x00\x00\xff'b'\xff\xff\xff'b'\xff\xff\xff'
             b'\xff\x00\x00'b'\xff\x00\x00'b'\x00\xff\x00'b'\x00\xff\x00'
             b'\xff\x00\x00'b'\xff\x00\x00'b'\x00\xff\x00'b'\x00\xff\x00')
        )

    def test_scissor_leak(self):
        """Make sure we don't leak scissor values to other fbos"""
        size = 2, 2
        fbo1 = self.create_fbo(size, components=4)
        fbo2 = self.create_fbo(size, components=4)

        fbo1.scissor = 0, 0, 1, 1
        fbo2.scissor = 1, 1, 1, 1

        fbo1.clear(color=(1.0, 0.0, 0.0, 0.0))
        fbo2.clear(color=(0.0, 1.0, 0.0, 0.0))

        self.assertEqual(fbo1.read(), b'\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(fbo2.read(), b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\x00')

    def test_clear_with_viewport(self):
        """Clearing with viewport take precense over scissor"""
        size = 2, 1
        fbo = self.create_fbo(size)
        fbo.scissor = 1, 0, 1, 1
        fbo.clear(color=(1.0, 1.0, 1.0, 1.0), viewport=(0, 0, 1, 1))
        self.assertEqual(fbo.read(), b'\xff\xff\xff\x00\x00\x00')
