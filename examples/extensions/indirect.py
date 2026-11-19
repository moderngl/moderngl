import ctypes
import math
import random
import struct
import sys

import moderngl
import pygame

GLAPI = ctypes.WINFUNCTYPE if sys.platform == 'win32' else ctypes.CFUNCTYPE


class Indirect:
    def __init__(self, ctx):
        def load(name, *signature):
            return GLAPI(*signature)(moderngl.get_proc_address(name))

        u32, i32, ptr = ctypes.c_uint32, ctypes.c_int32, ctypes.c_void_p
        self.ctx = ctx
        self.glGetIntegerv = load('glGetIntegerv', None, u32, ctypes.POINTER(i32))
        self.glCreateShader = load('glCreateShader', u32, u32)
        self.glShaderSource = load('glShaderSource', None, u32, i32, ctypes.POINTER(ctypes.c_char_p), ptr)
        self.glCompileShader = load('glCompileShader', None, u32)
        self.glGetShaderiv = load('glGetShaderiv', None, u32, u32, ctypes.POINTER(i32))
        self.glGetShaderInfoLog = load('glGetShaderInfoLog', None, u32, i32, ptr, ctypes.c_char_p)
        self.glDeleteShader = load('glDeleteShader', None, u32)
        self.glCreateProgram = load('glCreateProgram', u32)
        self.glAttachShader = load('glAttachShader', None, u32, u32)
        self.glLinkProgram = load('glLinkProgram', None, u32)
        self.glUseProgram = load('glUseProgram', None, u32)
        self.glGetUniformLocation = load('glGetUniformLocation', i32, u32, ctypes.c_char_p)
        self.glProgramUniform1i = load('glProgramUniform1i', None, u32, i32, i32)
        self.glProgramUniform4i = load('glProgramUniform4i', None, u32, i32, i32, i32, i32, i32)
        self.glBindBufferBase = load('glBindBufferBase', None, u32, u32, u32)
        self.glBindBuffer = load('glBindBuffer', None, u32, u32)
        self.glBindVertexArray = load('glBindVertexArray', None, u32)
        self.glDispatchCompute = load('glDispatchCompute', None, u32, u32, u32)
        self.glMemoryBarrier = load('glMemoryBarrier', None, u32)
        self.glPrimitiveRestartIndex = load('glPrimitiveRestartIndex', None, u32)
        self.glMultiDrawElementsIndirect = load('glMultiDrawElementsIndirect', None, u32, u32, ptr, i32, i32)

    def compute_program(self, name):
        source = self.ctx.source(name).encode()
        shader = self.glCreateShader(0x91B9)
        self.glShaderSource(shader, 1, ctypes.byref(ctypes.c_char_p(source)), None)
        self.glCompileShader(shader)
        status = ctypes.c_int32(0)
        self.glGetShaderiv(shader, 0x8B81, ctypes.byref(status))
        if not status.value:
            log = ctypes.create_string_buffer(4096)
            self.glGetShaderInfoLog(shader, 4096, None, log)
            raise ValueError(f'Compute Shader Error in "{name}"\n\n{log.value.decode()}')
        program = self.glCreateProgram()
        self.glAttachShader(program, shader)
        self.glLinkProgram(program)
        self.glDeleteShader(shader)
        return program

    def uniform(self, program, name, *values):
        location = self.glGetUniformLocation(program, name.encode())
        if len(values) == 1:
            self.glProgramUniform1i(program, location, *values)
        else:
            self.glProgramUniform4i(program, location, *values)

    def dispatch(self, program, groups, storage_buffers, uniform_buffers):
        previous = ctypes.c_int32(0)
        self.glGetIntegerv(0x8B8D, ctypes.byref(previous))
        for binding, buffer in storage_buffers.items():
            self.glBindBufferBase(0x90D2, binding, buffer.glo)
        for binding, buffer in uniform_buffers.items():
            self.glBindBufferBase(0x8A11, binding, buffer.glo)
        self.glUseProgram(program)
        self.glDispatchCompute(groups, 1, 1)
        self.glUseProgram(previous.value)
        self.glMemoryBarrier(0x40 | 0x2000)

    def draw(self, vertex_array, commands, count):
        index_type = {1: 0x1401, 2: 0x1403, 4: 0x1405}[vertex_array.index_size]
        self.glBindVertexArray(vertex_array.glo)
        self.glBindBuffer(0x8F3F, commands.glo)
        self.glPrimitiveRestartIndex((1 << vertex_array.index_size * 8) - 1)
        self.glMultiDrawElementsIndirect(0x0004, index_type, None, count, 0)
        self.glBindBuffer(0x8F3F, 0)
        bound = self.ctx.bound_vertex_array
        self.glBindVertexArray(bound.glo if bound else 0)


def cube():
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
    return vertices, indices


def octahedron():
    vertices, indices = bytearray(), []
    for x in (-1.0, 1.0):
        for y in (-1.0, 1.0):
            for z in (-1.0, 1.0):
                corners = [(x * 1.4, 0.0, 0.0), (0.0, y * 1.4, 0.0), (0.0, 0.0, z * 1.4)]
                if x * y * z < 0.0:
                    corners.reverse()
                normal = (x / math.sqrt(3.0), y / math.sqrt(3.0), z / math.sqrt(3.0))
                base = len(vertices) // 24
                for corner in corners:
                    vertices += struct.pack('6f', *corner, *normal)
                indices += [base, base + 1, base + 2]
    return vertices, indices


