'''
    Renders a floating, oscillating, 3d islans with lights
'''

import numpy as np
from pyrr import Matrix44

import moderngl
from _example import Example


class ColorsAndTexture(Example):
    gl_version = (3, 3)
    title = "Colors and Textures"

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

                uniform vec3 Light;
                uniform vec3 Color;
                uniform bool UseTexture;
                uniform sampler2D Texture;

                in vec3 v_vert;
                in vec3 v_norm;
                in vec2 v_text;

                out vec4 f_color;

                void main() {
                    float lum = clamp(dot(normalize(Light - v_vert), normalize(v_norm)), 0.0, 1.0) * 0.8 + 0.2;
                    if (UseTexture) {
                        f_color = vec4(texture(Texture, v_text).rgb * lum, 1.0);
                    } else {
                        f_color = vec4(Color * lum, 1.0);
                    }
                }
            ''',
        )

        self.mvp = self.prog['Mvp']
        self.light = self.prog['Light']
        self.color = self.prog['Color']
        self.use_texture = self.prog['UseTexture']

        # Note: This is a fairly manual way to loading and rendering wavefront files.
        # There are easier ways when loading multiple objects in a single obj file.

        # Load obj files
        self.scene_ground = self.load_scene('scene-1-ground.obj')
        self.scene_grass = self.load_scene('scene-1-grass.obj')
        self.scene_billboard = self.load_scene('scene-1-billboard.obj')
        self.scene_holder = self.load_scene('scene-1-billboard-holder.obj')
        self.scene_image = self.load_scene('scene-1-billboard-image.obj')

        # Extract the VAOs from the scene
        self.vao_ground = self.scene_ground.root_nodes[0].mesh.vao.instance(self.prog)
        self.vao_grass = self.scene_grass.root_nodes[0].mesh.vao.instance(self.prog)
        self.vao_billboard = self.scene_billboard.root_nodes[0].mesh.vao.instance(self.prog)
        self.vao_holder = self.scene_holder.root_nodes[0].mesh.vao.instance(self.prog)
        self.vao_image = self.scene_image.root_nodes[0].mesh.vao.instance(self.prog)

        # texture on billboard
        self.texture = self.load_texture_2d('infographic-1.jpg')

    def on_render(self, time: float, frame_time: float):
        self.ctx.clear(1.0, 1.0, 1.0)
        self.ctx.enable(moderngl.DEPTH_TEST | moderngl.CULL_FACE)

        proj = Matrix44.perspective_projection(45.0, self.aspect_ratio, 0.1, 1000.0)
        lookat = Matrix44.look_at(
            (47.697, -8.147, 24.498),
            (0.0, 0.0, 8.0),
            (0.0, 0.0, 1.0),
        )

        rotate = Matrix44.from_z_rotation(np.sin(time) * 0.5 + 0.2)

        self.use_texture.value = False

        self.light.value = (67.69, -8.14, 52.49)
        self.mvp.write((proj * lookat * rotate).astype('f4'))

        self.color.value = (0.67, 0.49, 0.29)
        self.vao_ground.render()

        self.color.value = (0.46, 0.67, 0.29)
        self.vao_grass.render()

        self.color.value = (1.0, 1.0, 1.0)
        self.vao_billboard.render()

        self.color.value = (0.2, 0.2, 0.2)
        self.vao_holder.render()

        self.use_texture.value = True
        self.texture.use()

        self.vao_image.render()


if __name__ == '__main__':
    ColorsAndTexture.run()
