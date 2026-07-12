import os
import platform
import re
import sys

from setuptools import Extension, setup

PLATFORMS = {"windows", "linux", "darwin", "cygwin", "android"}

target = platform.system().lower()

if "pydroid3" in sys.executable.lower():
    target = "android"

for known in PLATFORMS:
    if target.startswith(known):
        target = known

if target not in PLATFORMS:
    target = "linux"

libraries = {
    "windows": [],
    "linux": [],
    "cygwin": [],
    "darwin": [],
    "android": [],
}

extra_compile_args = {
    "windows": ["-fpermissive"],
    "linux": ["-fpermissive"],
    "cygwin": ["-fpermissive"],
    "darwin": ["-Wno-deprecated-declarations"],
    "android": ["-fpermissive"],
}

extra_linker_args = {
    "windows": [],
    "linux": [],
    "cygwin": [],
    "darwin": [],
    "android": [],
}

if os.getenv("MODERNGL_COVERAGE"):
    extra_compile_args[target] += ["-O0", "--coverage"]
    extra_linker_args[target] += ["-O0", "--coverage"]

mgl = Extension(
    name="moderngl.mgl",
    libraries=libraries[target],
    extra_compile_args=extra_compile_args[target],
    extra_link_args=extra_linker_args[target],
    sources=["src/moderngl.cpp"],
)

short_description = "ModernGL: High performance rendering for Python 3"

with open("README.md") as f:
    long_description = re.sub(r"</?div[^>]*>|\r", "", f.read(), flags=re.M)

keywords = [
    "ModernGL",
    "OpenGL",
    "PyOpenGL",
    "rendering",
    "graphics",
    "shader",
    "GLSL",
    "GPU",
    "visualization",
    "2D",
    "3D",
]

classifiers = [
    "Development Status :: 5 - Production/Stable",
    "License :: OSI Approved",
    "License :: OSI Approved :: MIT License",
    "Operating System :: OS Independent",
    "Topic :: Games/Entertainment",
    "Topic :: Multimedia :: Graphics",
    "Topic :: Multimedia :: Graphics :: 3D Rendering",
    "Topic :: Scientific/Engineering :: Visualization",
    "Programming Language :: Python :: 3 :: Only",
]

project_urls = {
    "Documentation": "https://moderngl.readthedocs.io/",
    "Source": "https://github.com/moderngl/moderngl/",
    "Bug Tracker": "https://github.com/moderngl/moderngl/issues/",
}

setup(
    name="moderngl",
    version="5.12.1",
    description=short_description,
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/moderngl/moderngl",
    author="Szabolcs Dombi",
    author_email="szabolcs@szabolcsdombi.com",
    license="MIT",
    project_urls=project_urls,
    classifiers=classifiers,
    keywords=keywords,
    include_package_data=True,
    package_data={"moderngl-stubs": ["__init__.pyi"]},
    packages=["moderngl", "moderngl-stubs"],
    py_modules=["_moderngl"],
    ext_modules=[mgl],
    platforms=["any"],
    extras_require={
        "headless": ["glcontext>=3.0.0"],
    },
    install_requires=["glcontext>=3.0.0"],
    python_requires=">=3.7",
)
