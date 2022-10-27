.. py:currentmodule:: moderngl


The Lifecycle of a ModernGL Object
==================================

From moderngl 5.7 we support three different garbage collection modes.
This should be configured using the :py:attr:`Context.gc_mode` attribute
preferably right after the context is created.

The current supported modes are:

* ``None``: (default) No garbage collection is performed. Objects needs to
  to be manually released like in previous versions of moderngl.
* ``"context_gc"``: Dead objects are collected in :py:attr:`Context.objects`.
  These can periodically be released using :py:meth:`Context.gc`.
* ``"auto"``: Dead objects are destroyed automatically like we would
  expect in python.

It's important to realize here that garbage collection is not about
the python objects itself, but the underlying OpenGL objects. ModernGL
operates in many different environments were garbage collection can be
a challenge. This depends on factors like who is controlling the existence
of the OpenGL context and challenges around threading in python.

Standalone / Headless Context
-----------------------------

In this instance we control when the context is created and destroyed.
Using ``"auto"`` garbage collection is perfectly reasonable in this
situation.

Context Detection
-----------------

When detecting an existing context from some window library we have no
direct control over the existence of the context. Using ``"auto"`` mode
is dangerous can can cause crashes especially on application exit.
The window and context is destroyed and closed, then moderngl will
try to destroy resources in a context that no longer exists.
Use ``"context_gc"`` mode to avoid this.

It can be possible to switch the ``gc_mode`` to ``None`` when
the window is closed. This can still be a problem if you have
race conditions due to resources being created in the render loop.

The Threading Issue
-------------------

When using threads in python the garbage collector can run in any thread.
This is a problem for OpenGL because only the main thread is allowed
to interact with the context. When using threads in your application
you should be using ``"context_gc"`` mode and periodically call ``Context.gc``
for example during every frame swap.

Manually Releasing Objects
--------------------------

Objects in moderngl don't automatically release the OpenGL resources when
``gc_mode=None`` is used.
Each type has a ``release()`` method that needs to be called to properly clean
up everything::

    # Create a texture
    texture = ctx.texture((10, 10), 4)

    # Properly release the opengl resources
    texture.release()

Detecting Released Objects
--------------------------

If you for some reason need to detect if a resource was released it can be done
by checking the type of the internal moderngl object (``.mglo`` property)::

    >> import moderngl
    >> ctx = moderngl.create_standalone_context()
    >> buffer = ctx.buffer(reserve=1024)
    >> type(buffer.mglo)
    <class 'mgl.Buffer'>
    >> buffer.release()
    >> type(buffer.mglo)
    <class '_moderngl.InvalidObject'>
    >> type(buffer.mglo) == moderngl.mgl.InvalidObject
    True
