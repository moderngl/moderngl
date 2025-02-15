Texture
=======

.. py:class:: Texture

    Returned by :py:meth:`Context.texture` and :py:meth:`Context.depth_texture`

    A Texture is an OpenGL object that contains one or more images that all have the same image format.

    A texture can be used in two ways. It can
    be the source of a texture access from a Shader, or it can be used
    as a render target.

    A Texture object cannot be instantiated directly, it requires a context.
    Use :py:meth:`Context.texture` or :py:meth:`Context.depth_texture`
    to create one.

Methods
-------

.. py:method:: Texture.read(alignment: int = 1, gil=False) -> bytes

    Read the pixel data as bytes into system memory.

    :param int alignment: The byte alignment of the pixels.
    :param bool gil: Whether to hold the GIL while waiting for the data.

.. py:method:: Texture.read_into(buffer: Any, alignment: int = 1, write_offset: int = 0, gil=False)

    Read the content of the texture into a bytearray or :py:class:`~moderngl.Buffer`.

    The advantage of reading into a :py:class:`~moderngl.Buffer` is that pixel data
    does not need to travel all the way to system memory::

        # Reading pixel data into a bytearray
        data = bytearray(8)
        texture = ctx.texture3d((2, 2, 2), 1)
        texture.read_into(data)

        # Reading pixel data into a buffer
        data = ctx.buffer(reserve=8)
        texture = ctx.texture3d((2, 2, 2), 1)
        texture.read_into(data)

    :param bytearray buffer: The buffer that will receive the pixels.
    :param int alignment: The byte alignment of the pixels.
    :param int write_offset: The write offset.
    :param bool gil: Whether to hold the GIL while waiting for the data.

.. py:method:: Texture.write(data: Any, viewport: tuple, alignment: int = 1)

    Update the content of the texture from byte data or a moderngl :py:class:`~moderngl.Buffer`.

    Examples::

        # Write data from a moderngl Buffer
        data = ctx.buffer(reserve=8)
        texture = ctx.texture3d((2, 2, 2), 1)
        texture.write(data)

        # Write data from bytes
        data = b'\xff\xff\xff\xff\xff\xff\xff\xff'
        texture = ctx.texture3d((2, 2), 1)
        texture.write(data)

    :param bytes data: The pixel data.
    :param tuple viewport: The viewport.
    :param int alignment: The byte alignment of the pixels.

.. py:method:: Texture.build_mipmaps(base: int = 0, max_level: int = 1000) -> None

    Generate mipmaps.

    This also changes the texture filter to ``LINEAR_MIPMAP_LINEAR, LINEAR``
    (Will be removed in ``6.x``)

    :param int base: The base level
    :param int max_level: The maximum levels to generate

