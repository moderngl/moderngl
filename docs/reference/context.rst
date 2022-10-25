Context
=======

.. py:currentmodule:: moderngl

.. autoclass:: moderngl.Context

Create
------

.. autofunction:: moderngl.create_context
.. autofunction:: moderngl.create_standalone_context

ModernGL Objects
----------------

.. automethod:: Context.program
.. automethod:: Context.simple_vertex_array
.. automethod:: Context.vertex_array
.. automethod:: Context.buffer
.. automethod:: Context.texture
.. automethod:: Context.depth_texture
.. automethod:: Context.texture3d
.. automethod:: Context.texture_array
.. automethod:: Context.texture_cube
.. automethod:: Context.external_texture
.. automethod:: Context.simple_framebuffer
.. automethod:: Context.framebuffer
.. automethod:: Context.renderbuffer
.. automethod:: Context.depth_renderbuffer
.. automethod:: Context.scope
.. automethod:: Context.query
.. automethod:: Context.compute_shader
.. automethod:: Context.sampler
.. automethod:: Context.clear_samplers
.. automethod:: Context.release

Methods
-------

.. automethod:: Context.clear
.. automethod:: Context.enable_only
.. automethod:: Context.enable
.. automethod:: Context.disable
.. automethod:: Context.enable_direct
.. automethod:: Context.disable_direct
.. automethod:: Context.finish
.. automethod:: Context.copy_buffer
.. automethod:: Context.copy_framebuffer
.. automethod:: Context.detect_framebuffer
.. automethod:: Context.gc
.. automethod:: Context.__enter__
.. automethod:: Context.__exit__

Attributes
----------

.. autoattribute:: Context.gc_mode
.. autoattribute:: Context.objects
.. autoattribute:: Context.line_width
.. autoattribute:: Context.point_size
.. autoattribute:: Context.depth_func
.. autoattribute:: Context.blend_func
.. autoattribute:: Context.blend_equation
.. autoattribute:: Context.viewport
.. autoattribute:: Context.scissor
.. autoattribute:: Context.version_code
.. autoattribute:: Context.screen
.. autoattribute:: Context.fbo
.. autoattribute:: Context.front_face
.. autoattribute:: Context.cull_face
.. autoattribute:: Context.wireframe
.. autoattribute:: Context.max_samples
.. autoattribute:: Context.max_integer_samples
.. autoattribute:: Context.max_texture_units
.. autoattribute:: Context.default_texture_unit
.. autoattribute:: Context.max_anisotropy
.. autoattribute:: Context.multisample
.. autoattribute:: Context.patch_vertices
.. autoattribute:: Context.provoking_vertex
.. autoattribute:: Context.polygon_offset
.. autoattribute:: Context.error
.. autoattribute:: Context.extensions
.. autoattribute:: Context.info
.. autoattribute:: Context.mglo
.. autoattribute:: Context.extra

Context Flags
-------------

Context flags are used to enable or disable states in the context.
These are not the same enum values as in opengl, but are rather
bit flags so we can ``or`` them together setting multiple states
in a simple way.

These values are available in the ``Context`` object and in the
``moderngl`` module when you don't have access to the context.

.. code:: python

    import moderngl

    # From moderngl
    ctx.enable_only(moderngl.DEPTH_TEST | moderngl.CULL_FACE)

    # From context
    ctx.enable_only(ctx.DEPTH_TEST | ctx.CULL_FACE)

.. autoattribute:: Context.NOTHING
.. autoattribute:: Context.BLEND
.. autoattribute:: Context.DEPTH_TEST
.. autoattribute:: Context.CULL_FACE
.. autoattribute:: Context.RASTERIZER_DISCARD
.. autoattribute:: Context.PROGRAM_POINT_SIZE

Primitive Modes
---------------

.. autoattribute:: Context.POINTS
.. autoattribute:: Context.LINES
.. autoattribute:: Context.LINE_LOOP
.. autoattribute:: Context.LINE_STRIP
.. autoattribute:: Context.TRIANGLES
.. autoattribute:: Context.TRIANGLE_STRIP
.. autoattribute:: Context.TRIANGLE_FAN
.. autoattribute:: Context.LINES_ADJACENCY
.. autoattribute:: Context.LINE_STRIP_ADJACENCY
.. autoattribute:: Context.TRIANGLES_ADJACENCY
.. autoattribute:: Context.TRIANGLE_STRIP_ADJACENCY
.. autoattribute:: Context.PATCHES

Texture Filters
~~~~~~~~~~~~~~~

Also available in the :py:class:`Context` instance
including mode details.

.. autoattribute:: Context.NEAREST
.. autoattribute:: Context.LINEAR
.. autoattribute:: Context.NEAREST_MIPMAP_NEAREST
.. autoattribute:: Context.LINEAR_MIPMAP_NEAREST
.. autoattribute:: Context.NEAREST_MIPMAP_LINEAR
.. autoattribute:: Context.LINEAR_MIPMAP_LINEAR

Blend Functions
---------------

Blend functions are used with :py:attr:`Context.blend_func`
to control blending operations.

.. code::

    # Default value
    ctx.blend_func = ctx.SRC_ALPHA, ctx.ONE_MINUS_SRC_ALPHA

.. autoattribute:: Context.ZERO
.. autoattribute:: Context.ONE
.. autoattribute:: Context.SRC_COLOR
.. autoattribute:: Context.ONE_MINUS_SRC_COLOR
.. autoattribute:: Context.SRC_ALPHA
.. autoattribute:: Context.ONE_MINUS_SRC_ALPHA
.. autoattribute:: Context.DST_ALPHA
.. autoattribute:: Context.ONE_MINUS_DST_ALPHA
.. autoattribute:: Context.DST_COLOR
.. autoattribute:: Context.ONE_MINUS_DST_COLOR

Blend Function Shortcuts
------------------------

.. autoattribute:: Context.DEFAULT_BLENDING
.. autoattribute:: Context.ADDITIVE_BLENDING
.. autoattribute:: Context.PREMULTIPLIED_ALPHA

Blend Equations
---------------

Used with :py:attr:`Context.blend_equation`.

.. autoattribute:: Context.FUNC_ADD
.. autoattribute:: Context.FUNC_SUBTRACT
.. autoattribute:: Context.FUNC_REVERSE_SUBTRACT
.. autoattribute:: Context.MIN
.. autoattribute:: Context.MAX

Other Enums
-----------

.. autoattribute:: Context.FIRST_VERTEX_CONVENTION
.. autoattribute:: Context.LAST_VERTEX_CONVENTION

Examples
--------

ModernGL Context
~~~~~~~~~~~~~~~~

.. code-block:: python

    import moderngl
    # create a window
    ctx = moderngl.create_context()
    print(ctx.version_code)

Standalone ModernGL Context
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    import moderngl
    ctx = moderngl.create_standalone_context()
    print(ctx.version_code)

ContextManager
~~~~~~~~~~~~~~

.. rubric:: context_manager.py

.. literalinclude:: ../../examples/context_manager.py
    :linenos:

.. rubric:: example.py

.. code-block:: python
    :linenos:

    from context_manager import ContextManager

    ctx = ContextManager.get_default_context()
    print(ctx.version_code)

.. toctree::
    :maxdepth: 2
