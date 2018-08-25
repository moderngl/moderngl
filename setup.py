#!/usr/bin/env python
import platform
import re
import sys

from setuptools import Extension, setup, find_namespace_packages

# pylint: disable=C0103, W0212

if sys.version_info < (3, 0):
    raise Exception('Python 2 is not supported!')

PLATFORMS = {'windows', 'linux', 'darwin', 'cygwin'}

target = platform.system().lower()

for known in PLATFORMS:
    if target.startswith(known):
        target = known

if target not in PLATFORMS:
    target = 'linux'

if target in ['linux', 'cygwin']:
    from distutils import sysconfig
    cvars = sysconfig.get_config_vars()

    if 'OPT' in cvars:
        sysconfig._config_vars['OPT'] = cvars['OPT'].replace('-Wstrict-prototypes', '')
        sysconfig._config_vars['OPT'] = cvars['OPT'].replace('-Wimplicit-function-declaration', '')

    if 'CFLAGS' in cvars:
        sysconfig._config_vars['CFLAGS'] = cvars['CFLAGS'].replace('-Wstrict-prototypes', '')
        sysconfig._config_vars['CFLAGS'] = cvars['CFLAGS'].replace('-Wimplicit-function-declaration', '')

install_requires = []

if sys.version_info < (3, 5):
    install_requires.append('typing')

libraries = {
    'windows': ['gdi32', 'opengl32', 'user32'],
    'linux': ['GL', 'dl', 'X11'],
    'cygwin': ['GL', 'X11'],
    'darwin': [],
}

extra_compile_args = {
    'windows': [],
    'linux': [],
    'cygwin': [],
    'darwin': ['-Wno-deprecated-declarations'],
}

extra_linker_args = {
    'windows': [],
    'linux': [],
    'cygwin': [],
    'darwin': ['-framework', 'OpenGL', '-Wno-deprecated'],
}

mgl = Extension(
    name='moderngl.mgl',
    include_dirs=['src'],
    define_macros=[],
    libraries=libraries[target],
    extra_compile_args=extra_compile_args[target],
    extra_link_args=extra_linker_args[target],
    sources=[
        'src/Sampler.cpp',
        'src/Attribute.cpp',
        'src/Buffer.cpp',
        'src/BufferFormat.cpp',
        'src/ComputeShader.cpp',
        'src/Context.cpp',
        'src/DataType.cpp',
        'src/Error.cpp',
        'src/Framebuffer.cpp',
        'src/GLContext.cpp',
        'src/GLMethods.cpp',
        'src/InvalidObject.cpp',
        'src/ModernGL.cpp',
        'src/Program.cpp',
        'src/Query.cpp',
        'src/Renderbuffer.cpp',
        'src/Scope.cpp',
        'src/Texture.cpp',
        'src/Texture3D.cpp',
        'src/TextureArray.cpp',
        'src/TextureCube.cpp',
        'src/Uniform.cpp',
        'src/UniformBlock.cpp',
        'src/UniformGetters.cpp',
        'src/UniformSetters.cpp',
        'src/VertexArray.cpp',
    ],
)

short_description = 'ModernGL: High performance rendering for Python 3'

with open('README.md') as f:
    long_description = f.read()
    long_description = re.sub(r'</?div[^>]*>|\r', '', long_description, flags=re.M)

keywords = [
    'ModernGL',
    'OpenGL',
    'PyOpenGL',
    'rendering',
    'graphics',
    'shader',
    'GLSL',
    'GPU',
    'visualization',
    '2D',
    '3D',
]

classifiers = [
    'Development Status :: 5 - Production/Stable',
    'License :: OSI Approved :: MIT License',
    'Operating System :: OS Independent',
    'Topic :: Games/Entertainment',
    'Topic :: Multimedia :: Graphics',
    'Topic :: Multimedia :: Graphics :: 3D Rendering',
    'Topic :: Scientific/Engineering :: Visualization',
    'Programming Language :: Python :: 3 :: Only',
]

setup(
    name='moderngl',
    version='5.4.2',
    description=short_description,
    long_description=long_description,
    long_description_content_type='text/markdown',
    url='https://github.com/cprogrammer1994/ModernGL',
    author='Szabolcs Dombi',
    author_email='cprogrammer1994@gmail.com',
    license='MIT',
    classifiers=classifiers,
    keywords=keywords,
    packages=find_namespace_packages(include=['moderngl', 'moderngl.*']),
    install_requires=install_requires,
    ext_modules=[mgl],
    platforms=['any'],
)
