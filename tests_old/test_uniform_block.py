import struct
import unittest

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()
        if 'llvmpipe' in cls.ctx.info['GL_RENDERER']:
            raise unittest.SkipTest('Temp disable for llvmpipe')

        cls.prog = cls.ctx.program(
            vertex_shader='''
                #version 330

                in vec2 in_v;
                out vec2 out_v;

                uniform Block1 {
                    float x;
                };

                uniform Block2 {
                    float y;
                };

                uniform Block3 {
                    float z;
                };

                void main() {
                    out_v = in_v * z + vec2(x, y);
                }
            ''',
            varyings=['out_v']
        )

    def test_1(self):
        buf_v = self.ctx.buffer(struct.pack('2f', 100.0, 1000.0))
        buf_u1 = self.ctx.buffer(struct.pack('f', 9.5))
        buf_u2 = self.ctx.buffer(struct.pack('f', 4.0))
        buf_u3 = self.ctx.buffer(struct.pack('f', 3.0))
        buf_u4 = self.ctx.buffer(struct.pack('f', 0.0))
        buf_r = self.ctx.buffer(reserve=buf_v.size)

        vao = self.ctx.vertex_array(self.prog, [
            (buf_v, '2f', 'in_v'),
        ])

        self.assertIn('Block1', self.prog)
        self.assertIn('Block2', self.prog)
        self.assertIn('Block3', self.prog)

        self.assertNotIn('x', self.prog)
        self.assertNotIn('y', self.prog)
        self.assertNotIn('z', self.prog)

        self.prog['Block1'].binding = 2
        self.prog['Block2'].binding = 4
        self.prog['Block3'].binding = 1

        buf_u1.bind_to_uniform_block(2)
        buf_u2.bind_to_uniform_block(4)
        buf_u4.bind_to_uniform_block(1)

        vao.transform(buf_r)
        a, b = struct.unpack('2f', buf_r.read())
        self.assertAlmostEqual(a, 9.5)
        self.assertAlmostEqual(b, 4.0)

        buf_u3.bind_to_uniform_block(1)

        vao.transform(buf_r)
        a, b = struct.unpack('2f', buf_r.read())
        self.assertAlmostEqual(a, 309.5)
        self.assertAlmostEqual(b, 3004.0)

    def test_2(self):
        min_offset = self.ctx.info['GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT']
        u3_len = (min_offset // 4) + 1
        u3_data = list(range(u3_len))

        buf_v = self.ctx.buffer(struct.pack('2f', 100.0, 1000.0))
        buf_u1 = self.ctx.buffer(struct.pack('f', 9.5))
        buf_u2 = self.ctx.buffer(struct.pack('f', 4.0))
        buf_u3 = self.ctx.buffer(struct.pack('{:d}f'.format(u3_len), *u3_data))
        buf_r = self.ctx.buffer(reserve=buf_v.size)

        vao = self.ctx.vertex_array(self.prog, [
            (buf_v, '2f', 'in_v'),
        ])

        self.prog['Block1'].binding = 2
        self.prog['Block2'].binding = 4
        self.prog['Block3'].binding = 1

        buf_u1.bind_to_uniform_block(2)
        buf_u2.bind_to_uniform_block(4)
        buf_u3.bind_to_uniform_block(1, offset=min_offset)

        vao.transform(buf_r)
        a, b = struct.unpack('2f', buf_r.read())
        self.assertAlmostEqual(a, u3_data[-1] * 100 + 9.5)
        self.assertAlmostEqual(b, u3_data[-1] * 1000 + 4.0)

        buf_u3.bind_to_uniform_block(1, offset=0)

        vao.transform(buf_r)
        a, b = struct.unpack('2f', buf_r.read())
        self.assertAlmostEqual(a, 9.5)
        self.assertAlmostEqual(b, 4.0)


if __name__ == '__main__':
    unittest.main()
