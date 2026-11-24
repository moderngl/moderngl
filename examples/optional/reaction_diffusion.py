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

ctx.shaders['reaction_diffusion.comp'] = '''
    #version 430 core

    layout (local_size_x = 16, local_size_y = 16) in;

    layout (rg32f) uniform image2D State;
    layout (rg32f) uniform image2D Next;

    uniform vec2 brush;

    void main() {
        ivec2 grid = imageSize(State);
        ivec2 p = ivec2(gl_GlobalInvocationID.xy);
        vec2 c = imageLoad(State, p).xy;
        vec2 laplace = -c;
        laplace += 0.2 * imageLoad(State, (p + ivec2(1, 0) + grid) % grid).xy;
        laplace += 0.2 * imageLoad(State, (p + ivec2(-1, 0) + grid) % grid).xy;
        laplace += 0.2 * imageLoad(State, (p + ivec2(0, 1) + grid) % grid).xy;
        laplace += 0.2 * imageLoad(State, (p + ivec2(0, -1) + grid) % grid).xy;
        laplace += 0.05 * imageLoad(State, (p + ivec2(1, 1) + grid) % grid).xy;
        laplace += 0.05 * imageLoad(State, (p + ivec2(-1, 1) + grid) % grid).xy;
        laplace += 0.05 * imageLoad(State, (p + ivec2(1, -1) + grid) % grid).xy;
        laplace += 0.05 * imageLoad(State, (p + ivec2(-1, -1) + grid) % grid).xy;
        float reaction = c.x * c.y * c.y;
        float a = c.x + laplace.x - reaction + 0.037 * (1.0 - c.x);
        float b = c.y + 0.5 * laplace.y + reaction - 0.097 * c.y;
        if (distance(vec2(p), brush) < 6.0) {
            b = 0.9;
        }
        imageStore(Next, p, vec4(clamp(a, 0.0, 1.0), clamp(b, 0.0, 1.0), 0.0, 0.0));
    }
'''

ctx.shaders['fullscreen.vert'] = '''
    #version 330 core

    vec2 vertices[3] = vec2[](vec2(-1.0, -1.0), vec2(3.0, -1.0), vec2(-1.0, 3.0));

    out vec2 v_uv;

    void main() {
        gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
        v_uv = vertices[gl_VertexID] * 0.5 + 0.5;
    }
'''

ctx.shaders['display.frag'] = '''
    #version 330 core

    uniform sampler2D Texture;

    in vec2 v_uv;

    layout (location = 0) out vec4 out_color;

    void main() {
        float b = texture(Texture, v_uv).y;
        vec3 color = mix(vec3(0.02, 0.05, 0.15), vec3(0.4, 0.7, 1.0), smoothstep(0.1, 0.4, b));
        color = mix(color, vec3(1.0), smoothstep(0.35, 0.5, b));
        out_color = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
    }
'''

grid = (size[0] // 2, size[1] // 2)
data = bytearray(struct.pack('2f', 1.0, 0.0) * grid[0] * grid[1])
for _ in range(40):
    cx, cy = random.randrange(grid[0]), random.randrange(grid[1])
    for y in range(cy - 4, cy + 4):
        for x in range(cx - 4, cx + 4):
            struct.pack_into('2f', data, ((y % grid[1]) * grid[0] + x % grid[0]) * 8, 0.5, 0.5)

states = [ctx.texture(grid, 'rg32float', data), ctx.texture(grid, 'rg32float')]
image = ctx.texture(size, 'rgba8unorm')
framebuffer = ctx.framebuffer([image])

steps = [
    ctx.compute(
        'reaction_diffusion.comp',
        storage_images={'State': 0, 'Next': 1},
        uniforms={'brush': 'vec2'},
        resources=ctx.resources(storage_images={0: states[i], 1: states[1 - i]}),
        group_count=((grid[0] + 15) // 16, (grid[1] + 15) // 16, 1),
    )
    for i in range(2)
]

display = ctx.pipeline(
    'fullscreen.vert',
    'display.frag',
    samplers={'Texture': 0},
    vertex_array=ctx.vertex_array([]),
    framebuffer=framebuffer,
    resources=ctx.resources(samplers={0: ctx.sampler(states[0], min_filter='linear', mag_filter='linear')}),
    vertices=3,
)

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    mx, my = pygame.mouse.get_pos()
    brush = (mx / size[0] * grid[0], (1.0 - my / size[1]) * grid[1]) if pygame.mouse.get_pressed()[0] else (-100.0, -100.0)

    ctx.start_frame()
    for _ in range(8):
        for step in steps:
            step['brush'] = brush
            step.run()
            ctx.memory_barrier()
    display.run()
    ctx.present(image)
    pygame.display.flip()

pygame.quit()
