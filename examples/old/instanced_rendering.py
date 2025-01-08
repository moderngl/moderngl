"""
Instanced rendering without supplying per instance data
doing offsets with gl_InstanceID in vertex shader.
"""
import numpy as np

import moderngl
from _example import Example


class InstancedRendering(Example):
    title = "Instanced Rendering"
    gl_version = (3, 3)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        # gl_InstanceID offsets rotation per instance
        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                in vec2 in_vert;
                in vec4 in_color;

                out vec4 v_color;

                uniform float Rotation;
                uniform vec2 Scale;

                void main() {
                    v_color = in_color;
                    float r = Rotation * (0.5 + gl_InstanceID * 0.05);
                    mat2 rot = mat2(cos(r), sin(r), -sin(r), cos(r));
                    gl_Position = vec4((rot * in_vert) * Scale, 0.0, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330

                in vec4 v_color;
                out vec4 f_color;

                void main() {
                    f_color = v_color;
                }
            ''',
        )

        self.scale = self.prog['Scale']
        self.rotation = self.prog['Rotation']

        vertices = np.array([
            # x, y, red, green, blue, alpha
            1.0, 0.0, 1.0, 0.0, 0.0, 0.5,
            -0.5, 0.86, 0.0, 1.0, 0.0, 0.5,
            -0.5, -0.86, 0.0, 0.0, 1.0, 0.5,
        ], dtype='f4')

        self.vbo = self.ctx.buffer(vertices)
        self.vao = self.ctx.vertex_array(
            self.prog,
            [(self.vbo, '2f 4f', 'in_vert', 'in_color')],
        )

    def on_render(self, time, frame_time):
        self.ctx.clear(1.0, 1.0, 1.0)
        self.ctx.enable(moderngl.BLEND)
        self.scale.value = (0.5, self.aspect_ratio * 0.5)
        self.rotation.value = time
        # For every instanced rendered gl_InstanceID increments by 1
        self.vao.render(instances=10)


if __name__ == '__main__':
    InstancedRendering.run()
