import math
import os
import struct

import array
import glm
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
depth = ctx.texture(size, 'depth24plus', multisampled=True)
framebuffer = ctx.framebuffer([image, depth])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

vertex_buffer = ctx.buffer(array.array('f', [
    -0.5, -0.5, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
    -0.5, 0.5, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0,
    0.5, -0.5, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0,
]))

index_buffer = ctx.buffer(array.array('i', [0, 2, 1, 1, 2, 3]), index=True)
uniform_buffer = ctx.buffer(size=96)
texture = ctx.texture((8, 8), 'rgba8unorm', os.urandom(8 * 8 * 4))
common = ctx.resources(uniform_buffers={0: uniform_buffer})

ctx.shaders['hello_model.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
        vec4 light_position;
    };

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;
    layout (location = 2) in vec2 in_uv;

    out vec3 v_vertex;
    out vec3 v_normal;
    out vec2 v_uv;

    void main() {
        v_vertex = in_vertex;
        v_normal = in_normal;
        v_uv = in_uv;
        gl_Position = camera * vec4(v_vertex, 1.0);
    }
'''

ctx.shaders['hello_model.frag'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
        vec4 light_position;
    };

    uniform sampler2D Texture;

    in vec3 v_vertex;
    in vec3 v_normal;
    in vec2 v_uv;

    layout (location = 0) out vec4 out_color;

    vec3 blinn_phong(
            vec3 vertex, vec3 normal, vec3 camera_position, vec3 light_position, float shininess, vec3 ambient_color,
            vec3 diffuse_color, vec3 light_color, vec3 spec_color, float light_power) {

        vec3 light_dir = light_position - vertex;
        float light_distance = length(light_dir);
        light_distance = light_distance * light_distance;
        light_dir = normalize(light_dir);

        float lambertian = max(dot(light_dir, normal), 0.0);
        float specular = 0.0;

        if (lambertian > 0.0) {
            vec3 view_dir = normalize(camera_position - vertex);
            vec3 half_dir = normalize(light_dir + view_dir);
            float spec_angle = max(dot(half_dir, normal), 0.0);
            specular = pow(spec_angle, shininess);
        }

        vec3 color_linear = ambient_color +
            diffuse_color * lambertian * light_color * light_power / light_distance +
            spec_color * specular * light_color * light_power / light_distance;

        return color_linear;
    }

    void main() {
        vec3 diffuse_color = texture(Texture, v_uv).rgb;
        vec3 ambient_color = diffuse_color * 0.1;
        float light_power = 0.1;

        vec3 color = blinn_phong(
            v_vertex, v_normal, camera_position.xyz, light_position.xyz, 16.0,
            ambient_color, diffuse_color, vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), light_power
        );
        out_color = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
    }
'''

ctx.shaders['debug_light.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
        vec4 light_position;
    };

    vec3 vertices[36] = vec3[](
        vec3(-0.5, -0.5, -0.5),
        vec3(-0.5, 0.5, -0.5),
        vec3(0.5, 0.5, -0.5),
        vec3(0.5, 0.5, -0.5),
        vec3(0.5, -0.5, -0.5),
        vec3(-0.5, -0.5, -0.5),
        vec3(-0.5, -0.5, 0.5),
        vec3(0.5, -0.5, 0.5),
        vec3(0.5, 0.5, 0.5),
        vec3(0.5, 0.5, 0.5),
        vec3(-0.5, 0.5, 0.5),
        vec3(-0.5, -0.5, 0.5),
        vec3(-0.5, -0.5, -0.5),
        vec3(0.5, -0.5, -0.5),
        vec3(0.5, -0.5, 0.5),
        vec3(0.5, -0.5, 0.5),
        vec3(-0.5, -0.5, 0.5),
        vec3(-0.5, -0.5, -0.5),
        vec3(0.5, -0.5, -0.5),
        vec3(0.5, 0.5, -0.5),
        vec3(0.5, 0.5, 0.5),
        vec3(0.5, 0.5, 0.5),
        vec3(0.5, -0.5, 0.5),
        vec3(0.5, -0.5, -0.5),
        vec3(0.5, 0.5, -0.5),
        vec3(-0.5, 0.5, -0.5),
        vec3(-0.5, 0.5, 0.5),
        vec3(-0.5, 0.5, 0.5),
        vec3(0.5, 0.5, 0.5),
        vec3(0.5, 0.5, -0.5),
        vec3(-0.5, 0.5, -0.5),
        vec3(-0.5, -0.5, -0.5),
        vec3(-0.5, -0.5, 0.5),
        vec3(-0.5, -0.5, 0.5),
        vec3(-0.5, 0.5, 0.5),
        vec3(-0.5, 0.5, -0.5)
    );

    void main() {
        gl_Position = camera * vec4(light_position.xyz + vertices[gl_VertexID] * 0.025, 1.0);
    }
'''

ctx.shaders['debug_light.frag'] = '''
    #version 330 core

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(1.0, 1.0, 1.0, 1.0);
    }
'''

pipeline = ctx.pipeline(
    'hello_model.vert',
    'hello_model.frag',
    samplers={'Texture': 0},
    uniform_buffers={'Common': 0},
    vertex_array=ctx.vertex_array(vertex_buffer.bind('3f 3f 2f', 0, 1, 2), index_buffer),
    framebuffer=framebuffer,
    resources=ctx.resources(
        common,
        samplers={
            0: ctx.sampler(
                texture,
                wrap_x='clamp_to_edge',
                wrap_y='clamp_to_edge',
                min_filter='nearest',
                mag_filter='nearest',
            ),
        },
    ),
    depth_test=True,
    cull_face='back',
)

debug_light = ctx.pipeline(
    'debug_light.vert',
    'debug_light.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.empty_vertex_array,
    vertices=36,
    framebuffer=framebuffer,
    resources=common,
    depth_test=True,
    cull_face='back',
)

yaw, pitch, spin = 0.0, 0.46, 0.2

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
            pitch = min(max(pitch + event.rel[1] * 0.004, 0.05), 1.4)

    now = pygame.time.get_ticks() / 1000

    ctx.start_frame()
    framebuffer.clear()
    light = (math.cos(now * 0.6) * 0.4, 0.0, abs(math.sin(now * 0.6) * 0.4) + 0.01)
    eye = (math.cos(yaw) * math.cos(pitch) * 1.118, math.sin(yaw) * math.cos(pitch) * 1.118, math.sin(pitch) * 1.118)
    camera = (glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))).to_bytes()
    uniform_buffer.write(struct.pack('64s3f4x3f4x', camera, *eye, *light))
    pipeline.run()
    debug_light.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
