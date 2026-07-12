import moderngl
import numpy as np
from pyrr import Matrix44

from _example import Example


class CrateExample(Example):
    title = "Crate"

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                uniform mat4 Mvp;

                in vec3 in_position;
                in vec3 in_normal;
                in vec2 in_texcoord_0;

                out vec3 v_vert;
                out vec3 v_norm;
                out vec2 v_text;

                void main() {
                    gl_Position = Mvp * vec4(in_position, 1.0);
                    v_vert = in_position;
                    v_norm = in_normal;
                    v_text = in_texcoord_0;
                }
            ''',
            fragment_shader='''
                #version 330
                // #extension GL_ARB_explicit_uniform_location : require
                #extension GL_ARB_bindless_texture : enable

                uniform vec3 Light;
                uniform int texture_selector;
                layout (bindless_sampler) uniform sampler2D Textures[6];

                in vec3 v_vert;
                in vec3 v_norm;
                in vec2 v_text;

                out vec4 f_color;

                void main() {
                    float lum = clamp(dot(normalize(Light - v_vert), normalize(v_norm)), 0.0, 1.0) * 0.8 + 0.2;
                    f_color = vec4(texture(Textures[texture_selector], v_text).rgb * lum, 1.0);
                }
            ''',
        )

        self.mvp = self.prog['Mvp']
        self.light = self.prog['Light']
        self.texture_selector = self.prog['texture_selector']

        self.scene = self.load_scene('../data/models/crate.obj')
        self.vao = self.scene.root_nodes[0].mesh.vao.instance(self.prog)

        # Load a set of textures to cycle through
        textures = [self.load_texture_2d('../data/textures/crate.png'),
                    self.load_texture_2d('../data/textures/rock.jpg'),
                    self.load_texture_2d('../data/textures/arrows.png'),
                    self.load_texture_2d('../data/textures/wood.jpg'),
                    self.load_texture_2d('../data/textures/grass.jpg'),
                    self.load_texture_2d('../data/textures/tiles.jpg')]
        handles = [t.get_handle() for t in textures]
        self.prog['Textures'].handle = handles

    def on_render(self, time, frame_time):
        angle = time
        self.ctx.clear(1.0, 1.0, 1.0)
        self.ctx.enable(moderngl.DEPTH_TEST)

        camera_pos = (np.cos(angle) * 3.0, np.sin(angle) * 3.0, 2.0)

        proj = Matrix44.perspective_projection(45.0, self.aspect_ratio, 0.1, 100.0)
        lookat = Matrix44.look_at(
            camera_pos,
            (0.0, 0.0, 0.5),
            (0.0, 0.0, 1.0),
        )

        self.mvp.write((proj * lookat).astype('f4'))
        self.light.value = camera_pos
        self.texture_selector.value = int(time) % 6
        # self.texture.use()
        self.vao.render()


if __name__ == '__main__':
    CrateExample.run()
