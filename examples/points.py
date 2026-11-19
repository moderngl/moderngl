from colorsys import hls_to_rgb

import extras
import moderngl
import numpy as np
import pygame


class ParticleSystem:
    def __init__(self, N, K):
        self.N = N
        self.K = K
        self.position = np.zeros((N, 2), 'f4')
        self.velocity = np.zeros((N, 2), 'f4')
        self.position[:, 1] = -1.0
        self.sweep = 0

    def update(self):
        self.position[self.sweep:self.sweep + self.K] = (0.0, -1.0)
        self.velocity[self.sweep:self.sweep + self.K] = np.random.normal((0.0, 0.02), (0.003, 0.002), (self.K, 2))
        self.sweep = (self.sweep + self.K) % self.N
        self.position += self.velocity
        self.velocity -= (0.0, 0.0002)

    def get_buffer(self):
        return self.position.astype('f4').tobytes()


extras.init()

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

window_size = pygame.display.get_window_size()

ctx = moderngl.get_context()

image = ctx.texture(window_size, 'rgba8unorm', samples=4)
framebuffer = ctx.framebuffer([image])
framebuffer.clear_color = (0.0, 0.0, 0.0, 1.0)

ps = ParticleSystem(10000, 50)

vertex_buffer = ctx.buffer(size=ps.N * 8, usage='stream_draw')
color_buffer = ctx.buffer(np.array([
    hls_to_rgb(np.random.uniform(0.0, 1.0), 0.6, 0.8)
    for _ in range(ps.N)
]).astype('f4').tobytes())

ctx.shaders['points.vert'] = '''
    #version 330 core

    layout (location = 0) in vec2 in_vert;
    layout (location = 1) in vec3 in_color;

    out vec3 v_color;

    void main() {
        gl_PointSize = 3.0;
        gl_Position = vec4(in_vert, 0.0, 1.0);
        v_color = in_color;
    }
'''

ctx.shaders['points.frag'] = '''
    #version 330 core

    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(v_color, 0.7);
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
        ps.update()
    vertex_buffer.write(ps.get_buffer())
    particles.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
