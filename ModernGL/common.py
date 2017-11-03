'''
    ModernGL common
'''

# pylint: disable=too-few-public-methods

try:
    from . import mgl
except ImportError:
    from .mock import mgl


class InvalidObject:
    '''
        A ModernGL object turns into an InvalidObject
        once the ``release()`` method is successfully called.
    '''

    __slots__ = ['mglo']


class EnableFlag:
    '''
        EnableFlag
    '''

    __slots__ = ['flags']

    @staticmethod
    def new(flags) -> 'EnableFlag':
        '''
            For internal use only.
        '''

        res = EnableFlag.__new__(EnableFlag)
        res.flags = flags
        return res

    def __init__(self):
        self.flags = 0
        raise NotImplementedError('EnableFlag')

    def __or__(self, other) -> 'EnableFlag':
        return EnableFlag.new(self.flags | other.flags)

    def __and__(self, other) -> 'EnableFlag':
        return EnableFlag.new(self.flags & other.flags)

    def __inv__(self) -> 'EnableFlag':
        return EnableFlag.new(mgl.ENABLE_MASK & ~self.flags)

    def __repr__(self):
        return '<ModernGL.EnableFlag>'

    def __eq__(self, other):
        return self.flags == other.flags

    def __ne__(self, other):
        return self.flags != other.flags


BLEND = EnableFlag.new(mgl.BLEND)
'''
    GL_BLEND
'''

DEPTH_TEST = EnableFlag.new(mgl.DEPTH_TEST)
'''
    GL_DEPTH_TEST
'''

CULL_FACE = EnableFlag.new(mgl.CULL_FACE)
'''
    GL_CULL_FACE
'''


class Primitive:
    '''
        Primitive
    '''

    __slots__ = ['mglo', 'name']

    @staticmethod
    def new(obj, name) -> 'Primitive':
        '''
            For internal use only.
        '''

        res = Primitive.__new__(Primitive)
        obj.wrapper = res
        res.name = name
        res.mglo = obj
        return res

    def __init__(self):
        self.name = None
        self.mglo = None
        raise NotImplementedError('Primitive')

    def __repr__(self):
        return 'ModernGL.%s' % self.name

    def __eq__(self, other):
        return self.mglo is other.mglo

    def __ne__(self, other):
        return self.mglo is not other.mglo


TRIANGLES = Primitive.new(mgl.TRIANGLES, 'TRIANGLES')
'''
    GL_TRIANGLES
'''

TRIANGLE_STRIP = Primitive.new(mgl.TRIANGLE_STRIP, 'TRIANGLE_STRIP')
'''
    GL_TRIANGLE_STRIP
'''

TRIANGLE_FAN = Primitive.new(mgl.TRIANGLE_FAN, 'TRIANGLE_FAN')
'''
    GL_TRIANGLE_FAN
'''

LINES = Primitive.new(mgl.LINES, 'LINES')
'''
    GL_LINES
'''

LINE_STRIP = Primitive.new(mgl.LINE_STRIP, 'LINE_STRIP')
'''
    GL_LINE_STRIP
'''

LINE_LOOP = Primitive.new(mgl.LINE_LOOP, 'LINE_LOOP')
'''
    GL_LINE_LOOP
'''

POINTS = Primitive.new(mgl.POINTS, 'POINTS')
'''
    GL_POINTS
'''

LINE_STRIP_ADJACENCY = Primitive.new(mgl.LINE_STRIP_ADJACENCY, 'LINE_STRIP_ADJACENCY')
'''
    GL_LINE_STRIP_ADJACENCY
'''

LINES_ADJACENCY = Primitive.new(mgl.LINES_ADJACENCY, 'LINES_ADJACENCY')
'''
    GL_LINES_ADJACENCY
'''

TRIANGLE_STRIP_ADJACENCY = Primitive.new(mgl.TRIANGLE_STRIP_ADJACENCY, 'TRIANGLE_STRIP_ADJACENCY')
'''
    GL_TRIANGLE_STRIP_ADJACENCY
'''

TRIANGLES_ADJACENCY = Primitive.new(mgl.TRIANGLES_ADJACENCY, 'TRIANGLES_ADJACENCY')
'''
    GL_TRIANGLES_ADJACENCY
'''


class TextureFilter:
    '''
        TextureFilter
    '''

    __slots__ = ['mglo', 'name']

    @staticmethod
    def new(obj, name) -> 'TextureFilter':
        '''
            For internal use only.
        '''

        res = TextureFilter.__new__(TextureFilter)
        obj.wrapper = res
        res.name = name
        res.mglo = obj
        return res

    def __init__(self):
        self.name = None
        self.mglo = None
        raise NotImplementedError('TextureFilter')

    def __repr__(self):
        return 'ModernGL.%s' % self.name

    def __eq__(self, other):
        return self.mglo is other.mglo

    def __ne__(self, other):
        return self.mglo is not other.mglo


LINEAR = TextureFilter.new(mgl.LINEAR, 'LINEAR')
'''
    (GL_LINEAR, GL_LINEAR)
'''

NEAREST = TextureFilter.new(mgl.NEAREST, 'NEAREST')
'''
    (GL_NEAREST, GL_NEAREST)
'''

LINEAR_MIPMAP = TextureFilter.new(mgl.LINEAR_MIPMAP, 'LINEAR_MIPMAP')
'''
    (GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR)
'''

NEAREST_MIPMAP = TextureFilter.new(mgl.NEAREST_MIPMAP, 'NEAREST_MIPMAP')
'''
    (GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST)
'''

MIPMAP = LINEAR_MIPMAP
