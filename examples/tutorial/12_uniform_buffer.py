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

    def set_light_direction(self, x, y, z):
        self.data[64:80] = struct.pack('4f', x, y, z, 0.0)

    def use(self):
        self.ubo.write(self.data)
        self.ubo.use(0)


class ColorMaterial:
    def __init__(self):
        self.ctx = moderngl.get_context()
        self.ctx.shaders['color_material.vert'] = '''
            #version 330 core

            layout (std140) uniform Common {
                mat4 camera;
                vec4 light_direction;
            };

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

            layout (std140) uniform Common {
                mat4 camera;
                vec4 light_direction;
            };

            uniform vec3 color;

            in vec3 v_vertex;
            in vec3 v_normal;
            in vec2 v_uv;

            layout (location = 0) out vec4 out_color;

            void main() {
                out_color = vec4(color, 1.0);
                float lum = dot(normalize(v_normal), normalize(light_direction.xyz));
                out_color.rgb *= max(lum, 0.0) * 0.5 + 0.5;
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

            layout (std140) uniform Common {
                mat4 camera;
                vec4 light_direction;
            };

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

            layout (std140) uniform Common {
                mat4 camera;
                vec4 light_direction;
            };

            uniform sampler2D Texture;

            in vec3 v_vertex;
            in vec3 v_normal;
            in vec2 v_uv;

            layout (location = 0) out vec4 out_color;

            void main() {
                out_color = texture(Texture, v_uv);
                float lum = dot(normalize(v_normal), normalize(light_direction.xyz));
                out_color.rgb *= max(lum, 0.0) * 0.5 + 0.5;
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
        self.uniform_buffer.set_light_direction(1.0, 2.0, 3.0)
        self.uniform_buffer.use()

        self.crate.render((0.0, 0.0, 0.0), 0.2)

        self.color_material.color = (1.0, 0.0, 0.0)
        self.car.render((-0.4, 0.0, 0.0), 0.2)

        self.color_material.color = (0.0, 0.0, 1.0)
        self.car.render((0.4, 0.0, 0.0), 0.2)


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
