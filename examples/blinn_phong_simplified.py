import math
import os
import struct
import sys
from colorsys import hls_to_rgb

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

count = 100
rotations = np.random.normal(size=(count, 4))
axes = np.random.normal(size=(count, 3))
instances = np.zeros((count, 16), 'f4')
instances[:, 0] = np.arange(count) % 10 * 4.0 - 18.0
instances[:, 1] = np.arange(count) // 10 * 4.0
instances[:, 3:7] = rotations / np.linalg.norm(rotations, axis=1, keepdims=True)
instances[:, 7:10] = axes / np.linalg.norm(axes, axis=1, keepdims=True)
instances[:, 10:13] = [hls_to_rgb(h, 0.5, 0.5) for h in np.random.uniform(0.0, 1.0, count)]
instances[:, 13:16] = (0.1, 0.3, 16.0)
instance_buffer = ctx.buffer(instances)

uniform_buffer = ctx.buffer(size=128)

ctx.shaders['blinn_phong_simplified.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
        vec4 light_position;
        vec4 light_color;
        float light_power;
        float time;
    };

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;
    layout (location = 2) in vec3 in_position;
    layout (location = 3) in vec4 in_rotation;
    layout (location = 4) in vec3 in_axis;
    layout (location = 5) in vec3 in_color;
    layout (location = 6) in vec3 in_parameters;

    out vec3 v_vertex;
    out vec3 v_normal;
    out vec3 v_color;
    out vec3 v_parameters;

    vec4 qmul(vec4 a, vec4 b) {
        return vec4(a.w * b.xyz + b.w * a.xyz + cross(a.xyz, b.xyz), a.w * b.w - dot(a.xyz, b.xyz));
    }

    vec3 qtransform(vec4 q, vec3 v) {
        return v + 2.0 * cross(cross(v, q.xyz) - q.w * v, q.xyz);
    }

    void main() {
        vec4 rotation = qmul(vec4(in_axis * sin(time * 0.3), cos(time * 0.3)), in_rotation);
        v_vertex = in_position + qtransform(rotation, in_vertex);
        v_normal = qtransform(rotation, in_normal);
        v_color = in_color;
        v_parameters = in_parameters;
        gl_Position = camera * vec4(v_vertex, 1.0);
    }
'''

ctx.shaders['blinn_phong_simplified.frag'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
        vec4 light_position;
        vec4 light_color;
        float light_power;
        float time;
    };

    in vec3 v_vertex;
    in vec3 v_normal;
    in vec3 v_color;
    in vec3 v_parameters;

    layout (location = 0) out vec4 out_color;

    void main() {
        float ambient = v_parameters.x;
        float facing = v_parameters.y;
        float shininess = v_parameters.z;

        vec3 normal = normalize(v_normal);
        vec3 light_dir = light_position.xyz - v_vertex;
        float light_distance = dot(light_dir, light_dir);
        light_dir = normalize(light_dir);

        float lambertian = max(dot(light_dir, normal), 0.0);
        vec3 view_dir = normalize(camera_position.xyz - v_vertex);
        vec3 half_dir = normalize(light_dir + view_dir);
        float specular = pow(max(dot(half_dir, normal), 0.0), shininess) * float(lambertian > 0.0);
        float facing_view_dot = max(dot(view_dir, normal), 0.0);

        vec3 color = v_color * ambient + v_color * facing_view_dot * facing +
            v_color * lambertian * light_color.rgb * light_power / light_distance +
            specular * light_color.rgb * light_power / light_distance;

        out_color = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
    }
'''

pipeline = ctx.pipeline(
    'blinn_phong_simplified.vert',
    'blinn_phong_simplified.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.vertex_array([
        *vertex_buffer.bind('3f 3f', 0, 1),
        *instance_buffer.bind('3f 4f 3f 3f 3f /i', 2, 3, 4, 5, 6),
    ]),
    instances=count,
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    depth_test=True,
    cull_face='back',
)


yaw, pitch, spin = -1.57, 0.38, 0.2

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
            pitch = min(max(pitch + event.rel[1] * 0.004, 0.05), 1.4)

    eye = (math.cos(yaw) * math.cos(pitch) * 26.9, 10.0 + math.sin(yaw) * math.cos(pitch) * 26.9, math.sin(pitch) * 26.9)

    ctx.start_frame()
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 10.0, 0.0), (0.0, 0.0, 1.0))
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x3f4x3f4xff8x', *eye, 2.0, 3.0, 4.0, 1.0, 1.0, 1.0, 40.0, pygame.time.get_ticks() / 1000.0))
    framebuffer.clear()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
