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

                out vec3 v_color;

                in vec2 in_position;

                vec2 positions[3] = vec2[](
                    vec2(0.0, 0.08),
                    vec2(-0.06, -0.08),
                    vec2(0.06, -0.08)
                );

                vec3 colors[3] = vec3[](
                    vec3(1.0, 0.0, 0.0),
                    vec3(0.0, 1.0, 0.0),
                    vec3(0.0, 0.0, 1.0)
                );

                void main() {
                    gl_Position = vec4(in_position + positions[gl_VertexID], 0.0, 1.0);
                    v_color = colors[gl_VertexID];
                }
            ''',
            fragment_shader='''
                #version 330

                in vec3 v_color;

                layout (location = 0) out vec4 out_color;

                void main() {
                    out_color = vec4(v_color, 1.0);
                }
            ''',
        )

        positions = np.random.uniform(-1.0, 1.0, (100, 2)).astype('f4')

        self.vbo = self.ctx.buffer(positions)
        self.vao = self.ctx.vertex_array(self.prog, [
            (self.vbo, '2f/i', 'in_position'),
        ])
        self.vao.vertices = 3

    def on_render(self, time, frame_time):
        self.ctx.clear(1.0, 1.0, 1.0)
        self.vao.render(instances=100)


if __name__ == '__main__':
    HelloWorld.run()
