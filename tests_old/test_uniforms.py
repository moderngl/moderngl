from array import array
import struct
import unittest

import moderngl
from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()
        cls.vbo = cls.ctx.buffer(reserve=1024)
        cls.res = cls.ctx.buffer(reserve=1024)
        cls.vbo.clear()

        cls.shader = '''
            #version %(version)s
            uniform %(type)s Uniform;
            in %(type)s v_in;
            out %(type)s v_out;
            void main() {
                v_out = Uniform * v_in;
            }
        '''

    def test_float_uniform(self):
        prog = self.ctx.program(
            vertex_shader='''
            #version 330
            uniform float Uniform;
            in float v_in;
            out float v_out;
            void main() {
                v_out = Uniform * v_in;
            }
        ''',
            varyings=['v_out']
        )
        vbo = self.ctx.buffer(struct.pack('f', 1.0))
        vao = self.ctx.simple_vertex_array(prog, vbo, 'v_in')

        prog['Uniform'].value = 7.0
        vao.transform(self.res)

        val = struct.unpack('f', self.res.read(4))[0]
        self.assertAlmostEqual(val, 7.0)

    def test_int_uniform(self):
        prog = self.ctx.program(
            vertex_shader='''
                #version 330
                uniform int Uniform;
                in int v_in;
                out int v_out;
                void main() {
                    v_out = Uniform * v_in;
                }
            ''',
            varyings=['v_out']
        )
        vbo = self.ctx.buffer(struct.pack('i', 1))
        vao = self.ctx.simple_vertex_array(prog, vbo, 'v_in')

        prog['Uniform'].value = -2
        vao.transform(self.res)

        val = struct.unpack('i', self.res.read(4))[0]
        self.assertAlmostEqual(val, -2)

    def test_vec_uniform(self):
        prog = self.ctx.program(
            vertex_shader='''
                #version 330
                uniform vec3 Uniform;
                in vec3 v_in;
                out vec3 v_out;
                void main() {
                    v_out = Uniform * v_in;
                }
            ''',
            varyings=['v_out']
        )

        vbo = self.ctx.buffer(struct.pack('3f', 1.0, 1.0, 1.0))
        vao = self.ctx.simple_vertex_array(prog, vbo, 'v_in')

        prog['Uniform'].value = (0.5, 1.0, 1.5)
        vao.transform(self.res)

        x, y, z = struct.unpack('3f', self.res.read(12))
        self.assertAlmostEqual(x, 0.5)
        self.assertAlmostEqual(y, 1.0)
        self.assertAlmostEqual(z, 1.5)

    def test_mat_uniform(self):
        prog = self.ctx.program(
            vertex_shader='''
                #version 330
                uniform mat2x3 Uniform;
                in float v_in;
                out mat2x3 v_out;
                void main() {
                    v_out = Uniform * v_in;
                }
            ''',
            varyings=['v_out']
        )

        vbo = self.ctx.buffer(struct.pack('f', 1.0))
        vao = self.ctx.simple_vertex_array(prog, vbo, 'v_in')

        prog['Uniform'].value = (0.0, 1.0, 2.0, 3.0, 4.0, 5.0)
        vao.transform(self.res)

        m = struct.unpack('6f', self.res.read(24))
        self.assertAlmostEqual(m[0], 0.0)
        self.assertAlmostEqual(m[1], 1.0)
        self.assertAlmostEqual(m[2], 2.0)
        self.assertAlmostEqual(m[3], 3.0)
        self.assertAlmostEqual(m[4], 4.0)
        self.assertAlmostEqual(m[5], 5.0)

    def test_sampler_2d(self):
        """RGBA8 2d sampler"""
        prog = self.ctx.program(vertex_shader="""
            #version 330
            uniform sampler2D tex;
            out float color;
            void main() {
                color = texelFetch(tex, ivec2(gl_VertexID, 0), 0).r;
            }
            """,
            varyings=['color',],
        )
        tex = self.ctx.texture((4, 1), 1, data=array('B', [127, 0, 255, 64]), dtype='f1')
        buff = self.ctx.buffer(reserve=4 * 4)
        vao = self.ctx.vertex_array(prog, [])
        tex.use(0)
        vao.transform(buff, vertices=4)
        data = struct.unpack('4f', buff.read())
        self.assertAlmostEqual(data[0], 0.498, places=2)
        self.assertAlmostEqual(data[1], 0.0)
        self.assertAlmostEqual(data[2], 1.0)
        self.assertAlmostEqual(data[3], 0.25, places=2)

    def test_sampler_2d_int(self):
        prog = self.ctx.program(vertex_shader="""
            #version 330
            uniform isampler2D tex;
            out int color;
            void main() {
                color = texelFetch(tex, ivec2(gl_VertexID, 0), 0).r;
            }
            """,
            varyings=['color',],
        )
        tex = self.ctx.texture((4, 1), 1, data=array('i', [-1, 0, 1000, 4353454]), dtype='i4')
        buff = self.ctx.buffer(reserve=4 * 4)
        vao = self.ctx.vertex_array(prog, [])
        tex.use(0)
        vao.transform(buff, vertices=4)
        data = struct.unpack('4i', buff.read())
        self.assertEqual(data[0], -1)
        self.assertEqual(data[1], 0)
        self.assertEqual(data[2], 1000)
        self.assertEqual(data[3], 4353454)

    def test_sampler_2d_uint(self):
        prog = self.ctx.program(vertex_shader="""
            #version 330
            uniform usampler2D tex;
            out uint color;
            void main() {
                color = texelFetch(tex, ivec2(gl_VertexID, 0), 0).r;
            }
            """,
            varyings=['color',],
        )
        tex = self.ctx.texture((4, 1), 1, data=array('I', [0, 500, 1000, 4353454]), dtype='u4')
        buff = self.ctx.buffer(reserve=4 * 4)
        vao = self.ctx.vertex_array(prog, [])
        tex.use(0)
        vao.transform(buff, vertices=4)
        data = struct.unpack('4I', buff.read())
        print(data)
        self.assertEqual(data[0], 0)
        self.assertEqual(data[1], 500)
        self.assertEqual(data[2], 1000)
        self.assertEqual(data[3], 4353454)

    def test_sampler_2d_array(self):
        """RGBA8 2d array sampler. Read two pixels from two different layers"""
        prog = self.ctx.program(vertex_shader="""
            #version 330
            uniform sampler2DArray tex;
            out float color;
            void main() {
                color = texelFetch(tex, ivec3(gl_VertexID % 2, 0, gl_VertexID / 2), 0).r;
            }
            """,
            varyings=['color',],
        )
        tex = self.ctx.texture_array((2, 1, 2), 1, data=array('B', [127, 0, 255, 64]), dtype='f1')
        buff = self.ctx.buffer(reserve=4 * 4)
        vao = self.ctx.vertex_array(prog, [])
        tex.use(0)
        vao.transform(buff, vertices=4)
        data = struct.unpack('4f', buff.read())
        self.assertAlmostEqual(data[0], 0.498, places=2)
        self.assertAlmostEqual(data[1], 0.0)
        self.assertAlmostEqual(data[2], 1.0)
        self.assertAlmostEqual(data[3], 0.25, places=2)

    def test_sampler_1d(self):
        prog = self.ctx.program(
            vertex_shader="""
            #version 330
            uniform sampler1D tex;
            out vec4 color;
            void main() {
                color = texelFetch(tex, 0, 0);
            }
            """,
            varyings=["color"],
        )
        self.assertIn("tex", prog)


if __name__ == '__main__':
    unittest.main()
