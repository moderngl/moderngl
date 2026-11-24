import os
import string
import struct

import moderngl
import pygame
from PIL import Image, ImageDraw, ImageFont

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

atlas_size = (1024, 1024)
atlas = Image.new('L', atlas_size)
draw = ImageDraw.Draw(atlas)
glyphs = {}
x, y, row = 0, 0, 0
for font_size in (16, 24, 32, 48):
    font = ImageFont.truetype('examples/data/font/OpenSans-Medium.ttf', font_size)
    for c in string.printable[:95]:
        x0, y0, x1, y1 = font.getbbox(c)
        w, h = x1 - x0 + 2, y1 - y0 + 2
        if x + w > atlas_size[0]:
            x, y, row = 0, y + row, 0
        draw.text((x - x0 + 1, y - y0 + 1), c, 255, font=font)
        glyphs[font_size, c] = (x, y, x + w, y + h, x0 - 1, y0 - 1, font.getlength(c))
        x, row = x + w, max(row, h)

texture = ctx.texture(atlas_size, 'r8unorm', atlas.tobytes())

instance = struct.Struct('2f4f4B')
instances = bytearray()


def text(x, y, line, font_size, color):
    global instances
    for c in line:
        u0, v0, u1, v1, xoff, yoff, advance = glyphs[font_size, c]
        instances += instance.pack(round(x + xoff), y + yoff, u0, v0, u1, v1, *color)
        x += advance


text(100, 80, 'Hello World!', 48, (255, 255, 255, 255))
text(100, 160, 'Hello World!', 32, (255, 96, 96, 255))
text(400, 160, 'Hello World!', 24, (96, 255, 96, 255))
text(650, 160, 'Hello World!', 16, (96, 160, 255, 255))
text(100, 280, 'Lorem ipsum dolor sit amet, consectetur adipiscing elit.', 24, (220, 220, 220, 255))
text(100, 316, 'Aenean commodo purus vel molestie pellentesque.', 24, (220, 220, 220, 255))
text(100, 352, 'Curabitur non egestas mi. Nulla tincidunt suscipit enim id tristique.', 24, (220, 220, 220, 255))
text(100, 440, string.printable[:62], 16, (255, 200, 80, 255))
text(100, 464, string.printable[62:94], 16, (255, 200, 80, 255))

instance_buffer = ctx.buffer(instances)

ctx.shaders['font.vert'] = '''
    #version 330 core

    uniform vec2 screen_size;
    uniform vec2 atlas_size;

    vec2 vertices[4] = vec2[](
        vec2(0.0, 0.0),
        vec2(0.0, 1.0),
        vec2(1.0, 0.0),
        vec2(1.0, 1.0)
    );

    layout (location = 0) in vec2 in_position;
    layout (location = 1) in vec4 in_rect;
    layout (location = 2) in vec4 in_color;

    out vec2 v_uv;
    out vec4 v_color;

    void main() {
        vec2 corner = vertices[gl_VertexID];
        vec2 vertex = in_position + corner * (in_rect.zw - in_rect.xy);
        gl_Position = vec4(vertex / screen_size * vec2(2.0, -2.0) + vec2(-1.0, 1.0), 0.0, 1.0);
        v_uv = mix(in_rect.xy, in_rect.zw, corner) / atlas_size;
        v_color = in_color;
    }
'''

ctx.shaders['font.frag'] = '''
    #version 330 core

    uniform sampler2D Texture;

    in vec2 v_uv;
    in vec4 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        float alpha = texture(Texture, v_uv).r;
        out_color = vec4(v_color.rgb, v_color.a * alpha);
    }
'''

pipeline = ctx.pipeline(
    'font.vert',
    'font.frag',
    samplers={'Texture': 0},
    uniforms={'screen_size': 'vec2', 'atlas_size': 'vec2'},
    vertex_array=ctx.vertex_array(instance_buffer.bind('2f 4f 4nu1 /i', 0, 1, 2)),
    mode='triangle_strip',
    vertices=4,
    instances=len(instances) // instance.size,
    framebuffer=framebuffer,
    resources=ctx.resources(samplers={0: ctx.sampler(texture, min_filter='nearest', mag_filter='nearest')}),
    blend={'src': 'src_alpha', 'dst': 'one_minus_src_alpha'},
)

pipeline['screen_size'] = size
pipeline['atlas_size'] = atlas_size

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    ctx.start_frame()
    framebuffer.clear()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
