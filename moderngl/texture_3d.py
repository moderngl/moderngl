from typing import Tuple

from .buffer import Buffer

__all__ = ['Texture3D']


class Texture3D:
    '''
        A Texture is an OpenGL object that contains one or more images that all
        have the same image format. A texture can be used in two ways. It can
        be the source of a texture access from a Shader, or it can be used
        as a render target.

        A Texture3D object cannot be instantiated directly, it requires a context.
        Use :py:meth:`Context.texture3d` to create one.
    '''

    __slots__ = ['mglo', '_size', '_components', '_samples', '_dtype', '_glo', 'ctx', 'extra']

    def __init__(self):
        self.mglo = None  #: Internal representation for debug purposes only.
        self._size = (None, None, None)
        self._components = None
        self._samples = None
        self._dtype = None
        self._glo = None
        self.ctx = None  #: The context this object belongs to
        self.extra = None  #: Any - Attribute for storing user defined objects
        raise TypeError()

    def __repr__(self):
        return '<Texture3D: %d>' % self.glo

    def __eq__(self, other):
        return type(self) is type(other) and self.mglo is other.mglo

    @property
    def repeat_x(self) -> bool:
        '''
            bool: The x repeat flag for the texture (Default ``True``)

            Example::

                # Enable texture repeat (GL_REPEAT)
                texture.repeat_x = True

                # Disable texture repeat (GL_CLAMP_TO_EDGE)
                texture.repeat_x = False
        '''

        return self.mglo.repeat_x

    @repeat_x.setter
    def repeat_x(self, value):
        self.mglo.repeat_x = value

    @property
    def repeat_y(self) -> bool:
        '''
            bool: The y repeat flag for the texture (Default ``True``)

            Example::

                # Enable texture repeat (GL_REPEAT)
                texture.repeat_y = True

                # Disable texture repeat (GL_CLAMP_TO_EDGE)
                texture.repeat_y = False
        '''

        return self.mglo.repeat_y

    @repeat_y.setter
    def repeat_y(self, value):
        self.mglo.repeat_y = value

    @property
    def repeat_z(self) -> bool:
        '''
            bool: The z repeat flag for the texture (Default ``True``)

            Example::

                # Enable texture repeat (GL_REPEAT)
                texture.repeat_z = True

                # Disable texture repeat (GL_CLAMP_TO_EDGE)
                texture.repeat_z = False
        '''

        return self.mglo.repeat_z

    @repeat_z.setter
    def repeat_z(self, value):
        self.mglo.repeat_z = value

    @property
    def filter(self) -> Tuple[int, int]:
        '''
            tuple: The filter of the texture.
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
    def width(self) -> int:
        '''
            int: The width of the texture.
        '''

        return self._size[0]

    @property
    def height(self) -> int:
        '''
            int: The height of the texture.
        '''

        return self._size[1]

    @property
    def depth(self) -> int:
        '''
            int: The depth of the texture.
        '''

        return self._size[2]

    @property
    def size(self) -> tuple:
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
    def glo(self) -> int:
        '''
            int: The internal OpenGL object.
            This values is provided for debug purposes only.
        '''

        return self._glo

    def read(self, *, alignment=1) -> bytes:
        '''
            Read the content of the texture into a buffer.

            Keyword Args:
                alignment (int): The byte alignment of the pixels.

            Returns:
                bytes
        '''

        return self.mglo.read(alignment)

    def read_into(self, buffer, *, alignment=1, write_offset=0) -> None:
        '''
            Read the content of the texture into a buffer.

            Args:
                buffer (bytearray): The buffer that will receive the pixels.

            Keyword Args:
                alignment (int): The byte alignment of the pixels.
                write_offset (int): The write offset.
        '''

        if type(buffer) is Buffer:
            buffer = buffer.mglo

        return self.mglo.read_into(buffer, alignment, write_offset)

    def write(self, data, viewport=None, *, alignment=1) -> None:
        '''
            Update the content of the texture.

            Args:
                data (bytes): The pixel data.
                viewport (tuple): The viewport.

            Keyword Args:
                alignment (int): The byte alignment of the pixels.
        '''

        if type(data) is Buffer:
            data = data.mglo

        self.mglo.write(data, viewport, alignment)

    def build_mipmaps(self, base=0, max_level=1000) -> None:
        '''
            Generate mipmaps.

            This also changes the texture filter to ``LINEAR_MIPMAP_LINEAR, LINEAR``
            (Will be removed in ``6.x``)

            Keyword Args:
                base (int): The base level
                max_level (int): The maximum levels to generate
        '''

        self.mglo.build_mipmaps(base, max_level)

    def use(self, location=0) -> None:
        '''
            Bind the texture.

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

    def transfer(self, context) -> 'Texture3D':
        '''
            share the texture to a new context

            Args:
                context (:py:class:`Context`): shared context
            Returns:
                :py:class:`Texture3D` object
        '''
        res = Texture3D.__new__(Texture3D)
        res.mglo, res._glo = self.mglo.transfer(context.mglo)
        res._size = self._size
        res._components = self._components
        res._samples = self._samples
        res._dtype = self._dtype
        res._depth = self._depth
        res.ctx = context
        res.extra = self.extra
        return res
