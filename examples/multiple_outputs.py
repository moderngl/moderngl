
import extras
import moderngl
import pygame

extras.init()

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()

image_size = (640, 720)
image1 = ctx.texture(image_size, 'rgba8unorm', samples=4)
image2 = ctx.texture(image_size, 'rgba8unorm', samples=4)
framebuffer = ctx.framebuffer([image1, image2])
framebuffer.clear_color = ((0.0, 0.0, 0.0, 1.0), (0.0, 0.0, 0.0, 1.0))
resolved = ctx.texture(image_size, 'rgba8unorm')

ctx.shaders['multiple_outputs.vert'] = '''
    #version 330 core

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
        gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
        v_color = colors[gl_VertexID];
    }
'''

ctx.shaders['multiple_outputs.frag'] = '''
    #version 330 core

    in vec3 v_color;

    layout (location = 0) out vec4 out_color1;
    layout (location = 1) out vec4 out_color2;

    void main() {
        out_color1 = vec4(v_color, 1.0);
        out_color2 = vec4(0.5, 0.5, 0.5, 1.0);
    }
'''

triangle = ctx.pipeline(
    'multiple_outputs.vert',
    'multiple_outputs.frag',
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
    triangle.run()
    ctx.copy_texture(image1, ctx.screen)
    ctx.copy_texture(image2, resolved)
    ctx.copy_texture(resolved, ctx.screen, dst_offset=(640, 0))
    ctx.end_frame()

    pygame.display.flip()

pygame.quit()
