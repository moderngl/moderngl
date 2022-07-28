import inspect
import os
import re
import unittest
from types import ModuleType

import moderngl


class TestCase(unittest.TestCase):
    root = os.path.dirname(os.path.dirname(__file__))
    maxDiff = 10_000

    def validate_cls(self, filename, classname, ignore, include=None):
        docs = self._read_rst_file(filename)

        methods = re.findall(r'^\.\. automethod:: ([^\(\n]+)', docs, flags=re.M)
        attributes = re.findall(r'^\.\. autoattribute:: ([^\n]+)', docs, flags=re.M)

        documented = set(filter(lambda x: x.startswith(classname), methods + attributes))
        implemented = set(classname + '.' + x for x in dir(getattr(moderngl, classname)) if not x.startswith('_'))

        include = set('{}.{}'.format(classname, e) for e in include or [])
        ignored = set(classname + '.' + x for x in ignore)

        self.assertSetEqual(implemented - documented - ignored, set(), msg='Implemented but not Documented')
        self.assertSetEqual(documented - implemented - include, set(), msg='Documented but not Implemented')

    def validate_module(self, filename, ignore=[], include=None):
        docs = self._read_rst_file(filename)

        functions = re.findall(r'^\.\. autofunction:: ([^\(\n]+)', docs, flags=re.M)
        attributes = [f.split(".")[-1] for f in re.findall(r'^\.\. autodata:: ([^\n]+)', docs, flags=re.M)]
        documented = set(functions + attributes)
        implemented = set(x for x in dir(moderngl) if not x.startswith('_') and not isinstance(getattr(moderngl, x), ModuleType) and not inspect.isclass(getattr(moderngl, x)))
        ignored = set(ignore)
        include = set()
        self.assertSetEqual(implemented - documented - ignored, set(), msg='Implemented but not Documented')
        self.assertSetEqual(documented - implemented - include, set(), msg='Documented but not Implemented')

    def _read_rst_file(self, filename):
        with open(os.path.normpath(os.path.join(self.root, 'docs', 'reference', filename))) as f:
            return f.read()

    def test_context_docs(self):
        self.validate_cls('context.rst', 'Context', ['core_profile_check'], ['__enter__', '__exit__'])

    def test_program_docs(self):
        self.validate_cls('program.rst', 'Program', [], ['__getitem__', '__setitem__', '__eq__', '__iter__'])

    def test_vertex_array_docs(self):
        self.validate_cls('vertex_array.rst', 'VertexArray', [])

    def test_buffer_docs(self):
        self.validate_cls('buffer.rst', 'Buffer', [])

    def test_texture_docs(self):
        self.validate_cls('texture.rst', 'Texture', [])

    def test_texture_array_docs(self):
        self.validate_cls('texture_array.rst', 'TextureArray', [])

    def test_texture3d_docs(self):
        self.validate_cls('texture3d.rst', 'Texture3D', [])

    def test_texture_cube_docs(self):
        self.validate_cls('texture_cube.rst', 'TextureCube', [])

    def test_framebuffer_docs(self):
        self.validate_cls('framebuffer.rst', 'Framebuffer', [])

    def test_renderbuffer_docs(self):
        self.validate_cls('renderbuffer.rst', 'Renderbuffer', [])

    def test_query_docs(self):
        self.validate_cls('query.rst', 'Query', [])

    def test_scope_docs(self):
        self.validate_cls('scope.rst', 'Scope', [], include=['__enter__', '__exit__'])

    def test_compute_shader_docs(self):
        self.validate_cls('compute_shader.rst', 'ComputeShader', [], ['__getitem__', '__setitem__', '__eq__', '__iter__'])

    def test_subroutine_docs(self):
        self.validate_cls('subroutine.rst', 'Subroutine', [])

    def test_uniform_docs(self):
        self.validate_cls('uniform.rst', 'Uniform', [])

    def test_uniform_block_docs(self):
        self.validate_cls('uniform_block.rst', 'UniformBlock', [])

    def test_varying_docs(self):
        self.validate_cls('varying.rst', 'Varying', [])

    def test_conditional_render_docs(self):
        self.validate_cls('conditional_render.rst', 'ConditionalRender', [])

    def test_sampler_docs(self):
        self.validate_cls('sampler.rst', 'Sampler', [])

    def test_moderngl_docs(self):
        self.validate_module(
            'moderngl.rst',
            ignore=[
            ],
        )


if __name__ == '__main__':
    unittest.main()
