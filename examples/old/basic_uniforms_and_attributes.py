'''
    Renders a rotating and scaling triangle
'''

import numpy as np

from _example import Example


class UniformsAndAttributes(Example):
    gl_version = (3, 3)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330
                in vec2 vert;

                uniform vec2 scale;
                uniform float rotation;

                void main() {

                    mat2 rot = mat2(
                        cos(rotation), sin(rotation),
                        -sin(rotation), cos(rotation)
                    );
                    gl_Position = vec4((rot * vert) * scale, 0.0, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330

                out vec4 color;
                void main() {
                    color = vec4(0.3, 0.5, 1.0, 1.0);
                }
            ''',
        )

        self.scale = self.prog['scale']
        self.rotation = self.prog['rotation']

        self.scale.value = (self.wnd.width / self.wnd.height * 0.75, 0.25)

        vertices = np.array([
            1.0, 0.0,
            -0.5, 0.86,
            -0.5, -0.86,
        ], dtype='f4')

        self.vbo = self.ctx.buffer(vertices)
        self.vao = self.ctx.vertex_array(self.prog, [self.vbo.bind('vert')])

    def on_render(self, time: float, frame_time: float):
        sin_scale = np.sin(np.deg2rad(time * 60))

        self.ctx.clear(1.0, 1.0, 1.0)
        self.vao.render()
        self.rotation.value = time

        # Change the scale of the triangle sin-ly
        self.scale.value = (sin_scale * 0.75, 0.75)


if __name__ == '__main__':
    UniformsAndAttributes.run()
