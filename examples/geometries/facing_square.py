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

ctx.shaders['facing_square.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    vec2 vertices[4] = vec2[](
        vec2(-0.5, -0.5),
        vec2(0.5, -0.5),
        vec2(-0.5, 0.5),
        vec2(0.5, 0.5)
    );

    vec3 position = vec3(0.0, 0.0, 0.0);
    vec3 up = vec3(0.0, 0.0, 1.0);

    out vec3 v_vertex;
    out vec3 v_normal;
    out vec2 v_uv;

    void main() {
        v_normal = normalize(camera_position.xyz - position);
        vec3 tangent = normalize(cross(up, v_normal));
        vec3 bitangent = cross(v_normal, tangent);
        v_vertex = position + tangent * vertices[gl_VertexID].x + bitangent * vertices[gl_VertexID].y;
        v_uv = vertices[gl_VertexID] + 0.5;
        gl_Position = camera * vec4(v_vertex, 1.0);
    }
'''

ctx.shaders['facing_square.frag'] = '''
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
    'facing_square.vert',
    'facing_square.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.empty_vertex_array,
    mode='triangle_strip',
    vertices=4,
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
