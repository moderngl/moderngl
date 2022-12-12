import struct
import pytest

def test_1(ctx):
    if ctx.version_code < 400:
        pytest.skip('Subroutines are not supported')

    vbo1 = ctx.buffer(struct.pack('4f', 0.0, 0.0, 0.0, 0.0))
    vbo2 = ctx.buffer(struct.pack('4f', 0.0, 0.0, 0.0, 0.0))

    prog = ctx.program(
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
    vao = ctx.simple_vertex_array(prog, vbo1, 'vert')

    vao.subroutines = [prog['ColorRed'].index]
    vao.transform(vbo2)

    x, y, z, w = struct.unpack('4f', vbo2.read())
    assert pytest.approx(x) == 1.0
    assert pytest.approx(y) == 0.0
    assert pytest.approx(z) == 0.0
    assert pytest.approx(w) == 1.0

    vao.subroutines = [prog['ColorBlue'].index]
    vao.transform(vbo2)

    x, y, z, w = struct.unpack('4f', vbo2.read())
    assert pytest.approx(x) == 0.0
    assert pytest.approx(y) == 0.5
    assert pytest.approx(z) == 1.0
    assert pytest.approx(w) == 1.0

    vao.subroutines = [prog['ColorYellow'].index]
    vao.transform(vbo2)

    x, y, z, w = struct.unpack('4f', vbo2.read())
    assert pytest.approx(x) == 1.0
    assert pytest.approx(y) == 1.0
    assert pytest.approx(z) == 0.0
    assert pytest.approx(w) == 1.0
