import os

import glm
import moderngl
import moderngl_window as mglw


class Example(mglw.WindowConfig):
    title = 'Resource Loading with ModernGL Window'
    resizable = False
    gl_version = (3, 3)
    window_size = (800, 800)
    aspect_ratio = 1.0
    resource_dir = os.path.normpath(os.path.join(__file__, '../data'))

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.obj = self.load_scene('models/sitting_dummy.obj')
        self.texture = self.load_texture_2d('textures/wood.jpg')

        self.program = self.ctx.program(
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
                    v_vert = in_position;
                    v_norm = in_normal;
                    v_text = in_texcoord_0;
                    gl_Position = Mvp * vec4(in_position, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330

                uniform sampler2D Texture;
                uniform vec4 Color;
                uniform vec3 Light;

                in vec3 v_vert;
                in vec3 v_norm;
                in vec2 v_text;

                out vec4 f_color;

                void main() {
                    float lum = dot(normalize(v_norm), normalize(v_vert - Light));
                    lum = acos(lum) / 3.14159265;
                    lum = clamp(lum, 0.0, 1.0);
                    lum = lum * lum;
                    lum = smoothstep(0.0, 1.0, lum);
                    lum *= smoothstep(0.0, 80.0, v_vert.z) * 0.3 + 0.7;
                    lum = lum * 0.8 + 0.2;

                    vec3 color = texture(Texture, v_text).rgb;
                    color = color * (1.0 - Color.a) + Color.rgb * Color.a;
                    f_color = vec4(color * lum, 1.0);
                }
            ''',
        )

        # Create a vao from the first root node (attribs are auto mapped)
        self.vao = self.obj.root_nodes[0].mesh.vao.instance(self.program)

    def camera_matrix(self):
        proj = glm.perspective(45.0, self.aspect_ratio, 0.1, 1000.0)
        look = glm.lookAt((-85.0, -180.0, 140.0), (0.0, 0.0, 65.0), (0.0, 0.0, 1.0))
        return proj * look

    def on_render(self, time, frame_time):
        self.ctx.clear(1.0, 1.0, 1.0)
        self.ctx.enable(moderngl.DEPTH_TEST)

        self.program['Light'] = (-140.0, -300.0, 350.0)
        self.program['Color'] = (1.0, 1.0, 1.0, 0.25)
        self.program['Mvp'].write(self.camera_matrix())

        self.texture.use()
        self.vao.render()


if __name__ == '__main__':
    Example.run()
