import struct
import unittest
import warnings

import moderngl

from common import get_context

vtypes = [
    {
        'version': 330,
        'type': 'int',
        'input': [-1],
        'output': [-2],
    },
    {
        'version': 330,
        'type': 'ivec2',
        'input': [-1, -1],
        'output': [-2, -2],
    },
    {
        'version': 330,
        'type': 'ivec3',
        'input': [-1, -1, -1],
        'output': [-2, -2, -2],
    },
    {
        'version': 330,
        'type': 'ivec4',
        'input': [-1, -1, -1, -1],
        'output': [-2, -2, -2, -2],
    },
    {
        'version': 330,
        'type': 'uint',
        'input': [1],
        'output': [2],
    },
    {
        'version': 330,
        'type': 'uvec2',
        'input': [1, 1],
        'output': [2, 2],
    },
    {
        'version': 330,
        'type': 'uvec3',
        'input': [1, 1, 1],
        'output': [2, 2, 2],
    },
    {
        'version': 330,
        'type': 'uvec4',
        'input': [1, 1, 1, 1],
        'output': [2, 2, 2, 2],
    },
    {
        'version': 330,
        'type': 'float',
        'input': [1.0],
        'output': [2.0],
    },
    {
        'version': 330,
        'type': 'vec2',
        'input': [1.0, 1.0],
        'output': [2.0, 2.0],
    },
    {
        'version': 330,
        'type': 'vec3',
        'input': [1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0],
    },
    {
        'version': 330,
        'type': 'vec4',
        'input': [1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 330,
        'type': 'mat2',
        'input': [1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 330,
        'type': 'mat2x3',
        'input': [1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 330,
        'type': 'mat2x4',
        'input': [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 330,
        'type': 'mat3x2',
        'input': [1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 330,
        'type': 'mat3',
        'input': [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 330,
        'type': 'mat3x4',
        'input': [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 330,
        'type': 'mat4x2',
        'input': [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 330,
        'type': 'mat4x3',
        'input': [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 330,
        'type': 'mat4',
        'input': [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0],
    },
]


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context(require=330)

    def test_simple(self):
        vert_src = '''
            #version %(version)s

            in %(type)s v_in;
            out %(type)s v_out;

            void main() {
                v_out = v_in + v_in;
            }
        '''

        for vtype in vtypes:
            with self.subTest(vert_src=vert_src, vtype=vtype):
                if self.ctx.version_code < vtype['version']:
                    warnings.warn('skipping version %s' % vtype['version'])
                    continue

                prog = self.ctx.program(vertex_shader=vert_src % vtype, varyings=['v_out'])

                if prog.get('v_in', None) is None:
                    warnings.warn('skipping %s' % vtype['type'])
                    continue

                fmt = moderngl.detect_format(prog, ['v_in'], mode='struct')
                vbo1 = self.ctx.buffer(struct.pack(fmt, *vtype['input']))
                vbo2 = self.ctx.buffer(b'\xAA' * struct.calcsize(fmt))
                vao = self.ctx.simple_vertex_array(prog, vbo1, 'v_in')
                vao.transform(vbo2, moderngl.POINTS, 1)

                for a, b in zip(struct.unpack(fmt, vbo2.read()), vtype['output']):
                    self.assertAlmostEqual(a, b)

    def test_arrays(self):
        vert_src = '''
            #version %(version)s

            in %(type)s v_in[2];
            out %(type)s v_out[2];

            void main() {
                v_out[0] = v_in[0] + v_in[0];
                v_out[1] = v_in[1] + v_in[1];
            }
        '''

        for vtype in vtypes:
            with self.subTest(vert_src=vert_src, vtype=vtype):
                if self.ctx.version_code < vtype['version']:
                    warnings.warn('skipping version %s' % vtype['version'])
                    continue

                prog = self.ctx.program(vertex_shader=vert_src % vtype, varyings=['v_out'])

                if prog.get('v_in', None) is None:
                    warnings.warn('skipping %s' % vtype['type'])
                    continue

                fmt = moderngl.detect_format(prog, ['v_in'], mode='struct')
                vbo1 = self.ctx.buffer(struct.pack(fmt, *(vtype['input'] * 2)))
                vbo2 = self.ctx.buffer(b'\xAA' * struct.calcsize(fmt))
                vao = self.ctx.simple_vertex_array(prog, vbo1, 'v_in')
                vao.transform(vbo2, moderngl.POINTS, 1)

                for a, b in zip(struct.unpack(fmt, vbo2.read()), vtype['output'] * 2):
                    self.assertAlmostEqual(a, b)


if __name__ == '__main__':
    unittest.main()
