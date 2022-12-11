import numpy as np
import numpy.testing as npt
import moderngl


def test_1(ctx):
    prog = ctx.program(
        vertex_shader='''
            #version 330

            in vec2 in_vert;

            void main() {
                gl_Position = vec4(in_vert, 0.0, 1.0);
            }
        ''',
        fragment_shader='''
            #version 330

            out vec4 f_color;

            void main() {
                f_color = vec4(0.0, 0.5, 1.0, 1.0);
            }
        ''',
    )

    vbo = ctx.buffer(np.array([-1, -1, -1, 1, 1, -1, 1, 1], dtype='f4'))
    vao = ctx.simple_vertex_array(prog, vbo, 'in_vert')
    fbo = ctx.simple_framebuffer((4, 4), dtype='f2')

    fbo.use()
    fbo.clear()
    vao.render(moderngl.TRIANGLE_STRIP)
    res = np.frombuffer(fbo.read(dtype='f2'), dtype='f2')
    expected = np.tile([0.0, 0.5, 1.0], 4 * 4)
    npt.assert_almost_equal(res, expected, decimal=2)
