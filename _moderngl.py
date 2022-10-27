import struct
from typing import Any


class Attribute:
    """This class represents a program attribute."""

    def __init__(self):
        self._location: int = None
        self._array_length: int = None
        self._dimension: int = None
        self._shape: int = None
        self._name: str = None
        self.extra: Any = None  #: Attribute for storing user defined objects

    def __repr__(self) -> str:
        return '<Attribute: %d>' % self._location

    @property
    def mglo(self):
        return self

    @property
    def location(self) -> int:
        """
        int: The location of the attribute.

        The result of the glGetAttribLocation.
        """
        return self._location

    @property
    def array_length(self) -> int:
        """
        int: If the attribute is an array the array_length.

        is the length of the array otherwise `1`.
        """
        return self._array_length

    @property
    def dimension(self) -> int:
        """
        int: The attribute dimension.

        +-----------+-----------+
        | GLSL type | dimension |
        +===========+===========+
        | int       | 1         |
        +-----------+-----------+
        | ivec2     | 2         |
        +-----------+-----------+
        | ivec3     | 3         |
        +-----------+-----------+
        | ivec4     | 4         |
        +-----------+-----------+
        | uint      | 1         |
        +-----------+-----------+
        | uvec2     | 2         |
        +-----------+-----------+
        | uvec3     | 3         |
        +-----------+-----------+
        | uvec4     | 4         |
        +-----------+-----------+
        | float     | 1         |
        +-----------+-----------+
        | vec2      | 2         |
        +-----------+-----------+
        | vec3      | 3         |
        +-----------+-----------+
        | vec4      | 4         |
        +-----------+-----------+
        | double    | 1         |
        +-----------+-----------+
        | dvec2     | 2         |
        +-----------+-----------+
        | dvec3     | 3         |
        +-----------+-----------+
        | dvec4     | 4         |
        +-----------+-----------+
        | mat2      | 4         |
        +-----------+-----------+
        | mat2x3    | 6         |
        +-----------+-----------+
        | mat2x4    | 8         |
        +-----------+-----------+
        | mat3x2    | 6         |
        +-----------+-----------+
        | mat3      | 9         |
        +-----------+-----------+
        | mat3x4    | 12        |
        +-----------+-----------+
        | mat4x2    | 8         |
        +-----------+-----------+
        | mat4x3    | 12        |
        +-----------+-----------+
        | mat4      | 16        |
        +-----------+-----------+
        | dmat2     | 4         |
        +-----------+-----------+
        | dmat2x3   | 6         |
        +-----------+-----------+
        | dmat2x4   | 8         |
        +-----------+-----------+
        | dmat3x2   | 6         |
        +-----------+-----------+
        | dmat3     | 9         |
        +-----------+-----------+
        | dmat3x4   | 12        |
        +-----------+-----------+
        | dmat4x2   | 8         |
        +-----------+-----------+
        | dmat4x3   | 12        |
        +-----------+-----------+
        | dmat4     | 16        |
        +-----------+-----------+
        """
        return self._dimension

    @property
    def shape(self) -> str:
        """
        str: The shape is a single character, representing the scalar type of the attribute.

        +---------+--------------------------+
        | shape   | GLSL types               |
        +=========+==========================+
        | ``'i'`` | int                      |
        |         +--------------------------+
        |         | ivec2 ivec3 ivec4        |
        +---------+--------------------------+
        | ``'I'`` | uint                     |
        |         +--------------------------+
        |         | uvec2 uvec3 uvec4        |
        +---------+--------------------------+
        | ``'f'`` | float                    |
        |         +--------------------------+
        |         | vec2 vec3 vec4           |
        |         +--------------------------+
        |         | mat2 mat3 mat4           |
        |         +--------------------------+
        |         | mat2x3 mat2x4 mat3x4     |
        |         | mat4x2 mat4x2 mat4x3     |
        +---------+--------------------------+
        | ``'d'`` | double                   |
        |         +--------------------------+
        |         | dvec2 dvec3 dvec4        |
        |         +--------------------------+
        |         | dmat2 dmat3 dmat4        |
        |         +--------------------------+
        |         | dmat2x3 dmat2x4 dmat3x4  |
        |         | dmat4x2 dmat4x2 dmat4x3  |
        +---------+--------------------------+
        """
        return self._shape

    @property
    def name(self) -> str:
        """
        str: The attribute name.

        The name will be filtered to have no array syntax on it's end.
        Attribute name without ``'[0]'`` ending if any.
        """
        return self._name


