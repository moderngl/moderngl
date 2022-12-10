import unittest

import numpy as np
import numpy.testing as npt

import moderngl

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_1(self):
        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                in vec2 in_vert;

                void main() {
                    gl_Position = vec4(in_vert, 0.0, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330

                out vec4 f_color;

                void main() {
                    f_color = vec4(0.0, 0.5, 1.0, 1.0);
                }
            ''',
        )

        self.vbo = self.ctx.buffer(np.array([-1, -1, -1, 1, 1, -1, 1, 1], dtype='f4'))
        self.vao = self.ctx.simple_vertex_array(self.prog, self.vbo, 'in_vert')
        self.fbo = self.ctx.simple_framebuffer((4, 4), dtype='f2')

        self.fbo.use()
        self.fbo.clear()
        self.vao.render(moderngl.TRIANGLE_STRIP)
        res = np.frombuffer(self.fbo.read(dtype='f2'), dtype='f2')
        expected = np.tile([0.0, 0.5, 1.0], 4 * 4)
        npt.assert_almost_equal(res, expected, decimal=2)


if __name__ == '__main__':
    unittest.main()
