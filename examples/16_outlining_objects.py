import math
import os
import struct
import sys

from pyglm import glm
import moderngl
import pygame
from PIL import Image
from objloader import Obj

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'

pygame.init()
pygame.display.set_mode((800, 800), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)


def init_includes():
    ctx = moderngl.get_context()
    print(f"{ctx.version_code=}")

    ctx.includes['uniform_buffer'] = '''
        struct Light {
            vec4 light_position;
            vec4 light_color;
            float light_power;
        };

        layout (std140) uniform Common {
            mat4 camera;
            vec4 camera_position;
            Light lights[2];
        };
    '''

    ctx.includes['blinn_phong'] = '''
        vec3 blinn_phong(
                vec3 vertex, vec3 normal, vec3 camera_position, vec3 light_position, float shininess, vec3 ambient_color,
                vec3 diffuse_color, vec3 light_color, vec3 spec_color, float light_power) {

            vec3 light_dir = light_position - vertex;
            float light_distance = length(light_dir);
            light_distance = light_distance * light_distance;
            light_dir = normalize(light_dir);

            float lambertian = max(dot(light_dir, normal), 0.0);
            float specular = 0.0;

            if (lambertian > 0.0) {
                vec3 view_dir = normalize(camera_position - vertex);
                vec3 half_dir = normalize(light_dir + view_dir);
                float spec_angle = max(dot(half_dir, normal), 0.0);
                specular = pow(spec_angle, shininess);
            }

            vec3 color_linear = ambient_color +
                diffuse_color * lambertian * light_color * light_power / light_distance +
                spec_color * specular * light_color * light_power / light_distance;

            return color_linear;
        }
    '''

    ctx.includes['calculate_lights'] = '''
        vec3 calculate_lights(vec3 vertex, vec3 normal, vec3 color, vec3 camera_position) {
            vec3 result = vec3(0.0);
            for (int i = 0; i < 2; ++i) {
                result += blinn_phong(
                    vertex, normal, camera_position, lights[i].light_position.xyz, 16.0, color * 0.05,
                    color, lights[i].light_color.rgb, vec3(1.0, 1.0, 1.0), lights[i].light_power
                );
            }
            return result;
        }
    '''

    ctx.includes['srgb'] = '''
        vec3 srgb_to_linear(vec3 color) {
            return pow(color, vec3(2.2));
        }
        vec3 linear_to_srgb(vec3 color) {
            return pow(color, vec3(1.0 / 2.2));
        }
    '''


class RenderScreen:
    def __init__(self, texture):
        self.ctx = moderngl.get_context()
        self.program = self.ctx.program(
            vertex_shader='''
                #version 330 core

                vec2 positions[3] = vec2[](
                    vec2(-1.0, -1.0),
                    vec2(3.0, -1.0),
                    vec2(-1.0, 3.0)
                );

                void main() {
                    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330 core

                uniform sampler2D Texture;

                layout (location = 0) out vec4 out_color;

                void main() {
                    ivec2 at = ivec2(gl_FragCoord.xy);
                    out_color = texelFetch(Texture, at, 0);
                }
            ''',
        )
        self.sampler = self.ctx.sampler(texture=texture)
        self.vao = self.ctx.vertex_array(self.program, [])
        self.vao.vertices = 3

    def render(self, now):
        self.ctx.enable_only(self.ctx.NOTHING)
        self.sampler.use()
        self.vao.render()


class UniformBuffer:
    def __init__(self):
        self.ctx = moderngl.get_context()
        self.data = bytearray(1024)
        self.ubo = self.ctx.buffer(self.data)

    def set_camera(self, eye, target):
        proj = glm.perspective(45.0, 1.0, 0.1, 1000.0)
        look = glm.lookAt(eye, target, (0.0, 0.0, 1.0))
        camera = proj * look
        self.data[0:64] = camera.to_bytes()
        self.data[64:80] = struct.pack('4f', *eye, 0.0)

    def set_light(self, light_index, position, color, power):
        offset = 80 + light_index * 48
        self.data[offset + 0: offset + 16] = struct.pack('4f', *position, 0.0)
        self.data[offset + 16: offset + 32] = struct.pack('4f', *color, 0.0)
        self.data[offset + 32: offset + 36] = struct.pack('f', power)

    def use(self):
        self.ubo.write(self.data)
        self.ubo.bind_to_uniform_block()


