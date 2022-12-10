import unittest

import moderngl
from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context(require=430)
        if not cls.ctx:
            raise unittest.SkipTest('Compute shader not supported')

    def test_compute_shader(self):
        if self.ctx.version_code < 430:
            self.skipTest('OpenGL 4.3 is not supported')

        src = '''
            #version 430
            layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

            struct Something
            {
                vec4 pos;
            };

            layout(std430, binding=0) buffer something_in
            {
                Something entries[];
            } In;

            // output into the buffer bound to 1
            layout(std430, binding=1) buffer something_out
            {
                Something entries[];
            } Out;

            uniform float multiplier;

            void main() {
                int x = int(gl_GlobalInvocationID);
                Something entry = In.entries[x];

                Out.entries[x] = Something(entry.pos * multiplier);
            }
        '''

        self.buffer_in = self.ctx.buffer(reserve=16)
        self.buffer_out = self.ctx.buffer(reserve=16)

        self.buffer_in.bind_to_storage_buffer(binding=0)
        self.buffer_out.bind_to_storage_buffer(binding=1)

        compute_shader = self.ctx.compute_shader(src)

        self.assertIsInstance(compute_shader.glo, int)
        self.assertIsNotNone(compute_shader.mglo)
        compute_shader.extra = {'name': 'test'}
        self.assertEqual(compute_shader.extra, {'name': 'test'})
        self.assertIsInstance(compute_shader.ctx, moderngl.Context)

        compute_shader['multiplier'] = 1.0
        compute_shader['multiplier'].value = 1.0
        compute_shader.run()
        self.assertIsNotNone(compute_shader.get('multiplier', None))
        self.assertEqual(compute_shader, compute_shader)
        self.assertEqual([i for i in compute_shader], ['multiplier'])
        compute_shader.release()


if __name__ == '__main__':
    unittest.main()
