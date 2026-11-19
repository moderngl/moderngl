import subprocess

import extras
import moderngl
import pygame

extras.init()

pygame.init()
pygame.display.set_mode((64, 64), flags=pygame.OPENGL | pygame.DOUBLEBUF | pygame.HIDDEN)

ctx = moderngl.get_context()

size = (512, 512)
image = ctx.texture(size, 'rgba8unorm', samples=4)
resolved = ctx.texture(size, 'rgba8unorm')
framebuffer = ctx.framebuffer([image])
framebuffer.clear_color = (0.0, 0.0, 0.0, 1.0)
output = ctx.texture(size, 'bgra8unorm')

ctx.shaders['capture.vert'] = '''
    #version 330 core

    uniform float time;

    vec2 vertices[3] = vec2[](
        vec2(0.0, 0.8),
        vec2(-0.693, -0.4),
        vec2(0.693, -0.4)
    );

    vec3 colors[3] = vec3[](
        vec3(1.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 0.0, 1.0)
    );

    out vec3 v_color;

    void main() {
        mat2 rotation = mat2(cos(time), sin(time), -sin(time), cos(time));
        gl_Position = vec4(rotation * vertices[gl_VertexID], 0.0, 1.0);
        v_color = colors[gl_VertexID];
    }
'''

ctx.shaders['capture.frag'] = '''
    #version 330 core

    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(pow(v_color, vec3(1.0 / 2.2)), 1.0);
    }
'''

pipeline = ctx.pipeline(
    'capture.vert',
    'capture.frag',
    uniforms={'time': 'float'},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=framebuffer,
)

encoder = subprocess.Popen(
    [
        'ffmpeg', '-y', '-loglevel', 'error',
        '-f', 'rawvideo', '-pix_fmt', 'bgra', '-s', f'{size[0]}x{size[1]}', '-r', '60', '-i', '-',
        '-c:v', 'libx264', '-preset', 'veryfast', '-crf', '18', '-pix_fmt', 'yuv420p', 'temp_capture.mkv',
    ],
    stdin=subprocess.PIPE,
)

frame = bytearray(size[0] * size[1] * 4)

for i in range(180):
    ctx.start_frame()
    framebuffer.clear()
    pipeline['time'] = i / 180.0 * 2.0 * 3.14159265
    pipeline.run()
    ctx.copy_texture(image, resolved)
    ctx.copy_texture(resolved, output, flip_y=True)
    output.read_into(frame)
    encoder.stdin.write(frame)
    ctx.end_frame()

encoder.stdin.close()
encoder.wait()
