
import extras
import moderngl
import numpy as np
import pygame

extras.init()

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

window_size = pygame.display.get_window_size()

ctx = moderngl.get_context()

image = ctx.texture(window_size, 'rgba8unorm', samples=4)
framebuffer = ctx.framebuffer([image])
framebuffer.clear_color = (0.0, 0.0, 0.0, 1.0)

ctx.shaders['vertex_attributes'] = '''
    layout (location = 0) in vec2 in_vert;
    layout (location = 1) in vec3 in_color;
'''

vertex_buffer = ctx.buffer(np.array([
    0.0, 0.8, 1.0, 0.0, 0.0,
    -0.6, -0.8, 0.0, 1.0, 0.0,
    0.6, -0.8, 0.0, 0.0, 1.0,
], 'f4'))

ctx.shaders['shader_include.vert'] = '''
    #version 330 core

    #include "vertex_attributes"

    out vec3 v_color;

    void main() {
        gl_Position = vec4(in_vert, 0.0, 1.0);
        v_color = in_color;
    }
'''

ctx.shaders['shader_include.frag'] = '''
    #version 330 core

    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(v_color, 1.0);
    }
'''

triangle = ctx.pipeline(
    'shader_include.vert',
    'shader_include.frag',
    vertex_array=ctx.vertex_array(vertex_buffer.bind('2f 3f', 0, 1)),
    framebuffer=framebuffer,
)

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    ctx.start_frame()
    framebuffer.clear()
    triangle.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
