import math
import os
import struct
import sys

import glm
import moderngl
import numpy as np
import pygame

sys.path.insert(0, 'examples')
from glb import load_glb

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

vertex_buffer = ctx.buffer(load_glb('examples/data/blob/blob.glb', texcoords=False)['models']['blob'])

count = 400
rotations = np.random.normal(size=(count, 4))
axes = np.random.normal(size=(count, 3))
instances = np.zeros((count, 10), 'f4')
instances[:, 0] = np.arange(count) // 20 * 4.0 - 38.0
instances[:, 1] = np.arange(count) % 20 * 4.0 - 38.0
instances[:, 3:7] = rotations / np.linalg.norm(rotations, axis=1, keepdims=True)
instances[:, 7:10] = axes / np.linalg.norm(axes, axis=1, keepdims=True)
instance_buffer = ctx.buffer(instances)

uniform_buffer = ctx.buffer(size=96)

ctx.shaders['quaternions.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
        float time;
    };

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;
    layout (location = 2) in vec3 in_position;
    layout (location = 3) in vec4 in_rotation;
    layout (location = 4) in vec3 in_axis;

    out vec3 v_normal;

    vec4 qmul(vec4 a, vec4 b) {
        return vec4(a.w * b.xyz + b.w * a.xyz + cross(a.xyz, b.xyz), a.w * b.w - dot(a.xyz, b.xyz));
    }

    vec3 qtransform(vec4 q, vec3 v) {
        return v + 2.0 * cross(cross(v, q.xyz) - q.w * v, q.xyz);
    }

    void main() {
        vec4 rotation = qmul(vec4(in_axis * sin(time * 0.3), cos(time * 0.3)), in_rotation);
        gl_Position = camera * vec4(in_position + qtransform(rotation, in_vertex), 1.0);
        v_normal = qtransform(rotation, in_normal);
    }
'''

ctx.shaders['quaternions.frag'] = '''
    #version 330 core

    in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 light = vec3(0.0, 0.0, 1.0);
        float lum = dot(normalize(light), normalize(v_normal)) * 0.5 + 0.5;
        out_color = vec4(lum, lum, lum, 1.0);
    }
'''

pipeline = ctx.pipeline(
    'quaternions.vert',
    'quaternions.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.vertex_array([
        *vertex_buffer.bind('3f 3f', 0, 1),
        *instance_buffer.bind('3f 4f 3f /i', 2, 3, 4),
    ]),
    instances=count,
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    depth_test=True,
    cull_face='back',
)

yaw, pitch, spin = 0.0, 0.25, 0.2

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

    eye = (math.cos(yaw) * math.cos(pitch) * 61.8, math.sin(yaw) * math.cos(pitch) * 61.8, math.sin(pitch) * 61.8)

    ctx.start_frame()
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4xf12x', *eye, pygame.time.get_ticks() / 1000.0))
    framebuffer.clear()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
