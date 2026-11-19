import array

import extras
import moderngl
import pygame

extras.init()

pygame.init()
pygame.display.set_mode((720, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()

size = pygame.display.get_window_size()
image = ctx.texture(size, 'rgba8unorm', samples=4)
framebuffer = ctx.framebuffer([image])

triangle = array.array('f', [
    1.0, 0.0, 1.0, 0.0, 0.0, 0.5,
    -0.5, 0.86, 0.0, 1.0, 0.0, 0.5,
    -0.5, -0.86, 0.0, 0.0, 1.0, 0.5,
])

vertex_buffer = ctx.buffer(triangle)

ctx.shaders['blending.vert'] = '''
    #version 330 core

    uniform vec2 scale;
    uniform float rotation;

    layout (location = 0) in vec2 in_vertex;
    layout (location = 1) in vec4 in_color;

    out vec4 v_color;

    void main() {
        float r = rotation * (0.5 + float(gl_InstanceID) * 0.05);
        mat2 rot = mat2(cos(r), sin(r), -sin(r), cos(r));
        gl_Position = vec4((rot * in_vertex) * scale, 0.0, 1.0);
        v_color = in_color;
    }
'''

ctx.shaders['blending.frag'] = '''
    #version 330 core

    in vec4 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(v_color);
        out_color.rgb = pow(out_color.rgb, vec3(1.0 / 2.2));
    }
'''

pipeline = ctx.pipeline(
    'blending.vert',
    'blending.frag',
    uniforms={
        'scale': 'vec2',
        'rotation': 'float',
    },
    vertex_array=ctx.vertex_array(vertex_buffer.bind('2f 4f', 0, 1)),
    framebuffer=framebuffer,
    instances=10,
    blend={
        'src': 'src_alpha',
        'dst': 'one_minus_src_alpha',
    },
)

pipeline['scale'] = (0.8, 0.8)

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    now = pygame.time.get_ticks() / 1000.0

    ctx.start_frame()
    framebuffer.clear()
    pipeline['rotation'] = now
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
