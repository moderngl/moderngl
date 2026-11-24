import os

import moderngl
import pygame

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'
pygame.display.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, 1)

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()

size = pygame.display.get_window_size()
image = ctx.texture(size, 'rgba8unorm', multisampled=True)
framebuffer = ctx.framebuffer([image])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

ctx.shaders['viewports.vert'] = '''
    #version 330 core

    out vec3 v_color;

    vec2 positions[3] = vec2[](
        vec2(0.0, 0.8),
        vec2(-0.6, -0.8),
        vec2(0.6, -0.8)
    );

    vec3 colors[3] = vec3[](
        vec3(1.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 0.0, 1.0)
    );

    void main() {
        gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
        v_color = colors[gl_VertexID];
    }
'''

ctx.shaders['viewports.frag'] = '''
    #version 330 core

    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(pow(v_color, vec3(1.0 / 2.2)), 1.0);
    }
'''

triangle = ctx.pipeline(
    'viewports.vert',
    'viewports.frag',
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=framebuffer,
)

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    ctx.start_frame()
    framebuffer.clear()
    triangle.viewport = (0, 0, 640, 360)
    triangle.run()
    triangle.viewport = (0, 360, 640, 360)
    triangle.run()
    triangle.viewport = (640, 0, 640, 360)
    triangle.run()
    triangle.viewport = (640, 360, 640, 360)
    triangle.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
