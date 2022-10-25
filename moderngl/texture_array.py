from typing import Any, Optional, Tuple, Union

from moderngl.mgl import InvalidObject  # type: ignore

from .buffer import Buffer

__all__ = ['TextureArray']


class TextureArray:
    """
    An Array Texture is a Texture where each mipmap level contains an array of images of the same size.

    Array textures may have Mipmaps, but each mipmap
    in the texture has the same number of levels.

    A TextureArray object cannot be instantiated directly, it requires a context.
    Use :py:meth:`Context.texture_array` to create one.
    """

    __slots__ = ['mglo', '_size', '_components', '_samples', '_dtype', '_depth', '_glo', 'ctx', 'extra']

    def __init__(self):
        self.mglo = None  #: Internal representation for debug purposes only.
        self._size = (None, None, None)
        self._components = None
        self._samples = None
        self._dtype = None
        self._depth = None
        self._glo = None
        self.ctx = None  #: The context this object belongs to
        self.extra = None  #: Any - Attribute for storing user defined objects
        raise TypeError()

    def __repr__(self):
        if hasattr(self, '_glo'):
            return f"<{self.__class__.__name__}: {self._glo}>"
        else:
            return f"<{self.__class__.__name__}: INCOMPLETE>"

    def __eq__(self, other: Any):
        return type(self) is type(other) and self.mglo is other.mglo

    def __hash__(self) -> int:
        return id(self)

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    @property
    def repeat_x(self) -> bool:
        """
        bool: The x repeat flag for the texture (Default ``True``).

        Example::

            # Enable texture repeat (GL_REPEAT)
            texture.repeat_x = True

            # Disable texture repeat (GL_CLAMP_TO_EDGE)
            texture.repeat_x = False
        """
        return self.mglo.repeat_x

    @repeat_x.setter
    def repeat_x(self, value: bool) -> None:
        self.mglo.repeat_x = value

    @property
    def repeat_y(self) -> bool:
        """
        bool: The y repeat flag for the texture (Default ``True``).

        Example::

            # Enable texture repeat (GL_REPEAT)
            texture.repeat_y = True

            # Disable texture repeat (GL_CLAMP_TO_EDGE)
            texture.repeat_y = False
        """
        return self.mglo.repeat_y

    @repeat_y.setter
    def repeat_y(self, value: bool) -> None:
        self.mglo.repeat_y = value

    @property
    def filter(self) -> Tuple[int, int]:
        """
        tuple: The minification and magnification filter for the texture.

        (Default ``(moderngl.LINEAR. moderngl.LINEAR)``)

        Example::

            texture.filter == (moderngl.NEAREST, moderngl.NEAREST)
            texture.filter == (moderngl.LINEAR_MIPMAP_LINEAR, moderngl.LINEAR)
            texture.filter == (moderngl.NEAREST_MIPMAP_LINEAR, moderngl.NEAREST)
            texture.filter == (moderngl.LINEAR_MIPMAP_NEAREST, moderngl.NEAREST)
        """
        return self.mglo.filter

    @filter.setter
    def filter(self, value: Tuple[int, int]) -> None:
        self.mglo.filter = value

    @property
    def swizzle(self) -> str:
        """
        str: The swizzle mask of the texture (Default ``'RGBA'``).

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
        """
        return self.mglo.swizzle

    @swizzle.setter
    def swizzle(self, value: str) -> None:
        self.mglo.swizzle = value

    @property
    def anisotropy(self) -> float:
        """
        float: Number of samples for anisotropic filtering (Default ``1.0``).

        The value will be clamped in range ``1.0`` and ``ctx.max_anisotropy``.

        Any value greater than 1.0 counts as a use of anisotropic filtering::

            # Disable anisotropic filtering
            texture.anisotropy = 1.0

            # Enable anisotropic filtering suggesting 16 samples as a maximum
            texture.anisotropy = 16.0
        """
        return self.mglo.anisotropy

    @anisotropy.setter
    def anisotropy(self, value: float) -> None:
        self.mglo.anisotropy = value

    @property
    def width(self) -> int:
        """int: The width of the texture array."""
        return self._size[0]

    @property
    def height(self) -> int:
        """int: The height of the texture array."""
        return self._size[1]

    @property
    def layers(self) -> int:
        """int: The number of layers of the texture array."""
        return self._size[2]

    @property
    def size(self) -> tuple:
        """tuple: The size of the texture array."""
        return self._size

    @property
    def components(self) -> int:
        """int: The number of components of the texture array."""
        return self._components

    @property
    def dtype(self) -> str:
        """str: Data type."""
        return self._dtype

    @property
    def glo(self) -> int:
        """
        int: The internal OpenGL object.

        This values is provided for debug purposes only.
        """
        return self._glo

    def read(self, *, alignment: int = 1) -> bytes:
        """
        Read the pixel data as bytes into system memory.

        Keyword Args:
            alignment (int): The byte alignment of the pixels.

        Returns:
            bytes
        """
        return self.mglo.read(alignment)

    def read_into(
        self,
        buffer: Any,
        *,
        alignment: int = 1,
        write_offset: int = 0,
    ) -> None:
        """
        Read the content of the texture array into a bytearray or :py:class:`~moderngl.Buffer`.

        The advantage of reading into a :py:class:`~moderngl.Buffer` is that pixel data
        does not need to travel all the way to system memory::

            # Reading pixel data into a bytearray
            data = bytearray(8)
            texture = ctx.texture((2, 2, 2), 1)
            texture.read_into(data)

            # Reading pixel data into a buffer
            data = ctx.buffer(reserve=8)
            texture = ctx.texture((2, 2, 2), 1)
            texture.read_into(data)

        Args:
            buffer (Union[bytearray, Buffer]): The buffer that will receive the pixels.

        Keyword Args:
            alignment (int): The byte alignment of the pixels.
            write_offset (int): The write offset.
        """
        if type(buffer) is Buffer:
            buffer = buffer.mglo

        return self.mglo.read_into(buffer, alignment, write_offset)

    def write(
        self,
        data: Any,
        viewport: Optional[Union[Tuple[int, int, int], Tuple[int, int, int, int, int, int]]] = None,
        *,
        alignment: int = 1,
    ) -> None:
        r"""
        Update the content of the texture array from byte data or a moderngl :py:class:`~moderngl.Buffer`.

        The ``viewport`` can be used for finer control of where the
        data should be written in the array. The valid versions are::

            # Writing multiple layers from the begining of the texture
            texture.write(data, viewport=(width, hight, num_layers))

            # Writing sub-sections of the array
            texture.write(data, viewport=(x, y, layer, width, height, num_layers))

        Like with other texture types we can also use bytes or :py:class:`~moderngl.Buffer`
        as a source::

            # Using a moderngl buffer
            data = ctx.buffer(reserve=8)
            texture = ctx.texture_array((2, 2, 2), 1)
            texture.write(data)

            # Using byte data from system memory
            data = b"\xff\xff\xff\xff\xff\xff\xff\xff"
            texture = ctx.texture_array((2, 2, 2), 1)
            texture.write(data)

        Args:
            data (bytes): The pixel data.
            viewport (tuple): The viewport.

        Keyword Args:
            alignment (int): The byte alignment of the pixels.
        """
        if type(data) is Buffer:
            data = data.mglo

        self.mglo.write(data, viewport, alignment)

    def build_mipmaps(self, base: int = 0, max_level: int = 1000) -> None:
        """
        Generate mipmaps.

        This also changes the texture filter to ``LINEAR_MIPMAP_LINEAR, LINEAR``
        (Will be removed in ``6.x``)

        Keyword Args:
            base (int): The base level
            max_level (int): The maximum levels to generate
        """
        self.mglo.build_mipmaps(base, max_level)

    def use(self, location: int = 0) -> None:
        """
        Bind the texture to a texture unit.

        The location is the texture unit we want to bind the texture.
        This should correspond with the value of the ``sampler2DArray``
        uniform in the shader because samplers read from the texture
        unit we assign to them::

            # Define what texture unit our two sampler2DArray uniforms should represent
            program['texture_a'] = 0
            program['texture_b'] = 1
            # Bind textures to the texture units
            first_texture.use(location=0)
            second_texture.use(location=1)

        Args:
            location (int): The texture location/unit.
        """
        self.mglo.use(location)

    def bind_to_image(self, unit: int, read: bool = True, write: bool = True, level: int = 0, format: int = 0) -> None:
        """
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

        Note that we bind the texture array as layered to make
        all the layers accessible. This can be updated to map
        single layers in the future.

        More information:

        - https://www.khronos.org/opengl/wiki/Image_Load_Store
        - https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindImageTexture.xhtml

        Args:
            unit (int): Specifies the index of the image unit to which to bind the texture
            texture (:py:class:`moderngl.Texture`): The texture to bind
        Keyword Args:
            read (bool): Allows the shader to read the image (default: ``True``)
            write (bool): Allows the shader to write to the image (default: ``True``)
            level (int): Level of the texture to bind (default: ``0``).
            format (int): (optional) The OpenGL enum value representing the format (defaults to the texture's format)
        """
        self.mglo.bind(unit, read, write, level, format)

    def release(self) -> None:
        """Release the ModernGL object."""
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
