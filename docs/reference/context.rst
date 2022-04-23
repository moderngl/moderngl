Context
=======

.. py:currentmodule:: moderngl

.. autoclass:: moderngl.Context

Create
------

.. autofunction:: moderngl.create_context(require=None) -> Context
.. autofunction:: moderngl.create_standalone_context(require=None) -> Context

ModernGL Objects
----------------

.. automethod:: Context.program(vertex_shader: str, fragment_shader: Optional[str] = None, geometry_shader: Optional[str] = None, tess_control_shader: Optional[str] = None, tess_evaluation_shader: Optional[str] = None, varyings: Tuple[str, ...] = (), fragment_outputs: Optional[Dict[str, int]] = None, varyings_capture_mode: str = 'interleaved') -> Program
.. automethod:: Context.simple_vertex_array(program: Program, buffer: Buffer, *attributes: Union[List[str], Tuple[str, ...]], index_buffer: Optional[Buffer] = None, index_element_size: int = 4, mode: Optional[int] = None) -> VertexArray
.. automethod:: Context.vertex_array(*args, **kwargs) -> VertexArray
.. automethod:: Context.buffer(data: Optional[Any] = None, reserve: int = 0, dynamic: bool = False) -> Buffer
.. automethod:: Context.texture(size: Tuple[int, int], components: int, data: Optional[Any] = None, samples: int = 0, alignment: int = 1, dtype: str = 'f1', internal_format: int = None) -> Texture
.. automethod:: Context.depth_texture(size: Tuple[int, int], data: Optional[Any] = None, samples: int = 0, alignment: int = 4) -> Texture
.. automethod:: Context.texture3d(size: Tuple[int, int, int], components: int, data: Optional[Any] = None, alignment: int = 1, dtype: str = 'f1') -> Texture3D
.. automethod:: Context.texture_array(size: Tuple[int, int, int], components: int, data: Optional[Any] = None, alignment: int = 1, dtype: str = 'f1') -> TextureArray
.. automethod:: Context.texture_cube(size: Tuple[int, int], components: int, data: Optional[Any] = None, alignment: int = 1, dtype: str = 'f1') -> TextureCube
.. automethod:: Context.simple_framebuffer(size: Tuple[int, int], components: int = 4, samples: int = 0, dtype: str = 'f1') -> Framebuffer
.. automethod:: Context.framebuffer(color_attachments: Any = (), depth_attachment: Union[Texture, Renderbuffer, NoneType] = None) -> Framebuffer
.. automethod:: Context.renderbuffer(size: Tuple[int, int], components: int = 4, samples: int = 0, dtype: str = 'f1') -> Renderbuffer
.. automethod:: Context.depth_renderbuffer(size: Tuple[int, int], samples: int = 0) -> Renderbuffer
.. automethod:: Context.scope(framebuffer: Optional[Framebuffer] = None, enable_only: Optional[int] = None, textures: Tuple[Tuple[Texture, int], ...] = (), uniform_buffers: Tuple[Tuple[Buffer, int], ...] = (), storage_buffers: Tuple[Tuple[Buffer, int], ...] = (), samplers: Tuple[Tuple[Sampler, int], ...] = (), enable: Optional[int] = None) -> Scope
.. automethod:: Context.query(samples: bool = False, any_samples: bool = False, time: bool = False, primitives: bool = False) -> Query
.. automethod:: Context.compute_shader(source: str) -> ComputeShader
.. automethod:: Context.sampler(repeat_x: bool = True, repeat_y: bool = True, repeat_z: bool = True, filter: Tuple[int, int] = None, anisotropy: float = 1.0, compare_func: str = '?', border_color: Tuple[float, float, float, float] = None, min_lod: float = -1000.0, max_lod: float = 1000.0, texture: Optional[Texture] = None) -> Sampler
.. automethod:: Context.clear_samplers(start: int = 0, end: int = -1)
.. automethod:: Context.release()


Methods
-------

.. automethod:: Context.clear(red: float = 0.0, green: float = 0.0, blue: float = 0.0, alpha: float = 0.0, depth: float = 1.0, viewport: Union[Tuple[int, int], Tuple[int, int, int, int], NoneType] = None, color: Optional[Tuple[float, float, float, float]] = None)
.. automethod:: Context.enable_only(flags: int)
.. automethod:: Context.enable(flags: int)
.. automethod:: Context.disable(flags: int)
.. automethod:: Context.enable_direct(enum: int)
.. automethod:: Context.disable_direct(enum: int)
.. automethod:: Context.finish()
.. automethod:: Context.copy_buffer(dst: Buffer, src: Buffer, size: int = -1, read_offset: int = 0, write_offset: int = 0)
.. automethod:: Context.copy_framebuffer(dst: Union[Framebuffer, Texture], src: Framebuffer)
.. automethod:: Context.detect_framebuffer(glo: Optional[int] = None) -> Framebuffer
.. automethod:: Context.gc() -> int
.. automethod:: Context.__enter__()
.. automethod:: Context.__exit__(exc_type, exc_val, exc_tb)

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
