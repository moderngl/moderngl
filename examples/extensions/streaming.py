import ctypes
import subprocess
import sys

import moderngl
import pygame

GLAPI = ctypes.WINFUNCTYPE if sys.platform == 'win32' else ctypes.CFUNCTYPE


class Streaming:
    def __init__(self, ctx):
        def load(name, *signature):
            return GLAPI(*signature)(moderngl.get_proc_address(name))

        u32, i32, ptr, size = ctypes.c_uint32, ctypes.c_int32, ctypes.c_void_p, ctypes.c_ssize_t
        self.ctx = ctx
        self.glCreateBuffers = load('glCreateBuffers', None, i32, ctypes.POINTER(u32))
        self.glNamedBufferStorage = load('glNamedBufferStorage', None, u32, size, ptr, u32)
        self.glMapNamedBufferRange = load('glMapNamedBufferRange', ptr, u32, size, size, u32)
        self.glUnmapNamedBuffer = load('glUnmapNamedBuffer', ctypes.c_uint8, u32)
        self.glDeleteBuffers = load('glDeleteBuffers', None, i32, ctypes.POINTER(u32))
        self.glFenceSync = load('glFenceSync', ptr, u32, u32)
        self.glClientWaitSync = load('glClientWaitSync', u32, ptr, u32, ctypes.c_uint64)
        self.glDeleteSync = load('glDeleteSync', None, ptr)

    def read_buffer(self, size):
        glo = ctypes.c_uint32(0)
        self.glCreateBuffers(1, ctypes.byref(glo))
        flags = 0x1 | 0x40 | 0x80 | 0x200
        self.glNamedBufferStorage(glo, size, None, flags)
        address = self.glMapNamedBufferRange(glo, 0, size, 0x1 | 0x40 | 0x80)
        memory = memoryview((ctypes.c_char * size).from_address(address)).cast('B')
        return self.ctx.external_buffer(glo.value, size), memory

    def release_buffer(self, buffer):
        glo = ctypes.c_uint32(buffer.glo)
        self.glUnmapNamedBuffer(glo)
        self.glDeleteBuffers(1, ctypes.byref(glo))

    def fence(self):
        return self.glFenceSync(0x9117, 0)

    def wait(self, fence, timeout=0):
        res = self.glClientWaitSync(fence, 0x1, timeout)
        if res in (0x911A, 0x911C):
            self.glDeleteSync(fence)
            return True
        return False


class Readback:
    def __init__(self, streaming, size, frames=3):
        self.streaming = streaming
        self.slots = [streaming.read_buffer(size) for _ in range(frames)]
        self.fences = [None] * frames
        self.index = 0
        self.stalls = 0

    def push(self, texture):
        buffer, memory = self.slots[self.index]
        ready = None
        if self.fences[self.index]:
            if not self.streaming.wait(self.fences[self.index]):
                self.stalls += 1
                self.streaming.wait(self.fences[self.index], 1_000_000_000)
            ready = memory
        self.streaming.ctx.copy_texture_to_buffer(texture, buffer)
        self.fences[self.index] = self.streaming.fence()
        self.index = (self.index + 1) % len(self.slots)
        return ready

    def drain(self):
        for _ in range(len(self.slots)):
            fence = self.fences[self.index]
            if fence:
                self.streaming.wait(fence, 1_000_000_000)
                yield self.slots[self.index][1]
            self.fences[self.index] = None
            self.index = (self.index + 1) % len(self.slots)

    def release(self):
        for buffer, memory in self.slots:
            memory.release()
            self.streaming.release_buffer(buffer)


pygame.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 4)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 6)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()
streaming = Streaming(ctx)

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

ctx.shaders['plasma.frag'] = '''
    #version 330 core

    uniform float time;
    uniform vec2 aspect;

    in vec2 v_uv;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec2 p = (v_uv - 0.5) * aspect * 6.0;
        float v = sin(p.x + time) + sin(p.y * 1.3 - time * 0.7) + sin(length(p + vec2(sin(time), cos(time))) * 2.0);
        vec3 color = 0.5 + 0.5 * cos(v + vec3(0.0, 2.1, 4.2) + time * 0.3);
        float marker = step(abs(v_uv.y - 0.9), 0.02) * step(v_uv.x, 0.1);
        out_color = vec4(mix(color, vec3(1.0), marker), 1.0);
    }
'''

size = pygame.display.get_window_size()
image = ctx.texture(size, 'rgba8unorm')
frame = ctx.texture(size, 'bgra8unorm')
framebuffer = ctx.framebuffer([image])

plasma = ctx.pipeline(
    'fullscreen.vert',
    'plasma.frag',
    uniforms={'time': 'float', 'aspect': 'vec2'},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=framebuffer,
)
plasma['aspect'] = (size[0] / size[1], 1.0)

readback = Readback(streaming, size[0] * size[1] * 4)

encoder = subprocess.Popen(
    [
        'ffmpeg', '-y', '-loglevel', 'error',
        '-f', 'rawvideo', '-pix_fmt', 'bgra', '-s', f'{size[0]}x{size[1]}', '-r', '60', '-i', '-',
        '-c:v', 'libx264', '-preset', 'veryfast', '-crf', '18', '-pix_fmt', 'yuv420p', 'temp_streaming.mkv',
    ],
    stdin=subprocess.PIPE,
)

clock = pygame.time.Clock()
frames = 0
report = 0.0

print('recording to temp_streaming.mkv, the marker is top left, escape: stop')

while True:
    events = pygame.event.get()
    if any(event.type == pygame.QUIT or event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE for event in events):
        break

    dt = clock.tick() / 1000.0

    ctx.start_frame()
    plasma['time'] = frames / 60.0
    plasma.run()
    ctx.copy_texture(image, frame, flip_y=True)

    ready = readback.push(frame)
    if ready is not None:
        encoder.stdin.write(ready)

    ctx.present(image)
    pygame.display.flip()
    frames += 1

    report += dt
    if report > 1.0:
        report = 0.0
        print(f'{frames} frames, {clock.get_fps():.0f} fps, {readback.stalls} stalls')

for ready in readback.drain():
    encoder.stdin.write(ready)

readback.release()
encoder.stdin.close()
encoder.wait()
pygame.quit()
print(f'wrote {frames} frames to temp_streaming.mkv')
