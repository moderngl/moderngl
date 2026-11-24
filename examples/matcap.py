import math
import os
import struct
import sys

import glm
import moderngl
import pygame
from PIL import Image

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

matcaps = [
    ctx.texture((512, 512), 'rgba8unorm', Image.open(path).convert('RGBA').transpose(Image.FLIP_TOP_BOTTOM).tobytes())
    for path in [
        'examples/data/matcap/metal_carpaint.jpg',
        'examples/data/matcap/fullmetal.jpg',
        'examples/data/matcap/ceramic_lightbulb.jpg',
    ]
]

blob = load_glb('examples/data/blob/blob.glb', texcoords=False)['models']['blob']
vertices = len(blob) // 24

vertex_data = bytearray()
for offset in (-3.6, 0.0, 3.6):
    for x, y, z, nx, ny, nz in struct.iter_unpack('6f', blob):
        vertex_data += struct.pack('6f', x + offset, y, z, nx, ny, nz)

vertex_buffer = ctx.buffer(vertex_data)

uniform_buffer = ctx.buffer(size=128)

ctx.shaders['background.vert'] = '''
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

ctx.shaders['background.frag'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        mat4 view;
    };

    in vec2 v_vertex;

    layout (location = 0) out vec4 out_color;

    void main() {
        mat4 inv_camera = inverse(camera);
        vec4 position_temp = inv_camera * vec4(v_vertex, -1.0, 1.0);
        vec4 target_temp = inv_camera * vec4(v_vertex, 1.0, 1.0);
        vec3 position = position_temp.xyz / position_temp.w;
        vec3 target = target_temp.xyz / target_temp.w;
        vec3 direction = normalize(target - position);

        vec3 color;
        if (direction.z > 0.0) {
            color = mix(vec3(1.0, 1.0, 1.0), vec3(0.1, 0.5, 0.9), pow(direction.z, 0.4));
        } else {
            color = mix(vec3(1.0, 1.0, 1.0), vec3(0.2, 0.2, 0.2), pow(-direction.z, 0.1));
        }

        out_color = vec4(color, 1.0);
    }
'''

ctx.shaders['matcap.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        mat4 view;
    };

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;

    out vec3 v_normal;

    void main() {
        gl_Position = camera * vec4(in_vertex, 1.0);
        v_normal = mat3(view) * in_normal;
    }
'''

ctx.shaders['matcap.frag'] = '''
    #version 330 core

    uniform sampler2D Matcap;

    in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec2 uv = vec2(0.5, 0.5) + normalize(v_normal).xy * 0.49;
        out_color = vec4(texture(Matcap, uv).rgb, 1.0);
    }
'''

background = ctx.pipeline(
    'background.vert',
    'background.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
)

vertex_array = ctx.vertex_array(vertex_buffer.bind('3f 3f', 0, 1))

models = [
    ctx.pipeline(
        'matcap.vert',
        'matcap.frag',
        samplers={'Matcap': 0},
        uniform_buffers={'Common': 0},
        vertex_array=vertex_array,
        framebuffer=framebuffer,
        resources=ctx.resources(uniform_buffers={0: uniform_buffer}, samplers={0: ctx.sampler(matcap, wrap_x='clamp_to_edge', wrap_y='clamp_to_edge')}),
        first=i * vertices,
        vertices=vertices,
        depth_test=True,
        cull_face='back',
    )
    for i, matcap in enumerate(matcaps)
]

yaw, pitch, spin = -math.pi / 2.0, 0.2, 0.2

running = True
while running:
    now = pygame.time.get_ticks() / 1000.0
    if spin:
        yaw = now * spin - math.pi / 2.0
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.MOUSEMOTION and event.buttons[0]:
            spin = 0.0
            yaw -= event.rel[0] * 0.004
            pitch = min(max(pitch + event.rel[1] * 0.004, -1.4), 1.4)

    eye = (math.cos(yaw) * math.cos(pitch) * 9.0, math.sin(yaw) * math.cos(pitch) * 9.0, math.sin(pitch) * 9.0)
    view = glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * view

    ctx.start_frame()
    uniform_buffer.write(camera.to_bytes() + view.to_bytes())
    framebuffer.clear()
    background.run()
    for model in models:
        model.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
