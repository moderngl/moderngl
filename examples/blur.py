import math
import os
import struct
import sys

import glm
import moderngl
import pygame

sys.path.insert(0, 'examples')
from glb import load_glb

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
depth = ctx.texture(size, 'depth24plus')
temp = ctx.texture(size, 'rgba8unorm')
output = ctx.texture(size, 'rgba8unorm')
framebuffer = ctx.framebuffer([image, depth])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

vertex_buffer = ctx.buffer(load_glb('examples/data/blob/blob.glb', texcoords=False)['scene'])

uniform_buffer = ctx.buffer(size=80)

ctx.shaders['scene.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;

    out vec3 v_normal;

    void main() {
        gl_Position = camera * vec4(in_vertex, 1.0);
        v_normal = in_normal;
    }
'''

ctx.shaders['scene.frag'] = '''
    #version 330 core

    in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 light = vec3(4.0, 3.0, 10.0);
        float lum = dot(normalize(light), normalize(v_normal)) * 0.7 + 0.3;
        out_color = vec4(lum, lum, lum, 1.0);
    }
'''

kernel = [math.exp(-x * x / (19.0 * 19.0 / 4.0)) for x in range(-19, 20)]
kernel = ', '.join(f'{x / sum(kernel):.8f}' for x in kernel)

ctx.shaders['blur.vert'] = '''
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

ctx.shaders['blur.frag'] = f'''
    #version 330 core

    uniform sampler2D Texture;
    uniform ivec2 direction;

    layout (location = 0) out vec4 out_color;

    float coeff[39] = float[]({kernel});

    void main() {{
        vec3 color = vec3(0.0);
        for (int i = 0; i < 39; ++i) {{
            ivec2 at = clamp(ivec2(gl_FragCoord.xy) + direction * (i - 19), ivec2(0), textureSize(Texture, 0) - 1);
            color += texelFetch(Texture, at, 0).rgb * coeff[i];
        }}
        out_color = vec4(color, 1.0);
    }}
'''

scene = ctx.pipeline(
    'scene.vert',
    'scene.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.vertex_array(vertex_buffer.bind('3f 3f', 0, 1)),
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    depth_test=True,
    cull_face='back',
)

blur_x = ctx.pipeline(
    'blur.vert',
    'blur.frag',
    samplers={'Texture': 0},
    uniforms={'direction': 'ivec2'},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=ctx.framebuffer([temp]),
    resources=ctx.resources(samplers={0: ctx.sampler(image)}),
)

blur_y = ctx.pipeline(
    'blur.vert',
    'blur.frag',
    samplers={'Texture': 0},
    uniforms={'direction': 'ivec2'},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=ctx.framebuffer([output]),
    resources=ctx.resources(samplers={0: ctx.sampler(temp)}),
)

blur_x['direction'] = (1, 0)
blur_y['direction'] = (0, 1)

yaw, pitch, spin = 0.0, 0.38, 0.2

running = True
while running:
    if spin:
        yaw = pygame.time.get_ticks() / 1000.0 * spin
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.MOUSEMOTION and event.buttons[0]:
            spin = 0.0
            yaw -= event.rel[0] * 0.004
            pitch = min(max(pitch + event.rel[1] * 0.004, -0.5), 1.4)

    eye = (math.cos(yaw) * math.cos(pitch) * 4.27, math.sin(yaw) * math.cos(pitch) * 4.27, 0.5 + math.sin(pitch) * 4.27)

    ctx.start_frame()
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.5), (0.0, 0.0, 1.0))
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x', *eye))
    framebuffer.clear()
    scene.run()
    blur_x.run()
    blur_y.run()
    ctx.present(output)

    pygame.display.flip()

pygame.quit()
