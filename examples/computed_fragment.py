
import extras
import moderngl
import pygame

extras.init()

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

window_size = pygame.display.get_window_size()

ctx = moderngl.get_context()

image = ctx.texture(window_size, 'rgba8unorm')

ctx.shaders['computed_fragment.vert'] = '''
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

ctx.shaders['computed_fragment.frag'] = '''
    #version 330 core

    uniform float time;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec2 p = gl_FragCoord.xy / 180.0;
        float t = time * 0.3;
        for (int i = 1; i < 9; ++i) {
            p += vec2(sin(p.y * float(i) + t), cos(p.x * float(i) - t * 0.7)) * 0.6 / float(i);
        }
        float v = sin(p.x * 3.0) + cos(p.y * 3.0);
        vec3 color = 0.5 + 0.5 * cos(6.2831 * (v * 0.25 + t * 0.1 + vec3(0.5, 0.6, 0.7)));
        out_color = vec4(pow(color, vec3(1.5)) * 0.8, 1.0);
    }
'''

pipeline = ctx.pipeline(
    'computed_fragment.vert',
    'computed_fragment.frag',
    uniforms={'time': 'float'},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=ctx.framebuffer([image]),
)

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
