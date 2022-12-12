"""
Test transform feedback.

Note that some of the tests are commented out because they currently don't
work with mesa/lvvmpipe. They are left here for future reference.
"""

from array import array
import struct
import platform
import numpy as np

import pytest
import moderngl

def test_transform(ctx):

    program = ctx.program(
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

    assert program.geometry_input is None
    assert program.geometry_output is None
    assert program.is_transform is True

    assert 'vert' in program
    assert 'vert_length' in program

def test_vertex_points(ctx):
    """Transform with points"""
    print(ctx.error)
    data = (0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0)
    buffer = ctx.buffer(array('f', data))
    program = ctx.program(
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

    vao = ctx.vertex_array(program, [(buffer, "2f", "in_pos"),])
    assert vao.mode == moderngl.POINTS
    buffer2 = ctx.buffer(reserve=buffer.size)
    vao.transform(buffer2, mode=ctx.POINTS)
    assert data == struct.unpack(f"{len(data)}f", buffer.read())

def test_vertex_lines(ctx):
    data = 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0
    buffer = ctx.buffer(array('f', data))
    program = ctx.program(
        vertex_shader="""
        #version 330

        in vec2 in_pos;
        out vec2 out_pos;

        void main() {
            out_pos = in_pos * 2.0;
        }
        """,
        varyings=["out_pos"],
    )

    vao = ctx.vertex_array(program, [(buffer, "2f", "in_pos"),])
    buffer2 = ctx.buffer(reserve=buffer.size)
    vao.transform(buffer2, mode=ctx.LINES)
    assert tuple(v * 2 for v in data) == struct.unpack(f"{len(data)}f", buffer2.read())

# NOTE: This fails in tests and may be non-standard or require higher gl version
# def test_vertex_lines_adjacency(ctx):
#     if platform.system().lower() in ["darwin"]:
#         self.skipTest('Transforms with adjacency primitives now working on OSX')

#     data = (
#         1.0, 1.0,
#         2.0, 2.0,
#         3.0, 3.0,
#         4.0, 4.0,
#     )
#     buffer = ctx.buffer(array('f', data))
#     program = ctx.program(
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
#     vao = ctx.vertex_array(program, [(buffer, "2f", "in_pos"),])
#     buffer2 = ctx.buffer(reserve=4 * 4)
#     vao.transform(buffer2, mode=ctx.LINES_ADJACENCY)

#     # Start end end line is removed
#     self.assertEqual(tuple(data[2:6]), struct.unpack(f"4f", buffer2.read()))

def test_vertex_line_strip(ctx):
    data = (
        1.0, 1.0,
        2.0, 2.0,
        3.0, 3.0,
        4.0, 4.0,
    )
    buffer = ctx.buffer(array('f', data))
    program = ctx.program(
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
    vao = ctx.vertex_array(program, [(buffer, "2f", "in_pos"),])
    buffer2 = ctx.buffer(reserve=4 * 12)
    vao.transform(buffer2, mode=ctx.LINE_STRIP)

    expected = (1.0, 1.0, 2.0, 2.0, 2.0, 2.0, 3.0, 3.0, 3.0, 3.0, 4.0, 4.0)
    assert expected == struct.unpack("12f", buffer2.read())


# NOTE: Fails in tests and might be non-standard or require higher gl version
# def test_vertex_line_strip_adjacency(ctx):
#     if platform.system().lower() in ["darwin"]:
#         self.skipTest('Transforms with adjacency primitives now working on OSX')

#     data = (
#         1.0, 1.0,
#         2.0, 2.0,
#         3.0, 3.0,
#         4.0, 4.0,
#         5.0, 5.0,
#     )
#     buffer = ctx.buffer(array('f', data))
#     program = ctx.program(
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
#     vao = ctx.vertex_array(program, [(buffer, "2f", "in_pos"),])
#     buffer2 = ctx.buffer(reserve=4 * 8)
#     vao.transform(buffer2, mode=ctx.LINE_STRIP_ADJACENCY)
#     self.assertEqual(
#         (2.0, 2.0, 3.0, 3.0, 3.0, 3.0, 4.0, 4.0),
#         struct.unpack("8f", buffer2.read())
#     )

def test_vertex_triangles(ctx):
    data = (
        1.0, 1.0,
        2.0, 2.0,
        3.0, 3.0,
        4.0, 4.0,
        5.0, 5.0,
        6.0, 6.0,
    )
    buffer = ctx.buffer(array('f', data))
    program = ctx.program(
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
    vao = ctx.vertex_array(program, [(buffer, "2f", "in_pos"),])
    buffer2 = ctx.buffer(reserve=4 * 12)
    vao.transform(buffer2, mode=ctx.TRIANGLES)
    assert tuple(v * 2 for v in data) == struct.unpack("12f", buffer2.read())

def test_vertex_triangles_adjacency(ctx):
    if platform.system().lower() in ["darwin"]:
        pytest.skip('Transforms with adjacency primitives now working on OSX')
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
    buffer = ctx.buffer(array('f', data))
    program = ctx.program(
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
    vao = ctx.vertex_array(program, [(buffer, "2f", "in_pos"),])
    buffer2 = ctx.buffer(reserve=4 * 6)
    vao.transform(buffer2, mode=ctx.TRIANGLES_ADJACENCY)
    assert (1.0, 1.0, 3.0, 3.0, 5.0, 5.0) == struct.unpack("6f", buffer2.read())

# def test_vertex_triangle_strip(ctx):
#     data = (
#         1.0, 1.0,
#         2.0, 2.0,
#         3.0, 3.0,
#         4.0, 4.0,
#     )
#     buffer = ctx.buffer(array('f', data))
#     program = ctx.program(
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
#     vao = ctx.vertex_array(program, [(buffer, "2f", "in_pos"),])
#     buffer2 = ctx.buffer(reserve=4 * 12)
#     vao.transform(buffer2, mode=ctx.TRIANGLE_STRIP)
#     expected = (1.0, 1.0, 2.0, 2.0, 3.0, 3.0, 3.0, 3.0, 2.0, 2.0, 4.0, 4.0),
#     assert expected == struct.unpack("12f", buffer2.read())

# def test_vertex_triangle_fan(ctx):
#     vertices = np.array(
#         [
#             0.0, 0.0,  # Initial triangle
#             1.0, 0.0,
#             1.0, 1.0,
#             0.0, 1.0,  # second triangle 
#         ],
#         dtype='f4',
#     )

#     prog = ctx.program(vertex_shader=
#         """
#         #version 330
#         in vec2 in_vert;
#         out vec2 out_vert;
#         void main(void) {
#             out_vert = in_vert;
#         }
#         """,
#         varyings=['out_vert'],
#     )

#     vbo = ctx.buffer(vertices)
#     vao = ctx.vertex_array(prog, [(vbo, '2f', 'in_vert')])
#     N = 6  # 6 vertices in end buffer
#     buffer = ctx.buffer(reserve=N * 4 * 2)
#     vao.transform(buffer, vertices=4, mode=ctx.TRIANGLE_FAN)

#     data = struct.unpack('{}f'.format(N * 2), buffer.read())
#     expected = (
#         0.0, 0.0,  # Initial triangle
#         1.0, 0.0,
#         1.0, 1.0,
#         0.0, 0.0,  # second triangle 
#         1.0, 1.0, 
#         0.0, 1.0,
#     )
#     assert data == expected

# # ------ Geometry shader -------

def test_geometry_points(ctx):
    data = (1.0, 2.0, 3.0, 4.0, 5.0, 6.0)
    vertices = np.array(data, dtype='f4')
    prog = ctx.program(
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
    vbo = ctx.buffer(vertices)
    vao = ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
    buffer = ctx.buffer(reserve=vbo.size)
    vao.transform(buffer, mode=moderngl.POINTS)
    assert tuple(v * 2 for v in data) == struct.unpack('6f', buffer.read())

def test_geometry_lines(ctx):
    data = (
        1.0, 2.0,
        3.0, 4.0,
        5.0, 6.0,
    )
    vertices = np.array(data, dtype='f4')
    prog = ctx.program(
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
    vbo = ctx.buffer(vertices)
    vao = ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
    buffer = ctx.buffer(reserve=vbo.size)
    vao.transform(buffer, mode=moderngl.LINES)
    assert data == struct.unpack('6f', buffer.read())

# def test_geometry_line_loop(ctx):
#     data = (
#         1.0, 2.0, 3.0, 4.0, 5.0, 6.0,
#     )
#     vertices = np.array(data, dtype='f4')
#     prog = ctx.program(
#         vertex_shader=
#         """
#         #version 330

#         in float in_value;

#         void main() {
#             gl_Position = vec4(in_value);
#         }
#         """,
#         geometry_shader="""
#         #version 330

#         out float out_value;

#         layout(lines) in;
#         layout(line_strip, max_vertices=2) out;

#         void main() {
#             out_value = gl_in[0].gl_Position.x;
#             EmitVertex();
#             out_value = gl_in[1].gl_Position.x;
#             EmitVertex();
#             EndPrimitive();
#         }
#         """,
#         varyings=['out_value'],
#     )
#     vbo = ctx.buffer(vertices)
#     vao = ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
#     buffer = ctx.buffer(reserve=4 * 12)
#     vao.transform(buffer, mode=moderngl.LINE_LOOP)
#     expected = (1.0, 2.0, 2.0, 3.0, 3.0, 4.0, 4.0, 5.0, 5.0, 6.0, 6.0, 1.0),
#     assert struct.unpack('12f', buffer.read()) == expected

# def test_geometry_line_strip(ctx):
#     data = (
#         1.0, 2.0, 3.0, 4.0, 5.0
#     )
#     vertices = np.array(data, dtype='f4')
#     prog = ctx.program(
#         vertex_shader=
#         """
#         #version 330

#         in float in_value;

#         void main() {
#             gl_Position = vec4(in_value);
#         }
#         """,
#         geometry_shader="""
#         #version 330

#         out float out_value;

#         layout(lines) in;
#         layout(line_strip, max_vertices=2) out;

#         void main() {
#             out_value = gl_in[0].gl_Position.x;
#             EmitVertex();
#             out_value = gl_in[1].gl_Position.x;
#             EmitVertex();
#             EndPrimitive();
#         }
#         """,
#         varyings=['out_value'],
#     )
#     vbo = ctx.buffer(vertices)
#     vao = ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
#     buffer = ctx.buffer(reserve=4 * 8)
#     vao.transform(buffer, mode=moderngl.LINE_LOOP)
#     expected = (1.0, 2.0, 2.0, 3.0, 3.0, 4.0, 4.0, 5.0),
#     assert struct.unpack('8f', buffer.read()) == expected

# def test_geometry_lines_adjacency(ctx):
#     data = (
#         1.0, 2.0, 3.0, 4.0,
#     )
#     vertices = np.array(data, dtype='f4')
#     prog = ctx.program(
#         vertex_shader=
#         """
#         #version 330

#         in float in_value;

#         void main() {
#             gl_Position = vec4(in_value);
#         }
#         """,
#         geometry_shader="""
#         #version 330

#         out float out_value;

#         layout(lines_adjacency) in;
#         layout(line_strip, max_vertices=2) out;

#         void main() {
#             out_value = gl_in[1].gl_Position.x;
#             EmitVertex();
#             out_value = gl_in[2].gl_Position.x;
#             EmitVertex();
#             EndPrimitive();
#         }
#         """,
#         varyings=['out_value'],
#     )
#     vbo = ctx.buffer(vertices)
#     vao = ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
#     buffer = ctx.buffer(reserve=4 * 2)
#     vao.transform(buffer, mode=moderngl.LINES_ADJACENCY)
#     assert struct.unpack('2f', buffer.read()) == (2.0, 3.0)

# def test_geometry_line_strip_adjacency(ctx):
#     data = (
#         1.0, 2.0, 3.0, 4.0, 5.0,
#     )
#     vertices = np.array(data, dtype='f4')
#     prog = ctx.program(
#         vertex_shader=
#         """
#         #version 330

#         in float in_value;

#         void main() {
#             gl_Position = vec4(in_value);
#         }
#         """,
#         geometry_shader="""
#         #version 330

#         out float out_value;

#         layout(lines_adjacency) in;
#         layout(line_strip, max_vertices=2) out;

#         void main() {
#             out_value = gl_in[1].gl_Position.x;
#             EmitVertex();
#             out_value = gl_in[2].gl_Position.x;
#             EmitVertex();
#             EndPrimitive();
#         }
#         """,
#         varyings=['out_value'],
#     )
#     vbo = ctx.buffer(vertices)
#     vao = ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
#     buffer = ctx.buffer(reserve=4 * 4)
#     vao.transform(buffer, mode=moderngl.LINE_STRIP_ADJACENCY)       
#     assert struct.unpack('4f', buffer.read()) == (2.0, 3.0, 3.0, 4.0)

# # ---

def test_geometry_triangles(ctx):
    data = (
        1.0, 2.0, 3.0, 4.0, 5.0, 6.0,
    )
    vertices = np.array(data, dtype='f4')
    prog = ctx.program(
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
    vbo = ctx.buffer(vertices)
    vao = ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
    buffer = ctx.buffer(reserve=vbo.size)
    vao.transform(buffer, mode=moderngl.TRIANGLES)
    assert struct.unpack('6f', buffer.read()) == data

# def test_geometry_triangle_strip(ctx):
#     data = (
#         1.0, 2.0, 3.0, 4.0,
#     )
#     vertices = np.array(data, dtype='f4')
#     prog = ctx.program(
#         vertex_shader=
#         """
#         #version 330

#         in float in_value;

#         void main() {
#             gl_Position = vec4(in_value);
#         }
#         """,
#         geometry_shader="""
#         #version 330

#         out float out_value;

#         layout(triangles) in;
#         layout(triangle_strip, max_vertices=3) out;

#         void main() {
#             out_value = gl_in[0].gl_Position.x;
#             EmitVertex();
#             out_value = gl_in[1].gl_Position.x;
#             EmitVertex();
#             out_value = gl_in[2].gl_Position.x;
#             EmitVertex();
#             EndPrimitive();
#         }
#         """,
#         varyings=['out_value'],
#     )
#     vbo = ctx.buffer(vertices)
#     vao = ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
#     buffer = ctx.buffer(reserve=4 * 6)
#     vao.transform(buffer, mode=moderngl.TRIANGLE_STRIP)
#     expected = (1.0, 2.0, 3.0, 3.0, 2.0, 4.0)
#     assert struct.unpack('6f', buffer.read()) == expected

# def test_geometry_triangle_fan(ctx):
#     data = (
#         0.0, 1.0, 2.0, 3.0,
#     )
#     vertices = np.array(data, dtype='f4')
#     prog = ctx.program(
#         vertex_shader=
#         """
#         #version 330

#         in float in_value;

#         void main() {
#             gl_Position = vec4(in_value);
#         }
#         """,
#         geometry_shader="""
#         #version 330

#         out float out_value;

#         layout(triangles) in;
#         layout(triangle_strip, max_vertices=3) out;

#         void main() {
#             out_value = gl_in[0].gl_Position.x;
#             EmitVertex();
#             out_value = gl_in[1].gl_Position.x;
#             EmitVertex();
#             out_value = gl_in[2].gl_Position.x;
#             EmitVertex();
#             EndPrimitive();
#         }
#         """,
#         varyings=['out_value'],
#     )
#     vbo = ctx.buffer(vertices)
#     vao = ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
#     buffer = ctx.buffer(reserve=4 * 6)
#     vao.transform(buffer, mode=moderngl.TRIANGLE_FAN)
#     expected = (0.0, 1.0, 2.0, 0.0, 2.0, 3.0)
#     assert struct.unpack('6f', buffer.read()) == expected

# def test_geometry_triangles_adjacency(ctx):
#     data = (
#         1.0, 2.0, 3.0, 4.0, 5.0, 6.0,
#     )
#     vertices = np.array(data, dtype='f4')
#     prog = ctx.program(
#         vertex_shader=
#         """
#         #version 330

#         in float in_value;

#         void main() {
#             gl_Position = vec4(in_value);
#         }
#         """,
#         geometry_shader="""
#         #version 330

#         out float out_value;

#         layout(triangles_adjacency) in;
#         layout(triangle_strip, max_vertices=3) out;

#         void main() {
#             out_value = gl_in[0].gl_Position.x;
#             EmitVertex();
#             out_value = gl_in[2].gl_Position.x;
#             EmitVertex();
#             out_value = gl_in[4].gl_Position.x;
#             EmitVertex();
#             EndPrimitive();
#         }
#         """,
#         varyings=['out_value'],
#     )
#     vbo = ctx.buffer(vertices)
#     vao = ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
#     buffer = ctx.buffer(reserve=4 * 3)
#     vao.transform(buffer, mode=moderngl.TRIANGLES_ADJACENCY)
#     assert struct.unpack('3f', buffer.read()) == (1.0, 3.0, 5.0)

# def test_geometry_triangle_strip_adjacency(ctx):
#     data = (
#         1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0,
#     )
#     vertices = np.array(data, dtype='f4')
#     prog = ctx.program(
#         vertex_shader=
#         """
#         #version 330

#         in float in_value;

#         void main() {
#             gl_Position = vec4(in_value);
#         }
#         """,
#         geometry_shader="""
#         #version 330

#         out float out_value;

#         layout(triangles_adjacency) in;
#         layout(triangle_strip, max_vertices=3) out;

#         void main() {
#             out_value = gl_in[0].gl_Position.x;
#             EmitVertex();
#             out_value = gl_in[2].gl_Position.x;
#             EmitVertex();
#             out_value = gl_in[4].gl_Position.x;
#             EmitVertex();
#             EndPrimitive();
#         }
#         """,
#         varyings=['out_value'],
#     )
#     vbo = ctx.buffer(vertices)
#     vao = ctx.vertex_array(prog, [(vbo, 'f', 'in_value')])
#     buffer = ctx.buffer(reserve=4 * 6)
#     vao.transform(buffer, mode=moderngl.TRIANGLE_STRIP_ADJACENCY)

#     expected = (1.0, 3.0, 5.0, 5.0, 3.0, 7.0)
#     assert struct.unpack('6f', buffer.read()) == expected