class ColorMaterial:
    def __init__(self):
        self.ctx = moderngl.get_context()
        self.program = self.ctx.program(
            vertex_shader='''
                #version 330 core
                #include "uniform_buffer"

                uniform vec3 position;
                uniform float scale;

                layout (location = 0) in vec3 in_vertex;
                layout (location = 1) in vec3 in_normal;
                layout (location = 2) in vec2 in_uv;

                out vec3 v_vertex;
                out vec3 v_normal;
                out vec2 v_uv;

                void main() {
                    v_vertex = position + in_vertex * scale;
                    v_normal = in_normal;
                    v_uv = in_uv;

                    gl_Position = camera * vec4(v_vertex, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330 core
                #include "uniform_buffer"
                #include "blinn_phong"
                #include "calculate_lights"
                #include "srgb"

                uniform vec3 color;

                in vec3 v_vertex;
                in vec3 v_normal;
                in vec2 v_uv;

                layout (location = 0) out vec4 out_color;

                void main() {
                    vec3 color_linear = calculate_lights(v_vertex, v_normal, srgb_to_linear(color), camera_position.xyz);
                    out_color = vec4(linear_to_srgb(color_linear), 1.0);
                }
            ''',
        )

        self.color = (1.0, 1.0, 1.0)

    def use(self):
        self.program['color'] = self.color

    def vertex_array(self, buffer):
        return self.ctx.vertex_array(self.program, [(buffer, '3f 3f 8x', 'in_vertex', 'in_normal')])


class OutlineMaterial:
    def __init__(self):
        self.ctx = moderngl.get_context()
        self.program = self.ctx.program(
            vertex_shader='''
                #version 330 core
                #include "uniform_buffer"

                uniform vec3 position;
                uniform float scale;

                layout (location = 0) in vec3 in_vertex;
                layout (location = 1) in vec3 in_normal;
                layout (location = 2) in vec2 in_uv;

                out vec3 v_vertex;
                out vec3 v_normal;
                out vec2 v_uv;

                void main() {
                    v_vertex = position + in_vertex * scale;
                    v_normal = in_normal;
                    v_uv = in_uv;

                    gl_Position = camera * vec4(v_vertex, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330 core
                #include "uniform_buffer"
                
                uniform vec3 color;

                in vec3 v_vertex;
                in vec3 v_normal;
                in vec2 v_uv;

                layout (location = 0) out vec4 out_color;

                void main() {                    
                    out_color = vec4(color, max(1.0, v_vertex.z + v_normal.z));
                }
            ''',
        )

        self.color = (1.0, 1.0, 1.0)

    def use(self):
        self.program['color'] = self.color

    def vertex_array(self, buffer):
        return self.ctx.vertex_array(self.program, [(buffer, '3f 3f 8x', 'in_vertex', 'in_normal')])


class TextureMaterial:
    def __init__(self, texture):
        self.texture = texture
        self.ctx = moderngl.get_context()
        self.program = self.ctx.program(
            vertex_shader='''
                #version 330 core
                #include "uniform_buffer"

                uniform vec3 position;
                uniform float scale;

                layout (location = 0) in vec3 in_vertex;
                layout (location = 1) in vec3 in_normal;
                layout (location = 2) in vec2 in_uv;

                out vec3 v_vertex;
                out vec3 v_normal;
                out vec2 v_uv;

                void main() {
                    v_vertex = position + in_vertex * scale;
                    v_normal = in_normal;
                    v_uv = in_uv;

                    gl_Position = camera * vec4(v_vertex, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330 core
                #include "uniform_buffer"
                #include "blinn_phong"
                #include "calculate_lights"
                #include "srgb"

                uniform sampler2D Texture;

                in vec3 v_vertex;
                in vec3 v_normal;
                in vec2 v_uv;

                layout (location = 0) out vec4 out_color;

                void main() {
                    vec3 color = texture(Texture, v_uv).rgb;
                    vec3 color_linear = calculate_lights(v_vertex, v_normal, srgb_to_linear(color), camera_position.xyz);
                    out_color = vec4(linear_to_srgb(color_linear), 1.0);
                }
            ''',
        )

    def use(self):
        self.texture.use()

    def vertex_array(self, buffer):
        return self.ctx.vertex_array(self.program, [(buffer, '3f 3f 2f', 'in_vertex', 'in_normal', 'in_uv')])


class ImageTexture:
    def __init__(self, path):
        self.ctx = moderngl.get_context()

        img = Image.open(path).convert('RGBA')
        self.texture = self.ctx.texture(img.size, 4, img.tobytes())
        self.sampler = self.ctx.sampler(texture=self.texture)

    def use(self):
        self.sampler.use()


class ModelGeometry:
    def __init__(self, path):
        self.ctx = moderngl.get_context()

        obj = Obj.open(path)
        self.vbo = self.ctx.buffer(obj.pack('vx vy vz nx ny nz tx ty'))