.. py:method:: Texture.bind_to_image(unit: int, read: bool = True, write: bool = True, level: int = 0, format: int = 0) -> None

    Bind a texture to an image unit (OpenGL 4.2 required).

    This is used to bind textures to image units for shaders.
    The idea with image load/store is that the user can bind
    one of the images in a Texture to a number of image binding points
    (which are separate from texture image units). Shaders can read
    information from these images and write information to them,
    in ways that they cannot with textures.

    It's important to specify the right access type for the image.
    This can be set with the ``read`` and ``write`` arguments.
    Allowed combinations are:

    - **Read-only**: ``read=True`` and ``write=False``
    - **Write-only**: ``read=False`` and ``write=True``
    - **Read-write**: ``read=True`` and ``write=True``

    ``format`` specifies the format that is to be used when performing
    formatted stores into the image from shaders. ``format`` must be
    compatible with the texture's internal format. **By default the format
    of the texture is passed in. The format parameter is only needed
    when overriding this behavior.**

    Note that we bind the 3D textured layered making the entire texture
    readable and writable. It is possible to bind a specific 2D section
    in the future.

    More information:

    - https://www.khronos.org/opengl/wiki/Image_Load_Store
    - https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindImageTexture.xhtml

    :param int unit: Specifies the index of the image unit to which to bind the texture
    :param Texture texture: The texture to bind
    :param bool read: Allows the shader to read the image (default: ``True``)
    :param bool write: Allows the shader to write to the image (default: ``True``)
    :param int level: Level of the texture to bind (default: ``0``).
    :param int format: (optional) The OpenGL enum value representing the format (defaults to the texture's format)

.. py:method:: Texture.use(location: int = 0) -> None

    Better to use :py:class:`Sampler` objects and avoid this call on the Texture object directly.

    Bind the texture to a texture unit.

    :param int location: The texture location/unit.

    The location is the texture unit we want to bind the texture.
    This should correspond with the value of the ``sampler2D``
    uniform in the shader because samplers read from the texture
    unit we assign to them::

        # Define what texture unit our two sampler3D uniforms should represent
        program['texture_a'] = 0
        program['texture_b'] = 1
        # Bind textures to the texture units
        first_texture.use(location=0)
        second_texture.use(location=1)

.. py:method:: Texture.get_handle(resident: bool = True) -> int

    Handle for Bindless Textures.

    :param bool resident: Make the texture resident.

    Once a handle is created its parameters cannot be changed.
    Attempting to do so will have no effect. (filter, wrap etc).
    There is no way to undo this immutability.

    Handles cannot be used by shaders until they are resident.
    This method can be called multiple times to move a texture
    in and out of residency::

        >> texture.get_handle(resident=False)
        4294969856
        >> texture.get_handle(resident=True)
        4294969856

    Ths same handle is returned if the handle already exists.

    .. note:: Limitations from the OpenGL wiki

        The amount of storage available for resident images/textures may be less
        than the total storage for textures that is available. As such, you should
        attempt to minimize the time a texture spends being resident. Do not attempt
        to take steps like making textures resident/unresident every frame or something.
        But if you are finished using a texture for some time, make it unresident.

.. py:method:: Texture.release

Attributes
----------

.. py:attribute:: Texture.width
    :type: int

    The width of the texture.

.. py:attribute:: Texture.height
    :type: int

    The height of the texture.

.. py:attribute:: Texture.size
    :type: Tuple[int, int]

    The size of the texture.

.. py:attribute:: Texture.components
    :type: int

    The number of components of the texture.

.. py:attribute:: Texture.samples
    :type: int

    The number of samples set for the texture used in multisampling.

.. py:attribute:: Texture.depth
    :type: bool

    Determines if the texture contains depth values.

.. py:attribute:: Texture.dtype
    :type: str

    Data type.

.. py:attribute:: Texture.swizzle
    :type: str

    The swizzle mask of the texture (Default ``'RGBA'``).

    The swizzle mask change/reorder the ``vec4`` value returned by the ``texture()`` function
    in a GLSL shaders. This is represented by a 4 character string were each
    character can be::

        'R' GL_RED
        'G' GL_GREEN
        'B' GL_BLUE
        'A' GL_ALPHA
        '0' GL_ZERO
        '1' GL_ONE

    Example::

        # Alpha channel will always return 1.0
        texture.swizzle = 'RGB1'

        # Only return the red component. The rest is masked to 0.0
        texture.swizzle = 'R000'

        # Reverse the components
        texture.swizzle = 'ABGR'

.. py:attribute:: Texture.repeat_x

    See :py:class:`Sampler.repeat_x`

.. py:attribute:: Texture.repeat_y

    See :py:class:`Sampler.repeat_y`

.. py:attribute:: Texture.filter

    See :py:class:`Sampler.filter`

.. py:attribute:: Texture.compare_func

    See :py:class:`Sampler.compare_func`

.. py:attribute:: Texture.anisotropy

    See :py:class:`Sampler.anisotropy`

.. py:attribute:: Texture.ctx
    :type: Context

    The context this object belongs to

.. py:attribute:: Texture.glo
    :type: int

    The internal OpenGL object.
    This values is provided for interoperability and debug purposes only.

.. py:attribute:: Texture.extra
    :type: Any

    User defined data.
