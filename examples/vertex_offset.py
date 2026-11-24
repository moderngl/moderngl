import math
import os
import struct

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


def uv_sphere(rows, cols):
    theta = np.linspace(0.0, np.pi, rows + 1)
    phi = np.linspace(0.0, np.pi * 2.0, cols + 1)
    t, p = np.meshgrid(theta, phi, indexing='ij')
    points = np.stack([np.sin(t) * np.cos(p), np.sin(t) * np.sin(p), np.cos(t)], axis=-1)
    a, b, c, d = points[:-1, :-1], points[:-1, 1:], points[1:, :-1], points[1:, 1:]
    tris = np.stack([a, c, b, b, c, d], axis=2).reshape(-1, 3, 3)
    normals = np.repeat(np.cross(tris[:, 1] - tris[:, 0], tris[:, 2] - tris[:, 0]), 3, axis=0)
    normals /= np.linalg.norm(normals, axis=1, keepdims=True) + 1e-9
    return np.concatenate([tris.reshape(-1, 3), normals], axis=1).astype('f4')


levels = []
data = bytearray()
for res in range(3, 40):
    mesh = uv_sphere(res, res * 2)
    levels.append((len(data) // 24, len(mesh)))
    data += mesh.tobytes()

vertex_buffer = ctx.buffer(data)
vertex_array = ctx.vertex_array(vertex_buffer.bind('3f 3f', 0, 1))

ctx.shaders['vertex_offset.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };
    uniform vec3 position;

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;

    out vec3 v_normal;

    void main() {
        gl_Position = camera * vec4(position + in_vertex, 1.0);
        v_normal = in_normal;
    }
'''

ctx.shaders['vertex_offset.frag'] = '''
    #version 330 core

    in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 light = vec3(4.0, 3.0, 10.0);
        float lum = dot(normalize(light), normalize(v_normal)) * 0.7 + 0.3;
        out_color = vec4(lum, lum, lum, 1.0);
    }
'''

uniform_buffer = ctx.buffer(size=80)

pipeline = ctx.pipeline(
    'vertex_offset.vert',
    'vertex_offset.frag',
    uniform_buffers={'Common': 0},
    uniforms={'position': 'vec3'},
    vertex_array=vertex_array,
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    depth_test=True,
    cull_face='back',
)

yaw, pitch, spin = -1.57, 0.42, 0.2

running = True
while running:
    if spin:
        yaw = pygame.time.get_ticks() / 1000.0 * spin - 1.57
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.MOUSEMOTION and event.buttons[0]:
            spin = 0.0
            yaw -= event.rel[0] * 0.004
            pitch = min(max(pitch + event.rel[1] * 0.004, -1.4), 1.4)

    now = pygame.time.get_ticks() / 1000.0

    eye = (math.cos(yaw) * math.cos(pitch) * 9.85, math.sin(yaw) * math.cos(pitch) * 9.85, math.sin(pitch) * 9.85)
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))

    ctx.start_frame()
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x', *eye))
    framebuffer.clear()
    pipeline.use()
    for i in range(5):
        first, vertices = levels[int((0.5 - math.cos(now * 0.5 + i * 0.6) * 0.5) * (len(levels) - 1))]
        pipeline.program['position'] = (i * 2.4 - 4.8, 0.0, 0.0)
        vertex_array.draw(vertices, first)
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
