import glob
import platform
import sys

from setuptools import Extension, setup

target = platform.system().lower()

if target == 'linux':
    from distutils import sysconfig
    cvars = sysconfig.get_config_vars()

    if 'OPT' in cvars:
        sysconfig._config_vars['OPT'] = cvars['OPT'].replace('-Wstrict-prototypes', '')

    if 'CFLAGS' in cvars:
        sysconfig._config_vars['CFLAGS'] = cvars['CFLAGS'].replace('-Wstrict-prototypes', '')

install_requires = []

if sys.version_info < (3, 0):
    raise Exception('Python 2 is not supported!')

if sys.version_info < (3, 5):
    install_requires.append('typing')

libraries = {
    'windows': ['gdi32', 'opengl32', 'user32'],
    'linux': ['GL', 'dl', 'X11'],
    'darwin': [],
}

extra_compile_args = {
    'windows': [],
    'linux': [],
    'darwin': ['-Wno-deprecated-declarations'],
}

extra_linker_args = {
    'windows': [],
    'linux': [],
    'darwin': ['-framework', 'OpenGL', '-Wno-deprecated'],
}

ModernGL = Extension(
    name='ModernGL.mgl',
    include_dirs=['src'],
    define_macros=[],
    libraries=libraries[target],
    extra_compile_args=extra_compile_args[target],
    extra_link_args=extra_linker_args[target],
    sources=glob.glob('src/*.cpp'),
)

short_description = 'ModernGL: PyOpenGL alternative'

long_description = '''
`ModernGL on github <https://github.com/cprogrammer1994/ModernGL>`_

`Documentation <https://moderngl.readthedocs.io/>`_

`Examples <https://moderngl.github.io/Examples.html>`_

OpenGL is a great environment for developing portable, platform independent,
interactive 2D and 3D graphics applications. The API implementation in Python
is cumbersome, resulting in applications with high latency. To solve this
problem we have developed ModernGL, a wrapper over OpenGL that simplifies the
creation of simple graphics applications like scientific simulations, small
games or user interfaces. Usually, acquiring in-depth knowledge of OpenGL
requires a steep learning curve. In contrast, ModernGL is easy to learn and
use, moreover it is capable of rendering with the same performance and quality,
with less code written.
'''

keywords = [
    'ModernGL',
    'modern OpenGL',
    'OpenGL',
    'PyOpenGL',
    'visualization',
    'ray-tracing',
    'compute shader',
    'shader',
    'documentation',
    'graphics',
    'GLSL',
    'GPU',
    'GPGPU',
    'nvidia',
    'amd',
    'GL',
    'GLU',
    'GLEXT',
    'WGL',
    'WGLEXT',
    'ARB',
    'GLX',
    '2D',
    '3D',
    'CAD',
    'design',
    'video',
]

classifiers = [
    'Development Status :: 5 - Production/Stable',
    'Environment :: MacOS X',
    'Environment :: Win32 (MS Windows)',
    'Environment :: X11 Applications',
    'Intended Audience :: Developers',
    'Intended Audience :: Education',
    'Intended Audience :: Science/Research',
    'License :: OSI Approved :: MIT License',
    'Operating System :: MacOS :: MacOS X',
    'Operating System :: Microsoft :: Windows',
    'Operating System :: OS Independent',
    'Operating System :: Unix',
    'Topic :: Artistic Software',
    'Topic :: Desktop Environment',
    'Topic :: Documentation :: Sphinx',
    'Topic :: Games/Entertainment',
    'Topic :: Multimedia',
    'Topic :: Multimedia :: Graphics',
    'Topic :: Multimedia :: Graphics :: 3D Modeling',
    'Topic :: Multimedia :: Graphics :: 3D Rendering',
    'Topic :: Multimedia :: Video :: Display',
    'Topic :: Scientific/Engineering :: Visualization',
    'Programming Language :: Python :: 3',
    'Programming Language :: Python :: 3.4',
    'Programming Language :: Python :: 3.5',
    'Programming Language :: Python :: 3.6',
    'Programming Language :: Python :: 3 :: Only',
]

args = {
    'name': 'ModernGL',
    'version': '4.2.0',
    'description': short_description,
    'long_description': long_description.strip(),
    'url': 'https://github.com/cprogrammer1994/ModernGL',
    'download_url': 'https://github.com/cprogrammer1994/ModernGL/releases',
    'author': 'Szabolcs Dombi',
    'author_email': 'cprogrammer1994@gmail.com',
    'license': 'MIT',
    'classifiers': classifiers,
    'keywords': keywords,
    'packages': ['ModernGL', 'ModernGL.ext'],
    'install_requires': install_requires,
    'ext_modules': [ModernGL],
    'platforms': ['any'],
}

if target == 'windows':
    args['zip_safe'] = False

setup(**args)
