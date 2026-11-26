<div align="center">

[![preview](https://github.com/moderngl/moderngl/assets/11232402/b314f7af-0c0a-4b7d-b4f5-857f426454ca)](#readme)

</div>

# ModernGL

ModernGL is a thin Python layer over OpenGL 3.3 core. It keeps the OpenGL model, removes the boilerplate and checks what can be checked up front.
No dependencies, the window comes from your own library of choice, and the same code runs in the browser with Pyodide and WebGL2.

```
pip install --pre moderngl
```

- [Documentation](https://moderngl.readthedocs.io/en/latest/)
- [Examples in the Browser](https://moderngl.readthedocs.io/en/latest/examples.html)
- [Porting from ModernGL 5](https://moderngl.readthedocs.io/en/latest/index.html#porting)
- [ModernGL on PyPI](https://pypi.org/project/ModernGL/)
- [ModernGL Discord Server](https://discord.gg/UEMtW8D)

## Features

- Python 3.11 or newer, the free-threaded build included
- No dependencies, no OpenGL headers or libraries needed to build
- The window comes from your own library of choice (pygame-ce, moderngl-window, pyglet, glfw, ...)
- Headless rendering with `moderngl.headless_context()`, no extra package, no display server, see [Headless](https://moderngl.readthedocs.io/en/latest/index.html#headless)
- Compute shaders, storage buffers and indirect drawing on OpenGL 4.3, storage images on OpenGL 4.2
- Newer OpenGL features (mesh shaders, GPU draw counts, queries) through small extension modules

## moderngl-window

[moderngl-window](https://github.com/moderngl/moderngl-window) creates windows and loads resources.

- TODO: port it to ModernGL 6

## Sample usage

```py
>>> import moderngl
>>> ctx = moderngl.get_context()
>>> buf = ctx.buffer(b'Hello World!')  # allocated on the GPU
>>> buf.read().wait()
b'Hello World!'
```

The examples run from the repository root:

```
git clone https://github.com/moderngl/moderngl.git
cd moderngl
pip install -e . -r examples/requirements.txt
python examples/hello_triangle.py
```

## Easy to use with Pillow

```py
img = Image.open('texture.jpg').convert('RGBA')
texture = ctx.texture(img.size, 'rgba8unorm', img.tobytes())
```

## Easy to use with NumPy

```py
buf = ctx.buffer(np.array([0.0, 0.0, 1.0, 1.0], 'f4'))
```

## Notes

ModernGL is implemented in C, a single render call with many OpenGL calls behind it counts as a single Python function call.

ModernGL requires **OpenGL 3.3** core with the fixed primitive restart index (OpenGL 4.3 or `ARB_ES3_compatibility`). Storage images require **OpenGL 4.2**, compute shaders and storage buffers require **OpenGL 4.3**.

ModernGL calls the OpenGL API and provides a Pythonic API instead.
It works with any window library that creates an OpenGL context, and without a window too.

ModernGL does not implement the full OpenGL feature set. You can interact with the ModernGL objects from OpenGL.

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
