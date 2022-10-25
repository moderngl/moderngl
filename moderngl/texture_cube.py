from typing import Any, Optional, Tuple, Union

from moderngl.mgl import InvalidObject  # type: ignore

from .buffer import Buffer

__all__ = ['TextureCube']


class TextureCube:
    """
    Cubemaps are a texture using the type GL_TEXTURE_CUBE_MAP.

    They are similar to 2D textures in that they have two dimensions.
    However, each mipmap level has 6 faces, with each face having the
    same size as the other faces.

    The width and height of a cubemap must be the same (ie: cubemaps are squares),
    but these sizes need not be powers of two.

    .. Note:: ModernGL enables ``GL_TEXTURE_CUBE_MAP_SEAMLESS`` globally
                to ensure filtering will be done across the cube faces.

    A Texture3D object cannot be instantiated directly, it requires a context.
    Use :py:meth:`Context.texture_cube` to create one.
    """

    __slots__ = ['mglo', '_size', '_components', '_dtype', '_glo', 'ctx', 'extra']

    def __init__(self):
        self.mglo = None  #: Internal representation for debug purposes only.
        self._size = (None, None)
        self._components = None
        self._dtype = None
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

    def __del__(self) -> None:
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    @property
    def size(self) -> Tuple[int, int]:
        """tuple: The size of the texture cube (single face)."""
        return self._size

    @property
    def components(self) -> int:
        """int: The number of components of the texture."""
        return self._components

    @property
    def dtype(self) -> str:
        """str: Data type."""
        return self._dtype

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
    def glo(self) -> int:
        """
        int: The internal OpenGL object.

        This values is provided for debug purposes only.
        """
        return self._glo

    def read(self, face: int, *, alignment: int = 1) -> bytes:
        """
        Read a face from the cubemap as bytes into system memory.

        Face values are::

            0: Positive X
            1: Negative X
            2: Positive Y
            3: Negative Y
            4: Positive Z
            5: Negative Z

        Args:
            face (int): The face to read.

        Keyword Args:
            alignment (int): The byte alignment of the pixels.
        """
        return self.mglo.read(face, alignment)

    def read_into(
        self,
        buffer: Any,
        face: int,
        *,
        alignment: int = 1,
        write_offset: int = 0,
    ) -> None:
        """
        Read a face from the cubemap texture.

        Read a face of the cubemap into a bytearray or :py:class:`~moderngl.Buffer`.
        The advantage of reading into a :py:class:`~moderngl.Buffer` is that pixel data
        does not need to travel all the way to system memory::

            # Reading pixel data into a bytearray
            data = bytearray(4)
            texture = ctx.texture_cube((2, 2), 1)
            texture.read_into(data, 0)

            # Reading pixel data into a buffer
            data = ctx.buffer(reserve=4)
            texture = ctx.texture_cube((2, 2), 1)
            texture.read_into(data, 0)

        Args:
            buffer (bytearray): The buffer that will receive the pixels.
            face (int): The face to read.

        Keyword Args:
            alignment (int): The byte alignment of the pixels.
            write_offset (int): The write offset.
        """
        if type(buffer) is Buffer:
            buffer = buffer.mglo

        return self.mglo.read_into(buffer, face, alignment, write_offset)

    def write(
        self,
        face: int,
        data: Any,
        viewport: Optional[Union[Tuple[int, int], Tuple[int, int, int, int]]] = None,
        *,
        alignment: int = 1,
    ) -> None:
        r"""
        Update the content of the texture.

        Update the content of a face in the cubemap from byte data
        or a moderngl :py:class:`~moderngl.Buffer`::

            # Write data from a moderngl Buffer
            data = ctx.buffer(reserve=4)
            texture = ctx.texture_cube((2, 2), 1)
            texture.write(0, data)

            # Write data from bytes
            data = b'\xff\xff\xff\xff'
            texture = ctx.texture_cube((2, 2), 1)
            texture.write(0, data)

        Args:
            face (int): The face to update.
            data (bytes): The pixel data.
            viewport (tuple): The viewport.

        Keyword Args:
            alignment (int): The byte alignment of the pixels.
        """
        if type(data) is Buffer:
            data = data.mglo

        self.mglo.write(face, data, viewport, alignment)

    def use(self, location: int = 0) -> None:
        """
        Bind the texture to a texture unit.

        The location is the texture unit we want to bind the texture.
        This should correspond with the value of the ``samplerCube``
        uniform in the shader because samplers read from the texture
        unit we assign to them::

            # Define what texture unit our two samplerCube uniforms should represent
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

        Note that we bind the texture cube as layered to make
        all the faces accessible. This can be updated to map
        single faces in the future. The Z component in imageLoad/Store
        represents the face id we are writing to (0-5).

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
