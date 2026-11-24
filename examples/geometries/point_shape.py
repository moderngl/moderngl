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

ctx.shaders['point_shape.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    vec3 vertices[12] = vec3[](
        vec3(0.000000, 0.000000, -1.000000),
        vec3(0.723600, -0.525720, -0.447215),
        vec3(-0.276385, -0.850640, -0.447215),
        vec3(-0.894425, 0.000000, -0.447215),
        vec3(-0.276385, 0.850640, -0.447215),
        vec3(0.723600, 0.525720, -0.447215),
        vec3(0.276385, -0.850640, 0.447215),
        vec3(-0.723600, -0.525720, 0.447215),
        vec3(-0.723600, 0.525720, 0.447215),
        vec3(0.276385, 0.850640, 0.447215),
        vec3(0.894425, 0.000000, 0.447215),
        vec3(0.000000, 0.000000, 1.000000)
    );

    vec2 texcoords[22] = vec2[](
        vec2(0.181819, 0.000000),
        vec2(0.363637, 0.000000),
        vec2(0.909091, 0.000000),
        vec2(0.727273, 0.000000),
        vec2(0.545455, 0.000000),
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
        vec2(0.272728, 0.472382),
        vec2(0.090910, 0.472382),
        vec2(0.818182, 0.472382),
        vec2(0.636364, 0.472382),
        vec2(0.454546, 0.472382)
    );

    int vertex_indices[60] = int[](
        0, 1, 2, 1, 0, 5, 0, 2, 3, 0, 3, 4, 0, 4, 5, 1, 5, 10, 2, 1, 6, 3, 2, 7, 4, 3, 8, 5, 4, 9, 1, 10,
        6, 2, 6, 7, 3, 7, 8, 4, 8, 9, 5, 9, 10, 6, 10, 11, 7, 6, 11, 8, 7, 11, 9, 8, 11, 10, 9, 11
    );

    int texcoord_indices[60] = int[](
        0, 5, 7, 5, 1, 10, 2, 6, 8, 3, 8, 9, 4, 9, 10, 5, 10, 16, 7, 5, 11, 8, 6, 13, 9, 8, 14, 10, 9, 15,
        5, 16, 11, 7, 11, 12, 8, 13, 14, 9, 14, 15, 10, 15, 16, 11, 16, 17, 12, 11, 18, 14, 13, 19, 15, 14,
        20, 16, 15, 21
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

ctx.shaders['point_shape.frag'] = '''
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
    'point_shape.vert',
    'point_shape.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.empty_vertex_array,
    mode='triangles',
    vertices=60,
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
