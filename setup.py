import os
import sys
import sysconfig

from setuptools import Extension, setup

wasm = os.getenv('PYODIDE') or str(sysconfig.get_config_var('HOST_GNU_TYPE')).startswith('wasm')
libraries = [] if wasm else {'linux': ['dl'], 'win32': ['user32', 'gdi32']}.get(sys.platform, [])
define_macros = []
extra_args = ['-O0', '--coverage'] if os.getenv('MODERNGL_COVERAGE') else []

if wasm:
    define_macros += [('EXTERN_GL', None)]

setup(
    name='moderngl',
    version='6.0.0b2',
    description='ModernGL: a thin Python layer over OpenGL 3.3 core',
    long_description=open('README.md').read(),
    long_description_content_type='text/markdown',
    url='https://github.com/moderngl/moderngl',
    author='Szabolcs Dombi',
    license='MIT',
    python_requires='>=3.11',
    ext_modules=[Extension(name='moderngl', sources=['moderngl.c'], depends=['gl.h'], libraries=libraries, define_macros=define_macros, extra_compile_args=extra_args, extra_link_args=extra_args)],
    packages=['moderngl-stubs'],
    package_data={'moderngl-stubs': ['__init__.pyi']},
    include_package_data=True,
)
