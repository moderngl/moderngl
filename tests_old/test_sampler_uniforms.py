import unittest

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_sampler_2D_getsetter(self):
        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330
                uniform sampler2D Texture2D;
                void main() {
                    gl_Position = texture(Texture2D, vec2(0.0, 0.0));
                }
            '''
        )

        sampler_uniform = self.prog['Texture2D']

        sampler_uniform.value = 3
        self.assertEqual(sampler_uniform.value, 3)

        sampler_uniform.value = 1
        self.assertEqual(sampler_uniform.value, 1)

    def test_sampler_3D_getsetter(self):
        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330
                uniform sampler3D Texture3D;
                void main() {
                    gl_Position = texture(Texture3D, vec3(0.0, 0.0, 0.0));
                }
            '''
        )

        sampler_uniform = self.prog['Texture3D']

        sampler_uniform.value = 3
        self.assertEqual(sampler_uniform.value, 3)

        sampler_uniform.value = 1
        self.assertEqual(sampler_uniform.value, 1)


if __name__ == '__main__':
    unittest.main()
