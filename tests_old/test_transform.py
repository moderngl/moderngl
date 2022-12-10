import struct
import platform
from array import array
import unittest
import numpy as np

import moderngl

from common import get_context


class TestCase(unittest.TestCase):
    """Test all combinations of transforms"""

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_transform(self):

        program = self.ctx.program(
            vertex_shader='''
                #version 330

                in vec4 vert;
                out float vert_length;

                void main() {
                    vert_length = length(vert);
                }
            ''',
            varyings=['vert_length']
        )

        self.assertIs(program.geometry_input, None)
        self.assertIs(program.geometry_output, None)
        self.assertIs(program.is_transform, True)

        self.assertIn('vert', program)
        self.assertIn('vert_length', program)

    def test_vertex_points(self):
        """Transform with points"""
        data = (0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0)
        buffer = self.ctx.buffer(array('f', data))
        program = self.ctx.program(
            vertex_shader="""
            #version 330

            in vec2 in_pos;
            out vec2 out_pos;

            void main() {
                out_pos = in_pos;
            }
            """,
            varyings=["out_pos"],
        )

        vao = self.ctx.vertex_array(program, [(buffer, "2f", "in_pos"),])
        self.assertIs(vao.mode, moderngl.POINTS)
        buffer2 = self.ctx.buffer(reserve=buffer.size)
        vao.transform(buffer2, mode=self.ctx.POINTS)
        self.assertEqual(data, struct.unpack(f"{len(data)}f", buffer.read()))

    def test_vertex_lines(self):
        data = (
            0.0, 0.0,
            1.0, 1.0,
            2.0, 2.0,
            3.0, 3.0,
        )
        buffer = self.ctx.buffer(array('f', data))
        program = self.ctx.program(
            vertex_shader="""
            #version 330

            in vec2 in_pos;
            out vec2 out_pos;

            void main() {
                out_pos = in_pos * 2;
            }
            """,
            varyings=["out_pos"],
        )

        vao = self.ctx.vertex_array(program, [(buffer, "2f", "in_pos"),])
        buffer2 = self.ctx.buffer(reserve=buffer.size)
        vao.transform(buffer2, mode=self.ctx.LINES)
        self.assertEqual(tuple(v * 2 for v in data), struct.unpack(f"{len(data)}f", buffer2.read()))

    # NOTE: This fails in tests and may be non-standard or require higher gl version
    # def test_vertex_lines_adjacency(self):
    #     if platform.system().lower() in ["darwin"]:
    #         self.skipTest('Tranforms with adjacency primitives now working on OSX')

    #     data = (
    #         1.0, 1.0,
    #         2.0, 2.0,
    #         3.0, 3.0,
    #         4.0, 4.0,
    #     )
    #     buffer = self.ctx.buffer(array('f', data))
    #     program = self.ctx.program(
    #         vertex_shader="""
    #         #version 330

    #         in vec2 in_pos;
    #         out vec2 out_pos;

    #         void main() {
    #             out_pos = in_pos;
    #         }
    #         """,
    #         varyings=["out_pos"],
    #     )
    #     vao = self.ctx.vertex_array(program, [(buffer, "2f", "in_pos"),])
    #     buffer2 = self.ctx.buffer(reserve=4 * 4)
    #     vao.transform(buffer2, mode=self.ctx.LINES_ADJACENCY)

    #     # Start end end line is removed
    #     self.assertEqual(tuple(data[2:6]), struct.unpack(f"4f", buffer2.read()))

    def test_vertex_line_strip(self):
        data = (
            1.0, 1.0,
            2.0, 2.0,
            3.0, 3.0,
            4.0, 4.0,
        )
        buffer = self.ctx.buffer(array('f', data))
        program = self.ctx.program(
            vertex_shader="""
            #version 330

            in vec2 in_pos;
            out vec2 out_pos;

            void main() {
                out_pos = in_pos;
            }
            """,
            varyings=["out_pos"],
        )
        vao = self.ctx.vertex_array(program, [(buffer, "2f", "in_pos"),])
        buffer2 = self.ctx.buffer(reserve=4 * 12)
        vao.transform(buffer2, mode=self.ctx.LINE_STRIP)
        self.assertEqual(
            (1.0, 1.0, 2.0, 2.0, 2.0, 2.0, 3.0, 3.0, 3.0, 3.0, 4.0, 4.0),
            struct.unpack("12f", buffer2.read())
        )

    # NOTE: Fails in tests and might be non-standard or require higher gl version
    # def test_vertex_line_strip_adjacency(self):
    #     if platform.system().lower() in ["darwin"]:
    #         self.skipTest('Tranforms with adjacency primitives now working on OSX')

    #     data = (
    #         1.0, 1.0,
    #         2.0, 2.0,
    #         3.0, 3.0,
    #         4.0, 4.0,
    #         5.0, 5.0,
    #     )
    #     buffer = self.ctx.buffer(array('f', data))
    #     program = self.ctx.program(
    #         vertex_shader="""
    #         #version 330

    #         in vec2 in_pos;
    #         out vec2 out_pos;

    #         void main() {
    #             out_pos = in_pos;
    #         }
    #         """,
    #         varyings=["out_pos"],
    #     )
    #     vao = self.ctx.vertex_array(program, [(buffer, "2f", "in_pos"),])
    #     buffer2 = self.ctx.buffer(reserve=4 * 8)
    #     vao.transform(buffer2, mode=self.ctx.LINE_STRIP_ADJACENCY)
    #     self.assertEqual(
    #         (2.0, 2.0, 3.0, 3.0, 3.0, 3.0, 4.0, 4.0),
    #         struct.unpack("8f", buffer2.read())
    #     )

    def test_vertex_triangles(self):
        data = (
            1.0, 1.0,
            2.0, 2.0,
            3.0, 3.0,
            4.0, 4.0,
            5.0, 5.0,
            6.0, 6.0,
        )
        buffer = self.ctx.buffer(array('f', data))
        program = self.ctx.program(
            vertex_shader="""
            #version 330

            in vec2 in_pos;
            out vec2 out_pos;

            void main() {
                out_pos = in_pos * 2;
            }
            """,
            varyings=["out_pos"],
        )
        vao = self.ctx.vertex_array(program, [(buffer, "2f", "in_pos"),])
        buffer2 = self.ctx.buffer(reserve=4 * 12)
        vao.transform(buffer2, mode=self.ctx.TRIANGLES)
        self.assertEqual(
            tuple(v * 2 for v in data),
            struct.unpack("12f", buffer2.read())
        )

    def test_vertex_triangles_adjacency(self):
        if platform.system().lower() in ["darwin"]:
            self.skipTest('Tranforms with adjacency primitives now working on OSX')
        data = (
            1.0, 1.0, # outer points
            2.0, 2.0,
            3.0, 3.0,
            4.0, 4.0,
            5.0, 5.0,
            6.0, 6.0,
            7.0, 7.0, # inner triangle
            8.0, 8.0,
            9.0, 9.0,
        )
        buffer = self.ctx.buffer(array('f', data))
        program = self.ctx.program(
            vertex_shader="""
            #version 330

            in vec2 in_pos;
            out vec2 out_pos;

            void main() {
                out_pos = in_pos;
            }
            """,
            varyings=["out_pos"],
        )
        vao = self.ctx.vertex_array(program, [(buffer, "2f", "in_pos"),])
        buffer2 = self.ctx.buffer(reserve=4 * 6)
        vao.transform(buffer2, mode=self.ctx.TRIANGLES_ADJACENCY)
        self.assertEqual(
            (1.0, 1.0, 3.0, 3.0, 5.0, 5.0),
            struct.unpack("6f", buffer2.read())
        )

    def test_vertex_triangle_strip(self):

        data = (
            1.0, 1.0,
            2.0, 2.0,
            3.0, 3.0,
            4.0, 4.0,
        )
        buffer = self.ctx.buffer(array('f', data))
        program = self.ctx.program(
            vertex_shader="""
            #version 330

            in vec2 in_pos;
            out vec2 out_pos;

            void main() {
                out_pos = in_pos;
            }
            """,
            varyings=["out_pos"],
        )
        vao = self.ctx.vertex_array(program, [(buffer, "2f", "in_pos"),])
        buffer2 = self.ctx.buffer(reserve=4 * 12)
        vao.transform(buffer2, mode=self.ctx.TRIANGLE_STRIP)
        self.assertEqual(
            (1.0, 1.0, 2.0, 2.0, 3.0, 3.0, 3.0, 3.0, 2.0, 2.0, 4.0, 4.0),
            struct.unpack("12f", buffer2.read())
        )        

    def test_vertex_triangle_fan(self):
        vertices = np.array(
            [
                0.0, 0.0,  # Initial triangle
                1.0, 0.0,
                1.0, 1.0,
                0.0, 1.0,  # second triangle 
            ],
            dtype='f4',
        )

        prog = self.ctx.program(vertex_shader=
            """
            #version 330
            in vec2 in_vert;
            out vec2 out_vert;
            void main(void) {
                out_vert = in_vert;
            }
            """,
            varyings=['out_vert'],
        )

        vbo = self.ctx.buffer(vertices)
        vao = self.ctx.vertex_array(prog, [(vbo, '2f', 'in_vert')])
        N = 6  # 6 vertices in end buffer
        buffer = self.ctx.buffer(reserve=N * 4 * 2)
        vao.transform(buffer, vertices=4, mode=self.ctx.TRIANGLE_FAN)

        data = struct.unpack('{}f'.format(N * 2), buffer.read())
        self.assertEqual(data,
            (
                0.0, 0.0,  # Initial triangle
                1.0, 0.0,
                1.0, 1.0,
                0.0, 0.0,  # second triangle 
                1.0, 1.0, 
                0.0, 1.0,
            )
        )

    # ------ Geometry shader -------

    def test_geometry_points(self):
        data = (1.0, 2.0, 3.0, 4.0, 5.0, 6.0)
        vertices = np.array(data, dtype='f4')
        prog = self.ctx.program(
            vertex_shader=
            """
            #version 330

            in float in_value;

            void main() {
                gl_Position = vec4(in_value);
            }
            """,
            geometry_shader="""
            #version 330

            out float out_value;

            layout(points) in;
            layout(points, max_vertices=1) out;

            void main() {
                out_value = gl_in[0].gl_Position.x * 2;
                EmitVertex();
            }
            """,
            varyings=['out_value'],
        )
        vbo = self.ctx.buffer(vertices)
        vao = self.ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
        buffer = self.ctx.buffer(reserve=vbo.size)
        vao.transform(buffer, mode=moderngl.POINTS)
        self.assertEqual(tuple(v * 2 for v in data), struct.unpack('6f', buffer.read()))

    def test_geometry_lines(self):
        data = (
            1.0, 2.0,
            3.0, 4.0,
            5.0, 6.0,
        )
        vertices = np.array(data, dtype='f4')
        prog = self.ctx.program(
            vertex_shader=
            """
            #version 330

            in float in_value;

            void main() {
                gl_Position = vec4(in_value);
            }
            """,
            geometry_shader="""
            #version 330

            out float out_value;

            layout(lines) in;
            layout(line_strip, max_vertices=2) out;

            void main() {
                out_value = gl_in[0].gl_Position.x;
                EmitVertex();
                out_value = gl_in[1].gl_Position.x;
                EmitVertex();
                EndPrimitive();
            }
            """,
            varyings=['out_value'],
        )
        vbo = self.ctx.buffer(vertices)
        vao = self.ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
        buffer = self.ctx.buffer(reserve=vbo.size)
        vao.transform(buffer, mode=moderngl.LINES)
        self.assertEqual(data, struct.unpack('6f', buffer.read()))

    def test_geometry_line_loop(self):
        data = (
            1.0, 2.0, 3.0, 4.0, 5.0, 6.0,
        )
        vertices = np.array(data, dtype='f4')
        prog = self.ctx.program(
            vertex_shader=
            """
            #version 330

            in float in_value;

            void main() {
                gl_Position = vec4(in_value);
            }
            """,
            geometry_shader="""
            #version 330

            out float out_value;

            layout(lines) in;
            layout(line_strip, max_vertices=2) out;

            void main() {
                out_value = gl_in[0].gl_Position.x;
                EmitVertex();
                out_value = gl_in[1].gl_Position.x;
                EmitVertex();
                EndPrimitive();
            }
            """,
            varyings=['out_value'],
        )
        vbo = self.ctx.buffer(vertices)
        vao = self.ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
        buffer = self.ctx.buffer(reserve=4 * 12)
        vao.transform(buffer, mode=moderngl.LINE_LOOP)
        self.assertEqual(
            (1.0, 2.0, 2.0, 3.0, 3.0, 4.0, 4.0, 5.0, 5.0, 6.0, 6.0, 1.0),
            struct.unpack('12f', buffer.read())
        )

    def test_geometry_line_strip(self):
        data = (
            1.0, 2.0, 3.0, 4.0, 5.0
        )
        vertices = np.array(data, dtype='f4')
        prog = self.ctx.program(
            vertex_shader=
            """
            #version 330

            in float in_value;

            void main() {
                gl_Position = vec4(in_value);
            }
            """,
            geometry_shader="""
            #version 330

            out float out_value;

            layout(lines) in;
            layout(line_strip, max_vertices=2) out;

            void main() {
                out_value = gl_in[0].gl_Position.x;
                EmitVertex();
                out_value = gl_in[1].gl_Position.x;
                EmitVertex();
                EndPrimitive();
            }
            """,
            varyings=['out_value'],
        )
        vbo = self.ctx.buffer(vertices)
        vao = self.ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
        buffer = self.ctx.buffer(reserve=4 * 8)
        vao.transform(buffer, mode=moderngl.LINE_LOOP)
        self.assertEqual(
            (1.0, 2.0, 2.0, 3.0, 3.0, 4.0, 4.0, 5.0),
            struct.unpack('8f', buffer.read())
        )

    def test_geometry_lines_adjacency(self):
        data = (
            1.0, 2.0, 3.0, 4.0,
        )
        vertices = np.array(data, dtype='f4')
        prog = self.ctx.program(
            vertex_shader=
            """
            #version 330

            in float in_value;

            void main() {
                gl_Position = vec4(in_value);
            }
            """,
            geometry_shader="""
            #version 330

            out float out_value;

            layout(lines_adjacency) in;
            layout(line_strip, max_vertices=2) out;

            void main() {
                out_value = gl_in[1].gl_Position.x;
                EmitVertex();
                out_value = gl_in[2].gl_Position.x;
                EmitVertex();
                EndPrimitive();
            }
            """,
            varyings=['out_value'],
        )
        vbo = self.ctx.buffer(vertices)
        vao = self.ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
        buffer = self.ctx.buffer(reserve=4 * 2)
        vao.transform(buffer, mode=moderngl.LINES_ADJACENCY)
        self.assertEqual(
            (2.0, 3.0),
            struct.unpack('2f', buffer.read())
        )

    def test_geometry_line_strip_adjacency(self):
        data = (
            1.0, 2.0, 3.0, 4.0, 5.0,
        )
        vertices = np.array(data, dtype='f4')
        prog = self.ctx.program(
            vertex_shader=
            """
            #version 330

            in float in_value;

            void main() {
                gl_Position = vec4(in_value);
            }
            """,
            geometry_shader="""
            #version 330

            out float out_value;

            layout(lines_adjacency) in;
            layout(line_strip, max_vertices=2) out;

            void main() {
                out_value = gl_in[1].gl_Position.x;
                EmitVertex();
                out_value = gl_in[2].gl_Position.x;
                EmitVertex();
                EndPrimitive();
            }
            """,
            varyings=['out_value'],
        )
        vbo = self.ctx.buffer(vertices)
        vao = self.ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
        buffer = self.ctx.buffer(reserve=4 * 4)
        vao.transform(buffer, mode=moderngl.LINE_STRIP_ADJACENCY)
        self.assertEqual(
            (2.0, 3.0, 3.0, 4.0),
            struct.unpack('4f', buffer.read())
        )

    # ---

    def test_geometry_triangles(self):
        data = (
            1.0, 2.0, 3.0, 4.0, 5.0, 6.0,
        )
        vertices = np.array(data, dtype='f4')
        prog = self.ctx.program(
            vertex_shader=
            """
            #version 330

            in float in_value;

            void main() {
                gl_Position = vec4(in_value);
            }
            """,
            geometry_shader="""
            #version 330

            out float out_value;

            layout(triangles) in;
            layout(triangle_strip, max_vertices=3) out;

            void main() {
                out_value = gl_in[0].gl_Position.x;
                EmitVertex();
                out_value = gl_in[1].gl_Position.x;
                EmitVertex();
                out_value = gl_in[2].gl_Position.x;
                EmitVertex();
                EndPrimitive();
            }
            """,
            varyings=['out_value'],
        )
        vbo = self.ctx.buffer(vertices)
        vao = self.ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
        buffer = self.ctx.buffer(reserve=vbo.size)
        vao.transform(buffer, mode=moderngl.TRIANGLES)
        self.assertEqual(data, struct.unpack('6f', buffer.read()))

    def test_geometry_triangle_strip(self):
        data = (
            1.0, 2.0, 3.0, 4.0,
        )
        vertices = np.array(data, dtype='f4')
        prog = self.ctx.program(
            vertex_shader=
            """
            #version 330

            in float in_value;

            void main() {
                gl_Position = vec4(in_value);
            }
            """,
            geometry_shader="""
            #version 330

            out float out_value;

            layout(triangles) in;
            layout(triangle_strip, max_vertices=3) out;

            void main() {
                out_value = gl_in[0].gl_Position.x;
                EmitVertex();
                out_value = gl_in[1].gl_Position.x;
                EmitVertex();
                out_value = gl_in[2].gl_Position.x;
                EmitVertex();
                EndPrimitive();
            }
            """,
            varyings=['out_value'],
        )
        vbo = self.ctx.buffer(vertices)
        vao = self.ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
        buffer = self.ctx.buffer(reserve=4 * 6)
        vao.transform(buffer, mode=moderngl.TRIANGLE_STRIP)
        self.assertEqual(
            (1.0, 2.0, 3.0, 3.0, 2.0, 4.0),
            struct.unpack('6f', buffer.read())
        )

    def test_geometry_triangle_fan(self):
        data = (
            0.0, 1.0, 2.0, 3.0,
        )
        vertices = np.array(data, dtype='f4')
        prog = self.ctx.program(
            vertex_shader=
            """
            #version 330

            in float in_value;

            void main() {
                gl_Position = vec4(in_value);
            }
            """,
            geometry_shader="""
            #version 330

            out float out_value;

            layout(triangles) in;
            layout(triangle_strip, max_vertices=3) out;

            void main() {
                out_value = gl_in[0].gl_Position.x;
                EmitVertex();
                out_value = gl_in[1].gl_Position.x;
                EmitVertex();
                out_value = gl_in[2].gl_Position.x;
                EmitVertex();
                EndPrimitive();
            }
            """,
            varyings=['out_value'],
        )
        vbo = self.ctx.buffer(vertices)
        vao = self.ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
        buffer = self.ctx.buffer(reserve=4 * 6)
        vao.transform(buffer, mode=moderngl.TRIANGLE_FAN)
        self.assertEqual(
            (0.0, 1.0, 2.0, 0.0, 2.0, 3.0),
            struct.unpack('6f', buffer.read())
        )

    def test_geometry_triangles_adjacency(self):
        data = (
            1.0, 2.0, 3.0, 4.0, 5.0, 6.0,
        )
        vertices = np.array(data, dtype='f4')
        prog = self.ctx.program(
            vertex_shader=
            """
            #version 330

            in float in_value;

            void main() {
                gl_Position = vec4(in_value);
            }
            """,
            geometry_shader="""
            #version 330

            out float out_value;

            layout(triangles_adjacency) in;
            layout(triangle_strip, max_vertices=3) out;

            void main() {
                out_value = gl_in[0].gl_Position.x;
                EmitVertex();
                out_value = gl_in[2].gl_Position.x;
                EmitVertex();
                out_value = gl_in[4].gl_Position.x;
                EmitVertex();
                EndPrimitive();
            }
            """,
            varyings=['out_value'],
        )
        vbo = self.ctx.buffer(vertices)
        vao = self.ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
        buffer = self.ctx.buffer(reserve=4 * 3)
        vao.transform(buffer, mode=moderngl.TRIANGLES_ADJACENCY)
        self.assertEqual(
            (1.0, 3.0, 5.0),
            struct.unpack('3f', buffer.read())
        )

    def test_geometry_triangle_strip_adjacency(self):
        data = (
            1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0,
        )
        vertices = np.array(data, dtype='f4')
        prog = self.ctx.program(
            vertex_shader=
            """
            #version 330

            in float in_value;

            void main() {
                gl_Position = vec4(in_value);
            }
            """,
            geometry_shader="""
            #version 330

            out float out_value;

            layout(triangles_adjacency) in;
            layout(triangle_strip, max_vertices=3) out;

            void main() {
                out_value = gl_in[0].gl_Position.x;
                EmitVertex();
                out_value = gl_in[2].gl_Position.x;
                EmitVertex();
                out_value = gl_in[4].gl_Position.x;
                EmitVertex();
                EndPrimitive();
            }
            """,
            varyings=['out_value'],
        )
        vbo = self.ctx.buffer(vertices)
        vao = self.ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
        buffer = self.ctx.buffer(reserve=4 * 6)
        vao.transform(buffer, mode=moderngl.TRIANGLE_STRIP_ADJACENCY)
        self.assertEqual(
            (1.0, 3.0, 5.0, 5.0, 3.0, 7.0),
            struct.unpack('6f', buffer.read())
        )

    def gl_error(self, raise_exception=True):
        error = self.ctx.error
        if error != "GL_NO_ERROR" and raise_exception:
            raise ValueError(error)


if __name__ == '__main__':
    unittest.main()
