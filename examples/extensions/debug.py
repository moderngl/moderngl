import ctypes
import sys
from contextlib import contextmanager

import moderngl
import pygame

GLAPI = ctypes.WINFUNCTYPE if sys.platform == 'win32' else ctypes.CFUNCTYPE

CALLBACK = GLAPI(None, ctypes.c_uint32, ctypes.c_uint32, ctypes.c_uint32, ctypes.c_uint32, ctypes.c_int32, ctypes.c_char_p, ctypes.c_void_p)

SOURCES = {0x8246: 'api', 0x8247: 'window', 0x8248: 'compiler', 0x8249: 'third_party', 0x824A: 'application', 0x824B: 'other'}
TYPES = {0x824C: 'error', 0x824D: 'deprecated', 0x824E: 'undefined', 0x824F: 'portability', 0x8250: 'performance', 0x8251: 'other', 0x8268: 'marker', 0x8269: 'push', 0x826A: 'pop'}
SEVERITIES = {0x9146: 'high', 0x9147: 'medium', 0x9148: 'low', 0x826B: 'notification'}


class Debug:
    def __init__(self, callback=None):
        def load(name, *signature):
            return GLAPI(*signature)(moderngl.get_proc_address(name))

        u32, i32, ptr = ctypes.c_uint32, ctypes.c_int32, ctypes.c_void_p
        self.glEnable = load('glEnable', None, u32)
        self.glGetIntegerv = load('glGetIntegerv', None, u32, ctypes.POINTER(i32))
        self.glDebugMessageCallback = load('glDebugMessageCallback', None, CALLBACK, ptr)
        self.glDebugMessageControl = load('glDebugMessageControl', None, u32, u32, u32, i32, ptr, ctypes.c_uint8)
        self.glDebugMessageInsert = load('glDebugMessageInsert', None, u32, u32, u32, u32, i32, ctypes.c_char_p)
        self.glObjectLabel = load('glObjectLabel', None, u32, u32, i32, ctypes.c_char_p)
        self.glPushDebugGroup = load('glPushDebugGroup', None, u32, u32, i32, ctypes.c_char_p)
        self.glPopDebugGroup = load('glPopDebugGroup', None)

        flags = i32(0)
        self.glGetIntegerv(0x821E, ctypes.byref(flags))
        self.debug_context = bool(flags.value & 0x2)

        self.callback = callback or self.print_message
        self.c_callback = CALLBACK(self.receive)
        self.glEnable(0x92E0)
        self.glEnable(0x8242)
        self.glDebugMessageCallback(self.c_callback, None)
        self.glDebugMessageControl(0x1100, 0x1100, 0x1100, 0, None, 1)
        self.glDebugMessageControl(0x1100, 0x8269, 0x1100, 0, None, 0)
        self.glDebugMessageControl(0x1100, 0x826A, 0x1100, 0, None, 0)

    def receive(self, source, kind, id, severity, length, message, user):
        self.callback(SOURCES.get(source, source), TYPES.get(kind, kind), id, SEVERITIES.get(severity, severity), message[:length].decode(errors='replace'))

    def print_message(self, source, kind, id, severity, message):
        print(f'[{severity}] {source} {kind} {id}: {message}')

    def label(self, obj, name):
        identifier = {
            moderngl.Buffer: 0x82E0,
            moderngl.Program: 0x82E2,
            moderngl.VertexArray: 0x8074,
            moderngl.Sampler: 0x82E6,
            moderngl.Framebuffer: 0x8D40,
        }.get(type(obj))
        if isinstance(obj, moderngl.Texture):
            identifier = 0x8D41 if obj.renderbuffer else 0x1702
        raw = name.encode()
        self.glObjectLabel(identifier, obj.glo, len(raw), raw)

    def insert(self, message):
        raw = message.encode()
        self.glDebugMessageInsert(0x824A, 0x8268, 0, 0x826B, len(raw), raw)

    @contextmanager
    def group(self, name):
        raw = name.encode()
        self.glPushDebugGroup(0x824A, 0, len(raw), raw)
        try:
            yield
        finally:
            self.glPopDebugGroup()


pygame.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 4)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 6)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_FLAGS, pygame.GL_CONTEXT_DEBUG_FLAG)
pygame.display.set_mode((800, 600), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()
debug = Debug()

print(ctx.info['version'], '| debug context:', debug.debug_context)
debug.insert('hello from test.py')

ctx.shaders['triangle.vert'] = '''
    #version 330 core

    vec2 vertices[3] = vec2[](
        vec2(0.0, 0.8),
        vec2(-0.6, -0.8),
        vec2(0.6, -0.8)
    );

    void main() {
        gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
    }
'''

ctx.shaders['triangle.frag'] = '''
    #version 330 core

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(0.2, 0.6, 1.0, 1.0);
    }
'''

image = ctx.texture((800, 600), 'rgba8unorm', samples=4)
output = ctx.texture((800, 600), 'rgba8unorm')
framebuffer = ctx.framebuffer([image])
framebuffer.clear_color = (0.1, 0.1, 0.1, 1.0)

pipeline = ctx.pipeline(
    'triangle.vert',
    'triangle.frag',
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=framebuffer,
)

debug.label(image, 'msaa color')
debug.label(output, 'resolved color')
debug.label(framebuffer, 'main framebuffer')
debug.label(pipeline.program, 'triangle program')

print('space: make an invalid OpenGL call, escape: quit')

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT or event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
            pygame.quit()
            sys.exit()
        if event.type == pygame.KEYDOWN and event.key == pygame.K_SPACE:
            debug.glEnable(0xDEAD)

    ctx.start_frame()
    with debug.group('triangle pass'):
        framebuffer.clear()
        pipeline.run()
    with debug.group('present'):
        ctx.copy_texture(image, output)
        ctx.present(output)
    pygame.display.flip()
