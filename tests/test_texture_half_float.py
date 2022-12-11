import numpy as np
import numpy.testing as npt

import moderngl

def test_1(ctx):
    prog = ctx.program(
        vertex_shader='''
            #version 330

            in vec2 in_vert;
            out vec2 v_text;

            void main() {
                v_text = in_vert / 2.0 + 0.5;
                gl_Position = vec4(in_vert, 0.0, 1.0);
            }
        ''',
        fragment_shader='''
            #version 330

            uniform sampler2D Texture;

            in vec2 v_text;
            out vec4 f_color;

            void main() {
                f_color = texture(Texture, v_text);
            }
        ''',
    )

    pixels = np.tile([0.0, 0.5, 1.0], 4 * 4).astype('f2')
    tex = ctx.texture((4, 4), 3, pixels, dtype='f2')

    vbo = ctx.buffer(np.array([-1, -1, -1, 1, 1, -1, 1, 1], dtype='f4'))
    vao = ctx.simple_vertex_array(prog, vbo, 'in_vert')
    fbo = ctx.simple_framebuffer((4, 4))

    fbo.use()
    fbo.clear()
    tex.use()
    vao.render(moderngl.TRIANGLE_STRIP)
    res = np.frombuffer(fbo.read(dtype='f2'), dtype='f2')
    npt.assert_almost_equal(res, pixels, decimal=2)
