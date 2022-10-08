
From PyPI (pip)
---------------

ModernGL is available on PyPI for Windows, OS X and Linux as pre-built
wheels. No complication is needed unless you are setting up a
development environment.

.. code-block:: sh

    $ pip install moderngl

Verify that the package is working:

.. code:: sh

    $ python -m moderngl
    moderngl 5.6.0
    --------------
    vendor: NVIDIA Corporation
    renderer: GeForce RTX 2080 SUPER/PCIe/SSE2
    version: 3.3.0 NVIDIA 441.87
    python: 3.7.6 (tags/v3.7.6:43364a7ae0, Dec 19 2019, 00:42:30) [MSC v.1916 64 bit (AMD64)]
    platform: win32
    code: 330

.. Note:: If you experience issues it's probably related to context creation.
          More configuration might be needed to run moderngl in some cases.
          This is especially true on linux running without X. See the context section.

Development Environment
-----------------------

Ideally you want to fork the repository first.

.. code-block:: sh

    # .. or clone for your fork
    git clone https://github.com/moderngl/moderngl.git
    cd moderngl

Building on various platforms:

* On Windows you need visual c++ build tools installed:
  https://visualstudio.microsoft.com/visual-cpp-build-tools/
* On OS X you need X Code installed + command line tools
  (``xcode-select --install``)
* Building on linux should pretty much work out of the box
* To compile moderngl: ``python setup.py build_ext --inplace``

Package and dev dependencies:

* Install ``requirements.txt``, ``tests/requirements.txt`` and ``docs/requirements.txt``
* Install the package in editable mode: ``pip install -e .``

Using with Mesa 3D on Windows
-----------------------------

If you have an old Graphics Card that raises errors when running moderngl, you can try using
this method, to make Moderngl work.

There are essentially two ways,

* Compiling Mesa yourselves see https://docs.mesa3d.org/install.html.
* Using msys2, which provides pre-compiled Mesa binaries.

Using MSYS2
___________

* Download and Install https://www.msys2.org/#installation
* Check whether you have 32-bit or 64-bit python.


32-bit python
+++++++++++++

If you have 32-bit python, then open ``C:\msys64\mingw32.exe`` and type the following

.. code-block:: sh

    pacman -S mingw-w64-i686-mesa



It will install mesa and its dependencies. Then you can add ``C:\msys64\mingw32\bin``
to PATH before ``C:\Windows`` and moderngl should be working. Also, you should set
an environment variable called ``GLCONTEXT_WIN_LIBGL`` which contains the path to opengl32
dll from mesa. In this case it should be ``GLCONTEXT_WIN_LIBGL=C:\msys64\mingw32\bin\opengl32.dll``.


64-bit python
+++++++++++++

If you have 64-bit python, then open ``C:\msys64\mingw64.exe`` and type the following

.. code-block:: sh

    pacman -S mingw-w64-x86_64-mesa

It will install mesa and it's dependencies. Then you can add ``C:\msys64\mingw64\bin`` to PATH before
``C:\Windows`` and moderngl should be working. Also, you should set an environment variable called
``GLCONTEXT_WIN_LIBGL`` which contains the path to opengl32
dll from mesa. In this case it should be ``GLCONTEXT_WIN_LIBGL=C:\msys64\mingw64\bin\opengl32.dll``
