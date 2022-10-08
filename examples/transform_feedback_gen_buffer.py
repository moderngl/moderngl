"""
Generates buffer data on the gpu using transform feedback.
We use an empty VertexArray running it vertices times outputting data to the buffer.

More fancy calculations can of course be done. This example just shows the concept.
"""
import struct
from ported._example import Example


class GenerateData(Example):
    title = "Generate Data"
    gl_version = (3, 3)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                out vec3 pos;
                out vec3 color;

                void main() {
                    pos = vec3(gl_VertexID * 2, gl_VertexID * 2, gl_VertexID * 2);
                    color = vec3(fract(gl_VertexID * 12.344), fract(gl_VertexID * 34.111), fract(gl_VertexID * 7.244));
                }
            ''',
            varyings=['pos', 'color'],
        )

        N = 4
        self.buffer = self.ctx.buffer(reserve=N * 4 * 6)
        self.vao = self.ctx.vertex_array(self.prog, [])
        self.vao.transform(self.buffer, vertices=N)

        data = struct.unpack('{}f'.format(N * 6), self.buffer.read())
        for i in range(0, N * 6, 6):
            print(data[i:i + 3], data[i + 3:i + 6])

    def render(self, time, frame_time):
        exit(0)


if __name__ == '__main__':
    GenerateData.run()
