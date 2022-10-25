"""
Links:
- http://www.opengl-tutorial.org/fr/intermediate-tutorials/tutorial-16-shadow-mapping/#shadowmap-basique
- https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
- https://learnopengl.com/Advanced-OpenGL/Depth-testing
"""
from typing import Final, Dict

import numpy as np
from moderngl_window.scene import Scene
from pyrr import Matrix44
from pyrr import Vector3

import moderngl
#
from ported._example import Example

SHADOW_SIZE: Final[int] = 2 << 7  # 512²


class ShadowMappingSample(Example):
    title = "ShadowMapping"
    window_size = (1280, 720)
    gl_version = (3, 3)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog_render_scene_with_sm = self.ctx.program(
            vertex_shader='''
                #version 330

                uniform mat4 u_mvp;
                uniform mat4 u_depth_bias_mvp;

                in vec3 in_position;
                in vec3 in_normal;
                in vec2 in_texcoord_0;

                out vec3 v_vert;
                out vec3 v_norm;
                out vec2 v_text;

                out vec4 v_shadow_coord;

                void main() {
                    gl_Position = u_mvp * vec4(in_position, 1.0);

                    v_shadow_coord = u_depth_bias_mvp * vec4(in_position, 1.0);

                    v_vert = in_position;
                    v_norm = in_normal;
                    v_text = in_texcoord_0;
                }
            ''',
            fragment_shader='''
                #version 330

                uniform vec3 u_light;
                uniform vec3 u_color;

                uniform sampler2DShadow u_sampler_shadow;
                
                uniform bool u_use_color_texture;
                uniform sampler2D u_sampler_billboard;

                in vec3 v_vert;
                in vec3 v_norm;
                in vec2 v_text;

                in vec4 v_shadow_coord;

                out vec4 f_color;
                
                const float LIGHT_AMBIENT = 0.20;

                float compute_visibility(in float cos_theta) {
                    // shadow coordinates in light space
                    vec2 shadow_coord_ls = v_shadow_coord.xy / v_shadow_coord.w;
                    
                    // bias according to the slope
                    float bias = 0.005 * tan(acos(cos_theta));
                    bias = clamp(bias, 0, 0.01);

                    float z_from_cam = v_shadow_coord.z / v_shadow_coord.w - bias;
                    vec3 shadow_coord = vec3(shadow_coord_ls, z_from_cam);
                    float shadow_value = texture(u_sampler_shadow, shadow_coord);
                    return 1.0 - shadow_value;
                }
                                
                void main() {
                    // Lighting
                    // Diffuse lighting + ambient
                    vec3 light_vector_obj_space = normalize(u_light - v_vert);
                    vec3 normal_obj_space = normalize(v_norm);
                    float cos_theta = dot(light_vector_obj_space, normal_obj_space);
                    float diffuse = clamp(cos_theta, 0.0, 1.0);
                    // Shadow component                
                    float lum = mix(LIGHT_AMBIENT, 1.0, diffuse * compute_visibility(cos_theta));
                    // Color object (color or texture)                    
                    vec4 object_color = u_use_color_texture ? vec4(texture(u_sampler_billboard, v_text).rgb, 1.0) : vec4(u_color, 1.0);
                    // Final pixel color
                    f_color = object_color * lum;
                }
            ''',
        )

        self.prog_depth = self.ctx.program(
            vertex_shader='''
                #version 330

                uniform mat4 u_mvp;

                in vec3 in_position;

                void main() {
                    gl_Position = u_mvp * vec4(in_position, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330
                void main() {
                }
            ''',
        )
        # textures unit
        self.prog_render_scene_with_sm['u_sampler_shadow'].value = 0
        self.prog_render_scene_with_sm['u_sampler_billboard'].value = 1
        # uniforms
        self.mvp = self.prog_render_scene_with_sm['u_mvp']
        self.mvp_depth = self.prog_render_scene_with_sm['u_depth_bias_mvp']
        self.light = self.prog_render_scene_with_sm['u_light']
        self.color = self.prog_render_scene_with_sm['u_color']
        self.use_color_texture = self.prog_render_scene_with_sm['u_use_color_texture']
        self.mvp_shadow = self.prog_depth['u_mvp']

        self.objects: Dict[str, moderngl.VertexArray] = {}
        self.objects_shadow: Dict[str, moderngl.VertexArray] = {}

        for name in {'ground', 'grass', 'billboard', 'billboard-holder', 'billboard-image'}:
            scene: Scene = self.load_scene(f'scene-1-{name}.obj')
            vao = scene.root_nodes[0].mesh.vao
            self.objects[name] = vao.instance(self.prog_render_scene_with_sm)
            self.objects_shadow[name] = vao.instance(self.prog_depth)

        # texture on billboard
        self.texture_on_billboard = self.load_texture_2d('infographic-1.jpg')
        self.texture_on_billboard.use(location=1)

        shadow_size = (SHADOW_SIZE, SHADOW_SIZE,)

        self.tex_depth = self.ctx.depth_texture(shadow_size)
        self.tex_color_depth = self.ctx.texture(shadow_size, components=1, dtype='f4')
        self.fbo_depth = self.ctx.framebuffer(
            color_attachments=[self.tex_color_depth],
            depth_attachment=self.tex_depth
        )
        self.sampler_depth = self.ctx.sampler(
            # bi-linear interpolation on depth fetch -> Percentage Close Filtering (2x2)
            filter=(moderngl.LINEAR, moderngl.LINEAR),
            compare_func='>=',  # enable depth func
            repeat_x=False, repeat_y=False,
        )

        self.ctx.enable(moderngl.CULL_FACE)

    def render(self, time: float, _frame_time: float):
        # pass 0: clear buffers
        self.ctx.clear(1.0, 1.0, 1.0)
        self.ctx.enable(moderngl.DEPTH_TEST)

        cam_proj = Matrix44.perspective_projection(45.0, self.aspect_ratio, 0.1, 1000.0)
        cam_look_at = Matrix44.look_at(
            (47.697, -8.147, 24.498),
            (0.0, 0.0, 8.0),
            (0.0, 0.0, 1.0),
        )
        cam_rotate = Matrix44.from_z_rotation(np.sin(time) * 0.5 + 0.2)
        cam_mvp = cam_proj * cam_look_at * cam_rotate
        self.mvp.write(cam_mvp.astype('f4').tobytes())

        # build light camera
        light_rotate = Matrix44.from_z_rotation(time)
        light_pos = light_rotate * Vector3((-60.69, -40.14, 52.49))
        self.light.value = tuple(light_pos)
        light_look_at = Matrix44.look_at(
            light_pos,
            target=(0, 0, 0),
            up=(0.0, 0.0, 1.0),
        )
        # light projection matrix (scene dependent)
        light_proj = Matrix44.perspective_projection(
            fovy=90.0 / 4,  # smaller value increase shadow precision
            aspect=self.wnd.aspect_ratio,
            near=60.0,
            far=100.0
        )
        # light model view projection matrix
        mvp_light = light_proj * light_look_at
        bias_matrix = (
                Matrix44.from_translation((0.5, 0.5, 0.5), dtype='f4') *
                Matrix44.from_scale((0.5, 0.5, 0.5), dtype='f4')
        )
        mvp_depth_bias = bias_matrix * mvp_light
        # send uniforms to shaders
        self.mvp_depth.write(mvp_depth_bias.astype('f4').tobytes())
        self.mvp_shadow.write(mvp_light.astype('f4').tobytes())

        # pass 1: render shadow-map (depth framebuffer -> texture) from light view
        self.fbo_depth.use()
        self.fbo_depth.clear(1.0, 1.0, 1.0)
        # https://moderngl.readthedocs.io/en/stable/reference/context.html?highlight=culling#moderngl.Context.front_face
        # clock wise -> render back faces
        self.ctx.front_face = 'cw'
        for vao_shadow in self.objects_shadow.values():
            vao_shadow.render()

        # pass 2: render the scene and retro project depth shadow-map
        # counter clock wise -> render front faces
        self.ctx.front_face = 'ccw'
        self.use_color_texture.value = False

        self.ctx.screen.use()
        self.sampler_depth.use(location=0)
        self.tex_depth.use(location=0)

        # pass 2a: render colored scene with shadow
        for object_name, object_color in (
                ("ground", (0.67, 0.49, 0.29)),
                ("grass", (0.46, 0.67, 0.29)),
                ("billboard", (1.0, 1.0, 1.0)),
                ("billboard-holder", (0.2, 0.2, 0.2)),
        ):
            self.color.value = object_color
            self.objects[object_name].render()
        # pass 2b: render textured scene with shadow
        self.use_color_texture.value = True
        self.objects['billboard-image'].render()


if __name__ == '__main__':
    ShadowMappingSample.run()
