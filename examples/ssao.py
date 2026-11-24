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
position = ctx.texture(size, 'rgba32float')
normal = ctx.texture(size, 'rgba32float')
depth = ctx.texture(size, 'depth24plus')
gbuffer = ctx.framebuffer([position, normal, depth])
gbuffer.clear_color = ((0.0, 0.0, -1000.0, 0.0), (0.0, 0.0, 0.0, 0.0))

vertex_buffer = ctx.buffer(load_glb('examples/data/blob/blob.glb', texcoords=False)['scene'])

uniform_buffer = ctx.buffer(size=80)

ctx.shaders['gbuffer.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;

    out vec3 v_vertex;
    out vec3 v_normal;

    void main() {
        gl_Position = camera * vec4(in_vertex, 1.0);
        v_vertex = in_vertex;
        v_normal = in_normal;
    }
'''

ctx.shaders['gbuffer.frag'] = '''
    #version 330 core

    in vec3 v_vertex;
    in vec3 v_normal;

    layout (location = 0) out vec4 out_position;
    layout (location = 1) out vec4 out_normal;

    void main() {
        out_position = vec4(v_vertex, 0.0);
        out_normal = vec4(v_normal, 0.0);
    }
'''

ctx.shaders['ssao.vert'] = '''
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

ctx.shaders['ssao.frag'] = '''
    #version 330 core

    uniform sampler2D Position;
    uniform sampler2D Normal;

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    layout (location = 0) out vec4 out_color;

    vec3 points[16] = vec3[](
        vec3(-0.2227, 0.7356, 0.4659),
        vec3(0.1868, 0.7353, 0.0808),
        vec3(0.0237, 0.1313, 0.3295),
        vec3(-0.0870, 0.6000, 0.6392),
        vec3(0.4394, -0.2024, 0.6497),
        vec3(0.5340, -0.5363, 0.1430),
        vec3(0.6900, 0.4900, 0.2762),
        vec3(-0.3859, 0.1028, 0.6229),
        vec3(-0.0469, -0.3474, 0.5782),
        vec3(0.6194, -0.0360, 0.4796),
        vec3(0.2145, 0.1505, 0.5911),
        vec3(-0.0271, 0.2603, 0.6902),
        vec3(0.1280, -0.5275, 0.2892),
        vec3(-0.6619, 0.1548, 0.2046),
        vec3(-0.1323, -0.7756, 0.3461),
        vec3(-0.4802, -0.5583, 0.2939)
    );

    float hash13(vec3 p3) {
        p3 = fract(p3 * 0.1031);
        p3 += dot(p3, p3.zyx + 31.32);
        return fract((p3.x + p3.y) * p3.z);
    }

    void main() {
        ivec2 at = ivec2(gl_FragCoord.xy);
        vec3 position = texelFetch(Position, at, 0).rgb;
        vec3 normal = texelFetch(Normal, at, 0).rgb;
        if (normal == vec3(0.0)) {
            out_color = vec4(0.05, 0.05, 0.05, 1.0);
            return;
        }
        vec3 x = cross(normal, vec3(0.0, 0.0, 1.0));
        vec3 y = cross(normal, x);
        float angle = hash13(gl_FragCoord.xyz);
        mat3 rot = mat3(
            cos(angle), -sin(angle), 0.0,
            sin(angle), cos(angle), 0.0,
            0.0, 0.0, 1.0
        );
        mat3 basis = mat3(x, y, normal) * rot;

        float lum = 0.0;
        for (int i = 0; i < 16; i++) {
            vec4 tmp = camera * vec4(position + basis * points[i] * 0.08, 1.0);
            vec2 uv = (tmp.xy / tmp.w) * 0.5 + 0.5;
            vec3 pick = texture(Position, uv).rgb;
            if (distance(pick, camera_position.xyz) > distance(position, camera_position.xyz) - 1e-2) {
                lum += 1.0 / 16.0;
            }
        }

        out_color = vec4(lum, lum, lum, 1.0);
    }
'''

scene = ctx.pipeline(
    'gbuffer.vert',
    'gbuffer.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.vertex_array(vertex_buffer.bind('3f 3f', 0, 1)),
    framebuffer=gbuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    depth_test=True,
    cull_face='back',
)

ssao = ctx.pipeline(
    'ssao.vert',
    'ssao.frag',
    samplers={'Position': 0, 'Normal': 1},
    uniform_buffers={'Common': 0},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=ctx.framebuffer([image]),
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}, 
        samplers={
            0: ctx.sampler(position, min_filter='nearest', mag_filter='nearest'),
            1: ctx.sampler(normal, min_filter='nearest', mag_filter='nearest'),
        },
    ),
)

yaw, pitch, spin = -1.57, 0.19, 0.2

running = True
while running:
    if spin:
        yaw = pygame.time.get_ticks() / 1000.0 * spin - 1.57
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.MOUSEMOTION and event.buttons[0]:
            spin = 0.0
            yaw -= event.rel[0] * 0.004
            pitch = min(max(pitch + event.rel[1] * 0.004, 0.05), 1.4)

    eye = (math.cos(yaw) * math.cos(pitch) * 4.5, math.sin(yaw) * math.cos(pitch) * 4.5, 0.5 + math.sin(pitch) * 4.5)
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.5), (0.0, 0.0, 1.0))

    ctx.start_frame()
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x', *eye))
    gbuffer.clear()
    scene.run()
    ssao.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
