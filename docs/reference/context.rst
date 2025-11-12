Context
=======

.. py:class:: Context

    Returned by :py:meth:`moderngl.create_context`

    Class exposing OpenGL features.

    ModernGL objects can be created from this class.

Objects
-------

.. py:method:: Context.program(vertex_shader: str, fragment_shader: str, geometry_shader: str, tess_control_shader: str, tess_evaluation_shader: str, varyings: Tuple[str, ...], fragment_outputs: Dict[str, int], varyings_capture_mode: str = 'interleaved') -> Program

    Create a :py:class:`Program` object.

    The ``varyings`` are only used when a transform program is created
    to specify the names of the output varyings to capture in the output buffer.

    ``fragment_outputs`` can be used to programmatically map named fragment
    shader outputs to a framebuffer attachment numbers. This can also be done
    by using ``layout(location=N)`` in the fragment shader.

    :param str vertex_shader: The vertex shader source.
    :param str fragment_shader: The fragment shader source.
    :param str geometry_shader: The geometry shader source.
    :param str tess_control_shader: The tessellation control shader source.
    :param str tess_evaluation_shader: The tessellation evaluation shader source.
    :param list varyings: A list of varyings.
    :param dict fragment_outputs: A dictionary of fragment outputs.

.. py:method:: Context.buffer(data = None, reserve: int = 0, dynamic: bool = False) -> Buffer

    Returns a new :py:class:`Buffer` object.

    The `data` can be anything supporting the buffer interface.

    The `data` and `reserve` parameters are mutually exclusive.

    :param bytes data: Content of the new buffer.
    :param int reserve: The number of bytes to reserve.
    :param bool dynamic: Treat buffer as dynamic.

.. py:method:: Context.vertex_array(program: Program, content: list, index_buffer: Buffer = None, index_element_size: int = 4, mode: int = ...) -> VertexArray

    Returns a new :py:class:`VertexArray` object.

    A VertexArray describes how buffers are read by a shader program.
    The content is a list of tuples containing a buffer, a format string and any number of attribute names.
    Attribute names are defined by the user in the Vertex Shader program stage.

    The default `mode` is :py:attr:`~Context.TRIANGLES`.

    :param Program program: The program used when rendering
    :param list content: A list of (buffer, format, attributes). See :ref:`buffer-format-label`.
    :param Buffer index_buffer: An index buffer (optional)
    :param int index_element_size: byte size of each index element, 1, 2 or 4.
    :param bool skip_errors: Ignore errors during creation
    :param int mode: The default draw mode (for example: ``TRIANGLES``)

    Examples::

        # Empty vertext array (no attribute input)
        vao = ctx.vertex_array(program)

        # Multiple buffers
        vao = ctx.vertex_array(program, [
            (buffer1, '3f', 'in_position'),
            (buffer2, '3f', 'in_normal'),
        ])
        vao = ctx.vertex_array(
            program,
            [
                (buffer1, '3f', 'in_position'),
                (buffer2, '3f', 'in_normal'),
            ],
            index_buffer=ibo,
            index_element_size=2,  # 16 bit / 'u2' index buffer
        )

    Backward Compatible Version::

        # Simple version with a single buffer
        vao = ctx.vertex_array(program, buffer, 'in_position', 'in_normal')
        vao = ctx.vertex_array(program, buffer, 'in_position', 'in_normal', index_buffer=ibo)

.. py:method:: Context.simple_vertex_array(...)

    Deprecated, use :py:meth:`Context.vertex_array` instead.

.. py:method:: Context.texture(size: Tuple[int, int], components: int, data: Any = None, samples: int = 0, alignment: int = 1, dtype: str = 'f1') -> Texture

    Returns a new :py:class:`Texture` object.

    A Texture is a 2D image that can be used for sampler2D uniforms or as render targets if framebuffers.

    :param tuple size: The width and height of the texture.
    :param int components: The number of components 1, 2, 3 or 4.
    :param bytes data: Content of the texture.
    :param int samples: The number of samples. Value 0 means no multisample format.
    :param int alignment: The byte alignment 1, 2, 4 or 8.
    :param str dtype: Data type.
    :param int internal_format: Override the internalformat of the texture (IF needed)

    Example::

        from PIL import Image

        img = Image.open(...).convert('RGBA')
        texture = ctx.texture(img.size, components=4, data=img.tobytes())

        # float texture
        texture = ctx.texture((64, 64), components=..., dtype='f4')

        # integer texture
        texture = ctx.texture((64, 64), components=..., dtype='i4')

    .. Note:: Do not play with ``internal_format`` unless you know exactly
                    you are doing. This is an override to support sRGB and
                    compressed textures if needed.

.. py:method:: Context.framebuffer(color_attachments: List[Texture], depth_attachment: Texture = None) -> Framebuffer

    Returns a new :py:class:`Framebuffer` object.

    A Framebuffer is a collection of images that can be used as render targets.
    The images of the Framebuffer object can be either Textures or Renderbuffers.

    :param list color_attachments: A list of :py:class:`Texture` or :py:class:`Renderbuffer` objects.
    :param Texture depth_attachment: The depth attachment.

.. py:method:: Context.sampler(repeat_x: bool, repeat_y: bool, repeat_z: bool, filter: tuple, anisotropy: float, compare_func: str, border_color: tuple, min_lod: float, max_lod: float, texture: Texture) -> Sampler

    Returns a new :py:class:`Sampler` object.

    Samplers bind Textures to uniform samplers within a Program object.
    Binding a Sampler object also binds the texture object attached to it.

    :param bool repeat_x: Repeat texture on x
    :param bool repeat_y: Repeat texture on y
    :param bool repeat_z: Repeat texture on z
    :param tuple filter: The min and max filter
    :param float anisotropy: Number of samples for anisotropic filtering. Any value greater than 1.0 counts as a use of anisotropic filtering
    :param str compare_func: Compare function for depth textures
    :param tuple border_color: The (r, g, b, a) color for the texture border. When this value is set the ``repeat_`` values are overridden setting the texture wrap to return the border color when outside ``[0, 1]`` range.
    :param float min_lod: Minimum level-of-detail parameter (Default ``-1000.0``). This floating-point value limits the selection of highest resolution mipmap (lowest mipmap level)
    :param float max_lod: Minimum level-of-detail parameter (Default ``1000.0``). This floating-point value limits the selection of the lowest resolution mipmap (highest mipmap level)
    :param Texture texture: The texture for this sampler

