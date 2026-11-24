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


def color(hex_color: str):
    r = int(hex_color[1:3], 16) / 255.0
    g = int(hex_color[3:5], 16) / 255.0
    b = int(hex_color[5:7], 16) / 255.0
    return r, g, b


size = pygame.display.get_window_size()
image = ctx.texture(size, 'rgba8unorm')

ctx.shaders['gradient_background.vert'] = '''
    #version 330 core

    vec2 vertices[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2(3.0, -1.0),
        vec2(-1.0, 3.0)
    );

    void main() {
        gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
    }
'''

ctx.shaders['gradient_background.frag'] = '''
    #version 330 core

    uniform vec2 size;
    uniform vec3 color1;
    uniform vec3 color2;
    uniform float noise;
    uniform float time;

    layout (location = 0) out vec4 out_color;

    float hash12(vec2 p) {
        vec3 p3 = fract(vec3(p.xyx) * 0.1031);
        p3 += dot(p3, p3.yzx + 33.33);
        return fract((p3.x + p3.y) * p3.z);
    }

    void main() {
        vec2 uv = gl_FragCoord.xy / size;
        float c = distance(uv, vec2(0.5)) / sqrt(2.0);
        c += hash12(uv * size + time) * noise;
        vec3 color = mix(color1, color2, c);
        out_color = vec4(color, 1.0);
    }
'''

pipeline = ctx.pipeline(
    'gradient_background.vert',
    'gradient_background.frag',
    uniforms={
        'size': 'vec2',
        'color1': 'vec3',
        'color2': 'vec3',
        'noise': 'float',
        'time': 'float',
    },
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=ctx.framebuffer([image]),
)

pipeline['size'] = size
pipeline['color1'] = color('#0C5291')
pipeline['color2'] = color('#06294F')
pipeline['noise'] = 0.03

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    now = pygame.time.get_ticks() / 1000.0

    ctx.start_frame()
    pipeline['time'] = now
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
