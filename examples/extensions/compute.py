import ctypes
import math
import random
import struct
import sys

import moderngl
import pygame

GLAPI = ctypes.WINFUNCTYPE if sys.platform == 'win32' else ctypes.CFUNCTYPE


class Compute:
    def __init__(self, ctx):
        def load(name, *signature):
            return GLAPI(*signature)(moderngl.get_proc_address(name))

        u32, i32, f32, ptr = ctypes.c_uint32, ctypes.c_int32, ctypes.c_float, ctypes.c_void_p
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
        self.glGetProgramiv = load('glGetProgramiv', None, u32, u32, ctypes.POINTER(i32))
        self.glGetProgramInfoLog = load('glGetProgramInfoLog', None, u32, i32, ptr, ctypes.c_char_p)
        self.glDeleteProgram = load('glDeleteProgram', None, u32)
        self.glUseProgram = load('glUseProgram', None, u32)
        self.glGetUniformLocation = load('glGetUniformLocation', i32, u32, ctypes.c_char_p)
        self.glProgramUniform1i = load('glProgramUniform1i', None, u32, i32, i32)
        self.glProgramUniform1f = load('glProgramUniform1f', None, u32, i32, f32)
        self.glProgramUniform2f = load('glProgramUniform2f', None, u32, i32, f32, f32)
        self.glProgramUniform3f = load('glProgramUniform3f', None, u32, i32, f32, f32, f32)
        self.glProgramUniform4f = load('glProgramUniform4f', None, u32, i32, f32, f32, f32, f32)
        self.glBindBufferBase = load('glBindBufferBase', None, u32, u32, u32)
        self.glDispatchCompute = load('glDispatchCompute', None, u32, u32, u32)
        self.glMemoryBarrier = load('glMemoryBarrier', None, u32)

    def program(self, name):
        source = self.ctx.source(name).encode()
        shader = self.glCreateShader(0x91B9)
        self.glShaderSource(shader, 1, ctypes.byref(ctypes.c_char_p(source)), None)
        self.glCompileShader(shader)
        status = ctypes.c_int32(0)
        self.glGetShaderiv(shader, 0x8B81, ctypes.byref(status))
        if not status.value:
            log = ctypes.create_string_buffer(4096)
            self.glGetShaderInfoLog(shader, 4096, None, log)
            self.glDeleteShader(shader)
            raise ValueError(f'Compute Shader Error in "{name}"\n\n{log.value.decode()}')

        program = self.glCreateProgram()
        self.glAttachShader(program, shader)
        self.glLinkProgram(program)
        self.glDeleteShader(shader)
        self.glGetProgramiv(program, 0x8B82, ctypes.byref(status))
        if not status.value:
            log = ctypes.create_string_buffer(4096)
            self.glGetProgramInfoLog(program, 4096, None, log)
            self.glDeleteProgram(program)
            raise ValueError(f'Linker Error in "{name}"\n\n{log.value.decode()}')

        return ComputeProgram(self, program)


class ComputeProgram:
    def __init__(self, compute, glo):
        self.compute = compute
        self.glo = glo
        self.locations = {}

    def location(self, name):
        if name not in self.locations:
            self.locations[name] = self.compute.glGetUniformLocation(self.glo, name.encode())
        return self.locations[name]

    def __setitem__(self, name, value):
        c = self.compute
        if isinstance(value, int):
            c.glProgramUniform1i(self.glo, self.location(name), value)
        elif isinstance(value, float):
            c.glProgramUniform1f(self.glo, self.location(name), value)
        else:
            [None, c.glProgramUniform1f, c.glProgramUniform2f, c.glProgramUniform3f, c.glProgramUniform4f][len(value)](self.glo, self.location(name), *value)

    def run(self, groups, storage_buffers=None, barrier=0xFFFFFFFF):
        c = self.compute
        previous = ctypes.c_int32(0)
        c.glGetIntegerv(0x8B8D, ctypes.byref(previous))
        for binding, buffer in (storage_buffers or {}).items():
            c.glBindBufferBase(0x90D2, binding, buffer.glo)
        c.glUseProgram(self.glo)
        c.glDispatchCompute(*groups)
        c.glUseProgram(previous.value)
        c.glMemoryBarrier(barrier)

    def release(self):
        self.compute.glDeleteProgram(self.glo)