.. py:method:: Context.depth_texture(size: Tuple[int, int], data: Any = None, samples: int = 0, alignment: int = 4) -> Texture

    Returns a new :py:class:`Texture` object.

    A depth texture can be used for sampler2D and sampler2DShadow uniforms and as a depth attachment for framebuffers.

    :param tuple size: The width and height of the texture.
    :param bytes data: Content of the texture.
    :param int samples: The number of samples. Value 0 means no multisample format.
    :param int alignment: The byte alignment 1, 2, 4 or 8.

.. py:method:: Context.texture3d(size: Tuple[int, int, int], components: int, data: Any = None, alignment: int = 1, dtype: str = 'f1') -> Texture3D

    Returns a new :py:class:`Texture3D` object.

    :param tuple size: The width, height and depth of the texture.
    :param int components: The number of components 1, 2, 3 or 4.
    :param bytes data: Content of the texture.
    :param int alignment: The byte alignment 1, 2, 4 or 8.
    :param str dtype: Data type.

.. py:method:: Context.texture_array(size: Tuple[int, int, int], components: int, data: Any = None, *, alignment: int = 1, dtype: str = 'f1') -> TextureArray

    Returns a new :py:class:`TextureArray` object.

    :param tuple size: The ``(width, height, layers)`` of the texture.
    :param int components: The number of components 1, 2, 3 or 4.
    :param bytes data: Content of the texture. The size must be ``(width, height * layers)`` so each layer is stacked vertically.
    :param int alignment: The byte alignment 1, 2, 4 or 8.
    :param str dtype: Data type.

.. py:method:: Context.texture_cube(size: Tuple[int, int], components: int, data: Any = None, alignment: int = 1, dtype: str = 'f1') -> TextureCube

    Returns a new :py:class:`TextureCube` object.

    Note that the width and height of the cubemap must be the same.

    :param tuple size: The width, height of the texture. Each side of the cube will have this size.
    :param int components: The number of components 1, 2, 3 or 4.
    :param bytes data: Content of the texture. The data should be have the following ordering: positive_x, negative_x, positive_y, negative_y, positive_z, negative_z
    :param int alignment: The byte alignment 1, 2, 4 or 8.
    :param str dtype: Data type.
    :param int internal_format: Override the internalformat of the texture (IF needed)

.. py:method:: Context.depth_texture_cube(size: Tuple[int, int], data: Optional[Any] = None, alignment: int = 4) -> TextureCube

    Returns a new :py:class:`TextureCube` object.

    :param tuple size: The width and height of the texture.
    :param bytes data: Content of the texture.
    :param int alignment: The byte alignment 1, 2, 4 or 8.

.. py:method:: Context.simple_framebuffer(...)

    Deprecated, use :py:meth:`Context.framebuffer` instead.

.. py:method:: Context.renderbuffer(size: Tuple[int, int], components: int = 4, samples: int = 0, dtype: str = 'f1') -> Renderbuffer

    Returns a new :py:class:`Renderbuffer` object.

    Similar to textures, renderbuffers can be attached to framebuffers as render targets, but they cannot be sampled as textures.

    :param tuple size: The width and height of the renderbuffer.
    :param int components: The number of components 1, 2, 3 or 4.
    :param int samples: The number of samples. Value 0 means no multisample format.
    :param str dtype: Data type.

.. py:method:: Context.depth_renderbuffer(size: Tuple[int, int], samples: int = 0) -> Renderbuffer

    Returns a new :py:class:`Renderbuffer` object.

    :param tuple size: The width and height of the renderbuffer.
    :param int samples: The number of samples. Value 0 means no multisample format.

.. py:method:: Context.scope(framebuffer, enable_only, textures, uniform_buffers, storage_buffers, samplers)

    Returns a new :py:class:`Scope` object.

    Scope objects can be attached to VertexArray objects to minimize the possibility of rendering within the wrong scope.
    VertexArrays with an attached scope always have the scope settings at render time.

    :param Framebuffer framebuffer: The framebuffer to use when entering.
    :param int enable_only: The enable_only flags to set when entering.
    :param tuple textures: List of (texture, binding) tuples.
    :param tuple uniform_buffers: Tuple of (buffer, binding) tuples.
    :param tuple storage_buffers: Tuple of (buffer, binding) tuples.
    :param tuple samplers: Tuple of sampler bindings

.. py:method:: Context.query(samples: bool, any_samples: bool, time: bool, primitives: bool) -> Query

    Returns a new :py:class:`Query` object.

    :param bool samples: Query ``GL_SAMPLES_PASSED`` or not.
    :param bool any_samples: Query ``GL_ANY_SAMPLES_PASSED`` or not.
    :param bool time: Query ``GL_TIME_ELAPSED`` or not.
    :param bool primitives: Query ``GL_PRIMITIVES_GENERATED`` or not.

.. py:method:: Context.compute_shader(...)

    A :py:class:`ComputeShader` is a Shader Stage that is used entirely \
    for computing arbitrary information. While it can do rendering, it \
    is generally used for tasks not directly related to drawing.

    :param str source: The source of the compute shader.

External Objects
----------------

External objects are only useful for interoperability with other libraries.

.. py:method:: Context.external_buffer(glo: int, size: int, dynamic: bool) -> Buffer

    TBD

.. py:method:: Context.external_texture(glo: int, size: Tuple[int, int], components: int, samples: int, dtype: str) -> Texture

    Returns a new :py:class:`Texture` object from an existing OpenGL texture object.

    The content of the texture is referenced and it is not copied.

    :param int glo: External OpenGL texture object.
    :param tuple size: The width and height of the texture.
    :param int components: The number of components 1, 2, 3 or 4.
    :param int samples: The number of samples. Value 0 means no multisample format.
    :param str dtype: Data type.

