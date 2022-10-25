import os
import platform
import re
import sys
from distutils.sysconfig import get_config_var
from distutils.version import LooseVersion

from setuptools import Extension, setup

# pylint: disable=C0103, W0212

if sys.version_info < (3, 6):
    raise Exception('Python 3.6 or higher is required')

PLATFORMS = {'windows', 'linux', 'darwin', 'cygwin', 'android'}

target = platform.system().lower()

if 'pydroid3' in sys.executable.lower():
    target = 'android'

for known in PLATFORMS:
    if target.startswith(known):
        target = known

if target not in PLATFORMS:
    target = 'linux'

# For mac, ensure extensions are built for macos 10.9 when compiling on a
# 10.9 system or above, overriding distuitls behavior which is to target
# the version that python was built for. This may be overridden by setting
# MACOSX_DEPLOYMENT_TARGET before calling setup.py
if target == 'darwin':
    if 'MACOSX_DEPLOYMENT_TARGET' not in os.environ:
        current_system = LooseVersion(platform.mac_ver()[0])
        python_target = LooseVersion(get_config_var('MACOSX_DEPLOYMENT_TARGET'))
        if python_target < '10.9' and current_system >= '10.9':
            os.environ['MACOSX_DEPLOYMENT_TARGET'] = '10.9'

if target in ['linux', 'cygwin']:
    from distutils import sysconfig
    cvars = sysconfig.get_config_vars()
    
    if hasattr(sysconfig, '_config_vars') and sysconfig._config_vars is not None:
        if 'OPT' in cvars:
            sysconfig._config_vars['OPT'] = cvars['OPT'].replace('-Wstrict-prototypes', '')
            sysconfig._config_vars['OPT'] = cvars['OPT'].replace('-Wimplicit-function-declaration', '')

        if 'CFLAGS' in cvars:
            sysconfig._config_vars['CFLAGS'] = cvars['CFLAGS'].replace('-Wstrict-prototypes', '')
            sysconfig._config_vars['CFLAGS'] = cvars['CFLAGS'].replace('-Wimplicit-function-declaration', '')

libraries = {
    'windows': [],
    'linux': [],
    'cygwin': [],
    'darwin': [],
    'android': [],
}

extra_compile_args = {
    'windows': [],
    'linux': [],
    'cygwin': [],
    'darwin': ['-Wno-deprecated-declarations'],
    'android': [],
}

extra_linker_args = {
    'windows': [],
    'linux': [],
    'cygwin': [],
    'darwin': [],
    'android': [],
}

mgl = Extension(
    name='moderngl.mgl',
    include_dirs=['src', 'moderngl', 'moderngl/mgl'],
    define_macros=[],
    libraries=libraries[target],
    extra_compile_args=extra_compile_args[target],
    extra_link_args=extra_linker_args[target],
    sources=[
        'moderngl/src/Sampler.cpp',
        'moderngl/src/Attribute.cpp',
        'moderngl/src/Buffer.cpp',
        'moderngl/src/BufferFormat.cpp',
        'moderngl/src/ComputeShader.cpp',
        'moderngl/src/Context.cpp',
        'moderngl/src/DataType.cpp',
        'moderngl/src/Error.cpp',
        'moderngl/src/Framebuffer.cpp',
        'moderngl/src/InvalidObject.cpp',
        'moderngl/src/ModernGL.cpp',
        'moderngl/src/Program.cpp',
        'moderngl/src/Query.cpp',
        'moderngl/src/Renderbuffer.cpp',
        'moderngl/src/Scope.cpp',
        'moderngl/src/Texture.cpp',
        'moderngl/src/Texture3D.cpp',
        'moderngl/src/TextureArray.cpp',
        'moderngl/src/TextureCube.cpp',
        'moderngl/src/Uniform.cpp',
        'moderngl/src/UniformBlock.cpp',
        'moderngl/src/UniformGetters.cpp',
        'moderngl/src/UniformSetters.cpp',
        'moderngl/src/VertexArray.cpp',
    ],
    depends=[
        'moderngl/src/gl_methods.hpp',
        'moderngl/src/OpenGL.hpp',

        'moderngl/src/BufferFormat.hpp',
        'moderngl/src/Error.hpp',
        'moderngl/src/InlineMethods.hpp',
        'moderngl/src/OpenGL.hpp',
        'moderngl/src/Python.hpp',
        'moderngl/src/Types.hpp',
        'moderngl/src/UniformGetSetters.hpp',
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

project_urls = {
    'Documentation': 'https://moderngl.readthedocs.io/',
    'Source': 'https://github.com/moderngl/moderngl/',
    'Bug Tracker': 'https://github.com/moderngl/moderngl/issues/',
}

setup(
    name='moderngl',
    version='5.7.0',
    description=short_description,
    long_description=long_description,
    long_description_content_type='text/markdown',
    url='https://github.com/moderngl/moderngl',
    author='Szabolcs Dombi',
    author_email='cprogrammer1994@gmail.com',
    license='MIT',
    project_urls=project_urls,
    classifiers=classifiers,
    keywords=keywords,
    packages=['moderngl', 'moderngl.program_members'],
    ext_modules=[mgl],
    platforms=['any'],
    install_requires=['glcontext>=2.3.6,<3'],
)
