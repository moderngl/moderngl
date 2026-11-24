import math
import os
import struct

import glm
import moderngl
import numpy as np
import pygame

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'
pygame.display.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 4)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 2)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, 1)

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

size = pygame.display.get_window_size()

ctx = moderngl.get_context()

image = ctx.texture(size, 'rgba8unorm', multisampled=True)
depth = ctx.texture(size, 'depth24plus', multisampled=True)
framebuffer = ctx.framebuffer([image, depth])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

cube = []
for axis in range(3):
    for sign in (-1.0, 1.0):
        normal = np.zeros(3)
        normal[axis] = sign
        u = np.roll(normal, 1) * sign
        v = np.roll(normal, 2)
        cube += [(*(normal + u * a + v * b) * 0.5, *normal) for a, b in ((-1, -1), (1, -1), (1, 1), (-1, -1), (1, 1), (-1, 1))]

octahedron = []
for x in (-1.0, 1.0):
    for y in (-1.0, 1.0):
        for z in (-1.0, 1.0):
            corners = [(x * 0.7, 0.0, 0.0), (0.0, y * 0.7, 0.0), (0.0, 0.0, z * 0.7)]
            if x * y * z < 0.0:
                corners.reverse()
            octahedron += [(*corner, x / math.sqrt(3.0), y / math.sqrt(3.0), z / math.sqrt(3.0)) for corner in corners]

t, p = np.meshgrid(np.linspace(0.0, np.pi, 13), np.linspace(0.0, np.pi * 2.0, 25), indexing='ij')
points = np.stack([np.sin(t) * np.cos(p), np.sin(t) * np.sin(p), np.cos(t)], axis=-1)
a, b, c, d = points[:-1, :-1], points[:-1, 1:], points[1:, :-1], points[1:, 1:]
sphere = np.stack([a, c, b, b, c, d], axis=2).reshape(-1, 3)
sphere = np.concatenate([sphere * 0.55, sphere], axis=1)

meshes = [np.array(cube, 'f4'), np.array(octahedron, 'f4'), sphere.astype('f4')]
ranges = []
first = 0
for mesh in meshes:
    ranges.append((len(mesh), first))
    first += len(mesh)

vertex_buffer = ctx.buffer(np.concatenate(meshes))

count = 24
index = np.arange(count)
angle = index / count * np.pi * 2.0
objects = np.zeros((count, 2, 4), 'f4')
objects[:, 0, 0] = np.cos(angle) * 4.0
objects[:, 0, 1] = np.sin(angle) * 4.0
objects[:, 0, 2] = np.sin(angle * 3.0) * 0.5
objects[:, 0, 3] = 0.6
objects[:, 1, 0] = np.cos(angle + 0.0) * 0.4 + 0.5
objects[:, 1, 1] = np.cos(angle + 2.1) * 0.4 + 0.5
objects[:, 1, 2] = np.cos(angle + 4.2) * 0.4 + 0.5

object_buffer = ctx.buffer(objects)
object_index_buffer = ctx.buffer(index.astype('i4'))
uniform_buffer = ctx.buffer(size=80)

groups = [(mesh, list(range(mesh, count, 3))) for mesh in range(3)]
order = np.concatenate([np.array(objects_of_mesh, 'i4') for _, objects_of_mesh in groups])
object_index_buffer.write(order)

ctx.shaders['first_instance.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    layout (std140) uniform Objects {
        vec4 objects[48];
    };

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;
    layout (location = 2) in int in_object;

    out vec3 v_normal;
    out vec3 v_color;

    void main() {
        vec4 position = objects[in_object * 2];
        gl_Position = camera * vec4(position.xyz + in_vertex * position.w, 1.0);
        v_normal = in_normal;
        v_color = objects[in_object * 2 + 1].rgb;
    }
'''

ctx.shaders['first_instance.frag'] = '''
    #version 330 core

    in vec3 v_normal;
    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        float lum = dot(normalize(vec3(0.3, 0.5, 1.0)), normalize(v_normal)) * 0.4 + 0.6;
        vec3 color = pow(v_color, vec3(2.2)) * lum;
        out_color = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
    }
'''

vertex_array = ctx.vertex_array([
    *vertex_buffer.bind('3f 3f', 0, 1),
    *object_index_buffer.bind('1i /i', 2),
])

pipeline = ctx.pipeline(
    'first_instance.vert',
    'first_instance.frag',
    uniform_buffers={'Common': 0, 'Objects': 1},
    vertex_array=vertex_array,
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer, 1: object_buffer}),
    depth_test=True,
    cull_face='back',
)

yaw, pitch, spin = 0.0, 0.6, 0.2

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
            pitch = min(max(pitch + event.rel[1] * 0.004, -1.4), 1.4)

    eye = (math.cos(yaw) * math.cos(pitch) * 10.0, math.sin(yaw) * math.cos(pitch) * 10.0, math.sin(pitch) * 10.0)
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))

    ctx.start_frame()
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x', *eye))
    framebuffer.clear()
    pipeline.use()
    first_instance = 0
    for mesh, objects_of_mesh in groups:
        vertices, first = ranges[mesh]
        vertex_array.draw(vertices, first, instances=len(objects_of_mesh), first_instance=first_instance)
        first_instance += len(objects_of_mesh)
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
