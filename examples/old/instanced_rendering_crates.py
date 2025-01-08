import numpy as np
from pyrr import Matrix44

import moderngl
from _example import Example


class InstancedCrates(Example):
    '''
        This example renders 32x32 crates.
        For each crate the location is [x, y, sin(a * time + b)]
        There are 1024 crates aligned in a grid.
    '''
    title = "Instanced Crates"
    gl_version = (3, 3)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                uniform mat4 Mvp;

                in vec3 in_move;

                in vec3 in_position;
                in vec3 in_normal;
                in vec2 in_texcoord_0;

                out vec3 v_vert;
                out vec3 v_norm;
                out vec2 v_text;

                void main() {
                    gl_Position = Mvp * vec4(in_position + in_move, 1.0);
                    v_vert = in_position + in_move;
                    v_norm = in_normal;
                    v_text = in_texcoord_0;
                }
            ''',
            fragment_shader='''
                #version 330

                uniform vec3 Light;
                uniform sampler2D Texture;

                in vec3 v_vert;
                in vec3 v_norm;
                in vec2 v_text;

                out vec4 f_color;

                void main() {
                    float lum = clamp(dot(normalize(Light - v_vert), normalize(v_norm)), 0.0, 1.0) * 0.8 + 0.2;
                    f_color = vec4(texture(Texture, v_text).rgb * lum, 1.0);
                }
            ''',
        )

        self.mvp = self.prog['Mvp']
        self.light = self.prog['Light']

        self.scene = self.load_scene('crate.obj')
        self.texture = self.load_texture_2d('crate.png')

        # Add a new buffer into the VAO wrapper in the scene.
        # This is simply a collection of named buffers that is auto mapped
        # to attributes in the vertex shader with the same name.
        self.instance_data = self.ctx.buffer(reserve=12 * 1024)
        vao_wrapper = self.scene.root_nodes[0].mesh.vao
        vao_wrapper.buffer(self.instance_data, '3f/i', 'in_move')
        # Create the actual vao instance (auto mapping in action)
        self.vao = vao_wrapper.instance(self.prog)

        self.crate_a = np.random.uniform(0.7, 0.8, 32 * 32)
        self.crate_b = np.random.uniform(0.0, 6.3, 32 * 32)
        self.crate_x = (np.tile(np.arange(32), 32) - 16) * 1.5
        self.crate_y = (np.repeat(np.arange(32), 32) - 16) * 1.5
        self.crate_x += np.random.uniform(-0.2, 0.2, 32 * 32)
        self.crate_y += np.random.uniform(-0.2, 0.2, 32 * 32)

    def on_render(self, time, frame_time):
        angle = time * 0.2
        self.ctx.clear(1.0, 1.0, 1.0)
        self.ctx.enable(moderngl.DEPTH_TEST)

        camera_pos = (np.cos(angle) * 5.0, np.sin(angle) * 5.0, 2.0)

        proj = Matrix44.perspective_projection(45.0, self.aspect_ratio, 0.1, 1000.0)
        lookat = Matrix44.look_at(
            camera_pos,
            (0.0, 0.0, 0.5),
            (0.0, 0.0, 1.0),
        )

        self.mvp.write((proj * lookat).astype('f4'))
        self.light.value = camera_pos

        crate_z = np.sin(self.crate_a * time + self.crate_b) * 0.2
        coordinates = np.dstack([self.crate_x, self.crate_y, crate_z])

        self.instance_data.write(coordinates.astype('f4'))
        self.texture.use()
        self.vao.render(instances=1024)


if __name__ == '__main__':
    InstancedCrates.run()
