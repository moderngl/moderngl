import math
import os

import glm
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

ctx = moderngl.get_context()


size = pygame.display.get_window_size()
image = ctx.texture(size, 'rgba8unorm', multisampled=True)
depth = ctx.texture(size, 'depth24plus', multisampled=True)
framebuffer = ctx.framebuffer([image, depth])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

V = np.random.normal(0.0, 0.005, 64 * 64)
for _ in range(128):
    k = np.random.randint(5, 15)
    x, y = np.random.randint(0, 64, 2)
    sx, ex, sy, ey = np.clip([x - k, x + k + 1, y - k, y + k + 1], 0, 64)
    IX, IY = np.meshgrid(np.arange(sx, ex), np.arange(sy, ey))
    D = np.clip(np.sqrt(np.square((IX - x) / k) + np.square((IY - y) / k)), 0.0, 1.0)
    V[IX * 64 + IY] += np.cos(D * np.pi / 2.0) * k * np.random.normal(0.0, 0.005)

X, Y = np.meshgrid(np.linspace(-1.0, 1.0, 64), np.linspace(-1.0, 1.0, 64))
A, B = np.meshgrid(np.arange(64 + 1), np.arange(64))
Q = np.concatenate([A + B * 64, A * 64 + B])
Q[:, -1] = -1

vertex_buffer = ctx.buffer(np.array([X.flatten(), Y.flatten(), V]).T.astype('f4'))
index_buffer = ctx.buffer(Q.astype('i4'), index=True)
uniform_buffer = ctx.buffer(size=64)

ctx.shaders['wireframe_terrain.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
    };

    layout (location = 0) in vec3 in_vertex;

    void main() {
        gl_Position = camera * vec4(in_vertex, 1.0);
    }
'''

ctx.shaders['wireframe_terrain.frag'] = '''
    #version 330 core

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(1.0, 1.0, 1.0, 1.0);
    }
'''

pipeline = ctx.pipeline(
    'wireframe_terrain.vert',
    'wireframe_terrain.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.vertex_array(vertex_buffer.bind('3f', 0), index_buffer, mode='line_strip'),
    framebuffer=framebuffer,
    resources=ctx.resources(
        uniform_buffers={
            0: uniform_buffer,
        },
    ),
    depth_test=True,
)

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


    ctx.start_frame()
    eye = (math.cos(yaw) * math.cos(pitch) * 3.354, math.sin(yaw) * math.cos(pitch) * 3.354, math.sin(pitch) * 3.354)
    camera = (glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))).to_bytes()
    uniform_buffer.write(camera)
    framebuffer.clear()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
