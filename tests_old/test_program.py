import unittest

from common import get_context
import moderngl


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_program(self):
        program = self.ctx.program(
            vertex_shader='''
                #version 330

                uniform vec2 pos;
                uniform float scale;

                in vec2 vert;
                out vec2 v_vert;

                void main() {
                    gl_Position = vec4(pos + vert * scale, 0.0, 1.0);
                    v_vert = vert;
                }
            ''',
            fragment_shader='''
                #version 330

                in vec2 v_vert;
                out vec4 color;

                void main() {
                    color = vec4(v_vert, 0.0, 1.0);
                }
            ''',
        )

        self.assertIn('vert', program)
        self.assertIn('pos', program)
        self.assertIn('scale', program)
        self.assertIs(program.is_transform, False)

        self.assertIsInstance(program['vert'], moderngl.Attribute)
        self.assertIsInstance(program['pos'], moderngl.Uniform)
        self.assertIsInstance(program['scale'], moderngl.Uniform)

    def test_geo_input_output_primitive(self):
        """Test detection of all geo shader input/output primitives types"""
        input_types = [
            ("points", moderngl.POINTS),
            ("lines", moderngl.LINES),
            ("lines_adjacency", moderngl.LINES_ADJACENCY),
            ("triangles", moderngl.TRIANGLES),
            ("triangles_adjacency", moderngl.TRIANGLES_ADJACENCY),
        ]
        output_types = [
            ("points", moderngl.POINTS),
            ("line_strip", moderngl.LINES),
            ("triangle_strip", moderngl.TRIANGLES),
        ]

        for out_name, out_type in output_types:
            for in_name, in_type in input_types:
                p = self.ctx.program(
                    vertex_shader="""
                    #version 330
                    void main() {
                        gl_Position = vec4(1.0);
                    }
                    """,
                    geometry_shader=(
                        "#version 330\n"
                        f"layout({in_name}) in;\n"
                        f"layout({out_name}, max_vertices=6) out;\n"
                        "void main() {\n"
                        "    gl_Position = vec4(1.0);\n"
                        "    EmitVertex();\n"
                        "    EndPrimitive();\n"
                        "}\n"
                    ),
                )
                self.assertEqual(p.geometry_input, in_type)
                self.assertEqual(p.geometry_output, out_type, msg=f"input: {in_name}, output: {out_name}")


if __name__ == '__main__':
    unittest.main()
