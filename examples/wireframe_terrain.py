import math

import extras
import moderngl
import numpy as np
import pygame

extras.init()

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()


def create_terrain(size):
    V = np.random.normal(0.0, 0.005, size * size)

    for _ in range(128):
        k = np.random.randint(5, 15)
        x, y = np.random.randint(0, size, 2)
        sx, ex, sy, ey = np.clip([x - k, x + k + 1, y - k, y + k + 1], 0, size)
        IX, IY = np.meshgrid(np.arange(sx, ex), np.arange(sy, ey))
        D = np.clip(np.sqrt(np.square((IX - x) / k) + np.square((IY - y) / k)), 0.0, 1.0)
        V[IX * size + IY] += np.cos(D * np.pi / 2.0) * k * np.random.normal(0.0, 0.005)

    X, Y = np.meshgrid(np.linspace(-1.0, 1.0, size), np.linspace(-1.0, 1.0, size))
    P = np.array([X.flatten(), Y.flatten(), V]).T
    A, B = np.meshgrid(np.arange(size + 1), np.arange(size))
    Q = np.concatenate([A + B * size, A * size + B])
    Q[:, -1] = -1
    return P.astype('f4').tobytes(), Q.astype('i4').tobytes()


size = pygame.display.get_window_size()
image = ctx.texture(size, 'rgba8unorm', samples=4)
depth = ctx.texture(size, 'depth24plus', samples=4)
framebuffer = ctx.framebuffer([image, depth])

vertex_data, index_data = create_terrain(64)
vertex_buffer = ctx.buffer(vertex_data)
index_buffer = ctx.buffer(index_data, kind='index_buffer')
uniform_buffer = ctx.buffer(size=64, kind='uniform_buffer')

ctx.shaders['wireframe_terrain.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 mvp;
    };

    layout (location = 0) in vec3 in_vert;

    void main() {
        gl_Position = mvp * vec4(in_vert, 1.0);
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


    ctx.start_frame()
    eye = (math.cos(yaw) * math.cos(pitch) * 3.354, math.sin(yaw) * math.cos(pitch) * 3.354, math.sin(pitch) * 3.354)
    camera = moderngl.camera(eye, (0.0, 0.0, 0.0), aspect=1.777, fov=45.0)
    uniform_buffer.write(camera)
    framebuffer.clear()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