class Uniform:
    """
    A uniform is a global GLSL variable declared with the "uniform" storage qualifier.

    These act as parameters that the user of a shader program can pass to that program.

    In ModernGL, Uniforms can be accessed using :py:meth:`Program.__getitem__`
    or :py:meth:`Program.__iter__`
    """

    def __init__(self):
        self._program_obj = None
        self._type = None
        self._fmt = None
        self._location = None
        self._array_length = None
        self._element_size = None
        self._dimension = None
        self._name = None
        self._matrix = None
        self._ctx = None
        self.extra = None  #: Any - Attribute for storing user defined objects

    def __repr__(self) -> str:
        return '<Uniform: %d>' % self._location

    @property
    def mglo(self):
        return self

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
        data = self.read()
        if self._array_length > 1:
            return [
                struct.unpack(self._fmt, data[i * self._element_size : i * self._element_size + self._element_size])
                for i in range(self._array_length)
            ]
        elif self._dimension > 1:
            return struct.unpack(self._fmt, data)
        else:
            return struct.unpack(self._fmt, data)[0]

    @value.setter
    def value(self, value: Any) -> None:
        if self._array_length > 1:
            data = b''.join(struct.pack(self._fmt, *row) for row in value)
        elif self._dimension > 1:
            data = struct.pack(self._fmt, *value)
        else:
            data = struct.pack(self._fmt, value)
        self.write(data)

    def read(self) -> bytes:
        """Read the value of the uniform."""
        return self._ctx._read_uniform(self._program_obj, self._location, self._type, self._array_length, self._element_size)

    def write(self, data: Any) -> None:
        """Write the value of the uniform."""
        self._ctx._write_uniform(self._program_obj, self._location, self._type, self._array_length, data)


class UniformBlock:
    """Uniform Block metadata."""

    def __init__(self):
        self._program_obj = None
        self._index = None
        self._size = None
        self._name = None
        self._ctx = None
        self.extra = None  #: Any - Attribute for storing user defined objects

    def __repr__(self):
        return '<UniformBlock: %d>' % self._index

    @property
    def mglo(self):
        return self

    @property
    def binding(self) -> int:
        """int: The binding of the uniform block."""
        return self._ctx._get_ubo_binding(self._program_obj, self._index)

    @binding.setter
    def binding(self, binding: int) -> None:
        self._ctx._set_ubo_binding(self._program_obj, self._index, binding)

    @property
    def value(self) -> int:
        """int: The value of the uniform block."""
        return self._ctx._get_ubo_binding(self._program_obj, self._index)

    @value.setter
    def value(self, value: int) -> None:
        self._ctx._set_ubo_binding(self._program_obj, self._index, value)

    @property
    def name(self) -> str:
        """str: The name of the uniform block."""
        return self._name

    @property
    def index(self) -> int:
        """int: The index of the uniform block."""
        return self._index

    @property
    def size(self) -> int:
        """int: The size of the uniform block."""
        return self._size



class Subroutine:
    """This class represents a program subroutine."""

    def __init__(self):
        self._index = None
        self._name = None
        self.extra = None  #: Any - Attribute for storing user defined objects

    def __repr__(self) -> str:
        return '<Subroutine: %d>' % self._index

    @property
    def mglo(self):
        return self

    @property
    def index(self) -> int:
        """int: The index of the subroutine."""
        return self._index

    @property
    def name(self) -> str:
        """str: The name of the subroutine."""
        return self._name


class Varying:
    """This class represents a program varying."""

    def __init__(self):
        self._number = None
        self._array_length = None
        self._dimension = None
        self._name = None
        self.extra: Any = None  #: Attribute for storing user defined objects

    def __repr__(self) -> str:
        return '<Varying: %d>' % self.number

    @property
    def mglo(self):
        return self

    @property
    def number(self) -> int:
        """int: The number of the varying."""
        return self._number

    @property
    def name(self) -> str:
        """str: The name of the varying."""
        return self._name


class Error(Exception):
    """Generic moderngl error."""
    pass


