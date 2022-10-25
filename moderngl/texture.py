from typing import Any, Optional, Tuple, Union

from moderngl.mgl import InvalidObject  # type: ignore

from .buffer import Buffer

__all__ = ['Texture',
           'NEAREST', 'LINEAR', 'NEAREST_MIPMAP_NEAREST', 'LINEAR_MIPMAP_NEAREST', 'NEAREST_MIPMAP_LINEAR',
           'LINEAR_MIPMAP_LINEAR']

#: Returns the value of the texture element that is nearest
#: (in Manhattan distance) to the specified texture coordinates.
NEAREST = 0x2600
#: Returns the weighted average of the four texture elements
#: that are closest to the specified texture coordinates.
#: These can include items wrapped or repeated from other parts
#: of a texture, depending on the values of texture repeat mode,
#: and on the exact mapping.
LINEAR = 0x2601
#: Chooses the mipmap that most closely matches the size of the
#: pixel being textured and uses the ``NEAREST``` criterion (the texture
#: element closest to the specified texture coordinates) to produce
#: a texture value.
NEAREST_MIPMAP_NEAREST = 0x2700
#: Chooses the mipmap that most closely matches the size of the pixel
#: being textured and uses the ``LINEAR`` criterion (a weighted average
#: of the four texture elements that are closest to the specified
#: texture coordinates) to produce a texture value.
LINEAR_MIPMAP_NEAREST = 0x2701
#: Chooses the two mipmaps that most closely match the size of the
#: pixel being textured and uses the ``NEAREST`` criterion (the texture
#: element closest to the specified texture coordinates ) to produce
#: a texture value from each mipmap. The final texture value is a
#: weighted average of those two values.
NEAREST_MIPMAP_LINEAR = 0x2702
#: Chooses the two mipmaps that most closely match the size of the pixel
#: being textured and uses the ``LINEAR`` criterion (a weighted average
#: of the texture elements that are closest to the specified texture
#: coordinates) to produce a texture value from each mipmap.
#: The final texture value is a weighted average of those two values.
LINEAR_MIPMAP_LINEAR = 0x2703


