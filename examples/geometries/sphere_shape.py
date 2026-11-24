import math
import os
import struct

import glm
import moderngl
import pygame

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'

pygame.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, 1)
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()

size = pygame.display.get_window_size()
image = ctx.texture(size, 'rgba8unorm', multisampled=True)
depth = ctx.texture(size, 'depth24plus', multisampled=True)
framebuffer = ctx.framebuffer([image, depth])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

uniform_buffer = ctx.buffer(size=80)

ctx.shaders['sphere_shape.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    vec3 vertices[42] = vec3[](
        vec3(0.000000, 0.000000, -1.000000),
        vec3(0.723607, -0.525725, -0.447220),
        vec3(-0.276388, -0.850649, -0.447220),
        vec3(-0.894426, 0.000000, -0.447216),
        vec3(-0.276388, 0.850649, -0.447220),
        vec3(0.723607, 0.525725, -0.447220),
        vec3(0.276388, -0.850649, 0.447220),
        vec3(-0.723607, -0.525725, 0.447220),
        vec3(-0.723607, 0.525725, 0.447220),
        vec3(0.276388, 0.850649, 0.447220),
        vec3(0.894426, 0.000000, 0.447216),
        vec3(0.000000, 0.000000, 1.000000),
        vec3(-0.162456, -0.499995, -0.850654),
        vec3(0.425323, -0.309011, -0.850654),
        vec3(0.262869, -0.809012, -0.525738),
        vec3(0.850648, 0.000000, -0.525736),
        vec3(0.425323, 0.309011, -0.850654),
        vec3(-0.525730, 0.000000, -0.850652),
        vec3(-0.688189, -0.499997, -0.525736),
        vec3(-0.162456, 0.499995, -0.850654),
        vec3(-0.688189, 0.499997, -0.525736),
        vec3(0.262869, 0.809012, -0.525738),
        vec3(0.951058, -0.309013, 0.000000),
        vec3(0.951058, 0.309013, 0.000000),
        vec3(0.000000, -1.000000, 0.000000),
        vec3(0.587786, -0.809017, 0.000000),
        vec3(-0.951058, -0.309013, 0.000000),
        vec3(-0.587786, -0.809017, 0.000000),
        vec3(-0.587786, 0.809017, 0.000000),
        vec3(-0.951058, 0.309013, 0.000000),
        vec3(0.587786, 0.809017, 0.000000),
        vec3(0.000000, 1.000000, 0.000000),
        vec3(0.688189, -0.499997, 0.525736),
        vec3(-0.262869, -0.809012, 0.525738),
        vec3(-0.850648, 0.000000, 0.525736),
        vec3(-0.262869, 0.809012, 0.525738),
        vec3(0.688189, 0.499997, 0.525736),
        vec3(0.162456, -0.499995, 0.850654),
        vec3(0.525730, 0.000000, 0.850652),
        vec3(-0.425323, -0.309011, 0.850654),
        vec3(-0.425323, 0.309011, 0.850654),
        vec3(0.162456, 0.499995, 0.850654)
    );

    vec2 texcoords[63] = vec2[](
        vec2(0.181819, 0.000000),
        vec2(0.909091, 0.000000),
        vec2(0.727273, 0.000000),
        vec2(0.545455, 0.000000),
        vec2(0.363637, 0.000000),
        vec2(0.272728, 0.157461),
        vec2(1.000000, 0.157461),
        vec2(0.090910, 0.157461),
        vec2(0.818182, 0.157461),
        vec2(0.636364, 0.157461),
        vec2(0.454546, 0.157461),
        vec2(0.181819, 0.314921),
        vec2(0.000000, 0.314921),
        vec2(0.909091, 0.314921),
        vec2(0.727273, 0.314921),
        vec2(0.545455, 0.314921),
        vec2(0.363637, 0.314921),
        vec2(0.454546, 0.472382),
        vec2(0.636364, 0.472382),
        vec2(0.818182, 0.472382),
        vec2(0.090910, 0.472382),
        vec2(0.272728, 0.472382),
        vec2(0.954545, 0.078731),
        vec2(0.136365, 0.078731),
        vec2(0.318182, 0.078731),
        vec2(0.227273, 0.078731),
        vec2(0.181819, 0.157461),
        vec2(0.363637, 0.157461),
        vec2(0.500000, 0.078731),
        vec2(0.409092, 0.078731),
        vec2(0.772727, 0.078731),
        vec2(0.863636, 0.078731),
        vec2(0.909091, 0.157461),
        vec2(0.590909, 0.078731),
        vec2(0.681818, 0.078731),
        vec2(0.727273, 0.157461),
        vec2(0.545455, 0.157461),
        vec2(0.318182, 0.236191),
        vec2(0.409092, 0.236191),
        vec2(0.136365, 0.236191),
        vec2(0.227273, 0.236191),
        vec2(0.863636, 0.236191),
        vec2(0.045455, 0.236191),
        vec2(0.954545, 0.236191),
        vec2(0.681818, 0.236191),
        vec2(0.772727, 0.236191),
        vec2(0.500000, 0.236191),
        vec2(0.590909, 0.236191),
        vec2(0.272728, 0.314921),
        vec2(0.090910, 0.314921),
        vec2(0.818182, 0.314921),
        vec2(0.636364, 0.314921),
        vec2(0.454546, 0.314921),
        vec2(0.136365, 0.393651),
        vec2(0.227273, 0.393651),
        vec2(0.409092, 0.393651),
        vec2(0.318182, 0.393651),
        vec2(0.863636, 0.393651),
        vec2(0.045455, 0.393651),
        vec2(0.681818, 0.393651),
        vec2(0.772727, 0.393651),
        vec2(0.500000, 0.393651),
        vec2(0.590909, 0.393651)
    );

    int vertex_indices[240] = int[](
        0, 13, 12, 1, 13, 15, 0, 12, 17, 0, 17, 19, 0, 19, 16, 1, 15, 22, 2, 14, 24, 3, 18, 26, 4, 20, 28,
        5, 21, 30, 1, 22, 25, 2, 24, 27, 3, 26, 29, 4, 28, 31, 5, 30, 23, 6, 32, 37, 7, 33, 39, 8, 34, 40,
        9, 35, 41, 10, 36, 38, 38, 41, 11, 38, 36, 41, 36, 9, 41, 41, 40, 11, 41, 35, 40, 35, 8, 40, 40,
        39, 11, 40, 34, 39, 34, 7, 39, 39, 37, 11, 39, 33, 37, 33, 6, 37, 37, 38, 11, 37, 32, 38, 32, 10,
        38, 23, 36, 10, 23, 30, 36, 30, 9, 36, 31, 35, 9, 31, 28, 35, 28, 8, 35, 29, 34, 8, 29, 26, 34, 26,
        7, 34, 27, 33, 7, 27, 24, 33, 24, 6, 33, 25, 32, 6, 25, 22, 32, 22, 10, 32, 30, 31, 9, 30, 21, 31,
        21, 4, 31, 28, 29, 8, 28, 20, 29, 20, 3, 29, 26, 27, 7, 26, 18, 27, 18, 2, 27, 24, 25, 6, 24, 14,
        25, 14, 1, 25, 22, 23, 10, 22, 15, 23, 15, 5, 23, 16, 21, 5, 16, 19, 21, 19, 4, 21, 19, 20, 4, 19,
        17, 20, 17, 3, 20, 17, 18, 3, 17, 12, 18, 12, 2, 18, 15, 16, 5, 15, 13, 16, 13, 0, 16, 12, 14, 2,
        12, 13, 14, 13, 1, 14
    );

    int texcoord_indices[240] = int[](
        0, 25, 23, 5, 24, 27, 1, 22, 31, 2, 30, 34, 3, 33, 28, 5, 27, 37, 7, 26, 39, 8, 32, 41, 9, 35, 44,
        10, 36, 46, 5, 37, 40, 7, 39, 42, 8, 41, 45, 9, 44, 47, 10, 46, 38, 11, 48, 54, 12, 49, 58, 14, 50,
        60, 15, 51, 62, 16, 52, 55, 55, 61, 17, 55, 52, 61, 52, 15, 61, 62, 59, 18, 62, 51, 59, 51, 14, 59,
        60, 57, 19, 60, 50, 57, 50, 13, 57, 58, 53, 20, 58, 49, 53, 49, 11, 53, 54, 56, 21, 54, 48, 56, 48,
        16, 56, 38, 52, 16, 38, 46, 52, 46, 15, 52, 47, 51, 15, 47, 44, 51, 44, 14, 51, 45, 50, 14, 45, 41,
        50, 41, 13, 50, 42, 49, 12, 42, 39, 49, 39, 11, 49, 40, 48, 11, 40, 37, 48, 37, 16, 48, 46, 47, 15,
        46, 36, 47, 36, 9, 47, 44, 45, 14, 44, 35, 45, 35, 8, 45, 41, 43, 13, 41, 32, 43, 32, 6, 43, 39,
        40, 11, 39, 26, 40, 26, 5, 40, 37, 38, 16, 37, 27, 38, 27, 10, 38, 28, 36, 10, 28, 33, 36, 33, 9,
        36, 34, 35, 9, 34, 30, 35, 30, 8, 35, 31, 32, 8, 31, 22, 32, 22, 6, 32, 27, 29, 10, 27, 24, 29,
        24, 4, 29, 23, 26, 7, 23, 25, 26, 25, 5, 26
    );

    out vec3 v_vertex;
    out vec3 v_normal;
    out vec2 v_uv;

    void main() {
        v_vertex = vertices[vertex_indices[gl_VertexID]];
        v_normal = vertices[vertex_indices[gl_VertexID]];
        v_uv = texcoords[texcoord_indices[gl_VertexID]];
        gl_Position = camera * vec4(v_vertex, 1.0);
    }
'''

ctx.shaders['sphere_shape.frag'] = '''
    #version 330 core

    in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 light_direction = vec3(0.48, 0.32, 0.81);
        float lum = dot(light_direction, normalize(v_normal)) * 0.7 + 0.3;
        out_color = vec4(lum, lum, lum, 1.0);
    }
'''

pipeline = ctx.pipeline(
    'sphere_shape.vert',
    'sphere_shape.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.empty_vertex_array,
    mode='triangles',
    vertices=240,
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    depth_test=True,
    cull_face='back',
)

yaw, pitch, spin = 0.64, 0.38, 0.3

running = True
while running:
    if spin:
        yaw = pygame.time.get_ticks() / 1000.0 * spin + 0.64
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.MOUSEMOTION and event.buttons[0]:
            spin = 0.0
            yaw -= event.rel[0] * 0.004
            pitch = min(max(pitch + event.rel[1] * 0.004, -1.4), 1.4)

    eye = (math.cos(yaw) * math.cos(pitch) * 3.5, math.sin(yaw) * math.cos(pitch) * 3.5, math.sin(pitch) * 3.5)

    ctx.start_frame()
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x', *eye))
    framebuffer.clear()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
