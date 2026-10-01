moderngl
========

.. py:module:: moderngl

.. code-block:: python

    import moderngl

    window = ...
    ctx = moderngl.create_context()
    # store a ref to ctx

The module object itself is responsible for creating a :py:class:`Context` object.

.. py:function:: moderngl.create_context(require: int = 330, standalone: bool = False) -> Context

    Create a ModernGL context by loading OpenGL functions from an existing OpenGL context.
    An OpenGL context must exist. Call this after a window is created or opt for the windowless standalone mode.
    Other backend specific settings are passed as keyword arguments.

    Context sharing is known to not work properly, please avoid using it.
    There is a parameter `share` for that to attempt to create a shared context.

    :param int require: OpenGL version code
    :param bool standalone: Headless flag

    Example::

        # Accept the current context version
        ctx = moderngl.create_context()

        # Require at least OpenGL 4.3
        ctx = moderngl.create_context(require=430)

        # Create a windowless context
        ctx = moderngl.create_context(standalone=True)

.. py:function:: moderngl.create_standalone_context(...) -> Context

    Deprecated, use :py:func:`moderngl.create_context()` with the standalone parameter set.

.. py:function:: moderngl.get_context() -> Context

    Returns the previously created context object.

    The context is stored in a global variable of the process, shared by all threads.
    If contexts are created on several threads, this is the last one created
    on any thread: keep the context objects instead of using this function.
    See :ref:`threading`.

    Example::

        # my_app.py

        from moderngl import create_context

        ctx = create_context(...)

        # my_renderer.py

        from moderngl import get_context

        class MyRenderer:
            def __init__(self):
                self.ctx = get_context()
                self.program = ...
                self.vao = ...

Context Flags
-------------

These were moved to :py:class:`Context`.

.. py:attribute:: moderngl.NOTHING

    See :py:attr:`Context.NOTHING`

.. py:attribute:: moderngl.BLEND

    See :py:attr:`Context.BLEND`

.. py:attribute:: moderngl.DEPTH_TEST

    See :py:attr:`Context.DEPTH_TEST`

.. py:attribute:: moderngl.CULL_FACE

    See :py:attr:`Context.CULL_FACE`

.. py:attribute:: moderngl.RASTERIZER_DISCARD

    See :py:attr:`Context.RASTERIZER_DISCARD`

.. py:attribute:: moderngl.PROGRAM_POINT_SIZE

    See :py:attr:`Context.PROGRAM_POINT_SIZE`

.. py:attribute:: moderngl.POINTS

    See :py:attr:`Context.POINTS`

.. py:attribute:: moderngl.LINES

    See :py:attr:`Context.LINES`

.. py:attribute:: moderngl.LINE_LOOP

    See :py:attr:`Context.LINE_LOOP`

.. py:attribute:: moderngl.LINE_STRIP

    See :py:attr:`Context.LINE_STRIP`

.. py:attribute:: moderngl.TRIANGLES

    See :py:attr:`Context.TRIANGLES`

.. py:attribute:: moderngl.TRIANGLE_STRIP

    See :py:attr:`Context.TRIANGLE_STRIP`

.. py:attribute:: moderngl.TRIANGLE_FAN

    See :py:attr:`Context.TRIANGLE_FAN`

.. py:attribute:: moderngl.LINES_ADJACENCY

    See :py:attr:`Context.LINES_ADJACENCY`

.. py:attribute:: moderngl.LINE_STRIP_ADJACENCY

    See :py:attr:`Context.LINE_STRIP_ADJACENCY`

.. py:attribute:: moderngl.TRIANGLES_ADJACENCY

    See :py:attr:`Context.TRIANGLES_ADJACENCY`

.. py:attribute:: moderngl.TRIANGLE_STRIP_ADJACENCY

    See :py:attr:`Context.TRIANGLE_STRIP_ADJACENCY`

.. py:attribute:: moderngl.PATCHES

    See :py:attr:`Context.PATCHES`

.. py:attribute:: moderngl.NEAREST

    See :py:attr:`Context.NEAREST`

.. py:attribute:: moderngl.LINEAR

    See :py:attr:`Context.LINEAR`

.. py:attribute:: moderngl.NEAREST_MIPMAP_NEAREST

    See :py:attr:`Context.NEAREST_MIPMAP_NEAREST`

.. py:attribute:: moderngl.LINEAR_MIPMAP_NEAREST

    See :py:attr:`Context.LINEAR_MIPMAP_NEAREST`

.. py:attribute:: moderngl.NEAREST_MIPMAP_LINEAR

    See :py:attr:`Context.NEAREST_MIPMAP_LINEAR`

.. py:attribute:: moderngl.LINEAR_MIPMAP_LINEAR

    See :py:attr:`Context.LINEAR_MIPMAP_LINEAR`

.. py:attribute:: moderngl.ZERO

    See :py:attr:`Context.ZERO`

.. py:attribute:: moderngl.ONE

    See :py:attr:`Context.ONE`

.. py:attribute:: moderngl.SRC_COLOR

    See :py:attr:`Context.SRC_COLOR`

.. py:attribute:: moderngl.ONE_MINUS_SRC_COLOR

    See :py:attr:`Context.ONE_MINUS_SRC_COLOR`

.. py:attribute:: moderngl.SRC_ALPHA

    See :py:attr:`Context.SRC_ALPHA`

.. py:attribute:: moderngl.ONE_MINUS_SRC_ALPHA

    See :py:attr:`Context.ONE_MINUS_SRC_ALPHA`

.. py:attribute:: moderngl.DST_ALPHA

    See :py:attr:`Context.DST_ALPHA`

.. py:attribute:: moderngl.ONE_MINUS_DST_ALPHA

    See :py:attr:`Context.ONE_MINUS_DST_ALPHA`

.. py:attribute:: moderngl.DST_COLOR

    See :py:attr:`Context.DST_COLOR`

.. py:attribute:: moderngl.ONE_MINUS_DST_COLOR

    See :py:attr:`Context.ONE_MINUS_DST_COLOR`

.. py:attribute:: moderngl.DEFAULT_BLENDING

    See :py:attr:`Context.DEFAULT_BLENDING`

.. py:attribute:: moderngl.ADDITIVE_BLENDING

    See :py:attr:`Context.ADDITIVE_BLENDING`

.. py:attribute:: moderngl.PREMULTIPLIED_ALPHA

    See :py:attr:`Context.PREMULTIPLIED_ALPHA`

.. py:attribute:: moderngl.FUNC_ADD

    See :py:attr:`Context.FUNC_ADD`

.. py:attribute:: moderngl.FUNC_SUBTRACT

    See :py:attr:`Context.FUNC_SUBTRACT`

.. py:attribute:: moderngl.FUNC_REVERSE_SUBTRACT

    See :py:attr:`Context.FUNC_REVERSE_SUBTRACT`

.. py:attribute:: moderngl.MIN

    See :py:attr:`Context.MIN`

.. py:attribute:: moderngl.MAX

    See :py:attr:`Context.MAX`

.. py:attribute:: moderngl.FIRST_VERTEX_CONVENTION

    See :py:attr:`Context.FIRST_VERTEX_CONVENTION`

.. py:attribute:: moderngl.LAST_VERTEX_CONVENTION

    See :py:attr:`Context.LAST_VERTEX_CONVENTION`
