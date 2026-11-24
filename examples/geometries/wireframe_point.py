import math
import os
import struct

import glm
import moderngl
import pygame

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'

pygame.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, 1)
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()

size = pygame.display.get_window_size()
image = ctx.texture(size, 'rgba8unorm', multisampled=True)
depth = ctx.texture(size, 'depth24plus', multisampled=True)
framebuffer = ctx.framebuffer([image, depth])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

uniform_buffer = ctx.buffer(size=80)

ctx.shaders['wireframe_point.vert'] = '''
    #version 330 core

    const float pi = 3.14159265358979323;

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    out vec3 v_vertex;

    void main() {
        int axis = gl_VertexID / 64;
        float f = (float(gl_VertexID % 64) - 0.5) / 64.0;
        vec3 vertex = vec3(cos(f * pi * 2.0), sin(f * pi * 2.0), 0.0);
        switch (axis) {
            case 0: vertex = vertex.xyz; break;
            case 1: vertex = vertex.xzy; break;
            case 2: vertex = vertex.zxy; break;
        }
        gl_Position = camera * vec4(vertex, 1.0);
    }
'''

ctx.shaders['wireframe_point.frag'] = '''
    #version 330 core

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(1.0, 1.0, 1.0, 1.0);
    }
'''

pipeline = ctx.pipeline(
    'wireframe_point.vert',
    'wireframe_point.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.empty_vertex_array,
    mode='lines',
    vertices=192,
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    depth_test=True,
)

yaw, pitch, spin = 0.64, 0.38, 0.3

running = True
while running:
    if spin:
        yaw = pygame.time.get_ticks() / 1000.0 * spin + 0.64
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.MOUSEMOTION and event.buttons[0]:
            spin = 0.0
            yaw -= event.rel[0] * 0.004
            pitch = min(max(pitch + event.rel[1] * 0.004, -1.4), 1.4)

    eye = (math.cos(yaw) * math.cos(pitch) * 3.5, math.sin(yaw) * math.cos(pitch) * 3.5, math.sin(pitch) * 3.5)

    ctx.start_frame()
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x', *eye))
    framebuffer.clear()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
