import ctypes
import sys
from contextlib import contextmanager

import moderngl
import pygame

GLAPI = ctypes.WINFUNCTYPE if sys.platform == 'win32' else ctypes.CFUNCTYPE


class Queries:
    def __init__(self, smoothing=0.9):
        def load(name, *signature):
            return GLAPI(*signature)(moderngl.get_proc_address(name))

        u32, i32 = ctypes.c_uint32, ctypes.c_int32
        self.glGenQueries = load('glGenQueries', None, i32, ctypes.POINTER(u32))
        self.glQueryCounter = load('glQueryCounter', None, u32, u32)
        self.glBeginQuery = load('glBeginQuery', None, u32, u32)
        self.glEndQuery = load('glEndQuery', None, u32)
        self.glGetQueryObjectuiv = load('glGetQueryObjectuiv', None, u32, u32, ctypes.POINTER(u32))
        self.glGetQueryObjectui64v = load('glGetQueryObjectui64v', None, u32, u32, ctypes.POINTER(ctypes.c_uint64))

        self.smoothing = smoothing
        self.free = []
        self.recording = []
        self.pending = []
        self.times = {}
        self.samples = {}

    def query(self):
        if not self.free:
            query = ctypes.c_uint32(0)
            self.glGenQueries(1, ctypes.byref(query))
            return query.value
        return self.free.pop()

    @contextmanager
    def time(self, name):
        start, end = self.query(), self.query()
        self.glQueryCounter(start, 0x8E28)
        try:
            yield
        finally:
            self.glQueryCounter(end, 0x8E28)
            self.recording.append(('time', name, start, end))

    @contextmanager
    def count_samples(self, name):
        query = self.query()
        self.glBeginQuery(0x8914, query)
        try:
            yield
        finally:
            self.glEndQuery(0x8914)
            self.recording.append(('samples', name, query, query))

    def available(self, query):
        res = ctypes.c_uint32(0)
        self.glGetQueryObjectuiv(query, 0x8867, ctypes.byref(res))
        return res.value

    def result(self, query):
        res = ctypes.c_uint64(0)
        self.glGetQueryObjectui64v(query, 0x8866, ctypes.byref(res))
        return res.value

    def end_frame(self):
        self.pending.append(self.recording)
        self.recording = []
        while self.pending and all(self.available(last) for kind, name, first, last in self.pending[0]):
            for kind, name, first, last in self.pending.pop(0):
                if kind == 'time':
                    ms = (self.result(last) - self.result(first)) / 1e6
                    self.times[name] = self.times.get(name, ms) * self.smoothing + ms * (1.0 - self.smoothing)
                    self.free += [first, last]
                else:
                    self.samples[name] = self.result(first)
                    self.free.append(first)


pygame.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
pygame.display.set_mode((800, 600), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()
queries = Queries()

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

ctx.shaders['heavy.frag'] = '''
    #version 330 core

    uniform int iterations;
    uniform float time;

    in vec2 v_uv;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec2 z = (v_uv - 0.5) * 3.0;
        vec2 c = vec2(cos(time * 0.3), sin(time * 0.2)) * 0.7885;
        int i = 0;
        for (; i < iterations && dot(z, z) < 4.0; ++i) {
            z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
        }
        float t = float(i) / float(iterations);
        out_color = vec4(t, t * t, sqrt(t), 1.0);
    }
'''

ctx.shaders['triangle.vert'] = '''
    #version 330 core

    vec2 vertices[3] = vec2[](
        vec2(0.0, 0.5),
        vec2(-0.4, -0.5),
        vec2(0.4, -0.5)
    );

    void main() {
        gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
    }
'''

ctx.shaders['triangle.frag'] = '''
    #version 330 core

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(1.0, 1.0, 1.0, 0.5);
    }
'''

size = pygame.display.get_window_size()
image = ctx.texture(size, 'rgba8unorm')
framebuffer = ctx.framebuffer([image])

background = ctx.pipeline(
    'fullscreen.vert',
    'heavy.frag',
    uniforms={'iterations': 'int', 'time': 'float'},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=framebuffer,
)

triangle = ctx.pipeline(
    'triangle.vert',
    'triangle.frag',
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=framebuffer,
    blend={'src': 'src_alpha', 'dst': 'one_minus_src_alpha'},
)

iterations = 64
clock = pygame.time.Clock()
time = 0.0
report = 0.0

print('up/down: shader cost, escape: quit')

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT or event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
            pygame.quit()
            sys.exit()
        if event.type == pygame.KEYDOWN and event.key == pygame.K_UP:
            iterations = min(iterations * 2, 4096)
        if event.type == pygame.KEYDOWN and event.key == pygame.K_DOWN:
            iterations = max(iterations // 2, 1)

    dt = clock.tick() / 1000.0
    time += dt

    ctx.start_frame()
    with queries.time('frame'):
        with queries.time('background'):
            background['iterations'] = iterations
            background['time'] = time
            background.run()
        with queries.time('triangle'), queries.count_samples('triangle'):
            triangle.run()
        with queries.time('present'):
            ctx.present(image)
    queries.end_frame()

    report += dt
    if report > 0.5:
        report = 0.0
        times = ' '.join(f'{name} {ms:.2f}ms' for name, ms in queries.times.items())
        pixels = queries.samples.get('triangle', 0)
        print(f'iterations {iterations} | {times} | triangle pixels {pixels}')

    pygame.display.flip()
