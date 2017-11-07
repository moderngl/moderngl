'''
    ModernGL renderbuffers
'''

from .common import InvalidObject


class Renderbuffer:
    '''
        Renderbuffer objects are OpenGL objects that contain images.
        They are created and used specifically with :py:class:`Framebuffer` objects.
        They are optimized for use as render targets, while :py:class:`Texture` objects
        may not be, and are the logical choice when you do not need to sample
        from the produced image. If you need to resample, use Textures instead.
        Renderbuffer objects also natively accommodate multisampling.

        A Renderbuffer object cannot be instantiated directly, it requires a context.
        Use :py:meth:`Context.renderbuffer` or :py:meth:`Context.depth_renderbuffer`
        to create one.
    '''

    __slots__ = ['mglo']

    def __init__(self):
        self.mglo = None
        raise NotImplementedError()

    def __repr__(self):
        return '<Renderbuffer: %d>' % self.glo

    def __eq__(self, other):
        return type(self) is type(other) and self.mglo is other.mglo

    def __ne__(self, other):
        return type(self) is not type(other) or self.mglo is not other.mglo

    @property
    def width(self) -> int:
        '''
            int: The width of the renderbuffer.
        '''

        return self.mglo.width

    @property
    def height(self) -> int:
        '''
            int: The height of the renderbuffer.
        '''

        return self.mglo.height

    @property
    def size(self) -> tuple:
        '''
            tuple: The size of the renderbuffer.
        '''

        return (self.mglo.width, self.mglo.height)

    @property
    def samples(self) -> int:
        '''
            int: The samples of the renderbuffer.
        '''

        return self.mglo.samples

    @property
    def components(self) -> int:
        '''
            int: The components of the renderbuffer.
        '''

        return self.mglo.components

    @property
    def depth(self) -> bool:
        '''
            bool: Is the renderbuffer a depth renderbuffer?
        '''

        return self.mglo.depth

    @property
    def glo(self) -> int:
        '''
            int: The internal OpenGL object.
            This values is provided for debug purposes only.
        '''

        return self.mglo.glo

    def release(self) -> None:
        '''
            Release the ModernGL object.
        '''

        self.mglo.release()
        self.__class__ = InvalidObject
