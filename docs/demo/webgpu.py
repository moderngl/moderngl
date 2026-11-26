import math
import struct

import js
import pyodide_js
from pyodide.ffi import create_proxy

import moderngl


class Loader:
    backend = 'webgpu'


canvas = js.document.getElementById('demo')
if not hasattr(js.navigator, 'gpu'):
    raise RuntimeError('WebGPU is not available in this browser')
adapter = await js.navigator.gpu.requestAdapter()
if not adapter:
    raise RuntimeError('WebGPU is not available in this browser')
device = await adapter.requestDevice()
symbols = js.window.eval(moderngl._extern_webgpu)(pyodide_js._module, device, canvas.getContext('webgpu'))
pyodide_js._module.mergeLibSymbols(symbols)

moderngl.init(Loader())
ctx = moderngl.get_context()

ctx.shaders['cube.vert'] = '''
    @group(0) @binding(0) var<uniform> camera: mat4x4f;

    struct Varyings {
        @builtin(position) position: vec4f,
        @location(0) normal: vec3f,
    };

    @vertex fn main(@location(0) vertex: vec3f, @location(1) normal: vec3f) -> Varyings {
        return Varyings(camera * vec4f(vertex, 1.0), normal);
    }
'''

ctx.shaders['cube.frag'] = '''
    @fragment fn main(@location(0) normal: vec3f) -> @location(0) vec4f {
        let light = normalize(vec3f(4.0, 3.0, 10.0));
        let lum = 0.3 + 0.7 * max(dot(normalize(normal), light), 0.0);
        return vec4f(pow((abs(normal) * 0.6 + 0.4) * lum, vec3f(1.0 / 2.2)), 1.0);
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
    vertex_array=ctx.vertex_array(vertex_buffer.bind('3f 3f', 0, 1)),
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: camera_buffer}),
    depth_test=True,
    cull_face='back',
)


def render(timestamp):
    t = timestamp / 1000.0
    eye = (math.cos(t * 0.5) * 5.0, math.sin(t * 0.5) * 5.0, 3.0)
    camera_buffer.write(moderngl.camera(eye, aspect=size[0] / size[1], fov=45.0, clip=True))
    ctx.start_frame()
    framebuffer.clear()
    cube.run()
    ctx.copy_texture(image, output)
    ctx.present(output)
    js.requestAnimationFrame(render_proxy)


render_proxy = create_proxy(render)
js.requestAnimationFrame(render_proxy)