Methods
-------

.. py:method:: Context.clear

    Clear the bound framebuffer.

    If a `viewport` passed in, a scissor test will be used to clear the given viewport.
    This viewport take prescense over the framebuffers :py:attr:`~moderngl.Framebuffer.scissor`.
    Clearing can still be done with scissor if no viewport is passed in.

    This method also respects the
    :py:attr:`~moderngl.Framebuffer.color_mask` and
    :py:attr:`~moderngl.Framebuffer.depth_mask`. It can for example be used to only clear
    the depth or color buffer or specific components in the color buffer.

    If the `viewport` is a 2-tuple it will clear the
    ``(0, 0, width, height)`` where ``(width, height)`` is the 2-tuple.

    If the `viewport` is a 4-tuple it will clear the given viewport.

    Args:
        red (float): color component.
        green (float): color component.
        blue (float): color component.
        alpha (float): alpha component.
        depth (float): depth value.

    Keyword Args:
        viewport (tuple): The viewport.
        color (tuple): Optional rgba color tuple

.. py:method:: Context.enable_only

    Clears all existing flags applying new ones.

    Note that the enum values defined in moderngl
    are not the same as the ones in opengl.
    These are defined as bit flags so we can logical
    `or` them together.

    Available flags:

    - :py:data:`moderngl.NOTHING`
    - :py:data:`moderngl.BLEND`
    - :py:data:`moderngl.DEPTH_TEST`
    - :py:data:`moderngl.CULL_FACE`
    - :py:data:`moderngl.RASTERIZER_DISCARD`
    - :py:data:`moderngl.PROGRAM_POINT_SIZE`

    Examples::

        # Disable all flags
        ctx.enable_only(moderngl.NOTHING)

        # Ensure only depth testing and face culling is enabled
        ctx.enable_only(moderngl.DEPTH_TEST | moderngl.CULL_FACE)

    Args:
        flags (EnableFlag): The flags to enable

.. py:method:: Context.enable

    Enable flags.

    Note that the enum values defined in moderngl
    are not the same as the ones in opengl.
    These are defined as bit flags so we can logical
    `or` them together.

    For valid flags, please see :py:meth:`enable_only`.

    Examples::

        # Enable a single flag
        ctx.enable(moderngl.DEPTH_TEST)

        # Enable multiple flags
        ctx.enable(moderngl.DEPTH_TEST | moderngl.CULL_FACE | moderngl.BLEND)

    Args:
        flag (int): The flags to enable.

.. py:method:: Context.disable

    Disable flags.

    For valid flags, please see :py:meth:`enable_only`.

    Examples::

        # Only disable depth testing
        ctx.disable(moderngl.DEPTH_TEST)

        # Disable depth testing and face culling
        ctx.disable(moderngl.DEPTH_TEST | moderngl.CULL_FACE)

    Args:
        flag (int): The flags to disable.

.. py:method:: Context.enable_direct

    Gives direct access to ``glEnable`` so unsupported capabilities in ModernGL can be enabled.

    Do not use this to set already supported context flags.

    Example::

        # Enum value from the opengl registry
        GL_CONSERVATIVE_RASTERIZATION_NV = 0x9346
        ctx.enable_direct(GL_CONSERVATIVE_RASTERIZATION_NV)

.. py:method:: Context.disable_direct

    Gives direct access to ``glDisable`` so unsupported capabilities in ModernGL can be disabled.

    Do not use this to set already supported context flags.

    Example::

        # Enum value from the opengl registry
        GL_CONSERVATIVE_RASTERIZATION_NV = 0x9346
        ctx.disable_direct(GL_CONSERVATIVE_RASTERIZATION_NV)

.. py:method:: Context.finish

    Wait for all drawing commands to finish.

.. py:method:: Context.clear_samplers

    Unbinds samplers from texture units.

    Sampler bindings do clear automatically between every frame,
    but lingering samplers can still be a source of weird bugs during
    the frame rendering. This methods provides a fairly brute force
    and efficient way to ensure texture units are clear.

    :param int start: The texture unit index to start the clearing samplers
    :param int stop: The texture unit index to stop clearing samplers

    Example::

        # Clear texture unit 0, 1, 2, 3, 4
        ctx.clear_samplers(start=0, end=5)

        # Clear texture unit 4, 5, 6, 7
        ctx.clear_samplers(start=4, end=8)

.. py:method:: Context.copy_buffer

    Copy buffer content.

    Args:
        dst (Buffer): The destination buffer.
        src (Buffer): The source buffer.
        size (int): The number of bytes to copy.

    Keyword Args:
        read_offset (int): The read offset.
        write_offset (int): The write offset.

