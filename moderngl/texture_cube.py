from .buffer import Buffer

__all__ = ['TextureCube']


class TextureCube:
    '''
        A Texture is an OpenGL object that contains one or more images that all
        have the same image format. A texture can be used in two ways. It can
        be the source of a texture access from a Shader, or it can be used
        as a render target.

        .. Note:: ModernGL enables ``GL_TEXTURE_CUBE_MAP_SEAMLESS`` globally
                  to ensure filtering will be done across the cube faces.

        A Texture3D object cannot be instantiated directly, it requires a context.
        Use :py:meth:`Context.texture_cube` to create one.
    '''

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
        return '<TextureCube: %d>' % self.glo

    def __eq__(self, other):
        return type(self) is type(other) and self.mglo is other.mglo

    @property
    def size(self):
        '''
            tuple: The size of the texture.
        '''
        return self._size

    @property
    def components(self) -> int:
        '''
            int: The number of components of the texture.
        '''
        return self._components

    @property
    def dtype(self) -> str:
        '''
            str: Data type.
        '''

        return self._dtype

    @property
    def filter(self):
        '''
            tuple: The minification and magnification filter for the texture.
            (Default ``(moderngl.LINEAR. moderngl.LINEAR)``)

            Example::

                texture.filter == (moderngl.NEAREST, moderngl.NEAREST)
                texture.filter == (moderngl.LINEAR_MIPMAP_LINEAR, moderngl.LINEAR)
                texture.filter == (moderngl.NEAREST_MIPMAP_LINEAR, moderngl.NEAREST)
                texture.filter == (moderngl.LINEAR_MIPMAP_NEAREST, moderngl.NEAREST)
        '''
        return self.mglo.filter

    @filter.setter
    def filter(self, value):
        self.mglo.filter = value

    @property
    def swizzle(self) -> str:
        '''
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
        '''

        return self.mglo.swizzle

    @swizzle.setter
    def swizzle(self, value):
        self.mglo.swizzle = value

    @property
    def anisotropy(self):
        '''
            float: Number of samples for anisotropic filtering (Default ``1.0``).
            The value will be clamped in range ``1.0`` and ``ctx.max_anisotropy``.

            Any value greater than 1.0 counts as a use of anisotropic filtering::

                # Disable anisotropic filtering
                texture.anisotropy = 1.0

                # Enable anisotropic filtering suggesting 16 samples as a maximum
                texture.anisotropy = 16.0
        '''
        return self.mglo.anisotropy

    @anisotropy.setter
    def anisotropy(self, value):
        self.mglo.anisotropy = value

    @property
    def glo(self) -> int:
        '''
            int: The internal OpenGL object.
            This values is provided for debug purposes only.
        '''

        return self._glo

    def read(self, face, *, alignment=1) -> bytes:
        '''
            Read a face from the cubemap texture.

            Args:
                face (int): The face to read.

            Keyword Args:
                alignment (int): The byte alignment of the pixels.
        '''

        return self.mglo.read(face, alignment)

    def read_into(self, buffer, face, *, alignment=1, write_offset=0) -> None:
        '''
            Read a face from the cubemap texture.

            Args:
                buffer (bytearray): The buffer that will receive the pixels.
                face (int): The face to read.

            Keyword Args:
                alignment (int): The byte alignment of the pixels.
                write_offset (int): The write offset.
        '''

        if type(buffer) is Buffer:
            buffer = buffer.mglo

        return self.mglo.read_into(buffer, face, alignment, write_offset)

    def write(self, face, data, viewport=None, *, alignment=1) -> None:
        '''
            Update the content of the texture.

            Args:
                face (int): The face to update.
                data (bytes): The pixel data.
                viewport (tuple): The viewport.

            Keyword Args:
                alignment (int): The byte alignment of the pixels.
        '''

        if type(data) is Buffer:
            data = data.mglo

        self.mglo.write(face, data, viewport, alignment)

    def use(self, location=0) -> None:
        '''
            Bind the cubemap texture.

            Args:
                location (int): The texture location.
                    Same as the integer value that is used for sampler3D
                    uniforms in the shaders. The value ``0`` will bind the
                    texture to the ``GL_TEXTURE0`` binding point.
        '''

        self.mglo.use(location)

    def release(self) -> None:
        '''
            Release the ModernGL object.
        '''

        self.mglo.release()

    def transfer(self, context) -> 'TextureCube':
        '''
            share the texure to a new context

            Args:
                context (:py:class:`TextureCube`): shared context
            Returns:
                :py:class:`TextureCube` object
        '''
        res = TextureCube.__new__(TextureCube)
        res.mglo, res._glo = self.mglo.transfer(context.mglo)
        res._size = self._size
        res._components = self._components
        res._samples = self._samples
        res._dtype = self._dtype
        res._depth = self._depth
        res.ctx = context
        res.extra = self.extra
        return res