pygame.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 4)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 6)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()
compute = Compute(ctx)

count = 1 << 18

ctx.shaders['particle'] = '''
    struct Particle {
        vec4 position;
        vec4 velocity;
    };
'''

ctx.shaders['update.comp'] = '''
    #version 430 core
    #include "particle"

    layout (local_size_x = 256) in;

    layout (std430, binding = 0) buffer Particles {
        Particle particles[];
    };

    uniform vec2 attractor;
    uniform float strength;
    uniform float dt;

    void main() {
        Particle p = particles[gl_GlobalInvocationID.x];
        vec2 offset = attractor - p.position.xy;
        float distance = max(length(offset), 0.05);
        p.velocity.xy += offset / (distance * distance * distance) * strength * dt;
        p.velocity.xy *= pow(0.4, dt);
        p.position.xy += p.velocity.xy * dt;
        particles[gl_GlobalInvocationID.x] = p;
    }
'''

ctx.shaders['particle.vert'] = '''
    #version 330 core

    uniform vec2 scale;

    layout (location = 0) in vec4 in_position;
    layout (location = 1) in vec4 in_velocity;

    out vec3 v_color;

    void main() {
        gl_Position = vec4(in_position.xy * scale, 0.0, 1.0);
        gl_PointSize = 1.5;
        float speed = length(in_velocity.xy);
        v_color = mix(vec3(0.1, 0.3, 1.0), vec3(1.0, 0.6, 0.2), clamp(speed * 0.5, 0.0, 1.0)) * 0.15;
    }
'''

ctx.shaders['particle.frag'] = '''
    #version 330 core

    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(v_color, 1.0);
    }
'''

particles = bytearray()
for i in range(count):
    angle = random.uniform(0.0, math.tau)
    radius = random.uniform(0.2, 1.0)
    x, y = math.cos(angle) * radius, math.sin(angle) * radius
    particles += struct.pack('8f', x, y, 0.0, 1.0, -y * 0.8, x * 0.8, 0.0, 0.0)

particle_buffer = ctx.buffer(particles)
vertex_array = ctx.vertex_array(particle_buffer.bind('4f 4f', 0, 1), mode='points')

size = pygame.display.get_window_size()
image = ctx.texture(size, 'rgba16float')
framebuffer = ctx.framebuffer([image])
framebuffer.clear_color = (0.0, 0.0, 0.0, 1.0)

update = compute.program('update.comp')

render = ctx.pipeline(
    'particle.vert',
    'particle.frag',
    uniforms={'scale': 'vec2'},
    vertex_array=vertex_array,
    framebuffer=framebuffer,
    blend={'src': 'one', 'dst': 'one'},
)
render['scale'] = (size[1] / size[0], 1.0)

output = ctx.texture(size, 'rgba8unorm')
clock = pygame.time.Clock()

print(f'{count} particles, mouse: attractor, hold left button: stronger, escape: quit')

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT or event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
            pygame.quit()
            sys.exit()

    dt = min(clock.tick() / 1000.0, 1.0 / 30.0)
    mx, my = pygame.mouse.get_pos()
    attractor = ((mx / size[0] * 2.0 - 1.0) * size[0] / size[1], 1.0 - my / size[1] * 2.0)

    update['attractor'] = attractor
    update['strength'] = 0.6 if pygame.mouse.get_pressed()[0] else 0.15
    update['dt'] = dt

    ctx.start_frame()
    update.run((count // 256, 1, 1), storage_buffers={0: particle_buffer})
    framebuffer.clear()
    render.run()
    ctx.copy_texture(image, output)
    ctx.present(output)
    pygame.display.flip()
