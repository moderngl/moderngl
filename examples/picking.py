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

size = pygame.display.get_window_size()

ctx = moderngl.get_context()

image = ctx.texture(size, 'rgba8unorm')
picker = ctx.texture(size, 'rgba8unorm')
depth = ctx.texture(size, 'depth24plus')
framebuffer = ctx.framebuffer([image, picker, depth])
framebuffer.clear_color = ((0.05, 0.05, 0.05, 1.0), (0.0, 0.0, 0.0, 0.0))

vertices = []
for axis in range(3):
    for sign in (-1.0, 1.0):
        normal = np.zeros(3)
        normal[axis] = sign
        u = np.roll(normal, 1) * sign
        v = np.roll(normal, 2)
        corners = [normal + u * a + v * b for a, b in ((-1, -1), (1, -1), (1, 1), (-1, -1), (1, 1), (-1, 1))]
        vertices += [(*corner * 0.4, *normal) for corner in corners]

vertex_buffer = ctx.buffer(np.array(vertices, 'f4'))

count = 64
x, y = np.meshgrid(np.arange(8) - 3.5, np.arange(8) - 3.5)
hue = np.arange(count) / count
instance_buffer = ctx.buffer(np.array([
    x.flatten(),
    y.flatten(),
    np.zeros(count),
    np.cos((hue + 0.0) * np.pi * 2.0) * 0.4 + 0.5,
    np.cos((hue + 0.33) * np.pi * 2.0) * 0.4 + 0.5,
    np.cos((hue + 0.67) * np.pi * 2.0) * 0.4 + 0.5,
]).T.astype('f4'))

uniform_buffer = ctx.buffer(size=80)

ctx.shaders['picking.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    uniform int hovered;

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;
    layout (location = 2) in vec3 in_position;
    layout (location = 3) in vec3 in_color;

    out vec3 v_normal;
    out vec3 v_color;
    flat out int v_id;

    void main() {
        float lift = gl_InstanceID == hovered ? 0.3 : 0.0;
        gl_Position = camera * vec4(in_position + in_vertex + vec3(0.0, 0.0, lift), 1.0);
        v_normal = in_normal;
        v_color = gl_InstanceID == hovered ? vec3(1.0, 1.0, 1.0) : in_color;
        v_id = gl_InstanceID + 1;
    }
'''

ctx.shaders['picking.frag'] = '''
    #version 330 core

    in vec3 v_normal;
    in vec3 v_color;
    flat in int v_id;

    layout (location = 0) out vec4 out_color;
    layout (location = 1) out vec4 out_id;

    void main() {
        float lum = dot(normalize(vec3(0.3, 0.5, 1.0)), normalize(v_normal)) * 0.4 + 0.6;
        vec3 color = pow(v_color, vec3(2.2)) * lum;
        out_color = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
        out_id = vec4(float(v_id % 256) / 255.0, float(v_id / 256) / 255.0, 0.0, 1.0);
    }
'''

pipeline = ctx.pipeline(
    'picking.vert',
    'picking.frag',
    uniform_buffers={'Common': 0},
    uniforms={'hovered': 'int'},
    vertex_array=ctx.vertex_array([
        *vertex_buffer.bind('3f 3f', 0, 1),
        *instance_buffer.bind('3f 3f /i', 2, 3),
    ]),
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    instances=count,
    depth_test=True,
    cull_face='back',
)

pipeline['hovered'] = -1
request = None

yaw, pitch, spin = 0.0, 0.7, 0.2

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
            pitch = min(max(pitch + event.rel[1] * 0.004, 0.1), 1.4)

    eye = (math.cos(yaw) * math.cos(pitch) * 12.0, math.sin(yaw) * math.cos(pitch) * 12.0, math.sin(pitch) * 12.0)
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))

    if request and request.is_ready():
        red, green = request.wait()[:2]
        pipeline['hovered'] = red + green * 256 - 1
        request = None

    ctx.start_frame()
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x', *eye))
    framebuffer.clear()
    pipeline.run()

    mx, my = pygame.mouse.get_pos()
    if not request and 0 <= mx < size[0] and 0 <= my < size[1]:
        request = picker.read(size=(1, 1), offset=(mx, size[1] - 1 - my))

    ctx.present(image)

    pygame.display.flip()

pygame.quit()
