import numpy as np
from pyrr import Matrix44

import moderngl
from _example import Example


def terrain(size):
    vertices = np.dstack(np.mgrid[0:size, 0:size][::-1]) / size
    temp = np.dstack([np.arange(0, size * size - size), np.arange(size, size * size)])
    index = np.pad(temp.reshape(size - 1, 2 * size), [[0, 0], [0, 1]], 'constant', constant_values=-1)
    return vertices, index


class MultiTextireTerrain(Example):
    title = "Multitexture Terrain"
    gl_version = (3, 3)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                uniform mat4 Mvp;
                uniform sampler2D Heightmap;

                in vec2 in_vert;
                out vec2 v_text;

                void main() {
                    vec4 vertex = vec4(in_vert - 0.5, texture(Heightmap, in_vert).r * 0.2, 1.0);
                    gl_Position = Mvp * vertex;
                    v_text = in_vert;
                }
            ''',
            fragment_shader='''
                #version 330

                uniform sampler2D Heightmap;

                uniform sampler2D Color1;
                uniform sampler2D Color2;

                uniform sampler2D Cracks;
                uniform sampler2D Darken;

                in vec2 v_text;

                out vec4 f_color;

                void main() {
                    float height = texture(Heightmap, v_text).r;
                    float border = smoothstep(0.5, 0.7, height);

                    vec3 color1 = texture(Color1, v_text * 7.0).rgb;
                    vec3 color2 = texture(Color2, v_text * 6.0).rgb;

                    vec3 color = color1 * (1.0 - border) + color2 * border;

                    color *= 0.8 + 0.2 * texture(Darken, v_text * 3.0).r;
                    color *= 0.5 + 0.5 * texture(Cracks, v_text * 5.0).r;
                    color *= 0.5 + 0.5 * height;

                    f_color = vec4(color, 1.0);
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

        self.tex0 = self.load_texture_2d('heightmap.jpg')
        self.tex0.build_mipmaps()
        self.tex1 = self.load_texture_2d('grass.jpg')
        self.tex1.build_mipmaps()
        self.tex2 = self.load_texture_2d('rock.jpg')
        self.tex2.build_mipmaps()
        self.tex3 = self.load_texture_2d('cracks.jpg')
        self.tex3.build_mipmaps()
        self.tex4 = self.load_texture_2d('checked.jpg')
        self.tex4.build_mipmaps()

        self.prog['Heightmap'].value = 0
        self.prog['Color1'].value = 1
        self.prog['Color2'].value = 2
        self.prog['Cracks'].value = 3
        self.prog['Darken'].value = 4

    def on_render(self, time, frame_time):
        angle = time * 0.2
        self.ctx.clear(1.0, 1.0, 1.0)
        self.ctx.enable(moderngl.DEPTH_TEST)

        self.tex0.use(0)
        self.tex1.use(1)
        self.tex2.use(2)
        self.tex3.use(3)
        self.tex4.use(4)

        proj = Matrix44.perspective_projection(45.0, self.aspect_ratio, 0.1, 1000.0)
        lookat = Matrix44.look_at(
            (np.cos(angle), np.sin(angle), 0.8),
            (0.0, 0.0, 0.1),
            (0.0, 0.0, 1.0),
        )

        self.mvp.write((proj * lookat).astype('f4'))
        self.vao.render(moderngl.TRIANGLE_STRIP)


if __name__ == '__main__':
    MultiTextireTerrain.run()
