import math
import struct

import js
import pyodide_js
from pyodide.ffi import create_proxy

import moderngl

canvas = js.document.getElementById('demo')
gl = canvas.getContext('webgl2', antialias=False, depth=False, stencil=False, alpha=True)
symbols = js.window.eval(moderngl._extern_gl)(pyodide_js._module, gl)
pyodide_js._module.mergeLibSymbols(symbols)

ctx = moderngl.get_context()
ctx.screen.size = (canvas.width, canvas.height)

ctx.shaders['common'] = '''
    layout (std140) uniform Common {
        mat4 camera;
    };
'''

ctx.shaders['cube.vert'] = '''
    #version 330 core
    #include "common"

    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec3 in_normal;

    out vec3 v_normal;

    void main() {
        gl_Position = camera * vec4(in_vertex, 1.0);
        v_normal = in_normal;
    }
'''

ctx.shaders['cube.frag'] = '''
    #version 330 core

    in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 light = normalize(vec3(4.0, 3.0, 10.0));
        float lum = 0.3 + 0.7 * max(dot(normalize(v_normal), light), 0.0);
        out_color = vec4((abs(v_normal) * 0.6 + 0.4) * lum, 1.0);
        out_color.rgb = pow(out_color.rgb, vec3(1.0 / 2.2));
    }
'''

vertices = bytearray()
for axis in range(3):
    for sign in (-1.0, 1.0):
        normal = [0.0, 0.0, 0.0]
        normal[axis] = sign
        u = [0.0, 0.0, 0.0]
        v = [0.0, 0.0, 0.0]
        u[(axis + 1) % 3] = 1.0
        v[(axis + 2) % 3] = sign
        for a, b in ((-1, -1), (1, -1), (1, 1), (-1, -1), (1, 1), (-1, 1)):
            position = [normal[i] + u[i] * a + v[i] * b for i in range(3)]
            vertices += struct.pack('6f', *position, *normal)

vertex_buffer = ctx.buffer(vertices)
camera_buffer = ctx.buffer(size=64)

size = ctx.screen.size
image = ctx.texture(size, 'rgba8unorm', multisampled=True)
depth = ctx.texture(size, 'depth24plus', multisampled=True)
output = ctx.texture(size, 'rgba8unorm')
framebuffer = ctx.framebuffer([image, depth])
framebuffer.clear_color = (0.1, 0.1, 0.12, 1.0)

cube = ctx.pipeline(
    'cube.vert',
    'cube.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.vertex_array(vertex_buffer.bind('3f 3f', 0, 1)),
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: camera_buffer}),
    depth_test=True,
    cull_face='back',
)


def render(timestamp):
    t = timestamp / 1000.0
    eye = (math.cos(t * 0.5) * 5.0, math.sin(t * 0.5) * 5.0, 3.0)
    camera_buffer.write(moderngl.camera(eye, aspect=size[0] / size[1], fov=45.0))
    ctx.start_frame()
    framebuffer.clear()
    cube.run()
    ctx.copy_texture(image, output)
    ctx.present(output)
    js.requestAnimationFrame(render_proxy)


render_proxy = create_proxy(render)
js.requestAnimationFrame(render_proxy)
