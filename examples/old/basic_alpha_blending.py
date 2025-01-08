'''
    Alpha blending
'''

import numpy as np

import moderngl
from _example import Example


class AlphaBlending(Example):
    gl_version = (3, 3)
    title = "Alpha Blending"

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                in vec2 vert;

                in vec4 vert_color;
                out vec4 frag_color;

                uniform vec2 scale;
                uniform float rotation;

                void main() {
                    frag_color = vert_color;
                    float r = rotation * (0.5 + gl_InstanceID * 0.05);
                    mat2 rot = mat2(cos(r), sin(r), -sin(r), cos(r));
                    gl_Position = vec4((rot * vert) * scale, 0.0, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330
                in vec4 frag_color;
                out vec4 color;
                void main() {
                    color = vec4(frag_color);
                }
            ''',
        )

        self.scale = self.prog['scale']
        self.rotation = self.prog['rotation']

        self.scale.value = (0.5, self.aspect_ratio * 0.5)

        vertices = np.array([
            1.0, 0.0,
            1.0, 0.0, 0.0, 0.5,

            -0.5, 0.86,
            0.0, 1.0, 0.0, 0.5,

            -0.5, -0.86,
            0.0, 0.0, 1.0, 0.5,
        ], dtype='f4')

        self.vbo = self.ctx.buffer(vertices)
        self.vao = self.ctx.vertex_array(self.prog, [
            self.vbo.bind('vert', 'vert_color'),
        ])

    def on_render(self, time: float, frame_time: float):
        self.ctx.clear(1.0, 1.0, 1.0)
        self.ctx.enable(moderngl.BLEND)
        self.rotation.value = time
        self.vao.render(instances=10)


if __name__ == '__main__':
    AlphaBlending.run()
