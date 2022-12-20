import struct
from typing import Any


class Attribute:
    def __init__(self):
        self.gl_type = None
        self.program_obj = None
        self.scalar_type = None
        self.rows_length = None
        self.row_length = None
        self.normalizable = None
        self.location = None
        self.array_length = None
        self.dimension = None
        self.shape = None
        self.name = None
        self.extra = None

    def __repr__(self):
        return f'<Attribute: {self.location}>'

    @property
    def mglo(self):
        return self


class Uniform:
    def __init__(self):
        self.program_obj = None
        self.gl_type = None
        self.fmt = None
        self.location = None
        self.array_length = None
        self.element_size = None
        self.dimension = None
        self.name = None
        self.matrix = None
        self.ctx = None
        self.extra = None

    def __repr__(self):
        return f'<Uniform: {self.location}>'

    @property
    def mglo(self):
        return self

    @property
    def value(self):
        data = self.read()
        if self.array_length > 1:
            if self.dimension > 1:
                return [
                    struct.unpack(self.fmt, data[i * self.element_size : i * self.element_size + self.element_size])
                    for i in range(self.array_length)
                ]
            else:
                return [
                    struct.unpack(self.fmt, data[i * self.element_size : i * self.element_size + self.element_size])[0]
                    for i in range(self.array_length)
                ]
        elif self.dimension > 1:
            return struct.unpack(self.fmt, data)
        else:
            return struct.unpack(self.fmt, data)[0]

    @value.setter
    def value(self, value):
        if self.array_length > 1:
            if self.dimension > 1:
                data = b''.join(struct.pack(self.fmt, *row) for row in value)
            else:
                data = b''.join(struct.pack(self.fmt, item) for item in value)
        elif self.dimension > 1:
            data = struct.pack(self.fmt, *value)
        else:
            data = struct.pack(self.fmt, value)
        self.write(data)

    def read(self):
        return self.ctx._read_uniform(self.program_obj, self.location, self.gl_type, self.array_length, self.element_size)

    def write(self, data: Any):
        self.ctx._write_uniform(self.program_obj, self.location, self.gl_type, self.array_length, data)


class UniformBlock:
    def __init__(self):
        self.program_obj = None
        self.index = None
        self.size = None
        self.name = None
        self.ctx = None
        self.extra = None

    def __repr__(self):
        return f'<UniformBlock: {self.index}>'

    @property
    def mglo(self):
        return self

    @property
    def binding(self):
        return self.ctx._get_ubo_binding(self.program_obj, self.index)

    @binding.setter
    def binding(self, binding):
        self.ctx._set_ubo_binding(self.program_obj, self.index, binding)

    @property
    def value(self):
        return self.ctx._get_ubo_binding(self.program_obj, self.index)

    @value.setter
    def value(self, value):
        self.ctx._set_ubo_binding(self.program_obj, self.index, value)


class StorageBlock:
    def __init__(self):
        self.program_obj = None
        self.index = None
        self.name = None
        self.ctx = None
        self.extra = None

    def __repr__(self):
        return f'<StorageBlock: {self.index}>'

    @property
    def mglo(self):
        return self

    @property
    def binding(self):
        return self.ctx._get_storage_block_binding(self.program_obj, self.index)

    @binding.setter
    def binding(self, binding):
        self.ctx._set_storage_block_binding(self.program_obj, self.index, binding)

    @property
    def value(self):
        return self.ctx._get_storage_block_binding(self.program_obj, self.index)

    @value.setter
    def value(self, value):
        self.ctx._set_storage_block_binding(self.program_obj, self.index, value)


class Subroutine:
    def __init__(self):
        self.index = None
        self.name = None
        self.extra = None

    def __repr__(self):
        return f'<Subroutine: {self.index}>'

    @property
    def mglo(self):
        return self


class Varying:
    def __init__(self):
        self.number = None
        self.array_length = None
        self.dimension = None
        self.name = None
        self.extra = None

    def __repr__(self):
        return f'<Varying: {self.number}>'

    @property
    def mglo(self):
        return self


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
    res.gl_type = gl_type
    res.program_obj = program_obj
    res.scalar_type = scalar_type
    res.rows_length = rows_length
    res.row_length = row_length
    res.normalizable = normalizable
    res.location = location
    res.array_length = array_length
    res.dimension = dimension
    res.shape = shape
    res.name = name
    return res


def make_uniform(name, gl_type, program_obj, location, array_length, ctx):
    tmp = UNIFORM_LOOKUP_TABLE.get(gl_type, (False, 1, 4, '1i'))
    matrix, dimension, element_size, fmt = tmp
    res = Uniform()
    res.name = name
    res.gl_type = gl_type
    res.fmt = fmt
    res.program_obj = program_obj
    res.location = location
    res.array_length = array_length
    res.matrix = matrix
    res.dimension = dimension
    res.element_size = element_size
    res.ctx = ctx
    return res


def make_uniform_block(name, program_obj, index, size, ctx):
    res = UniformBlock()
    res.name = name
    res.program_obj = program_obj
    res.index = index
    res.size = size
    res.ctx = ctx
    return res


def make_storage_block(name, program_obj, index, ctx):
    res = StorageBlock()
    res.name = name
    res.program_obj = program_obj
    res.index = index
    res.ctx = ctx
    return res


def make_subroutine(name, index):
    res = Subroutine()
    res.name = name
    res.index = index
    return res


def make_varying(name, number, array_length, dimension):
    res = Varying()
    res.number = number
    res.name = name
    res.array_length = array_length
    res.dimension = dimension
    return res


class InvalidObject:
    pass
