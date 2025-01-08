import os

import glm
import moderngl
import moderngl_window as mglw


class Example(mglw.WindowConfig):
    title = 'Resource Loading with ModernGL Window'
    resizable = False
    gl_version = (3, 3)
    window_size = (800, 800)
    aspect_ratio = 1.0
    resource_dir = os.path.normpath(os.path.join(__file__, '../data'))

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.program = self.ctx.program(
            vertex_shader='''
                #version 330 core

                vec2 positions[3] = vec2[](
                    vec2(-1.0, -1.0),
                    vec2(3.0, -1.0),
                    vec2(-1.0, 3.0)
                );

                out vec2 v_uv;

                void main() {
                    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
                    v_uv = positions[gl_VertexID];
                }
            ''',
            fragment_shader='''
                #version 330

                in vec2 v_uv;
                out vec4 f_color;

                uniform sampler2D Texture;

                uniform vec2 seed;
                uniform int iter;

                void main() {
                    vec2 c = seed;
                    int i;

                    vec2 z = v_uv * vec2(2.0, 2.0);

                    for (i = 0; i < iter; i++) {
                        float x = (z.x * z.x - z.y * z.y) + c.x;
                        float y = (z.y * z.x + z.x * z.y) + c.y;

                        if ((x * x + y * y) > 4.0) {
                            break;
                        }

                        z.x = x;
                        z.y = y;
                    }

                    f_color = texture(Texture, vec2((i == iter ? 0.0 : float(i)) / 100.0, 0.0));
                }
            ''',
        )

        self.texture = self.load_texture_2d('colormaps/fractal.png')
        self.sampler = self.ctx.sampler(texture=self.texture)
        self.vao = self.ctx.vertex_array(self.program, [])
        self.vao.vertices = 3

    def on_render(self, time, frame_time):
        self.ctx.clear()
        self.program['seed'] = (-0.8, 0.156)
        self.program['iter'] = 100
        self.sampler.use()
        self.vao.render()


if __name__ == '__main__':
    Example.run()
