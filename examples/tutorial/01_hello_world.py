import math
import os

import moderngl
import pygame

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=False)


class Scene:
    def __init__(self):
        self.ctx = moderngl.get_context()

    def render(self):
        now = pygame.time.get_ticks() / 1000.0

        r = math.sin(now + 0.0) * 0.5 + 0.5
        g = math.sin(now + 2.1) * 0.5 + 0.5
        b = math.sin(now + 4.2) * 0.5 + 0.5
        self.ctx.screen.clear_color = (r, g, b, 1.0)
        self.ctx.screen.clear()


scene = Scene()

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    scene.render()

    pygame.display.flip()

pygame.quit()
