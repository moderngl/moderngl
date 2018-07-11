import numpy as np
import moderngl as mgl

ctx = mgl.create_standalone_context()
ctx.enable(mgl.DEPTH_TEST)
prog = ctx.program(
    vertex_shader='''
        #version 330

        in vec3 in_vert;

        void main() {
            gl_Position = vec4(in_vert, 1.0);
        }
    ''',
    fragment_shader='''
        #version 330

        out vec3 f_color;

        void main() {
            f_color = vec3(1.0);
            gl_FragDepth = 0.1234;
        }
    ''',
)

vertices = np.array([
    [-1, 1, 0.],
    [-1, -1, 0.],
    [1, -1, 0.],
    ])
vbo = ctx.buffer(vertices.astype('f4').tobytes())

size = (4, 4)
cbo = ctx.renderbuffer(size)
dbo = ctx.depth_texture(size, alignment=1)

fbo = ctx.framebuffer(color_attachments=(cbo), depth_attachment=dbo)
fbo.use()
fbo.clear(depth=1.0)

depth = np.frombuffer(dbo.read(alignment=1), dtype=np.dtype('f4')).reshape(size[::-1])
print(depth) # should all be 1

# Now we render a triangle in there
vao = ctx.simple_vertex_array(prog, vbo, 'in_vert')
vao.render()

# It should have some different values in the middle
depth = np.frombuffer(dbo.read(alignment=1), dtype=np.dtype('f4')).reshape(size[::-1])
print(depth) # some stuff should be 1s and 0.5 where the triangle lies.

# lets just check the value of the fbo for safety
data = np.frombuffer(fbo.read(), dtype=np.dtype('u1')).reshape((size[1], size[0], 3))
print(data[:,:,0])

depth_data = np.frombuffer(fbo.read(components=-1), dtype=np.dtype('f4')).reshape((size[1], size[0], 1))
print(depth_data[:,:,0])
