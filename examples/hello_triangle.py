
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

ctx.shaders['hello_triangle.vert'] = '''
    #version 330 core

    vec2 vertices[3] = vec2[](
        vec2(0.0, 0.8),
        vec2(-0.866, -0.7),
        vec2(0.866, -0.7)
    );

    vec3 colors[3] = vec3[](
        vec3(1.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 0.0, 1.0)
    );

    out vec3 v_color;

    void main() {
        gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
        v_color = colors[gl_VertexID];
    }
'''

ctx.shaders['hello_triangle.frag'] = '''
    #version 330 core

    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(v_color, 1.0);
        out_color.rgb = pow(out_color.rgb, vec3(1.0 / 2.2));
    }
'''

pipeline = ctx.pipeline(
    'hello_triangle.vert',
    'hello_triangle.frag',
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=framebuffer,
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
