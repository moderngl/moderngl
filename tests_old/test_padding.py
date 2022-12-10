import struct
import unittest

import moderngl

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

        cls.prog = cls.ctx.program(
            vertex_shader='''
                #version 330

                in int a_in;
                in int b_in;
                in int c_in;
                in int d_in;

                out int a_out;
                out int b_out;
                out int c_out;
                out int d_out;

                void main() {
                    a_out = a_in * 2;
                    b_out = b_in * 2;
                    c_out = c_in * 2;
                    d_out = d_in * 2;
                }
            ''',
            varyings=['a_out', 'b_out', 'c_out', 'd_out']
        )

    def test_padding_1(self):
        buf = self.ctx.buffer(struct.pack('=ixi12xii', 1, 2, 3, 4))
        res = self.ctx.buffer(reserve=16)

        vao = self.ctx.vertex_array(self.prog, [
            (buf, 'i x i 12x i i', 'a_in', 'b_in', 'c_in', 'd_in'),
        ])

        vao.transform(res, moderngl.POINTS)
        a, b, c, d = struct.unpack('=iiii', res.read())

        self.assertEqual(a, 2)
        self.assertEqual(b, 4)
        self.assertEqual(c, 6)
        self.assertEqual(d, 8)

    def test_padding_2(self):
        buf = self.ctx.buffer(struct.pack('=i8xi8xi8xi8x', 1, 2, 3, 4))
        res = self.ctx.buffer(reserve=64)

        vao = self.ctx.vertex_array(self.prog, [
            (buf, 'i 8x', 'a_in'),
        ])

        vao.transform(res, moderngl.POINTS)

        self.assertEqual(vao.vertices, 4)

        a1, a2, a3, a4 = struct.unpack('=i12xi12xi12xi12x', res.read())

        self.assertEqual(a1, 2)
        self.assertEqual(a2, 4)
        self.assertEqual(a3, 6)
        self.assertEqual(a4, 8)

    def test_padding_3(self):
        buf = self.ctx.buffer(struct.pack('=128xiiii', 1, 2, 3, 4))
        res = self.ctx.buffer(reserve=16)

        vao = self.ctx.vertex_array(self.prog, [
            (buf, '128x i i i i', 'a_in', 'b_in', 'c_in', 'd_in'),
        ])

        vao.transform(res, moderngl.POINTS)
        a, b, c, d = struct.unpack('=iiii', res.read())

        self.assertEqual(a, 2)
        self.assertEqual(b, 4)
        self.assertEqual(c, 6)
        self.assertEqual(d, 8)


if __name__ == '__main__':
    unittest.main()
