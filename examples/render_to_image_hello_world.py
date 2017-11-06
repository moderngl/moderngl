import ModernGL
import numpy as np
from PIL import Image

"""
    Renders a blue triangle to an Image
"""

ctx = ModernGL.create_standalone_context()
fbo = ctx.simple_framebuffer((1280, 720))
fbo.use()

prog = ctx.program([
    ctx.vertex_shader('''
        #version 330

        in vec2 in_vert;

        void main() {
            gl_Position = vec4(in_vert, 0.0, 1.0);
        }
    '''),
    ctx.fragment_shader('''
        #version 330

        out vec4 f_color;

        void main() {
            f_color = vec4(0.3, 0.5, 1.0, 1.0);
        }
    '''),
])

vertices = np.array([
    0.0, 0.8,
    -0.6, -0.8,
    0.6, -0.8,
])

vbo = ctx.buffer(vertices.astype('f4').tobytes())
vao = ctx.simple_vertex_array(prog, vbo, ['in_vert'])

ctx.clear(1.0, 1.0, 1.0)
vao.render()

img = Image.frombytes('RGB', fbo.size, fbo.read(), 'raw', 'RGB', 0, -1)
img.show()
