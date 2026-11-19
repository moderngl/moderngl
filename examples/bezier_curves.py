from colorsys import hls_to_rgb

import extras
import moderngl
import numpy as np
import pygame

extras.init()

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()


def curve_mesh(N=16, M=128):
    t = np.linspace(0.0, np.pi / 2.0, N, endpoint=False)

    sx = np.repeat(-np.cos(t), 2)[1:]
    sy = np.array([np.sin(t), -np.sin(t)]).T.flatten()[1:]
    sz = np.zeros(N * 2 - 1)

    vx = np.zeros(M * 2)
    vy = np.tile([1.0, -1.0], M)
    vz = np.repeat(np.linspace(0.0, 1.0, M), 2)

    x = np.concatenate([sx, vx, -sx[::-1]])
    y = np.concatenate([sy, vy, -sy[::-1]])
    z = np.concatenate([sz, vz, sz + 1.0])

    return np.array([x, y, z]).T.astype('f4').tobytes()


class Curves:
    def __init__(self, center, scale, instance_count=32):
        curves = []
        for _ in range(instance_count):
            a = np.random.uniform(0.0, np.pi * 2.0)
            b = a + np.pi + np.random.uniform(-0.5, 0.5)
            c = np.random.uniform(0.0, np.pi * 2.0)
            d = np.random.uniform(0.0, np.pi * 2.0)
            x1, y1 = np.cos(a) * scale + center[0], np.sin(a) * scale + center[1]
            x2, y2 = np.cos(b) * scale + center[0], np.sin(b) * scale + center[1]
            x3, y3 = np.cos(c) * scale * 0.5 + center[0] - x1, np.sin(c) * scale * 0.5 + center[1] - y1
            x4, y4 = x2 - (np.cos(d) * scale * 0.5 + center[0]), y2 - (np.sin(d) * scale * 0.5 + center[1])
            r, g, b = hls_to_rgb(np.random.uniform(0.0, 1.0), 0.3, 1.0)
            s = np.random.uniform(5.0, 15.0) * scale / 300.0
            curves.append([x1, y1, x3, y3, x2, y2, x4, y4, r, g, b, s])

        self.scale = scale
        self.instance_count = instance_count
        self.offset = np.random.uniform(0.0, np.pi * 2.0, (4, instance_count)).astype('f4')
        self.curves = np.array(curves, 'f4')

    def instances(self, time):
        updates = np.array([
            np.sin(self.offset[0] + time) * self.scale / 30.0,
            np.cos(self.offset[0] + time) * self.scale / 30.0,
            np.sin(self.offset[1] + time) * self.scale / 6.0,
            np.cos(self.offset[1] + time) * self.scale / 6.0,
            np.sin(self.offset[2] + time) * self.scale / 30.0,
            np.cos(self.offset[2] + time) * self.scale / 30.0,
            np.sin(self.offset[3] + time) * self.scale / 6.0,
            np.cos(self.offset[3] + time) * self.scale / 6.0,
            *np.zeros((4, self.instance_count), 'f4'),
        ], 'f4').T
        return self.curves + updates


size = pygame.display.get_window_size()
image = ctx.texture(size, 'rgba8unorm', samples=4)
framebuffer = ctx.framebuffer([image])

curves = Curves((size[0] / 2.0, size[1] / 2.0), min(size) * 0.4)

vertex_buffer = ctx.buffer(curve_mesh())
instance_buffer = ctx.buffer(curves.instances(0.0))

ctx.shaders['screen_size'] = f'vec2 screen_size = vec2({float(size[0])}, {float(size[1])});'

ctx.shaders['bezier_curves.vert'] = '''
    #version 330 core
    precision highp float;

    #include "screen_size"

    layout (location = 0) in vec3 in_vert;

    layout (location = 1) in vec4 in_a;
    layout (location = 2) in vec4 in_b;
    layout (location = 3) in vec4 in_color_and_size;

    out vec3 v_color;

    void main() {
        float t = in_vert.z;
        vec2 A = in_a.xy;
        vec2 B = in_a.xy + in_a.zw;
        vec2 C = in_b.xy - in_b.zw;
        vec2 D = in_b.xy;
        vec2 E = B - A;
        vec2 F = C - B;
        vec2 G = D - C;
        vec2 H = A + E * t;
        vec2 I = B + F * t;
        vec2 J = C + G * t;
        vec2 K = H + (I - H) * t;
        vec2 L = I + (J - I) * t;
        vec2 M = K + (L - K) * t;
        vec2 N = E + (F - E) * t;
        vec2 O = F + (G - F) * t;
        vec2 P = N + (O - N) * t;
        vec2 n = normalize(P);
        mat2 basis = mat2(n, vec2(-n.y, n.x));
        vec2 vert = M + basis * in_vert.xy * in_color_and_size.w;
        gl_Position = vec4((vert / screen_size) * 2.0 - 1.0, 0.0, 1.0);
        v_color = in_color_and_size.rgb;
    }
'''

ctx.shaders['bezier_curves.frag'] = '''
    #version 330 core
    precision highp float;

    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(pow(v_color, vec3(1.0 / 2.2)), 1.0);
    }
'''

pipeline = ctx.pipeline(
    'bezier_curves.vert',
    'bezier_curves.frag',
    vertex_array=ctx.vertex_array(
        [
            *vertex_buffer.bind('3f', 0),
            *instance_buffer.bind('4f 4f 4f /i', 1, 2, 3),
        ],
        mode='triangle_strip',
    ),
    framebuffer=framebuffer,
    instances=curves.instance_count,
)

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    now = pygame.time.get_ticks() / 1000.0

    ctx.start_frame()
    instance_buffer.write(curves.instances(now))
    framebuffer.clear()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
