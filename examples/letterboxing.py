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
pygame.display.set_mode((800, 600), flags=pygame.OPENGL | pygame.DOUBLEBUF | pygame.RESIZABLE, vsync=True)

ctx = moderngl.get_context()

render_size = (640, 360)
image = ctx.texture(render_size, 'rgba8unorm', os.urandom(render_size[0] * render_size[1] * 4))

ctx.shaders['letterboxing.vert'] = '''
    #version 330 core

    vec2 vertices[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2(3.0, -1.0),
        vec2(-1.0, 3.0)
    );

    out vec2 vertex;

    void main() {
        gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
        vertex = vertices[gl_VertexID];
    }
'''

ctx.shaders['letterboxing.frag'] = '''
    #version 330 core

    uniform sampler2D Texture;
    uniform vec2 window_size;
    uniform vec2 render_size;

    in vec2 vertex;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec2 scale = window_size / render_size;
        vec2 uv = vertex * scale / min(scale.x, scale.y);
        uv = uv * 0.5 + 0.5;
        if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
            out_color = vec4(0.0, 0.0, 0.0, 1.0);
            return;
        }
        out_color = texture(Texture, uv);
    }
'''

pipeline = ctx.pipeline(
    'letterboxing.vert',
    'letterboxing.frag',
    samplers={'Texture': 0},
    uniforms={
        'window_size': 'vec2',
        'render_size': 'vec2',
    },
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=ctx.screen,
    resources=ctx.resources(
        samplers={
            0: ctx.sampler(
                image,
                wrap_x='clamp_to_edge',
                wrap_y='clamp_to_edge',
                min_filter='nearest',
                mag_filter='nearest',
            ),
        },
    ),
)

pipeline['render_size'] = render_size

pipeline['window_size'] = ctx.screen.size

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

        if event.type == pygame.VIDEORESIZE:
            ctx.screen.size = event.size
            pipeline['window_size'] = event.size

    ctx.start_frame()
    pipeline.run()
    ctx.end_frame()

    pygame.display.flip()

pygame.quit()