.. py:method:: Context.copy_framebuffer

    Copy framebuffer content.

    Use this method to:

        - blit framebuffers.
        - copy framebuffer content into a texture.
        - downsample framebuffers. (it will allow to read the framebuffer's content)
        - downsample a framebuffer directly to a texture.

    Args:
        dst (Framebuffer or Texture): Destination framebuffer or texture.
        src (Framebuffer): Source framebuffer.

.. py:method:: Context.detect_framebuffer

    Detect a framebuffer.

    This is already done when creating a context,
    but if the underlying window library for some changes the default framebuffer
    during the lifetime of the application this might be necessary.

    Args:
        glo (int): Framebuffer object.

.. py:method:: Context.memory_barrier

    Applying a memory barrier.

    The memory barrier is needed in particular to correctly change buffers or textures
    between each shader. If the same buffer is changed in two shaders,
    it can cause an effect like 'depth fighting' on a buffer or texture.

    The method should be used between :py:class:`Program` -s, between :py:class:`ComputeShader` -s,
    and between :py:class:`Program` -s and :py:class:`ComputeShader` -s.

    Keyword Args:
        barriers (int): Affected barriers, default moderngl.ALL_BARRIER_BITS.
        by_region (bool): Memory barrier mode by region. More read on https://registry.khronos.org/OpenGL-Refpages/gl4/html/glMemoryBarrier.xhtml

.. py:method:: Context.gc() -> int

    Deletes OpenGL objects.
    Returns the number of objects deleted.

    This method must be called to garbage collect
    OpenGL resources when ``gc_mode`` is ``'context_gc'```.

    Calling this method with any other ``gc_mode`` configuration
    has no effect and is perfectly safe.

.. py:method:: Context.release

Attributes
----------

.. py:attribute:: Context.gc_mode
    :type: str

    The garbage collection mode.

    The default mode is ``None`` meaning no automatic
    garbage collection is done. Other modes are ``auto``
    and ``context_gc``. Please see documentation for
    the appropriate configuration.

    Examples::

        # Disable automatic garbage collection.
        # Each objects needs to be explicitly released.
        ctx.gc_mode = None

        # Collect all dead objects in the context and
        # release them by calling Context.gc()
        ctx.gc_mode = 'context_gc'
        ctx.gc()  # Deletes the collected objects

        # Enable automatic garbage collection like
        # we normally expect in python.
        ctx.gc_mode = 'auto'

.. py:attribute:: Context.objects
    :type: deque

    Moderngl objects scheduled for deletion.

    These are deleted when calling :py:meth:`Context.gc`.

.. py:attribute:: Context.line_width
    :type: float

    Set the default line width.

    .. Warning:: A line width other than 1.0 is not guaranteed to work
                    across different OpenGL implementations. For wide
                    lines you should be using geometry shaders.

.. py:attribute:: Context.point_size
    :type: float

    Set/get the point size.

    Point size changes the pixel size of rendered points. The min and max values
    are limited by ``POINT_SIZE_RANGE``.
    This value usually at least ``(1, 100)``, but this depends on the drivers/vendors.

    If variable point size is needed you can enable ``PROGRAM_POINT_SIZE``
    and write to ``gl_PointSize`` in the vertex or geometry shader.

    .. Note::

        Using a geometry shader to create triangle strips from points is often a safer
        way to render large points since you don't have have any size restrictions.

.. py:attribute:: Context.depth_func
    :type: str

    Set the default depth func.

    Example::

        ctx.depth_func = '<='  # GL_LEQUAL
        ctx.depth_func = '<'   # GL_LESS
        ctx.depth_func = '>='  # GL_GEQUAL
        ctx.depth_func = '>'   # GL_GREATER
        ctx.depth_func = '=='  # GL_EQUAL
        ctx.depth_func = '!='  # GL_NOTEQUAL
        ctx.depth_func = '0'   # GL_NEVER
        ctx.depth_func = '1'   # GL_ALWAYS

.. py:attribute:: Context.depth_clamp_range
    :type: Tuple[float, float]

    Setting up depth clamp range (write only, by default ``None``).

    ``ctx.depth_clamp_range`` offers uniform use of GL_DEPTH_CLAMP and glDepthRange.

    ``GL_DEPTH_CLAMP`` is needed to disable clipping of fragments outside
    near limit of projection matrix.
    For example, this will allow you to draw between 0 and 1 in the Z (depth) coordinate,
    even if ``near`` is set to 0.5 in the projection matrix.

    .. note::

        All fragments outside the ``near`` of the projection matrix will have a depth of ``near``.

    See https://www.khronos.org/opengl/wiki/Vertex_Post-Processing#Depth_clamping for more info.

    ``glDepthRange(nearVal, farVal)`` is needed to specify mapping of depth values from normalized device coordinates to window coordinates.
    See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDepthRange.xhtml for more info.

    Example::

        # For glDisable(GL_DEPTH_CLAMP) and glDepthRange(0, 1)
        ctx.depth_clamp_range = None

        # For glEnable(GL_DEPTH_CLAMP) and glDepthRange(near, far)
        ctx.depth_clamp_range = (near, far)

.. py:attribute:: Context.blend_func
    :type: tuple

    Set the blend func (write only).

    Blend func can be set for rgb and alpha separately if needed.

    Supported blend functions are::

        moderngl.ZERO
        moderngl.ONE
        moderngl.SRC_COLOR
        moderngl.ONE_MINUS_SRC_COLOR
        moderngl.DST_COLOR
        moderngl.ONE_MINUS_DST_COLOR
        moderngl.SRC_ALPHA
        moderngl.ONE_MINUS_SRC_ALPHA
        moderngl.DST_ALPHA
        moderngl.ONE_MINUS_DST_ALPHA

        # Shortcuts
        moderngl.DEFAULT_BLENDING     # (SRC_ALPHA, ONE_MINUS_SRC_ALPHA)
        moderngl.ADDITIVE_BLENDING    # (ONE, ONE)
        moderngl.PREMULTIPLIED_ALPHA  # (SRC_ALPHA, ONE)

    Example::

        # For both rgb and alpha
        ctx.blend_func = moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA

        # Separate for rgb and alpha
        ctx.blend_func = (
            moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA,
            moderngl.ONE, moderngl.ONE
        )

.. py:method:: Context.blend_func_i(buffer: int, value: tuple) -> None

    Set the blend func for a specific buffer.

    Example::
        #sets blend func for all buffers in a framebuffer
        ctx.blend_func = moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA

        #sets blend func only for the second buffer
        ctx.blend_func_i(
            1,
            (moderngl.ONE, moderngl.ONE)
        )

.. py:attribute:: Context.blend_equation
    :type: tuple

    Set the blend equation (write only).

    Blend equations specify how source and destination colors are combined
    in blending operations. By default ``FUNC_ADD`` is used.

    Blend equation can be set for rgb and alpha separately if needed.

    Supported functions are::

        moderngl.FUNC_ADD               # source + destination
        moderngl.FUNC_SUBTRACT          # source - destination
        moderngl.FUNC_REVERSE_SUBTRACT  # destination - source
        moderngl.MIN                    # Minimum of source and destination
        moderngl.MAX                    # Maximum of source and destination

    Example::

        # For both rgb and alpha channel
        ctx.blend_equation = moderngl.FUNC_ADD

        # Separate for rgb and alpha channel
        ctx.blend_equation = moderngl.FUNC_ADD, moderngl.MAX

.. py:attribute:: Context.multisample
    :type: bool

    Enable/disable multisample mode (``GL_MULTISAMPLE``).

    This property is write only.

    Example::

        # Enable
        ctx.multisample = True
        # Disable
        ctx.multisample = False

.. py:attribute:: Context.viewport
    :type: tuple

    Get or set the viewport of the active framebuffer.

    Example::

        >>> ctx.viewport
        (0, 0, 1280, 720)
        >>> ctx.viewport = (0, 0, 640, 360)
        >>> ctx.viewport
        (0, 0, 640, 360)

    If no framebuffer is bound ``(0, 0, 0, 0)`` will be returned.

.. py:attribute:: Context.scissor
    :type: tuple

    Get or set the scissor box for the active framebuffer.

    When scissor testing is enabled fragments outside
    the defined scissor box will be discarded. This
    applies to rendered geometry or :py:meth:`Context.clear`.

    Setting is value enables scissor testing in the framebuffer.
    Setting the scissor to ``None`` disables scissor testing
    and reverts the scissor box to match the framebuffer size.

    Example::

        # Enable scissor testing
        >>> ctx.scissor = 100, 100, 200, 100
        # Disable scissor testing
        >>> ctx.scissor = None

    If no framebuffer is bound ``(0, 0, 0, 0)`` will be returned.

.. py:attribute:: Context.version_code
    :type: int



.. py:attribute:: Context.screen
    :type: Framebuffer

    A Framebuffer instance representing the screen.

    Normally set when creating a context with ``create_context()`` attaching to
    an existing context. This is the special system framebuffer
    represented by framebuffer ``id=0``.

    When creating a standalone context this property is not set since
    there are no default framebuffer.

.. py:attribute:: Context.fbo
    :type: Framebuffer



.. py:attribute:: Context.front_face
    :type: str

    The front_face. Acceptable values are ``'ccw'`` (default) or ``'cw'``.

    Face culling must be enabled for this to have any effect:
    ``ctx.enable(moderngl.CULL_FACE)``.

    Example::

        # Triangles winded counter-clockwise considered front facing
        ctx.front_face = 'ccw'
        # Triangles winded clockwise considered front facing
        ctx.front_face = 'cw'

.. py:attribute:: Context.cull_face
    :type: str

    The face side to cull. Acceptable values are ``'back'`` (default) ``'front'`` or ``'front_and_back'``.

    This is similar to :py:meth:`Context.front_face`

    Face culling must be enabled for this to have any effect:
    ``ctx.enable(moderngl.CULL_FACE)``.

    Example::

        ctx.cull_face = 'front'
        ctx.cull_face = 'back'
        ctx.cull_face = 'front_and_back'

.. py:attribute:: Context.wireframe
    :type: bool

    Wireframe settings for debugging.

.. py:attribute:: Context.max_samples
    :type: int

    The maximum supported number of samples for multisampling.

.. py:attribute:: Context.max_integer_samples
    :type: int

    The max integer samples.

.. py:attribute:: Context.max_texture_units
    :type: int

    The max texture units.

.. py:attribute:: Context.max_anisotropy
    :type: float

    The maximum value supported for anisotropic filtering.

.. py:attribute:: Context.default_texture_unit
    :type: int

    The default texture unit.

.. py:attribute:: Context.patch_vertices
    :type: int

    The number of vertices that will be used to make up a single patch primitive.

.. py:attribute:: Context.provoking_vertex
    :type: int

    Specifies the vertex to be used as the source of data for flat shaded varyings.

    Flatshading a vertex shader varying output (ie. ``flat out vec3 pos``) means to assign
    all vetices of the primitive the same value for that output. The vertex from which
    these values is derived is known as the provoking vertex.

    It can be configured to be the first or the last vertex.

    This property is write only.

    Example::

        # Use first vertex
        ctx.provoking_vertex = moderngl.FIRST_VERTEX_CONVENTION

        # Use last vertex
        ctx.provoking_vertex = moderngl.LAST_VERTEX_CONVENTION

.. py:attribute:: Context.polygon_offset
    :type: tuple

    Get or set the current polygon offset.

    The tuple values represents two float values: ``unit`` and a ``factor``::

        ctx.polygon_offset = unit, factor

    When drawing polygons, lines or points directly on top of
    exiting geometry the result is often not visually pleasant.
    We can experience z-fighting or partially fading fragments
    due to different primitives not being rasterized in the exact
    same way or simply depth buffer precision issues.

    For example when visualizing polygons drawing a wireframe
    version on top of the original mesh, these issues are
    immediately apparent. Applying decals to surfaces is
    another common example.

    The official documentation states the following::

        When enabled, the depth value of each fragment is added
        to a calculated offset value. The offset is added before
        the depth test is performed and before the depth value
        is written into the depth buffer. The offset value o is calculated by:
        o = m * factor + r * units
        where m is the maximum depth slope of the polygon and r is the smallest
        value guaranteed to produce a resolvable difference in window coordinate
        depth values. The value r is an implementation-specific int.

    In simpler terms: We use polygon offset to either add a positive offset to
    the geometry (push it away from you) or a negative offset to geometry
    (pull it towards you).

    * ``units`` is a int offset to depth and will do the job alone
        if we are working with geometry parallel to the near/far plane.
    * The ``factor`` helps you handle sloped geometry (not parallel to near/far plane).

    In most cases you can get away with ``[-1.0, 1.0]`` for both factor and units,
    but definitely play around with the values. When both values are set to ``0``
    polygon offset is disabled internally.

    To just get started with something you can try::

        # Either push the geomtry away or pull it towards you
        # with support for handling small to medium sloped geometry
        ctx.polygon_offset = 1.0, 1.0
        ctx.polygon_offset = -1.0, -1.0

        # Disable polygon offset
        ctx.polygon_offset = 0, 0

.. py:attribute:: Context.error
    :type: str

    The result of ``glGetError()`` but human readable.

    This values is provided for debug purposes only and is likely to
    reduce performace when used in a draw loop.

.. py:attribute:: Context.extensions
    :type: Set[str]

    The extensions supported by the context.

    All extensions names have a ``GL_`` prefix, so if the spec refers to ``ARB_compute_shader``
    we need to look for ``GL_ARB_compute_shader``::

        # If compute shaders are supported ...
        >> 'GL_ARB_compute_shader' in ctx.extensions
        True

    Example data::

        {
            'GL_ARB_multi_bind',
            'GL_ARB_shader_objects',
            'GL_ARB_half_float_vertex',
            'GL_ARB_map_buffer_alignment',
            'GL_ARB_arrays_of_arrays',
            'GL_ARB_pipeline_statistics_query',
            'GL_ARB_provoking_vertex',
            'GL_ARB_gpu_shader5',
            'GL_ARB_uniform_buffer_object',
            'GL_EXT_blend_equation_separate',
            'GL_ARB_tessellation_shader',
            'GL_ARB_multi_draw_indirect',
            'GL_ARB_multisample',
            .. etc ..
        }

.. py:attribute:: Context.info
    :type: Dict[str, Any]

    OpenGL Limits and information about the context.

    Example::

        # The maximum width and height of a texture
        >> ctx.info['GL_MAX_TEXTURE_SIZE']
        16384

        # Vendor and renderer
        >> ctx.info['GL_VENDOR']
        NVIDIA Corporation
        >> ctx.info['GL_RENDERER']
        NVIDIA GeForce GT 650M OpenGL Engine

    Example data::

        {
            'GL_VENDOR': 'NVIDIA Corporation',
            'GL_RENDERER': 'NVIDIA GeForce GT 650M OpenGL Engine',
            'GL_VERSION': '4.1 NVIDIA-10.32.0 355.11.10.10.40.102',
            'GL_POINT_SIZE_RANGE': (1.0, 2047.0),
            'GL_SMOOTH_LINE_WIDTH_RANGE': (0.5, 1.0),
            'GL_ALIASED_LINE_WIDTH_RANGE': (1.0, 1.0),
            'GL_POINT_FADE_THRESHOLD_SIZE': 1.0,
            'GL_POINT_SIZE_GRANULARITY': 0.125,
            'GL_SMOOTH_LINE_WIDTH_GRANULARITY': 0.125,
            'GL_MIN_PROGRAM_TEXEL_OFFSET': -8.0,
            'GL_MAX_PROGRAM_TEXEL_OFFSET': 7.0,
            'GL_MINOR_VERSION': 1,
            'GL_MAJOR_VERSION': 4,
            'GL_SAMPLE_BUFFERS': 0,
            'GL_SUBPIXEL_BITS': 8,
            'GL_CONTEXT_PROFILE_MASK': 1,
            'GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT': 256,
            'GL_DOUBLEBUFFER': False,
            'GL_STEREO': False,
            'GL_MAX_VIEWPORT_DIMS': (16384, 16384),
            'GL_MAX_3D_TEXTURE_SIZE': 2048,
            'GL_MAX_ARRAY_TEXTURE_LAYERS': 2048,
            'GL_MAX_CLIP_DISTANCES': 8,
            'GL_MAX_COLOR_ATTACHMENTS': 8,
            'GL_MAX_COLOR_TEXTURE_SAMPLES': 8,
            'GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS': 233472,
            'GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS': 231424,
            'GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS': 80,
            'GL_MAX_COMBINED_UNIFORM_BLOCKS': 70,
            'GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS': 233472,
            'GL_MAX_CUBE_MAP_TEXTURE_SIZE': 16384,
            'GL_MAX_DEPTH_TEXTURE_SAMPLES': 8,
            'GL_MAX_DRAW_BUFFERS': 8,
            'GL_MAX_DUAL_SOURCE_DRAW_BUFFERS': 1,
            'GL_MAX_ELEMENTS_INDICES': 150000,
            'GL_MAX_ELEMENTS_VERTICES': 1048575,
            'GL_MAX_FRAGMENT_INPUT_COMPONENTS': 128,
            'GL_MAX_FRAGMENT_UNIFORM_COMPONENTS': 4096,
            'GL_MAX_FRAGMENT_UNIFORM_VECTORS': 1024,
            'GL_MAX_FRAGMENT_UNIFORM_BLOCKS': 14,
            'GL_MAX_GEOMETRY_INPUT_COMPONENTS': 128,
            'GL_MAX_GEOMETRY_OUTPUT_COMPONENTS': 128,
            'GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS': 16,
            'GL_MAX_GEOMETRY_UNIFORM_BLOCKS': 14,
            'GL_MAX_GEOMETRY_UNIFORM_COMPONENTS': 2048,
            'GL_MAX_INTEGER_SAMPLES': 1,
            'GL_MAX_SAMPLES': 8,
            'GL_MAX_RECTANGLE_TEXTURE_SIZE': 16384,
            'GL_MAX_RENDERBUFFER_SIZE': 16384,
            'GL_MAX_SAMPLE_MASK_WORDS': 1,
            'GL_MAX_SERVER_WAIT_TIMEOUT': -1,
            'GL_MAX_TEXTURE_BUFFER_SIZE': 134217728,
            'GL_MAX_TEXTURE_IMAGE_UNITS': 16,
            'GL_MAX_TEXTURE_LOD_BIAS': 15,
            'GL_MAX_TEXTURE_SIZE': 16384,
            'GL_MAX_UNIFORM_BUFFER_BINDINGS': 70,
            'GL_MAX_UNIFORM_BLOCK_SIZE': 65536,
            'GL_MAX_VARYING_COMPONENTS': 0,
            'GL_MAX_VARYING_VECTORS': 31,
            'GL_MAX_VARYING_FLOATS': 0,
            'GL_MAX_VERTEX_ATTRIBS': 16,
            'GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS': 16,
            'GL_MAX_VERTEX_UNIFORM_COMPONENTS': 4096,
            'GL_MAX_VERTEX_UNIFORM_VECTORS': 1024,
            'GL_MAX_VERTEX_OUTPUT_COMPONENTS': 128,
            'GL_MAX_VERTEX_UNIFORM_BLOCKS': 14,
            'GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET': 0,
            'GL_MAX_VERTEX_ATTRIB_BINDINGS': 0,
            'GL_VIEWPORT_BOUNDS_RANGE': (-32768, 32768),
            'GL_VIEWPORT_SUBPIXEL_BITS': 0,
            'GL_MAX_VIEWPORTS': 16
        }

.. py:attribute:: Context.includes
    :type: Dict[str, str]

    Mapping used for include statements.

.. py:attribute:: Context.extra
    :type: Any

    User defined data.

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

.. py:attribute:: Context.NOTHING
    :type: int

    Represents no states. Can be used with :py:meth:`Context.enable_only` to disable all states.

.. py:attribute:: Context.BLEND
    :type: int

    Enable/disable blending

.. py:attribute:: Context.DEPTH_TEST
    :type: int

    Enable/disable depth testing

.. py:attribute:: Context.CULL_FACE
    :type: int

    Enable/disable face culling

.. py:attribute:: Context.RASTERIZER_DISCARD
    :type: int

    Enable/disable rasterization

    Context flag: Enables ``gl_PointSize`` in vertex or geometry shaders.

    When enabled we can write to ``gl_PointSize`` in the vertex shader to specify the point size
    for each individual point.

    If this value is not set in the shader the behavior is undefined. This means the points may
    or may not appear depending if the drivers enforce some default value for ``gl_PointSize``.

.. py:attribute:: Context.PROGRAM_POINT_SIZE
    :type: int

    When disabled :py:attr:`Context.point_size` is used.

Primitive Modes
---------------

.. py:attribute:: Context.POINTS
    :type: int

    Each vertex represents a point

.. py:attribute:: Context.LINES
    :type: int

    Vertices 0 and 1 are considered a line. Vertices 2 and 3 are considered a line.
    And so on. If the user specifies a non-even number of vertices, then the extra vertex is ignored.

.. py:attribute:: Context.LINE_LOOP
    :type: int

    As line strips, except that the first and last vertices are also used as a line.
    Thus, you get n lines for n input vertices. If the user only specifies 1 vertex,
    the drawing command is ignored. The line between the first and last vertices happens
    after all of the previous lines in the sequence.

.. py:attribute:: Context.LINE_STRIP
    :type: int

    The adjacent vertices are considered lines. Thus, if you pass n vertices, you will get n-1 lines.
    If the user only specifies 1 vertex, the drawing command is ignored.

.. py:attribute:: Context.TRIANGLES
    :type: int

    Vertices 0, 1, and 2 form a triangle. Vertices 3, 4, and 5 form a triangle. And so on.

.. py:attribute:: Context.TRIANGLE_STRIP
    :type: int

    Every group of 3 adjacent vertices forms a triangle. The face direction of the
    strip is determined by the winding of the first triangle. Each successive triangle
    will have its effective face order reversed, so the system compensates for that
    by testing it in the opposite way. A vertex stream of n length will generate n-2 triangles.

.. py:attribute:: Context.TRIANGLE_FAN
    :type: int

    The first vertex is always held fixed. From there on, every group of 2 adjacent
    vertices form a triangle with the first. So with a vertex stream, you get a list
    of triangles like so: (0, 1, 2) (0, 2, 3), (0, 3, 4), etc. A vertex stream of
    n length will generate n-2 triangles.

.. py:attribute:: Context.LINES_ADJACENCY
    :type: int

    These are special primitives that are expected to be used specifically with
    geomtry shaders. These primitives give the geometry shader more vertices
    to work with for each input primitive. Data needs to be duplicated in buffers.

.. py:attribute:: Context.LINE_STRIP_ADJACENCY
    :type: int

    These are special primitives that are expected to be used specifically with
    geomtry shaders. These primitives give the geometry shader more vertices
    to work with for each input primitive. Data needs to be duplicated in buffers.

.. py:attribute:: Context.TRIANGLES_ADJACENCY
    :type: int

    These are special primitives that are expected to be used specifically with
    geomtry shaders. These primitives give the geometry shader more vertices
    to work with for each input primitive. Data needs to be duplicated in buffers.

.. py:attribute:: Context.TRIANGLE_STRIP_ADJACENCY
    :type: int

    These are special primitives that are expected to be used specifically with
    geomtry shaders. These primitives give the geometry shader more vertices
    to work with for each input primitive. Data needs to be duplicated in buffers.

.. py:attribute:: Context.PATCHES
    :type: int

    primitive type can only be used when Tessellation is active. It is a primitive
    with a user-defined number of vertices, which is then tessellated based on the
    control and evaluation shaders into regular points, lines, or triangles, depending
    on the TES's settings.


Texture Filters
~~~~~~~~~~~~~~~

Also available in the :py:class:`Context` instance
including mode details.


.. py:attribute:: Context.NEAREST
    :type: int

    Returns the value of the texture element that is nearest
    (in Manhattan distance) to the specified texture coordinates.

.. py:attribute:: Context.LINEAR
    :type: int

    Returns the weighted average of the four texture elements
    that are closest to the specified texture coordinates.
    These can include items wrapped or repeated from other parts
    of a texture, depending on the values of texture repeat mode,
    and on the exact mapping.

.. py:attribute:: Context.NEAREST_MIPMAP_NEAREST
    :type: int

    Chooses the mipmap that most closely matches the size of the
    pixel being textured and uses the ``NEAREST`` criterion (the texture
    element closest to the specified texture coordinates) to produce
    a texture value.

.. py:attribute:: Context.LINEAR_MIPMAP_NEAREST
    :type: int

    Chooses the mipmap that most closely matches the size of the pixel
    being textured and uses the ``LINEAR`` criterion (a weighted average
    of the four texture elements that are closest to the specified
    texture coordinates) to produce a texture value.

.. py:attribute:: Context.NEAREST_MIPMAP_LINEAR
    :type: int

    Chooses the two mipmaps that most closely match the size of the
    pixel being textured and uses the ``NEAREST`` criterion (the texture
    element closest to the specified texture coordinates ) to produce
    a texture value from each mipmap. The final texture value is a
    weighted average of those two values.

.. py:attribute:: Context.LINEAR_MIPMAP_LINEAR
    :type: int

    Chooses the two mipmaps that most closely match the size of the pixel
    being textured and uses the ``LINEAR`` criterion (a weighted average
    of the texture elements that are closest to the specified texture
    coordinates) to produce a texture value from each mipmap.
    The final texture value is a weighted average of those two values.


Blend Functions
---------------

Blend functions are used with :py:attr:`Context.blend_func`
to control blending operations.

.. code::

    # Default value
    ctx.blend_func = ctx.SRC_ALPHA, ctx.ONE_MINUS_SRC_ALPHA

.. py:attribute:: Context.ZERO
    :type: int

    (0,0,0,0)

.. py:attribute:: Context.ONE
    :type: int

    (1,1,1,1)

.. py:attribute:: Context.SRC_COLOR
    :type: int

    (Rs0/kR,Gs0/kG,Bs0/kB,As0/kA)

.. py:attribute:: Context.ONE_MINUS_SRC_COLOR
    :type: int

    (1,1,1,1) - (Rs0/kR,Gs0/kG,Bs0/kB,As0/kA)

.. py:attribute:: Context.SRC_ALPHA
    :type: int

    (As0/kA,As0/kA,As0/kA,As0/kA)

.. py:attribute:: Context.ONE_MINUS_SRC_ALPHA
    :type: int

    (1,1,1,1) - (As0/kA,As0/kA,As0/kA,As0/kA)

.. py:attribute:: Context.DST_ALPHA
    :type: int

    (Ad/kA,Ad/kA,Ad/kA,Ad/kA)

.. py:attribute:: Context.ONE_MINUS_DST_ALPHA
    :type: int

    (1,1,1,1) - (Ad/kA,Ad/kA,Ad/kA,Ad/kA)

.. py:attribute:: Context.DST_COLOR
    :type: int

    (Rd/kR,Gd/kG,Bd/kB,Ad/kA)

.. py:attribute:: Context.ONE_MINUS_DST_COLOR
    :type: int

    (1,1,1,1) - (Rd/kR,Gd/kG,Bd/kB,Ad/kA)


Blend Function Shortcuts
------------------------

.. py:attribute:: Context.DEFAULT_BLENDING
    :type: tuple

    Shotcut for the default blending ``SRC_ALPHA, ONE_MINUS_SRC_ALPHA``

.. py:attribute:: Context.ADDITIVE_BLENDING
    :type: tuple

    Shotcut for additive blending ``ONE, ONE``

.. py:attribute:: Context.PREMULTIPLIED_ALPHA
    :type: tuple

    Shotcut for blend mode when using premultiplied alpha ``SRC_ALPHA, ONE``


Blend Equations
---------------

Used with :py:attr:`Context.blend_equation`.

.. py:attribute:: Context.FUNC_ADD
    :type: int

    source + destination

.. py:attribute:: Context.FUNC_SUBTRACT
    :type: int

    source - destination

.. py:attribute:: Context.FUNC_REVERSE_SUBTRACT
    :type: int

    destination - source

.. py:attribute:: Context.MIN
    :type: int

    Minimum of source and destination

.. py:attribute:: Context.MAX
    :type: int

    Maximum of source and destination


Other Enums
-----------

.. py:attribute:: Context.FIRST_VERTEX_CONVENTION
    :type: int

    Specifies the first vertex should be used as the source of data for flat shaded varyings.
    Used with :py:attr:`Context.provoking_vertex`.

.. py:attribute:: Context.LAST_VERTEX_CONVENTION
    :type: int

    Specifies the last vertex should be used as the source of data for flat shaded varyings.
    Used with :py:attr:`Context.provoking_vertex`.

.. py:attribute:: Context.VERTEX_ATTRIB_ARRAY_BARRIER_BIT
    :type: int

    VERTEX_ATTRIB_ARRAY_BARRIER_BIT

.. py:attribute:: Context.ELEMENT_ARRAY_BARRIER_BIT
    :type: int

    ELEMENT_ARRAY_BARRIER_BIT

.. py:attribute:: Context.UNIFORM_BARRIER_BIT
    :type: int

    UNIFORM_BARRIER_BIT

.. py:attribute:: Context.TEXTURE_FETCH_BARRIER_BIT
    :type: int

    TEXTURE_FETCH_BARRIER_BIT

.. py:attribute:: Context.SHADER_IMAGE_ACCESS_BARRIER_BIT
    :type: int

    SHADER_IMAGE_ACCESS_BARRIER_BIT

.. py:attribute:: Context.COMMAND_BARRIER_BIT
    :type: int

    COMMAND_BARRIER_BIT

.. py:attribute:: Context.PIXEL_BUFFER_BARRIER_BIT
    :type: int

    PIXEL_BUFFER_BARRIER_BIT

.. py:attribute:: Context.TEXTURE_UPDATE_BARRIER_BIT
    :type: int

    TEXTURE_UPDATE_BARRIER_BIT

.. py:attribute:: Context.BUFFER_UPDATE_BARRIER_BIT
    :type: int

    BUFFER_UPDATE_BARRIER_BIT

.. py:attribute:: Context.FRAMEBUFFER_BARRIER_BIT
    :type: int

    FRAMEBUFFER_BARRIER_BIT

.. py:attribute:: Context.TRANSFORM_FEEDBACK_BARRIER_BIT
    :type: int

    TRANSFORM_FEEDBACK_BARRIER_BIT

.. py:attribute:: Context.ATOMIC_COUNTER_BARRIER_BIT
    :type: int

    ATOMIC_COUNTER_BARRIER_BIT

.. py:attribute:: Context.SHADER_STORAGE_BARRIER_BIT
    :type: int

    SHADER_STORAGE_BARRIER_BIT

.. py:attribute:: Context.ALL_BARRIER_BITS
    :type: int

    ALL_BARRIER_BITS

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
