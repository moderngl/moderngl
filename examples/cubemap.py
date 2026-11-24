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

faces = [
    'examples/data/sky/xpos.jpg',
    'examples/data/sky/xneg.jpg',
    'examples/data/sky/ypos.jpg',
    'examples/data/sky/yneg.jpg',
    'examples/data/sky/zpos.jpg',
    'examples/data/sky/zneg.jpg',
]

skybox = ctx.texture((512, 512), 'rgba8unorm', b''.join(Image.open(path).convert('RGBA').tobytes() for path in faces), cubemap=True)
sampler = ctx.sampler(skybox, min_filter='linear', mag_filter='linear')

vertex_buffer = ctx.buffer(load_glb('examples/data/blob/blob.glb', texcoords=False)['models']['blob'])
uniform_buffer = ctx.buffer(size=80)

ctx.shaders['common'] = '''
    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    uniform samplerCube Skybox;

    vec3 sky(vec3 direction) {
        return texture(Skybox, vec3(direction.x, direction.z, -direction.y)).rgb;
    }
'''

ctx.shaders['skybox.vert'] = '''
    #version 330 core

    vec2 vertices[3] = vec2[](vec2(-1.0, -1.0), vec2(3.0, -1.0), vec2(-1.0, 3.0));

    out vec2 v_vertex;

    void main() {
        gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
        v_vertex = vertices[gl_VertexID];
    }
'''

ctx.shaders['skybox.frag'] = '''
    #version 330 core
    #include "common"

    in vec2 v_vertex;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec4 far = inverse(camera) * vec4(v_vertex, 1.0, 1.0);
        out_color = vec4(sky(far.xyz / far.w - camera_position.xyz), 1.0);
    }
'''

ctx.shaders['blob.vert'] = '''
    #version 330 core
    #include "common"

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;

    out vec3 v_vertex;
    out vec3 v_normal;

    void main() {
        v_vertex = in_vertex;
        v_normal = in_normal;
        gl_Position = camera * vec4(v_vertex, 1.0);
    }
'''

ctx.shaders['blob.frag'] = '''
    #version 330 core
    #include "common"

    in vec3 v_vertex;
    in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 view = normalize(v_vertex - camera_position.xyz);
        out_color = vec4(sky(reflect(view, normalize(v_normal))), 1.0);
    }
'''

resources = ctx.resources(samplers={0: sampler}, uniform_buffers={0: uniform_buffer})

background = ctx.pipeline(
    'skybox.vert',
    'skybox.frag',
    samplers={'Skybox': 0},
    uniform_buffers={'Common': 0},
    vertex_array=ctx.vertex_array([]),
    framebuffer=framebuffer,
    resources=resources,
    vertices=3,
)

blob = ctx.pipeline(
    'blob.vert',
    'blob.frag',
    samplers={'Skybox': 0},
    uniform_buffers={'Common': 0},
    vertex_array=ctx.vertex_array(vertex_buffer.bind('3f 3f', 0, 1)),
    framebuffer=framebuffer,
    resources=resources,
    depth_test=True,
    cull_face='back',
)

yaw, pitch, spin = 0.0, 0.2, 0.2

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
            pitch = min(max(pitch + event.rel[1] * 0.004, -1.4), 1.4)

    eye = (math.cos(yaw) * math.cos(pitch) * 4.0, math.sin(yaw) * math.cos(pitch) * 4.0, math.sin(pitch) * 4.0)

    ctx.start_frame()
    camera = glm.perspective(math.radians(60.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.0), (0.0, 0.0, 1.0))
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x', *eye))
    framebuffer.clear()
    background.run()
    blob.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
