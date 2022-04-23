from typing import TYPE_CHECKING, Any, List, Optional, Tuple, Union

from moderngl.mgl import InvalidObject  # type: ignore

if TYPE_CHECKING:
    from .program import Program
    from .buffer import Buffer

__all__ = ['VertexArray',
           'POINTS', 'LINES', 'LINE_LOOP', 'LINE_STRIP', 'TRIANGLES', 'TRIANGLE_STRIP', 'TRIANGLE_FAN',
           'LINES_ADJACENCY', 'LINE_STRIP_ADJACENCY', 'TRIANGLES_ADJACENCY', 'TRIANGLE_STRIP_ADJACENCY', 'PATCHES']

#: Each vertex represents a point
POINTS = 0x0000
#: Vertices 0 and 1 are considered a line. Vertices 2 and 3 are considered a line.
#: And so on. If the user specifies a non-even number of vertices, then the extra vertex is ignored.
LINES = 0x0001
#: As line strips, except that the first and last vertices are also used as a line.
#: Thus, you get n lines for n input vertices. If the user only specifies 1 vertex,
#: the drawing command is ignored. The line between the first and last vertices happens
#: after all of the previous lines in the sequence.
LINE_LOOP = 0x0002
#: The adjacent vertices are considered lines. Thus, if you pass n vertices, you will get n-1 lines.
#: If the user only specifies 1 vertex, the drawing command is ignored.
LINE_STRIP = 0x0003
#: Vertices 0, 1, and 2 form a triangle. Vertices 3, 4, and 5 form a triangle. And so on.
TRIANGLES = 0x0004
#: Every group of 3 adjacent vertices forms a triangle. The face direction of the
#: strip is determined by the winding of the first triangle. Each successive triangle
#: will have its effective face order reversed, so the system compensates for that
#: by testing it in the opposite way. A vertex stream of n length will generate n-2 triangles.
TRIANGLE_STRIP = 0x0005
#: The first vertex is always held fixed. From there on, every group of 2 adjacent
#: vertices form a triangle with the first. So with a vertex stream, you get a list
#: of triangles like so: (0, 1, 2) (0, 2, 3), (0, 3, 4), etc. A vertex stream of
#: n length will generate n-2 triangles.
TRIANGLE_FAN = 0x0006
#: These are special primitives that are expected to be used specifically with
#: geomtry shaders. These primitives give the geometry shader more vertices
#: to work with for each input primitive. Data needs to be duplicated in buffers.
LINES_ADJACENCY = 0x000A
#: These are special primitives that are expected to be used specifically with
#: geomtry shaders. These primitives give the geometry shader more vertices
#: to work with for each input primitive. Data needs to be duplicated in buffers.
LINE_STRIP_ADJACENCY = 0x000B
#: These are special primitives that are expected to be used specifically with
#: geomtry shaders. These primitives give the geometry shader more vertices
#: to work with for each input primitive. Data needs to be duplicated in buffers.
TRIANGLES_ADJACENCY = 0x000C
#: These are special primitives that are expected to be used specifically with
#: geomtry shaders. These primitives give the geometry shader more vertices
#: to work with for each input primitive. Data needs to be duplicated in buffers.
TRIANGLE_STRIP_ADJACENCY = 0x0000D
#: primitive type can only be used when Tessellation is active. It is a primitive
#: with a user-defined number of vertices, which is then tessellated based on the
#: control and evaluation shaders into regular points, lines, or triangles, depending
#: on the TES's settings.
PATCHES = 0x000E


