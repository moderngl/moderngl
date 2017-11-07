'''
    ModernGL textures
'''

from .common import InvalidObject, TextureFilter
from .buffers import Buffer


class Texture:
    '''
        A Texture is an OpenGL object that contains one or more images that all
        have the same image format. A texture can be used in two ways. It can
        be the source of a texture access from a Shader, or it can be used
        as a render target.

        A Texture object cannot be instantiated directly, it requires a context.
        Use :py:meth:`Context.texture` or :py:meth:`Context.depth_texture`
        to create one.
    '''

    __slots__ = ['mglo']

    def __init__(self):
        self.mglo = None
        raise NotImplementedError()

    def __repr__(self):
        return '<Texture: %d>' % self.glo

    def __eq__(self, other):
        return self.mglo is other.mglo

    def __ne__(self, other):
        return self.mglo is not other.mglo

    @property
    def repeat_x(self) -> bool:
        '''
            bool: The repeat_x of the texture.
        '''

        return self.mglo.repeat_x

    @repeat_x.setter
    def repeat_x(self, value):
        self.mglo.repeat_x = value

    @property
    def repeat_y(self) -> bool:
        '''
            bool: The repeat_y of the texture.
        '''

        return self.mglo.repeat_y

    @repeat_y.setter
    def repeat_y(self, value):
        self.mglo.repeat_y = value

    @property
    def filter(self) -> TextureFilter:
        '''
            TextureFilter: The filter of the texture.
        '''

        return self.mglo.filter

    @filter.setter
    def filter(self, value):
        self.mglo.filter = value.mglo

    @property
    def swizzle(self) -> str:
        '''
            str: The swizzle of the texture.
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

        return self.mglo.width

    @property
    def height(self) -> int:
        '''
            int: The height of the texture.
        '''

        return self.mglo.height

    @property
    def size(self) -> tuple:
        '''
            tuple: The size of the texture.
        '''

        return (self.mglo.width, self.mglo.height)

    @property
    def components(self) -> int:
        '''
            int: The number of components of the texture.
        '''

        return self.mglo.components

    @property
    def samples(self) -> int:
        '''
            int: The number of samples of the texture.
        '''

        return self.mglo.samples

    @property
    def floats(self) -> bool:
        '''
            bool: Is the texture using floats?
        '''

        return self.mglo.floats

    @property
    def depth(self) -> bool:
        '''
            bool: Is the texture a depth texture?
        '''

        return self.mglo.depth

    @property
    def glo(self) -> int:
        '''
            int: The internal OpenGL object.
            This values is provided for debug purposes only.
        '''

        return self.mglo.glo

    def read(self, *, alignment=1) -> bytes:
        '''
            Read the content of the texture into a buffer.

            Keyword Args:
                alignment (int): The byte alignment of the pixels.

            Returns:
                bytes: the pixels
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
        '''

        self.mglo.build_mipmaps(base, max_level)

    def use(self, location=0) -> None:
        '''
            Bind the texture.

            Args:
                location (int): The texture location.
                    Same as the integer value that is used for sampler2D
                    uniforms in the shaders. The value ``0`` will bind the
                    texture to the ``GL_TEXTURE0`` binding point.
        '''

        self.mglo.use(location)

    def release(self) -> None:
        '''
            Release the ModernGL object.
        '''

        self.mglo.release()
        self.__class__ = InvalidObject


class Texture3D:
    '''
        A Texture is an OpenGL object that contains one or more images that all
        have the same image format. A texture can be used in two ways. It can
        be the source of a texture access from a Shader, or it can be used
        as a render target.

        A Texture3D object cannot be instantiated directly, it requires a context.
        Use :py:meth:`Context.texture3d` to create one.
    '''

    __slots__ = ['mglo']

    def __init__(self):
        self.mglo = None
        raise NotImplementedError()

    def __repr__(self):
        return '<Texture3D: %d>' % self.glo

    def __eq__(self, other):
        return self.mglo is other.mglo

    def __ne__(self, other):
        return self.mglo is not other.mglo

    @property
    def repeat_x(self) -> bool:
        '''
            bool: The repeat_x of the texture.
        '''

        return self.mglo.repeat_x

    @repeat_x.setter
    def repeat_x(self, value):
        self.mglo.repeat_x = value

    @property
    def repeat_y(self) -> bool:
        '''
            bool: The repeat_y of the texture.
        '''

        return self.mglo.repeat_y

    @repeat_y.setter
    def repeat_y(self, value):
        self.mglo.repeat_y = value

    @property
    def repeat_z(self) -> bool:
        '''
            bool: The repeat_z of the texture.
        '''

        return self.mglo.repeat_z

    @repeat_z.setter
    def repeat_z(self, value):
        self.mglo.repeat_z = value

    @property
    def filter(self) -> TextureFilter:
        '''
            TextureFilter: The filter of the texture.
        '''

        return self.mglo.filter

    @filter.setter
    def filter(self, value):
        self.mglo.filter = value.mglo

    @property
    def swizzle(self) -> str:
        '''
            str: The swizzle of the texture.
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

        return self.mglo.width

    @property
    def height(self) -> int:
        '''
            int: The height of the texture.
        '''

        return self.mglo.height

    @property
    def depth(self) -> int:
        '''
            int: The depth of the texture.
        '''

        return self.mglo.depth

    @property
    def size(self) -> tuple:
        '''
            tuple: The size of the texture.
        '''

        return (self.mglo.width, self.mglo.height, self.mglo.depth)

    @property
    def components(self) -> int:
        '''
            int: The number of components of the texture.
        '''

        return self.mglo.components

    @property
    def floats(self) -> bool:
        '''
            bool: Is the texture using floats?
        '''

        return self.mglo.floats

    @property
    def glo(self) -> int:
        '''
            int: The internal OpenGL object.
            This values is provided for debug purposes only.
        '''

        return self.mglo.glo

    def read(self, *, alignment=1) -> bytes:
        '''
            Read the content of the texture into a buffer.

            Keyword Args:
                alignment (int): The byte alignment of the pixels.

            Returns:
                bytes: the pixels
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
        self.__class__ = InvalidObject


class TextureCube:
    '''
        A Texture is an OpenGL object that contains one or more images that all
        have the same image format. A texture can be used in two ways. It can
        be the source of a texture access from a Shader, or it can be used
        as a render target.

        A Texture3D object cannot be instantiated directly, it requires a context.
        Use :py:meth:`Context.texture_cube` to create one.
    '''

    __slots__ = ['mglo']

    def __init__(self):
        self.mglo = None
        raise NotImplementedError()

    def __repr__(self):
        return '<TextureCube: %d>' % self.glo

    def __eq__(self, other):
        return self.mglo is other.mglo

    def __ne__(self, other):
        return self.mglo is not other.mglo

    @property
    def glo(self) -> int:
        '''
            int: The internal OpenGL object.
            This values is provided for debug purposes only.
        '''

        return self.mglo.glo

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
        self.__class__ = InvalidObject
