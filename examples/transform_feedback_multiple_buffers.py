"""
Generates buffer data on the gpu using transform feedback.
We use an empty VertexArray running it vertices times outputing data to the buffer.

More fancy calculations can of course be done. This example just shows the concept.
"""
import struct
import numpy as np
from ported._example import Example


class GenerateData(Example):
    title = "Generate Data"
    gl_version = (3, 3)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                in float number;

                out float times_two;
                out float times_three;

                void main() {
                    times_two = number * 2.0;
                    times_three = number * 3.0;
                }
            ''',
            varyings=['times_two', 'times_three'],
            varyings_capture_mode='separate',
        )

        input_array = np.array([1.0, 2.0, 3.0, 4.0, 5.0], 'f4')
        self.input_buffer = self.ctx.buffer(input_array.tobytes())
        self.output_buffer_0 = self.ctx.buffer(reserve=self.input_buffer.size)
        self.output_buffer_1 = self.ctx.buffer(reserve=self.input_buffer.size)
        self.vao = self.ctx.vertex_array(self.prog, [(self.input_buffer, '1f', 'number')])
        self.vao.transform([self.output_buffer_0, self.output_buffer_1], vertices=len(input_array))

        output_array_0 = np.frombuffer(self.output_buffer_0.read(), 'f4')
        output_array_1 = np.frombuffer(self.output_buffer_1.read(), 'f4')
        print(output_array_0)
        print(output_array_1)

    def render(self, time, frame_time):
        exit(0)


if __name__ == '__main__':
    GenerateData.run()
