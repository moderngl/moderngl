import unittest

import moderngl

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_1(self):
        prog = self.ctx.program(
            vertex_shader='''
                #version 330

                in vec2 in_vert;
                out vec2 v_vert;

                uniform mat2 Mvp;

                uniform VertexShaderUniforms {
                    vec2 Position;
                    float Scale;
                };

                void main() {
                    v_vert = Position + in_vert * Scale;
                    gl_Position = vec4(Mvp * v_vert, 0.0, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330

                uniform bool UseTexture;
                uniform sampler2D Texture;

                uniform FragmentShaderUniforms {
                    vec3 Color;
                    float Alpha;
                };

                in vec2 v_vert;
                out vec4 f_color;

                void main() {
                    if (UseTexture) {
                        f_color = texture(Texture, v_vert);
                        f_color.rgb *= Color;
                        f_color.a *= Alpha;
                    } else {
                        f_color = vec4(Color, Alpha);
                    }
                }
            ''',
        )

        self.assertIn('in_vert', prog)
        self.assertNotIn('Position', prog)
        self.assertNotIn('Scale', prog)
        self.assertIn('Mvp', prog)
        self.assertIn('UseTexture', prog)
        self.assertIn('Texture', prog)
        self.assertNotIn('Color', prog)
        self.assertNotIn('Alpha', prog)
        self.assertIn('VertexShaderUniforms', prog)
        self.assertIn('FragmentShaderUniforms', prog)

        self.assertIsInstance(prog['in_vert'], moderngl.Attribute)
        self.assertIsInstance(prog['Mvp'], moderngl.Uniform)
        self.assertIsInstance(prog['UseTexture'], moderngl.Uniform)
        self.assertIsInstance(prog['Texture'], moderngl.Uniform)
        self.assertIsInstance(prog['VertexShaderUniforms'], moderngl.UniformBlock)
        self.assertIsInstance(prog['FragmentShaderUniforms'], moderngl.UniformBlock)


if __name__ == '__main__':
    unittest.main()