pygame.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 4)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 6)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()
indirect = Indirect(ctx)

object_count = 1 << 16

ctx.shaders['common'] = '''
    layout (std140) uniform Common {
        mat4 camera;
    };
'''

ctx.shaders['cull.comp'] = '''
    #version 430 core
    #include "common"

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

    layout (std430, binding = 0) readonly buffer Objects {
        Object objects[];
    };

    layout (std430, binding = 1) writeonly buffer Commands {
        Command commands[];
    };

    layout (std430, binding = 2) buffer Counter {
        uint visible;
    };

    uniform ivec4 meshes[2];
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

        ivec4 mesh = meshes[int(object.color.w)];
        commands[index] = Command(uint(mesh.x), inside ? 1u : 0u, uint(mesh.y), 0u, uint(index));
        if (inside) {
            atomicAdd(visible, 1u);
        }
    }
'''

ctx.shaders['object.vert'] = '''
    #version 330 core
    #include "common"

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

ctx.shaders['object.frag'] = '''
    #version 330 core

    in vec3 v_normal;
    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        float lum = 0.35 + 0.65 * max(dot(normalize(v_normal), normalize(vec3(0.4, 0.8, 0.5))), 0.0);
        out_color = vec4(pow(v_color * lum, vec3(1.0 / 2.2)), 1.0);
    }
'''

cube_vertices, cube_indices = cube()
octahedron_vertices, octahedron_indices = octahedron()
cube_vertex_count = len(cube_vertices) // 24
vertex_buffer = ctx.buffer(cube_vertices + octahedron_vertices)
index_buffer = ctx.buffer(struct.pack(f'{len(cube_indices) + len(octahedron_indices)}I', *cube_indices, *[i + cube_vertex_count for i in octahedron_indices]), kind='index_buffer')
meshes = [(len(cube_indices), 0, 0, 0), (len(octahedron_indices), len(cube_indices), 0, 0)]

objects = bytearray()
for i in range(object_count):
    x, y, z = (random.uniform(-200.0, 200.0) for _ in range(3))
    objects += struct.pack('8f', x, y, z, random.uniform(0.3, 1.2), random.random(), random.random(), random.random(), random.randrange(2))

object_buffer = ctx.buffer(objects)
commands = ctx.buffer(size=object_count * 20, kind='copy_buffer')
counter = ctx.buffer(size=4, kind='copy_buffer')
camera_buffer = ctx.buffer(size=64, kind='uniform_buffer')

vertex_array = ctx.vertex_array(
    [*vertex_buffer.bind('3f 3f', 0, 1), *object_buffer.bind('4f 4f /i', 2, 3)],
    index_buffer,
)

cull = indirect.compute_program('cull.comp')
indirect.uniform(cull, 'object_count', object_count)
for i, mesh in enumerate(meshes):
    indirect.uniform(cull, f'meshes[{i}]', *mesh)

program = ctx.program('object.vert', 'object.frag', uniform_buffers={'Common': 0})
resources = ctx.resources(uniform_buffers={0: camera_buffer})

size = pygame.display.get_window_size()
image = ctx.texture(size, 'rgba8unorm', samples=4)
depth = ctx.texture(size, 'depth24plus', samples=4)
output = ctx.texture(size, 'rgba8unorm')
framebuffer = ctx.framebuffer([image, depth])
framebuffer.clear_color = (0.05, 0.05, 0.08, 1.0)

clock = pygame.time.Clock()
time = 0.0
report = 0.0

print(f'{object_count} objects, culled on the GPU, drawn with one call, escape: quit')

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT or event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
            pygame.quit()
            sys.exit()

    dt = clock.tick() / 1000.0
    time += dt

    eye = (math.cos(time * 0.2) * 120.0, math.sin(time * 0.13) * 40.0, math.sin(time * 0.2) * 120.0)
    target = (math.cos(time * 0.2 + 1.0) * 60.0, 0.0, math.sin(time * 0.2 + 1.0) * 60.0)
    camera_buffer.write(moderngl.camera(eye, target, (0.0, 1.0, 0.0), aspect=size[0] / size[1], fov=60.0, near=0.5, far=500.0))
    counter.write(b'\x00\x00\x00\x00')

    ctx.start_frame()
    indirect.dispatch(cull, (object_count + 63) // 64, {0: object_buffer, 1: commands, 2: counter}, {0: camera_buffer})

    framebuffer.use()
    framebuffer.clear()
    ctx.depth_test = True
    ctx.cull_face = 'back'
    program.use()
    resources.use()
    indirect.draw(vertex_array, commands, object_count)

    ctx.copy_texture(image, output)
    ctx.present(output)
    pygame.display.flip()

    report += dt
    if report > 1.0:
        report = 0.0
        visible = struct.unpack('I', counter.read())[0]
        print(f'{visible} of {object_count} visible, {clock.get_fps():.0f} fps')
