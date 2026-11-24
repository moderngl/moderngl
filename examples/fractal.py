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
image = ctx.texture(size, 'rgba8unorm')
framebuffer = ctx.framebuffer([image])

ctx.shaders['fractal.vert'] = '''
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

ctx.shaders['fractal.frag'] = '''
    #version 330 core

    uniform float aspect;

    in vec2 vertex;

    layout (location = 0) out vec4 out_color;

    float mandelbrot(vec2 uv) {
        vec2 z = vec2(0.0, 0.0);
        vec2 c = uv;
        int i;
        for (i = 0; i < 100; ++i) {
            vec2 v = vec2(
                (z.x * z.x - z.y * z.y) + c.x,
                (z.y * z.x + z.x * z.y) + c.y
            );
            if (dot(v, v) > 4.0) break;
            z = v;
        }
        if (i == 100) {
            i = 0;
        }
        return float(i) / 100.0;
    }

    void main() {
        float cm = mandelbrot(vertex * vec2(aspect, 1.0) * 1.3 - vec2(0.6, 0.0));
        vec3 color = 0.5 + 0.5 * cos(6.2831 * (sqrt(cm) * 1.5 + vec3(0.6, 0.7, 0.8)));
        out_color = vec4(color * min(cm * 8.0, 1.0), 1.0);
    }
'''

pipeline = ctx.pipeline(
    'fractal.vert',
    'fractal.frag',
    vertex_array=ctx.empty_vertex_array,
    uniforms={'aspect': 'float'},
    vertices=3,
    framebuffer=framebuffer,
)

pipeline['aspect'] = size[0] / size[1]

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
