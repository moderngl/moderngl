"""
Example showing how to resize Buffers with orphan()

This can be useful for batch drawing an arbitrary
amount of geometry over time.

We just render a growing amount of random points.
"""
import struct
import random
import moderngl
from pyrr import matrix44
from _example import Example


class Points:
    """Simple point batching.

    The point set is created using an initial buffer allocation.
    When the buffer is to small we double the size.
    When the buffer do not need resizing we only do a partial
    buffer update.
    """
    def __init__(self, ctx, num_points):
        """
        Args:
            ctx: moderngl context
            num_points: Initial number of points to allocate
        """
        self.points = []
        self.ctx = ctx
        self.buffer = self.ctx.buffer(reserve=num_points * 12)  # 12 bytes for a 3f
        self.program = self.ctx.program(
            vertex_shader="""
            #version 330
            in vec3 in_position;
            uniform mat4 model_matrix;
            void main() {
                gl_Position = model_matrix * vec4(in_position, 1.0);
            }
            """,
            fragment_shader="""
            #version 330
            out vec4 outColor;
            void main() {
                outColor = vec4(1.0);
            }
            """,
        )
        self.vao = self.ctx.vertex_array(
            self.program,
            [(self.buffer, '3f', 'in_position')],
        )

    def render(self, time):
        self.program['model_matrix'].write(matrix44.create_from_eulers((0, time / 8, 0), dtype='f4'))
        self.vao.render(vertices=self.count, mode=moderngl.POINTS)

    @property
    def count(self):
        return len(self.points) // 3

    @property
    def byte_size(self):
        """int: Byte size of the point data"""
        return len(self.points) * 4  # 4 byte per float

    def add(self, num):
        """Adds num points random points"""
        resized = False
        old_points_size = self.byte_size
        new = list(self._gen_random_points(num))
        self.points = self.points + new

        # Keep doubling the buffer size until we reach an acceptable size
        while self.byte_size > self.buffer.size:
            resized = True
            print("Buffer resized {} -> {}".format(self.buffer.size, self.buffer.size * 2))
            print("New capacity is {} points".format(self.buffer.size * 2 // 12))
            self.buffer.orphan(self.buffer.size * 2)

        if resized:
            # Re-write the entire buffer
            self.buffer.write(struct.pack('{}f'.format(len(self.points)), *self.points))
        else:
            # Partial buffer update
            print("Partial buffer update adding {} points".format(len(new) // 3))
            self.buffer.write(struct.pack('{}f'.format(len(new)), *new), offset=old_points_size)

    def _gen_random_points(self, num):
        for _ in range(num * 3):
            yield random.uniform(-1.5, 1.5)


class GrowingBuffers(Example):
    """Growing buffers"""
    gl_version = (3, 3)
    title = "Buffer Resize / Batch Draw"
    window_size = 720, 720
    aspect_ratio = 1.0

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.points = Points(self.ctx, 12 * 10)
        self.points.add(10)

    def on_render(self, time, frametime):
        self.points.render(time)

        # Add more points every 60 frames
        if self.wnd.frames % 60 == 0:
            self.points.add(5000)


if __name__ == '__main__':
    GrowingBuffers.run()
