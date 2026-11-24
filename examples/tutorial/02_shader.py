import os

import moderngl
import pygame

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)


class Scene:
    def __init__(self):
        self.ctx = moderngl.get_context()
        self.ctx.screen.clear_color = (0.05, 0.05, 0.05, 1.0)

        self.ctx.shaders['shader.vert'] = '''
            #version 330 core

            vec3 vertices[3] = vec3[](
                vec3(0.0, 0.4, 0.0),
                vec3(-0.225, -0.3, 0.0),
                vec3(0.225, -0.3, 0.0)
            );

            void main() {
                gl_Position = vec4(vertices[gl_VertexID], 1.0);
            }
        '''

        self.ctx.shaders['shader.frag'] = '''
            #version 330 core

            layout (location = 0) out vec4 out_color;

            void main() {
                out_color = vec4(1.0, 1.0, 1.0, 1.0);
            }
        '''

        self.program = self.ctx.program('shader.vert', 'shader.frag')

        self.vao = self.ctx.vertex_array([])
        self.vao.vertices = 3

    def render(self):
        self.program.use()
        self.ctx.screen.clear()
        self.vao.render()


scene = Scene()

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    scene.render()

    pygame.display.flip()

pygame.quit()
