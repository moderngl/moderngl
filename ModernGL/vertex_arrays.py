'''
    ModernGL vertex arrays
'''

# pylint: disable=too-few-public-methods

from typing import Tuple

from .common import InvalidObject, TRIANGLES, POINTS
from .programs import Program
from .buffers import Buffer


class VertexArrayAttribute:
    '''
        VertexArrayAttribute
    '''

    __slots__ = ['mglo']

    @staticmethod
    def new(obj):
        '''
            For internal use only.
        '''

        res = VertexArrayAttribute.__new__(VertexArrayAttribute)
        res.mglo = obj
        return res

    def __init__(self):
        self.mglo = None
        raise NotImplementedError()

    def __getitem__(self, key) -> 'VertexArrayAttribute':
        return VertexArrayAttribute.new(self.mglo[key])

    @property
    def location(self):
        '''
            location
        '''

        return self.mglo.location

    @property
    def divisor(self):
        '''
            divisor
        '''

        return self.mglo.divisor

    @property
    def stride(self):
        '''
            stride
        '''

        return self.mglo.stride

    @property
    def enabled(self):
        '''
            enabled
        '''

        return self.mglo.enabled

    @enabled.setter
    def enabled(self, value):
        self.mglo.enabled = value

    def bind(self, buffer, offset=0, stride=0, divisor=0) -> None:
        '''
            bind
        '''

        self.mglo.bind(buffer.mglo, offset, stride, divisor)

    def enable(self) -> None:
        '''
            enable
        '''

        self.mglo.enable()

    def disable(self) -> None:
        '''
            disable
        '''

        self.mglo.disable()


class VertexArrayAttributeMap:
    '''
        VertexArrayAttributeMap
    '''

    __slots__ = ['mglo', '_map']

    @staticmethod
    def new(obj):
        '''
            For internal use only.
        '''

        res = VertexArrayAttributeMap.__new__(VertexArrayAttributeMap)
        res.mglo = obj
        return res


    def __init__(self):
        self.mglo = None
        self._map = {}
        raise NotImplementedError()

    def __getitem__(self, key) -> VertexArrayAttribute:
        return VertexArrayAttribute.new(self.mglo[key])

    def __contains__(self, key):
        return key in self.mglo

    def __iter__(self):
        yield from self.mglo.items()

    def __len__(self):
        return len(self.mglo)

    def __repr__(self):
        return repr(self.mglo)

    def __eq__(self, other):
        return self.mglo is other.mglo

    def __ne__(self, other):
        return self.mglo is not other.mglo


class VertexArray:
    '''
        A VertexArray object is an OpenGL object that stores all of the state
        needed to supply vertex data. It stores the format of the vertex data
        as well as the Buffer objects providing the vertex data arrays.

        In ModernGL, the VertexArray object also stores a reference
        for a :py:class:`Program` object, and some Subroutine information.

        A VertexArray object cannot be instantiated directly, it requires a context.
        Use :py:meth:`Context.vertex_array` or :py:meth:`Context.simple_vertex_array`
        to create one.
    '''

    __slots__ = ['mglo', '_program', '_index_buffer']

    def __init__(self):
        self.mglo = None
        self._program = None
        self._index_buffer = None
        raise NotImplementedError()

    def __repr__(self):
        return '<VertexArray: %d>' % self.glo

    def __eq__(self, other):
        return self.mglo is other.mglo

    def __ne__(self, other):
        return self.mglo is not other.mglo

    @property
    def program(self) -> Program:
        '''
            Program: The program assinged to the VertexArray.
            The program used when rendering or transforming primitives.
        '''

        return self._program

    @property
    def attributes(self) -> VertexArrayAttributeMap:
        '''
            VertexArrayAttributeMap: Individual vertex attributes.
            Use the :py:meth:`VertexArrayAttribute.bind` method
            to assign vertex attributes to buffers.
        '''

        return VertexArrayAttributeMap.new(self.mglo.attributes)

    @property
    def index_buffer(self) -> Buffer:
        '''
            Buffer: The index buffer if the index_buffer is set, otherwise ``None``.
        '''

        return self._index_buffer

    @property
    def vertices(self) -> int:
        '''
            int: The number of vertices detected.
            This is the minimum of the number of vertices possible per Buffer.
            The size of the index_buffer determines the number of vertices.
            Per instance vertex attributes does not affect this number.
        '''

        return self.mglo.vertices

    @property
    def subroutines(self) -> Tuple[int]:
        '''
            tuple: The subroutines assinged to the VertexArray.
            The subroutines used when rendering or transforming primitives.
        '''

        return self.mglo.subroutines

    @subroutines.setter
    def subroutines(self, value):
        self.mglo.subroutines = tuple(value)

    @property
    def glo(self) -> int:
        '''
            int: The internal OpenGL object.
            This values is provided for debug purposes only.
        '''

        return self.mglo.glo

    def render(self, mode=TRIANGLES, vertices=-1, *, first=0, instances=1) -> None:
        '''
            The render primitive (mode) must be the same as
            the input primitive of the GeometryShader.

            Args:
                mode (Primitive): By default `TRIANGLES` will be used.
                vertices (int): The number of vertices to transform.

            Keyword Args:
                first (int): The index of the first vertex to start with.
                instances (int): The number of instances.
        '''

        self.mglo.render(mode.mglo, vertices, first, instances)

    def transform(self, buffer, mode=POINTS, vertices=-1, *, first=0, instances=1) -> None:
        '''
            Transform vertices.
            Stores the output in a single buffer.
            The transform primitive (mode) must be the same as
            the input primitive of the GeometryShader.

            Args:
                buffer (Buffer): The buffer to store the output.
                mode (Primitive): By default `TRIANGLES` will be used.
                vertices (int): The number of vertices to transform.

            Keyword Args:
                first (int): The index of the first vertex to start with.
                instances (int): The number of instances.
        '''

        self.mglo.transform(buffer.mglo, mode.mglo, vertices, first, instances)

    def release(self) -> None:
        '''
            Release the ModernGL object.
        '''

        self.mglo.release()
        self.__class__ = InvalidObject
