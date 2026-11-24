import ctypes
import math
import struct
import sys

import glm
import moderngl
import pygame

GLAPI = ctypes.WINFUNCTYPE if sys.platform == 'win32' else ctypes.CFUNCTYPE


class MeshShaders:
    def __init__(self, ctx):
        def load(name, *signature):
            return GLAPI(*signature)(moderngl.get_proc_address(name))

        u32, i32, ptr = ctypes.c_uint32, ctypes.c_int32, ctypes.c_void_p
        self.ctx = ctx
        self.glGetIntegerv = load('glGetIntegerv', None, u32, ctypes.POINTER(i32))
        self.glGetStringi = load('glGetStringi', ctypes.c_char_p, u32, u32)
        self.glCreateShader = load('glCreateShader', u32, u32)
        self.glShaderSource = load('glShaderSource', None, u32, i32, ctypes.POINTER(ctypes.c_char_p), ptr)
        self.glCompileShader = load('glCompileShader', None, u32)
        self.glGetShaderiv = load('glGetShaderiv', None, u32, u32, ctypes.POINTER(i32))
        self.glGetShaderInfoLog = load('glGetShaderInfoLog', None, u32, i32, ptr, ctypes.c_char_p)
        self.glDeleteShader = load('glDeleteShader', None, u32)
        self.glCreateProgram = load('glCreateProgram', u32)
        self.glAttachShader = load('glAttachShader', None, u32, u32)
        self.glLinkProgram = load('glLinkProgram', None, u32)
        self.glGetProgramiv = load('glGetProgramiv', None, u32, u32, ctypes.POINTER(i32))
        self.glGetProgramInfoLog = load('glGetProgramInfoLog', None, u32, i32, ptr, ctypes.c_char_p)
        self.glUseProgram = load('glUseProgram', None, u32)
        self.glBindBufferBase = load('glBindBufferBase', None, u32, u32, u32)

        count = ctypes.c_int32(0)
        self.glGetIntegerv(0x821D, ctypes.byref(count))
        self.extensions = {self.glGetStringi(0x1F03, i).decode() for i in range(count.value)}
        self.supported = 'GL_NV_mesh_shader' in self.extensions
        if self.supported:
            self.glDrawMeshTasksNV = load('glDrawMeshTasksNV', None, u32, u32)

    def limit(self, pname):
        value = ctypes.c_int32(0)
        self.glGetIntegerv(pname, ctypes.byref(value))
        return value.value

    def shader(self, kind, name):
        source = self.ctx.source(name).encode()
        shader = self.glCreateShader(kind)
        self.glShaderSource(shader, 1, ctypes.byref(ctypes.c_char_p(source)), None)
        self.glCompileShader(shader)
        status = ctypes.c_int32(0)
        self.glGetShaderiv(shader, 0x8B81, ctypes.byref(status))
        if not status.value:
            self.glGetShaderiv(shader, 0x8B84, ctypes.byref(status))
            log = ctypes.create_string_buffer(status.value + 1)
            self.glGetShaderInfoLog(shader, status.value + 1, None, log)
            open('temp_shader_error.log', 'wb').write(log.value)
            raise ValueError(f'Shader Error in "{name}", full log in temp_shader_error.log\n\n{log.value.decode(errors="replace")}')
        return shader

    def program(self, mesh_shader, fragment_shader, task_shader=None):
        shaders = [self.shader(0x9559, mesh_shader), self.shader(0x8B30, fragment_shader)]
        if task_shader:
            shaders.append(self.shader(0x955A, task_shader))
        program = self.glCreateProgram()
        for shader in shaders:
            self.glAttachShader(program, shader)
        self.glLinkProgram(program)
        for shader in shaders:
            self.glDeleteShader(shader)
        status = ctypes.c_int32(0)
        self.glGetProgramiv(program, 0x8B82, ctypes.byref(status))
        if not status.value:
            self.glGetProgramiv(program, 0x8B84, ctypes.byref(status))
            log = ctypes.create_string_buffer(status.value + 1)
            self.glGetProgramInfoLog(program, status.value + 1, None, log)
            open('temp_shader_error.log', 'wb').write(log.value)
            raise ValueError(f'Linker Error in "{mesh_shader}" and "{fragment_shader}", full log in temp_shader_error.log\n\n{log.value.decode(errors="replace")}')
        return program

    def draw(self, program, tasks, uniform_buffers=None):
        previous = ctypes.c_int32(0)
        self.glGetIntegerv(0x8B8D, ctypes.byref(previous))
        for binding, buffer in (uniform_buffers or {}).items():
            self.glBindBufferBase(0x8A11, binding, buffer.glo)
        self.glUseProgram(program)
        self.glDrawMeshTasksNV(0, tasks)
        self.glUseProgram(previous.value)


pygame.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 4)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 6)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()
mesh_shaders = MeshShaders(ctx)

