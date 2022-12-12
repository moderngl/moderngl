import struct
import pytest
import moderngl

def test_vao_attribs(ctx):

    program = ctx.program(
        vertex_shader='''
            #version 330

            layout(location = 0) in vec4 in_vert;
            out vec4 out_vert;

            void main() {
                out_vert = in_vert;
            }
        ''',
        varyings=['out_vert']
    )

    vbo = ctx.buffer(struct.pack('12f', 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12))
    res = ctx.buffer(reserve=struct.calcsize('8f'))

    vao = ctx.simple_vertex_array(program, vbo, 'in_vert')
    vao.transform(res, moderngl.POINTS, 2)

    for a, b in zip(struct.unpack('8f', res.read()), (1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0)):
        assert pytest.approx(a) == b

    vao.bind(0, 'f', vbo, '4f', offset=4, stride=0, divisor=0)
    vao.transform(res, moderngl.POINTS, 2)

    for a, b in zip(struct.unpack('8f', res.read()), (2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0)):
        assert pytest.approx(a) == b

    vao.bind(0, 'f', vbo, '4f', offset=8, stride=20, divisor=0)
    vao.transform(res, moderngl.POINTS, 2)

    for a, b in zip(struct.unpack('8f', res.read()), (3.0, 4.0, 5.0, 6.0, 8.0, 9.0, 10.0, 11.0)):
        assert pytest.approx(a) == b

    vao.bind(0, 'f', vbo, '4f', offset=12, stride=0, divisor=1)
    vao.transform(res, moderngl.POINTS, 2)

    for a, b in zip(struct.unpack('8f', res.read()), (4.0, 5.0, 6.0, 7.0, 4.0, 5.0, 6.0, 7.0)):
        assert pytest.approx(a) == b
