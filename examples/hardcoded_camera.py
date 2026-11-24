import os
import sys

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
depth = ctx.texture(size, 'depth24plus', multisampled=True)
framebuffer = ctx.framebuffer([image, depth])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

vertex_buffer = ctx.buffer(load_glb('examples/data/blob/blob.glb', texcoords=False)['scene'])

ctx.shaders['hardcoded_camera.vert'] = '''
    #version 330 core

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;

    out vec3 v_normal;

    mat4 perspective(float fovy, float aspect, float znear, float zfar) {
        float tan_half_fovy = tan(radians(fovy) * 0.5);
        return mat4(
            1.0 / (aspect * tan_half_fovy), 0.0, 0.0, 0.0,
            0.0, 1.0 / tan_half_fovy, 0.0, 0.0,
            0.0, 0.0, -(zfar + znear) / (zfar - znear), -1.0,
            0.0, 0.0, -(2.0 * zfar * znear) / (zfar - znear), 0.0
        );
    }

    mat4 lookat(vec3 eye, vec3 center, vec3 up) {
        vec3 f = normalize(center - eye);
        vec3 s = normalize(cross(f, up));
        vec3 u = cross(s, f);
        return mat4(
            s.x, u.x, -f.x, 0.0,
            s.y, u.y, -f.y, 0.0,
            s.z, u.z, -f.z, 0.0,
            -dot(s, eye), -dot(u, eye), dot(f, eye), 1.0
        );
    }

    void main() {
        mat4 projection = perspective(45.0, 16.0 / 9.0, 0.1, 1000.0);
        mat4 view = lookat(vec3(4.0, 3.0, 2.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0));
        gl_Position = projection * view * vec4(in_vertex, 1.0);
        v_normal = in_normal;
    }
'''

ctx.shaders['hardcoded_camera.frag'] = '''
    #version 330 core

    in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 light = vec3(4.0, 3.0, 10.0);
        float lum = dot(normalize(light), normalize(v_normal)) * 0.7 + 0.3;
        out_color = vec4(lum, lum, lum, 1.0);
    }
'''

pipeline = ctx.pipeline(
    'hardcoded_camera.vert',
    'hardcoded_camera.frag',
    vertex_array=ctx.vertex_array(vertex_buffer.bind('3f 3f', 0, 1)),
    framebuffer=framebuffer,
    depth_test=True,
    cull_face='back',
)

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    ctx.start_frame()
    framebuffer.clear()
    pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
