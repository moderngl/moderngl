"""This example is a port to ModernGL of code by Nicolas P. Rougier from his "Python & OpenGL
for Scientific Visualization" free online book. Available under the (new) BSD License.

Book is available here:
https://github.com/rougier/python-opengl

Background information on this code:
https://github.com/rougier/python-opengl/blob/master/09-lines.rst

Original code on which this example is based:
https://github.com/rougier/python-opengl/blob/master/code/chapter-09/geom-path.py
"""

import numpy as np
from pyrr import Matrix44

import moderngl
from _example import Example


# prepare geometry
def star(inner=0.45, outer=1.0, n=5):
    R = np.array([inner, outer] * n)
    T = np.linspace(-0.5 * np.pi, 1.5 * np.pi, 2 * n, endpoint=False)
    P = np.zeros((2 * n, 2))
    P[:, 0] = R * np.cos(T)
    P[:, 1] = R * np.sin(T)
    return np.vstack([P, P[0]])


def rect(x, y, w, h):
    return np.array([(x, y), (x + w, y), (x + w, y + h), (x, y + h), (x, y)])


def build_buffers(lines):
    """Prepare the buffers for multi-polyline rendering. Closed polyline must have their
    last point identical to their first point."""

    lines = [np.array(line, dtype="f4") for line in lines]

    indices = []
    reset_index = [-1]
    start_index = 0
    for line in lines:
        if np.all(line[0] == line[-1]):  # closed path
            idx = np.arange(len(line) + 3) - 1
            idx[0], idx[-2], idx[-1] = len(line) - 1, 0, 1
        else:
            idx = np.arange(len(line) + 2) - 1
            idx[0], idx[-1] = 0, len(line) - 1

        indices.append(idx + start_index)
        start_index += len(line)
        indices.append(reset_index)

    return np.vstack(lines).astype("f4"), np.concatenate(indices).astype("i4")


class RichLines(Example):
    title = "Rich Lines"
    gl_version = (3, 3)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.line_prog = self.load_program("rich_lines.glsl")

        lines = [
            star(n=5) * 300 + (400, 400),
            star(n=8) * 150 + (900, 200),
            rect(900, 600, 150, 50),
            [(1200, 100), (1400, 200), (1300, 100)],
        ]

        vertex, index = build_buffers(lines)

        vbo = self.ctx.buffer(vertex)
        ibo = self.ctx.buffer(index)
        self.vao = self.ctx.simple_vertex_array(self.line_prog, vbo, "position",
                                                index_buffer=ibo)

        # Set the desired properties for the lines.
        # Note:
        # - round cap/ends are used if miter_limit < 0
        # - antialias value is in model space and should probably be scaled to be ~1.5px in
        #   screen space

        self.line_prog["linewidth"].value = 15
        self.line_prog["antialias"].value = 1.5
        self.line_prog["miter_limit"].value = -1
        self.line_prog["color"].value = 0, 0, 1, 1
        self.line_prog["projection"].write(
            Matrix44.orthogonal_projection(0, 1600, 800, 0, 0.5, -0.5, dtype="f4")
        )

    def on_render(self, time, frame_time):
        self.ctx.clear(1, 1, 1, 1)
        self.ctx.enable(moderngl.BLEND)
        self.vao.render(moderngl.LINE_STRIP_ADJACENCY)


if __name__ == '__main__':
    RichLines.run()