class Mesh:
    def __init__(self, material, geometry):
        self.ctx = moderngl.get_context()
        self.vao = material.vertex_array(geometry.vbo)
        self.material = material

    def render(self, position, scale):
        self.material.use()
        self.vao.program['position'] = position
        self.vao.program['scale'] = scale
        self.vao.render()


class Scene:
    def __init__(self):
        self.ctx = moderngl.get_context()

        size = pygame.display.get_window_size()
        self.screen = self.ctx.texture(size, 4)
        self.depth = self.ctx.depth_texture(size, use_stencilbuffer=True)
        self.framebuffer = self.ctx.framebuffer(color_attachments=[self.screen], depth_attachment=self.depth)

        self.uniform_buffer = UniformBuffer()
        self.texture = ImageTexture('examples/data/textures/crate.png')

        self.color_material = ColorMaterial()
        self.outline_material = OutlineMaterial()
        self.texture_material = TextureMaterial(self.texture)

        self.car_geometry = ModelGeometry('examples/data/models/lowpoly_toy_car.obj')
        self.car = Mesh(self.color_material, self.car_geometry)
        self.outline_car = Mesh(self.outline_material, self.car_geometry)

        self.crate_geometry = ModelGeometry('examples/data/models/crate.obj')
        self.crate = Mesh(self.texture_material, self.crate_geometry)
        self.outline_crate = Mesh(self.outline_material, self.crate_geometry)

        self.render_screen = RenderScreen(self.screen)

    def render(self):
        now = pygame.time.get_ticks() / 1000.0

        eye = (math.cos(now), math.sin(now), 0.5)

        self.framebuffer.use()
        self.ctx.clear()

        self.uniform_buffer.set_camera(eye, (0.0, 0.0, 0.0))
        self.uniform_buffer.set_light(
            light_index=0,
            position=(1.0, 2.0, 3.0),
            color=(1.0, 1.0, 1.0),
            power=10.0,
        )
        self.uniform_buffer.set_light(
            light_index=1,
            position=(-2.0, -1.0, 4.0),
            color=(1.0, 1.0, 1.0),
            power=10.0,
        )
        self.uniform_buffer.use()

        # 1. Enable stencil writing
        self.ctx.enable(self.ctx.DEPTH_TEST | self.ctx.STENCIL_TEST | moderngl.CULL_FACE)
        self.ctx.cull_face = "back"
        self.ctx.stencil_mask = 0xFF
        # 2.a Set the stencil func to GL_ALWAYS before drawing the (to be outlined) objects
        self.ctx.stencil_func = ("1", 1, 0xFF)
        # 2.b updating the stencil buffer with 1s wherever the objects' fragments are rendered.
        self.ctx.stencil_op = (moderngl.KEEP, moderngl.KEEP, moderngl.REPLACE)
        # 3. Render the objects.
        self.crate.render((0.0, 0.0, 0.0), 0.2)
        self.color_material.color = (1.0, 0.0, 0.0)
        self.car.render((-0.4, 0.0, 0.0), 0.2)
        self.color_material.color = (0.0, 0.0, 1.0)
        self.car.render((0.4, 0.0, 0.0), 0.2)

        # 4.a Disable stencil writing
        self.ctx.stencil_mask = 0x00
        # 4.b Disable depth testing
        self.ctx.disable(self.ctx.DEPTH_TEST)
        # 5. Scale each of the objects by a small amount.
        # 6. Use a different fragment shader that outputs a single (border) color.
        # 7. Draw the objects again, but only if their fragments' stencil values are not equal to 1.
        self.ctx.stencil_func = ("!=", 1, 0xFF)
        # render scene
        self.outline_material.color = (0, 1, 0)
        scale = 0.075
        self.outline_crate.render((0.0, 0.0, 0.0), 0.2 * (1 + scale))
        self.outline_car.render((-0.4, 0.0, 0.0), 0.2 * (1 + scale))
        self.outline_car.render((0.4, 0.0, 0.0), 0.2 * (1 + scale))
        # 8.a Enable depth testing again
        self.ctx.enable(self.ctx.DEPTH_TEST)
        # 8.b restore stencil func to GL_KEEP.
        # self.ctx.stencil_mask = 0xFF
        # self.ctx.stencil_func = ("1", 1, 0xFF)
        # self.ctx.stencil_op = (moderngl.KEEP, moderngl.KEEP, moderngl.KEEP)
        self.ctx.disable(moderngl.STENCIL_TEST)

        # Post-processing rendering
        self.ctx.screen.use()
        self.render_screen.render(now)


if __name__ == "__main__":
    init_includes()
    scene = Scene()

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()

        scene.render()

        pygame.display.flip()
