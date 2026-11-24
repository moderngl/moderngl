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
vertex = ctx.texture(size, 'rgba32float')
normal = ctx.texture(size, 'rgba32float')
color = ctx.texture(size, 'rgba8unorm')
depth = ctx.texture(size, 'depth24plus')
gbuffer = ctx.framebuffer([vertex, normal, color, depth])
gbuffer.clear_color = ((0.0, 0.0, 0.0, 0.0), (0.0, 0.0, 0.0, 0.0), (0.0, 0.0, 0.0, 1.0))

image = ctx.texture(size, 'rgba8unorm')
light_framebuffer = ctx.framebuffer([image])
light_framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)
marker_framebuffer = ctx.framebuffer([image, depth])

blob = load_glb('examples/data/blob/blob.glb', texcoords=False)
vertex_buffer = ctx.buffer(blob['scene'])

t, p = np.meshgrid(np.linspace(0.0, np.pi, 13), np.linspace(0.0, np.pi * 2.0, 25), indexing='ij')
points = np.stack([np.sin(t) * np.cos(p), np.sin(t) * np.sin(p), np.cos(t)], axis=-1)
a, b, c, d = points[:-1, :-1], points[:-1, 1:], points[1:, :-1], points[1:, 1:]
sphere_buffer = ctx.buffer(np.stack([a, c, b, b, c, d], axis=2).reshape(-1, 3).astype('f4'))

mesh = np.frombuffer(b''.join(data for name, data in blob['nodes'].items() if name != 'ground'), 'f4').reshape(-1, 6)
candidates = mesh[np.random.choice(len(mesh), 1000)]
candidates = candidates[:, :3] + candidates[:, 3:] * np.random.uniform(0.04, 0.12, (1000, 1))
nearest = np.concatenate([((mesh[None, :, :3] - chunk[:, None]) ** 2).sum(axis=2).argmin(axis=1) for chunk in np.split(candidates, 20)])
outside = ((candidates - mesh[nearest, :3]) * mesh[nearest, 3:]).sum(axis=1) > 0.03
positions = candidates[outside & (candidates[:, 2] > 0.04)][:300]

count = len(positions)
light_color = np.array([hls_to_rgb(x, 0.5, 1.0) for x in np.random.uniform(0.0, 1.0, count)])
light_instances = np.zeros((count, 7), 'f4')
light_instances[:, 0:3] = positions
light_instances[:, 3] = np.random.uniform(0.2, 0.35, count)
phase = np.random.uniform(0.0, np.pi * 2.0, count)
speed = np.random.uniform(1.0, 3.0, count)
light_instance_buffer = ctx.buffer(light_instances)

uniform_buffer = ctx.buffer(size=80)

ctx.shaders['gbuffer.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;

    out vec3 v_vertex;
    out vec3 v_normal;

    void main() {
        v_vertex = in_vertex;
        v_normal = in_normal;
        gl_Position = camera * vec4(v_vertex, 1.0);
    }
'''

ctx.shaders['gbuffer.frag'] = '''
    #version 330 core

    in vec3 v_vertex;
    in vec3 v_normal;

    layout (location = 0) out vec4 out_vertex;
    layout (location = 1) out vec4 out_normal;
    layout (location = 2) out vec4 out_color;

    void main() {
        out_vertex = vec4(v_vertex, 1.0);
        out_normal = vec4(normalize(v_normal), 1.0);
        out_color = vec4(1.0, 1.0, 1.0, 1.0);
    }
'''

ctx.shaders['lights.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_position;
    layout (location = 2) in float in_size;
    layout (location = 3) in vec3 in_color;

    out vec3 v_position;
    out float v_size;
    out vec3 v_color;

    void main() {
        v_position = in_position;
        v_size = in_size;
        v_color = in_color;
        gl_Position = camera * vec4(in_position + in_vertex * in_size * 1.1, 1.0);
    }
'''

ctx.shaders['lights.frag'] = '''
    #version 330 core

    uniform sampler2D Vertex;
    uniform sampler2D Normal;
    uniform sampler2D Color;

    in vec3 v_position;
    in float v_size;
    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        ivec2 at = ivec2(gl_FragCoord.xy);
        vec4 normal = texelFetch(Normal, at, 0);
        if (normal.w == 0.0) {
            discard;
        }
        vec3 vertex = texelFetch(Vertex, at, 0).xyz;
        vec3 color = texelFetch(Color, at, 0).rgb;
        vec3 ray = v_position - vertex;
        float falloff = pow(max(1.0 - length(ray) / v_size, 0.0), 2.0);
        float lum = max(dot(normalize(ray), normal.xyz), 0.0) * falloff;
        out_color = vec4(pow(color * v_color * lum, vec3(1.0 / 2.2)), 1.0);
    }
'''

ctx.shaders['markers.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_position;
    layout (location = 2) in vec3 in_color;

    out vec3 v_color;

    void main() {
        v_color = in_color;
        gl_Position = camera * vec4(in_position + in_vertex * 0.012, 1.0);
    }
'''

ctx.shaders['markers.frag'] = '''
    #version 330 core

    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(mix(v_color, vec3(1.0), 0.3), 1.0);
    }
'''

scene = ctx.pipeline(
    'gbuffer.vert',
    'gbuffer.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.vertex_array(vertex_buffer.bind('3f 3f', 0, 1)),
    framebuffer=gbuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    depth_test=True,
    cull_face='back',
)

lights = ctx.pipeline(
    'lights.vert',
    'lights.frag',
    samplers={'Vertex': 0, 'Normal': 1, 'Color': 2},
    uniform_buffers={'Common': 0},
    vertex_array=ctx.vertex_array([
        *sphere_buffer.bind('3f', 0),
        *light_instance_buffer.bind('3f 1f 3f /i', 1, 2, 3),
    ]),
    instances=count,
    framebuffer=light_framebuffer,
    resources=ctx.resources(
        uniform_buffers={0: uniform_buffer},
        samplers={
            0: ctx.sampler(vertex, min_filter='nearest', mag_filter='nearest'),
            1: ctx.sampler(normal, min_filter='nearest', mag_filter='nearest'),
            2: ctx.sampler(color, min_filter='nearest', mag_filter='nearest'),
        },
    ),
    cull_face='front',
    blend={'src': 'one', 'dst': 'one'},
)

markers = ctx.pipeline(
    'markers.vert',
    'markers.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.vertex_array([
        *sphere_buffer.bind('3f', 0),
        *light_instance_buffer.bind('3f 4x 3f /i', 1, 2),
    ]),
    instances=count,
    framebuffer=marker_framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    depth_test=True,
    cull_face='back',
)

yaw, pitch, spin = 0.0, 0.38, 0.2

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

    now = pygame.time.get_ticks() / 1000.0
    light_instances[:, 4:7] = light_color * (0.5 + 0.5 * np.sin(phase + now * speed))[:, None]

    eye = (math.cos(yaw) * math.cos(pitch) * 4.5, math.sin(yaw) * math.cos(pitch) * 4.5, 0.5 + math.sin(pitch) * 4.5)
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.5), (0.0, 0.0, 1.0))

    ctx.start_frame()
    light_instance_buffer.write(light_instances)
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x', *eye))
    gbuffer.clear()
    scene.run()
    light_framebuffer.clear()
    lights.run()
    markers.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
