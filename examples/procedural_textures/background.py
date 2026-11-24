import os

import moderngl
import pygame

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'

pygame.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, 1)
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()

size = pygame.display.get_window_size()
image = ctx.texture(size, 'rgba8unorm')

ctx.shaders['background.vert'] = '''
    #version 330 core

    vec2 positions[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2(3.0, -1.0),
        vec2(-1.0, 3.0)
    );

    void main() {
        gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
    }
'''

ctx.shaders['background.frag'] = '''
    #version 330 core

    layout (location = 0) out vec4 out_color;

    void main() {
        float gray = 0.7 + float((int(gl_FragCoord.x) / 40 + int(gl_FragCoord.y) / 40) % 2) * 0.2;
        out_color = vec4(gray, gray, gray, 1.0);
    }
'''

pipeline = ctx.pipeline(
    'background.vert',
    'background.frag',
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=ctx.framebuffer([image]),
)

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    ctx.start_frame()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
