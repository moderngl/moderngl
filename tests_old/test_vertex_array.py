from array import array
import unittest

import moderngl
import numpy as np

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_padding(self):
        prog = self.ctx.program(
            vertex_shader="""
            #version 330
            in vec2 pos;
            in vec2 velocity;
            out vec2 out_pos;
            void main() {
                out_pos = pos + velocity;
            }
            """,
            varyings=["out_pos"],
        )
        buffer = self.ctx.buffer(array('f', range(16)))
        self.ctx.vertex_array(prog, [(buffer, '2f 2x4', 'pos')])
        self.ctx.vertex_array(prog, [(buffer, '2f 2f', 'pos', 'velocity')])

    def test_empty(self):
        prog = self.ctx.program(
            vertex_shader="""
            #version 330
            in vec2 pos;
            in vec2 velocity;
            out vec2 out_pos;
            void main() {
                out_pos = pos + velocity;
            }
            """,
        )
        self.ctx.vertex_array(prog, [])

    # def test_optional(self):
    #     prog = self.ctx.program(
    #         vertex_shader="""
    #         #version 330
    #         in vec2 pos;
    #         in vec2 velocity;
    #         in vec4 color;
    #         out vec2 out_pos;
    #         void main() {
    #             out_pos = pos + velocity;
    #         }
    #         """,
    #     )
    #     buffer = self.ctx.buffer(array('f', range(16)))
    #     self.ctx.vertex_array(prog, [(buffer, '2f 2f 4f', 'pos', 'velocity', 'color?')])

    def test_1(self):
        prog = self.ctx.program(
            vertex_shader='''
                #version 330
                in vec4 in_vert;
                out vec4 out_vert;
                void main() {
                    out_vert = in_vert;
                }
            ''',
            varyings=['out_vert']
        )

        vbo1 = self.ctx.buffer(np.array([4.0, 2.0, 7.5, 1.8], dtype='f4').tobytes())
        vbo2 = self.ctx.buffer(reserve=vbo1.size)
        vao = self.ctx.simple_vertex_array(prog, vbo1, 'in_vert')
        self.assertEqual(vao.mode, moderngl.POINTS)
        vao.transform(vbo2, moderngl.POINTS)
        res = np.frombuffer(vbo2.read(), dtype='f4')
        np.testing.assert_almost_equal(res, [4.0, 2.0, 7.5, 1.8])

    def test_2(self):
        prog = self.ctx.program(
            vertex_shader='''
                #version 330

                in vec4 in_vert;
                out vec4 out_vert;

                void main() {
                    out_vert = in_vert;
                }
            ''',
            varyings=['out_vert']
        )

        vbo1 = self.ctx.buffer(np.array([4.0, 2.0, 7.5, 1.8], dtype='f4').tobytes())
        vbo2 = self.ctx.buffer(reserve=16)

        vao = self.ctx.vertex_array(prog, [(vbo1, '4f', 'in_vert')])
        vao.transform(vbo2, moderngl.POINTS, vertices=1)
        res = np.frombuffer(vbo2.read(), dtype='f4')
        np.testing.assert_almost_equal(res, [4.0, 2.0, 7.5, 1.8])

        vao = self.ctx.vertex_array(prog, [(vbo1, '3f', 'in_vert')])
        vao.transform(vbo2, moderngl.POINTS, vertices=1)
        res = np.frombuffer(vbo2.read(), dtype='f4')
        np.testing.assert_almost_equal(res, [4.0, 2.0, 7.5, 1.0])

        vao = self.ctx.vertex_array(prog, [(vbo1, '2f', 'in_vert')])
        vao.transform(vbo2, moderngl.POINTS, vertices=1)
        res = np.frombuffer(vbo2.read(), dtype='f4')
        np.testing.assert_almost_equal(res, [4.0, 2.0, 0.0, 1.0])

        vao = self.ctx.vertex_array(prog, [(vbo1, '1f', 'in_vert')])
        vao.transform(vbo2, moderngl.POINTS, vertices=1)
        res = np.frombuffer(vbo2.read(), dtype='f4')
        np.testing.assert_almost_equal(res, [4.0, 0.0, 0.0, 1.0])

    def test_3(self):
        prog = self.ctx.program(
            vertex_shader='''
                #version 330

                in mat4 in_mat;
                out mat4 out_mat;

                void main() {
                    out_mat = in_mat;
                }
            ''',
            varyings=['out_mat']
        )

        vbo1 = self.ctx.buffer(np.arange(1, 17, dtype='f4').tobytes())
        vbo2 = self.ctx.buffer(reserve=64)

        vao = self.ctx.vertex_array(prog, [(vbo1, '16f', 'in_mat')])
        vao.transform(vbo2, moderngl.POINTS, vertices=1)
        res = np.frombuffer(vbo2.read(), dtype='f4')
        np.testing.assert_almost_equal(res, [
            1, 2, 3, 4,
            5, 6, 7, 8,
            9, 10, 11, 12,
            13, 14, 15, 16,
        ])

        vao = self.ctx.vertex_array(prog, [(vbo1, '12f', 'in_mat')])
        vao.transform(vbo2, moderngl.POINTS, vertices=1)
        res = np.frombuffer(vbo2.read(), dtype='f4')
        np.testing.assert_almost_equal(res, [
            1, 2, 3, 1,
            4, 5, 6, 1,
            7, 8, 9, 1,
            10, 11, 12, 1,
        ])

        vao = self.ctx.vertex_array(prog, [(vbo1, '8f', 'in_mat')])
        vao.transform(vbo2, moderngl.POINTS, vertices=1)
        res = np.frombuffer(vbo2.read(), dtype='f4')
        np.testing.assert_almost_equal(res, [
            1, 2, 0, 1,
            3, 4, 0, 1,
            5, 6, 0, 1,
            7, 8, 0, 1,
        ])

        vao = self.ctx.vertex_array(prog, [(vbo1, '4f', 'in_mat')])
        vao.transform(vbo2, moderngl.POINTS, vertices=1)
        res = np.frombuffer(vbo2.read(), dtype='f4')
        np.testing.assert_almost_equal(res, [
            1, 0, 0, 1,
            2, 0, 0, 1,
            3, 0, 0, 1,
            4, 0, 0, 1,
        ])

    def test_4(self):
        prog = self.ctx.program(
            vertex_shader='''
                #version 330

                in mat2 in_mat;
                out mat2 out_mat;

                void main() {
                    out_mat = in_mat;
                }
            ''',
            varyings=['out_mat']
        )

        vbo1 = self.ctx.buffer(np.array([4.0, 2.0, 7.5, 1.8], dtype='f4').tobytes())
        vbo2 = self.ctx.buffer(reserve=16)

        vao = self.ctx.vertex_array(prog, [(vbo1, '4f', 'in_mat')])
        vao.transform(vbo2, moderngl.POINTS, vertices=1)
        res = np.frombuffer(vbo2.read(), dtype='f4')
        np.testing.assert_almost_equal(res, [4.0, 2.0, 7.5, 1.8])

        vao = self.ctx.vertex_array(prog, [(vbo1, '2f', 'in_mat')])
        vao.transform(vbo2, moderngl.POINTS, vertices=1)
        res = np.frombuffer(vbo2.read(), dtype='f4')
        np.testing.assert_almost_equal(res, [4.0, 0.0, 2.0, 0.0])


if __name__ == '__main__':
    unittest.main()