ATTRIBUTE_LOOKUP_TABLE = {
    0x1404: (1, 0x1404, 1, 1, False, 'i'),
    0x8b53: (2, 0x1404, 1, 2, False, 'i'),
    0x8b54: (3, 0x1404, 1, 3, False, 'i'),
    0x8b55: (4, 0x1404, 1, 4, False, 'i'),
    0x1405: (1, 0x1405, 1, 1, False, 'i'),
    0x8dc6: (2, 0x1405, 1, 2, False, 'i'),
    0x8dc7: (3, 0x1405, 1, 3, False, 'i'),
    0x8dc8: (4, 0x1405, 1, 4, False, 'i'),
    0x1406: (1, 0x1406, 1, 1, True, 'f'),
    0x8b50: (2, 0x1406, 1, 2, True, 'f'),
    0x8b51: (3, 0x1406, 1, 3, True, 'f'),
    0x8b52: (4, 0x1406, 1, 4, True, 'f'),
    0x140a: (1, 0x140a, 1, 1, False, 'd'),
    0x8ffc: (2, 0x140a, 1, 2, False, 'd'),
    0x8ffd: (3, 0x140a, 1, 3, False, 'd'),
    0x8ffe: (4, 0x140a, 1, 4, False, 'd'),
    0x8b5a: (4, 0x1406, 2, 2, True, 'f'),
    0x8b65: (6, 0x1406, 2, 3, True, 'f'),
    0x8b66: (8, 0x1406, 2, 4, True, 'f'),
    0x8b67: (6, 0x1406, 3, 2, True, 'f'),
    0x8b5b: (9, 0x1406, 3, 3, True, 'f'),
    0x8b68: (12, 0x1406, 3, 4, True, 'f'),
    0x8b69: (8, 0x1406, 4, 2, True, 'f'),
    0x8b6a: (12, 0x1406, 4, 3, True, 'f'),
    0x8b5c: (16, 0x1406, 4, 4, True, 'f'),
    0x8f46: (4, 0x140a, 2, 2, False, 'd'),
    0x8f49: (6, 0x140a, 2, 3, False, 'd'),
    0x8f4a: (8, 0x140a, 2, 4, False, 'd'),
    0x8f4b: (6, 0x140a, 3, 2, False, 'd'),
    0x8f47: (9, 0x140a, 3, 3, False, 'd'),
    0x8f4c: (12, 0x140a, 3, 4, False, 'd'),
    0x8f4d: (8, 0x140a, 4, 2, False, 'd'),
    0x8f4e: (12, 0x140a, 4, 3, False, 'd'),
    0x8f48: (16, 0x140a, 4, 4, False, 'd'),
}

UNIFORM_LOOKUP_TABLE = {
    0x8B56: (False, 1, 4, '1i'),  # GL_BOOL
    0x8B57: (False, 2, 8, '2i'),  # GL_BOOL_VEC2
    0x8B58: (False, 3, 12, '3i'),  # GL_BOOL_VEC3
    0x8B59: (False, 4, 16, '4i'),  # GL_BOOL_VEC4
    0x1404: (False, 1, 4, '1i'),  # GL_INT
    0x8B53: (False, 2, 8, '2i'),  # GL_INT_VEC2
    0x8B54: (False, 3, 12, '3i'),  # GL_INT_VEC3
    0x8B55: (False, 4, 16, '4i'),  # GL_INT_VEC4
    0x1405: (False, 1, 4, '1I'),  # GL_UNSIGNED_INT
    0x8DC6: (False, 2, 8, '2I'),  # GL_UNSIGNED_INT_VEC2
    0x8DC7: (False, 3, 12, '3I'),  # GL_UNSIGNED_INT_VEC3
    0x8DC8: (False, 4, 16, '4I'),  # GL_UNSIGNED_INT_VEC4
    0x1406: (False, 1, 4, '1f'),  # GL_FLOAT
    0x8B50: (False, 2, 8, '2f'),  # GL_FLOAT_VEC2
    0x8B51: (False, 3, 12, '3f'),  # GL_FLOAT_VEC3
    0x8B52: (False, 4, 16, '4f'),  # GL_FLOAT_VEC4
    0x140A: (False, 1, 8, '1d'),  # GL_DOUBLE
    0x8FFC: (False, 2, 16, '2d'),  # GL_DOUBLE_VEC2
    0x8FFD: (False, 3, 24, '3d'),  # GL_DOUBLE_VEC3
    0x8FFE: (False, 4, 32, '4d'),  # GL_DOUBLE_VEC4
    0x8B5D: (False, 1, 4, '1i'),  # GL_SAMPLER_1D
    0x8DC0: (False, 1, 4, '1i'),  # GL_SAMPLER_1D_ARRAY
    0x8DC9: (False, 1, 4, '1i'),  # GL_INT_SAMPLER_1D
    0x8DCE: (False, 1, 4, '1i'),  # GL_INT_SAMPLER_1D_ARRAY
    0x8B5E: (False, 1, 4, '1i'),  # GL_SAMPLER_2D
    0x8DCA: (False, 1, 4, '1i'),  # GL_INT_SAMPLER_2D
    0x8DD2: (False, 1, 4, '1i'),  # GL_UNSIGNED_INT_SAMPLER_2D
    0x8DC1: (False, 1, 4, '1i'),  # GL_SAMPLER_2D_ARRAY
    0x8DCF: (False, 1, 4, '1i'),  # GL_INT_SAMPLER_2D_ARRAY
    0x8DD7: (False, 1, 4, '1i'),  # GL_UNSIGNED_INT_SAMPLER_2D_ARRAY
    0x8B5F: (False, 1, 4, '1i'),  # GL_SAMPLER_3D
    0x8DCB: (False, 1, 4, '1i'),  # GL_INT_SAMPLER_3D
    0x8DD3: (False, 1, 4, '1i'),  # GL_UNSIGNED_INT_SAMPLER_3D
    0x8B62: (False, 1, 4, '1i'),  # GL_SAMPLER_2D_SHADOW
    0x9108: (False, 1, 4, '1i'),  # GL_SAMPLER_2D_MULTISAMPLE
    0x9109: (False, 1, 4, '1i'),  # GL_INT_SAMPLER_2D_MULTISAMPLE
    0x910A: (False, 1, 4, '1i'),  # GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE
    0x910B: (False, 1, 4, '1i'),  # GL_SAMPLER_2D_MULTISAMPLE_ARRAY
    0x910C: (False, 1, 4, '1i'),  # GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
    0x910D: (False, 1, 4, '1i'),  # GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
    0x8B60: (False, 1, 4, '1i'),  # GL_SAMPLER_CUBE
    0x8DCC: (False, 1, 4, '1i'),  # GL_INT_SAMPLER_CUBE
    0x8DD4: (False, 1, 4, '1i'),  # GL_UNSIGNED_INT_SAMPLER_CUBE
    0x904D: (False, 1, 4, '1i'),  # GL_IMAGE_2D
    0x8B5A: (True, 4, 16, '4f'),  # GL_FLOAT_MAT2
    0x8B65: (True, 6, 24, '6f'),  # GL_FLOAT_MAT2x3
    0x8B66: (True, 8, 32, '8f'),  # GL_FLOAT_MAT2x4
    0x8B67: (True, 6, 24, '6f'),  # GL_FLOAT_MAT3x2
    0x8B5B: (True, 9, 36, '9f'),  # GL_FLOAT_MAT3
    0x8B68: (True, 12, 48, '12f'),  # GL_FLOAT_MAT3x4
    0x8B69: (True, 8, 32, '8f'),  # GL_FLOAT_MAT4x2
    0x8B6A: (True, 12, 48, '12f'),  # GL_FLOAT_MAT4x3
    0x8B5C: (True, 16, 64, '16f'),  # GL_FLOAT_MAT4
    0x8F46: (True, 4, 32, '4d'),  # GL_DOUBLE_MAT2
    0x8F49: (True, 6, 48, '6d'),  # GL_DOUBLE_MAT2x3
    0x8F4A: (True, 8, 64, '8d'),  # GL_DOUBLE_MAT2x4
    0x8F4B: (True, 6, 48, '6d'),  # GL_DOUBLE_MAT3x2
    0x8F47: (True, 9, 72, '9d'),  # GL_DOUBLE_MAT3
    0x8F4C: (True, 12, 96, '12d'),  # GL_DOUBLE_MAT3x4
    0x8F4D: (True, 8, 64, '8d'),  # GL_DOUBLE_MAT4x2
    0x8F4E: (True, 12, 96, '12d'),  # GL_DOUBLE_MAT4x3
    0x8F48: (True, 16, 128, '16d'),  # GL_DOUBLE_MAT4
}


