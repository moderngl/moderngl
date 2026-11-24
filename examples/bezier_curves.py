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

ctx = moderngl.get_context()


size = pygame.display.get_window_size()
image = ctx.texture(size, 'rgba8unorm', multisampled=True)
framebuffer = ctx.framebuffer([image])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

t = np.linspace(0.0, np.pi / 2.0, 16, endpoint=False)
sx = np.repeat(-np.cos(t), 2)[1:]
sy = np.array([np.sin(t), -np.sin(t)]).T.flatten()[1:]
sz = np.zeros(16 * 2 - 1)
vx = np.zeros(128 * 2)
vy = np.tile([1.0, -1.0], 128)
vz = np.repeat(np.linspace(0.0, 1.0, 128), 2)
vertex_buffer = ctx.buffer(np.array([
    np.concatenate([sx, vx, -sx[::-1]]),
    np.concatenate([sy, vy, -sy[::-1]]),
    np.concatenate([sz, vz, sz + 1.0]),
]).T.astype('f4'))

count = 32
center = size[0] / 2.0, size[1] / 2.0
scale = min(size) * 0.4
curves = []
for _ in range(count):
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
curves = np.array(curves, 'f4')
offset = np.random.uniform(0.0, np.pi * 2.0, (4, count)).astype('f4')

instance_buffer = ctx.buffer(curves)

ctx.shaders['bezier_curves.vert'] = '''
    #version 330 core

    uniform vec2 screen_size;

    layout (location = 0) in vec3 in_vertex;

    layout (location = 1) in vec4 in_a;
    layout (location = 2) in vec4 in_b;
    layout (location = 3) in vec4 in_color_and_size;

    out vec3 v_color;

    void main() {
        float t = in_vertex.z;
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
        vec2 vert = M + basis * in_vertex.xy * in_color_and_size.w;
        gl_Position = vec4((vert / screen_size) * 2.0 - 1.0, 0.0, 1.0);
        v_color = in_color_and_size.rgb;
    }
'''

ctx.shaders['bezier_curves.frag'] = '''
    #version 330 core

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
    uniforms={'screen_size': 'vec2'},
    framebuffer=framebuffer,
    instances=count,
)

pipeline['screen_size'] = size

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    now = pygame.time.get_ticks() / 1000.0

    ctx.start_frame()
    instance_buffer.write(curves + np.array([
        np.sin(offset[0] + now) * scale / 30.0,
        np.cos(offset[0] + now) * scale / 30.0,
        np.sin(offset[1] + now) * scale / 6.0,
        np.cos(offset[1] + now) * scale / 6.0,
        np.sin(offset[2] + now) * scale / 30.0,
        np.cos(offset[2] + now) * scale / 30.0,
        np.sin(offset[3] + now) * scale / 6.0,
        np.cos(offset[3] + now) * scale / 6.0,
        *np.zeros((4, count), 'f4'),
    ], 'f4').T)
    framebuffer.clear()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
