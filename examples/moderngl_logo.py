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
image = ctx.texture(size, 'rgba8unorm', multisampled=True)
framebuffer = ctx.framebuffer([image])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

ctx.shaders['logo.vert'] = '''
    #version 330 core

    uniform float time;

    vec2 vertices[3] = vec2[](
        vec2(1.0, 0.0),
        vec2(-0.5, -0.86),
        vec2(-0.5, 0.86)
    );

    vec3 colors[3] = vec3[](
        vec3(0.0, 0.0, 1.0),
        vec3(0.0, 1.0, 0.0),
        vec3(1.0, 0.0, 0.0)
    );

    out vec3 v_color;

    void main() {
        float r = float(gl_InstanceID - 4) * 0.15 * (1.0 + sin(time) * 0.3);
        mat2 rot = mat2(cos(r), sin(r), -sin(r), cos(r));
        gl_Position = vec4(rot * vertices[gl_VertexID] * vec2(0.5625, 1.0) * 0.95, 0.0, 1.0);
        v_color = colors[gl_VertexID];
    }
'''

ctx.shaders['logo.frag'] = '''
    #version 330 core

    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(pow(v_color, vec3(1.0 / 2.2)), 0.5);
    }
'''

logo = ctx.pipeline(
    'logo.vert',
    'logo.frag',
    uniforms={'time': 'float'},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    instances=9,
    framebuffer=framebuffer,
    blend={'src': 'src_alpha', 'dst': 'one_minus_src_alpha'},
)

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    ctx.start_frame()
    framebuffer.clear()
    logo['time'] = pygame.time.get_ticks() / 1000.0
    logo.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
