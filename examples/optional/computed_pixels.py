import os

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

image = ctx.texture(size, 'rgba8unorm')
pixel_buffer = ctx.buffer(size=size[0] * size[1] * 4)

ctx.shaders['computed_pixels.comp'] = '''
    #version 430 core

    layout (local_size_x = 8, local_size_y = 8) in;

    layout (std430) writeonly buffer Pixels {
        uint pixels[];
    };

    uniform ivec2 size;
    uniform float time;

    void main() {
        ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
        if (coord.x >= size.x || coord.y >= size.y) {
            return;
        }
        vec2 p = (vec2(coord) + 0.5) / 180.0;
        float t = time * 0.3;
        for (int i = 1; i < 9; ++i) {
            p += vec2(sin(p.y * float(i) + t), cos(p.x * float(i) - t * 0.7)) * 0.6 / float(i);
        }
        float v = sin(p.x * 3.0) + cos(p.y * 3.0);
        vec3 color = 0.5 + 0.5 * cos(6.2831 * (v * 0.25 + t * 0.1 + vec3(0.5, 0.6, 0.7)));
        pixels[coord.y * size.x + coord.x] = packUnorm4x8(vec4(pow(color, vec3(1.5)) * 0.8, 1.0));
    }
'''

compute = ctx.compute(
    'computed_pixels.comp',
    storage_buffers={'Pixels': 0},
    uniforms={'size': 'ivec2', 'time': 'float'},
    resources=ctx.resources(storage_buffers={0: pixel_buffer}),
    group_count=((size[0] + 7) // 8, (size[1] + 7) // 8, 1),
)

compute['size'] = size

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    now = pygame.time.get_ticks() / 1000.0

    ctx.start_frame()
    compute['time'] = now
    compute.run()
    ctx.memory_barrier()
    ctx.copy_buffer_to_texture(pixel_buffer, image)
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
