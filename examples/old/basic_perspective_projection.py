'''
    Renders a blue triangle
'''

import numpy as np

import moderngl
from _example import Example


class PerspectiveProjection(Example):
    gl_version = (3, 3)
    title = "Perspective Projection"

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                in vec3 vert;

                uniform float z_near;
                uniform float z_far;
                uniform float fovy;
                uniform float ratio;

                uniform vec3 center;
                uniform vec3 eye;
                uniform vec3 up;

                mat4 perspective() {
                    float zmul = (-2.0 * z_near * z_far) / (z_far - z_near);
                    float ymul = 1.0 / tan(fovy * 3.14159265 / 360);
                    float xmul = ymul / ratio;

                    return mat4(
                        xmul, 0.0, 0.0, 0.0,
                        0.0, ymul, 0.0, 0.0,
                        0.0, 0.0, -1.0, -1.0,
                        0.0, 0.0, zmul, 0.0
                    );
                }

                mat4 lookat() {
                    vec3 forward = normalize(center - eye);
                    vec3 side = normalize(cross(forward, up));
                    vec3 upward = cross(side, forward);

                    return mat4(
                        side.x, upward.x, -forward.x, 0,
                        side.y, upward.y, -forward.y, 0,
                        side.z, upward.z, -forward.z, 0,
                        -dot(eye, side), -dot(eye, upward), dot(eye, forward), 1
                    );
                }

                void main() {
                    gl_Position = perspective() * lookat() * vec4(vert, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330

                out vec4 color;

                void main() {
                    color = vec4(0.04, 0.04, 0.04, 1.0);

                }
            ''',
        )

        self.prog['z_near'].value = 0.1
        self.prog['z_far'].value = 1000.0
        self.prog['ratio'].value = self.aspect_ratio
        self.prog['fovy'].value = 60

        self.prog['eye'].value = (3, 3, 3)
        self.prog['center'].value = (0, 0, 0)
        self.prog['up'].value = (0, 0, 1)

        grid = []

        for i in range(65):
            grid.append([i - 32, -32.0, 0.0, i - 32, 32.0, 0.0])
            grid.append([-32.0, i - 32, 0.0, 32.0, i - 32, 0.0])

        grid = np.array(grid, dtype='f4')

        self.vbo = self.ctx.buffer(grid)
        self.vao = self.ctx.vertex_array(self.prog, [self.vbo.bind('vert')])

    def on_render(self, time: float, frame_time: float):
        self.ctx.clear(1.0, 1.0, 1.0)
        self.vao.render(moderngl.LINES, 65 * 4)


if __name__ == '__main__':
    PerspectiveProjection.run()
