__all__ = ['Uniform']


from typing import Any


class Uniform:
    """
    A uniform is a global GLSL variable declared with the "uniform" storage qualifier.

    These act as parameters that the user of a shader program can pass to that program.

    In ModernGL, Uniforms can be accessed using :py:meth:`Program.__getitem__`
    or :py:meth:`Program.__iter__`
    """

    __slots__ = ['mglo', '_location', '_array_length', '_dimension', '_name', 'extra']

    def __init__(self):
        self.mglo = None  #: Internal representation for debug purposes only.
        self._location = None
        self._array_length = None
        self._dimension = None
        self._name = None
        self.extra = None  #: Any - Attribute for storing user defined objects
        raise TypeError()

    def __repr__(self) -> str:
        return '<Uniform: %d>' % self._location

    def __hash__(self) -> int:
        return id(self)

    @property
    def location(self) -> int:
        """
        int: The location of the uniform.

        The location holds the value returned by the glGetUniformLocation.
        To set the value of the uniform use the :py:attr:`value` instead.
        """
        return self._location

    @property
    def dimension(self) -> int:
        """
        int: The dimension of the uniform.

        +-----------------+-----------+
        | GLSL type       | dimension |
        +=================+===========+
        | sampler2D       | 1         |
        +-----------------+-----------+
        | sampler2DCube   | 1         |
        +-----------------+-----------+
        | sampler2DShadow | 1         |
        +-----------------+-----------+
        | bool            | 1         |
        +-----------------+-----------+
        | bvec2           | 2         |
        +-----------------+-----------+
        | bvec3           | 3         |
        +-----------------+-----------+
        | bvec4           | 4         |
        +-----------------+-----------+
        | int             | 1         |
        +-----------------+-----------+
        | ivec2           | 2         |
        +-----------------+-----------+
        | ivec3           | 3         |
        +-----------------+-----------+
        | ivec4           | 4         |
        +-----------------+-----------+
        | uint            | 1         |
        +-----------------+-----------+
        | uvec2           | 2         |
        +-----------------+-----------+
        | uvec3           | 3         |
        +-----------------+-----------+
        | uvec4           | 4         |
        +-----------------+-----------+
        | float           | 1         |
        +-----------------+-----------+
        | vec2            | 2         |
        +-----------------+-----------+
        | vec3            | 3         |
        +-----------------+-----------+
        | vec4            | 4         |
        +-----------------+-----------+
        | double          | 1         |
        +-----------------+-----------+
        | dvec2           | 2         |
        +-----------------+-----------+
        | dvec3           | 3         |
        +-----------------+-----------+
        | dvec4           | 4         |
        +-----------------+-----------+
        | mat2            | 4         |
        +-----------------+-----------+
        | mat2x3          | 6         |
        +-----------------+-----------+
        | mat2x4          | 8         |
        +-----------------+-----------+
        | mat3x2          | 6         |
        +-----------------+-----------+
        | mat3            | 9         |
        +-----------------+-----------+
        | mat3x4          | 12        |
        +-----------------+-----------+
        | mat4x2          | 8         |
        +-----------------+-----------+
        | mat4x3          | 12        |
        +-----------------+-----------+
        | mat4            | 16        |
        +-----------------+-----------+
        | dmat2           | 4         |
        +-----------------+-----------+
        | dmat2x3         | 6         |
        +-----------------+-----------+
        | dmat2x4         | 8         |
        +-----------------+-----------+
        | dmat3x2         | 6         |
        +-----------------+-----------+
        | dmat3           | 9         |
        +-----------------+-----------+
        | dmat3x4         | 12        |
        +-----------------+-----------+
        | dmat4x2         | 8         |
        +-----------------+-----------+
        | dmat4x3         | 12        |
        +-----------------+-----------+
        | dmat4           | 16        |
        +-----------------+-----------+
        """
        return self._dimension

    @property
    def array_length(self) -> int:
        """
        int: The length of the array of the uniform.

        The array_length is `1` for non array uniforms.
        """
        return self._array_length

    @property
    def name(self) -> str:
        """
        str: The name of the uniform.

        The name does not contain leading `[0]`.
        The name may contain `[ ]` when the uniform is part of a struct.
        """
        return self._name

    @property
    def value(self) -> Any:
        """
        The value of the uniform.

        Reading the value of the uniform may force the GPU to sync.

        The value must be a tuple for non array uniforms.
        The value must be a list of tuples for array uniforms.
        """
        return self.mglo.value

    @value.setter
    def value(self, value: Any) -> None:
        self.mglo.value = value

    def read(self) -> bytes:
        """Read the value of the uniform."""
        return self.mglo.data

    def write(self, data: Any) -> None:
        """Write the value of the uniform."""
        self.mglo.data = data
