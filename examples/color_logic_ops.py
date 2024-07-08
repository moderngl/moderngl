'''
Demonstrates Per-Fragment Blending operation called Logical Operation.
Instead of blending fragment color based on linear combinations with alpha
channel, uses bitwise logical operations.

Defined in Section 17.3.9 of
https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf

Renders four overlapping squares.
Square i [0, 4) is assigned a red channel with value (1<<i).

'''
import numpy as np
import moderngl as gl
from _example import Example

class LogicalOp(Example):

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        # Blending will take bitwise OR between fragment color and
        # destination framebuffer color
        self.ctx.enable(gl.COLOR_LOGIC_OP)

        # ctx.logic_op can be one of 
        # gl.{AND,AND_REVERSE,COPY,AND_INVERTED,NOOP,XOR,OR,NOR,EQUIV,
        #     INVERT,OR_REVERSE,COPY_INVERTED,OR_INVERTED,NAND,SET}
        # See Table 17.3 of https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf
        self.ctx.logic_op = gl.OR

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330
                in vec2 point;
                in uint label;
                flat out uint out_value;

                void main() {
                  gl_Position = vec4(point * 2 - 1, 0.0, 1.0);
                  out_value = label; 
                }
            ''',
            fragment_shader='''
                #version 330
                flat in uint out_value;
                out vec4 out_color;

                void main() {
                  float scaled = float(out_value) / 16;
                  out_color = vec4(scaled, 0.0, scaled, 1.0);
                }
            ''')

        def make_quad(origin, size):
            quad = np.array([
                0, 0, 1, 0, 1, 1,
                0, 0, 1, 1, 0, 1,
                ]).astype('f4').reshape(-1, 2)
            quad *= size
            quad += origin 
            return quad

        # Four overlapping squares
        vertices = np.stack([
            make_quad((1.0, 1.0), (2, 2)),
            make_quad((2.6, 1.0), (2, 2)),
            make_quad((1.0, 2.6), (2, 2)),
            make_quad((2.6, 2.6), (2, 2))
            ]).flatten().astype('f4')
        vertices /= (vertices.max() + 1)

        print('vertices:\n', vertices.reshape(-1,2))
        self.vbo = self.ctx.buffer(vertices)

        # the labels associated with each square
        labels = np.concatenate([np.full(6, 1<<i) for i in range(4)]).astype('u4')
        self.labels_buf = self.ctx.buffer(labels)
        print(f'{vertices.shape=} {labels.shape=}')
        print(f'{self.prog._members.keys()=}')

        content = ((self.vbo, '2f4', 'point'), 
                   (self.labels_buf, '1u4', 'label'))
        self.vao = self.ctx.vertex_array(self.prog, content, mode=gl.TRIANGLES)

    def render(self, time, frame_time):
        # self.vbo.use()
        self.vao.render()
        

if __name__ == '__main__':
    LogicalOp.run()




