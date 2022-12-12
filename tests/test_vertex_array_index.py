import moderngl
import pytest
import numpy as np

def test_1(ctx):
    prog = ctx.program(
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

    vertices = [
        4.0, 2.0, 7.5, 1.8,
        3.0, 2.8, 6.0, 10.0
    ]
    count = 10
    indices = [0, 1] * 10

    # UNSIGNED_INT index
    vbo1 = ctx.buffer(np.array(vertices, dtype='f4').tobytes())
    vbo2 = ctx.buffer(reserve=vbo1.size * count)
    index = ctx.buffer(np.array(indices, dtype='u4').tobytes())
    vao = ctx.simple_vertex_array(prog, vbo1, 'in_vert', index_buffer=index, index_element_size=4)
    vao.transform(vbo2, moderngl.POINTS)
    res = np.frombuffer(vbo2.read(), dtype='f4')
    np.testing.assert_almost_equal(res, vertices * count)

    # UNSIGNED_SHORT index
    vbo1 = ctx.buffer(np.array(vertices, dtype='f4').tobytes())
    vbo2 = ctx.buffer(reserve=vbo1.size * count)
    index = ctx.buffer(np.array(indices, dtype='u2').tobytes())
    vao = ctx.simple_vertex_array(prog, vbo1, 'in_vert', index_buffer=index, index_element_size=2)
    vao.transform(vbo2, moderngl.POINTS)
    res = np.frombuffer(vbo2.read(), dtype='f4')
    np.testing.assert_almost_equal(res, vertices * count)

    # UNSIGNED_BYTE index
    vbo1 = ctx.buffer(np.array(vertices, dtype='f4').tobytes())
    vbo2 = ctx.buffer(reserve=vbo1.size * count)
    index = ctx.buffer(np.array(indices, dtype='u1').tobytes())
    vao = ctx.simple_vertex_array(prog, vbo1, 'in_vert', index_buffer=index, index_element_size=1)
    vao.transform(vbo2, moderngl.POINTS)
    res = np.frombuffer(vbo2.read(), dtype='f4')
    np.testing.assert_almost_equal(res, vertices * count)

def test_2(ctx):
    prog = ctx.program(
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

    vertices = [
        4.0, 2.0, 7.5, 1.8,
        3.0, 2.8, 6.0, 10.0
    ]
    indices = [0, 1, 0, 1, 0, 1, 0, 1, 0]

    vbo1 = ctx.buffer(np.array(vertices, dtype='f4').tobytes())
    index_u4 = ctx.buffer(np.array(indices, dtype='u4').tobytes())

    with pytest.raises(moderngl.Error):
        ctx.simple_vertex_array(prog, vbo1, 'in_vert', index_buffer=index_u4, index_element_size=0)