class VertexArray:
    """
    A VertexArray object is an OpenGL object that stores all of the state needed to supply vertex data.

    It stores the format of the vertex data
    as well as the Buffer objects providing the vertex data arrays.

    In ModernGL, the VertexArray object also stores a reference
    for a :py:class:`Program` object, and some Subroutine information.

    A VertexArray object cannot be instantiated directly, it requires a context.
    Use :py:meth:`Context.vertex_array` or :py:meth:`Context.simple_vertex_array`
    to create one.
    """

    __slots__ = [
        'mglo', '_program', '_index_buffer', '_content', '_index_element_size',
        '_glo', '_mode', 'ctx', 'extra', 'scope'
    ]

    def __init__(self):
        self.mglo = None  #: Internal representation for debug purposes only.
        self._program = None
        self._index_buffer = None
        self._content = None
        self._index_element_size = None
        self._glo = None
        self._mode = None  #: int: The default rendering mode
        self.ctx = None  #: The context this object belongs to
        self.extra = None  #: Any - Attribute for storing user defined objects
        self.scope = None  #: The :py:class:`moderngl.Scope`.
        raise TypeError()

    def __repr__(self) -> str:
        if hasattr(self, 'mglo'):
            return '<VertexArray: %d>' % self.glo
        else:
            return '<VertexArray: INCOMPLETE>'

    def __eq__(self, other: Any) -> bool:
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
    def mode(self) -> int:
        """
        int: Get or set the default rendering mode.

        This value is used when ``mode`` is not passed in rendering calls.

        Examples::

            vao.mode = moderngl.TRIANGLE_STRIPS
        """
        return self._mode

    @mode.setter
    def mode(self, value: int) -> None:
        self._mode = value

    @property
    def program(self) -> 'Program':
        """
        Program: The program assigned to the VertexArray.

        The program used when rendering or transforming primitives.
        """
        return self._program

    @property
    def index_buffer(self) -> 'Buffer':
        """Buffer: The index buffer if the index_buffer is set, otherwise ``None``."""
        return self._index_buffer

    @property
    def index_element_size(self) -> int:
        """int: The byte size of each element in the index buffer."""
        return self._index_element_size

    @property
    def vertices(self) -> int:
        """
        int: The number of vertices detected.

        This is the minimum of the number of vertices possible per Buffer.
        The size of the index_buffer determines the number of vertices.
        Per instance vertex attributes does not affect this number.
        """
        return self.mglo.vertices

    @vertices.setter
    def vertices(self, value: int) -> None:
        self.mglo.vertices = int(value)

    @property
    def instances(self) -> int:
        """int: Get or set the number of instances to render."""
        return self.mglo.instances

    @instances.setter
    def instances(self, value: int) -> None:
        self.mglo.instances = int(value)

    @property
    def subroutines(self) -> Tuple[int, ...]:
        """
        tuple: The subroutines assigned to the VertexArray.

        The subroutines used when rendering or transforming primitives.
        """
        return self.mglo.subroutines

    @subroutines.setter
    def subroutines(self, value: Tuple[int, ...]) -> None:
        self.mglo.subroutines = tuple(value)

    @property
    def glo(self) -> int:
        """
        int: The internal OpenGL object.

        This values is provided for debug purposes only.
        """
        return self._glo

    def render(
        self,
        mode: Optional[int] = None,
        vertices: int = -1,
        *,
        first: int = 0,
        instances: int = -1,
    ) -> None:
        """
        The render primitive (mode) must be the same as the input primitive of the GeometryShader.

        Args:
            mode (int): By default :py:data:`TRIANGLES` will be used.
            vertices (int): The number of vertices to transform.

        Keyword Args:
            first (int): The index of the first vertex to start with.
            instances (int): The number of instances.
        """
        if mode is None:
            mode = self._mode

        if self.scope:
            with self.scope:
                self.mglo.render(mode, vertices, first, instances)
        else:
            self.mglo.render(mode, vertices, first, instances)

    def render_indirect(
        self,
        buffer: "Buffer",
        mode: Optional[int] = None,
        count: int = -1,
        *,
        first: int = 0,
    ) -> None:
        """
        The render primitive (mode) must be the same as the input primitive of the GeometryShader.

        The draw commands are 5 integers: (count, instanceCount, firstIndex, baseVertex, baseInstance).

        Args:
            buffer (Buffer): Indirect drawing commands.
            mode (int): By default :py:data:`TRIANGLES` will be used.
            count (int): The number of draws.

        Keyword Args:
            first (int): The index of the first indirect draw command.
        """
        if mode is None:
            mode = self._mode

        if self.scope:
            with self.scope:
                self.mglo.render_indirect(buffer.mglo, mode, count, first)
        else:
            self.mglo.render_indirect(buffer.mglo, mode, count, first)

    def transform(
        self,
        buffer: Union["Buffer", List["Buffer"]],
        mode: int = None,
        vertices: int = -1,
        *,
        first: int = 0,
        instances: int = -1,
        buffer_offset: int = 0,
    ) -> None:
        """
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
            buffer_offset (int): Byte offset for the output buffer
        """
        if mode is None:
            mode = self._mode

        if isinstance(buffer, (list, tuple)):
            outputs = [buf.mglo for buf in buffer]
        else:
            outputs = [buffer.mglo]

        if self.scope:
            with self.scope:
                self.mglo.transform(outputs, mode, vertices, first, instances, buffer_offset)
        else:
            self.mglo.transform(outputs, mode, vertices, first, instances, buffer_offset)

    def bind(
        self,
        attribute: int,
        cls: str,
        buffer: "Buffer",
        fmt: str,
        *,
        offset: int = 0,
        stride: int = 0,
        divisor: int = 0,
        normalize: bool = False,
    ) -> None:
        """
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
        """
        self.mglo.bind(attribute, cls, buffer.mglo, fmt, offset, stride, divisor, normalize)

    def release(self) -> None:
        """Release the ModernGL object."""
        if not isinstance(self, InvalidObject) and hasattr(self, "ctx"):
            self._program = None
            self._index_buffer = None
            self._content = None
            self.mglo.release()
