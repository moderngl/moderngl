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
image = ctx.texture(size, 'rgba8unorm', multisampled=True)
depth = ctx.texture(size, 'depth24plus-stencil8', multisampled=True)
framebuffer = ctx.framebuffer([image, depth])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)
framebuffer.clear_stencil = 1

nodes = load_glb('examples/data/blob/blob.glb', texcoords=False)['nodes']
vertex_buffer = ctx.buffer(b''.join(data for name, data in nodes.items() if name != 'ground'))
vertex_array = ctx.vertex_array(vertex_buffer.bind('3f 3f', 0, 1))

uniform_buffer = ctx.buffer(size=80)

ctx.shaders['scene.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };
    uniform float mirror;

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;

    out vec3 v_normal;

    void main() {
        gl_Position = camera * vec4(in_vertex * vec3(1.0, 1.0, mirror), 1.0);
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

ctx.shaders['shadow.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    layout (location = 0) in vec3 in_vertex;

    void main() {
        vec3 vertex = vec3(in_vertex.xy + vec2(-0.7, -0.4) * in_vertex.z, 0.0);
        gl_Position = camera * vec4(vertex, 1.0);
    }
'''

ctx.shaders['shadow.frag'] = '''
    #version 330 core

    layout (location = 0) out vec4 out_color;

    void main() {
        gl_FragDepth = gl_FragCoord.z - 1e-4;
        out_color = vec4(0.0, 0.0, 0.0, 0.3);
    }
'''

ctx.shaders['plane.vert'] = '''
    #version 330 core

    layout (std140) uniform Common {
        mat4 camera;
        vec4 camera_position;
    };

    vec3 positions[4] = vec3[](
        vec3(-3.0, -3.0, 0.0),
        vec3(-3.0, 3.0, 0.0),
        vec3(3.0, -3.0, 0.0),
        vec3(3.0, 3.0, 0.0)
    );

    void main() {
        gl_Position = camera * vec4(positions[gl_VertexID], 1.0);
    }
'''

ctx.shaders['plane.frag'] = '''
    #version 330 core

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(1.0, 1.0, 1.0, 0.7);
    }
'''

scene = ctx.pipeline(
    'scene.vert',
    'scene.frag',
    uniform_buffers={'Common': 0},
    uniforms={'mirror': 'float'},
    vertex_array=vertex_array,
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    depth_test=True,
    cull_face='back',
)

reflection = ctx.pipeline(
    'scene.vert',
    'scene.frag',
    uniform_buffers={'Common': 0},
    uniforms={'mirror': 'float'},
    vertex_array=vertex_array,
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    depth_test=True,
    cull_face='front',
)

shadow = ctx.pipeline(
    'shadow.vert',
    'shadow.frag',
    uniform_buffers={'Common': 0},
    vertex_array=vertex_array,
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    depth_test=True,
    cull_face='back',
    blend={'src': 'src_alpha', 'dst': 'one_minus_src_alpha', 'src_alpha': 'one', 'dst_alpha': 'zero'},
    stencil={'func': 'equal', 'ref': 1, 'read_mask': 1, 'write_mask': 1, 'fail': 'zero', 'depth_fail': 'zero', 'depth_pass': 'zero'},
)

plane = ctx.pipeline(
    'plane.vert',
    'plane.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.empty_vertex_array,
    mode='triangle_strip',
    vertices=4,
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    depth_test=True,
    blend={'src': 'src_alpha', 'dst': 'one_minus_src_alpha', 'src_alpha': 'one', 'dst_alpha': 'zero'},
)

scene['mirror'] = 1.0
reflection['mirror'] = -1.0

yaw, pitch, spin = 1.57, 0.32, 0.2

running = True
while running:
    if spin:
        yaw = pygame.time.get_ticks() / 1000.0 * spin + 1.57
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.MOUSEMOTION and event.buttons[0]:
            spin = 0.0
            yaw -= event.rel[0] * 0.004
            pitch = min(max(pitch + event.rel[1] * 0.004, 0.05), 1.4)

    eye = (math.cos(yaw) * math.cos(pitch) * 5.15, math.sin(yaw) * math.cos(pitch) * 5.15, 0.5 + math.sin(pitch) * 5.15)
    camera = glm.perspective(math.radians(45.0), size[0] / size[1], 0.1, 1000.0) * glm.lookAt(eye, (0.0, 0.0, 0.5), (0.0, 0.0, 1.0))

    ctx.start_frame()
    uniform_buffer.write(camera.to_bytes() + struct.pack('3f4x', *eye))
    framebuffer.clear()
    scene.run()
    reflection.run()
    shadow.run()
    plane.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
