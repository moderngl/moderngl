import ctypes
import random
import struct
import sys

import moderngl
import pygame

GLAPI = ctypes.WINFUNCTYPE if sys.platform == 'win32' else ctypes.CFUNCTYPE

IMAGE_FORMATS = {
    'r32float': 0x822E,
    'rg32float': 0x8230,
    'rgba32float': 0x8814,
    'rgba16float': 0x881A,
    'rgba8unorm': 0x8058,
    'r32uint': 0x8236,
    'r32sint': 0x8235,
    'rgba32uint': 0x8D70,
}

ACCESS = {'read': 0x88B8, 'write': 0x88B9, 'read_write': 0x88BA}


class Images:
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
        self.glUseProgram = load('glUseProgram', None, u32)
        self.glGetUniformLocation = load('glGetUniformLocation', i32, u32, ctypes.c_char_p)
        self.glProgramUniform1i = load('glProgramUniform1i', None, u32, i32, i32)
        self.glProgramUniform1f = load('glProgramUniform1f', None, u32, i32, f32)
        self.glProgramUniform2f = load('glProgramUniform2f', None, u32, i32, f32, f32)
        self.glBindImageTexture = load('glBindImageTexture', None, u32, u32, i32, ctypes.c_uint8, i32, u32, u32)
        self.glDispatchCompute = load('glDispatchCompute', None, u32, u32, u32)
        self.glMemoryBarrier = load('glMemoryBarrier', None, u32)

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

    def uniform(self, program, name, value):
        location = self.glGetUniformLocation(program, name.encode())
        if isinstance(value, int):
            self.glProgramUniform1i(program, location, value)
        elif isinstance(value, float):
            self.glProgramUniform1f(program, location, value)
        else:
            self.glProgramUniform2f(program, location, *value)

    def bind(self, unit, texture, access, level=0, layer=None):
        layered = layer is None and (texture.array > 0 or texture.cubemap)
        self.glBindImageTexture(unit, texture.glo, level, layered, layer or 0, ACCESS[access], IMAGE_FORMATS[texture.format])

    def dispatch(self, program, groups):
        previous = ctypes.c_int32(0)
        self.glGetIntegerv(0x8B8D, ctypes.byref(previous))
        self.glUseProgram(program)
        self.glDispatchCompute(*groups)
        self.glUseProgram(previous.value)
        self.glMemoryBarrier(0x8 | 0x20 | 0x100)


pygame.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 4)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 6)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
pygame.display.set_mode((1024, 1024), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()
images = Images(ctx)

ctx.shaders['gray_scott.comp'] = '''
    #version 430 core

    layout (local_size_x = 16, local_size_y = 16) in;

    layout (rg32f, binding = 0) readonly uniform image2D state_in;
    layout (rg32f, binding = 1) writeonly uniform image2D state_out;

    uniform vec2 brush;
    uniform float brush_size;
    uniform float feed;
    uniform float kill;

    void main() {
        ivec2 size = imageSize(state_in);
        ivec2 p = ivec2(gl_GlobalInvocationID.xy);
        vec2 c = imageLoad(state_in, p).xy;
        vec2 laplace = -c;
        laplace += 0.2 * imageLoad(state_in, (p + ivec2(1, 0) + size) % size).xy;
        laplace += 0.2 * imageLoad(state_in, (p + ivec2(-1, 0) + size) % size).xy;
        laplace += 0.2 * imageLoad(state_in, (p + ivec2(0, 1) + size) % size).xy;
        laplace += 0.2 * imageLoad(state_in, (p + ivec2(0, -1) + size) % size).xy;
        laplace += 0.05 * imageLoad(state_in, (p + ivec2(1, 1) + size) % size).xy;
        laplace += 0.05 * imageLoad(state_in, (p + ivec2(-1, 1) + size) % size).xy;
        laplace += 0.05 * imageLoad(state_in, (p + ivec2(1, -1) + size) % size).xy;
        laplace += 0.05 * imageLoad(state_in, (p + ivec2(-1, -1) + size) % size).xy;
        float reaction = c.x * c.y * c.y;
        float a = c.x + 1.0 * laplace.x - reaction + feed * (1.0 - c.x);
        float b = c.y + 0.5 * laplace.y + reaction - (kill + feed) * c.y;
        if (distance(vec2(p), brush) < brush_size) {
            b = 0.9;
        }
        imageStore(state_out, p, vec4(clamp(a, 0.0, 1.0), clamp(b, 0.0, 1.0), 0.0, 0.0));
    }
'''

ctx.shaders['fullscreen.vert'] = '''
    #version 330 core

    vec2 vertices[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2(3.0, -1.0),
        vec2(-1.0, 3.0)
    );

    out vec2 v_uv;

    void main() {
        gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
        v_uv = vertices[gl_VertexID] * 0.5 + 0.5;
    }
'''

ctx.shaders['display.frag'] = '''
    #version 330 core

    uniform sampler2D State;

    in vec2 v_uv;

    layout (location = 0) out vec4 out_color;

    void main() {
        float b = texture(State, v_uv).y;
        float t = smoothstep(0.1, 0.4, b);
        vec3 color = mix(vec3(0.02, 0.03, 0.08), vec3(0.95, 0.75, 0.35), t);
        color = mix(color, vec3(1.0), smoothstep(0.35, 0.5, b));
        out_color = vec4(color, 1.0);
    }
'''

grid = 512
data = bytearray(struct.pack('2f', 1.0, 0.0) * grid * grid)
for _ in range(40):
    cx, cy = random.randrange(grid), random.randrange(grid)
    for y in range(cy - 4, cy + 4):
        for x in range(cx - 4, cx + 4):
            struct.pack_into('2f', data, ((y % grid) * grid + x % grid) * 8, 0.5, 0.5)

states = [ctx.texture((grid, grid), 'rg32float', data), ctx.texture((grid, grid), 'rg32float')]
samplers = [ctx.sampler(state, min_filter='linear', mag_filter='linear') for state in states]

step = images.compute_program('gray_scott.comp')
images.uniform(step, 'feed', 0.037)
images.uniform(step, 'kill', 0.06)
images.uniform(step, 'brush_size', 6.0)

display = ctx.program('fullscreen.vert', 'display.frag', samplers={'State': 0})

size = pygame.display.get_window_size()
clock = pygame.time.Clock()
current = 0

print('mouse: draw, 1-4: patterns, escape: quit')
patterns = {pygame.K_1: (0.037, 0.06), pygame.K_2: (0.03, 0.062), pygame.K_3: (0.025, 0.06), pygame.K_4: (0.078, 0.061)}

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT or event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
            pygame.quit()
            sys.exit()
        if event.type == pygame.KEYDOWN and event.key in patterns:
            feed, kill = patterns[event.key]
            images.uniform(step, 'feed', feed)
            images.uniform(step, 'kill', kill)

    mx, my = pygame.mouse.get_pos()
    brush = (mx / size[0] * grid, (1.0 - my / size[1]) * grid) if pygame.mouse.get_pressed()[0] else (-100.0, -100.0)
    images.uniform(step, 'brush', brush)

    ctx.start_frame()
    for _ in range(16):
        images.bind(0, states[current], 'read')
        images.bind(1, states[1 - current], 'write')
        images.dispatch(step, (grid // 16, grid // 16, 1))
        current = 1 - current

    ctx.screen.use()
    display.use()
    samplers[current].use(0)
    ctx.empty_vertex_array.render(vertices=3)
    ctx.end_frame()
    pygame.display.flip()
    clock.tick()
