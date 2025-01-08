import os

import numpy as np
from pyrr import Matrix44

import moderngl
from _example import Example


def terrain(size):
    vertices = np.dstack(np.mgrid[0:size, 0:size][::-1]) / size
    temp = np.dstack([np.arange(0, size * size - size), np.arange(size, size * size)])
    index = np.pad(temp.reshape(size - 1, 2 * size), [[0, 0], [0, 1]], 'constant', constant_values=-1)
    return vertices, index


class WireframeTerrain(Example):
    title = "Wireframe Terrain"
    gl_version = (3, 3)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                uniform mat4 Mvp;
                uniform sampler2D Heightmap;

                in vec2 in_vert;
                in vec3 in_color;

                out vec3 v_color;

                void main() {
                    v_color = in_color;
                    float height = texture(Heightmap, in_vert.xy).r * 0.5;
                    gl_Position = Mvp * vec4(in_vert.xy - 0.5, height, 1.0);
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

        self.mvp = self.prog['Mvp']

        vertices, index = terrain(32)

        self.vbo = self.ctx.buffer(vertices.astype('f4'))
        self.ibo = self.ctx.buffer(index.astype('i4'))

        vao_content = [
            (self.vbo, '2f', 'in_vert'),
        ]

        self.vao = self.ctx.vertex_array(self.prog, vao_content, self.ibo)
        self.texture = self.load_texture_2d('noise.jpg')

    def on_render(self, time, frame_time):
        angle = time * 0.2

        self.ctx.clear(1.0, 1.0, 1.0)
        self.ctx.enable(moderngl.DEPTH_TEST)
        self.ctx.wireframe = True

        proj = Matrix44.perspective_projection(45.0, self.aspect_ratio, 0.1, 1000.0)
        lookat = Matrix44.look_at(
            (np.cos(angle), np.sin(angle), 0.8),
            (0.0, 0.0, 0.1),
            (0.0, 0.0, 1.0),
        )

        self.texture.use()
        self.mvp.write((proj * lookat).astype('f4'))
        self.vao.render(moderngl.TRIANGLE_STRIP)


if __name__ == '__main__':
    WireframeTerrain.run()
