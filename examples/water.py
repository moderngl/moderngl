import os

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
image = ctx.texture(size, 'rgba8unorm')

ctx.shaders['water.vert'] = '''
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

ctx.shaders['water.frag'] = '''
    #version 330 core

    // Based on "Very fast procedural ocean" by afl_ext (MIT License), modified
    // https://www.shadertoy.com/view/MdXyzX

    uniform float time;
    uniform vec2 resolution;

    layout (location = 0) out vec4 out_color;

    #define DRAG_MULT 0.1
    #define WATER_DEPTH 0.5
    #define CAMERA_HEIGHT 1.0
    #define ITERATIONS_RAYMARCH 5
    #define ITERATIONS_NORMAL 10

    vec2 wavedx(vec2 position, vec2 direction, float frequency, float timeshift) {
        float x = dot(direction, position) * frequency + timeshift;
        float wave = exp(sin(x) - 1.0);
        float dx = wave * cos(x);
        return vec2(wave, -dx);
    }

    float getwaves(vec2 position, int iterations) {
        float iter = 0.0;
        float frequency = 1.0;
        float timeMultiplier = 2.0;
        float weight = 1.0;
        float sumOfValues = 0.0;
        float sumOfWeights = 0.0;
        for (int i = 0; i < iterations; i++) {
            vec2 p = vec2(sin(iter), cos(iter));
            vec2 res = wavedx(position, p, frequency, time * timeMultiplier);
            position += p * res.y * weight * DRAG_MULT;
            sumOfValues += res.x * weight;
            sumOfWeights += weight;
            weight *= 0.82;
            frequency *= 1.18;
            timeMultiplier *= 1.07;
            iter += 1232.399963;
        }
        return sumOfValues / sumOfWeights;
    }

    float raymarchwater(vec3 camera, vec3 start, vec3 end, float depth) {
        vec3 pos = start;
        vec3 dir = normalize(end - start);
        for (int i = 0; i < 64; i++) {
            float height = getwaves(pos.xz, ITERATIONS_RAYMARCH) * depth - depth;
            if (height + 0.01 > pos.y) {
                return distance(pos, camera);
            }
            pos += dir * (pos.y - height);
        }
        return distance(start, camera);
    }

    vec3 normal(vec2 pos, float e, float depth) {
        vec2 ex = vec2(e, 0);
        float H = getwaves(pos.xy, ITERATIONS_NORMAL) * depth;
        vec3 a = vec3(pos.x, H, pos.y);
        return normalize(
            cross(
                a - vec3(pos.x - e, getwaves(pos.xy - ex.xy, ITERATIONS_NORMAL) * depth, pos.y),
                a - vec3(pos.x, getwaves(pos.xy + ex.yx, ITERATIONS_NORMAL) * depth, pos.y + e)
            )
        );
    }

    mat3 createRotationMatrixAxisAngle(vec3 axis, float angle) {
        float s = sin(angle);
        float c = cos(angle);
        float oc = 1.0 - c;
        return mat3(
            oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s,
            oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s,
            oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c
        );
    }

    vec3 getRay(vec2 fragCoord) {
        vec2 uv = ((fragCoord.xy / resolution.xy) * 2.0 - 1.0) * vec2(resolution.x / resolution.y, 1.0);
        vec3 proj = normalize(vec3(uv.x, uv.y, 1.5));
        return createRotationMatrixAxisAngle(vec3(0.0, -1.0, 0.0), 3.0 * (0.5 * 2.0 - 1.0))
            * createRotationMatrixAxisAngle(vec3(1.0, 0.0, 0.0), 0.5 + 1.5 * ((0.22 * 1.5) * 2.0 - 1.0))
            * proj;
    }

    float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal) {
        return clamp(dot(point - origin, normal) / dot(direction, normal), -1.0, 9991999.0);
    }

    vec3 extra_cheap_atmosphere(vec3 raydir, vec3 sundir) {
        sundir.y = max(sundir.y, -0.07);
        float special_trick = 1.0 / (raydir.y * 1.0 + 0.1);
        float special_trick2 = 1.0 / (sundir.y * 11.0 + 1.0);
        float raysundt = pow(abs(dot(sundir, raydir)), 2.0);
        float sundt = pow(max(0.0, dot(sundir, raydir)), 8.0);
        float mymie = sundt * special_trick * 0.2;
        vec3 suncolor = mix(vec3(1.0), max(vec3(0.0), vec3(1.0) - vec3(5.5, 13.0, 22.4) / 22.4), special_trick2);
        vec3 bluesky = vec3(5.5, 13.0, 22.4) / 22.4 * suncolor;
        vec3 bluesky2 = max(vec3(0.0), bluesky - vec3(5.5, 13.0, 22.4) * 0.002 * (special_trick + -6.0 * sundir.y * sundir.y));
        bluesky2 *= special_trick * (0.24 + raysundt * 0.24);
        return bluesky2 * (1.0 + 1.0 * pow(1.0 - raydir.y, 3.0)) + mymie * suncolor;
    }

    vec3 getSunDirection() {
        return vec3(0.0, 0.316228, 0.948683);
    }

    vec3 getAtmosphere(vec3 dir) {
        return extra_cheap_atmosphere(dir, getSunDirection()) * 0.5;
    }

    float getSun(vec3 dir) {
        return pow(max(0.0, dot(dir, getSunDirection())), 720.0) * 210.0;
    }

    vec3 aces_tonemap(vec3 color) {
        mat3 m1 = mat3(0.59719, 0.07600, 0.02840, 0.35458, 0.90834, 0.13383, 0.04823, 0.01566, 0.83777);
        mat3 m2 = mat3(1.60475, -0.10208, -0.00327, -0.53108, 1.10813, -0.07276, -0.07367, -0.00605, 1.07602);
        vec3 v = m1 * color;
        vec3 a = v * (v + 0.0245786) - 0.000090537;
        vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
        return pow(clamp(m2 * (a / b), 0.0, 1.0), vec3(1.0 / 2.2));
    }

    void main() {
        vec3 ray = getRay(gl_FragCoord.xy);
        if (ray.y >= 0.0) {
            vec3 C = getAtmosphere(ray) + getSun(ray);
            out_color = vec4(aces_tonemap(C * 2.0), 1.0);
            return;
        }
        vec3 waterPlaneHigh = vec3(0.0, 0.0, 0.0);
        vec3 waterPlaneLow = vec3(0.0, -WATER_DEPTH, 0.0);
        vec3 origin = vec3(0.0, CAMERA_HEIGHT, time);
        float highPlaneHit = intersectPlane(origin, ray, waterPlaneHigh, vec3(0.0, 1.0, 0.0));
        float lowPlaneHit = intersectPlane(origin, ray, waterPlaneLow, vec3(0.0, 1.0, 0.0));
        vec3 highHitPos = origin + ray * highPlaneHit;
        vec3 lowHitPos = origin + ray * lowPlaneHit;
        float dist = raymarchwater(origin, highHitPos, lowHitPos, WATER_DEPTH);
        vec3 waterHitPos = origin + ray * dist;
        vec3 N = normal(waterHitPos.xz, 0.01, WATER_DEPTH);
        N = mix(N, vec3(0.0, 1.0, 0.0), 0.8 * min(1.0, sqrt(dist * 0.01) * 1.1));
        float fresnel = 0.04 + 0.96 * pow(1.0 - max(0.0, dot(-N, ray)), 5.0);
        vec3 R = normalize(reflect(ray, N));
        R.y = abs(R.y);
        vec3 reflection = getAtmosphere(R) + getSun(R);
        vec3 scattering = vec3(0.0293, 0.0698, 0.1717) * (0.2 + (waterHitPos.y + WATER_DEPTH) / WATER_DEPTH);
        vec3 C = fresnel * reflection + (1.0 - fresnel) * scattering;
        out_color = vec4(aces_tonemap(C * 2.0), 1.0);
    }
'''

pipeline = ctx.pipeline(
    'water.vert',
    'water.frag',
    uniforms={'time': 'float', 'resolution': 'vec2'},
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
    pipeline['time'] = pygame.time.get_ticks() / 1000.0
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
