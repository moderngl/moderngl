import struct
import unittest

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context(require=400)
        if not cls.ctx:
            raise unittest.SkipTest("Subroutines not supported")

    def test_1(self):
        vbo1 = self.ctx.buffer(struct.pack('4f', 0.0, 0.0, 0.0, 0.0))
        vbo2 = self.ctx.buffer(struct.pack('4f', 0.0, 0.0, 0.0, 0.0))

        prog = self.ctx.program(
            vertex_shader='''
                #version 400

                in vec4 vert;
                out vec4 color;

                subroutine vec4 color_t();

                subroutine(color_t)
                vec4 ColorRed() {
                    return vec4(1, 0, 0, 1);
                }

                subroutine(color_t)
                vec4 ColorBlue() {
                    return vec4(0, 0.5, 1, 1);
                }

                subroutine(color_t)
                vec4 ColorYellow() {
                    return vec4(1, 1, 0, 1);
                }

                subroutine uniform color_t Color;

                void main() {
                    color = vert + Color();
                }
            ''',
            varyings=['color']
        )
        vao = self.ctx.simple_vertex_array(prog, vbo1, 'vert')

        vao.subroutines = [prog['ColorRed'].index]
        vao.transform(vbo2)

        x, y, z, w = struct.unpack('4f', vbo2.read())
        self.assertAlmostEqual(x, 1.0)
        self.assertAlmostEqual(y, 0.0)
        self.assertAlmostEqual(z, 0.0)
        self.assertAlmostEqual(w, 1.0)

        vao.subroutines = [prog['ColorBlue'].index]
        vao.transform(vbo2)

        x, y, z, w = struct.unpack('4f', vbo2.read())
        self.assertAlmostEqual(x, 0.0)
        self.assertAlmostEqual(y, 0.5)
        self.assertAlmostEqual(z, 1.0)
        self.assertAlmostEqual(w, 1.0)

        vao.subroutines = [prog['ColorYellow'].index]
        vao.transform(vbo2)

        x, y, z, w = struct.unpack('4f', vbo2.read())
        self.assertAlmostEqual(x, 1.0)
        self.assertAlmostEqual(y, 1.0)
        self.assertAlmostEqual(z, 0.0)
        self.assertAlmostEqual(w, 1.0)


if __name__ == '__main__':
    unittest.main()
