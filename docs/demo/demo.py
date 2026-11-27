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
        float time;
    };
'''

ctx.shaders['terrain.vert'] = '''
    #version 330 core
    #include "common"

    const int N = 48;
    const ivec2 corners[6] = ivec2[](ivec2(0, 0), ivec2(1, 0), ivec2(1, 1), ivec2(0, 0), ivec2(1, 1), ivec2(0, 1));

    out vec3 v_vertex;

    float height(vec2 p) {
        float h = sin(p.x * 0.9) * sin(p.y * 0.7) * 0.8;
        h += sin(p.x * 2.1 + p.y * 1.3) * 0.25;
        h += sin(p.x * 4.7 - p.y * 3.9) * 0.08;
        return max(h, -0.5);
    }

    void main() {
        int quad = gl_VertexID / 6;
        vec2 cell = vec2(ivec2(quad % N, quad / N) + corners[gl_VertexID % 6]);
        vec2 xy = (cell / float(N) * 2.0 - 1.0) * 6.0;
        float scroll = time - mod(time, 0.25);
        v_vertex = vec3(xy.x, xy.y - mod(time, 0.25), height(xy + vec2(0.0, scroll)));
        gl_Position = camera * vec4(v_vertex, 1.0);
    }
'''

ctx.shaders['terrain.frag'] = '''
    #version 330 core

    in vec3 v_vertex;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 normal = normalize(cross(dFdx(v_vertex), dFdy(v_vertex)));
        vec3 light = normalize(vec3(-3.0, 2.0, 4.0));
        float lum = 0.4 + 0.6 * max(dot(normal, light), 0.0);
        vec3 color = vec3(0.45, 0.52, 0.58);
        color = mix(color, vec3(0.55, 0.6, 0.48), step(-0.49, v_vertex.z));
        color = mix(color, vec3(0.6, 0.57, 0.53), step(0.35, v_vertex.z));
        color = mix(color, vec3(0.92, 0.93, 0.95), step(0.75, v_vertex.z));
        float fog = smoothstep(4.0, 6.0, length(v_vertex.xy));
        out_color = vec4(mix(color * lum, vec3(0.1, 0.1, 0.12), fog), 1.0);
    }
'''

uniform_buffer = ctx.buffer(size=80)

size = ctx.screen.size
image = ctx.texture(size, 'rgba8unorm', multisampled=True)
depth = ctx.texture(size, 'depth24plus', multisampled=True)
output = ctx.texture(size, 'rgba8unorm')
framebuffer = ctx.framebuffer([image, depth])
framebuffer.clear_color = (0.1, 0.1, 0.12, 1.0)

terrain = ctx.pipeline(
    'terrain.vert',
    'terrain.frag',
    uniform_buffers={'Common': 0},
    vertex_array=ctx.empty_vertex_array,
    vertices=48 * 48 * 6,
    framebuffer=framebuffer,
    resources=ctx.resources(uniform_buffers={0: uniform_buffer}),
    depth_test=True,
)

camera = moderngl.camera((0.0, -6.5, 4.0), (0.0, 0.0, 0.0), aspect=size[0] / size[1], fov=45.0)


def render(timestamp):
    uniform_buffer.write(camera + struct.pack('4f', timestamp / 1000.0, 0.0, 0.0, 0.0))
    ctx.start_frame()
    framebuffer.clear()
    terrain.run()
    ctx.copy_texture(image, output)
    ctx.present(output)
    js.requestAnimationFrame(render_proxy)


render_proxy = create_proxy(render)
js.requestAnimationFrame(render_proxy)
