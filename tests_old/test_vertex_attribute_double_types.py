import struct
import unittest
import warnings

import moderngl

from common import get_context

vtypes = [
    {
        'version': 410,
        'type': 'double',
        'input': [1.0],
        'output': [2.0],
    },
    {
        'version': 410,
        'type': 'dvec2',
        'input': [1.0, 1.0],
        'output': [2.0, 2.0],
    },
    {
        'version': 410,
        'type': 'dvec3',
        'input': [1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0],
    },
    {
        'version': 410,
        'type': 'dvec4',
        'input': [1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 410,
        'type': 'dmat2',
        'input': [1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 410,
        'type': 'dmat2x3',
        'input': [1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 410,
        'type': 'dmat2x4',
        'input': [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 410,
        'type': 'dmat3x2',
        'input': [1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 410,
        'type': 'dmat3',
        'input': [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 410,
        'type': 'dmat3x4',
        'input': [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 410,
        'type': 'dmat4x2',
        'input': [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 410,
        'type': 'dmat4x3',
        'input': [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0],
    },
    {
        'version': 410,
        'type': 'dmat4',
        'input': [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
        'output': [2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0],
    },
]


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context(require=410)
        if not cls.ctx:
            raise unittest.SkipTest('double types not supported')

        # if cls.ctx.vendor.startswith('Intel'):
        #     raise unittest.SkipTest('')

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

            in %(type)s v_in[];
            out %(type)s v_out[];

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
                vao = self.ctx.vertex_array(prog, vbo1, 'v_in')
                vao.transform(vbo2, moderngl.POINTS, 1)

                for a, b in zip(struct.unpack(fmt, vbo2.read()), vtype['output'] * 2):
                    self.assertAlmostEqual(a, b)


if __name__ == '__main__':
    unittest.main()
