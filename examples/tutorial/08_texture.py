import math
import os

import glm
import moderngl
import numpy as np
import pygame

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)


class NoiseTexture:
    def __init__(self, width, height):
        self.ctx = moderngl.get_context()

        pixels = os.urandom(width * height * 4)
        self.texture = self.ctx.texture((width, height), 'rgba8unorm', pixels)

        self.sampler = self.ctx.sampler(self.texture, min_filter='nearest', mag_filter='nearest')

    def use(self):
        self.sampler.use(0)


class TriangleGeometry:
    def __init__(self):
        self.ctx = moderngl.get_context()
        vertices = np.array([
            0.0, 0.4, 0.0,
            -0.4, -0.3, 0.0,
            0.4, -0.3, 0.0,
        ])

        self.vbo = self.ctx.buffer(vertices.astype('f4').tobytes())

    def vertex_array(self):
        return self.ctx.vertex_array([*self.vbo.bind('3f', 0)])


class PlaneGeometry:
    def __init__(self):
        self.ctx = moderngl.get_context()
        vertices = np.array([
            -0.5, -0.5, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
            0.5, -0.5, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0,
            -0.5, 0.5, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0,
            -0.5, 0.5, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0,
            0.5, -0.5, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0,
            0.5, 0.5, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0,
        ])

        self.vbo = self.ctx.buffer(vertices.astype('f4').tobytes())

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

        self.ctx.shaders['texture.vert'] = '''
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

        self.ctx.shaders['texture.frag'] = '''
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
            'texture.vert',
            'texture.frag',
            samplers={'Texture': 0},
            uniforms={'camera': 'mat4', 'position': 'vec3', 'scale': 'float', 'use_texture': 'bool', 'color': 'vec3'},
        )

        self.texture = NoiseTexture(32, 32)

        self.triangle_geometry = TriangleGeometry()
        self.triangle = Mesh(self.program, self.triangle_geometry)

        self.plane_geometry = PlaneGeometry()
        self.plane = Mesh(self.program, self.plane_geometry, self.texture)

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

        self.triangle.render((-0.2, 0.0, 0.0), (1.0, 0.0, 0.0), 0.2)
        self.plane.render((0.0, 0.0, 0.0), (1.0, 1.0, 1.0), 0.2)
        self.triangle.render((0.2, 0.0, 0.0), (0.0, 0.0, 1.0), 0.2)


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
