import math
import os
import struct
import sys

import glm
import moderngl
import pygame

sys.path.insert(0, 'examples')
from glb import load_glb

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'
pygame.display.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, 1)

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()

size = pygame.display.get_window_size()
image = ctx.texture(size, 'rgba8unorm', multisampled=True)
depth = ctx.texture(size, 'depth24plus', multisampled=True)
framebuffer = ctx.framebuffer([image, depth])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

scene = load_glb('examples/data/crate/crate.glb')
vertex_buffer = ctx.buffer(scene['models']['crate'])

img = scene['textures']['crate'].convert('RGBA')
texture = ctx.texture(img.size, 'rgba8unorm', img.tobytes())

uniform_buffer = ctx.buffer(size=80)

ctx.shaders['crate.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;
    layout (location = 2) in vec2 in_uv;

    out vec3 v_vertex;
    out vec3 v_normal;
    out vec2 v_uv;

    void main() {
        gl_Position = camera * vec4(in_vertex, 1.0);
        v_vertex = in_vertex;
        v_normal = in_normal;
        v_uv = in_uv;
    }
'''

ctx.shaders['crate.frag'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    uniform sampler2D Texture;

    in vec3 v_vertex;
    in vec3 v_normal;
    in vec2 v_uv;

    layout (location = 0) out vec4 out_color;

    void main() {
        float lum = clamp(dot(normalize(camera_position.xyz - v_vertex), normalize(v_normal)), 0.0, 1.0) * 0.6 + 0.4;
        vec3 color = pow(texture(Texture, v_uv).rgb, vec3(2.2)) * lum;
        out_color = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
    }
'''

pipeline = ctx.pipeline(
    'crate.vert',
    'crate.frag',
    samplers={'Texture': 0},
    uniform_buffers={'Common': 0},
    vertex_array=ctx.vertex_array(vertex_buffer.bind('3f 3f 2f', 0, 1, 2)),
    framebuffer=framebuffer,
    resources=ctx.resources(
        samplers={0: ctx.sampler(texture)},
        uniform_buffers={0: uniform_buffer},
    ),
    depth_test=True,
    cull_face='back',
)

yaw, pitch, spin = 0.0, 0.46, 0.2

running = True
while running:
    if spin:
        yaw = pygame.time.get_ticks() / 1000.0 * spin
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.MOUSEMOTION and event.buttons[0]:
            spin = 0.0
            yaw -= event.rel[0] * 0.004
            pitch = min(max(pitch + event.rel[1] * 0.004, 0.05), 1.4)

    eye = (math.cos(yaw) * math.cos(pitch) * 3.354, math.sin(yaw) * math.cos(pitch) * 3.354, math.sin(pitch) * 3.354)
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.5), (0.0, 0.0, 1.0))

    ctx.start_frame()
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x', *eye))
    framebuffer.clear()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
