import struct
import pytest
import moderngl


def test_1(ctx):
    prog = ctx.program(
        vertex_shader='''
            #version 330

            in vec2 in_v1;
            in vec2 in_v2;

            out vec2 out_v;

            void main() {
                out_v = in_v1 + in_v2;
            }
        ''',
        varyings=['out_v']
    )

    buf_v1 = ctx.buffer(struct.pack('8f', 1, 2, 3, 4, 5, 6, 7, 8))
    buf_v2 = ctx.buffer(struct.pack('2f', 10, 100))
    res = ctx.buffer(reserve=buf_v1.size)

    vao = ctx.vertex_array(prog, [
        (buf_v1, '2f', 'in_v1'),
        (buf_v2, '2f/i', 'in_v2'),
    ])

    vao.transform(res, moderngl.POINTS)
    tup = struct.unpack('8f', res.read())
    assert pytest.approx(tup[0]) == 11.0
    assert pytest.approx(tup[1]) == 102.0
    assert pytest.approx(tup[2]) == 13.0
    assert pytest.approx(tup[3]) == 104.0
    assert pytest.approx(tup[4]) == 15.0
    assert pytest.approx(tup[5]) == 106.0
    assert pytest.approx(tup[6]) == 17.0
    assert pytest.approx(tup[7]) == 108.0
