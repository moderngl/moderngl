import moderngl
import numpy as np

from PIL import Image

ctx = moderngl.create_standalone_context()

prog = ctx.program(
    vertex_shader="""
        #version 330

        in vec2 in_vert;
        in vec3 in_color;

        uniform float scale;

        out vec3 v_color;

        void main() {
            v_color = in_color;
            gl_Position = vec4(in_vert * scale, 0.0, 1.0);
        }
    """,
    fragment_shader="""
        #version 330

        in vec3 v_color;

        out vec3 f_color;

        void main() {
            f_color = v_color;
        }
    """,
)

vertices = np.asarray([

    -0.75, -0.75,  1, 0, 0,
    0.75, -0.75,  0, 1, 0,
    0.0, 0.649,  0, 0, 1

], dtype='f4')

vbo = ctx.buffer(vertices.tobytes())
vao = ctx.vertex_array(prog, vbo, "in_vert", "in_color")

fbo = ctx.framebuffer(
    color_attachments=[ctx.texture((512, 512), 3)]
)
fbo.use()
fbo.clear(0.0, 0.0, 0.0, 1.0)

vao.program['scale'] = 2

vao.render()  # "mode" is moderngl.TRIANGLES by default

Image.frombytes(
    "RGB", fbo.size, fbo.color_attachments[0].read(),
    "raw", "RGB", 0, -1
).show()
