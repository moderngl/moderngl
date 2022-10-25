.. py:module:: moderngl
.. py:currentmodule:: moderngl

moderngl
========

Attributes
----------

Attributes available in the root ``moderngl`` module.
Some may be listed in their original sub-module,
but they are imported during initialization.

Context Flags
~~~~~~~~~~~~~

Also available in the :py:class:`Context` instance
including mode details.

.. autodata:: moderngl.context.NOTHING
.. autodata:: moderngl.context.BLEND
.. autodata:: moderngl.context.DEPTH_TEST
.. autodata:: moderngl.context.CULL_FACE
.. autodata:: moderngl.context.RASTERIZER_DISCARD
.. autodata:: moderngl.context.PROGRAM_POINT_SIZE

Primitive Modes
~~~~~~~~~~~~~~~

Also available in the :py:class:`Context` instance
including mode details.

.. autodata:: moderngl.vertex_array.POINTS
.. autodata:: moderngl.vertex_array.LINES
.. autodata:: moderngl.vertex_array.LINE_LOOP
.. autodata:: moderngl.vertex_array.LINE_STRIP
.. autodata:: moderngl.vertex_array.TRIANGLES
.. autodata:: moderngl.vertex_array.TRIANGLE_STRIP
.. autodata:: moderngl.vertex_array.TRIANGLE_FAN
.. autodata:: moderngl.vertex_array.LINES_ADJACENCY
.. autodata:: moderngl.vertex_array.LINE_STRIP_ADJACENCY
.. autodata:: moderngl.vertex_array.TRIANGLES_ADJACENCY
.. autodata:: moderngl.vertex_array.TRIANGLE_STRIP_ADJACENCY
.. autodata:: moderngl.vertex_array.PATCHES

Texture Filters
~~~~~~~~~~~~~~~

Also available in the :py:class:`Context` instance
including mode details.

.. autodata:: moderngl.texture.NEAREST
.. autodata:: moderngl.texture.LINEAR
.. autodata:: moderngl.texture.NEAREST_MIPMAP_NEAREST
.. autodata:: moderngl.texture.LINEAR_MIPMAP_NEAREST
.. autodata:: moderngl.texture.NEAREST_MIPMAP_LINEAR
.. autodata:: moderngl.texture.LINEAR_MIPMAP_LINEAR

Blend Functions
~~~~~~~~~~~~~~~

Also available in the :py:class:`Context` instance
including mode details.

.. autodata:: moderngl.context.ZERO
.. autodata:: moderngl.context.ONE
.. autodata:: moderngl.context.SRC_COLOR
.. autodata:: moderngl.context.ONE_MINUS_SRC_COLOR
.. autodata:: moderngl.context.SRC_ALPHA
.. autodata:: moderngl.context.ONE_MINUS_SRC_ALPHA
.. autodata:: moderngl.context.DST_ALPHA
.. autodata:: moderngl.context.ONE_MINUS_DST_ALPHA
.. autodata:: moderngl.context.DST_COLOR
.. autodata:: moderngl.context.ONE_MINUS_DST_COLOR

Shortcuts
^^^^^^^^^

.. autodata:: moderngl.context.DEFAULT_BLENDING
.. autodata:: moderngl.context.ADDITIVE_BLENDING
.. autodata:: moderngl.context.PREMULTIPLIED_ALPHA

Blend Equations
~~~~~~~~~~~~~~~

Also available in the :py:class:`Context` instance
including mode details.

.. autodata:: moderngl.context.FUNC_ADD
.. autodata:: moderngl.context.FUNC_SUBTRACT
.. autodata:: moderngl.context.FUNC_REVERSE_SUBTRACT
.. autodata:: moderngl.context.MIN
.. autodata:: moderngl.context.MAX

Provoking Vertex
~~~~~~~~~~~~~~~~

Also available in the :py:class:`Context` instance
including mode details.

.. autodata:: moderngl.context.FIRST_VERTEX_CONVENTION
.. autodata:: moderngl.context.LAST_VERTEX_CONVENTION

Functions
---------

Also see :py:class:`Context`.

.. autofunction:: create_context
    :noindex:
.. autofunction:: create_standalone_context
    :noindex:
.. autofunction:: detect_format
    :noindex:
