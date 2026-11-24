import os

import moderngl
import pygame

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
framebuffer = ctx.framebuffer([image])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)
texture = ctx.texture((8, 8), 'rgba8unorm', os.urandom(8 * 8 * 4))

ctx.shaders['hello_texture.vert'] = '''
    #version 330 core

    out vec2 v_uv;

    vec2 vertices[4] = vec2[](
        vec2(0.0, 0.0),
        vec2(0.0, 1.0),
        vec2(1.0, 0.0),
        vec2(1.0, 1.0)
    );

    void main() {
        vec2 vertex = vertices[gl_VertexID] - 0.5;
        gl_Position = vec4(vertex * vec2(0.5625, 1.0), 0.0, 1.0);
        v_uv = vertices[gl_VertexID];
    }
'''

ctx.shaders['hello_texture.frag'] = '''
    #version 330 core

    uniform sampler2D Texture;

    in vec2 v_uv;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = texture(Texture, v_uv);
    }
'''

pipeline = ctx.pipeline(
    'hello_texture.vert',
    'hello_texture.frag',
    samplers={'Texture': 0},
    vertex_array=ctx.empty_vertex_array,
    vertices=4,
    mode='triangle_strip',
    framebuffer=framebuffer,
    resources=ctx.resources(
        samplers={
            0: ctx.sampler(
                texture,
                wrap_x='clamp_to_edge',
                wrap_y='clamp_to_edge',
                min_filter='nearest',
                mag_filter='nearest',
            ),
        },
    ),
)

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
