import os

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
image = ctx.texture(size, 'rgba8unorm')

ctx.shaders['checker.vert'] = '''
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

ctx.shaders['checker.frag'] = '''
    #version 330 core

    layout (location = 0) out vec4 out_color;

    uniform vec2 resolution;

    struct CheckerTexture {
        float scale;
        vec3 color1;
        vec3 color2;
    };

    float checker_factor(vec2 uv, CheckerTexture mt) {
        vec2 s = sign(fract(uv * mt.scale * 0.5) - 0.5);
        return 0.5 - s.x * s.y * 0.5;
    }

    vec3 checker_color(vec2 uv, CheckerTexture mt) {
        return mix(mt.color1, mt.color2, checker_factor(uv, mt));
    }

    void main() {
        vec2 uv = gl_FragCoord.xy / resolution.y;

        CheckerTexture mt;
        mt.scale = 5.0;
        mt.color1 = vec3(0.2, 0.2, 0.2);
        mt.color2 = vec3(0.8, 0.8, 0.8);

        vec3 color = checker_color(uv, mt);
        out_color = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
    }
'''

pipeline = ctx.pipeline(
    'checker.vert',
    'checker.frag',
    uniforms={'resolution': 'vec2'},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=ctx.framebuffer([image]),
)

pipeline['resolution'] = size

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    ctx.start_frame()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