class Texture:
    """
    A Texture is an OpenGL object that contains one or more images that all have the same image format.

    A texture can be used in two ways. It can
    be the source of a texture access from a Shader, or it can be used
    as a render target.

    A Texture object cannot be instantiated directly, it requires a context.
    Use :py:meth:`Context.texture` or :py:meth:`Context.depth_texture`
    to create one.
    """

    __slots__ = ['mglo', '_size', '_components', '_samples', '_dtype', '_depth', '_glo', 'ctx', 'extra']

    def __init__(self):
        self.mglo = None  #: Internal representation for debug purposes only.
        self._size = (None, None)
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
    def compare_func(self) -> str:
        """
        tuple: The compare function of the depth texture (Default ``'<='``).

        By default depth textures have ``GL_TEXTURE_COMPARE_MODE`` set to
        ``GL_COMPARE_REF_TO_TEXTURE``, meaning any texture lookup will
        return a depth comparison value.

        If you need to read the actual depth value in shaders, setting
        ``compare_func`` to a blank string will set ``GL_TEXTURE_COMPARE_MODE`` to
        ``GL_NONE`` making you able to read the depth texture as a ``sampler2D``::

            uniform sampler2D depth;
            out vec4 fragColor;
            in vec2 uv;

            void main() {
                float raw_depth_nonlinear = texture(depth, uv);
                fragColor = vec4(raw_depth_nonlinear);
            }

        Accepted compare functions::

            texture.compare_func = ''    # Disable depth comparison completely
            texture.compare_func = '<='  # GL_LEQUAL
            texture.compare_func = '<'   # GL_LESS
            texture.compare_func = '>='  # GL_GEQUAL
            texture.compare_func = '>'   # GL_GREATER
            texture.compare_func = '=='  # GL_EQUAL
            texture.compare_func = '!='  # GL_NOTEQUAL
            texture.compare_func = '0'   # GL_NEVER
            texture.compare_func = '1'   # GL_ALWAYS
        """
        return self.mglo.compare_func

    @compare_func.setter
    def compare_func(self, value: str) -> None:
        self.mglo.compare_func = value

    @property
    def width(self) -> int:
        """int: The width of the texture."""
        return self._size[0]

    @property
    def height(self) -> int:
        """int: The height of the texture."""
        return self._size[1]

    @property
    def size(self) -> tuple:
        """tuple: The size of the texture."""
        return self._size

    @property
    def components(self) -> int:
        """int: The number of components of the texture."""
        return self._components

    @property
    def samples(self) -> int:
        """int: The number of samples set for the texture used in multisampling."""
        return self._samples

    @property
    def dtype(self) -> str:
        """str: Data type."""
        return self._dtype

    @property
    def depth(self) -> bool:
        """bool: Is the texture a depth texture?."""
        return self._depth

    @property
    def glo(self) -> int:
        """
        int: The internal OpenGL object.

        This values is provided for debug purposes only.
        """
        return self._glo

    def read(self, *, level: int = 0, alignment: int = 1) -> bytes:
        """
        Read the pixel data as bytes into system memory.

        The texture can also be attached to a :py:class:`Framebuffer`
        to gain access to :py:meth:`Framebuffer.read` for additional
        features such ad reading a subsection or converting to
        another ``dtype``.

        Keyword Args:
            level (int): The mipmap level.
            alignment (int): The byte alignment of the pixels.

        Returns:
            bytes
        """
        return self.mglo.read(level, alignment)

    def read_into(
        self,
        buffer: Any,
        *,
        level: int = 0,
        alignment: int = 1,
        write_offset: int = 0,
    ) -> None:
        """
        Read the content of the texture into a bytearray or :py:class:`~moderngl.Buffer`.

        The advantage of reading into a :py:class:`~moderngl.Buffer` is that pixel data
        does not need to travel all the way to system memory::

            # Reading pixel data into a bytearray
            data = bytearray(4)
            texture = ctx.texture((2, 2), 1)
            texture.read_into(data)

            # Reading pixel data into a buffer
            data = ctx.buffer(reserve=4)
            texture = ctx.texture((2, 2), 1)
            texture.read_into(data)

        Args:
            buffer (Union[bytearray, Buffer]): The buffer that will receive the pixels.

        Keyword Args:
            level (int): The mipmap level.
            alignment (int): The byte alignment of the pixels.
            write_offset (int): The write offset.
        """
        if type(buffer) is Buffer:
            buffer = buffer.mglo

        return self.mglo.read_into(buffer, level, alignment, write_offset)

    def write(
        self,
        data: Any,
        viewport: Optional[Union[Tuple[int, int], Tuple[int, int, int, int]]] = None,
        *,
        level: int = 0,
        alignment: int = 1,
    ) -> None:
        r"""
        Update the content of the texture from byte data or a moderngl :py:class:`~moderngl.Buffer`.

        Examples::

            # Write data from a moderngl Buffer
            data = ctx.buffer(reserve=4)
            texture = ctx.texture((2, 2), 1)
            texture.write(data)

            # Write data from bytes
            data = b'\xff\xff\xff\xff'
            texture = ctx.texture((2, 2), 1)
            texture.write(data)

            # Write to a sub-section of the texture using viewport
            texture = ctx.texture((100, 100), 4)
            # Fill the lower left 50x50 pixels with new data
            texture.write(data, viewport=(0, 0, 50, 50))

        Args:
            data (Union[bytes, Buffer]): The pixel data.
            viewport (tuple): The sub-section of the texture to update
                                in viewport coordinates. The data size
                                must match the size of the area.

        Keyword Args:
            level (int): The mipmap level.
            alignment (int): The byte alignment of the pixels.
        """
        if type(data) is Buffer:
            data = data.mglo

        self.mglo.write(data, viewport, level, alignment)

    def build_mipmaps(self, base: int = 0, max_level: int = 1000) -> None:
        """
        Generate mipmaps.

        This also changes the texture filter to ``LINEAR_MIPMAP_LINEAR, LINEAR``

        Keyword Args:
            base (int): The base level
            max_level (int): The maximum levels to generate
        """
        self.mglo.build_mipmaps(base, max_level)

    def use(self, location: int = 0) -> None:
        """
        Bind the texture to a texture unit.

        The location is the texture unit we want to bind the texture.
        This should correspond with the value of the ``sampler2D``
        uniform in the shader because samplers read from the texture
        unit we assign to them::

            # Define what texture unit our two sampler2D uniforms should represent
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
