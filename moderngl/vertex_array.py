from typing import Tuple

__all__ = ['VertexArray',
           'POINTS', 'LINES', 'LINE_LOOP', 'LINE_STRIP', 'TRIANGLES', 'TRIANGLE_STRIP', 'TRIANGLE_FAN',
           'LINES_ADJACENCY', 'LINE_STRIP_ADJACENCY', 'TRIANGLES_ADJACENCY', 'TRIANGLE_STRIP_ADJACENCY', 'PATCHES']


POINTS = 0x0000
LINES = 0x0001
LINE_LOOP = 0x0002
LINE_STRIP = 0x0003
TRIANGLES = 0x0004
TRIANGLE_STRIP = 0x0005
TRIANGLE_FAN = 0x0006
LINES_ADJACENCY = 0x000A
LINE_STRIP_ADJACENCY = 0x000B
TRIANGLES_ADJACENCY = 0x000C
TRIANGLE_STRIP_ADJACENCY = 0x000D
PATCHES = 0x000E


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

    __slots__ = ['mglo', '_program', '_buffers', '_index_buffer', '_index_element_size', '_glo', 'ctx', 'extra']

    def __init__(self):
        self.mglo = None
        self._program = None
        self._buffers = None
        self._index_buffer = None
        self._index_element_size = None
        self._glo = None
        self.ctx = None
        self.extra = None  #: Any - Attribute for storing user defined objects
        raise TypeError()

    def __repr__(self):
        return '<VertexArray: %d>' % self.glo

    def __eq__(self, other):
        return type(self) is type(other) and self.mglo is other.mglo

    @property
    def program(self) -> 'Program':
        '''
            Program: The program assinged to the VertexArray.
            The program used when rendering or transforming primitives.
        '''

        return self._program

    @property
    def buffers(self) -> 'list':
        '''
           list: List of vbo Buffers assigned to this vao object.
        '''
           
        return self._buffers

    @property
    def index_buffer(self) -> 'Buffer':
        '''
            Buffer: The index buffer if the index_buffer is set, otherwise ``None``.
        '''

        return self._index_buffer

    @property
    def index_element_size(self) -> int:
        '''
            int: The byte size of each element in the index buffer
        '''
        return self._index_element_size

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
    def subroutines(self) -> Tuple[int, ...]:
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

        return self._glo

    def render(self, mode=None, vertices=-1, *, first=0, instances=1) -> None:
        '''
            The render primitive (mode) must be the same as
            the input primitive of the GeometryShader.

            Args:
                mode (int): By default :py:data:`TRIANGLES` will be used.
                vertices (int): The number of vertices to transform.

            Keyword Args:
                first (int): The index of the first vertex to start with.
                instances (int): The number of instances.
        '''

        if mode is None:
            mode = TRIANGLES

        self.mglo.render(mode, vertices, first, instances)

    def render_indirect(self, buffer, mode=None, count=-1, *, first=0) -> None:
        '''
            The render primitive (mode) must be the same as
            the input primitive of the GeometryShader.

            The draw commands are 5 integers: (count, instanceCount, firstIndex, baseVertex, baseInstance).

            Args:
                buffer (Buffer): Indirect drawing commands.
                mode (int): By default :py:data:`TRIANGLES` will be used.
                count (int): The number of draws.

            Keyword Args:
                first (int): The index of the first indirect draw command.
        '''

        if mode is None:
            mode = TRIANGLES

        self.mglo.render_indirect(buffer.mglo, mode, count, first)

    def transform(self, buffer, mode=None, vertices=-1, *, first=0, instances=1) -> None:
        '''
            Transform vertices.
            Stores the output in a single buffer.
            The transform primitive (mode) must be the same as
            the input primitive of the GeometryShader.

            Args:
                buffer (Buffer): The buffer to store the output.
                mode (int): By default :py:data:`POINTS` will be used.
                vertices (int): The number of vertices to transform.

            Keyword Args:
                first (int): The index of the first vertex to start with.
                instances (int): The number of instances.
        '''

        if mode is None:
            mode = POINTS

        self.mglo.transform(buffer.mglo, mode, vertices, first, instances)

    def bind(self, attribute, cls, buffer, fmt, *, offset=0, stride=0, divisor=0, normalize=False) -> None:
        '''
            Bind individual attributes to buffers.

            Args:
                location (int): The attribute location.
                cls (str): The attribute class. Valid values are ``f``, ``i`` or ``d``.
                buffer (Buffer): The buffer.
                format (str): The buffer format.

            Keyword Args:
                offset (int): The offset.
                stride (int): The stride.
                divisor (int): The divisor.
                normalize (bool): The normalize parameter, if applicable.
        '''

        self.mglo.bind(attribute, cls, buffer.mglo, fmt, offset, stride, divisor, normalize)

    def release(self) -> None:
        '''
            Release the ModernGL object.
        '''

        self.mglo.release()
