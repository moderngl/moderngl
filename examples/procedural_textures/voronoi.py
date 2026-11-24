import os

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
image = ctx.texture(size, 'rgba8unorm')

ctx.shaders['voronoi.vert'] = '''
    #version 330 core

    vec2 positions[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2(3.0, -1.0),
        vec2(-1.0, 3.0)
    );

    void main() {
        gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
    }
'''

ctx.shaders['voronoi.frag'] = '''
    #version 330 core

    layout (location = 0) out vec4 out_color;

    uniform vec2 resolution;

    struct VoronoiTexture {
        float scale;
        float randomn;
    };

    vec3 hsv2rgb(vec3 c) {
        vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
        vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
        return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
    }

    float hash12(vec2 p) {
        vec3 p3 = fract(vec3(p.xyx) * 0.1031);
        p3 += dot(p3, p3.yzx + 33.33);
        return fract((p3.x + p3.y) * p3.z);
    }

    vec2 hash22(vec2 p) {
        vec3 p3 = fract(vec3(p.xyx) * vec3(0.1031, 0.1030, 0.0973));
        p3 += dot(p3, p3.yzx + 33.33);
        return fract((p3.xx + p3.yz) * p3.zy);
    }

    vec2 voronoi_point(vec2 cell, float randomn) {
        return cell + hash22(cell) * randomn;
    }

    vec2 offsets[9] = vec2[](
        vec2(-1.0, -1.0), vec2(0.0, -1.0), vec2(1.0, -1.0),
        vec2(-1.0, 0.0), vec2(0.0, 0.0), vec2(1.0, 0.0),
        vec2(-1.0, 1.0), vec2(0.0, 1.0), vec2(1.0, 1.0)
    );

    vec2 voronoi_position(vec2 uv, VoronoiTexture mt) {
        vec2 res;
        float dist = 100.0;
        vec2 cell = floor(uv * mt.scale);
        for (int i = 0; i < 9; ++i) {
            vec2 p = voronoi_point(cell + offsets[i], mt.randomn);
            float lng = length(uv * mt.scale - p);
            if (dist > lng) {
                dist = lng;
                res = p;
            }
        }
        return res;
    }

    float voronoi_distance(vec2 uv, VoronoiTexture mt) {
        return length(uv * mt.scale - voronoi_position(uv, mt));
    }

    vec3 voronoi_color(vec2 uv, VoronoiTexture mt) {
        vec2 pos = voronoi_position(uv, mt);
        return hsv2rgb(vec3(hash12(pos), 0.8, 0.8));
    }

    void main() {
        vec2 uv = gl_FragCoord.xy / resolution.y;

        VoronoiTexture mt;
        mt.scale = 5.0;
        mt.randomn = 1.0;

        vec3 color = voronoi_color(uv, mt);
        out_color = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
    }
'''

pipeline = ctx.pipeline(
    'voronoi.vert',
    'voronoi.frag',
    uniforms={'resolution': 'vec2'},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=ctx.framebuffer([image]),
)

pipeline['resolution'] = size

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    ctx.start_frame()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
