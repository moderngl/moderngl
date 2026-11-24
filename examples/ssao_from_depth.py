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
image = ctx.texture(size, 'rgba8unorm')
depth = ctx.texture(size, 'depth24plus')
output = ctx.texture(size, 'rgba8unorm')
scene_framebuffer = ctx.framebuffer([image, depth])
scene_framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

vertex_buffer = ctx.buffer(load_glb('examples/data/blob/blob.glb', texcoords=False)['scene'])

uniform_buffer = ctx.buffer(size=80)

ctx.shaders['scene.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;

    out vec3 v_normal;

    void main() {
        gl_Position = camera * vec4(in_vertex, 1.0);
        v_normal = in_normal;
    }
'''

ctx.shaders['scene.frag'] = '''
    #version 330 core

    in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 light = vec3(4.0, 3.0, 10.0);
        float lum = dot(normalize(light), normalize(v_normal)) * 0.7 + 0.3;
        out_color = vec4(lum, lum, lum, 1.0);
    }
'''

ctx.shaders['ssao.vert'] = '''
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

ctx.shaders['ssao.frag'] = '''
    #version 330 core

    uniform sampler2D DepthTexture;

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };
    uniform vec2 screen_size;

    layout (location = 0) out vec4 out_color;

    vec3 rays[64] = vec3[](
        vec3(-0.1669, -0.2530, 0.7978),
        vec3(0.3279, 0.3762, -0.4236),
        vec3(0.5932, -0.7286, -0.0465),
        vec3(0.5772, -0.1356, 0.5708),
        vec3(0.4421, -0.4194, 0.6556),
        vec3(-0.0442, 0.8306, -0.0454),
        vec3(0.2894, -0.8076, 0.2640),
        vec3(0.0126, 0.1203, 0.2371),
        vec3(0.6201, 0.0229, 0.7709),
        vec3(0.0982, 0.4849, -0.7408),
        vec3(0.0827, -0.9779, -0.0524),
        vec3(0.2060, 0.6016, -0.2310),
        vec3(0.4070, 0.6457, -0.1941),
        vec3(-0.5418, -0.4567, -0.1954),
        vec3(-0.3993, 0.3543, -0.7066),
        vec3(-0.5517, 0.1593, 0.2889),
        vec3(-0.3070, 0.2249, -0.2678),
        vec3(0.0814, 0.0627, -0.3199),
        vec3(0.1396, 0.2884, 0.0554),
        vec3(-0.3397, -0.3160, 0.5864),
        vec3(-0.0263, 0.6802, -0.3865),
        vec3(-0.2338, 0.0005, -0.0958),
        vec3(0.7629, 0.3395, 0.2867),
        vec3(-0.4334, 0.1431, 0.0225),
        vec3(0.7727, 0.0063, -0.3851),
        vec3(-0.2055, -0.7455, -0.3394),
        vec3(-0.8598, -0.3002, 0.3296),
        vec3(-0.2444, 0.1926, 0.6121),
        vec3(0.0156, 0.4316, -0.4928),
        vec3(-0.3591, 0.2297, -0.0925),
        vec3(0.1755, 0.3753, 0.4857),
        vec3(0.4496, 0.6460, -0.4983),
        vec3(0.1465, 0.2771, -0.1831),
        vec3(-0.5371, -0.4448, -0.3870),
        vec3(-0.3523, 0.9323, -0.0012),
        vec3(0.7249, -0.3177, -0.5820),
        vec3(-0.0412, 0.6660, -0.0729),
        vec3(0.7068, -0.2089, -0.6529),
        vec3(-0.8477, -0.3353, 0.2442),
        vec3(-0.4609, -0.6685, 0.3376),
        vec3(0.1253, -0.5237, -0.8342),
        vec3(0.2743, 0.3119, 0.2098),
        vec3(0.1949, 0.4532, -0.4878),
        vec3(0.9794, 0.0645, 0.0309),
        vec3(-0.6439, 0.4506, -0.0801),
        vec3(0.0337, 0.4867, -0.5241),
        vec3(0.8590, -0.4309, -0.0291),
        vec3(-0.1975, 0.2013, -0.9400),
        vec3(0.8519, 0.2340, 0.1364),
        vec3(-0.1198, -0.6137, 0.7647),
        vec3(0.6107, 0.1842, -0.3515),
        vec3(0.5673, -0.0507, 0.6820),
        vec3(-0.7303, -0.6290, 0.1026),
        vec3(-0.5826, 0.0610, -0.4710),
        vec3(0.1891, 0.1981, -0.5424),
        vec3(-0.3972, 0.5994, -0.6029),
        vec3(0.0783, 0.7544, -0.2385),
        vec3(0.4750, -0.4954, 0.2913),
        vec3(-0.4554, 0.5358, -0.4386),
        vec3(-0.4905, -0.6994, -0.0057),
        vec3(0.7436, 0.5501, -0.0728),
        vec3(0.6827, -0.0257, -0.7134),
        vec3(-0.5150, 0.0274, 0.1220),
        vec3(0.7423, -0.6506, 0.1338)
    );

    vec3 position(vec2 xy) {
        mat4 inv_camera = inverse(camera);
        float depth = texelFetch(DepthTexture, ivec2(xy), 0).r;
        vec2 uv = xy / screen_size * 2.0 - 1.0;
        vec4 point = inv_camera * vec4(uv, depth * 2.0 - 1.0, 1.0);
        return point.xyz / point.w;
    }

    float depth_difference(vec3 point) {
        vec4 t = camera * vec4(point, 1.0);
        vec3 v = (t.xyz / t.w) * 0.5 + 0.5;
        float d = texture(DepthTexture, v.xy).r;
        return d - v.z;
    }

    vec3 noise(vec2 p) {
        int x = int(p.x) % 2 * 2 + int(p.y) % 2;
        if (x == 0) {
            return vec3(1.0, 0.0, 0.0);
        }
        if (x == 1) {
            return vec3(0.0, 1.0, 0.0);
        }
        if (x == 2) {
            return vec3(0.0, 0.0, 1.0);
        }
        return normalize(vec3(-1.0, -1.0, -1.0));
    }

    void main() {
        if (texelFetch(DepthTexture, ivec2(gl_FragCoord.xy), 0).r == 1.0) {
            out_color = vec4(0.05, 0.05, 0.05, 1.0);
            return;
        }
        vec3 point = position(gl_FragCoord.xy);
        float lum = 0.0;
        for (int i = 0; i < 64; i++) {
            vec3 ray = reflect(rays[i] * 0.15, noise(gl_FragCoord.xy));
            if (depth_difference(point + ray) > -1e-4) {
                lum += 1.0 / 64.0;
            }
        }
        out_color = vec4(lum, lum, lum, 1.0);
    }
'''

