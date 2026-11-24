import math
import os
import random
import struct

import glm
import moderngl
import pygame

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'
pygame.display.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 4)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 3)
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

vertices, indices = bytearray(), []
for axis in range(3):
    for sign in (-1.0, 1.0):
        normal = [0.0, 0.0, 0.0]
        normal[axis] = sign
        u, v = [0.0, 0.0, 0.0], [0.0, 0.0, 0.0]
        u[(axis + 1) % 3] = 1.0
        v[(axis + 2) % 3] = sign
        base = len(vertices) // 24
        for a, b in ((-1, -1), (1, -1), (1, 1), (-1, 1)):
            vertices += struct.pack('6f', *[normal[i] + u[i] * a + v[i] * b for i in range(3)], *normal)
        indices += [base, base + 1, base + 2, base, base + 2, base + 3]

cube_index_count = len(indices)
for x in (-1.0, 1.0):
    for y in (-1.0, 1.0):
        for z in (-1.0, 1.0):
            corners = [(x * 1.4, 0.0, 0.0), (0.0, y * 1.4, 0.0), (0.0, 0.0, z * 1.4)]
            if x * y * z < 0.0:
                corners.reverse()
            base = len(vertices) // 24
            for corner in corners:
                vertices += struct.pack('6f', *corner, x / math.sqrt(3.0), y / math.sqrt(3.0), z / math.sqrt(3.0))
            indices += [base, base + 1, base + 2]

object_count = 1 << 16

objects = bytearray()
for i in range(object_count):
    x, y, z = (random.uniform(-200.0, 200.0) for _ in range(3))
    objects += struct.pack('8f', x, y, z, random.uniform(0.3, 1.2), random.random(), random.random(), random.random(), random.randrange(2))

vertex_buffer = ctx.buffer(vertices)
index_buffer = ctx.buffer(struct.pack(f'{len(indices)}I', *indices), index=True)
object_buffer = ctx.buffer(objects)
command_buffer = ctx.buffer(size=object_count * 20)
uniform_buffer = ctx.buffer(size=80)

ctx.shaders['indirect.comp'] = '''
    #version 430 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    layout (local_size_x = 64) in;

    struct Object {
        vec4 position;
        vec4 color;
    };

    struct Command {
        uint count;
        uint instance_count;
        uint first_index;
        uint base_vertex;
        uint base_instance;
    };

    layout (std430) readonly buffer Objects {
        Object objects[];
    };

    layout (std430) writeonly buffer Commands {
        Command commands[];
    };

    uniform ivec2 meshes[2];
    uniform int object_count;

    void main() {
        int index = int(gl_GlobalInvocationID.x);
        if (index >= object_count) {
            return;
        }

        Object object = objects[index];
        mat4 m = transpose(camera);
        vec4 planes[6] = vec4[](m[3] + m[0], m[3] - m[0], m[3] + m[1], m[3] - m[1], m[3] + m[2], m[3] - m[2]);
        bool inside = true;
        for (int i = 0; i < 6; ++i) {
            inside = inside && dot(planes[i].xyz, object.position.xyz) + planes[i].w > -object.position.w * 1.8 * length(planes[i].xyz);
        }

        ivec2 mesh = meshes[int(object.color.w)];
        commands[index] = Command(uint(mesh.x), inside ? 1u : 0u, uint(mesh.y), 0u, uint(index));
    }
'''

ctx.shaders['indirect.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;
    layout (location = 2) in vec4 in_object;
    layout (location = 3) in vec4 in_color;

    out vec3 v_normal;
    out vec3 v_color;

    void main() {
        gl_Position = camera * vec4(in_object.xyz + in_vertex * in_object.w, 1.0);
        v_normal = in_normal;
        v_color = in_color.rgb;
    }
'''

ctx.shaders['indirect.frag'] = '''
    #version 330 core

    in vec3 v_normal;
    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        float lum = 0.35 + 0.65 * max(dot(normalize(v_normal), normalize(vec3(0.4, 0.5, 0.8))), 0.0);
        out_color = vec4(pow(v_color * lum, vec3(1.0 / 2.2)), 1.0);
    }
'''

cull = ctx.compute(
    'indirect.comp',
    storage_buffers={'Objects': 0, 'Commands': 1},
    uniform_buffers={'Common': 0},
    uniforms={'meshes': 'ivec2[2]', 'object_count': 'int'},
    resources=ctx.resources(storage_buffers={0: object_buffer, 1: command_buffer}, uniform_buffers={0: uniform_buffer}),
    group_count=((object_count + 63) // 64, 1, 1),
)

cull['meshes'] = [(cube_index_count, 0), (len(indices) - cube_index_count, cube_index_count)]
cull['object_count'] = object_count

vertex_array = ctx.vertex_array([*vertex_buffer.bind('3f 3f', 0, 1), *object_buffer.bind('4f 4f /i', 2, 3)], index_buffer)

pipeline = ctx.pipeline(
    'indirect.vert',
    'indirect.frag',
    uniform_buffers={'Common': 0},
    vertex_array=vertex_array,
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    depth_test=True,
    cull_face='back',
)

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    now = pygame.time.get_ticks() / 1000.0
    eye = (math.cos(now * 0.2) * 120.0, math.sin(now * 0.2) * 120.0, math.sin(now * 0.13) * 40.0)
    target = (math.cos(now * 0.2 + 1.0) * 60.0, math.sin(now * 0.2 + 1.0) * 60.0, 0.0)
    camera = glm.perspective(math.radians(60.0), size[0] / size[1], 0.5, 500.0) * glm.lookAt(eye, target, (0.0, 0.0, 1.0))

    ctx.start_frame()
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x', *eye))
    cull.run()
    ctx.memory_barrier()
    framebuffer.clear()
    pipeline.use()
    vertex_array.draw_indirect(command_buffer, object_count)
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
