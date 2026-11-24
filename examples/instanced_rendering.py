import os

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

vertex_buffer = ctx.buffer(np.array([
    0.0, 1.0,
    -0.866, -0.5,
    0.866, -0.5,
], 'f4'))

count = 350
index = np.arange(count)
angle = index * 2.39996
radius = np.sqrt(index / count) * 0.95
hue = index / count
instances = np.array([
    np.cos(angle) * radius * 0.5625,
    np.sin(angle) * radius,
    np.cos((hue + 0.0) * np.pi * 2.0) * 0.5 + 0.5,
    np.cos((hue + 0.33) * np.pi * 2.0) * 0.5 + 0.5,
    np.cos((hue + 0.67) * np.pi * 2.0) * 0.5 + 0.5,
])

instance_buffer = ctx.buffer(instances.T.astype('f4').tobytes())

ctx.shaders['instanced_rendering.vert'] = '''
    #version 330 core

    uniform float time;

    layout (location = 0) in vec2 in_vertex;
    layout (location = 1) in vec2 in_pos;
    layout (location = 2) in vec3 in_color;

    out vec3 v_color;

    void main() {
        float r = time + float(gl_InstanceID) * 0.05;
        vec2 vert = mat2(cos(r), sin(r), -sin(r), cos(r)) * in_vertex * 0.035;
        gl_Position = vec4(in_pos + vert * vec2(0.5625, 1.0), 0.0, 1.0);
        v_color = in_color;
    }
'''

ctx.shaders['instanced_rendering.frag'] = '''
    #version 330 core

    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(v_color, 1.0);
    }
'''

triangle = ctx.pipeline(
    'instanced_rendering.vert',
    'instanced_rendering.frag',
    vertex_array=ctx.vertex_array([
        *vertex_buffer.bind('2f', 0),
        *instance_buffer.bind('2f 3f /i', 1, 2),
    ]),
    framebuffer=framebuffer,
    uniforms={'time': 'float'},
    instances=count,
)

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    ctx.start_frame()
    framebuffer.clear()
    triangle['time'] = pygame.time.get_ticks() / 1000.0
    triangle.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
