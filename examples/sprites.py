import os

import moderngl
import numpy as np
import pygame
from PIL import Image

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
image = ctx.texture(size, 'rgba8unorm')
framebuffer = ctx.framebuffer([image])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

atlas = Image.open('examples/data/sprites/sprites.png').convert('RGBA')
pixels = b''.join(atlas.crop((i % 8 * 64, i // 8 * 64, i % 8 * 64 + 64, i // 8 * 64 + 64)).transpose(Image.FLIP_TOP_BOTTOM).tobytes() for i in range(48))
texture = ctx.texture((64, 64), 'rgba8unorm', pixels, array=48)

count = 100
instances = np.array([
    np.random.uniform(0.0, size[0], count),
    np.random.uniform(0.0, size[1], count),
    np.random.uniform(0.0, np.pi, count),
    np.random.uniform(2, 48, count),
], 'f4').T.copy()
turn = np.random.uniform(-0.002, 0.002, count)
instance_buffer = ctx.buffer(instances)

ctx.shaders['sprites.vert'] = '''
    #version 330 core

    uniform vec2 screen_size;

    vec2 vertices[4] = vec2[](
        vec2(-1.0, -1.0),
        vec2(-1.0, 1.0),
        vec2(1.0, -1.0),
        vec2(1.0, 1.0)
    );

    layout (location = 0) in vec4 in_attributes;

    out vec3 v_uv;

    void main() {
        vec2 position = in_attributes.xy;
        float rotation = in_attributes.z;
        float layer = in_attributes.w;
        mat2 rot = mat2(cos(rotation), -sin(rotation), sin(rotation), cos(rotation));
        vec2 vertex = position + rot * vertices[gl_VertexID] * 32.0;
        gl_Position = vec4(vertex / screen_size * 2.0 - 1.0, 0.0, 1.0);
        v_uv = vec3(vertices[gl_VertexID] * 0.5 + 0.5, layer);
    }
'''

ctx.shaders['sprites.frag'] = '''
    #version 330 core

    uniform sampler2DArray Texture;

    in vec3 v_uv;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = texture(Texture, v_uv);
        if (out_color.a < 0.05) {
            discard;
        }
    }
'''

pipeline = ctx.pipeline(
    'sprites.vert',
    'sprites.frag',
    samplers={'Texture': 0},
    uniforms={'screen_size': 'vec2'},
    vertex_array=ctx.vertex_array(instance_buffer.bind('4f /i', 0)),
    mode='triangle_strip',
    vertices=4,
    instances=count,
    framebuffer=framebuffer,
    resources=ctx.resources(samplers={0: ctx.sampler(texture, wrap_x='clamp_to_edge', wrap_y='clamp_to_edge')}),
    blend={'src': 'src_alpha', 'dst': 'one_minus_src_alpha'},
)

pipeline['screen_size'] = size

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    instances[:, 0] = (instances[:, 0] + np.sin(instances[:, 2]) * 0.2) % size[0]
    instances[:, 1] = (instances[:, 1] + np.cos(instances[:, 2]) * 0.2) % size[1]
    instances[:, 2] += turn

    ctx.start_frame()
    instance_buffer.write(instances)
    framebuffer.clear()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
