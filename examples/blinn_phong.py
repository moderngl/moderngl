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
image = ctx.texture(size, 'rgba8unorm', multisampled=True)
depth = ctx.texture(size, 'depth24plus', multisampled=True)
framebuffer = ctx.framebuffer([image, depth])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

vertex_buffer = ctx.buffer(load_glb('examples/data/blob/blob.glb', texcoords=False)['models']['blob'])

uniform_buffer = ctx.buffer(size=128)
material_buffer = ctx.buffer(struct.pack('3f4x3f4x3f4xf12x', 0.0, 0.01, 0.05, 0.0, 0.1, 0.5, 1.0, 1.0, 1.0, 16.0))

ctx.shaders['blinn_phong.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
        vec4 light_position;
        vec4 light_color;
        float light_power;
    };

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;

    out vec3 v_vertex;
    out vec3 v_normal;

    void main() {
        v_vertex = in_vertex;
        v_normal = in_normal;
        gl_Position = camera * vec4(v_vertex, 1.0);
    }
'''

ctx.shaders['blinn_phong.frag'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
        vec4 light_position;
        vec4 light_color;
        float light_power;
    };

    layout (std140) uniform Material {
        vec4 ambient_color;
        vec4 diffuse_color;
        vec4 specular_color;
        float shininess;
    };

    in vec3 v_vertex;
    in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 normal = normalize(v_normal);
        vec3 light_dir = light_position.xyz - v_vertex;
        float light_distance = dot(light_dir, light_dir);
        light_dir = normalize(light_dir);

        float lambertian = max(dot(light_dir, normal), 0.0);
        vec3 view_dir = normalize(camera_position.xyz - v_vertex);
        vec3 half_dir = normalize(light_dir + view_dir);
        float specular = pow(max(dot(half_dir, normal), 0.0), shininess) * float(lambertian > 0.0);

        vec3 color = ambient_color.rgb +
            diffuse_color.rgb * lambertian * light_color.rgb * light_power / light_distance +
            specular_color.rgb * specular * light_color.rgb * light_power / light_distance;

        out_color = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
    }
'''

pipeline = ctx.pipeline(
    'blinn_phong.vert',
    'blinn_phong.frag',
    uniform_buffers={'Common': 0, 'Material': 1},
    vertex_array=ctx.vertex_array(vertex_buffer.bind('3f 3f', 0, 1)),
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer, 1: material_buffer}),
    depth_test=True,
    cull_face='back',
)


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
            pitch = min(max(pitch + event.rel[1] * 0.004, -1.4), 1.4)

    eye = (math.cos(yaw) * math.cos(pitch) * 5.39, math.sin(yaw) * math.cos(pitch) * 5.39, math.sin(pitch) * 5.39)

    ctx.start_frame()
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x3f4x3f4xf12x', *eye, 2.0, 3.0, 4.0, 1.0, 1.0, 1.0, 40.0))
    framebuffer.clear()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
