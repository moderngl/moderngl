
import extras
import moderngl
import numpy as np
import pygame

extras.init()

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

window_size = pygame.display.get_window_size()

ctx = moderngl.get_context()

image = ctx.texture(window_size, 'rgba8unorm', samples=4)
framebuffer = ctx.framebuffer([image])
framebuffer.clear_color = (0.0, 0.0, 0.0, 1.0)

uniform_buffer = ctx.buffer(size=16, kind='uniform_buffer')

ctx.shaders['uniform_buffer.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        vec2 move;
        vec2 scale;
    };

    out vec3 v_color;

    vec2 positions[3] = vec2[](
        vec2(0.0, 0.8),
        vec2(-0.6, -0.8),
        vec2(0.6, -0.8)
    );

    vec3 colors[3] = vec3[](
        vec3(1.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 0.0, 1.0)
    );

    void main() {
        gl_Position = vec4(positions[gl_VertexID] * scale + move, 0.0, 1.0);
        v_color = colors[gl_VertexID];
    }
'''

ctx.shaders['uniform_buffer.frag'] = '''
    #version 330 core

    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(v_color, 1.0);
    }
'''

triangle = ctx.pipeline(
    'uniform_buffer.vert',
    'uniform_buffer.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=framebuffer,
    resources=ctx.resources(
        uniform_buffers={
            0: uniform_buffer,
        },
    ),
)

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    now = pygame.time.get_ticks() / 1000.0

    ctx.start_frame()
    uniform_buffer.write(np.array([
        np.sin(now) * 0.1,
        np.cos(now) * 0.1,
        0.7 + np.sin(now * 5) * 0.05,
        0.7 + np.sin(now * 5) * 0.05,
    ], 'f4'))
    framebuffer.clear()
    triangle.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
