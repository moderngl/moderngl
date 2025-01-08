'''
    Renders a blue triangle
'''

import numpy as np

from _example import Example


class HelloWorld(Example):
    gl_version = (3, 3)
    title = "Hello World"

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                layout (location = 3) in vec2 in_vert;
                layout (location = 7) in vec3 in_color;

                out vec3 v_color;

                void main() {
                    v_color = in_color;
                    gl_Position = vec4(in_vert, 0.0, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330

                in vec3 v_color;
                out vec4 f_color;

                void main() {
                    f_color = vec4(v_color, 1.0);
                }
            ''',
        )

        vertices = np.array([
            0.0, 0.8, 0.3, 0.5, 1.0,
            -0.6, -0.8, 0.3, 0.5, 1.0,
            0.6, -0.8, 0.3, 0.5, 1.0,
        ], dtype='f4')

        self.vbo = self.ctx.buffer(vertices)
        self.vao = self.ctx.vertex_array(self.prog, [
            self.vbo.bind(3, 7),
        ])

    def on_render(self, time, frame_time):
        self.ctx.clear(1.0, 1.0, 1.0)
        self.vao.render()


if __name__ == '__main__':
    HelloWorld.run()
