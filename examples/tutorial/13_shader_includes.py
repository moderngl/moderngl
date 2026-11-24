import math
import os
import struct
import sys

import glm
import moderngl
import pygame

sys.path.insert(0, 'examples')
from glb import load_glb

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

def init_includes():
    ctx = moderngl.get_context()

    ctx.shaders['uniform_buffer'] = '''
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

    ctx.shaders['blinn_phong'] = '''
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

    ctx.shaders['calculate_lights'] = '''
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

    ctx.shaders['srgb'] = '''
    vec3 srgb_to_linear(vec3 color) {
        return pow(color, vec3(2.2));
    }
    vec3 linear_to_srgb(vec3 color) {
        return pow(color, vec3(1.0 / 2.2));
    }
    '''


class UniformBuffer:
    def __init__(self):
        self.ctx = moderngl.get_context()
        self.data = bytearray(1024)
        self.ubo = self.ctx.buffer(self.data)

    def set_camera(self, eye, target):
        width, height = pygame.display.get_window_size()
        proj = glm.perspective(math.radians(60.0), width / height, 0.1, 1000.0)
        look = glm.lookAt(eye, target, (0.0, 0.0, 1.0))
        camera = proj * look
        self.data[0:64] = camera.to_bytes()
        self.data[64:80] = struct.pack('4f', *eye, 0.0)

    def set_light(self, light_index, position, color, power):
        offset = 80 + light_index * 48
        self.data[offset + 0 : offset + 16] = struct.pack('4f', *position, 0.0)
        self.data[offset + 16 : offset + 32] = struct.pack('4f', *color, 0.0)
        self.data[offset + 32 : offset + 36] = struct.pack('f', power)

    def use(self):
        self.ubo.write(self.data)
        self.ubo.use(0)


class ColorMaterial:
    def __init__(self):
        self.ctx = moderngl.get_context()
        self.ctx.shaders['color_material.vert'] = '''
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
        '''

        self.ctx.shaders['color_material.frag'] = '''
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
        '''

        self.program = self.ctx.program(
            'color_material.vert',
            'color_material.frag',
            uniform_buffers={'Common': 0},
            uniforms={'position': 'vec3', 'scale': 'float', 'color': 'vec3'},
        )

        self.color = (1.0, 1.0, 1.0)

    def use(self):
        self.program['color'] = self.color

    def vertex_array(self, buffer):
        return self.ctx.vertex_array(buffer.bind('3f 3f 8x', 0, 1))


class TextureMaterial:
    def __init__(self, texture):
        self.texture = texture
        self.ctx = moderngl.get_context()
        self.ctx.shaders['texture_material.vert'] = '''
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
        '''

        self.ctx.shaders['texture_material.frag'] = '''
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
        '''

        self.program = self.ctx.program(
            'texture_material.vert',
            'texture_material.frag',
            samplers={'Texture': 0},
            uniform_buffers={'Common': 0},
            uniforms={'position': 'vec3', 'scale': 'float'},
        )

    def use(self):
        self.texture.use()

    def vertex_array(self, buffer):
        return self.ctx.vertex_array(buffer.bind('3f 3f 2f', 0, 1, 2))


class ImageTexture:
    def __init__(self, img):
        self.ctx = moderngl.get_context()

        img = img.convert('RGBA')
        self.texture = self.ctx.texture(img.size, 'rgba8unorm', img.tobytes())
        self.sampler = self.ctx.sampler(self.texture)

    def use(self):
        self.sampler.use(0)


class ModelGeometry:
    def __init__(self, data):
        self.ctx = moderngl.get_context()

        self.vbo = self.ctx.buffer(data)


class Mesh:
    def __init__(self, material, geometry):
        self.ctx = moderngl.get_context()
        self.vao = material.vertex_array(geometry.vbo)
        self.material = material

    def render(self, position, scale):
        self.material.program.use()
        self.material.use()
        self.material.program['position'] = position
        self.material.program['scale'] = scale
        self.vao.render()


class Scene:
    def __init__(self):
        self.ctx = moderngl.get_context()
        self.ctx.screen.clear_color = (0.05, 0.05, 0.05, 1.0)

        self.uniform_buffer = UniformBuffer()
        crate = load_glb('examples/data/crate/crate.glb')
        car = load_glb('examples/data/cars/cars.glb')

        self.texture = ImageTexture(crate['textures']['crate'])

        self.color_material = ColorMaterial()
        self.texture_material = TextureMaterial(self.texture)

        self.car_geometry = ModelGeometry(car['models']['car'])
        self.car = Mesh(self.color_material, self.car_geometry)

        self.crate_geometry = ModelGeometry(crate['models']['crate'])
        self.crate = Mesh(self.texture_material, self.crate_geometry)

    def render(self):

        eye = (math.cos(yaw) * math.cos(pitch) * 1.118, math.sin(yaw) * math.cos(pitch) * 1.118, math.sin(pitch) * 1.118)

        self.ctx.screen.clear()
        self.ctx.depth_test = True

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

        self.crate.render((0.0, 0.0, 0.0), 0.2)

        self.color_material.color = (1.0, 0.0, 0.0)
        self.car.render((-0.4, 0.0, 0.0), 0.2)

        self.color_material.color = (0.0, 0.0, 1.0)
        self.car.render((0.4, 0.0, 0.0), 0.2)


init_includes()
scene = Scene()

yaw, pitch, spin = 0.0, 0.46, 0.2

running = True
while running:
    if spin:
        yaw = pygame.time.get_ticks() / 1000.0 * spin
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.MOUSEMOTION and event.buttons[0]:
            spin = 0.0
            yaw -= event.rel[0] * 0.004
            pitch = min(max(pitch + event.rel[1] * 0.004, 0.05), 1.4)

    scene.render()

    pygame.display.flip()

pygame.quit()
