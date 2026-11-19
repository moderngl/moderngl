<div align="center">

[![preview](https://github.com/moderngl/moderngl/assets/11232402/b314f7af-0c0a-4b7d-b4f5-857f426454ca)](#readme)

</div>

# ModernGL

ModernGL is a thin Python layer over OpenGL 3.3 core. It keeps the OpenGL model, removes the boilerplate and checks what can be checked up front.
No dependencies, the window comes from your own library of choice, and the same code runs in the browser with Pyodide and WebGL2.

```
pip install --pre moderngl
```

- [Documentation](https://moderngl.github.io/moderngl/)
- [Examples](https://moderngl.github.io/moderngl/examples.html)
- [Porting from ModernGL 5](https://moderngl.github.io/moderngl/index.html#porting)
- [ModernGL on PyPI](https://pypi.org/project/ModernGL/)
- [ModernGL Discord Server](https://discord.gg/UEMtW8D)

## Features

- Python 3.11 or newer, the free-threaded build included
- No dependencies, no OpenGL headers or libraries needed to build
- The window comes from your own library of choice (pygame-ce, moderngl-window, pyglet, glfw, ...)
- Headless rendering with `moderngl.headless_context()`, no extra package
- Newer OpenGL features (compute, mesh shaders, indirect drawing, queries) through small extension modules

## Sample usage

```py
>>> import moderngl
>>> ctx = moderngl.get_context()
>>> buf = ctx.buffer(b'Hello World!')  # allocated on the GPU
>>> buf.read()
b'Hello World!'
```

The examples run from the repository root, their dependencies are pinned in `requirements.txt`:

```
pip install -r requirements.txt
python examples/hello_triangle.py
```

## TODO

- Upgrade [moderngl-window](https://github.com/moderngl/moderngl-window) to ModernGL 6
- Test the Windows and macOS builds
- Test an [ANGLE](https://chromium.googlesource.com/angle/angle) build to run ModernGL on macOS without OpenGL

## Citation

If you need to cite this repository in academic research:

```txt
@Online{Dombi2020,
  author = {Szabolcs Dombi},
  title = {ModernGL, high performance python bindings for OpenGL 3.3+},
  date = {2020-05-01},
  publisher = {GitHub},
  journal = {GitHub repository},
  howpublished = {\url{https://github.com/moderngl/moderngl}},
  commit = {<insert hash if needed>}
}
```

The commit hash can be found in the [Releases](https://github.com/moderngl/moderngl/releases).

## Community

- [ModernGL Discord Server](https://discord.gg/UEMtW8D)
- [Contributors](https://github.com/moderngl/moderngl/graphs/contributors)
