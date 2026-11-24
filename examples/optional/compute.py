import math
import os
import random
import struct

import moderngl
import pygame

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'
pygame.display.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 4)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, 1)

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

size = pygame.display.get_window_size()

ctx = moderngl.get_context()

image = ctx.texture(size, 'rgba16float')
framebuffer = ctx.framebuffer([image])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)
output = ctx.texture(size, 'rgba8unorm')

count = 1 << 18

particles = bytearray()
for i in range(count):
    angle = random.uniform(0.0, math.tau)
    radius = random.uniform(0.2, 1.0)
    x, y = math.cos(angle) * radius, math.sin(angle) * radius
    particles += struct.pack('8f', x, y, 0.0, 1.0, -y * 0.8, x * 0.8, 0.0, 0.0)

particle_buffer = ctx.buffer(particles)

ctx.shaders['particles.comp'] = '''
    #version 430 core

    struct Particle {
        vec4 position;
        vec4 velocity;
    };

    layout (local_size_x = 256) in;

    layout (std430) buffer Particles {
        Particle particles[];
    };

    uniform vec2 attractor;
    uniform float strength;

    void main() {
        float dt = 0.016;
        Particle p = particles[gl_GlobalInvocationID.x];
        vec2 offset = attractor - p.position.xy;
        float distance = max(length(offset), 0.05);
        p.velocity.xy += offset / (distance * distance * distance) * strength * dt;
        p.velocity.xy *= pow(0.4, dt);
        p.position.xy += p.velocity.xy * dt;
        particles[gl_GlobalInvocationID.x] = p;
    }
'''

ctx.shaders['particles.vert'] = '''
    #version 330 core

    uniform vec2 scale;

    layout (location = 0) in vec4 in_position;
    layout (location = 1) in vec4 in_velocity;

    out vec3 v_color;

    void main() {
        gl_Position = vec4(in_position.xy * scale, 0.0, 1.0);
        gl_PointSize = 1.5;
        float speed = length(in_velocity.xy);
        v_color = mix(vec3(0.02, 0.1, 0.6), vec3(0.3, 0.7, 1.0), clamp(speed * 0.5, 0.0, 1.0)) * 0.3;
    }
'''

ctx.shaders['particles.frag'] = '''
    #version 330 core

    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(v_color, 1.0);
    }
'''

update = ctx.compute(
    'particles.comp',
    storage_buffers={'Particles': 0},
    uniforms={'attractor': 'vec2', 'strength': 'float'},
    resources=ctx.resources(storage_buffers={0: particle_buffer}),
    group_count=(count // 256, 1, 1),
)

render = ctx.pipeline(
    'particles.vert',
    'particles.frag',
    uniforms={'scale': 'vec2'},
    vertex_array=ctx.vertex_array(particle_buffer.bind('4f 4f', 0, 1), mode='points'),
    framebuffer=framebuffer,
    blend={'src': 'one', 'dst': 'one'},
)

render['scale'] = (size[1] / size[0], 1.0)

sim_time = 0

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    mx, my = pygame.mouse.get_pos()
    update['attractor'] = ((mx / size[0] * 2.0 - 1.0) * size[0] / size[1], 1.0 - my / size[1] * 2.0)
    update['strength'] = 0.6 if pygame.mouse.get_pressed()[0] else 0.15

    ctx.start_frame()
    now = pygame.time.get_ticks()
    sim_time = max(sim_time, now - 100)
    while sim_time < now:
        sim_time += 16
        update.run()
    ctx.memory_barrier()
    framebuffer.clear()
    render.run()
    ctx.copy_texture(image, output)
    ctx.present(output)

    pygame.display.flip()

pygame.quit()
