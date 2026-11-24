import math
import os
import struct

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
uniform_buffer = ctx.buffer(size=112)
common = ctx.resources(uniform_buffers={0: uniform_buffer})

ctx.shaders['hello_raymarching.vert'] = '''
    #version 330 core

    vec2 vertices[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2(3.0, -1.0),
        vec2(-1.0, 3.0)
    );

    out vec2 v_vertex;

    void main() {
        gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
        v_vertex = vertices[gl_VertexID];
    }
'''

ctx.shaders['hello_raymarching.frag'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
        vec4 light_position;
        float time;
    };

    float sdf_soft_min(float a, float b, float k) {
        float h = max(k - abs(a - b), 0.0) / k;
        return min(a, b) - h * h * k * (1.0 / 4.0);
    }

    float sdf_sphere(vec3 p, float r) {
        return length(p) - r;
    }

    float sdf_plane(vec3 p, vec3 n, float h) {
        return dot(p, n) + h;
    }

    float sdf_scene(vec3 p) {
        float d1 = sdf_sphere(p - vec3(0.0, 0.0, sin(time * 6.0) * 0.1), 0.1);
        float d2 = sdf_sphere(p - vec3(0.0, -0.3, sin(time * 6.0 - 2.0) * 0.1), 0.1);
        float d3 = sdf_sphere(p - vec3(0.0, 0.3, sin(time * 6.0 + 2.0) * 0.1), 0.1);
        float d4 = sdf_plane(p - vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), 0.0);
        return sdf_soft_min(min(min(d1, d2), d3), d4, 0.1);
    }

    vec3 sdf_normal(vec3 p) {
        vec2 e = vec2(0.0001, 0.0);
        return normalize(vec3(
            sdf_scene(p + e.xyy) - sdf_scene(p - e.xyy),
            sdf_scene(p + e.yxy) - sdf_scene(p - e.yxy),
            sdf_scene(p + e.yyx) - sdf_scene(p - e.yyx)
        ));
    }

    float soft_checkers(vec3 p) {
        vec3 q = floor(p * 12.0);
        return mod(q.x + q.y, 2.0) - 1.0;
    }

    in vec2 v_vertex;

    layout (location = 0) out vec4 out_color;

    void main() {
        mat4 inv_camera = inverse(camera);
        vec4 position_temp = inv_camera * vec4(v_vertex, -1.0, 1.0);
        vec4 target_temp = inv_camera * vec4(v_vertex, 1.0, 1.0);
        vec3 position = position_temp.xyz / position_temp.w;
        vec3 target = target_temp.xyz / target_temp.w;
        vec3 direction = normalize(target - position);

        for (int i = 0; i < 64; i++) {
            position += direction * sdf_scene(position);
        }

        vec3 normal = sdf_normal(position);
        vec3 light_dir = light_position.xyz - position;
        float light = max(dot(normal, normalize(light_dir)), 0.0);
        light /= exp(length(light_dir) * 2.0);
        light = light * 0.99 + 0.01;
        vec3 color = (vec3(0.2, 0.4, 1.0) * soft_checkers(position) * 0.5 + 0.5) * light;
        float fog = smoothstep(1.5, 4.0, length(position - camera_position.xyz));
        color = mix(color, vec3(pow(0.05, 2.2)), fog);
        out_color = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);

        vec4 vertex = camera * vec4(position, 1.0);
        gl_FragDepth = (vertex.z / vertex.w) * 0.5 + 0.5;
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
    'hello_raymarching.vert',
    'hello_raymarching.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=framebuffer,
    resources=common,
    depth_test=True,
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
    uniform_buffer.write(struct.pack('64s3f4x3f4xf', camera, *eye, *light, now))
    pipeline.run()
    debug_light.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
