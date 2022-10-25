from typing import Any, Generator, Tuple, Union

from moderngl.mgl import InvalidObject  # type: ignore

from .program_members import (
    Attribute,
    Subroutine,
    Uniform,
    UniformBlock,
    Varying,
)

__all__ = ['Program', 'detect_format']


class Program:
    """
    A Program object represents fully processed executable code in the OpenGL Shading Language, \
    for one or more Shader stages.

    In ModernGL, a Program object can be assigned to :py:class:`VertexArray` objects.
    The VertexArray object  is capable of binding the Program object once the
    :py:meth:`VertexArray.render` or :py:meth:`VertexArray.transform` is called.

    Program objects has no method called ``use()``, VertexArrays encapsulate this mechanism.

    A Program object cannot be instantiated directly, it requires a context.
    Use :py:meth:`Context.program` to create one.

    Uniform buffers can be bound using :py:meth:`Buffer.bind_to_uniform_block`
    or can be set individually. For more complex binding yielding higher
    performance consider using :py:class:`moderngl.Scope`.
    """

    __slots__ = ['mglo', '_members', '_subroutines', '_geom', '_glo', '_is_transform', 'ctx', 'extra']

    def __init__(self):
        self.mglo = None  #: Internal representation for debug purposes only.
        self._members = {}
        self._subroutines = None
        self._geom = (None, None, None)
        self._glo = None
        self._is_transform = None  #: bool: If this is a transform program
        self.ctx = None  #: The context this object belongs to
        self.extra = None  #: Any - Attribute for storing user defined objects
        raise TypeError()

    def __repr__(self):
        if hasattr(self, '_glo'):
            return f"<{self.__class__.__name__}: {self._glo}>"
        else:
            return f"<{self.__class__.__name__}: INCOMPLETE>"

    def __eq__(self, other: Any) -> bool:
        """
        Compares two programs opengl names (mglo).

        Returns:
            bool: If the programs have the same opengl name

        Example::

            # True if the internal opengl name is the same
            program_1 == program_2
        """
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

    def __getitem__(self, key: str) -> Union[Uniform, UniformBlock, Subroutine, Attribute, Varying]:
        """
        Get a member such as uniforms, uniform blocks, subroutines, attributes and varyings by name.

        .. code-block:: python

            # Get a uniform
            uniform = program['color']

            # Uniform values can be set on the returned object
            # or the `__setitem__` shortcut can be used.
            program['color'].value = 1.0, 1.0, 1.0, 1.0

            # Still when writing byte data we need to use the `write()` method
            program['color'].write(buffer)
        """
        return self._members[key]

    def __setitem__(self, key: str, value: Any) -> None:
        """
        Set a value of uniform or uniform block.

        .. code-block:: python

            # Set a vec4 uniform
            uniform['color'] = 1.0, 1.0, 1.0, 1.0

            # Optionally we can store references to a member and set the value directly
            uniform = program['color']
            uniform.value = 1.0, 0.0, 0.0, 0.0

            uniform = program['cameraMatrix']
            uniform.write(camera_matrix)
        """
        self._members[key].value = value

    def __iter__(self) -> Generator[str, None, None]:
        """
        Yields the internal members names as strings.

        This includes all members such as uniforms, attributes etc.

        Example::

            # Print member information
            for name in program:
                member = program[name]
                print(name, type(member), member)

        Output::

            vert <class 'moderngl.program_members.attribute.Attribute'> <Attribute: 0>
            vert_color <class 'moderngl.program_members.attribute.Attribute'> <Attribute: 1>
            gl_InstanceID <class 'moderngl.program_members.attribute.Attribute'> <Attribute: -1>
            rotation <class 'moderngl.program_members.uniform.Uniform'> <Uniform: 0>
            scale <class 'moderngl.program_members.uniform.Uniform'> <Uniform: 1>

        We can filter on member type if needed::

            for name in prog:
                member = prog[name]
                if isinstance(member, moderngl.Uniform):
                    print("Uniform", name, member)

        or a less verbose version using dict comprehensions::

            uniforms = {name: self.prog[name] for name in self.prog
                        if isinstance(self.prog[name], moderngl.Uniform)}
            print(uniforms)

        Output::

            {'rotation': <Uniform: 0>, 'scale': <Uniform: 1>}

        """
        yield from self._members

    @property
    def is_transform(self) -> bool:
        """bool: If this is a tranform program (no fragment shader)."""
        return self._is_transform

    @property
    def geometry_input(self) -> int:
        """
        int: The geometry input primitive.

        The GeometryShader's input primitive if the GeometryShader exists.
        The geometry input primitive will be used for validation.
        (from ``layout(input_primitive) in;``)

        This can only be ``POINTS``, ``LINES``, ``LINES_ADJACENCY``, ``TRIANGLES``, ``TRIANGLE_ADJACENCY``.
        """
        return self._geom[0]

    @property
    def geometry_output(self) -> int:
        """
        int: The geometry output primitive.

        The GeometryShader's output primitive if the GeometryShader exists.
        This can only be ``POINTS``, ``LINE_STRIP`` and ``TRIANGLE_STRIP``
        (from ``layout(output_primitive, max_vertices = vert_count) out;``)
        """
        return self._geom[1]

    @property
    def geometry_vertices(self) -> int:
        """
        int: The maximum number of vertices that.

        the geometry shader will output.
        (from ``layout(output_primitive, max_vertices = vert_count) out;``)
        """
        return self._geom[2]

    @property
    def subroutines(self) -> Tuple[str, ...]:
        """tuple: The subroutine uniforms."""
        return self._subroutines

    @property
    def glo(self) -> int:
        """
        int: The internal OpenGL object.

        This values is provided for debug purposes only.
        """
        return self._glo

    def get(self, key: str, default: Any) -> Union[Uniform, UniformBlock, Subroutine, Attribute, Varying]:
        """
        Returns a Uniform, UniformBlock, Subroutine, Attribute or Varying.

        Args:
            default: This is the value to be returned in case key does not exist.

        Returns:
            :py:class:`Uniform`, :py:class:`UniformBlock`, :py:class:`Subroutine`,
            :py:class:`Attribute` or :py:class:`Varying`
        """
        return self._members.get(key, default)

    def release(self) -> None:
        """Release the ModernGL object."""
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()


def detect_format(
    program: Program,
    attributes: Any,
    mode: str = 'mgl',
) -> str:
    """
    Detect format for vertex attributes.

    The format returned does not contain padding.

    Args:
        program (Program): The program.
        attributes (list): A list of attribute names.

    Returns:
        str
    """
    def fmt(attr: Any) -> Tuple[int, str]:
        """For internal use only."""
        # Translate shape format into attribute format
        mgl_fmt = {
            'd': 'f8',
            'I': 'u'
        }
        # moderngl attribute format uses f, i and u
        if mode == 'mgl':
            return attr.array_length * attr.dimension, mgl_fmt.get(attr.shape) or attr.shape
        # struct attribute format uses f, d, i and I
        elif mode == 'struct':
            return attr.array_length * attr.dimension, attr.shape
        else:
            raise ValueError("invalid format mode: {0}".format(mode))

    return ' '.join('%d%s' % fmt(program[a]) for a in attributes)
