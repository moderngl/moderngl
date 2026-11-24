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

vertex_buffer = ctx.buffer(load_glb('examples/data/blob/blob.glb', texcoords=False)['scene'])

light_position = [(0.0, 3.0, 5.0), (2.6, -1.5, 5.0), (-2.6, -1.5, 5.0)]
light_color = [(1.0, 0.0, 0.0, 10.0), (0.0, 1.0, 0.0, 10.0), (0.0, 0.0, 1.0, 10.0)]
light_matrix = [glm.perspective(math.radians(45.0), 1.0, 0.1, 1000.0) * glm.lookAt(p, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0)) for p in light_position]

shadow_maps = ctx.texture((1024, 1024), 'r32float', array=3)
shadow_depth = ctx.texture((1024, 1024), 'depth24plus')
shadow_framebuffers = []
for i in range(3):
    shadow_framebuffer = ctx.framebuffer([shadow_maps.attachment(layer=i), shadow_depth])
    shadow_framebuffer.clear_color = 1e6
    shadow_framebuffers.append(shadow_framebuffer)

uniform_buffer = ctx.buffer(size=368)

ctx.shaders['shadow_depth.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
        mat4 light_matrix[3];
        vec4 light_position[3];
        vec4 light_color[3];
    };

    uniform int light;

    layout (location = 0) in vec3 in_vertex;

    out vec3 v_vertex;

    void main() {
        v_vertex = in_vertex;
        gl_Position = light_matrix[light] * vec4(v_vertex, 1.0);
    }
'''

ctx.shaders['shadow_depth.frag'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
        mat4 light_matrix[3];
        vec4 light_position[3];
        vec4 light_color[3];
    };

    uniform int light;

    in vec3 v_vertex;

    layout (location = 0) out float out_depth;

    void main() {
        out_depth = distance(light_position[light].xyz, v_vertex);
    }
'''

ctx.shaders['shadow.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
        mat4 light_matrix[3];
        vec4 light_position[3];
        vec4 light_color[3];
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

ctx.shaders['shadow.frag'] = '''
    #version 330 core

    uniform sampler2DArray Shadow;

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
        mat4 light_matrix[3];
        vec4 light_position[3];
        vec4 light_color[3];
    };

    in vec3 v_vertex;
    in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 normal = normalize(v_normal);
        vec3 view_dir = normalize(camera_position.xyz - v_vertex);
        vec3 color = vec3(0.01 + max(dot(view_dir, normal), 0.0) * 0.03);

        for (int i = 0; i < 3; ++i) {
            vec4 tmp = light_matrix[i] * vec4(v_vertex, 1.0);
            float shadow = texture(Shadow, vec3(tmp.xy / tmp.w * 0.5 + 0.5, float(i))).r;
            float lit = float(shadow + 0.02 > distance(light_position[i].xyz, v_vertex));
            vec3 light_dir = light_position[i].xyz - v_vertex;
            float light_distance = dot(light_dir, light_dir);
            light_dir = normalize(light_dir);
            float lambertian = max(dot(light_dir, normal), 0.0);
            vec3 half_dir = normalize(light_dir + view_dir);
            float specular = pow(max(dot(half_dir, normal), 0.0), 32.0) * float(lambertian > 0.0);
            color += (lambertian + specular) * light_color[i].rgb * light_color[i].a / light_distance * lit;
        }

        out_color = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
    }
'''

vertex_array = ctx.vertex_array(vertex_buffer.bind('3f 3f', 0, 1))

shadow_pipelines = []
for i in range(3):
    shadow_pipeline = ctx.pipeline(
        'shadow_depth.vert',
        'shadow_depth.frag',
        uniform_buffers={'Common': 0},
        uniforms={'light': 'int'},
        vertex_array=vertex_array,
        framebuffer=shadow_framebuffers[i],
        resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
        depth_test=True,
        cull_face='back',
    )
    shadow_pipeline['light'] = i
    shadow_pipelines.append(shadow_pipeline)

scene = ctx.pipeline(
    'shadow.vert',
    'shadow.frag',
    samplers={'Shadow': 0},
    uniform_buffers={'Common': 0},
    vertex_array=vertex_array,
    framebuffer=framebuffer,
    resources=ctx.resources(
        uniform_buffers={0: uniform_buffer},
        samplers={0: ctx.sampler(shadow_maps, min_filter='nearest', mag_filter='nearest')},
    ),
    depth_test=True,
    cull_face='back',
)

lights = b''.join(m.to_bytes() for m in light_matrix) + struct.pack('3f4x3f4x3f4x', *light_position[0], *light_position[1], *light_position[2]) + struct.pack('12f', *light_color[0], *light_color[1], *light_color[2])

yaw, pitch, spin = 0.59, 0.49, 0.2

running = True
while running:
    if spin:
        yaw = pygame.time.get_ticks() / 1000.0 * spin + 0.59
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.MOUSEMOTION and event.buttons[0]:
            spin = 0.0
            yaw -= event.rel[0] * 0.004
            pitch = min(max(pitch + event.rel[1] * 0.004, -0.5), 1.4)

    eye = (math.cos(yaw) * math.cos(pitch) * 5.0, math.sin(yaw) * math.cos(pitch) * 5.0, math.sin(pitch) * 5.0)

    ctx.start_frame()
    for shadow_framebuffer, shadow_pipeline in zip(shadow_framebuffers, shadow_pipelines):
        shadow_framebuffer.clear()
        shadow_pipeline.run()
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x', *eye) + lights)
    framebuffer.clear()
    scene.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