print(ctx.info['renderer'], '|', ctx.info['version'])
print('GL_NV_mesh_shader:', mesh_shaders.supported)
if not mesh_shaders.supported:
    sys.exit(1)

print('max mesh work group invocations:', mesh_shaders.limit(0x95A2), '| max draw mesh tasks:', mesh_shaders.limit(0x953D))
print('max mesh output vertices:', mesh_shaders.limit(0x9538), '| max mesh output primitives:', mesh_shaders.limit(0x9539))

ctx.shaders['cubes.mesh'] = '''
    #version 450 core
    #extension GL_NV_mesh_shader : require

    layout (std140, binding = 0) uniform Common {
        mat4 camera;
        vec4 params;
    };

    layout (local_size_x = 32) in;
    layout (triangles, max_vertices = 24, max_primitives = 12) out;

    layout (location = 0) out Vertex {
        vec3 normal;
        vec3 color;
    } v_out[];

    const vec3 axes[3] = vec3[](vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0));
    const vec2 corners[4] = vec2[](vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(1.0, 1.0), vec2(-1.0, 1.0));

    void main() {
        uint cube = gl_WorkGroupID.x;
        uint grid = uint(params.y);
        float time = params.x;
        vec2 cell = vec2(float(cube % grid), float(cube / grid)) - float(grid) * 0.5;
        float wave = sin(cell.x * 0.25 + time) * cos(cell.y * 0.25 + time * 0.7);
        vec3 center = vec3(cell.x, wave * 3.0, cell.y);
        vec3 color = mix(vec3(0.1, 0.4, 1.0), vec3(1.0, 0.5, 0.1), wave * 0.5 + 0.5);

        uint i = gl_LocalInvocationID.x;

        if (i < 24) {
            uint face = i / 4;
            uint axis = face / 2;
            float sign = face % 2 == 0 ? -1.0 : 1.0;
            vec3 normal = axes[axis] * sign;
            vec3 u = axes[(axis + 1) % 3];
            vec3 v = axes[(axis + 2) % 3] * sign;
            vec2 corner = corners[i % 4];
            vec3 position = center + (normal + u * corner.x + v * corner.y) * 0.4;
            gl_MeshVerticesNV[i].gl_Position = camera * vec4(position, 1.0);
            v_out[i].normal = normal;
            v_out[i].color = color;
        }

        if (i < 12) {
            uint base = i / 2 * 4;
            uint second = i % 2 == 0 ? 1 : 2;
            gl_PrimitiveIndicesNV[i * 3 + 0] = base;
            gl_PrimitiveIndicesNV[i * 3 + 1] = base + second;
            gl_PrimitiveIndicesNV[i * 3 + 2] = base + second + 1;
        }

        if (i == 0) {
            gl_PrimitiveCountNV = 12;
        }
    }
'''

ctx.shaders['cubes.frag'] = '''
    #version 450 core

    layout (location = 0) in Vertex {
        vec3 normal;
        vec3 color;
    } v_in;

    layout (location = 0) out vec4 out_color;

    void main() {
        float lum = 0.3 + 0.7 * max(dot(normalize(v_in.normal), normalize(vec3(0.4, 0.8, 0.3))), 0.0);
        out_color = vec4(pow(v_in.color * lum, vec3(1.0 / 2.2)), 1.0);
    }
'''

program = mesh_shaders.program('cubes.mesh', 'cubes.frag')

grid = 128
common_buffer = ctx.buffer(size=80)

size = pygame.display.get_window_size()
image = ctx.texture(size, 'rgba8unorm', multisampled=True)
depth = ctx.texture(size, 'depth24plus', multisampled=True)
output = ctx.texture(size, 'rgba8unorm')
framebuffer = ctx.framebuffer([image, depth])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

clock = pygame.time.Clock()
time = 0.0
report = 0.0

print(f'{grid * grid} cubes, one mesh shader work group each, no vertex buffers, escape: quit')

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT or event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
            pygame.quit()
            sys.exit()

    dt = clock.tick() / 1000.0
    time += dt

    eye = (math.cos(time * 0.1) * 90.0, 45.0, math.sin(time * 0.1) * 90.0)
    camera = (glm.perspective(math.radians(45.0), size[0] / size[1], 1.0, 500.0) * glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 1.0, 0.0))).to_bytes()
    common_buffer.write(camera + struct.pack('4f', time, grid, 0.0, 0.0))

    ctx.start_frame()
    framebuffer.use()
    framebuffer.clear()
    ctx.depth_test = True
    ctx.empty_vertex_array.use()
    mesh_shaders.draw(program, grid * grid, uniform_buffers={0: common_buffer})
    ctx.copy_texture(image, output)
    ctx.present(output)
    pygame.display.flip()

    report += dt
    if report > 1.0:
        report = 0.0
        print(f'{clock.get_fps():.0f} fps')
