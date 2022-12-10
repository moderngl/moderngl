import unittest

import moderngl

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

        cls.vertex_shader = '''
            #version 330
            uniform mat4 Mvp;
            in vec4 vertex;
            uniform Block {
                mat4 Transform;
            };
            void main() {
                gl_Position = Mvp * Transform * vertex;
            }
        '''

        cls.geometry_shader = '''
            #version 330
            layout(points) in;
            uniform vec4 Position;
            layout(line_strip, max_vertices = 2) out;
            out vec4 color;
            void main() {
                gl_Position = gl_in[0].gl_Position + Position;
                EmitVertex();
                gl_Position = gl_in[0].gl_Position - Position;
                EmitVertex();
                EndPrimitive();
            }
        '''

        cls.fragment_shader = '''
            #version 330
            uniform vec4 Color;
            uniform Lights {
                uniform vec4 Light[1];
            };
            out vec4 color;
            void main() {
                color = Color * Light[0];
            }
        '''

    def test_program_uniforms_1(self):
        program = self.ctx.program(vertex_shader=self.vertex_shader)

        self.assertIn('Mvp', program)
        self.assertIn('Block', program)
        self.assertNotIn('Transform', program)

    def test_program_uniforms_2(self):
        program = self.ctx.program(
            vertex_shader=self.vertex_shader,
            geometry_shader=self.geometry_shader,
            fragment_shader=self.fragment_shader,
        )

        self.assertIn('Mvp', program)
        self.assertIn('Position', program)
        self.assertIn('Color', program)

    def test_program_uniform_blocks(self):
        program = self.ctx.program(
            vertex_shader=self.vertex_shader,
            fragment_shader=self.fragment_shader,
        )

        self.assertIn('Block', program)
        self.assertIn('Lights', program)
        self.assertNotIn('Transform', program)
        self.assertNotIn('Light', program)

    def test_program_geometry_primitives_1(self):
        program = self.ctx.program(vertex_shader=self.vertex_shader)

        self.assertIsNone(program.geometry_input)
        self.assertIsNone(program.geometry_output)
        self.assertEqual(program.geometry_vertices, 0)

    def test_program_geometry_primitives_2(self):
        program = self.ctx.program(
            vertex_shader=self.vertex_shader,
            geometry_shader=self.geometry_shader,
        )
        self.assertEqual(program.geometry_vertices, 2)


if __name__ == '__main__':
    unittest.main()
