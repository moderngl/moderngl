import math
import os

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
uniform_buffer = ctx.buffer(size=64)

count = 200

blade = []
for i in range(7):
    u = i / 7
    v = math.sin(u * u * (math.pi - 1.0) + 1.0)
    blade.append((-v * 0.03, u * u * 0.2, u))
    blade.append((v * 0.03, u * u * 0.2, u))
blade.append((0.0, 0.2, 1.0))

ctx.shaders['grass.vert'] = '''
    #version 330 core

    uniform int N;
    uniform vec3 grass[15];

    vec4 hash41(float p) {
        vec4 p4 = fract(vec4(p) * vec4(0.1031, 0.1030, 0.0973, 0.1099));
        p4 += dot(p4, p4.wzxy + 33.33);
        return fract((p4.xxyz + p4.yzzw) * p4.zywx);
    }

    float hash11(float p) {
        p = fract(p * 0.1031);
        p *= p + 33.33;
        p *= p + p;
        return fract(p);
    }

    layout (std140) uniform Common {
        mat4 camera;
    };

    out vec3 v_data;

    void main() {
        vec3 v = grass[gl_VertexID];
        vec4 data = hash41(float(gl_InstanceID));
        vec2 cell = vec2(float(gl_InstanceID % N), float(gl_InstanceID / N));
        float height = (sin(cell.x * 0.1) + cos(cell.y * 0.1)) * 0.2;
        float scale = 0.9 + hash11(float(gl_InstanceID)) * 0.2;
        data.xy = (data.xy + cell - float(N / 2)) * 0.1;
        data.z *= 6.283184;
        vec3 vert = vec3(
            data.x + cos(data.z) * v.x + sin(data.z) * v.y,
            data.y + cos(data.z) * v.y - sin(data.z) * v.x,
            height + v.z
        );
        vert *= scale;
        gl_Position = camera * vec4(vert, 1.0);
        float dead = step(0.97, hash11(float(gl_InstanceID) + 17.0));
        v_data = vec3(data.w, v.z, dead);
    }
'''

ctx.shaders['grass.frag'] = '''
    #version 330 core

    in vec3 v_data;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 yl = pow(vec3(0.85, 0.82, 0.2), vec3(2.2));
        vec3 gn = pow(vec3(0.12, 0.45, 0.1), vec3(2.2));
        vec3 color = mix(yl, gn, v_data.x) * pow(v_data.y, 3.5) * (1.0 - v_data.z * 0.5);
        out_color = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
    }
'''

pipeline = ctx.pipeline(
    'grass.vert',
    'grass.frag',
    uniform_buffers={'Common': 0},
    uniforms={'N': 'int', 'grass': 'vec3[15]'},
    vertex_array=ctx.empty_vertex_array,
    vertices=15,
    mode='triangle_strip',
    framebuffer=framebuffer,
    resources=ctx.resources(
        uniform_buffers={
            0: uniform_buffer,
        },
    ),
    instances=count * count,
    depth_test=True,
)

pipeline['N'] = count
pipeline['grass'] = blade

yaw, pitch, spin = 0.0, 0.32, 0.2

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

    ctx.start_frame()
    eye = (math.cos(yaw) * math.cos(pitch) * 12.65, math.sin(yaw) * math.cos(pitch) * 12.65, math.sin(pitch) * 12.65)
    uniform_buffer.write((glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))).to_bytes())
    framebuffer.clear()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
