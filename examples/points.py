import os
from colorsys import hls_to_rgb

import moderngl
import numpy as np
import pygame

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'
pygame.display.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, 1)

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

size = pygame.display.get_window_size()

ctx = moderngl.get_context()

image = ctx.texture(size, 'rgba8unorm', multisampled=True)
framebuffer = ctx.framebuffer([image])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

count = 12500
emit = 50
position = np.zeros((count, 2), 'f4')
velocity = np.zeros((count, 2), 'f4')
position[:, 1] = -1.0
sweep = 0

vertex_buffer = ctx.buffer(size=count * 8, usage='stream_draw')
color_buffer = ctx.buffer(np.array([
    hls_to_rgb(np.random.uniform(0.0, 1.0), 0.6, 0.8)
    for _ in range(count)
]).astype('f4').tobytes())

ctx.shaders['points.vert'] = '''
    #version 330 core

    layout (location = 0) in vec2 in_vertex;
    layout (location = 1) in vec3 in_color;

    out vec3 v_color;

    void main() {
        gl_PointSize = 5.0;
        gl_Position = vec4(in_vertex, 0.0, 1.0);
        v_color = in_color;
    }
'''

ctx.shaders['points.frag'] = '''
    #version 330 core

    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        float alpha = 1.0 - smoothstep(0.3, 0.5, length(gl_PointCoord - 0.5));
        out_color = vec4(v_color, alpha * 0.8);
    }
'''

particles = ctx.pipeline(
    'points.vert',
    'points.frag',
    vertex_array=ctx.vertex_array(
        [
            *vertex_buffer.bind('2f', 0),
            *color_buffer.bind('3f', 1),
        ],
        mode='points',
    ),
    framebuffer=framebuffer,
    blend={
        'src': 'src_alpha',
        'dst': 'one_minus_src_alpha',
    },
)

sim_time = pygame.time.get_ticks()

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    ctx.start_frame()
    framebuffer.clear()
    now = pygame.time.get_ticks()
    sim_time = max(sim_time, now - 100)
    while sim_time < now:
        sim_time += 16
        position[sweep:sweep + emit] = (0.0, -1.0)
        velocity[sweep:sweep + emit] = np.random.normal((0.0, 0.0235), (0.002 * size[1] / size[0], 0.001), (emit, 2))
        sweep = (sweep + emit) % count
        position += velocity
        velocity -= (0.0, 0.0002)
    vertex_buffer.write(position)
    particles.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
