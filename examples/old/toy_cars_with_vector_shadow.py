import struct
from colorsys import hls_to_rgb as hls
from random import uniform

import numpy as np
from pyrr import Matrix44

import moderngl
from _example import Example


def random_color():
    return hls(uniform(0.0, 1.0), 0.5, 0.5)


cars = []
cars += [{'color': random_color(),
          'pos': (1.5, i * 2.0 - 9.0, 0.0),
          'angle': uniform(-0.5, 0.5)} for i in range(10)]

cars += [{'color': random_color(),
          'pos': (-1.5, i * 2.0 - 9.0, 0.0),
          'angle': uniform(-0.5, 0.5)} for i in range(10)]


class ToyCars(Example):
    title = "Toy Cars With Vector Shadow"
    gl_version = (3, 3)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                uniform mat4 Mvp;

                in vec3 in_position;
                in vec3 in_normal;

                in vec3 in_color;
                in vec3 in_origin;
                in mat3 in_basis;

                out vec3 v_vert;
                out vec3 v_norm;
                out vec3 v_color;

                void main() {
                    v_vert = in_origin + in_basis * in_position;
                    v_norm = in_basis * in_normal;
                    v_color = in_color;
                    gl_Position = Mvp * vec4(v_vert, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330

                uniform vec3 Light;
                uniform sampler2D Texture;

                in vec3 v_vert;
                in vec3 v_norm;
                in vec3 v_color;

                out vec4 f_color;

                void main() {
                    float lum = clamp(dot(normalize(Light - v_vert), normalize(v_norm)), 0.0, 1.0) * 0.8 + 0.2;
                    f_color = vec4(v_color * lum, 1.0);
                }
            ''',
        )

        self.mvp = self.prog['Mvp']
        self.light = self.prog['Light']

        obj = self.load_scene('lowpoly_toy_car.obj')

        self.vbo = self.ctx.buffer(struct.pack(
            '15f',
            1.0, 1.0, 1.0,
            0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, 1.0,
        ) * len(cars))

        vao_wrapper = obj.root_nodes[0].mesh.vao
        vao_wrapper.buffer(self.vbo, '3f 3f 9f/i', ['in_color', 'in_origin', 'in_basis'])
        self.vao = vao_wrapper.instance(self.prog)

    def on_render(self, time, frame_time):
        angle = time
        self.ctx.clear(1.0, 1.0, 1.0)
        self.ctx.enable(moderngl.DEPTH_TEST)

        camera_pos = (np.cos(angle) * 20.0, np.sin(angle) * 20.0, 5.0)

        proj = Matrix44.perspective_projection(45.0, self.aspect_ratio, 0.1, 1000.0)
        lookat = Matrix44.look_at(
            camera_pos,
            (0.0, 0.0, 0.5),
            (0.0, 0.0, 1.0),
        )

        self.mvp.write((proj * lookat).astype('f4'))
        self.light.value = camera_pos

        self.vbo.write(b''.join(struct.pack(
            '15f',
            *car['color'],
            *car['pos'],
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, 1.0,
        ) for car in cars))
        self.vao.render(instances=len(cars))

        self.vbo.write(b''.join(struct.pack(
            '15f',
            0.0, 0.0, 0.0,
            *car['pos'],
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.3, 0.6, 0.0,
        ) for car in cars))
        self.vao.render(instances=len(cars))


if __name__ == '__main__':
    ToyCars.run()
