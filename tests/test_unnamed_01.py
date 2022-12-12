import struct
import pytest
import moderngl

def test_1(ctx):
    prog = ctx.program(
        vertex_shader='''
            #version 330

            in mat2 in_m;
            in vec2 in_v;

            out vec2 out_v;

            uniform float mult;

            void main() {
                out_v = in_m * in_v * mult;
            }
        ''',
        varyings=['out_v']
    )

    buf_m = ctx.buffer(struct.pack('4f', 1, 1, 1, 2))
    buf_v = ctx.buffer(struct.pack('2f', 4, 7))
    res = ctx.buffer(reserve=buf_v.size)

    vao = ctx.vertex_array(prog, [
        (buf_m, '4f', 'in_m'),
        (buf_v, '2f', 'in_v'),
    ])

    prog['mult'].value = 0.0
    vao.transform(res, moderngl.POINTS)
    x, y = struct.unpack('2f', res.read())
    assert pytest.approx(x) == 0.0
    assert pytest.approx(y) == 0.0

    prog['mult'].value = 1.0
    vao.transform(res, moderngl.POINTS)
    x, y = struct.unpack('2f', res.read())
    assert pytest.approx(x) == 11.0
    assert pytest.approx(y) == 18.0

    prog['mult'].value = 2.0
    vao.transform(res, moderngl.POINTS)
    x, y = struct.unpack('2f', res.read())
    assert pytest.approx(x) == 22.0
    assert pytest.approx(y) == 36.0
