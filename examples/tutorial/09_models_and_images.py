import math
import os
import sys

import glm
import moderngl
import pygame

sys.path.insert(0, 'examples')
from glb import load_glb

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

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

    def vertex_array(self):
        return self.ctx.vertex_array([*self.vbo.bind('3f 12x 2f', 0, 2)])


class Mesh:
    def __init__(self, program, geometry, texture=None):
        self.ctx = moderngl.get_context()
        self.program = program
        self.vao = geometry.vertex_array()
        self.texture = texture

    def render(self, position, color, scale):
        self.program.use()
        self.program['use_texture'] = False

        if self.texture:
            self.program['use_texture'] = True
            self.texture.use()

        self.program['position'] = position
        self.program['color'] = color
        self.program['scale'] = scale
        self.vao.render()


class Scene:
    def __init__(self):
        self.ctx = moderngl.get_context()
        self.ctx.screen.clear_color = (0.05, 0.05, 0.05, 1.0)

        self.ctx.shaders['models_and_images.vert'] = '''
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

        self.ctx.shaders['models_and_images.frag'] = '''
            #version 330 core

            uniform sampler2D Texture;
            uniform bool use_texture;
            uniform vec3 color;

            in vec3 v_vertex;
            in vec3 v_normal;
            in vec2 v_uv;

            layout (location = 0) out vec4 out_color;

            void main() {
                out_color = vec4(color, 1.0);
                if (use_texture) {
                    out_color *= texture(Texture, v_uv);
                }
            }
        '''

        self.program = self.ctx.program(
            'models_and_images.vert',
            'models_and_images.frag',
            samplers={'Texture': 0},
            uniforms={'camera': 'mat4', 'position': 'vec3', 'scale': 'float', 'use_texture': 'bool', 'color': 'vec3'},
        )

        crate = load_glb('examples/data/crate/crate.glb')
        car = load_glb('examples/data/cars/cars.glb')

        self.texture = ImageTexture(crate['textures']['crate'])

        self.car_geometry = ModelGeometry(car['models']['car'])
        self.car = Mesh(self.program, self.car_geometry)

        self.crate_geometry = ModelGeometry(crate['models']['crate'])
        self.crate = Mesh(self.program, self.crate_geometry, self.texture)

    def camera_matrix(self):
        eye = (math.cos(yaw) * math.cos(pitch) * 1.118, math.sin(yaw) * math.cos(pitch) * 1.118, math.sin(pitch) * 1.118)
        width, height = pygame.display.get_window_size()
        proj = glm.perspective(math.radians(60.0), width / height, 0.1, 1000.0)
        look = glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))
        return proj * look

    def render(self):
        self.program.use()
        camera = self.camera_matrix()

        self.ctx.screen.clear()
        self.ctx.depth_test = True

        self.program['camera'] = camera

        self.car.render((-0.4, 0.0, 0.0), (1.0, 0.0, 0.0), 0.2)
        self.crate.render((0.0, 0.0, 0.0), (1.0, 1.0, 1.0), 0.2)
        self.car.render((0.4, 0.0, 0.0), (0.0, 0.0, 1.0), 0.2)


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
