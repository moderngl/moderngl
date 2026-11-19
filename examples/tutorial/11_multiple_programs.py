import math
import os
import re
import struct

import glm
import moderngl
import pygame
from PIL import Image

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=False)


def load_obj(path):
    text = open(path).read()
    vertices = re.findall(r'^v\s+(\S+)\s+(\S+)\s+(\S+)', text, re.M)
    uvs = re.findall(r'^vt\s+(\S+)\s+(\S+)', text, re.M)
    normals = re.findall(r'^vn\s+(\S+)\s+(\S+)\s+(\S+)', text, re.M)
    faces = re.findall(r'^f\s+(\d+)/(\d+)/(\d+)\s+(\d+)/(\d+)/(\d+)\s+(\d+)/(\d+)/(\d+)', text, re.M)
    data = []
    for face in faces:
        for v, vt, vn in zip(face[0::3], face[1::3], face[2::3]):
            data += vertices[int(v) - 1] + normals[int(vn) - 1] + uvs[int(vt) - 1]
    return struct.pack(f'{len(data)}f', *map(float, data))


class ColorMaterial:
    def __init__(self):
        self.ctx = moderngl.get_context()
        self.ctx.shaders['color_material.vert'] = '''
            #version 330 core

            uniform mat4 camera;
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

            uniform vec3 light_direction;
            uniform vec3 color;

            in vec3 v_vertex;
            in vec3 v_normal;
            in vec2 v_uv;

            layout (location = 0) out vec4 out_color;

            void main() {
                out_color = vec4(color, 1.0);
                float lum = dot(normalize(v_normal), normalize(light_direction));
                out_color.rgb *= max(lum, 0.0) * 0.5 + 0.5;
            }
        '''

        self.program = self.ctx.program(
            'color_material.vert',
            'color_material.frag',
            uniforms={'camera': 'mat4', 'position': 'vec3', 'scale': 'float', 'light_direction': 'vec3', 'color': 'vec3'},
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

            uniform mat4 camera;
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

            uniform sampler2D Texture;
            uniform vec3 light_direction;

            in vec3 v_vertex;
            in vec3 v_normal;
            in vec2 v_uv;

            layout (location = 0) out vec4 out_color;

            void main() {
                out_color = texture(Texture, v_uv);
                float lum = dot(normalize(v_normal), normalize(light_direction));
                out_color.rgb *= max(lum, 0.0) * 0.5 + 0.5;
            }
        '''

        self.program = self.ctx.program(
            'texture_material.vert',
            'texture_material.frag',
            samplers={'Texture': 0},
            uniforms={'camera': 'mat4', 'position': 'vec3', 'scale': 'float', 'light_direction': 'vec3'},
        )

    def use(self):
        self.texture.use()

    def vertex_array(self, buffer):
        return self.ctx.vertex_array(buffer.bind('3f 3f 2f', 0, 1, 2))


class ImageTexture:
    def __init__(self, path):
        self.ctx = moderngl.get_context()

        img = Image.open(path).convert('RGBA')
        self.texture = self.ctx.texture(img.size, 'rgba8unorm', img.tobytes())
        self.sampler = self.ctx.sampler(self.texture)

    def use(self):
        self.sampler.use(0)


class ModelGeometry:
    def __init__(self, path):
        self.ctx = moderngl.get_context()

        self.vbo = self.ctx.buffer(load_obj(path))


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

        self.texture = ImageTexture('examples/tutorial/data/textures/crate.png')

        self.color_material = ColorMaterial()
        self.texture_material = TextureMaterial(self.texture)

        self.car_geometry = ModelGeometry('examples/tutorial/data/models/lowpoly_toy_car.obj')
        self.car = Mesh(self.color_material, self.car_geometry)

        self.crate_geometry = ModelGeometry('examples/tutorial/data/models/crate.obj')
        self.crate = Mesh(self.texture_material, self.crate_geometry)

    def camera_matrix(self):
        eye = (math.cos(yaw) * math.cos(pitch) * 1.118, math.sin(yaw) * math.cos(pitch) * 1.118, math.sin(pitch) * 1.118)
        proj = glm.perspective(math.radians(60.0), 1.777, 0.1, 1000.0)
        look = glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))
        return proj * look

    def render(self):
        camera = self.camera_matrix()

        self.ctx.screen.clear()
        self.ctx.depth_test = True

        self.color_material.program.use()
        self.color_material.program['camera'] = camera
        self.color_material.program['light_direction'] = (1.0, 2.0, 3.0)

        self.texture_material.program.use()
        self.texture_material.program['camera'] = camera
        self.texture_material.program['light_direction'] = (1.0, 2.0, 3.0)

        self.crate.render((0.0, 0.0, 0.0), 0.2)

        self.color_material.color = (1.0, 0.0, 0.0)
        self.car.render((-0.4, 0.0, 0.0), 0.2)

        self.color_material.color = (0.0, 0.0, 1.0)
        self.car.render((0.4, 0.0, 0.0), 0.2)


scene = Scene()

yaw, pitch, spin = 0.0, 0.46, 0.3

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
