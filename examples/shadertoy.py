import struct

import extras
import moderngl
import pygame

extras.init()

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

window_size = pygame.display.get_window_size()

ctx = moderngl.get_context()

image = ctx.texture(window_size, 'rgba8unorm')
framebuffer = ctx.framebuffer([image])
uniform_buffer = ctx.buffer(size=64, kind='uniform_buffer')

# Tested with:
# Happy Jumping - https://www.shadertoy.com/view/3lsSzf
# Raymarching - Primitives - https://www.shadertoy.com/view/Xds3zN
# GLSL ray tracing test - https://www.shadertoy.com/view/3sc3z4
# Ray Marching: Part 6 - https://www.shadertoy.com/view/4tcGDr
# Seascape - https://www.shadertoy.com/view/Ms2SD1
# Mandelbulb - https://www.shadertoy.com/view/MdXSWn

# Paste your code below

ctx.shaders['shadertoy'] = '''
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy;

    // Time varying pixel color
    vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4));

    // Output to screen
    fragColor = vec4(col,1.0);
}
'''

ctx.shaders['uniforms'] = '''
    layout (std140) uniform Uniforms {
        vec3 iResolution;
        float iTime;
        float iTimeDelta;
        int iFrame;
        vec4 iMouse;
        vec4 iDate;
    };
'''

ctx.shaders['shadertoy.vert'] = '''
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

ctx.shaders['shadertoy.frag'] = '''
    #version 330 core

    #include "uniforms"
    #include "shadertoy"

    layout (location = 0) out vec4 shader_color_output;

    void main() {
        mainImage(shader_color_output, gl_FragCoord.xy);
    }
'''

canvas = ctx.pipeline(
    'shadertoy.vert',
    'shadertoy.frag',
    uniform_buffers={'Uniforms': 0},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=framebuffer,
    resources=ctx.resources(
        uniform_buffers={
            0: uniform_buffer,
        },
    ),
)

ubo = struct.Struct('=3f1f1f1i8x4f4f')
last_time = 0.0
frame = 0

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    mouse_pos = pygame.mouse.get_pos()
    now = pygame.time.get_ticks() / 1000.0

    ctx.start_frame()
    framebuffer.clear()
    uniform_buffer.write(ubo.pack(
        window_size[0], window_size[1], 0.0,
        now,
        now - last_time,
        frame,
        mouse_pos[0], window_size[1] - mouse_pos[1], 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
    ))
    canvas.run()
    ctx.present(image)

    last_time = now
    frame += 1

    pygame.display.flip()

pygame.quit()