def make_attribute(name, gl_type, program_obj, location, array_length):
    tmp = ATTRIBUTE_LOOKUP_TABLE.get(gl_type, (1, 0, 1, 1, False, '?'))
    dimension, scalar_type, rows_length, row_length, normalizable, shape = tmp
    rows_length *= array_length
    res = Attribute()
    res._type = gl_type
    res._program_obj = program_obj
    res._scalar_type = scalar_type
    res._rows_length = rows_length
    res._row_length = row_length
    res._normalizable = normalizable
    res._location = location
    res._array_length = array_length
    res._dimension = dimension
    res._shape = shape
    res._name = name
    return res


def make_uniform(name, gl_type, program_obj, location, array_length, ctx):
    tmp = UNIFORM_LOOKUP_TABLE.get(gl_type, (False, 1, 4, '1i'))
    matrix, dimension, element_size, fmt = tmp
    res = Uniform()
    res._name = name
    res._type = gl_type
    res._fmt = fmt
    res._program_obj = program_obj
    res._location = location
    res._array_length = array_length
    res._matrix = matrix
    res._dimension = dimension
    res._element_size = element_size
    res._ctx = ctx
    return res


def make_uniform_block(name, program_obj, index, size, ctx):
    res = UniformBlock()
    res._name = name
    res._program_obj = program_obj
    res._index = index
    res._size = size
    res._ctx = ctx
    return res


def make_subroutine(name, index):
    res = Subroutine()
    res._name = name
    res._index = index
    return res


def make_varying(name, number, array_length, dimension):
    res = Varying()
    res._name = name
    res._array_length = array_length
    res._dimension = dimension
    return res


class InvalidObject:
    pass
