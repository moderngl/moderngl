import math
import os
import struct
import sys
from colorsys import hls_to_rgb

import glm
import moderngl
import numpy as np
import pygame
from PIL import Image, ImageDraw, ImageFont

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

vertex_buffer = ctx.buffer(load_glb('examples/data/crate/crate.glb')['models']['crate'])

count = 30 * 30
instance_buffer = ctx.buffer(np.array([
    np.repeat(np.linspace(-20.0, 20.0, 30), 30),
    np.tile(np.linspace(-20.0, 20.0, 30), 30),
    np.random.normal(-0.5, 0.2, count),
    np.random.randint(0, 10, count),
]).T.astype('f4'))

font = ImageFont.truetype('examples/data/font/OpenSans-Medium.ttf', 64)

texture = ctx.texture((128, 128), 'rgba8unorm', array=10, levels=8)
for i in range(10):
    r, g, b = hls_to_rgb(i / 10, 0.6, 0.6)
    img = Image.new('RGBA', (128, 128), (int(r * 255), int(g * 255), int(b * 255), 255))
    ImageDraw.Draw(img).text((64, 64), f'{i + 1}', (0, 0, 0, 255), font=font, anchor='mm')
    texture.write(img.transpose(Image.FLIP_TOP_BOTTOM).tobytes(), layer=i)
texture.mipmaps()

uniform_buffer = ctx.buffer(size=80)

ctx.shaders['texture_array.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;
    layout (location = 2) in vec2 in_uv;
    layout (location = 3) in vec3 in_position;
    layout (location = 4) in float in_layer;

    out vec3 v_vertex;
    out vec3 v_normal;
    out vec3 v_uv;

    void main() {
        v_vertex = in_position + in_vertex;
        gl_Position = camera * vec4(v_vertex, 1.0);
        v_normal = in_normal;
        v_uv = vec3(in_uv, in_layer);
    }
'''

ctx.shaders['texture_array.frag'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    uniform sampler2DArray Texture;

    in vec3 v_vertex;
    in vec3 v_normal;
    in vec3 v_uv;

    layout (location = 0) out vec4 out_color;

    void main() {
        float lum = clamp(dot(normalize(camera_position.xyz - v_vertex), normalize(v_normal)), 0.0, 1.0) * 0.6 + 0.4;
        vec3 color = pow(texture(Texture, v_uv).rgb, vec3(2.2)) * lum;
        out_color = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
    }
'''

pipeline = ctx.pipeline(
    'texture_array.vert',
    'texture_array.frag',
    samplers={'Texture': 0},
    uniform_buffers={'Common': 0},
    vertex_array=ctx.vertex_array([
        *vertex_buffer.bind('3f 3f 2f', 0, 1, 2),
        *instance_buffer.bind('3f 1f /i', 3, 4),
    ]),
    instances=count,
    framebuffer=framebuffer,
    resources=ctx.resources(
        uniform_buffers={0: uniform_buffer},
        samplers={0: ctx.sampler(texture, min_filter='linear_mipmap_linear')},
    ),
    depth_test=True,
    cull_face='back',
)

yaw, pitch, spin = 0.59, 0.4, 0.2

running = True
while running:
    if spin:
        yaw = pygame.time.get_ticks() / 1000.0 * spin + 0.59
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.MOUSEMOTION and event.buttons[0]:
            spin = 0.0
            yaw -= event.rel[0] * 0.004
            pitch = min(max(pitch + event.rel[1] * 0.004, 0.05), 1.4)

    eye = (math.cos(yaw) * math.cos(pitch) * 9.0, math.sin(yaw) * math.cos(pitch) * 9.0, math.sin(pitch) * 9.0)
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))

    ctx.start_frame()
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x', *eye))
    framebuffer.clear()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