scene = ctx.pipeline(
    'scene.vert',
    'scene.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.vertex_array(vertex_buffer.bind('3f 3f', 0, 1)),
    framebuffer=scene_framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    depth_test=True,
    cull_face='back',
)

ssao = ctx.pipeline(
    'ssao.vert',
    'ssao.frag',
    samplers={'DepthTexture': 0},
    uniform_buffers={'Common': 0},
    uniforms={'screen_size': 'vec2'},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=ctx.framebuffer([output]),
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}, 
        samplers={
            0: ctx.sampler(depth, min_filter='nearest', mag_filter='nearest', wrap_x='clamp_to_edge', wrap_y='clamp_to_edge'),
        },
    ),
)

ssao['screen_size'] = size

yaw, pitch, spin = 1.7, 0.3, 0.2

running = True
while running:
    if spin:
        yaw = pygame.time.get_ticks() / 1000.0 * spin + 1.7
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.MOUSEMOTION and event.buttons[0]:
            spin = 0.0
            yaw -= event.rel[0] * 0.004
            pitch = min(max(pitch + event.rel[1] * 0.004, -0.3), 1.4)

    eye = (math.cos(yaw) * math.cos(pitch) * 4.5, math.sin(yaw) * math.cos(pitch) * 4.5, 0.5 + math.sin(pitch) * 4.5)
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.5), (0.0, 0.0, 1.0))

    ctx.start_frame()
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x', *eye))
    scene_framebuffer.clear()
    scene.run()
    ssao.run()
    ctx.present(output)

    pygame.display.flip()

pygame.quit()
