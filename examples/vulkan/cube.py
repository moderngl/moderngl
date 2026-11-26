import math
import os
import struct
import subprocess
import tempfile

import moderngl
import pygame
import swapchain


def spirv(source, stage):
    with tempfile.TemporaryDirectory() as tmp:
        path = os.path.join(tmp, 'shader.' + stage)
        open(path, 'w').write(source)
        subprocess.run(['glslangValidator', '-V', path, '-o', path + '.spv'], check=True, capture_output=True)
        return open(path + '.spv', 'rb').read()


os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'
pygame.init()
pygame.display.set_mode((1280, 720), vsync=True)

size = pygame.display.get_window_size()



class Loader:
    backend = 'vulkan'
    size = size
    instance_extensions = swapchain.instance_extensions()
    device_extensions = ['VK_KHR_swapchain']


moderngl.init(Loader())
ctx = moderngl.get_context()
print(ctx.info['renderer'])
window = swapchain.Swapchain(ctx, size)

ctx.shaders['cube.vert'] = spirv('''
    #version 450

    layout (set = 0, binding = 0) uniform Common {
        mat4 camera;
    };

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;

    layout (location = 0) out vec3 v_normal;

    void main() {
        gl_Position = camera * vec4(in_vertex, 1.0);
        v_normal = in_normal;
    }
''', 'vert')

ctx.shaders['cube.frag'] = spirv('''
    #version 450

    layout (location = 0) in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 light = normalize(vec3(4.0, 3.0, 10.0));
        float lum = 0.3 + 0.7 * max(dot(normalize(v_normal), light), 0.0);
        out_color = vec4(pow((abs(v_normal) * 0.6 + 0.4) * lum, vec3(1.0 / 2.2)), 1.0);
    }
''', 'frag')

vertices = bytearray()
for axis in range(3):
    for sign in (-1.0, 1.0):
        normal = [0.0, 0.0, 0.0]
        normal[axis] = sign
        u = [0.0, 0.0, 0.0]
        v = [0.0, 0.0, 0.0]
        u[(axis + 1) % 3] = 1.0
        v[(axis + 2) % 3] = sign
        for a, b in ((-1, -1), (1, -1), (1, 1), (-1, -1), (1, 1), (-1, 1)):
            position = [normal[i] + u[i] * a + v[i] * b for i in range(3)]
            vertices += struct.pack('6f', *position, *normal)

vertex_buffer = ctx.buffer(vertices)
camera_buffer = ctx.buffer(size=64)

image = ctx.texture(size, 'rgba8unorm', multisampled=True)
depth = ctx.texture(size, 'depth24plus', multisampled=True)
output = ctx.texture(size, 'rgba8unorm')
framebuffer = ctx.framebuffer([image, depth])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

cube = ctx.pipeline(
    'cube.vert',
    'cube.frag',
    vertex_array=ctx.vertex_array(vertex_buffer.bind('3f 3f', 0, 1)),
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: camera_buffer}),
    depth_test=True,
    cull_face='back',
)

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    t = pygame.time.get_ticks() / 1000.0
    eye = (math.cos(t * 0.5) * 5.0, math.sin(t * 0.5) * 5.0, 3.0)
    camera_buffer.write(moderngl.camera(eye, aspect=size[0] / size[1], fov=45.0))

    ctx.start_frame()
    framebuffer.clear()
    cube.run()
    ctx.copy_texture(image, output)
    window.present(output)

pygame.quit()
