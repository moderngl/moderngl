import struct
from typing import Any


class Attribute:
    def __init__(self):
        self._location: int = None
        self._array_length: int = None
        self._dimension: int = None
        self._shape: int = None
        self._name: str = None
        self.extra: Any = None

    def __repr__(self) -> str:
        return '<Attribute: %d>' % self._location

    @property
    def mglo(self):
        return self

    @property
    def location(self) -> int:
        return self._location

    @property
    def array_length(self) -> int:
        return self._array_length

    @property
    def dimension(self) -> int:
        return self._dimension

    @property
    def shape(self) -> int:
        return self._shape

    @property
    def name(self) -> str:
        return self._name


class Uniform:
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
        self.extra = None

    def __repr__(self) -> str:
        return '<Uniform: %d>' % self._location

    @property
    def mglo(self):
        return self

    @property
    def location(self) -> int:
        return self._location

    @property
    def dimension(self) -> int:
        return self._dimension

    @property
    def array_length(self) -> int:
        return self._array_length

    @property
    def name(self) -> str:
        return self._name

    @property
    def value(self) -> Any:
        data = self.read()
        if self._array_length > 1:
            if self._dimension > 1:
                return [
                    struct.unpack(self._fmt, data[i * self._element_size : i * self._element_size + self._element_size])
                    for i in range(self._array_length)
                ]
            else:
                return [
                    struct.unpack(self._fmt, data[i * self._element_size : i * self._element_size + self._element_size])[0]
                    for i in range(self._array_length)
                ]
        elif self._dimension > 1:
            return struct.unpack(self._fmt, data)
        else:
            return struct.unpack(self._fmt, data)[0]

    @value.setter
    def value(self, value: Any) -> None:
        if self._array_length > 1:
            if self._dimension > 1:
                data = b''.join(struct.pack(self._fmt, *row) for row in value)
            else:
                data = b''.join(struct.pack(self._fmt, item) for item in value)
        elif self._dimension > 1:
            data = struct.pack(self._fmt, *value)
        else:
            data = struct.pack(self._fmt, value)
        self.write(data)

    def read(self) -> bytes:
        return self._ctx._read_uniform(self._program_obj, self._location, self._type, self._array_length, self._element_size)

    def write(self, data: Any) -> None:
        self._ctx._write_uniform(self._program_obj, self._location, self._type, self._array_length, data)


class UniformBlock:
    def __init__(self):
        self._program_obj = None
        self._index = None
        self._size = None
        self._name = None
        self._ctx = None
        self.extra = None

    def __repr__(self):
        return '<UniformBlock: %d>' % self._index

    @property
    def mglo(self):
        return self

    @property
    def binding(self) -> int:
        return self._ctx._get_ubo_binding(self._program_obj, self._index)

    @binding.setter
    def binding(self, binding: int) -> None:
        self._ctx._set_ubo_binding(self._program_obj, self._index, binding)

    @property
    def value(self) -> int:
        return self._ctx._get_ubo_binding(self._program_obj, self._index)

    @value.setter
    def value(self, value: int) -> None:
        self._ctx._set_ubo_binding(self._program_obj, self._index, value)

    @property
    def name(self) -> str:
        return self._name

    @property
    def index(self) -> int:
        return self._index

    @property
    def size(self) -> int:
        return self._size



class Subroutine:
    def __init__(self):
        self._index = None
        self._name = None
        self.extra = None

    def __repr__(self) -> str:
        return '<Subroutine: %d>' % self._index

    @property
    def mglo(self):
        return self

    @property
    def index(self) -> int:
        return self._index

    @property
    def name(self) -> str:
        return self._name


class Varying:
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
        return self._number

    @property
    def name(self) -> str:
        return self._name


class Error(Exception):
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
