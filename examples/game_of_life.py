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
image = ctx.texture(size, 'rgba8unorm', os.urandom(size[0] * size[1] * 4))
temp = ctx.texture(size, 'rgba8unorm')

ctx.shaders['game_of_life.vert'] = '''
    #version 330 core

    vec2 positions[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2(3.0, -1.0),
        vec2(-1.0, 3.0)
    );

    void main() {
        gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
    }
'''

ctx.shaders['game_of_life.frag'] = '''
    #version 330 core

    uniform sampler2D Texture;
    uniform ivec2 Size;

    layout (location = 0) out vec4 out_color;

    int c(int x, int y) {
        ivec2 at = (ivec2(gl_FragCoord.xy) + ivec2(x, y) + Size) % Size;
        return texelFetch(Texture, at, 0).r < 0.5 ? 0 : 1;
    }

    void main() {
        float res;
        int neighbours = c(-1, -1) + c(-1, 0) + c(0, 1) + c(0, -1) + c(-1, 1) + c(1, -1) + c(1, 0) + c(1, 1);
        if (c(0, 0) == 1) {
            res = (neighbours == 2 || neighbours == 3) ? 1.0 : 0.0;
        } else {
            res = (neighbours == 3) ? 1.0 : 0.0;
        }
        out_color = vec4(res, res, res, 1.0);
    }
'''

pipeline = ctx.pipeline(
    'game_of_life.vert',
    'game_of_life.frag',
    samplers={'Texture': 0},
    uniforms={'Size': 'ivec2'},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=ctx.framebuffer([image]),
    resources=ctx.resources(
        samplers={
            0: ctx.sampler(temp),
        },
    ),
)

pipeline['Size'] = size

sim_time = pygame.time.get_ticks()

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    ctx.start_frame()
    now = pygame.time.get_ticks()
    sim_time = max(sim_time, now - 100)
    while sim_time < now:
        sim_time += 16
        ctx.copy_texture(image, temp)
        pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
