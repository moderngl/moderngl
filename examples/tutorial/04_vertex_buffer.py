import math
import os

import glm
import moderngl
import numpy as np
import pygame

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=False)


class Scene:
    def __init__(self):
        self.ctx = moderngl.get_context()

        self.ctx.shaders['vertex_buffer.vert'] = '''
            #version 330 core

            uniform mat4 camera;

            layout (location = 0) in vec3 in_vertex;

            void main() {
                gl_Position = camera * vec4(in_vertex, 1.0);
            }
        '''

        self.ctx.shaders['vertex_buffer.frag'] = '''
            #version 330 core

            layout (location = 0) out vec4 out_color;

            void main() {
                out_color = vec4(1.0, 1.0, 1.0, 1.0);
            }
        '''

        self.program = self.ctx.program(
            'vertex_buffer.vert',
            'vertex_buffer.frag',
            uniforms={'camera': 'mat4'},
        )

        vertices = np.array([
            0.0, 0.4, 0.0,
            -0.4, -0.3, 0.0,
            0.4, -0.3, 0.0,
        ])

        self.vbo = self.ctx.buffer(vertices.astype('f4').tobytes())
        self.vao = self.ctx.vertex_array([*self.vbo.bind('3f', 0)])

    def camera_matrix(self):
        eye = (math.cos(yaw) * math.cos(pitch) * 1.118, math.sin(yaw) * math.cos(pitch) * 1.118, math.sin(pitch) * 1.118)
        proj = glm.perspective(math.radians(60.0), 1.777, 0.1, 1000.0)
        look = glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))
        return proj * look

    def render(self):
        self.program.use()
        camera = self.camera_matrix()

        self.ctx.screen.clear()
        self.ctx.depth_test = True
        self.program['camera'] = camera
        self.vao.render()


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
