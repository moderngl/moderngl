.. py:currentmodule:: moderngl

Creating a Context
==================

Before we can do anything with ModernGL we need a :py:class:`Context`.
The :py:class:`Context` object makes us able to create OpenGL resources.
ModernGL can only create headless contexts (no window), but it can also detect
and use contexts from a large range of window libraries. The `moderngl-window`_
library is a good start or reference for rendering to a window.

Most of the example code here assumes a ``ctx`` variable exists with a
headless context::

    # standalone=True makes a headless context
    ctx = moderngl.create_context(standalone=True)

Detecting an active context created by a window library is simply::

    ctx = moderngl.create_context()

More details about context creation can be found in the :ref:`context`
section.

.. _moderngl-window: https://github.com/moderngl/moderngl-window
