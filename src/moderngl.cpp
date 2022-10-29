#include <Python.h>
#include "gl_methods.hpp"

#define MGLError_Set(...) PyErr_Format(moderngl_error, __VA_ARGS__)
#define MGL_max(a, b) (((a) > (b)) ? (a) : (b))
#define MGL_min(a, b) (((a) < (b)) ? (a) : (b))

enum MGLEnableFlag {
    MGL_NOTHING = 0,
    MGL_BLEND = 1,
    MGL_DEPTH_TEST = 2,
    MGL_CULL_FACE = 4,
    MGL_RASTERIZER_DISCARD = 8,
    MGL_PROGRAM_POINT_SIZE = 16,
    MGL_INVALID = 0x40000000,
};

enum SHADER_SLOT_ENUM {
    VERTEX_SHADER_SLOT,
    FRAGMENT_SHADER_SLOT,
    GEOMETRY_SHADER_SLOT,
    TESS_EVALUATION_SHADER_SLOT,
    TESS_CONTROL_SHADER_SLOT,
    NUM_SHADER_SLOTS,
};

static const int SHADER_TYPE[] = {
    GL_VERTEX_SHADER,
    GL_FRAGMENT_SHADER,
    GL_GEOMETRY_SHADER,
    GL_TESS_CONTROL_SHADER,
    GL_TESS_EVALUATION_SHADER,
};

struct MGLBuffer;
struct MGLComputeShader;
struct MGLContext;
struct MGLFramebuffer;
struct MGLProgram;
struct MGLRenderbuffer;
struct MGLTexture;
struct MGLTexture3D;
struct MGLTextureArray;
struct MGLTextureCube;
struct MGLVertexArray;
struct MGLSampler;

struct MGLDataType {
    int * base_format;
    int * internal_format;
    int gl_type;
    int size;
    bool float_type;
};

struct MGLBuffer {
    PyObject_HEAD
    MGLContext * context;
    int buffer_obj;
    Py_ssize_t size;
    bool dynamic;
    bool released;
};

struct MGLComputeShader {
    PyObject_HEAD
    MGLContext * context;
    int program_obj;
    int shader_obj;
    bool released;
};

struct MGLContext {
    PyObject_HEAD
    PyObject * ctx;
    PyObject * enter_func;
    PyObject * exit_func;
    PyObject * release_func;
    PyObject * extensions;
    MGLFramebuffer * default_framebuffer;
    MGLFramebuffer * bound_framebuffer;
    int version_code;
    int max_samples;
    int max_integer_samples;
    int max_color_attachments;
    int max_texture_units;
    int default_texture_unit;
    float max_anisotropy;
    int enable_flags;
    int front_face;
    int cull_face;
    int depth_func;
    int blend_func_src;
    int blend_func_dst;
    bool wireframe;
    bool multisample;
    int provoking_vertex;
    float polygon_offset_factor;
    float polygon_offset_units;
    GLMethods gl;
    bool released;
};

struct MGLFramebuffer {
    PyObject_HEAD
    MGLContext * context;
    bool * color_mask;
    unsigned * draw_buffers;
    int draw_buffers_len;
    int framebuffer_obj;
    int viewport_x;
    int viewport_y;
    int viewport_width;
    int viewport_height;
    bool scissor_enabled;
    int scissor_x;
    int scissor_y;
    int scissor_width;
    int scissor_height;

    // Flags this as a detected framebuffer we don't control the size of
    bool dynamic;
    int width;
    int height;
    int samples;
    bool depth_mask;
    bool released;
};

struct MGLProgram {
    PyObject_HEAD
    MGLContext * context;
    int geometry_input;
    int geometry_output;
    int program_obj;
    int num_vertex_shader_subroutines;
    int num_fragment_shader_subroutines;
    int num_geometry_shader_subroutines;
    int num_tess_evaluation_shader_subroutines;
    int num_tess_control_shader_subroutines;
    int geometry_vertices;
    int num_varyings;
    bool released;
};

enum MGLQueryKeys {
    SAMPLES_PASSED,
    ANY_SAMPLES_PASSED,
    TIME_ELAPSED,
    PRIMITIVES_GENERATED,
};

struct MGLQuery {
    PyObject_HEAD
    MGLContext * context;
    int query_obj[4];
    bool released;
};

struct MGLRenderbuffer {
    PyObject_HEAD
    MGLContext * context;
    MGLDataType * data_type;
    int renderbuffer_obj;
    int width;
    int height;
    int components;
    int samples;
    bool depth;
    bool released;
};

struct MGLScope {
    PyObject_HEAD
    MGLContext * context;
    MGLFramebuffer * framebuffer;
    MGLFramebuffer * old_framebuffer;
    int * textures;
    int * buffers;
    PyObject * samplers;
    int num_textures;
    int num_buffers;
    int enable_flags;
    int old_enable_flags;
    bool released;
};

struct MGLTexture {
    PyObject_HEAD
    MGLContext * context;
    MGLDataType * data_type;
    int texture_obj;
    int width;
    int height;
    int components;
    int samples;
    int min_filter;
    int mag_filter;
    int max_level;
    int compare_func;
    float anisotropy;
    bool depth;
    bool repeat_x;
    bool repeat_y;
    bool external;
    bool released;
};

struct MGLTexture3D {
    PyObject_HEAD
    MGLContext * context;
    MGLDataType * data_type;
    int texture_obj;
    int width;
    int height;
    int depth;
    int components;
    int min_filter;
    int mag_filter;
    int max_level;
    bool repeat_x;
    bool repeat_y;
    bool repeat_z;
    bool released;
};

struct MGLTextureArray {
    PyObject_HEAD
    MGLContext * context;
    MGLDataType * data_type;
    int texture_obj;
    int width;
    int height;
    int layers;
    int components;
    int min_filter;
    int mag_filter;
    int max_level;
    bool repeat_x;
    bool repeat_y;
    float anisotropy;
    bool released;
};

struct MGLTextureCube {
    PyObject_HEAD
    MGLContext * context;
    MGLDataType * data_type;
    int texture_obj;
    int width;
    int height;
    int depth;
    int components;
    int min_filter;
    int mag_filter;
    int max_level;
    float anisotropy;
    bool released;
};

struct MGLVertexArray {
    PyObject_HEAD
    MGLContext * context;
    MGLProgram * program;
    MGLBuffer * index_buffer;
    int index_element_size;
    int index_element_type;
    unsigned * subroutines;
    int num_subroutines;
    int vertex_array_obj;
    int num_vertices;
    int num_instances;
    bool released;
};

struct MGLSampler {
    PyObject_HEAD
    MGLContext * context;
    int sampler_obj;
    int min_filter;
    int mag_filter;
    float anisotropy;
    int compare_func;
    bool repeat_x;
    bool repeat_y;
    bool repeat_z;
    float border_color[4];
    float min_lod;
    float max_lod;
    bool released;
};

PyObject * helper;
PyObject * moderngl_error;

PyTypeObject * MGLBuffer_type;
PyTypeObject * MGLComputeShader_type;
PyTypeObject * MGLContext_type;
PyTypeObject * MGLFramebuffer_type;
PyTypeObject * MGLProgram_type;
PyTypeObject * MGLQuery_type;
PyTypeObject * MGLRenderbuffer_type;
PyTypeObject * MGLScope_type;
PyTypeObject * MGLTexture_type;
PyTypeObject * MGLTextureArray_type;
PyTypeObject * MGLTextureCube_type;
PyTypeObject * MGLTexture3D_type;
PyTypeObject * MGLVertexArray_type;
PyTypeObject * MGLSampler_type;

struct FormatNode {
    int size;
    int count;
    int type;
    bool normalize;
};

struct FormatInfo {
    int size;
    int nodes;
    int divisor;
    bool valid;

    static FormatInfo invalid() {
        FormatInfo invalid;
        invalid.size = 0;
        invalid.nodes = 0;
        invalid.divisor = 0;
        invalid.valid = false;
        return invalid;
    }
};

struct FormatIterator {
    const char * ptr;
    FormatNode node;

    FormatIterator(const char * str);

    FormatInfo info();
    FormatNode * next();
};

FormatNode * InvalidFormat = (FormatNode *)(-1);

static int float_base_format[5] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};
static int int_base_format[5] = {0, GL_RED_INTEGER, GL_RG_INTEGER, GL_RGB_INTEGER, GL_RGBA_INTEGER};

static int f1_internal_format[5] = {0, GL_R8, GL_RG8, GL_RGB8, GL_RGBA8};
static int f2_internal_format[5] = {0, GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F};
static int f4_internal_format[5] = {0, GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F};
static int u1_internal_format[5] = {0, GL_R8UI, GL_RG8UI, GL_RGB8UI, GL_RGBA8UI};
static int u2_internal_format[5] = {0, GL_R16UI, GL_RG16UI, GL_RGB16UI, GL_RGBA16UI};
static int u4_internal_format[5] = {0, GL_R32UI, GL_RG32UI, GL_RGB32UI, GL_RGBA32UI};
static int i1_internal_format[5] = {0, GL_R8I, GL_RG8I, GL_RGB8I, GL_RGBA8I};
static int i2_internal_format[5] = {0, GL_R16I, GL_RG16I, GL_RGB16I, GL_RGBA16I};
static int i4_internal_format[5] = {0, GL_R32I, GL_RG32I, GL_RGB32I, GL_RGBA32I};

static int n1_internal_format[5] = {0, GL_R8, GL_RG8, GL_RGB8, GL_RGBA8};
static int n2_internal_format[5] = {0, GL_R16, GL_RG16, GL_RGB16, GL_RGBA16};

static MGLDataType f1 = {float_base_format, f1_internal_format, GL_UNSIGNED_BYTE, 1, true};
static MGLDataType f2 = {float_base_format, f2_internal_format, GL_HALF_FLOAT, 2, true};
static MGLDataType f4 = {float_base_format, f4_internal_format, GL_FLOAT, 4, true};
static MGLDataType u1 = {int_base_format, u1_internal_format, GL_UNSIGNED_BYTE, 1, false};
static MGLDataType u2 = {int_base_format, u2_internal_format, GL_UNSIGNED_SHORT, 2, false};
static MGLDataType u4 = {int_base_format, u4_internal_format, GL_UNSIGNED_INT, 4, false};
static MGLDataType i1 = {int_base_format, i1_internal_format, GL_BYTE, 1, false};
static MGLDataType i2 = {int_base_format, i2_internal_format, GL_SHORT, 2, false};
static MGLDataType i4 = {int_base_format, i4_internal_format, GL_INT, 4, false};

static MGLDataType nu1 = {float_base_format, n1_internal_format, GL_UNSIGNED_BYTE, 1, false};
static MGLDataType nu2 = {float_base_format, n2_internal_format, GL_UNSIGNED_SHORT, 2, false};
static MGLDataType ni1 = {float_base_format, n1_internal_format, GL_BYTE, 1, false};
static MGLDataType ni2 = {float_base_format, n2_internal_format, GL_SHORT, 2, false};

MGLDataType * from_dtype(const char * dtype, Py_ssize_t size) {
    if (size < 2 || size > 3) return 0;

    if (size == 2) {
        switch (dtype[0] * 256 + dtype[1]) {
            case ('f' * 256 + '1'):
                return &f1;

            case ('f' * 256 + '2'):
                return &f2;

            case ('f' * 256 + '4'):
                return &f4;

            case ('u' * 256 + '1'):
                return &u1;

            case ('u' * 256 + '2'):
                return &u2;

            case ('u' * 256 + '4'):
                return &u4;

            case ('i' * 256 + '1'):
                return &i1;

            case ('i' * 256 + '2'):
                return &i2;

            case ('i' * 256 + '4'):
                return &i4;

            default:
                return 0;
        }
    }
    else if (size == 3)
    {
        switch (dtype[0] * 65536 + dtype[1] * 256 + dtype[2])
        {
            case ('n' * 65536 + 'i' * 256 + '1'):
                return &ni1;

            case ('n' * 65536 + 'i' * 256 + '2'):
                return &ni2;

            case ('n' * 65536 + 'u' * 256 + '1'):
                return &nu1;

            case ('n' * 65536 + 'u' * 256 + '2'):
                return &nu2;

            default:
                return 0;
        }
    }
    return 0;
}

PyObject * MGLContext_buffer(MGLContext * self, PyObject * args) {
    PyObject * data;
    int reserve;
    int dynamic;

    if (!PyArg_ParseTuple(args, "OIp", &data, &reserve, &dynamic)) {
        return 0;
    }

    if (data == Py_None && !reserve) {
        MGLError_Set("missing data or reserve");
        return 0;
    }

    if (data != Py_None && reserve) {
        MGLError_Set("data and reserve are mutually exclusive");
        return 0;
    }

    Py_buffer buffer_view;

    if (data != Py_None) {
        int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
        if (get_buffer < 0) {
            // Propagate the default error
            return 0;
        }
    } else {
        buffer_view.len = reserve;
        buffer_view.buf = 0;
    }

    if (!buffer_view.len) {
        if (data != Py_None) {
            PyBuffer_Release(&buffer_view);
        }
        MGLError_Set("the buffer cannot be empty");
        return 0;
    }

    MGLBuffer * buffer = PyObject_New(MGLBuffer, MGLBuffer_type);
    buffer->released = false;

    buffer->size = (int)buffer_view.len;
    buffer->dynamic = dynamic ? true : false;

    const GLMethods & gl = self->gl;

    buffer->buffer_obj = 0;
    gl.GenBuffers(1, (GLuint *)&buffer->buffer_obj);

    if (!buffer->buffer_obj) {
        MGLError_Set("cannot create buffer");
        Py_DECREF(buffer);
        return 0;
    }

    gl.BindBuffer(GL_ARRAY_BUFFER, buffer->buffer_obj);
    gl.BufferData(GL_ARRAY_BUFFER, buffer->size, buffer_view.buf, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    Py_INCREF(self);
    buffer->context = self;

    if (data != Py_None) {
        PyBuffer_Release(&buffer_view);
    }

    Py_INCREF(buffer);

    PyObject * result = PyTuple_New(3);
    PyTuple_SET_ITEM(result, 0, (PyObject *)buffer);
    PyTuple_SET_ITEM(result, 1, PyLong_FromSsize_t(buffer->size));
    PyTuple_SET_ITEM(result, 2, PyLong_FromLong(buffer->buffer_obj));
    return result;
}

PyObject * MGLBuffer_write(MGLBuffer * self, PyObject * args) {
    PyObject * data;
    Py_ssize_t offset;

    if (!PyArg_ParseTuple(args, "On", &data, &offset)) {
        return 0;
    }

    Py_buffer buffer_view;

    int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
    if (get_buffer < 0) {
        // Propagate the default error
        return 0;
    }

    if (offset < 0 || buffer_view.len + offset > self->size) {
        MGLError_Set("out of range offset = %d or size = %d", offset, buffer_view.len);
        PyBuffer_Release(&buffer_view);
        return 0;
    }

    const GLMethods & gl = self->context->gl;
    gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);
    gl.BufferSubData(GL_ARRAY_BUFFER, (GLintptr)offset, buffer_view.len, buffer_view.buf);
    PyBuffer_Release(&buffer_view);
    Py_RETURN_NONE;
}

PyObject * MGLBuffer_read(MGLBuffer * self, PyObject * args) {
    Py_ssize_t size;
    Py_ssize_t offset;

    if (!PyArg_ParseTuple(args, "nn", &size, &offset)) {
        return 0;
    }

    if (size < 0) {
        size = self->size - offset;
    }

    if (offset < 0 || offset + size > self->size) {
        MGLError_Set("out of rangeoffset = %d or size = %d", offset, size);
        return 0;
    }

    PyObject * res = PyBytes_FromStringAndSize(NULL, size);

    const GLMethods & gl = self->context->gl;
    gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);
    gl.GetBufferSubData(GL_ARRAY_BUFFER, offset, size, PyBytes_AsString(res));
    return res;
}

PyObject * MGLBuffer_write_chunks(MGLBuffer * self, PyObject * args) {
    PyObject * data;
    Py_ssize_t start;
    Py_ssize_t step;
    Py_ssize_t count;

    if (!PyArg_ParseTuple(args, "Onnn", &data, &start, &step, &count)) {
        return 0;
    }

    Py_ssize_t abs_step = step > 0 ? step : -step;

    Py_buffer buffer_view;

    int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
    if (get_buffer < 0) {
        // Propagate the default error
        return 0;
    }

    const GLMethods & gl = self->context->gl;
    gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);

    Py_ssize_t chunk_size = buffer_view.len / count;

    if (buffer_view.len != chunk_size * count) {
        MGLError_Set("data (%d bytes) cannot be divided to %d equal chunks", buffer_view.len, count);
        PyBuffer_Release(&buffer_view);
        return 0;
    }

    if (start < 0) {
        start = self->size + start;
    }

    if (start < 0 || chunk_size > abs_step || start + chunk_size > self->size || start + count * step - step < 0 || start + count * step - step + chunk_size > self->size) {
        MGLError_Set("buffer overflow");
        PyBuffer_Release(&buffer_view);
        return 0;
    }

    char * write_ptr = (char *)gl.MapBufferRange(GL_ARRAY_BUFFER, 0, self->size, GL_MAP_WRITE_BIT);
    char * read_ptr = (char *)buffer_view.buf;

    if (!write_ptr) {
        MGLError_Set("cannot map the buffer");
        PyBuffer_Release(&buffer_view);
        return 0;
    }

    write_ptr += start;
    for (Py_ssize_t i = 0; i < count; ++i) {
        memcpy(write_ptr, read_ptr, chunk_size);
        read_ptr += chunk_size;
        write_ptr += step;
    }

    gl.UnmapBuffer(GL_ARRAY_BUFFER);
    PyBuffer_Release(&buffer_view);
    Py_RETURN_NONE;
}

PyObject * MGLBuffer_read_chunks(MGLBuffer * self, PyObject * args) {
    Py_ssize_t chunk_size;
    Py_ssize_t start;
    Py_ssize_t step;
    Py_ssize_t count;

    if (!PyArg_ParseTuple(args, "nnnn", &chunk_size, &start, &step, &count)) {
        return 0;
    }

    Py_ssize_t abs_step = step > 0 ? step : -step;

    if (start < 0) {
        start = self->size + start;
    }

    if (start < 0 || chunk_size < 0 || chunk_size > abs_step || start + chunk_size > self->size || start + count * step - step < 0 || start + count * step - step + chunk_size > self->size) {
        MGLError_Set("size error");
        return 0;
    }

    const GLMethods & gl = self->context->gl;

    gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);

    char * read_ptr = (char *)gl.MapBufferRange(GL_ARRAY_BUFFER, 0, self->size, GL_MAP_READ_BIT);

    if (!read_ptr) {
        MGLError_Set("cannot map the buffer");
        return 0;
    }

    PyObject * data = PyBytes_FromStringAndSize(0, chunk_size * count);
    char * write_ptr = PyBytes_AS_STRING(data);

    read_ptr += start;
    for (Py_ssize_t i = 0; i < count; ++i) {
        memcpy(write_ptr, read_ptr, chunk_size);
        write_ptr += chunk_size;
        read_ptr += step;
    }

    gl.UnmapBuffer(GL_ARRAY_BUFFER);
    return data;
}

PyObject * MGLBuffer_read_chunks_into(MGLBuffer * self, PyObject * args) {
    PyObject * data;
    Py_ssize_t chunk_size;
    Py_ssize_t start;
    Py_ssize_t step;
    Py_ssize_t count;
    Py_ssize_t write_offset;

    if (!PyArg_ParseTuple(args, "Onnnnn", &data, &chunk_size, &start, &step, &count, &write_offset)) {
        return 0;
    }

    Py_buffer buffer_view;

    int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_WRITABLE);
    if (get_buffer < 0) {
        // Propagate the default error
        return 0;
    }

    const GLMethods & gl = self->context->gl;

    gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);

    char * read_ptr = (char *)gl.MapBufferRange(GL_ARRAY_BUFFER, 0, self->size, GL_MAP_READ_BIT);
    char * write_ptr = (char *)buffer_view.buf + write_offset;

    if (!read_ptr) {
        MGLError_Set("cannot map the buffer");
        return 0;
    }

    read_ptr += start;
    for (Py_ssize_t i = 0; i < count; ++i) {
        memcpy(write_ptr, read_ptr, chunk_size);
        write_ptr += chunk_size;
        read_ptr += step;
    }

    gl.UnmapBuffer(GL_ARRAY_BUFFER);
    PyBuffer_Release(&buffer_view);
    Py_RETURN_NONE;
}

PyObject * MGLBuffer_clear(MGLBuffer * self, PyObject * args) {
    Py_ssize_t size;
    Py_ssize_t offset;
    PyObject * chunk;

    if (!PyArg_ParseTuple(args, "nnO", &size, &offset, &chunk)) {
        return 0;
    }

    if (size < 0) {
        size = self->size - offset;
    }

    Py_buffer buffer_view;

    if (chunk != Py_None) {
        int get_buffer = PyObject_GetBuffer(chunk, &buffer_view, PyBUF_SIMPLE);
        if (get_buffer < 0) {
            // Propagate the default error
            return 0;
        }

        if (size % buffer_view.len != 0) {
            MGLError_Set("the chunk does not fit the size");
            PyBuffer_Release(&buffer_view);
            return 0;
        }
    } else {
        buffer_view.len = 0;
        buffer_view.buf = 0;
    }

    const GLMethods & gl = self->context->gl;
    gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);

    char * map = (char *)gl.MapBufferRange(GL_ARRAY_BUFFER, offset, size, GL_MAP_WRITE_BIT);

    if (!map) {
        MGLError_Set("cannot map the buffer");
        return 0;
    }

    if (buffer_view.len) {
        char * src = (char *)buffer_view.buf;
        Py_ssize_t divisor = buffer_view.len;

        for (Py_ssize_t i = 0; i < size; ++i) {
            map[i] = src[i % divisor];
        }
    } else {
        memset(map + offset, 0, size);
    }

    gl.UnmapBuffer(GL_ARRAY_BUFFER);

    if (chunk != Py_None) {
        PyBuffer_Release(&buffer_view);
    }

    Py_RETURN_NONE;
}

PyObject * MGLBuffer_orphan(MGLBuffer * self, PyObject * args) {
    Py_ssize_t size;

    if (!PyArg_ParseTuple(args, "n", &size)) {
        return 0;
    }

    if (size > 0) {
        self->size = size;
    }

    const GLMethods & gl = self->context->gl;
    gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);
    gl.BufferData(GL_ARRAY_BUFFER, self->size, 0, self->dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    Py_RETURN_NONE;
}

PyObject * MGLBuffer_bind_to_uniform_block(MGLBuffer * self, PyObject * args) {
    int binding;
    Py_ssize_t offset;
    Py_ssize_t size;

    if (!PyArg_ParseTuple(args, "Inn", &binding, &offset, &size)) {
        return 0;
    }

    if (size < 0) {
        size = self->size - offset;
    }

    const GLMethods & gl = self->context->gl;
    gl.BindBufferRange(GL_UNIFORM_BUFFER, binding, self->buffer_obj, offset, size);
    Py_RETURN_NONE;
}

PyObject * MGLBuffer_bind_to_storage_buffer(MGLBuffer * self, PyObject * args) {
    int binding;
    Py_ssize_t offset;
    Py_ssize_t size;

    if (!PyArg_ParseTuple(args, "Inn", &binding, &offset, &size)) {
        return 0;
    }

    if (size < 0) {
        size = self->size - offset;
    }

    const GLMethods & gl = self->context->gl;
    gl.BindBufferRange(GL_SHADER_STORAGE_BUFFER, binding, self->buffer_obj, offset, size);
    Py_RETURN_NONE;
}

PyObject * MGLBuffer_release(MGLBuffer * self) {
    if (self->released) {
        Py_RETURN_NONE;
    }
    self->released = true;

    const GLMethods & gl = self->context->gl;
    gl.DeleteBuffers(1, (GLuint *)&self->buffer_obj);

    Py_DECREF(self->context);
    Py_DECREF(self);
    Py_RETURN_NONE;
}

PyObject * MGLBuffer_size(MGLBuffer * self) {
    return PyLong_FromSsize_t(self->size);
}

int MGLBuffer_tp_as_buffer_get_view(MGLBuffer * self, Py_buffer * view, int flags) {
    int access = (flags == PyBUF_SIMPLE) ? GL_MAP_READ_BIT : (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);

    const GLMethods & gl = self->context->gl;
    gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);
    void * map = gl.MapBufferRange(GL_ARRAY_BUFFER, 0, self->size, access);

    if (!map) {
        PyErr_Format(PyExc_BufferError, "Cannot map buffer");
        view->obj = 0;
        return -1;
    }

    view->buf = map;
    view->len = self->size;
    view->itemsize = 1;

    view->format = 0;
    view->ndim = 0;
    view->shape = 0;
    view->strides = 0;
    view->suboffsets = 0;

    Py_INCREF(self);
    view->obj = (PyObject *)self;
    return 0;
}

void MGLBuffer_tp_as_buffer_release_view(MGLBuffer * self, Py_buffer * view) {
    const GLMethods & gl = self->context->gl;
    gl.UnmapBuffer(GL_ARRAY_BUFFER);
}

FormatIterator::FormatIterator(const char * str) : ptr(str) {
}

FormatInfo FormatIterator::info() {
    FormatInfo info;
    info.size = 0;
    info.nodes = 0;
    info.divisor = 0;
    info.valid = true;

    FormatIterator it = FormatIterator(ptr);
    while (FormatNode * node = it.next()) {
        if (node == InvalidFormat) {
            return FormatInfo::invalid();
        }
        info.size += node->size;
        if (node->type) {
            ++info.nodes;
        }
    }

    char post_chr = *it.ptr++;

    if (post_chr == '/') {
        char per_type = *it.ptr++;

        switch (per_type) {
            case 'i':
                info.divisor = 1;
                break;

            case 'r':
                info.divisor = 0x7fffffff;
                break;

            case 'v':
                break;

            default:
                return FormatInfo::invalid();
        }

        if (*it.ptr) {
            return FormatInfo::invalid();
        }
    }

    return info;
}

FormatNode * FormatIterator::next() {
    node.count = 0;
    while (true) {
        char chr = *ptr++;
        switch (chr) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                node.count = node.count * 10 + chr - '0';
                break;

            case 'f':
                if (node.count == 0) {
                    node.count = 1;
                }
                switch (*ptr++) {
                    case '1':
                        if (*ptr && *ptr != ' ' && *ptr != '/') {
                            return InvalidFormat;
                        }
                        node.size = 1 * node.count;
                        node.type = GL_UNSIGNED_BYTE;
                        node.normalize = true;
                        break;
                    case '2':
                        if (*ptr && *ptr != ' ' && *ptr != '/') {
                            return InvalidFormat;
                        }
                        node.size = 2 * node.count;
                        node.type = GL_HALF_FLOAT;
                        node.normalize = false;
                        break;
                    case '4':
                        if (*ptr && *ptr != ' ' && *ptr != '/') {
                            return InvalidFormat;
                        }
                        node.size = 4 * node.count;
                        node.type = GL_FLOAT;
                        node.normalize = false;
                        break;
                    case '8':
                        if (*ptr && *ptr != ' ' && *ptr != '/') {
                            return InvalidFormat;
                        }
                        node.size = 8 * node.count;
                        node.type = GL_DOUBLE;
                        node.normalize = false;
                        break;
                    case 0:
                    case '/':
                        --ptr;
                    case ' ':
                        node.size = 4 * node.count;
                        node.type = GL_FLOAT;
                        node.normalize = false;
                        break;
                    default:
                        return InvalidFormat;
                }
                return &node;

            case 'i':
                if (node.count == 0) {
                    node.count = 1;
                }
                node.normalize = false;
                switch (*ptr++) {
                    case '1':
                        if (*ptr && *ptr != ' ' && *ptr != '/') {
                            return InvalidFormat;
                        }
                        node.size = 1 * node.count;
                        node.type = GL_BYTE;
                        break;
                    case '2':
                        if (*ptr && *ptr != ' ' && *ptr != '/') {
                            return InvalidFormat;
                        }
                        node.size = 2 * node.count;
                        node.type = GL_SHORT;
                        break;
                    case '4':
                        if (*ptr && *ptr != ' ' && *ptr != '/') {
                            return InvalidFormat;
                        }
                        node.size = 4 * node.count;
                        node.type = GL_INT;
                        break;
                    case 0:
                    case '/':
                        --ptr;
                    case ' ':
                        node.size = 4 * node.count;
                        node.type = GL_INT;
                        break;
                    default:
                        return InvalidFormat;
                }
                return &node;

            case 'u':
                if (node.count == 0) {
                    node.count = 1;
                }
                node.normalize = false;
                switch (*ptr++) {
                    case '1':
                        if (*ptr && *ptr != ' ' && *ptr != '/') {
                            return InvalidFormat;
                        }
                        node.size = 1 * node.count;
                        node.type = GL_UNSIGNED_BYTE;
                        break;
                    case '2':
                        if (*ptr && *ptr != ' ' && *ptr != '/') {
                            return InvalidFormat;
                        }
                        node.size = 2 * node.count;
                        node.type = GL_UNSIGNED_SHORT;
                        break;
                    case '4':
                        if (*ptr && *ptr != ' ' && *ptr != '/') {
                            return InvalidFormat;
                        }
                        node.size = 4 * node.count;
                        node.type = GL_UNSIGNED_INT;
                        break;
                    case 0:
                    case '/':
                        --ptr;
                    case ' ':
                        node.size = 4 * node.count;
                        node.type = GL_UNSIGNED_INT;
                        break;
                    default:
                        return InvalidFormat;
                }
                return &node;

            case 'x':
                if (node.count == 0) {
                    node.count = 1;
                }
                node.type = 0;
                node.normalize = false;
                switch (*ptr++) {
                    case '1':
                        if (*ptr && *ptr != ' ' && *ptr != '/') {
                            return InvalidFormat;
                        }
                        node.size = 1 * node.count;
                        break;
                    case '2':
                        if (*ptr && *ptr != ' ' && *ptr != '/') {
                            return InvalidFormat;
                        }
                        node.size = 2 * node.count;
                        break;
                    case '4':
                        if (*ptr && *ptr != ' ' && *ptr != '/') {
                            return InvalidFormat;
                        }
                        node.size = 4 * node.count;
                        break;
                    case '8':
                        if (*ptr && *ptr != ' ' && *ptr != '/') {
                            return InvalidFormat;
                        }
                        node.size = 8 * node.count;
                        break;
                    case 0:
                    case '/':
                        --ptr;
                    case ' ':
                        node.size = 1 * node.count;
                        break;
                    default:
                        return InvalidFormat;
                }
                return &node;

            case ' ':
                break;

            case 0:
            case '/':
                --ptr;
                return node.count ? InvalidFormat : 0;

            default:
                return InvalidFormat;
        }
    }
}

PyObject * MGLContext_compute_shader(MGLContext * self, PyObject * args) {
    PyObject * source;

    int args_ok = PyArg_ParseTuple(args, "O", &source);
    if (!args_ok) {
        return 0;
    }

    if (!PyUnicode_Check(source)) {
        MGLError_Set("the source must be a string not %s", Py_TYPE(source)->tp_name);
        return 0;
    }

    const char * source_str = PyUnicode_AsUTF8(source);

    MGLComputeShader * compute_shader = PyObject_New(MGLComputeShader, MGLComputeShader_type);
    compute_shader->released = false;

    Py_INCREF(self);
    compute_shader->context = self;

    const GLMethods & gl = self->gl;

    int program_obj = gl.CreateProgram();

    if (!program_obj) {
        MGLError_Set("cannot create program");
        return 0;
    }

    int shader_obj = gl.CreateShader(GL_COMPUTE_SHADER);

    if (!shader_obj) {
        MGLError_Set("cannot create the shader object");
        return 0;
    }

    gl.ShaderSource(shader_obj, 1, &source_str, 0);
    gl.CompileShader(shader_obj);

    int compiled = GL_FALSE;
    gl.GetShaderiv(shader_obj, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        const char * message = "GLSL Compiler failed";
        const char * title = "ComputeShader";
        const char * underline = "=============";

        int log_len = 0;
        gl.GetShaderiv(shader_obj, GL_INFO_LOG_LENGTH, &log_len);

        char * log = new char[log_len];
        gl.GetShaderInfoLog(shader_obj, log_len, &log_len, log);

        gl.DeleteShader(shader_obj);

        MGLError_Set("%s\n\n%s\n%s\n%s\n", message, title, underline, log);

        delete[] log;
        return 0;
    }

    gl.AttachShader(program_obj, shader_obj);
    gl.LinkProgram(program_obj);

    int linked = GL_FALSE;
    gl.GetProgramiv(program_obj, GL_LINK_STATUS, &linked);

    if (!linked) {
        const char * message = "GLSL Linker failed";
        const char * title = "ComputeShader";
        const char * underline = "=============";

        int log_len = 0;
        gl.GetProgramiv(program_obj, GL_INFO_LOG_LENGTH, &log_len);

        char * log = new char[log_len];
        gl.GetProgramInfoLog(program_obj, log_len, &log_len, log);

        gl.DeleteProgram(program_obj);

        MGLError_Set("%s\n\n%s\n%s\n%s\n", message, title, underline, log);

        delete[] log;
        return 0;
    }

    compute_shader->shader_obj = shader_obj;
    compute_shader->program_obj = program_obj;

    Py_INCREF(compute_shader);

    int num_uniforms = 0;
    int num_uniform_blocks = 0;

    gl.GetProgramiv(program_obj, GL_ACTIVE_UNIFORMS, &num_uniforms);
    gl.GetProgramiv(program_obj, GL_ACTIVE_UNIFORM_BLOCKS, &num_uniform_blocks);

    PyObject * members_dict = PyDict_New();

    for (int i = 0; i < num_uniforms; ++i) {
        int type = 0;
        int array_length = 0;
        int name_len = 0;
        char name[256];

        gl.GetActiveUniform(program_obj, i, 256, &name_len, &array_length, (GLenum *)&type, name);
        int location = gl.GetUniformLocation(program_obj, name);

        if (location < 0) {
            continue;
        }

        PyObject * clean_name = PyObject_CallMethod(helper, "clean_glsl_name", "s", name);
        PyObject * item = PyObject_CallMethod(
            helper, "make_uniform", "(NiiiiO)",
            clean_name, type, program_obj, location, array_length, self
        );

        PyDict_SetItemString(members_dict, name, item);
        Py_DECREF(item);
    }

    for (int i = 0; i < num_uniform_blocks; ++i) {
        int size = 0;
        int name_len = 0;
        char name[256];

        gl.GetActiveUniformBlockName(program_obj, i, 256, &name_len, name);
        int index = gl.GetUniformBlockIndex(program_obj, name);
        gl.GetActiveUniformBlockiv(program_obj, index, GL_UNIFORM_BLOCK_DATA_SIZE, &size);

        PyObject * clean_name = PyObject_CallMethod(helper, "clean_glsl_name", "s", name);
        PyObject * item = PyObject_CallMethod(
            helper, "make_uniform_block", "(NiiiO)",
            clean_name, program_obj, index, size, self
        );

        PyDict_SetItemString(members_dict, name, item);
        Py_DECREF(item);
    }

    PyObject * result = PyTuple_New(3);
    PyTuple_SET_ITEM(result, 0, (PyObject *)compute_shader);
    PyTuple_SET_ITEM(result, 1, members_dict);
    PyTuple_SET_ITEM(result, 2, PyLong_FromLong(compute_shader->program_obj));
    return result;
}

PyObject * MGLComputeShader_run(MGLComputeShader * self, PyObject * args) {
    unsigned x;
    unsigned y;
    unsigned z;

    int args_ok = PyArg_ParseTuple(args, "III", &x, &y, &z);
    if (!args_ok) {
        return 0;
    }

    const GLMethods & gl = self->context->gl;

    gl.UseProgram(self->program_obj);
    gl.DispatchCompute(x, y, z);

    Py_RETURN_NONE;
}

PyObject * MGLComputeShader_release(MGLComputeShader * self) {
    if (self->released) {
        Py_RETURN_NONE;
    }
    self->released = true;

    const GLMethods & gl = self->context->gl;
    gl.DeleteShader(self->shader_obj);
    gl.DeleteProgram(self->program_obj);

    Py_DECREF(self->context);
    Py_DECREF(self);
    Py_RETURN_NONE;
}

PyObject * MGLContext_enable_only(MGLContext * self, PyObject * args) {
    int flags;

    int args_ok = PyArg_ParseTuple(args, "i", &flags);
    if (!args_ok) {
        return 0;
    }

    self->enable_flags = flags;

    if (flags & MGL_BLEND) {
        self->gl.Enable(GL_BLEND);
    } else {
        self->gl.Disable(GL_BLEND);
    }

    if (flags & MGL_DEPTH_TEST) {
        self->gl.Enable(GL_DEPTH_TEST);
    } else {
        self->gl.Disable(GL_DEPTH_TEST);
    }

    if (flags & MGL_CULL_FACE) {
        self->gl.Enable(GL_CULL_FACE);
    } else {
        self->gl.Disable(GL_CULL_FACE);
    }

    if (flags & MGL_RASTERIZER_DISCARD) {
        self->gl.Enable(GL_RASTERIZER_DISCARD);
    } else {
        self->gl.Disable(GL_RASTERIZER_DISCARD);
    }

    if (flags & MGL_PROGRAM_POINT_SIZE) {
        self->gl.Enable(GL_PROGRAM_POINT_SIZE);
    } else {
        self->gl.Disable(GL_PROGRAM_POINT_SIZE);
    }

    Py_RETURN_NONE;
}

PyObject * MGLContext_enable(MGLContext * self, PyObject * args) {
    int flags;

    int args_ok = PyArg_ParseTuple(args, "i", &flags);
    if (!args_ok) {
        return 0;
    }

    self->enable_flags |= flags;

    if (flags & MGL_BLEND) {
        self->gl.Enable(GL_BLEND);
    }

    if (flags & MGL_DEPTH_TEST) {
        self->gl.Enable(GL_DEPTH_TEST);
    }

    if (flags & MGL_CULL_FACE) {
        self->gl.Enable(GL_CULL_FACE);
    }

    if (flags & MGL_RASTERIZER_DISCARD) {
        self->gl.Enable(GL_RASTERIZER_DISCARD);
    }

    if (flags & MGL_PROGRAM_POINT_SIZE) {
        self->gl.Enable(GL_PROGRAM_POINT_SIZE);
    }

    Py_RETURN_NONE;
}

PyObject * MGLContext_disable(MGLContext * self, PyObject * args) {
    int flags;

    int args_ok = PyArg_ParseTuple(args, "i", &flags);
    if (!args_ok) {
        return 0;
    }

    self->enable_flags &= ~flags;

    if (flags & MGL_BLEND) {
        self->gl.Disable(GL_BLEND);
    }

    if (flags & MGL_DEPTH_TEST) {
        self->gl.Disable(GL_DEPTH_TEST);
    }

    if (flags & MGL_CULL_FACE) {
        self->gl.Disable(GL_CULL_FACE);
    }

    if (flags & MGL_RASTERIZER_DISCARD) {
        self->gl.Disable(GL_RASTERIZER_DISCARD);
    }

    if (flags & MGL_PROGRAM_POINT_SIZE) {
        self->gl.Disable(GL_PROGRAM_POINT_SIZE);
    }

    Py_RETURN_NONE;
}

PyObject * MGLContext_enable_direct(MGLContext * self, PyObject * args) {
    int value;

    int args_ok = PyArg_ParseTuple(args, "i", &value);
    if (!args_ok) {
        return 0;
    }

    self->gl.Enable(value);
    Py_RETURN_NONE;
}

PyObject * MGLContext_disable_direct(MGLContext * self, PyObject * args) {
    int value;

    int args_ok = PyArg_ParseTuple(args, "i", &value);
    if (!args_ok) {
        return 0;
    }

    self->gl.Disable(value);
    Py_RETURN_NONE;
}

PyObject * MGLContext_finish(MGLContext * self) {
    self->gl.Finish();
    Py_RETURN_NONE;
}

PyObject * MGLContext_copy_buffer(MGLContext * self, PyObject * args) {
    MGLBuffer * dst;
    MGLBuffer * src;

    Py_ssize_t size;
    Py_ssize_t read_offset;
    Py_ssize_t write_offset;

    int args_ok = PyArg_ParseTuple(
        args,
        "O!O!nnn",
        MGLBuffer_type,
        &dst,
        MGLBuffer_type,
        &src,
        &size,
        &read_offset,
        &write_offset
    );

    if (!args_ok) {
        return 0;
    }

    if (size < 0) {
        size = src->size - read_offset;
    }

    if (read_offset < 0 || write_offset < 0) {
        MGLError_Set("buffer underflow");
        return 0;
    }

    if (read_offset + size > src->size || write_offset + size > dst->size) {
        MGLError_Set("buffer overflow");
        return 0;
    }

    const GLMethods & gl = self->gl;

    gl.BindBuffer(GL_COPY_READ_BUFFER, src->buffer_obj);
    gl.BindBuffer(GL_COPY_WRITE_BUFFER, dst->buffer_obj);
    gl.CopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, read_offset, write_offset, size);

    Py_RETURN_NONE;
}

PyObject * MGLContext_copy_framebuffer(MGLContext * self, PyObject * args) {
    PyObject * dst;
    MGLFramebuffer * src;

    int args_ok = PyArg_ParseTuple(args, "OO!", &dst, MGLFramebuffer_type, &src);
    if (!args_ok) {
        return 0;
    }

    const GLMethods & gl = self->gl;

    // If the sizes of the source and destination rectangles are not equal,
    // filter specifies the interpolation method that will be applied to resize the source image,
    // and must be GL_NEAREST or GL_LINEAR. GL_LINEAR is only a valid interpolation
    // method for the color buffer. If filter is not GL_NEAREST and mask includes
    // GL_DEPTH_BUFFER_BIT or GL_STENCIL_BUFFER_BIT, no data is transferred and a
    // GL_INVALID_OPERATION error is generated.

    if (Py_TYPE(dst) == MGLFramebuffer_type) {

        MGLFramebuffer * dst_framebuffer = (MGLFramebuffer *)dst;

        int width = 0;
        int height = 0;

        if (!dst_framebuffer->framebuffer_obj) {
            width = src->width;
            height = src->height;
        } else if (!src->framebuffer_obj) {
            width = dst_framebuffer->width;
            height = dst_framebuffer->height;
        } else {
            width = src->width < dst_framebuffer->width ? src->width : dst_framebuffer->width;
            height = src->height < dst_framebuffer->height ? src->height : dst_framebuffer->height;
        }

        if (dst_framebuffer->draw_buffers_len != src->draw_buffers_len)
        {
            MGLError_Set("Destination and source framebuffers have different number of color attachments!");
            return 0;
        }


        int prev_read_buffer = -1;
        int prev_draw_buffer = -1;
        int color_attachment_len = dst_framebuffer->draw_buffers_len;
        gl.GetIntegerv(GL_READ_BUFFER, &prev_read_buffer);
        gl.GetIntegerv(GL_DRAW_BUFFER, &prev_draw_buffer);
        gl.BindFramebuffer(GL_READ_FRAMEBUFFER, src->framebuffer_obj);
        gl.BindFramebuffer(GL_DRAW_FRAMEBUFFER, dst_framebuffer->framebuffer_obj);

        for (int i = 0; i < color_attachment_len; ++i)
        {
            gl.ReadBuffer(src->draw_buffers[i]);
            gl.DrawBuffer(dst_framebuffer->draw_buffers[i]);

            gl.BlitFramebuffer(
                0, 0, width, height,
                0, 0, width, height,
                GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
                GL_NEAREST
            );
        }
        gl.BindFramebuffer(GL_FRAMEBUFFER, self->bound_framebuffer->framebuffer_obj);
        gl.ReadBuffer(prev_read_buffer);
        gl.DrawBuffer(prev_draw_buffer);
        gl.DrawBuffers(self->bound_framebuffer->draw_buffers_len, self->bound_framebuffer->draw_buffers);

    } else if (Py_TYPE(dst) == MGLTexture_type) {

        MGLTexture * dst_texture = (MGLTexture *)dst;

        if (dst_texture->samples) {
            MGLError_Set("multisample texture targets are not accepted");
            return 0;
        }

        if (src->samples) {
            MGLError_Set("multisample framebuffer source with texture targets are not accepted");
            return 0;
        }

        int width = src->width < dst_texture->width ? src->width : dst_texture->width;
        int height = src->height < dst_texture->height ? src->height : dst_texture->height;

        if (!src->framebuffer_obj) {
            width = dst_texture->width;
            height = dst_texture->height;
        } else {
            width = src->width < dst_texture->width ? src->width : dst_texture->width;
            height = src->height < dst_texture->height ? src->height : dst_texture->height;
        }

        const int formats[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};
        int texture_target = dst_texture->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        int format = formats[dst_texture->components];

        gl.BindFramebuffer(GL_READ_FRAMEBUFFER, src->framebuffer_obj);
        gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);
        gl.BindTexture(GL_TEXTURE_2D, dst_texture->texture_obj);
        gl.CopyTexImage2D(texture_target, 0, format, 0, 0, width, height, 0);
        gl.BindFramebuffer(GL_FRAMEBUFFER, self->bound_framebuffer->framebuffer_obj);

    } else {

        MGLError_Set("the dst must be a Framebuffer or Texture");
        return 0;

    }

    Py_RETURN_NONE;
}

PyObject * MGLContext_detect_framebuffer(MGLContext * self, PyObject * args) {
    PyObject * glo;

    int args_ok = PyArg_ParseTuple(args, "O", &glo);
    if (!args_ok) {
        return 0;
    }

    const GLMethods & gl = self->gl;

    int bound_framebuffer = 0;
    gl.GetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &bound_framebuffer);

    int framebuffer_obj = bound_framebuffer;
    if (glo != Py_None) {
        framebuffer_obj = PyLong_AsLong(glo);
        if (PyErr_Occurred()) {
            MGLError_Set("the glo must be an integer");
            return 0;
        }
    }

    if (!framebuffer_obj) {
        PyObject * size = PyTuple_New(2);
        PyTuple_SET_ITEM(size, 0, PyLong_FromLong(self->default_framebuffer->width));
        PyTuple_SET_ITEM(size, 1, PyLong_FromLong(self->default_framebuffer->height));

        Py_INCREF(self->default_framebuffer);
        PyObject * result = PyTuple_New(4);
        PyTuple_SET_ITEM(result, 0, (PyObject *)self->default_framebuffer);
        PyTuple_SET_ITEM(result, 1, size);
        PyTuple_SET_ITEM(result, 2, PyLong_FromLong(self->default_framebuffer->samples));
        PyTuple_SET_ITEM(result, 3, PyLong_FromLong(self->default_framebuffer->framebuffer_obj));
        return result;
    }

    gl.BindFramebuffer(GL_FRAMEBUFFER, framebuffer_obj);

    int num_color_attachments = self->max_color_attachments;

    for (int i = 0; i < self->max_color_attachments; ++i) {
        int color_attachment_type = 0;
        gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &color_attachment_type);

        if (!color_attachment_type) {
            num_color_attachments = i;
            break;
        }
    }

    int color_attachment_type = 0;
    gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &color_attachment_type);

    int color_attachment_name = 0;
    gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &color_attachment_name);

    int width = 0;
    int height = 0;

    switch (color_attachment_type) {
        case GL_RENDERBUFFER: {
            gl.BindRenderbuffer(GL_RENDERBUFFER, color_attachment_name);
            gl.GetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
            gl.GetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
            break;
        }
        case GL_TEXTURE: {
            gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);
            gl.BindTexture(GL_TEXTURE_2D, color_attachment_name);
            gl.GetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
            gl.GetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
            break;
        }
    }

    MGLFramebuffer * framebuffer = PyObject_New(MGLFramebuffer, MGLFramebuffer_type);
    framebuffer->released = false;

    framebuffer->framebuffer_obj = framebuffer_obj;

    framebuffer->draw_buffers_len = num_color_attachments;
    framebuffer->draw_buffers = new unsigned[num_color_attachments];
    framebuffer->color_mask = new bool[4 * num_color_attachments];

    for (int i = 0; i < num_color_attachments; ++i) {
        framebuffer->draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
        framebuffer->color_mask[i * 4 + 0] = true;
        framebuffer->color_mask[i * 4 + 1] = true;
        framebuffer->color_mask[i * 4 + 2] = true;
        framebuffer->color_mask[i * 4 + 3] = true;
    }

    framebuffer->depth_mask = true;

    framebuffer->context = self;

    framebuffer->viewport_x = 0;
    framebuffer->viewport_y = 0;
    framebuffer->viewport_width = width;
    framebuffer->viewport_height = height;

    framebuffer->scissor_enabled = false;
    framebuffer->scissor_x = 0;
    framebuffer->scissor_y = 0;
    framebuffer->scissor_width = width;
    framebuffer->scissor_height = height;

    framebuffer->width = width;
    framebuffer->height = height;
    framebuffer->dynamic = true;

    gl.BindFramebuffer(GL_FRAMEBUFFER, bound_framebuffer);

    Py_INCREF(framebuffer);

    PyObject * size = PyTuple_New(2);
    PyTuple_SET_ITEM(size, 0, PyLong_FromLong(framebuffer->width));
    PyTuple_SET_ITEM(size, 1, PyLong_FromLong(framebuffer->height));

    Py_INCREF(framebuffer);
    PyObject * result = PyTuple_New(4);
    PyTuple_SET_ITEM(result, 0, (PyObject *)framebuffer);
    PyTuple_SET_ITEM(result, 1, size);
    PyTuple_SET_ITEM(result, 2, PyLong_FromLong(framebuffer->samples));
    PyTuple_SET_ITEM(result, 3, PyLong_FromLong(framebuffer->framebuffer_obj));
    return result;
}

PyObject * MGLContext_clear_samplers(MGLContext * self, PyObject * args) {
    int start;
    int end;

    int args_ok = PyArg_ParseTuple(args, "ii", &start, &end);
    if (!args_ok) {
        return 0;
    }

    start = MGL_max(start, 0);
    if (end == -1) {
        end = self->max_texture_units;
    } else {
        end = MGL_min(end, self->max_texture_units);
    }

    const GLMethods & gl = self->gl;

    for(int i = start; i < end; i++) {
        gl.BindSampler(i, 0);
    }

    Py_RETURN_NONE;
}

PyObject * MGLContext_enter(MGLContext * self) {
    return PyObject_CallMethod(self->ctx, "__enter__", NULL);
}

PyObject * MGLContext_exit(MGLContext * self) {
    return PyObject_CallMethod(self->ctx, "__exit__", NULL);
}

PyObject * MGLContext_release(MGLContext * self) {
    if (self->released) {
        Py_RETURN_NONE;
    }
    self->released = true;

    PyObject * temp = PyObject_CallMethod(self->ctx, "release", NULL);
    if (!temp) {
        return NULL;
    }

    Py_DECREF(temp);
    Py_DECREF(self);
    Py_RETURN_NONE;
}

PyObject * MGLContext_get_ubo_binding(MGLContext * self, PyObject * args) {
    int program_obj;
    int index;
    if (!PyArg_ParseTuple(args, "II", &program_obj, &index)) {
        return NULL;
    }
    int binding = 0;
    self->gl.GetActiveUniformBlockiv(program_obj, index, GL_UNIFORM_BLOCK_BINDING, &binding);
    return PyLong_FromLong(binding);
}

PyObject * MGLContext_set_ubo_binding(MGLContext * self, PyObject * args) {
    int program_obj;
    int index;
    int binding;
    if (!PyArg_ParseTuple(args, "III", &program_obj, &index, &binding)) {
        return NULL;
    }
    self->gl.UniformBlockBinding(program_obj, index, binding);
    Py_RETURN_NONE;
}

PyObject * MGLContext_read_uniform(MGLContext * self, PyObject * args) {
    int program_obj;
    int location;
    int gl_type;
    int array_length;
    int element_size;

    if (!PyArg_ParseTuple(args, "IIIII", &program_obj, &location, &gl_type, &array_length, &element_size)) {
        return NULL;
    }

    int size = array_length * element_size;
    PyObject * res = PyBytes_FromStringAndSize(NULL, size);
    char * ptr = PyBytes_AsString(res);

    const GLMethods & gl = self->gl;

    for (int i = 0; i < array_length; ++i) {
        switch (gl_type) {
            case GL_BOOL: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_BOOL_VEC2: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_BOOL_VEC3: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_BOOL_VEC4: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_INT: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_INT_VEC2: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_INT_VEC3: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_INT_VEC4: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_UNSIGNED_INT: gl.GetUniformuiv(program_obj, location + i, (unsigned *)(ptr + i * element_size)); break;
            case GL_UNSIGNED_INT_VEC2: gl.GetUniformuiv(program_obj, location + i, (unsigned *)(ptr + i * element_size)); break;
            case GL_UNSIGNED_INT_VEC3: gl.GetUniformuiv(program_obj, location + i, (unsigned *)(ptr + i * element_size)); break;
            case GL_UNSIGNED_INT_VEC4: gl.GetUniformuiv(program_obj, location + i, (unsigned *)(ptr + i * element_size)); break;
            case GL_FLOAT: gl.GetUniformfv(program_obj, location + i, (float *)(ptr + i * element_size)); break;
            case GL_FLOAT_VEC2: gl.GetUniformfv(program_obj, location + i, (float *)(ptr + i * element_size)); break;
            case GL_FLOAT_VEC3: gl.GetUniformfv(program_obj, location + i, (float *)(ptr + i * element_size)); break;
            case GL_FLOAT_VEC4: gl.GetUniformfv(program_obj, location + i, (float *)(ptr + i * element_size)); break;
            case GL_DOUBLE: gl.GetUniformdv(program_obj, location + i, (double *)(ptr + i * element_size)); break;
            case GL_DOUBLE_VEC2: gl.GetUniformdv(program_obj, location + i, (double *)(ptr + i * element_size)); break;
            case GL_DOUBLE_VEC3: gl.GetUniformdv(program_obj, location + i, (double *)(ptr + i * element_size)); break;
            case GL_DOUBLE_VEC4: gl.GetUniformdv(program_obj, location + i, (double *)(ptr + i * element_size)); break;
            case GL_SAMPLER_1D: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_SAMPLER_1D_ARRAY: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_INT_SAMPLER_1D: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_INT_SAMPLER_1D_ARRAY: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_SAMPLER_2D: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_INT_SAMPLER_2D: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_UNSIGNED_INT_SAMPLER_2D: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_SAMPLER_2D_ARRAY: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_INT_SAMPLER_2D_ARRAY: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_SAMPLER_3D: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_INT_SAMPLER_3D: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_UNSIGNED_INT_SAMPLER_3D: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_SAMPLER_2D_SHADOW: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_SAMPLER_2D_MULTISAMPLE: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_INT_SAMPLER_2D_MULTISAMPLE: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_SAMPLER_CUBE: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_INT_SAMPLER_CUBE: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_UNSIGNED_INT_SAMPLER_CUBE: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_IMAGE_2D: gl.GetUniformiv(program_obj, location + i, (int *)(ptr + i * element_size)); break;
            case GL_FLOAT_MAT2: gl.GetUniformfv(program_obj, location + i, (float *)(ptr + i * element_size)); break;
            case GL_FLOAT_MAT2x3: gl.GetUniformfv(program_obj, location + i, (float *)(ptr + i * element_size)); break;
            case GL_FLOAT_MAT2x4: gl.GetUniformfv(program_obj, location + i, (float *)(ptr + i * element_size)); break;
            case GL_FLOAT_MAT3x2: gl.GetUniformfv(program_obj, location + i, (float *)(ptr + i * element_size)); break;
            case GL_FLOAT_MAT3: gl.GetUniformfv(program_obj, location + i, (float *)(ptr + i * element_size)); break;
            case GL_FLOAT_MAT3x4: gl.GetUniformfv(program_obj, location + i, (float *)(ptr + i * element_size)); break;
            case GL_FLOAT_MAT4x2: gl.GetUniformfv(program_obj, location + i, (float *)(ptr + i * element_size)); break;
            case GL_FLOAT_MAT4x3: gl.GetUniformfv(program_obj, location + i, (float *)(ptr + i * element_size)); break;
            case GL_FLOAT_MAT4: gl.GetUniformfv(program_obj, location + i, (float *)(ptr + i * element_size)); break;
            case GL_DOUBLE_MAT2: gl.GetUniformdv(program_obj, location + i, (double *)(ptr + i * element_size)); break;
            case GL_DOUBLE_MAT2x3: gl.GetUniformdv(program_obj, location + i, (double *)(ptr + i * element_size)); break;
            case GL_DOUBLE_MAT2x4: gl.GetUniformdv(program_obj, location + i, (double *)(ptr + i * element_size)); break;
            case GL_DOUBLE_MAT3x2: gl.GetUniformdv(program_obj, location + i, (double *)(ptr + i * element_size)); break;
            case GL_DOUBLE_MAT3: gl.GetUniformdv(program_obj, location + i, (double *)(ptr + i * element_size)); break;
            case GL_DOUBLE_MAT3x4: gl.GetUniformdv(program_obj, location + i, (double *)(ptr + i * element_size)); break;
            case GL_DOUBLE_MAT4x2: gl.GetUniformdv(program_obj, location + i, (double *)(ptr + i * element_size)); break;
            case GL_DOUBLE_MAT4x3: gl.GetUniformdv(program_obj, location + i, (double *)(ptr + i * element_size)); break;
            case GL_DOUBLE_MAT4: gl.GetUniformdv(program_obj, location + i, (double *)(ptr + i * element_size)); break;
        }
    }

    return res;
}

PyObject * MGLContext_write_uniform(MGLContext * self, PyObject * args) {
    int program_obj;
    int location;
    int gl_type;
    int array_length;
    Py_buffer view = {};

    if (!PyArg_ParseTuple(args, "IIIIy*", &program_obj, &location, &gl_type, &array_length, &view)) {
        return NULL;
    }

    const GLMethods & gl = self->gl;
    char * ptr = (char *)view.buf;

    gl.UseProgram(program_obj);

    switch (gl_type) {
        case GL_BOOL: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_BOOL_VEC2: gl.Uniform2iv(location, array_length, (int *)ptr); break;
        case GL_BOOL_VEC3: gl.Uniform3iv(location, array_length, (int *)ptr); break;
        case GL_BOOL_VEC4: gl.Uniform4iv(location, array_length, (int *)ptr); break;
        case GL_INT: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_INT_VEC2: gl.Uniform2iv(location, array_length, (int *)ptr); break;
        case GL_INT_VEC3: gl.Uniform3iv(location, array_length, (int *)ptr); break;
        case GL_INT_VEC4: gl.Uniform4iv(location, array_length, (int *)ptr); break;
        case GL_UNSIGNED_INT: gl.Uniform1uiv(location, array_length, (unsigned *)ptr); break;
        case GL_UNSIGNED_INT_VEC2: gl.Uniform2uiv(location, array_length, (unsigned *)ptr); break;
        case GL_UNSIGNED_INT_VEC3: gl.Uniform3uiv(location, array_length, (unsigned *)ptr); break;
        case GL_UNSIGNED_INT_VEC4: gl.Uniform4uiv(location, array_length, (unsigned *)ptr); break;
        case GL_FLOAT: gl.Uniform1fv(location, array_length, (float *)ptr); break;
        case GL_FLOAT_VEC2: gl.Uniform2fv(location, array_length, (float *)ptr); break;
        case GL_FLOAT_VEC3: gl.Uniform3fv(location, array_length, (float *)ptr); break;
        case GL_FLOAT_VEC4: gl.Uniform4fv(location, array_length, (float *)ptr); break;
        case GL_DOUBLE: gl.Uniform1dv(location, array_length, (double *)ptr); break;
        case GL_DOUBLE_VEC2: gl.Uniform2dv(location, array_length, (double *)ptr); break;
        case GL_DOUBLE_VEC3: gl.Uniform3dv(location, array_length, (double *)ptr); break;
        case GL_DOUBLE_VEC4: gl.Uniform4dv(location, array_length, (double *)ptr); break;
        case GL_SAMPLER_1D: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_SAMPLER_1D_ARRAY: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_INT_SAMPLER_1D: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_INT_SAMPLER_1D_ARRAY: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_SAMPLER_2D: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_INT_SAMPLER_2D: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_UNSIGNED_INT_SAMPLER_2D: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_SAMPLER_2D_ARRAY: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_INT_SAMPLER_2D_ARRAY: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_SAMPLER_3D: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_INT_SAMPLER_3D: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_UNSIGNED_INT_SAMPLER_3D: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_SAMPLER_2D_SHADOW: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_SAMPLER_2D_MULTISAMPLE: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_INT_SAMPLER_2D_MULTISAMPLE: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_SAMPLER_CUBE: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_INT_SAMPLER_CUBE: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_UNSIGNED_INT_SAMPLER_CUBE: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_IMAGE_2D: gl.Uniform1iv(location, array_length, (int *)ptr); break;
        case GL_FLOAT_MAT2: gl.UniformMatrix2fv(location, array_length, false, (float *)ptr); break;
        case GL_FLOAT_MAT2x3: gl.UniformMatrix2x3fv(location, array_length, false, (float *)ptr); break;
        case GL_FLOAT_MAT2x4: gl.UniformMatrix2x4fv(location, array_length, false, (float *)ptr); break;
        case GL_FLOAT_MAT3x2: gl.UniformMatrix3x2fv(location, array_length, false, (float *)ptr); break;
        case GL_FLOAT_MAT3: gl.UniformMatrix3fv(location, array_length, false, (float *)ptr); break;
        case GL_FLOAT_MAT3x4: gl.UniformMatrix3x4fv(location, array_length, false, (float *)ptr); break;
        case GL_FLOAT_MAT4x2: gl.UniformMatrix4x2fv(location, array_length, false, (float *)ptr); break;
        case GL_FLOAT_MAT4x3: gl.UniformMatrix4x3fv(location, array_length, false, (float *)ptr); break;
        case GL_FLOAT_MAT4: gl.UniformMatrix4fv(location, array_length, false, (float *)ptr); break;
        case GL_DOUBLE_MAT2: gl.UniformMatrix2dv(location, array_length, false, (double *)ptr); break;
        case GL_DOUBLE_MAT2x3: gl.UniformMatrix2x3dv(location, array_length, false, (double *)ptr); break;
        case GL_DOUBLE_MAT2x4: gl.UniformMatrix2x4dv(location, array_length, false, (double *)ptr); break;
        case GL_DOUBLE_MAT3x2: gl.UniformMatrix3x2dv(location, array_length, false, (double *)ptr); break;
        case GL_DOUBLE_MAT3: gl.UniformMatrix3dv(location, array_length, false, (double *)ptr); break;
        case GL_DOUBLE_MAT3x4: gl.UniformMatrix3x4dv(location, array_length, false, (double *)ptr); break;
        case GL_DOUBLE_MAT4x2: gl.UniformMatrix4x2dv(location, array_length, false, (double *)ptr); break;
        case GL_DOUBLE_MAT4x3: gl.UniformMatrix4x3dv(location, array_length, false, (double *)ptr); break;
        case GL_DOUBLE_MAT4: gl.UniformMatrix4dv(location, array_length, false, (double *)ptr); break;
    }

    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

PyObject * MGLContext_get_line_width(MGLContext * self) {
    float line_width = 0.0f;
    self->gl.GetFloatv(GL_LINE_WIDTH, &line_width);
    return PyFloat_FromDouble(line_width);
}

int MGLContext_set_line_width(MGLContext * self, PyObject * value) {
    float line_width = (float)PyFloat_AsDouble(value);
    if (PyErr_Occurred()) {
        return -1;
    }

    self->gl.LineWidth(line_width);
    return 0;
}

PyObject * MGLContext_get_point_size(MGLContext * self) {
    float point_size = 0.0f;
    self->gl.GetFloatv(GL_POINT_SIZE, &point_size);
    return PyFloat_FromDouble(point_size);
}

int MGLContext_set_point_size(MGLContext * self, PyObject * value) {
    float point_size = (float)PyFloat_AsDouble(value);
    if (PyErr_Occurred()) {
        return -1;
    }

    self->gl.PointSize(point_size);
    return 0;
}

// NOTE: currently never called from python
PyObject * MGLContext_get_blend_func(MGLContext * self) {
    PyObject * res = PyTuple_New(2);
    PyTuple_SET_ITEM(res, 0, PyLong_FromLong(self->blend_func_src));
    PyTuple_SET_ITEM(res, 1, PyLong_FromLong(self->blend_func_dst));
    return res;
}

int MGLContext_set_blend_func(MGLContext * self, PyObject * value) {
    Py_ssize_t num_values = PyTuple_GET_SIZE(value);

    if (!(num_values == 2 || num_values == 4)) {
        MGLError_Set("Invalid number of values. Must be 2 or 4.");
        return -1;
    }

    int src_rgb = (int)PyLong_AsLong(PyTuple_GET_ITEM(value, 0));
    int dst_rgb = (int)PyLong_AsLong(PyTuple_GET_ITEM(value, 1));
    int src_alpha = src_rgb;
    int dst_alpha = dst_rgb;

    if (num_values == 4) {
        src_alpha = (int)PyLong_AsLong(PyTuple_GET_ITEM(value, 2));
        dst_alpha = (int)PyLong_AsLong(PyTuple_GET_ITEM(value, 3));
    }

    if (PyErr_Occurred()) {
        return -1;
    }

    self->gl.BlendFuncSeparate(src_rgb, dst_rgb, src_alpha, dst_alpha);
    return 0;
}

// NOTE: currently never called from python
PyObject * MGLContext_get_blend_equation(MGLContext * self) {
    PyObject * res = PyTuple_New(2);
    PyTuple_SET_ITEM(res, 0, PyLong_FromLong(GL_FUNC_ADD));
    PyTuple_SET_ITEM(res, 1, PyLong_FromLong(GL_FUNC_ADD));
    return res;
}

int MGLContext_set_blend_equation(MGLContext * self, PyObject * value) {
    Py_ssize_t num_values = PyTuple_GET_SIZE(value);

    if (!(num_values == 1 || num_values == 2)) {
        MGLError_Set("Invalid number of values. Must be 1 or 2.");
        return -1;
    }

    int mode_rgb = (int)PyLong_AsLong(PyTuple_GET_ITEM(value, 0));
    int mode_alpha = mode_rgb;
    if (num_values == 2) {
        mode_alpha = (int)PyLong_AsLong(PyTuple_GET_ITEM(value, 1));
    }

    if (PyErr_Occurred()) {
        return -1;
    }

    self->gl.BlendEquationSeparate(mode_rgb, mode_alpha);
    return 0;
}

PyObject * MGLContext_get_depth_func(MGLContext * self) {
    return PyObject_CallMethod(helper, "compare_func_to_str", "(i)", self->depth_func);
}

int MGLContext_set_depth_func(MGLContext * self, PyObject * value) {
    PyObject * compare_func = PyObject_CallMethod(helper, "compare_func_from_str", "(O)", value);
    if (!compare_func) {
        return -1;
    }

    int depth_func = PyLong_AsLong(compare_func);
    if (!depth_func) {
        MGLError_Set("depth_func cannot be set to None");
    }

    self->depth_func = depth_func;
    self->gl.DepthFunc(self->depth_func);
    return 0;
}

PyObject * MGLContext_get_multisample(MGLContext * self) {
    return PyBool_FromLong(self->multisample);
}

int MGLContext_set_multisample(MGLContext * self, PyObject * value) {
    if (value == Py_True) {
        self->gl.Enable(GL_MULTISAMPLE);
        self->multisample = true;
        return 0;
    } else if (value == Py_False) {
        self->gl.Disable(GL_MULTISAMPLE);
        self->multisample = false;
        return 0;
    }
    return -1;
}

PyObject * MGLContext_get_provoking_vertex(MGLContext * self) {
    return PyLong_FromLong(self->provoking_vertex);
}

int MGLContext_set_provoking_vertex(MGLContext * self, PyObject * value) {
    int provoking_vertex_value = PyLong_AsLong(value);
    const GLMethods & gl = self->gl;

    if (provoking_vertex_value == GL_FIRST_VERTEX_CONVENTION || provoking_vertex_value == GL_LAST_VERTEX_CONVENTION) {
        gl.ProvokingVertex(provoking_vertex_value);
        self->provoking_vertex = provoking_vertex_value;
        return 0;
    }
    return -1;
}

PyObject * MGLContext_get_polygon_offset(MGLContext * self) {
    return Py_BuildValue("ff", self->polygon_offset_factor, self->polygon_offset_units);
}

int MGLContext_set_polygon_offset(MGLContext * self, PyObject * value) {
    if (!PyTuple_CheckExact(value) || PyTuple_Size(value) != 2) {
        return -1;
    }
    float polygon_offset_factor = (float)PyFloat_AsDouble(PyTuple_GetItem(value, 0));
    float polygon_offset_units = (float)PyFloat_AsDouble(PyTuple_GetItem(value, 1));

    const GLMethods & gl = self->gl;
    if (polygon_offset_factor || polygon_offset_units) {
        gl.Enable(GL_POLYGON_OFFSET_POINT);
        gl.Enable(GL_POLYGON_OFFSET_LINE);
        gl.Enable(GL_POLYGON_OFFSET_FILL);
    } else {
        gl.Disable(GL_POLYGON_OFFSET_POINT);
        gl.Disable(GL_POLYGON_OFFSET_LINE);
        gl.Disable(GL_POLYGON_OFFSET_FILL);
    }
    gl.PolygonOffset(polygon_offset_factor, polygon_offset_units);
    self->polygon_offset_factor = polygon_offset_factor;
    self->polygon_offset_units = polygon_offset_units;
    return 0;
}

PyObject * MGLContext_get_default_texture_unit(MGLContext * self) {
    return PyLong_FromLong(self->default_texture_unit);
}

int MGLContext_set_default_texture_unit(MGLContext * self, PyObject * value) {
    int default_texture_unit = PyLong_AsLong(value);
    if (PyErr_Occurred()) {
        return -1;
    }

    self->default_texture_unit = default_texture_unit;
    return 0;
}

PyObject * MGLContext_get_max_samples(MGLContext * self) {
    return PyLong_FromLong(self->max_samples);
}

PyObject * MGLContext_get_max_integer_samples(MGLContext * self) {
    return PyLong_FromLong(self->max_integer_samples);
}

PyObject * MGLContext_get_max_texture_units(MGLContext * self) {
    return PyLong_FromLong(self->max_texture_units);
}

PyObject * MGLContext_get_max_anisotropy(MGLContext * self) {
    return PyFloat_FromDouble(self->max_anisotropy);
}

MGLFramebuffer * MGLContext_get_fbo(MGLContext * self) {
    Py_INCREF(self->bound_framebuffer);
    return self->bound_framebuffer;
}

int MGLContext_set_fbo(MGLContext * self, PyObject * value) {
    if (Py_TYPE(value) != MGLFramebuffer_type) {
        return -1;
    }
    Py_INCREF(value);
    Py_DECREF(self->bound_framebuffer);
    self->bound_framebuffer = (MGLFramebuffer *)value;
    return 0;
}

PyObject * MGLContext_get_wireframe(MGLContext * self) {
    return PyBool_FromLong(self->wireframe);
}

int MGLContext_set_wireframe(MGLContext * self, PyObject * value) {
    if (value == Py_True) {
        self->gl.PolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        self->wireframe = true;
    } else if (value == Py_False) {
        self->gl.PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        self->wireframe = false;
    } else {
        MGLError_Set("invalid value for wireframe");
        return -1;
    }
    return 0;
}

PyObject * MGLContext_get_front_face(MGLContext * self) {
    if (self->front_face == GL_CW) {
        return PyUnicode_FromString("cw");
    }
    return PyUnicode_FromString("ccw");
}

int MGLContext_set_front_face(MGLContext * self, PyObject * value) {
    if (!PyUnicode_CheckExact(value)) {
        MGLError_Set("invalid front_face");
        return -1;
    }

    if (!PyUnicode_CompareWithASCIIString(value, "cw")) {
        self->front_face = GL_CW;
    } else if (!PyUnicode_CompareWithASCIIString(value, "ccw")) {
        self->front_face = GL_CCW;
    } else {
        MGLError_Set("invalid front_face");
        return -1;
    }

    self->gl.FrontFace(self->front_face);
    return 0;
}

PyObject * MGLContext_get_cull_face(MGLContext * self) {
    if (self->front_face == GL_FRONT) {
        return PyUnicode_FromString("front");
    } else if (self->front_face == GL_BACK) {
        return PyUnicode_FromString("back");
    }
    return PyUnicode_FromString("front_and_back");
}

int MGLContext_set_cull_face(MGLContext * self, PyObject * value) {
    if (!PyUnicode_CheckExact(value)) {
        MGLError_Set("invalid cull_face");
        return -1;
    }

    if (!PyUnicode_CompareWithASCIIString(value, "front")) {
        self->cull_face = GL_FRONT;
    } else if (!PyUnicode_CompareWithASCIIString(value, "back")) {
        self->cull_face = GL_BACK;
    } else if (!PyUnicode_CompareWithASCIIString(value, "front_and_back")) {
        self->cull_face = GL_FRONT_AND_BACK;
    } else {
        MGLError_Set("invalid cull_face");
        return -1;
    }

    self->gl.CullFace(self->cull_face);
    return 0;
}


PyObject * MGLContext_get_patch_vertices(MGLContext * self) {
    int patch_vertices = 0;
    self->gl.GetIntegerv(GL_PATCH_VERTICES, &patch_vertices);
    return PyLong_FromLong(patch_vertices);
}

int MGLContext_set_patch_vertices(MGLContext * self, PyObject * value) {
    int patch_vertices = PyLong_AsLong(value);
    if (PyErr_Occurred()) {
        return -1;
    }

    if (patch_vertices <= 0) {
        return -1;
    }

    self->gl.PatchParameteri(GL_PATCH_VERTICES, patch_vertices);
    return 0;
}

PyObject * MGLContext_get_error(MGLContext * self) {
    switch (self->gl.GetError()) {
        case GL_NO_ERROR:
            return PyUnicode_FromFormat("GL_NO_ERROR");
        case GL_INVALID_ENUM:
            return PyUnicode_FromFormat("GL_INVALID_ENUM");
        case GL_INVALID_VALUE:
            return PyUnicode_FromFormat("GL_INVALID_VALUE");
        case GL_INVALID_OPERATION:
            return PyUnicode_FromFormat("GL_INVALID_OPERATION");
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return PyUnicode_FromFormat("GL_INVALID_FRAMEBUFFER_OPERATION");
        case GL_OUT_OF_MEMORY:
            return PyUnicode_FromFormat("GL_OUT_OF_MEMORY");
        case GL_STACK_UNDERFLOW:
            return PyUnicode_FromFormat("GL_STACK_UNDERFLOW");
        case GL_STACK_OVERFLOW:
            return PyUnicode_FromFormat("GL_STACK_OVERFLOW");
    }
    return PyUnicode_FromFormat("GL_UNKNOWN_ERROR");
}

PyObject * MGLContext_get_version_code(MGLContext * self) {
    return PyLong_FromLong(self->version_code);
}

PyObject * MGLContext_get_extensions(MGLContext * self) {
    return self->extensions;
}

void set_key(PyObject * dict, const char * key, PyObject * value) {
    PyDict_SetItemString(dict, key, value);
    Py_DECREF(value);
}

PyObject * MGLContext_get_info(MGLContext * self) {
    const GLMethods & gl = self->gl;

    PyObject * info = PyDict_New();

    const char * vendor = (const char *)gl.GetString(GL_VENDOR);
    set_key(info, "GL_VENDOR", PyUnicode_FromString(vendor ? vendor : ""));

    const char * renderer = (const char *)gl.GetString(GL_RENDERER);
    set_key(info, "GL_RENDERER", PyUnicode_FromString(renderer ? renderer : ""));

    const char * version = (const char *)gl.GetString(GL_VERSION);
    set_key(info, "GL_VERSION", PyUnicode_FromString(version ? version : ""));

    {
        float gl_point_size_range[2] = {};
        gl.GetFloatv(GL_POINT_SIZE_RANGE, gl_point_size_range);
        set_key(info, "GL_POINT_SIZE_RANGE", Py_BuildValue("ff", gl_point_size_range[0], gl_point_size_range[1]));

        float gl_smooth_line_width_range[2] = {};
        gl.GetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, gl_smooth_line_width_range);
        set_key(info, "GL_SMOOTH_LINE_WIDTH_RANGE", Py_BuildValue("ff", gl_smooth_line_width_range[0], gl_smooth_line_width_range[1]));

        float gl_aliased_line_width_range[2] = {};
        gl.GetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, gl_aliased_line_width_range);
        set_key(info, "GL_ALIASED_LINE_WIDTH_RANGE", Py_BuildValue("ff", gl_aliased_line_width_range[0], gl_aliased_line_width_range[1]));

        float gl_point_fade_threshold_size = 0.0f;
        gl.GetFloatv(GL_POINT_FADE_THRESHOLD_SIZE, &gl_point_fade_threshold_size);

        float gl_point_size_granularity = 0.0f;
        gl.GetFloatv(GL_POINT_SIZE_GRANULARITY, &gl_point_size_granularity);

        float gl_smooth_line_width_granularity = 0.0f;
        gl.GetFloatv(GL_SMOOTH_LINE_WIDTH_GRANULARITY, &gl_smooth_line_width_granularity);

        float gl_min_program_texel_offset = 0.0f;
        gl.GetFloatv(GL_MIN_PROGRAM_TEXEL_OFFSET, &gl_min_program_texel_offset);

        float gl_max_program_texel_offset = 0.0f;
        gl.GetFloatv(GL_MAX_PROGRAM_TEXEL_OFFSET, &gl_max_program_texel_offset);

        set_key(info, "GL_POINT_FADE_THRESHOLD_SIZE", PyFloat_FromDouble(gl_point_fade_threshold_size));
        set_key(info, "GL_POINT_SIZE_GRANULARITY", PyFloat_FromDouble(gl_point_size_granularity));
        set_key(info, "GL_SMOOTH_LINE_WIDTH_GRANULARITY", PyFloat_FromDouble(gl_smooth_line_width_granularity));
        set_key(info, "GL_MIN_PROGRAM_TEXEL_OFFSET", PyFloat_FromDouble(gl_min_program_texel_offset));
        set_key(info, "GL_MAX_PROGRAM_TEXEL_OFFSET", PyFloat_FromDouble(gl_max_program_texel_offset));
    }

    {
        int gl_minor_version = 0;
        gl.GetIntegerv(GL_MINOR_VERSION, &gl_minor_version);

        int gl_major_version = 0;
        gl.GetIntegerv(GL_MAJOR_VERSION, &gl_major_version);

        int gl_sample_buffers = 0;
        gl.GetIntegerv(GL_SAMPLE_BUFFERS, &gl_sample_buffers);

        int gl_subpixel_bits = 0;
        gl.GetIntegerv(GL_SUBPIXEL_BITS, &gl_subpixel_bits);

        int gl_context_profile_mask = 0;
        gl.GetIntegerv(GL_CONTEXT_PROFILE_MASK, &gl_context_profile_mask);

        int gl_uniform_buffer_offset_alignment = 0;
        gl.GetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &gl_uniform_buffer_offset_alignment);

        set_key(info, "GL_MINOR_VERSION", PyLong_FromLong(gl_minor_version));
        set_key(info, "GL_MAJOR_VERSION", PyLong_FromLong(gl_major_version));
        set_key(info, "GL_SAMPLE_BUFFERS", PyLong_FromLong(gl_sample_buffers));
        set_key(info, "GL_SUBPIXEL_BITS", PyLong_FromLong(gl_subpixel_bits));
        set_key(info, "GL_CONTEXT_PROFILE_MASK", PyLong_FromLong(gl_context_profile_mask));
        set_key(info, "GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT", PyLong_FromLong(gl_uniform_buffer_offset_alignment));
    }

    {
        unsigned char gl_doublebuffer = 0;
        gl.GetBooleanv(GL_DOUBLEBUFFER, &gl_doublebuffer);

        unsigned char gl_stereo = 0;
        gl.GetBooleanv(GL_STEREO, &gl_stereo);

        set_key(info, "GL_DOUBLEBUFFER", PyBool_FromLong(gl_doublebuffer));
        set_key(info, "GL_STEREO", PyBool_FromLong(gl_stereo));
    }

    {
        int gl_max_viewport_dims[2] = {};
        gl.GetIntegerv(GL_MAX_VIEWPORT_DIMS, gl_max_viewport_dims);
        set_key(info, "GL_MAX_VIEWPORT_DIMS", Py_BuildValue("ii", gl_max_viewport_dims[0], gl_max_viewport_dims[1]));

        int gl_max_3d_texture_size = 0;
        gl.GetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &gl_max_3d_texture_size);

        int gl_max_array_texture_layers = 0;
        gl.GetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &gl_max_array_texture_layers);

        int gl_max_clip_distances = 0;
        gl.GetIntegerv(GL_MAX_CLIP_DISTANCES, &gl_max_clip_distances);

        int gl_max_color_attachments = 0;
        gl.GetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &gl_max_color_attachments);

        int gl_max_color_texture_samples = 0;
        gl.GetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &gl_max_color_texture_samples);

        int gl_max_combined_fragment_uniform_components = 0;
        gl.GetIntegerv(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, &gl_max_combined_fragment_uniform_components);

        int gl_max_combined_geometry_uniform_components = 0;
        gl.GetIntegerv(GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS, &gl_max_combined_geometry_uniform_components);

        int gl_max_combined_texture_image_units = 0;
        gl.GetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &gl_max_combined_texture_image_units);

        int gl_max_combined_uniform_blocks = 0;
        gl.GetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &gl_max_combined_uniform_blocks);

        int gl_max_combined_vertex_uniform_components = 0;
        gl.GetIntegerv(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, &gl_max_combined_vertex_uniform_components);

        int gl_max_cube_map_texture_size = 0;
        gl.GetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &gl_max_cube_map_texture_size);

        int gl_max_depth_texture_samples = 0;
        gl.GetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &gl_max_depth_texture_samples);

        int gl_max_draw_buffers = 0;
        gl.GetIntegerv(GL_MAX_DRAW_BUFFERS, &gl_max_draw_buffers);

        int gl_max_dual_source_draw_buffers = 0;
        gl.GetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, &gl_max_dual_source_draw_buffers);

        int gl_max_elements_indices = 0;
        gl.GetIntegerv(GL_MAX_ELEMENTS_INDICES, &gl_max_elements_indices);

        int gl_max_elements_vertices = 0;
        gl.GetIntegerv(GL_MAX_ELEMENTS_VERTICES, &gl_max_elements_vertices);

        int gl_max_fragment_input_components = 0;
        gl.GetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &gl_max_fragment_input_components);

        int gl_max_fragment_uniform_components = 0;
        gl.GetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &gl_max_fragment_uniform_components);

        int gl_max_fragment_uniform_vectors = 0;
        gl.GetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &gl_max_fragment_uniform_vectors);

        int gl_max_fragment_uniform_blocks = 0;
        gl.GetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &gl_max_fragment_uniform_blocks);

        int gl_max_geometry_input_components = 0;
        gl.GetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS, &gl_max_geometry_input_components);

        int gl_max_geometry_output_components = 0;
        gl.GetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, &gl_max_geometry_output_components);

        int gl_max_geometry_texture_image_units = 0;
        gl.GetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &gl_max_geometry_texture_image_units);

        int gl_max_geometry_uniform_blocks = 0;
        gl.GetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &gl_max_geometry_uniform_blocks);

        int gl_max_geometry_uniform_components = 0;
        gl.GetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, &gl_max_geometry_uniform_components);

        int gl_max_geometry_output_vertices = 0;
        gl.GetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &gl_max_geometry_output_vertices);

        int gl_max_integer_samples = 0;
        gl.GetIntegerv(GL_MAX_INTEGER_SAMPLES, &gl_max_integer_samples);

        int gl_max_samples = 0;
        gl.GetIntegerv(GL_MAX_SAMPLES, &gl_max_samples);

        int gl_max_rectangle_texture_size = 0;
        gl.GetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE, &gl_max_rectangle_texture_size);

        int gl_max_renderbuffer_size = 0;
        gl.GetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &gl_max_renderbuffer_size);

        int gl_max_sample_mask_words = 0;
        gl.GetIntegerv(GL_MAX_SAMPLE_MASK_WORDS, &gl_max_sample_mask_words);

        long long gl_max_server_wait_timeout = 0;

        if (gl.GetInteger64v) {
            gl.GetInteger64v(GL_MAX_SERVER_WAIT_TIMEOUT, &gl_max_server_wait_timeout);
        }

        int gl_max_texture_buffer_size = 0;
        gl.GetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &gl_max_texture_buffer_size);

        int gl_max_texture_image_units = 0;
        gl.GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &gl_max_texture_image_units);

        int gl_max_texture_lod_bias = 0;
        gl.GetIntegerv(GL_MAX_TEXTURE_LOD_BIAS, &gl_max_texture_lod_bias);

        int gl_max_texture_size = 0;
        gl.GetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_max_texture_size);

        int gl_max_uniform_buffer_bindings = 0;
        gl.GetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &gl_max_uniform_buffer_bindings);

        int gl_max_uniform_block_size = 0;
        gl.GetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &gl_max_uniform_block_size);

        int gl_max_varying_vectors = 0;
        gl.GetIntegerv(GL_MAX_VARYING_VECTORS, &gl_max_varying_vectors);

        int gl_max_vertex_attribs = 0;
        gl.GetIntegerv(GL_MAX_VERTEX_ATTRIBS, &gl_max_vertex_attribs);

        int gl_max_vertex_texture_image_units = 0;
        gl.GetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &gl_max_vertex_texture_image_units);

        int gl_max_vertex_uniform_components = 0;
        gl.GetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &gl_max_vertex_uniform_components);

        int gl_max_vertex_uniform_vectors = 0;
        gl.GetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &gl_max_vertex_uniform_vectors);

        int gl_max_vertex_output_components = 0;
        gl.GetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &gl_max_vertex_output_components);

        int gl_max_vertex_uniform_blocks = 0;
        gl.GetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &gl_max_vertex_uniform_blocks);

        int gl_max_vertex_attrib_relative_offset = 0;
        gl.GetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, &gl_max_vertex_attrib_relative_offset);

        int gl_max_vertex_attrib_bindings = 0;
        gl.GetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &gl_max_vertex_attrib_bindings);

        set_key(info, "GL_MAX_3D_TEXTURE_SIZE", PyLong_FromLong(gl_max_3d_texture_size));
        set_key(info, "GL_MAX_ARRAY_TEXTURE_LAYERS", PyLong_FromLong(gl_max_array_texture_layers));
        set_key(info, "GL_MAX_CLIP_DISTANCES", PyLong_FromLong(gl_max_clip_distances));
        set_key(info, "GL_MAX_COLOR_ATTACHMENTS", PyLong_FromLong(gl_max_color_attachments));
        set_key(info, "GL_MAX_COLOR_TEXTURE_SAMPLES", PyLong_FromLong(gl_max_color_texture_samples));
        set_key(info, "GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS", PyLong_FromLong(gl_max_combined_fragment_uniform_components));
        set_key(info, "GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS", PyLong_FromLong(gl_max_combined_geometry_uniform_components));
        set_key(info, "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", PyLong_FromLong(gl_max_combined_texture_image_units));
        set_key(info, "GL_MAX_COMBINED_UNIFORM_BLOCKS", PyLong_FromLong(gl_max_combined_uniform_blocks));
        set_key(info, "GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS", PyLong_FromLong(gl_max_combined_vertex_uniform_components));
        set_key(info, "GL_MAX_CUBE_MAP_TEXTURE_SIZE", PyLong_FromLong(gl_max_cube_map_texture_size));
        set_key(info, "GL_MAX_DEPTH_TEXTURE_SAMPLES", PyLong_FromLong(gl_max_depth_texture_samples));
        set_key(info, "GL_MAX_DRAW_BUFFERS", PyLong_FromLong(gl_max_draw_buffers));
        set_key(info, "GL_MAX_DUAL_SOURCE_DRAW_BUFFERS", PyLong_FromLong(gl_max_dual_source_draw_buffers));
        set_key(info, "GL_MAX_ELEMENTS_INDICES", PyLong_FromLong(gl_max_elements_indices));
        set_key(info, "GL_MAX_ELEMENTS_VERTICES", PyLong_FromLong(gl_max_elements_vertices));
        set_key(info, "GL_MAX_FRAGMENT_INPUT_COMPONENTS", PyLong_FromLong(gl_max_fragment_input_components));
        set_key(info, "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS", PyLong_FromLong(gl_max_fragment_uniform_components));
        set_key(info, "GL_MAX_FRAGMENT_UNIFORM_VECTORS", PyLong_FromLong(gl_max_fragment_uniform_vectors));
        set_key(info, "GL_MAX_FRAGMENT_UNIFORM_BLOCKS", PyLong_FromLong(gl_max_fragment_uniform_blocks));
        set_key(info, "GL_MAX_GEOMETRY_INPUT_COMPONENTS", PyLong_FromLong(gl_max_geometry_input_components));
        set_key(info, "GL_MAX_GEOMETRY_OUTPUT_COMPONENTS", PyLong_FromLong(gl_max_geometry_output_components));
        set_key(info, "GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS", PyLong_FromLong(gl_max_geometry_texture_image_units));
        set_key(info, "GL_MAX_GEOMETRY_UNIFORM_BLOCKS", PyLong_FromLong(gl_max_geometry_uniform_blocks));
        set_key(info, "GL_MAX_GEOMETRY_UNIFORM_COMPONENTS", PyLong_FromLong(gl_max_geometry_uniform_components));
        set_key(info, "GL_MAX_GEOMETRY_OUTPUT_VERTICES", PyLong_FromLong(gl_max_geometry_output_vertices));
        set_key(info, "GL_MAX_INTEGER_SAMPLES", PyLong_FromLong(gl_max_integer_samples));
        set_key(info, "GL_MAX_SAMPLES", PyLong_FromLong(gl_max_samples));
        set_key(info, "GL_MAX_RECTANGLE_TEXTURE_SIZE", PyLong_FromLong(gl_max_rectangle_texture_size));
        set_key(info, "GL_MAX_RENDERBUFFER_SIZE", PyLong_FromLong(gl_max_renderbuffer_size));
        set_key(info, "GL_MAX_SAMPLE_MASK_WORDS", PyLong_FromLong(gl_max_sample_mask_words));
        set_key(info, "GL_MAX_SERVER_WAIT_TIMEOUT", PyLong_FromLongLong(gl_max_server_wait_timeout));
        set_key(info, "GL_MAX_TEXTURE_BUFFER_SIZE", PyLong_FromLong(gl_max_texture_buffer_size));
        set_key(info, "GL_MAX_TEXTURE_IMAGE_UNITS", PyLong_FromLong(gl_max_texture_image_units));
        set_key(info, "GL_MAX_TEXTURE_LOD_BIAS", PyLong_FromLong(gl_max_texture_lod_bias));
        set_key(info, "GL_MAX_TEXTURE_SIZE", PyLong_FromLong(gl_max_texture_size));
        set_key(info, "GL_MAX_UNIFORM_BUFFER_BINDINGS", PyLong_FromLong(gl_max_uniform_buffer_bindings));
        set_key(info, "GL_MAX_UNIFORM_BLOCK_SIZE", PyLong_FromLong(gl_max_uniform_block_size));
        set_key(info, "GL_MAX_VARYING_VECTORS", PyLong_FromLong(gl_max_varying_vectors));
        set_key(info, "GL_MAX_VERTEX_ATTRIBS", PyLong_FromLong(gl_max_vertex_attribs));
        set_key(info, "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS", PyLong_FromLong(gl_max_vertex_texture_image_units));
        set_key(info, "GL_MAX_VERTEX_UNIFORM_COMPONENTS", PyLong_FromLong(gl_max_vertex_uniform_components));
        set_key(info, "GL_MAX_VERTEX_UNIFORM_VECTORS", PyLong_FromLong(gl_max_vertex_uniform_vectors));
        set_key(info, "GL_MAX_VERTEX_OUTPUT_COMPONENTS", PyLong_FromLong(gl_max_vertex_output_components));
        set_key(info, "GL_MAX_VERTEX_UNIFORM_BLOCKS", PyLong_FromLong(gl_max_vertex_uniform_blocks));
        set_key(info, "GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET", PyLong_FromLong(gl_max_vertex_attrib_relative_offset));
        set_key(info, "GL_MAX_VERTEX_ATTRIB_BINDINGS", PyLong_FromLong(gl_max_vertex_attrib_bindings));
    }

    if (self->version_code >= 410) {
        int gl_viewport_bounds_range[2] = {};
        gl.GetIntegerv(GL_VIEWPORT_BOUNDS_RANGE, gl_viewport_bounds_range);
        set_key(info, "GL_VIEWPORT_BOUNDS_RANGE", Py_BuildValue("ii", gl_viewport_bounds_range[0], gl_viewport_bounds_range[1]));

        int gl_viewport_subpixel_bits = 0;
        gl.GetIntegerv(GL_VIEWPORT_SUBPIXEL_BITS, &gl_viewport_subpixel_bits);

        int gl_max_viewports = 0;
        gl.GetIntegerv(GL_MAX_VIEWPORTS, &gl_max_viewports);
        set_key(info, "GL_VIEWPORT_SUBPIXEL_BITS", PyLong_FromLong(gl_viewport_subpixel_bits));
        set_key(info, "GL_MAX_VIEWPORTS", PyLong_FromLong(gl_max_viewports));
    }

    if (self->version_code >= 420) {
        int gl_min_map_buffer_alignment = 0;
        gl.GetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT, &gl_min_map_buffer_alignment);

        int gl_max_combined_atomic_counters = 0;
        gl.GetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTERS, &gl_max_combined_atomic_counters);

        int gl_max_fragment_atomic_counters = 0;
        gl.GetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTERS, &gl_max_fragment_atomic_counters);

        int gl_max_geometry_atomic_counters = 0;
        gl.GetIntegerv(GL_MAX_GEOMETRY_ATOMIC_COUNTERS, &gl_max_geometry_atomic_counters);

        int gl_max_tess_control_atomic_counters = 0;
        gl.GetIntegerv(GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS, &gl_max_tess_control_atomic_counters);

        int gl_max_tess_evaluation_atomic_counters = 0;
        gl.GetIntegerv(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS, &gl_max_tess_evaluation_atomic_counters);

        int gl_max_vertex_atomic_counters = 0;
        gl.GetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTERS, &gl_max_vertex_atomic_counters);

        set_key(info, "GL_MIN_MAP_BUFFER_ALIGNMENT", PyLong_FromLong(gl_min_map_buffer_alignment));
        set_key(info, "GL_MAX_COMBINED_ATOMIC_COUNTERS", PyLong_FromLong(gl_max_combined_atomic_counters));
        set_key(info, "GL_MAX_FRAGMENT_ATOMIC_COUNTERS", PyLong_FromLong(gl_max_fragment_atomic_counters));
        set_key(info, "GL_MAX_GEOMETRY_ATOMIC_COUNTERS", PyLong_FromLong(gl_max_geometry_atomic_counters));
        set_key(info, "GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS", PyLong_FromLong(gl_max_tess_control_atomic_counters));
        set_key(info, "GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS", PyLong_FromLong(gl_max_tess_evaluation_atomic_counters));
        set_key(info, "GL_MAX_VERTEX_ATOMIC_COUNTERS", PyLong_FromLong(gl_max_vertex_atomic_counters));
    }

    if (self->version_code >= 430) {
        int gl_max_compute_work_group_count[3] = {};
        gl.GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &gl_max_compute_work_group_count[0]);
        gl.GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &gl_max_compute_work_group_count[1]);
        gl.GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &gl_max_compute_work_group_count[2]);

        int gl_max_compute_work_group_size[3] = {};
        gl.GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &gl_max_compute_work_group_size[0]);
        gl.GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &gl_max_compute_work_group_size[1]);
        gl.GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &gl_max_compute_work_group_size[2]);

        set_key(
            info,
            "GL_MAX_COMPUTE_WORK_GROUP_COUNT",
            Py_BuildValue("III", gl_max_compute_work_group_count[0], gl_max_compute_work_group_count[1], gl_max_compute_work_group_count[2])
        );

        set_key(
            info,
            "GL_MAX_COMPUTE_WORK_GROUP_SIZE",
            Py_BuildValue("III", gl_max_compute_work_group_size[0], gl_max_compute_work_group_size[1], gl_max_compute_work_group_size[2])
        );

        int gl_max_shader_storage_buffer_bindings = 0;
        gl.GetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &gl_max_shader_storage_buffer_bindings);

        int gl_max_combined_shader_storage_blocks = 0;
        gl.GetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &gl_max_combined_shader_storage_blocks);

        int gl_max_vertex_shader_storage_blocks = 0;
        gl.GetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &gl_max_vertex_shader_storage_blocks);

        int gl_max_fragment_shader_storage_blocks = 0;
        gl.GetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &gl_max_fragment_shader_storage_blocks);

        int gl_max_geometry_shader_storage_blocks = 0;
        gl.GetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, &gl_max_geometry_shader_storage_blocks);

        int gl_max_tess_evaluation_shader_storage_blocks = 0;
        gl.GetIntegerv(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, &gl_max_tess_evaluation_shader_storage_blocks);

        int gl_max_tess_control_shader_storage_blocks = 0;
        gl.GetIntegerv(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS, &gl_max_tess_control_shader_storage_blocks);

        int gl_max_compute_shader_storage_blocks = 0;
        gl.GetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &gl_max_compute_shader_storage_blocks);

        int gl_max_compute_uniform_components = 0;
        gl.GetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, &gl_max_compute_uniform_components);

        int gl_max_compute_atomic_counters = 0;
        gl.GetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTERS, &gl_max_compute_atomic_counters);

        int gl_max_compute_atomic_counter_buffers = 0;
        gl.GetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, &gl_max_compute_atomic_counter_buffers);

        int gl_max_compute_work_group_invocations = 0;
        gl.GetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &gl_max_compute_work_group_invocations);

        int gl_max_compute_uniform_blocks = 0;
        gl.GetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, &gl_max_compute_uniform_blocks);

        int gl_max_compute_texture_image_units = 0;
        gl.GetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &gl_max_compute_texture_image_units);

        int gl_max_combined_compute_uniform_components = 0;
        gl.GetIntegerv(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, &gl_max_combined_compute_uniform_components);

        int gl_max_framebuffer_width = 0;
        gl.GetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &gl_max_framebuffer_width);

        int gl_max_framebuffer_height = 0;
        gl.GetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &gl_max_framebuffer_height);

        int gl_max_framebuffer_layers = 0;
        gl.GetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &gl_max_framebuffer_layers);

        int gl_max_framebuffer_samples = 0;
        gl.GetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &gl_max_framebuffer_samples);

        int gl_max_uniform_locations = 0;
        gl.GetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &gl_max_uniform_locations);

        long long gl_max_element_index = 0;

        if (gl.GetInteger64v) {
            gl.GetInteger64v(GL_MAX_ELEMENT_INDEX, &gl_max_element_index);
        }

        long long gl_max_shader_storage_block_size = 0;

        if (gl.GetInteger64v) {
            gl.GetInteger64v(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &gl_max_shader_storage_block_size);
        }

        set_key(info, "GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS", PyLong_FromLong(gl_max_shader_storage_buffer_bindings));
        set_key(info, "GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS", PyLong_FromLong(gl_max_combined_shader_storage_blocks));
        set_key(info, "GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS", PyLong_FromLong(gl_max_vertex_shader_storage_blocks));
        set_key(info, "GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS", PyLong_FromLong(gl_max_fragment_shader_storage_blocks));
        set_key(info, "GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS", PyLong_FromLong(gl_max_geometry_shader_storage_blocks));
        set_key(info, "GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS", PyLong_FromLong(gl_max_tess_evaluation_shader_storage_blocks));
        set_key(info, "GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS", PyLong_FromLong(gl_max_tess_control_shader_storage_blocks));
        set_key(info, "GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS", PyLong_FromLong(gl_max_compute_shader_storage_blocks));
        set_key(info, "GL_MAX_COMPUTE_UNIFORM_COMPONENTS", PyLong_FromLong(gl_max_compute_uniform_components));
        set_key(info, "GL_MAX_COMPUTE_ATOMIC_COUNTERS", PyLong_FromLong(gl_max_compute_atomic_counters));
        set_key(info, "GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS", PyLong_FromLong(gl_max_compute_atomic_counter_buffers));
        set_key(info, "GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS", PyLong_FromLong(gl_max_compute_work_group_invocations));
        set_key(info, "GL_MAX_COMPUTE_UNIFORM_BLOCKS", PyLong_FromLong(gl_max_compute_uniform_blocks));
        set_key(info, "GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS", PyLong_FromLong(gl_max_compute_texture_image_units));
        set_key(info, "GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS", PyLong_FromLong(gl_max_combined_compute_uniform_components));
        set_key(info, "GL_MAX_FRAMEBUFFER_WIDTH", PyLong_FromLong(gl_max_framebuffer_width));
        set_key(info, "GL_MAX_FRAMEBUFFER_HEIGHT", PyLong_FromLong(gl_max_framebuffer_height));
        set_key(info, "GL_MAX_FRAMEBUFFER_LAYERS", PyLong_FromLong(gl_max_framebuffer_layers));
        set_key(info, "GL_MAX_FRAMEBUFFER_SAMPLES", PyLong_FromLong(gl_max_framebuffer_samples));
        set_key(info, "GL_MAX_UNIFORM_LOCATIONS", PyLong_FromLong(gl_max_uniform_locations));
        set_key(info, "GL_MAX_ELEMENT_INDEX", PyLong_FromLongLong(gl_max_element_index));
        set_key(info, "GL_MAX_SHADER_STORAGE_BLOCK_SIZE", PyLong_FromLongLong(gl_max_shader_storage_block_size));
    }

    return info;
}

PyObject * MGLContext_framebuffer(MGLContext * self, PyObject * args) {
    PyObject * color_attachments;
    PyObject * depth_attachment;

    int args_ok = PyArg_ParseTuple(args, "OO", &color_attachments, &depth_attachment);
    if (!args_ok) {
        return 0;
    }

    // If the attachment sizes are not all identical, rendering will be limited to the
    // largest area that can fit in all of the attachments (an intersection of rectangles
    // having a lower left of (0; 0) and an upper right of (width; height) for each
    // attachment).

    int width = 0;
    int height = 0;
    int samples = 0;

    int color_attachments_len = (int)PyTuple_GET_SIZE(color_attachments);

    if (!color_attachments_len && depth_attachment == Py_None) {
        MGLError_Set("the framebuffer is empty");
        return 0;
    }

    for (int i = 0; i < color_attachments_len; ++i) {
        PyObject * item = PyTuple_GET_ITEM(color_attachments, i);

        if (Py_TYPE(item) == MGLTexture_type) {
            MGLTexture * texture = (MGLTexture *)item;

            if (texture->depth) {
                MGLError_Set("color_attachments[%d] is a depth attachment", i);
                return 0;
            }

            if (i == 0) {
                width = texture->width;
                height = texture->height;
                samples = texture->samples;
            } else {
                if (texture->width != width || texture->height != height || texture->samples != samples) {
                    MGLError_Set("the color_attachments have different sizes or samples");
                    return 0;
                }
            }

            if (texture->context != self) {
                MGLError_Set("color_attachments[%d] belongs to a different context", i);
                return 0;
            }
        } else if (Py_TYPE(item) == MGLRenderbuffer_type) {
            MGLRenderbuffer * renderbuffer = (MGLRenderbuffer *)item;

            if (renderbuffer->depth) {
                MGLError_Set("color_attachments[%d] is a depth attachment", i);
                return 0;
            }

            if (i == 0) {
                width = renderbuffer->width;
                height = renderbuffer->height;
                samples = renderbuffer->samples;
            } else {
                if (renderbuffer->width != width || renderbuffer->height != height || renderbuffer->samples != samples) {
                    MGLError_Set("the color_attachments have different sizes or samples");
                    return 0;
                }
            }

            if (renderbuffer->context != self) {
                MGLError_Set("color_attachments[%d] belongs to a different context", i);
                return 0;
            }
        } else {
            MGLError_Set("color_attachments[%d] must be a Renderbuffer or Texture not %s", i, Py_TYPE(item)->tp_name);
            return 0;
        }
    }

    const GLMethods & gl = self->gl;

    if (depth_attachment != Py_None) {

        if (Py_TYPE(depth_attachment) == MGLTexture_type) {
            MGLTexture * texture = (MGLTexture *)depth_attachment;

            if (!texture->depth) {
                MGLError_Set("the depth_attachment is a color attachment");
                return 0;
            }

            if (texture->context != self) {
                MGLError_Set("the depth_attachment belongs to a different context");
                return 0;
            }

            if (color_attachments_len) {
                if (texture->width != width || texture->height != height || texture->samples != samples) {
                    MGLError_Set("the depth_attachment have different sizes or samples");
                    return 0;
                }
            }
            else {
                width = texture->width;
                height = texture->height;
                samples = texture->samples;
            }
        } else if (Py_TYPE(depth_attachment) == MGLRenderbuffer_type) {
            MGLRenderbuffer * renderbuffer = (MGLRenderbuffer *)depth_attachment;

            if (!renderbuffer->depth) {
                MGLError_Set("the depth_attachment is a color attachment");
                return 0;
            }

            if (renderbuffer->context != self) {
                MGLError_Set("the depth_attachment belongs to a different context");
                return 0;
            }

            if (color_attachments_len) {
                if (renderbuffer->width != width || renderbuffer->height != height || renderbuffer->samples != samples) {
                    MGLError_Set("the depth_attachment have different sizes or samples");
                    return 0;
                }
            }
            else {
                width = renderbuffer->width;
                height = renderbuffer->height;
                samples = renderbuffer->samples;
            }
        } else {
            MGLError_Set("the depth_attachment must be a Renderbuffer or Texture not %s", Py_TYPE(depth_attachment)->tp_name);
            return 0;
        }
    }

    MGLFramebuffer * framebuffer = PyObject_New(MGLFramebuffer, MGLFramebuffer_type);
    framebuffer->released = false;

    framebuffer->framebuffer_obj = 0;
    gl.GenFramebuffers(1, (GLuint *)&framebuffer->framebuffer_obj);

    if (!framebuffer->framebuffer_obj) {
        MGLError_Set("cannot create framebuffer");
        Py_DECREF(framebuffer);
        return 0;
    }

    gl.BindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebuffer_obj);

    if (!color_attachments_len) {
        gl.DrawBuffer(GL_NONE); // No color buffer is drawn to.
    }

    for (int i = 0; i < color_attachments_len; ++i) {
        PyObject * item = PyTuple_GET_ITEM(color_attachments, i);

        if (Py_TYPE(item) == MGLTexture_type) {

            MGLTexture * texture = (MGLTexture *)item;

            gl.FramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0 + i,
                texture->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D,
                texture->texture_obj,
                0
            );

        } else if (Py_TYPE(item) == MGLRenderbuffer_type) {

            MGLRenderbuffer * renderbuffer = (MGLRenderbuffer *)item;

            gl.FramebufferRenderbuffer(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0 + i,
                GL_RENDERBUFFER,
                renderbuffer->renderbuffer_obj
            );
        }
    }

    if (Py_TYPE(depth_attachment) == MGLTexture_type) {
        MGLTexture * texture = (MGLTexture *)depth_attachment;

        gl.FramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            texture->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D,
            texture->texture_obj,
            0
        );

    } else if (Py_TYPE(depth_attachment) == MGLRenderbuffer_type) {
        MGLRenderbuffer * renderbuffer = (MGLRenderbuffer *)depth_attachment;

        gl.FramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            renderbuffer->renderbuffer_obj
        );
    }

    int status = gl.CheckFramebufferStatus(GL_FRAMEBUFFER);

    gl.BindFramebuffer(GL_FRAMEBUFFER, self->bound_framebuffer->framebuffer_obj);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        const char * message = "the framebuffer is not complete";

        switch (status) {
            case GL_FRAMEBUFFER_UNDEFINED:
                message = "the framebuffer is not complete (UNDEFINED)";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                message = "the framebuffer is not complete (INCOMPLETE_ATTACHMENT)";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                message = "the framebuffer is not complete (INCOMPLETE_MISSING_ATTACHMENT)";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                message = "the framebuffer is not complete (INCOMPLETE_DRAW_BUFFER)";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                message = "the framebuffer is not complete (INCOMPLETE_READ_BUFFER)";
                break;

            case GL_FRAMEBUFFER_UNSUPPORTED:
                message = "the framebuffer is not complete (UNSUPPORTED)";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                message = "the framebuffer is not complete (INCOMPLETE_MULTISAMPLE)";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                message = "the framebuffer is not complete (INCOMPLETE_LAYER_TARGETS)";
                break;
        }

        MGLError_Set(message);
        return 0;
    }

    framebuffer->draw_buffers = new unsigned[color_attachments_len];
    framebuffer->draw_buffers_len = color_attachments_len;

    for (int i = 0; i < color_attachments_len; ++i) {
        framebuffer->draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    framebuffer->color_mask = new bool[color_attachments_len * 4 + 1];

    for (int i = 0; i < color_attachments_len; ++i) {
        PyObject * item = PyTuple_GET_ITEM(color_attachments, i);
        if (Py_TYPE(item) == MGLTexture_type) {
            MGLTexture * texture = (MGLTexture *)item;
            framebuffer->color_mask[i * 4 + 0] = texture->components >= 1;
            framebuffer->color_mask[i * 4 + 1] = texture->components >= 2;
            framebuffer->color_mask[i * 4 + 2] = texture->components >= 3;
            framebuffer->color_mask[i * 4 + 3] = texture->components >= 4;
        } else if (Py_TYPE(item) == MGLRenderbuffer_type) {
            MGLTexture * renderbuffer = (MGLTexture *)item;
            framebuffer->color_mask[i * 4 + 0] = renderbuffer->components >= 1;
            framebuffer->color_mask[i * 4 + 1] = renderbuffer->components >= 2;
            framebuffer->color_mask[i * 4 + 2] = renderbuffer->components >= 3;
            framebuffer->color_mask[i * 4 + 3] = renderbuffer->components >= 4;
        }
    }

    framebuffer->depth_mask = (depth_attachment != Py_None);

    framebuffer->viewport_x = 0;
    framebuffer->viewport_y = 0;
    framebuffer->viewport_width = width;
    framebuffer->viewport_height = height;
    framebuffer->dynamic = false;

    framebuffer->scissor_enabled = false;
    framebuffer->scissor_x = 0;
    framebuffer->scissor_y = 0;
    framebuffer->scissor_width = width;
    framebuffer->scissor_height = height;

    framebuffer->width = width;
    framebuffer->height = height;
    framebuffer->samples = samples;

    Py_INCREF(self);
    framebuffer->context = self;

    Py_INCREF(framebuffer);

    PyObject * size = PyTuple_New(2);
    PyTuple_SET_ITEM(size, 0, PyLong_FromLong(framebuffer->width));
    PyTuple_SET_ITEM(size, 1, PyLong_FromLong(framebuffer->height));

    Py_INCREF(framebuffer);
    PyObject * result = PyTuple_New(4);
    PyTuple_SET_ITEM(result, 0, (PyObject *)framebuffer);
    PyTuple_SET_ITEM(result, 1, size);
    PyTuple_SET_ITEM(result, 2, PyLong_FromLong(framebuffer->samples));
    PyTuple_SET_ITEM(result, 3, PyLong_FromLong(framebuffer->framebuffer_obj));
    return result;
}

PyObject * MGLFramebuffer_release(MGLFramebuffer * self) {
    if (self->released) {
        Py_RETURN_NONE;
    }
    self->released = true;

    if (self->framebuffer_obj) {
        self->context->gl.DeleteFramebuffers(1, (GLuint *)&self->framebuffer_obj);
        Py_DECREF(self->context);
        delete[] self->draw_buffers;
        delete[] self->color_mask;
    }

    Py_DECREF(self);
    Py_RETURN_NONE;
}

void set_color_mask(MGLContext * context, bool * color_mask, int count) {
    const GLMethods & gl = context->gl;
    for (int i = 0; i < count; ++i) {
        gl.ColorMaski(i, color_mask[i * 4 + 0], color_mask[i * 4 + 1], color_mask[i * 4 + 2], color_mask[i * 4 + 3]);
    }
}

PyObject * MGLFramebuffer_clear(MGLFramebuffer * self, PyObject * args) {
    float r, g, b, a, depth;
    PyObject * viewport;

    int args_ok = PyArg_ParseTuple(args, "fffffO", &r, &g, &b, &a, &depth, &viewport);
    if (!args_ok) {
        return 0;
    }

    int x = 0;
    int y = 0;
    int width = self->width;
    int height = self->height;

    if (viewport != Py_None) {
        if (Py_TYPE(viewport) != &PyTuple_Type) {
            MGLError_Set("the viewport must be a tuple not %s", Py_TYPE(viewport)->tp_name);
            return 0;
        }

        if (PyTuple_GET_SIZE(viewport) == 4) {

            x = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 0));
            y = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 1));
            width = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 2));
            height = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 3));

        } else if (PyTuple_GET_SIZE(viewport) == 2) {

            width = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 0));
            height = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 1));

        } else {

            MGLError_Set("the viewport size %d is invalid", PyTuple_GET_SIZE(viewport));
            return 0;

        }

        if (PyErr_Occurred()) {
            MGLError_Set("wrong values in the viewport");
            return 0;
        }

    }

    const GLMethods & gl = self->context->gl;

    gl.BindFramebuffer(GL_FRAMEBUFFER, self->framebuffer_obj);

    if (self->framebuffer_obj) {
        gl.DrawBuffers(self->draw_buffers_len, self->draw_buffers);
    }

    gl.ClearColor(r, g, b, a);
    gl.ClearDepth(depth);

    set_color_mask(self->context, self->color_mask, self->draw_buffers_len);
    gl.DepthMask(self->depth_mask);

    // Respect the passed in viewport even with scissor enabled
    if (viewport != Py_None) {
        gl.Enable(GL_SCISSOR_TEST);
        gl.Scissor(x, y, width, height);
        gl.Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // restore scissor if enabled
        if (self->scissor_enabled) {
            gl.Scissor(
                self->scissor_x, self->scissor_y,
                self->scissor_width, self->scissor_height
            );
        } else {
            gl.Disable(GL_SCISSOR_TEST);
        }
    } else {
        // clear with scissor if enabled
        if (self->scissor_enabled) {
            gl.Enable(GL_SCISSOR_TEST);
            gl.Scissor(
                self->scissor_x, self->scissor_y,
                self->scissor_width, self->scissor_height
            );
        }
        gl.Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }

    gl.BindFramebuffer(GL_FRAMEBUFFER, self->context->bound_framebuffer->framebuffer_obj);

    Py_RETURN_NONE;
}

PyObject * MGLFramebuffer_use(MGLFramebuffer * self) {
    const GLMethods & gl = self->context->gl;

    gl.BindFramebuffer(GL_FRAMEBUFFER, self->framebuffer_obj);

    if (self->framebuffer_obj) {
        gl.DrawBuffers(self->draw_buffers_len, self->draw_buffers);
    }

    if (self->viewport_width && self->viewport_height) {
        gl.Viewport(self->viewport_x, self->viewport_y, self->viewport_width, self->viewport_height);
    }

    if (self->scissor_enabled) {
        gl.Enable(GL_SCISSOR_TEST);
        gl.Scissor(self->scissor_x, self->scissor_y, self->scissor_width, self->scissor_height);
    } else {
        gl.Disable(GL_SCISSOR_TEST);
    }

    set_color_mask(self->context, self->color_mask, self->draw_buffers_len);
    gl.DepthMask(self->depth_mask);

    Py_INCREF(self);
    Py_DECREF(self->context->bound_framebuffer);
    self->context->bound_framebuffer = self;
    Py_RETURN_NONE;
}

int expected_image_size(int width, int height, int alignment, int components, int pixel_size) {
    int expected_size = width * components * pixel_size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * height;
    return expected_size;
}

void read_framebuffer(MGLFramebuffer * self, int * viewport, int components, int alignment, int attachment, int clamp, MGLDataType * data_type, char * data) {
    bool read_depth = false;

    if (attachment == -1) {
        components = 1;
        read_depth = true;
    }

    int pixel_type = data_type->gl_type;
    int base_format = read_depth ? GL_DEPTH_COMPONENT : data_type->base_format[components];

    const GLMethods & gl = self->context->gl;

    if (clamp) {
        gl.ClampColor(GL_CLAMP_READ_COLOR, GL_TRUE);
    } else {
        gl.ClampColor(GL_CLAMP_READ_COLOR, GL_FIXED_ONLY);
    }

    gl.BindFramebuffer(GL_FRAMEBUFFER, self->framebuffer_obj);
    gl.ReadBuffer(read_depth ? GL_NONE : (GL_COLOR_ATTACHMENT0 + attachment));
    gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
    gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
    gl.ReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], base_format, pixel_type, data);
    gl.BindFramebuffer(GL_FRAMEBUFFER, self->context->bound_framebuffer->framebuffer_obj);
}

int parse_viewport(PyObject * arg, int * viewport) {
    PyObject * mem = PyMemoryView_FromMemory((char *)viewport, 16, PyBUF_WRITE);
    Py_XDECREF(PyObject_CallMethod(helper, "parse_viewport", "(ON)", arg, mem));
    return !PyErr_Occurred();
}

int parse_dtype(PyObject * arg, MGLDataType ** dtype) {
    if (!PyUnicode_CheckExact(arg)) {
        MGLError_Set("invalid dtype");
        return 0;
    }
    *dtype = from_dtype(PyUnicode_AsUTF8(arg), strlen(PyUnicode_AsUTF8(arg)));
    if (!*dtype) {
        MGLError_Set("invalid dtype");
        return 0;
    }
    return 1;
}

PyObject * MGLFramebuffer_read(MGLFramebuffer * self, PyObject * args) {
    int viewport[4] = {0, 0, self->width, self->height};
    int components, alignment, attachment, clamp;
    MGLDataType * dtype;

    if (!PyArg_ParseTuple(args, "O&IIIpO&", parse_viewport, viewport, &components, &attachment, &alignment, &clamp, parse_dtype, &dtype)) {
        return NULL;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return NULL;
    }

    int expected_size = expected_image_size(viewport[2], viewport[3], alignment, components, dtype->size);
    PyObject * res = PyBytes_FromStringAndSize(NULL, expected_size);
    read_framebuffer(self, viewport, components, alignment, attachment, clamp, dtype, PyBytes_AsString(res));
    return res;
}

PyObject * MGLFramebuffer_get_viewport(MGLFramebuffer * self) {
    return Py_BuildValue("(iiii)", self->viewport_x, self->viewport_y, self->viewport_width, self->viewport_height);
}

int MGLFramebuffer_set_viewport(MGLFramebuffer * self, PyObject * value) {
    PyObject * seq = PySequence_Fast(value, "invalid viewport");
    if (!seq || PySequence_Size(seq) != 4) {
        return -1;
    }

    int viewport_x = PyLong_AsLong(PySequence_Fast_GET_ITEM(value, 0));
    int viewport_y = PyLong_AsLong(PySequence_Fast_GET_ITEM(value, 1));
    int viewport_width = PyLong_AsLong(PySequence_Fast_GET_ITEM(value, 2));
    int viewport_height = PyLong_AsLong(PySequence_Fast_GET_ITEM(value, 3));
    Py_DECREF(seq);

    if (PyErr_Occurred()) {
        MGLError_Set("the viewport is invalid");
        return -1;
    }

    self->viewport_x = viewport_x;
    self->viewport_y = viewport_y;
    self->viewport_width = viewport_width;
    self->viewport_height = viewport_height;

    if (self->framebuffer_obj == self->context->bound_framebuffer->framebuffer_obj) {
        self->context->gl.Viewport(self->viewport_x, self->viewport_y, self->viewport_width, self->viewport_height);
    }

    return 0;
}

PyObject * MGLFramebuffer_get_scissor(MGLFramebuffer * self) {
    return Py_BuildValue("(iiii)", self->scissor_x, self->scissor_y, self->scissor_width, self->scissor_height);
}

int MGLFramebuffer_set_scissor(MGLFramebuffer * self, PyObject * value) {
    if (value == Py_None) {
        self->scissor_x = 0;
        self->scissor_y = 0;
        self->scissor_width = self->width;
        self->scissor_height = self->height;
        self->scissor_enabled = false;
    } else {
        PyObject * seq = PySequence_Fast(value, "invalid scissor");
        if (!seq || PySequence_Size(seq) != 4) {
            return -1;
        }

        int scissor_x = PyLong_AsLong(PySequence_Fast_GET_ITEM(value, 0));
        int scissor_y = PyLong_AsLong(PySequence_Fast_GET_ITEM(value, 1));
        int scissor_width = PyLong_AsLong(PySequence_Fast_GET_ITEM(value, 2));
        int scissor_height = PyLong_AsLong(PySequence_Fast_GET_ITEM(value, 3));
        Py_DECREF(seq);

        if (PyErr_Occurred()) {
            MGLError_Set("invalid scissor");
            return -1;
        }

        self->scissor_x = scissor_x;
        self->scissor_y = scissor_y;
        self->scissor_width = scissor_width;
        self->scissor_height = scissor_height;
        self->scissor_enabled = true;
    }

    if (self->framebuffer_obj == self->context->bound_framebuffer->framebuffer_obj) {
        const GLMethods & gl = self->context->gl;

        if (self->scissor_enabled) {
            gl.Enable(GL_SCISSOR_TEST);
        } else {
            gl.Disable(GL_SCISSOR_TEST);
        }

        gl.Scissor(self->scissor_x, self->scissor_y, self->scissor_width, self->scissor_height);
    }

    return 0;
}

PyObject * MGLFramebuffer_get_color_mask(MGLFramebuffer * self) {
    if (self->draw_buffers_len == 1) {
        return Py_BuildValue(
            "(OOOO)",
            self->color_mask[0] ? Py_True : Py_False,
            self->color_mask[1] ? Py_True : Py_False,
            self->color_mask[2] ? Py_True : Py_False,
            self->color_mask[3] ? Py_True : Py_False
        );
    }

    PyObject * res = PyTuple_New(self->draw_buffers_len);

    for (int i = 0; i < self->draw_buffers_len; ++i) {
        PyTuple_SET_ITEM(res, i, Py_BuildValue(
            "(OOOO)",
            self->color_mask[i * 4 + 0] ? Py_True : Py_False,
            self->color_mask[i * 4 + 1] ? Py_True : Py_False,
            self->color_mask[i * 4 + 2] ? Py_True : Py_False,
            self->color_mask[i * 4 + 3] ? Py_True : Py_False
        ));
    }

    return res;
}

int MGLFramebuffer_set_color_mask(MGLFramebuffer * self, PyObject * value) {
    PyObject * mem = PyMemoryView_FromMemory((char *)self->color_mask, self->draw_buffers_len * 4, PyBUF_WRITE);
    PyObject * res = PyObject_CallMethod(helper, "set_color_mask", "ON", value, mem);
    if (!res) {
        return -1;
    }

    if (self->framebuffer_obj == self->context->bound_framebuffer->framebuffer_obj) {
        set_color_mask(self->context, self->color_mask, self->draw_buffers_len);
    }

    return 0;
}

PyObject * MGLFramebuffer_get_depth_mask(MGLFramebuffer * self) {
    return PyBool_FromLong(self->depth_mask);
}

int MGLFramebuffer_set_depth_mask(MGLFramebuffer * self, PyObject * value) {
    self->depth_mask = PyObject_IsTrue(value);
    if (self->framebuffer_obj == self->context->bound_framebuffer->framebuffer_obj) {
        const GLMethods & gl = self->context->gl;
        gl.DepthMask(self->depth_mask);
    }
    return 0;
}

PyObject * MGLFramebuffer_get_bits(MGLFramebuffer * self) {
    if (self->framebuffer_obj) {
        MGLError_Set("only the default_framebuffer have bits");
        return 0;
    }

    int red_bits = 0;
    int green_bits = 0;
    int blue_bits = 0;
    int alpha_bits = 0;
    int depth_bits = 0;
    int stencil_bits = 0;

    const GLMethods & gl = self->context->gl;

    gl.BindFramebuffer(GL_FRAMEBUFFER, self->framebuffer_obj);
    gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE, &red_bits);
    gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE, &green_bits);
    gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE, &blue_bits);
    gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE, &alpha_bits);
    gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH, GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depth_bits);
    gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencil_bits);
    gl.BindFramebuffer(GL_FRAMEBUFFER, self->context->bound_framebuffer->framebuffer_obj);

    return Py_BuildValue(
        "{sisisisisisi}"
        "red", red_bits,
        "green", green_bits,
        "blue", blue_bits,
        "alpha", alpha_bits,
        "depth", depth_bits,
        "stencil", stencil_bits
    );
}

PyObject * MGLContext_program(MGLContext * self, PyObject * args) {
    PyObject * shaders[5];
    PyObject * outputs;
    PyObject * fragment_outputs;
    int interleaved;

    int args_ok = PyArg_ParseTuple(
        args,
        "OOOOOOOp",
        &shaders[0],
        &shaders[1],
        &shaders[2],
        &shaders[3],
        &shaders[4],
        &outputs,
        &fragment_outputs,
        &interleaved
    );

    if (!args_ok) {
        return 0;
    }

    int num_outputs = (int)PyTuple_GET_SIZE(outputs);

    for (int i = 0; i < num_outputs; ++i) {
        PyObject * item = PyTuple_GET_ITEM(outputs, i);
        if (Py_TYPE(item) != &PyUnicode_Type) {
            MGLError_Set("varyings[%d] must be a string not %s", i, Py_TYPE(item)->tp_name);
            return 0;
        }
    }

    MGLProgram * program = PyObject_New(MGLProgram, MGLProgram_type);
    program->released = false;

    Py_INCREF(self);
    program->context = self;

    const GLMethods & gl = program->context->gl;

    int program_obj = gl.CreateProgram();

    if (!program_obj) {
        MGLError_Set("cannot create program");
        return 0;
    }

    int shader_objs[] = {0, 0, 0, 0, 0};

    for (int i = 0; i < NUM_SHADER_SLOTS; ++i) {
        if (shaders[i] == Py_None) {
            continue;
        }

        const char * source_str = PyUnicode_AsUTF8(shaders[i]);

        int shader_obj = gl.CreateShader(SHADER_TYPE[i]);

        if (!shader_obj) {
            MGLError_Set("cannot create shader");
            return 0;
        }

        gl.ShaderSource(shader_obj, 1, &source_str, 0);
        gl.CompileShader(shader_obj);

        int compiled = GL_FALSE;
        gl.GetShaderiv(shader_obj, GL_COMPILE_STATUS, &compiled);

        if (!compiled) {
            const char * SHADER_NAME[] = {
                "vertex_shader",
                "fragment_shader",
                "geometry_shader",
                "tess_control_shader",
                "tess_evaluation_shader",
            };

            const char * SHADER_NAME_UNDERLINE[] = {
                "=============",
                "===============",
                "===============",
                "===================",
                "======================",
            };

            const char * message = "GLSL Compiler failed";
            const char * title = SHADER_NAME[i];
            const char * underline = SHADER_NAME_UNDERLINE[i];

            int log_len = 0;
            gl.GetShaderiv(shader_obj, GL_INFO_LOG_LENGTH, &log_len);

            char * log = new char[log_len];
            gl.GetShaderInfoLog(shader_obj, log_len, &log_len, log);

            gl.DeleteShader(shader_obj);

            MGLError_Set("%s\n\n%s\n%s\n%s\n", message, title, underline, log);

            delete[] log;
            return 0;
        }

        shader_objs[i] = shader_obj;
        gl.AttachShader(program_obj, shader_obj);
    }

    if (num_outputs) {
        const char ** varyings_array = new const char * [num_outputs];

        for (int i = 0; i < num_outputs; ++i) {
            varyings_array[i] = PyUnicode_AsUTF8(PyTuple_GET_ITEM(outputs, i));
        }

        int capture_mode = interleaved ? GL_INTERLEAVED_ATTRIBS : GL_SEPARATE_ATTRIBS;

        gl.TransformFeedbackVaryings(program_obj, num_outputs, varyings_array, capture_mode);

        delete[] varyings_array;
    }

    {
        PyObject * key = NULL;
        PyObject * value = NULL;
        Py_ssize_t pos = 0;

        while (PyDict_Next(fragment_outputs, &pos, &key, &value)) {
            gl.BindFragDataLocation(program_obj, PyLong_AsLong(value), PyUnicode_AsUTF8(key));
        }
    }

    gl.LinkProgram(program_obj);

    // Delete the shader objects after the program is linked
    for (int i = 0; i < NUM_SHADER_SLOTS; ++i) {
        if (shader_objs[i]) {
            gl.DeleteShader(shader_objs[i]);
        }
    }

    int linked = GL_FALSE;
    gl.GetProgramiv(program_obj, GL_LINK_STATUS, &linked);

    if (!linked) {
        const char * message = "GLSL Linker failed";
        const char * title = "Program";
        const char * underline = "=======";

        int log_len = 0;
        gl.GetProgramiv(program_obj, GL_INFO_LOG_LENGTH, &log_len);

        char * log = new char[log_len];
        gl.GetProgramInfoLog(program_obj, log_len, &log_len, log);

        gl.DeleteProgram(program_obj);

        MGLError_Set("%s\n\n%s\n%s\n%s\n", message, title, underline, log);

        delete[] log;
        return 0;
    }

    program->program_obj = program_obj;

    // int num_vertex_shader_subroutine_locations = 0;
    // int num_fragment_shader_subroutine_locations = 0;
    // int num_geometry_shader_subroutine_locations = 0;
    // int num_tess_evaluation_shader_subroutine_locations = 0;
    // int num_tess_control_shader_subroutine_locations = 0;

    int num_vertex_shader_subroutines = 0;
    int num_fragment_shader_subroutines = 0;
    int num_geometry_shader_subroutines = 0;
    int num_tess_evaluation_shader_subroutines = 0;
    int num_tess_control_shader_subroutines = 0;

    int num_vertex_shader_subroutine_uniforms = 0;
    int num_fragment_shader_subroutine_uniforms = 0;
    int num_geometry_shader_subroutine_uniforms = 0;
    int num_tess_evaluation_shader_subroutine_uniforms = 0;
    int num_tess_control_shader_subroutine_uniforms = 0;

    if (program->context->version_code >= 400) {
        if (shaders[VERTEX_SHADER_SLOT] != Py_None) {
            // gl.GetProgramStageiv(
            // 	program_obj,
            // 	GL_VERTEX_SHADER,
            // 	GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
            // 	&num_vertex_shader_subroutine_locations
            // );
            gl.GetProgramStageiv(
                program_obj,
                GL_VERTEX_SHADER,
                GL_ACTIVE_SUBROUTINES,
                &num_vertex_shader_subroutines
            );
            gl.GetProgramStageiv(
                program_obj,
                GL_VERTEX_SHADER,
                GL_ACTIVE_SUBROUTINE_UNIFORMS,
                &num_vertex_shader_subroutine_uniforms
            );
        }

        if (shaders[FRAGMENT_SHADER_SLOT] != Py_None) {
            // gl.GetProgramStageiv(
            // 	program_obj,
            // 	GL_FRAGMENT_SHADER,
            // 	GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
            // 	&num_fragment_shader_subroutine_locations
            // );
            gl.GetProgramStageiv(
                program_obj,
                GL_FRAGMENT_SHADER,
                GL_ACTIVE_SUBROUTINES,
                &num_fragment_shader_subroutines
            );
            gl.GetProgramStageiv(
                program_obj,
                GL_FRAGMENT_SHADER,
                GL_ACTIVE_SUBROUTINE_UNIFORMS,
                &num_fragment_shader_subroutine_uniforms
            );
        }

        if (shaders[GEOMETRY_SHADER_SLOT] != Py_None) {
            // gl.GetProgramStageiv(
            // 	program_obj,
            // 	GL_GEOMETRY_SHADER,
            // 	GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
            // 	&num_geometry_shader_subroutine_locations
            // );
            gl.GetProgramStageiv(
                program_obj,
                GL_GEOMETRY_SHADER,
                GL_ACTIVE_SUBROUTINES,
                &num_geometry_shader_subroutines
            );
            gl.GetProgramStageiv(
                program_obj,
                GL_GEOMETRY_SHADER,
                GL_ACTIVE_SUBROUTINE_UNIFORMS,
                &num_geometry_shader_subroutine_uniforms
            );
        }

        if (shaders[TESS_EVALUATION_SHADER_SLOT] != Py_None) {
            // gl.GetProgramStageiv(
            // 	program_obj,
            // 	GL_TESS_EVALUATION_SHADER,
            // 	GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
            // 	&num_tess_evaluation_shader_subroutine_locations
            // );
            gl.GetProgramStageiv(
                program_obj,
                GL_TESS_EVALUATION_SHADER,
                GL_ACTIVE_SUBROUTINES,
                &num_tess_evaluation_shader_subroutines
            );
            gl.GetProgramStageiv(
                program_obj,
                GL_TESS_EVALUATION_SHADER,
                GL_ACTIVE_SUBROUTINE_UNIFORMS,
                &num_tess_evaluation_shader_subroutine_uniforms
            );
        }

        if (shaders[TESS_CONTROL_SHADER_SLOT] != Py_None) {
            // gl.GetProgramStageiv(
            // 	program_obj,
            // 	GL_TESS_CONTROL_SHADER,
            // 	GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
            // 	&num_tess_control_shader_subroutine_locations
            // );
            gl.GetProgramStageiv(
                program_obj,
                GL_TESS_CONTROL_SHADER,
                GL_ACTIVE_SUBROUTINES,
                &num_tess_control_shader_subroutines
            );
            gl.GetProgramStageiv(
                program_obj,
                GL_TESS_CONTROL_SHADER,
                GL_ACTIVE_SUBROUTINE_UNIFORMS,
                &num_tess_control_shader_subroutine_uniforms
            );
        }
    }

    if (shaders[GEOMETRY_SHADER_SLOT] != Py_None) {

        int geometry_in = 0;
        int geometry_out = 0;
        program->geometry_vertices = 0;

        gl.GetProgramiv(program_obj, GL_GEOMETRY_INPUT_TYPE, &geometry_in);
        gl.GetProgramiv(program_obj, GL_GEOMETRY_OUTPUT_TYPE, &geometry_out);
        gl.GetProgramiv(program_obj, GL_GEOMETRY_VERTICES_OUT, &program->geometry_vertices);

        switch (geometry_in) {
            case GL_TRIANGLES:
                program->geometry_input = GL_TRIANGLES;
                break;

            case GL_TRIANGLE_STRIP:
                program->geometry_input = GL_TRIANGLE_STRIP;
                break;

            case GL_TRIANGLE_FAN:
                program->geometry_input = GL_TRIANGLE_FAN;
                break;

            case GL_LINES:
                program->geometry_input = GL_LINES;
                break;

            case GL_LINE_STRIP:
                program->geometry_input = GL_LINE_STRIP;
                break;

            case GL_LINE_LOOP:
                program->geometry_input = GL_LINE_LOOP;
                break;

            case GL_POINTS:
                program->geometry_input = GL_POINTS;
                break;

            case GL_LINE_STRIP_ADJACENCY:
                program->geometry_input = GL_LINE_STRIP_ADJACENCY;
                break;

            case GL_LINES_ADJACENCY:
                program->geometry_input = GL_LINES_ADJACENCY;
                break;

            case GL_TRIANGLE_STRIP_ADJACENCY:
                program->geometry_input = GL_TRIANGLE_STRIP_ADJACENCY;
                break;

            case GL_TRIANGLES_ADJACENCY:
                program->geometry_input = GL_TRIANGLES_ADJACENCY;
                break;

            default:
                program->geometry_input = -1;
                break;
        }

        switch (geometry_out) {
            case GL_TRIANGLES:
                program->geometry_output = GL_TRIANGLES;
                break;

            case GL_TRIANGLE_STRIP:
                program->geometry_output = GL_TRIANGLES;
                break;

            case GL_TRIANGLE_FAN:
                program->geometry_output = GL_TRIANGLES;
                break;

            case GL_LINES:
                program->geometry_output = GL_LINES;
                break;

            case GL_LINE_STRIP:
                program->geometry_output = GL_LINES;
                break;

            case GL_LINE_LOOP:
                program->geometry_output = GL_LINES;
                break;

            case GL_POINTS:
                program->geometry_output = GL_POINTS;
                break;

            case GL_LINE_STRIP_ADJACENCY:
                program->geometry_output = GL_LINES;
                break;

            case GL_LINES_ADJACENCY:
                program->geometry_output = GL_LINES;
                break;

            case GL_TRIANGLE_STRIP_ADJACENCY:
                program->geometry_output = GL_TRIANGLES;
                break;

            case GL_TRIANGLES_ADJACENCY:
                program->geometry_output = GL_TRIANGLES;
                break;

            default:
                program->geometry_output = -1;
                break;
        }

    } else {
        program->geometry_input = -1;
        program->geometry_output = -1;
        program->geometry_vertices = 0;
    }

    if (PyErr_Occurred()) {
        Py_DECREF(program);
        return 0;
    }

    Py_INCREF(program);

    int num_attributes = 0;
    int num_varyings = 0;
    int num_uniforms = 0;
    int num_uniform_blocks = 0;

    gl.GetProgramiv(program->program_obj, GL_ACTIVE_ATTRIBUTES, &num_attributes);
    gl.GetProgramiv(program->program_obj, GL_TRANSFORM_FEEDBACK_VARYINGS, &num_varyings);
    gl.GetProgramiv(program->program_obj, GL_ACTIVE_UNIFORMS, &num_uniforms);
    gl.GetProgramiv(program->program_obj, GL_ACTIVE_UNIFORM_BLOCKS, &num_uniform_blocks);

    int num_subroutines = num_vertex_shader_subroutines + num_fragment_shader_subroutines + num_geometry_shader_subroutines + num_tess_evaluation_shader_subroutines + num_tess_control_shader_subroutines;
    int num_subroutine_uniforms = num_vertex_shader_subroutine_uniforms + num_fragment_shader_subroutine_uniforms + num_geometry_shader_subroutine_uniforms + num_tess_evaluation_shader_subroutine_uniforms + num_tess_control_shader_subroutine_uniforms;

    program->num_vertex_shader_subroutines = num_vertex_shader_subroutine_uniforms;
    program->num_fragment_shader_subroutines = num_fragment_shader_subroutine_uniforms;
    program->num_geometry_shader_subroutines = num_geometry_shader_subroutine_uniforms;
    program->num_tess_evaluation_shader_subroutines = num_tess_evaluation_shader_subroutine_uniforms;
    program->num_tess_control_shader_subroutines = num_tess_control_shader_subroutine_uniforms;

    program->num_varyings = num_varyings;

    PyObject * subroutine_uniforms_lst = PyTuple_New(num_subroutine_uniforms);
    PyObject * members_dict = PyDict_New();

    for (int i = 0; i < num_attributes; ++i) {
        int type = 0;
        int array_length = 0;
        int name_len = 0;
        char name[256];

        gl.GetActiveAttrib(program->program_obj, i, 256, &name_len, &array_length, (GLenum *)&type, name);
        int location = gl.GetAttribLocation(program->program_obj, name);

        PyObject * clean_name = PyObject_CallMethod(helper, "clean_glsl_name", "s", name);
        PyObject * item = PyObject_CallMethod(
            helper, "make_attribute", "(Niiii)",
            clean_name, type, program->program_obj, location, array_length
        );

        PyDict_SetItemString(members_dict, name, item);
        Py_DECREF(item);
    }

    for (int i = 0; i < num_varyings; ++i) {
        int type = 0;
        int array_length = 0;
        int dimension = 0;
        int name_len = 0;
        char name[256];

        gl.GetTransformFeedbackVarying(program->program_obj, i, 256, &name_len, &array_length, (GLenum *)&type, name);

        PyObject * item = PyObject_CallMethod(helper, "make_varying", "(siii)", name, i, array_length, dimension);
        PyDict_SetItemString(members_dict, name, item);
        Py_DECREF(item);
    }

    for (int i = 0; i < num_uniforms; ++i) {
        int type = 0;
        int array_length = 0;
        int name_len = 0;
        char name[256];

        gl.GetActiveUniform(program->program_obj, i, 256, &name_len, &array_length, (GLenum *)&type, name);
        int location = gl.GetUniformLocation(program->program_obj, name);

        if (location < 0) {
            continue;
        }

        PyObject * clean_name = PyObject_CallMethod(helper, "clean_glsl_name", "s", name);
        PyObject * item = PyObject_CallMethod(
            helper, "make_uniform", "(NiiiiO)",
            clean_name, type, program->program_obj, location, array_length, self
        );

        PyDict_SetItemString(members_dict, name, item);
        Py_DECREF(item);
    }

    for (int i = 0; i < num_uniform_blocks; ++i) {
        int size = 0;
        int name_len = 0;
        char name[256];

        gl.GetActiveUniformBlockName(program->program_obj, i, 256, &name_len, name);
        int index = gl.GetUniformBlockIndex(program->program_obj, name);
        gl.GetActiveUniformBlockiv(program->program_obj, index, GL_UNIFORM_BLOCK_DATA_SIZE, &size);

        PyObject * clean_name = PyObject_CallMethod(helper, "clean_glsl_name", "s", name);
        PyObject * item = PyObject_CallMethod(
            helper, "make_uniform_block", "(NiiiO)",
            clean_name, program->program_obj, index, size, self
        );

        PyDict_SetItemString(members_dict, name, item);
        Py_DECREF(item);
    }

    int subroutine_uniforms_base = 0;
    int subroutines_base = 0;

    if (program->context->version_code >= 400) {
        const int shader_type[5] = {
            GL_VERTEX_SHADER,
            GL_FRAGMENT_SHADER,
            GL_GEOMETRY_SHADER,
            GL_TESS_EVALUATION_SHADER,
            GL_TESS_CONTROL_SHADER,
        };

        for (int st = 0; st < 5; ++st) {
            int num_subroutines = 0;
            gl.GetProgramStageiv(program_obj, shader_type[st], GL_ACTIVE_SUBROUTINES, &num_subroutines);

            int num_subroutine_uniforms = 0;
            gl.GetProgramStageiv(program_obj, shader_type[st], GL_ACTIVE_SUBROUTINE_UNIFORMS, &num_subroutine_uniforms);

            for (int i = 0; i < num_subroutines; ++i) {
                int name_len = 0;
                char name[256];

                gl.GetActiveSubroutineName(program_obj, shader_type[st], i, 256, &name_len, name);
                int index = gl.GetSubroutineIndex(program_obj, shader_type[st], name);

                PyObject * item = PyObject_CallMethod(helper, "make_subroutine", "(si)", name, index);
                PyDict_SetItemString(members_dict, name, item);
                Py_DECREF(item);
            }

            for (int i = 0; i < num_subroutine_uniforms; ++i) {
                int name_len = 0;
                char name[256];

                gl.GetActiveSubroutineUniformName(program_obj, shader_type[st], i, 256, &name_len, name);
                int location = subroutine_uniforms_base + gl.GetSubroutineUniformLocation(program_obj, shader_type[st], name);
                PyTuple_SET_ITEM(subroutine_uniforms_lst, location, PyUnicode_FromStringAndSize(name, name_len));
            }

            subroutine_uniforms_base += num_subroutine_uniforms;
            subroutines_base += num_subroutines;
        }
    }

    PyObject * geom_info = PyTuple_New(3);
    if (program->geometry_input != -1) {
        PyTuple_SET_ITEM(geom_info, 0, PyLong_FromLong(program->geometry_input));
    } else {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(geom_info, 0, Py_None);
    }
    if (program->geometry_output != -1) {
        PyTuple_SET_ITEM(geom_info, 1, PyLong_FromLong(program->geometry_output));
    } else {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(geom_info, 1, Py_None);
    }
    PyTuple_SET_ITEM(geom_info, 2, PyLong_FromLong(program->geometry_vertices));

    PyObject * result = PyTuple_New(5);
    PyTuple_SET_ITEM(result, 0, (PyObject *)program);
    PyTuple_SET_ITEM(result, 1, members_dict);
    PyTuple_SET_ITEM(result, 2, subroutine_uniforms_lst);
    PyTuple_SET_ITEM(result, 3, geom_info);
    PyTuple_SET_ITEM(result, 4, PyLong_FromLong(program->program_obj));
    return result;
}

PyObject * MGLProgram_release(MGLProgram * self) {
    if (self->released) {
        Py_RETURN_NONE;
    }
    self->released = true;

    const GLMethods & gl = self->context->gl;
    gl.DeleteProgram(self->program_obj);

    Py_DECREF(self);
    Py_RETURN_NONE;
}

PyObject * MGLContext_query(MGLContext * self, PyObject * args) {
    int samples_passed;
    int any_samples_passed;
    int time_elapsed;
    int primitives_generated;

    int args_ok = PyArg_ParseTuple(args, "pppp", &samples_passed, &any_samples_passed, &time_elapsed, &primitives_generated);
    if (!args_ok) {
        return 0;
    }

    // If none of them is set, all will be set.
    if (!(samples_passed + any_samples_passed + time_elapsed + primitives_generated)) {
        samples_passed = 1;
        any_samples_passed = 1;
        time_elapsed = 1;
        primitives_generated = 1;
    }

    MGLQuery * query = PyObject_New(MGLQuery, MGLQuery_type);
    query->released = false;

    Py_INCREF(self);
    query->context = self;

    const GLMethods & gl = self->gl;

    if (samples_passed) {
        gl.GenQueries(1, (GLuint *)&query->query_obj[SAMPLES_PASSED]);
    }
    if (any_samples_passed) {
        gl.GenQueries(1, (GLuint *)&query->query_obj[ANY_SAMPLES_PASSED]);
    }
    if (time_elapsed) {
        gl.GenQueries(1, (GLuint *)&query->query_obj[TIME_ELAPSED]);
    }
    if (primitives_generated) {
        gl.GenQueries(1, (GLuint *)&query->query_obj[PRIMITIVES_GENERATED]);
    }

    // PyObject * result = PyTuple_New(2);
    // PyTuple_SET_ITEM(result, 0, (PyObject *)query);
    // PyTuple_SET_ITEM(result, 1, PyLong_FromLong(query->query_obj));
    // return result;

    return (PyObject *)query;
}

PyObject * MGLQuery_begin(MGLQuery * self) {
    const GLMethods & gl = self->context->gl;

    if (self->query_obj[SAMPLES_PASSED]) {
        gl.BeginQuery(GL_SAMPLES_PASSED, self->query_obj[SAMPLES_PASSED]);
    }

    if (self->query_obj[ANY_SAMPLES_PASSED]) {
        gl.BeginQuery(GL_ANY_SAMPLES_PASSED, self->query_obj[ANY_SAMPLES_PASSED]);
    }

    if (self->query_obj[TIME_ELAPSED]) {
        gl.BeginQuery(GL_TIME_ELAPSED, self->query_obj[TIME_ELAPSED]);
    }

    if (self->query_obj[PRIMITIVES_GENERATED]) {
        gl.BeginQuery(GL_PRIMITIVES_GENERATED, self->query_obj[PRIMITIVES_GENERATED]);
    }

    Py_RETURN_NONE;
}

PyObject * MGLQuery_end(MGLQuery * self) {
    const GLMethods & gl = self->context->gl;

    if (self->query_obj[SAMPLES_PASSED]) {
        gl.EndQuery(GL_SAMPLES_PASSED);
    }

    if (self->query_obj[ANY_SAMPLES_PASSED]) {
        gl.EndQuery(GL_ANY_SAMPLES_PASSED);
    }

    if (self->query_obj[TIME_ELAPSED]) {
        gl.EndQuery(GL_TIME_ELAPSED);
    }

    if (self->query_obj[PRIMITIVES_GENERATED]) {
        gl.EndQuery(GL_PRIMITIVES_GENERATED);
    }

    Py_RETURN_NONE;
}

PyObject * MGLQuery_begin_render(MGLQuery * self) {
    const GLMethods & gl = self->context->gl;

    if (self->query_obj[ANY_SAMPLES_PASSED]) {
        gl.BeginConditionalRender(self->query_obj[ANY_SAMPLES_PASSED], GL_QUERY_NO_WAIT);
    } else if (self->query_obj[SAMPLES_PASSED]) {
        gl.BeginConditionalRender(self->query_obj[SAMPLES_PASSED], GL_QUERY_NO_WAIT);
    } else {
        MGLError_Set("no samples");
        return 0;
    }

    Py_RETURN_NONE;
}

PyObject * MGLQuery_end_render(MGLQuery * self) {
    const GLMethods & gl = self->context->gl;
    gl.EndConditionalRender();
    Py_RETURN_NONE;
}

PyObject * MGLQuery_get_samples(MGLQuery * self) {
    const GLMethods & gl = self->context->gl;

    int samples = 0;
    gl.GetQueryObjectiv(self->query_obj[SAMPLES_PASSED], GL_QUERY_RESULT, &samples);

    return PyLong_FromLong(samples);
}

PyObject * MGLQuery_get_primitives(MGLQuery * self) {
    const GLMethods & gl = self->context->gl;

    int primitives = 0;
    gl.GetQueryObjectiv(self->query_obj[PRIMITIVES_GENERATED], GL_QUERY_RESULT, &primitives);

    return PyLong_FromLong(primitives);
}

PyObject * MGLQuery_get_elapsed(MGLQuery * self) {
    const GLMethods & gl = self->context->gl;

    int elapsed = 0;
    gl.GetQueryObjectiv(self->query_obj[TIME_ELAPSED], GL_QUERY_RESULT, &elapsed);

    return PyLong_FromLong(elapsed);
}

PyObject * MGLContext_renderbuffer(MGLContext * self, PyObject * args) {
    int width;
    int height;

    int components;

    int samples;

    const char * dtype;
    Py_ssize_t dtype_size;

    int args_ok = PyArg_ParseTuple(args, "(II)IIs#", &width, &height, &components, &samples, &dtype, &dtype_size);
    if (!args_ok) {
        return 0;
    }

    if (components < 1 || components > 4) {
        MGLError_Set("the components must be 1, 2, 3 or 4");
        return 0;
    }

    if ((samples & (samples - 1)) || samples > self->max_samples) {
        MGLError_Set("the number of samples is invalid");
        return 0;
    }

    MGLDataType * data_type = from_dtype(dtype, dtype_size);

    if (!data_type) {
        MGLError_Set("invalid dtype");
        return 0;
    }

    int format = data_type->internal_format[components];

    const GLMethods & gl = self->gl;

    MGLRenderbuffer * renderbuffer = PyObject_New(MGLRenderbuffer, MGLRenderbuffer_type);
    renderbuffer->released = false;

    renderbuffer->renderbuffer_obj = 0;
    gl.GenRenderbuffers(1, (GLuint *)&renderbuffer->renderbuffer_obj);

    if (!renderbuffer->renderbuffer_obj) {
        MGLError_Set("cannot create renderbuffer");
        Py_DECREF(renderbuffer);
        return 0;
    }

    gl.BindRenderbuffer(GL_RENDERBUFFER, renderbuffer->renderbuffer_obj);

    if (samples == 0) {
        gl.RenderbufferStorage(GL_RENDERBUFFER, format, width, height);
    } else {
        gl.RenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
    }

    renderbuffer->width = width;
    renderbuffer->height = height;
    renderbuffer->components = components;
    renderbuffer->samples = samples;
    renderbuffer->data_type = data_type;
    renderbuffer->depth = false;

    Py_INCREF(self);
    renderbuffer->context = self;

    Py_INCREF(renderbuffer);

    PyObject * result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, (PyObject *)renderbuffer);
    PyTuple_SET_ITEM(result, 1, PyLong_FromLong(renderbuffer->renderbuffer_obj));
    return result;
}

PyObject * MGLContext_depth_renderbuffer(MGLContext * self, PyObject * args) {
    int width;
    int height;

    int samples;

    int args_ok = PyArg_ParseTuple(args, "(II)I", &width, &height, &samples);
    if (!args_ok) {
        return 0;
    }

    if ((samples & (samples - 1)) || samples > self->max_samples) {
        MGLError_Set("the number of samples is invalid");
        return 0;
    }

    const GLMethods & gl = self->gl;

    MGLRenderbuffer * renderbuffer = PyObject_New(MGLRenderbuffer, MGLRenderbuffer_type);
    renderbuffer->released = false;

    renderbuffer->renderbuffer_obj = 0;
    gl.GenRenderbuffers(1, (GLuint *)&renderbuffer->renderbuffer_obj);

    if (!renderbuffer->renderbuffer_obj) {
        MGLError_Set("cannot create renderbuffer");
        Py_DECREF(renderbuffer);
        return 0;
    }

    gl.BindRenderbuffer(GL_RENDERBUFFER, renderbuffer->renderbuffer_obj);

    if (samples == 0) {
        gl.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    } else {
        gl.RenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT24, width, height);
    }

    renderbuffer->width = width;
    renderbuffer->height = height;
    renderbuffer->components = 1;
    renderbuffer->samples = samples;
    renderbuffer->data_type = from_dtype("f4", 2);
    renderbuffer->depth = true;

    Py_INCREF(self);
    renderbuffer->context = self;

    Py_INCREF(renderbuffer);

    PyObject * result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, (PyObject *)renderbuffer);
    PyTuple_SET_ITEM(result, 1, PyLong_FromLong(renderbuffer->renderbuffer_obj));
    return result;
}

PyObject * MGLRenderbuffer_release(MGLRenderbuffer * self) {
    if (self->released) {
        Py_RETURN_NONE;
    }
    self->released = true;

    const GLMethods & gl = self->context->gl;
    gl.DeleteRenderbuffers(1, (GLuint *)&self->renderbuffer_obj);

    Py_DECREF(self);
    Py_RETURN_NONE;
}

PyObject * MGLContext_sampler(MGLContext * self) {
    const GLMethods & gl = self->gl;

    MGLSampler * sampler = PyObject_New(MGLSampler, MGLSampler_type);
    sampler->released = false;

    gl.GenSamplers(1, (GLuint *)&sampler->sampler_obj);

    sampler->min_filter = GL_LINEAR;
    sampler->mag_filter = GL_LINEAR;
    sampler->anisotropy = 1.0;
    sampler->repeat_x = true;
    sampler->repeat_y = true;
    sampler->repeat_z = true;
    sampler->compare_func = 0;
    sampler->border_color[0] = 0.0;
    sampler->border_color[1] = 0.0;
    sampler->border_color[2] = 0.0;
    sampler->border_color[3] = 0.0;
    sampler->min_lod = -1000.0;
    sampler->max_lod = 1000.0;

    Py_INCREF(self);
    sampler->context = self;

    Py_INCREF(sampler);

    PyObject * result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, (PyObject *)sampler);
    PyTuple_SET_ITEM(result, 1, PyLong_FromLong(sampler->sampler_obj));
    return result;
}

PyObject * MGLSampler_use(MGLSampler * self, PyObject * args) {
    int index;

    int args_ok = PyArg_ParseTuple(args, "I", &index);
    if (!args_ok) {
        return 0;
    }

    const GLMethods & gl = self->context->gl;
    gl.BindSampler(index, self->sampler_obj);

    Py_RETURN_NONE;
}

PyObject * MGLSampler_clear(MGLSampler * self, PyObject * args) {
    int index;

    int args_ok = PyArg_ParseTuple(args, "I", &index);
    if (!args_ok) {
        return 0;
    }

    const GLMethods & gl = self->context->gl;
    gl.BindSampler(index, 0);

    Py_RETURN_NONE;
}

PyObject * MGLSampler_release(MGLSampler * self) {
    if (self->released) {
        Py_RETURN_NONE;
    }
    self->released = true;

    const GLMethods & gl = self->context->gl;
    gl.DeleteSamplers(1, (GLuint *)&self->sampler_obj);

    Py_DECREF(self);
    Py_DECREF(self->context);
    Py_RETURN_NONE;
}

PyObject * MGLSampler_get_repeat_x(MGLSampler * self) {
    return PyBool_FromLong(self->repeat_x);
}

int MGLSampler_set_repeat_x(MGLSampler * self, PyObject * value) {
    const GLMethods & gl = self->context->gl;

    if (value == Py_True) {
        gl.SamplerParameteri(self->sampler_obj, GL_TEXTURE_WRAP_S, GL_REPEAT);
        self->repeat_x = true;
        return 0;
    } else if (value == Py_False) {
        gl.SamplerParameteri(self->sampler_obj, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        self->repeat_x = false;
        return 0;
    } else {
        MGLError_Set("invalid value for texture_x");
        return -1;
    }
}

PyObject * MGLSampler_get_repeat_y(MGLSampler * self) {
    return PyBool_FromLong(self->repeat_y);
}

int MGLSampler_set_repeat_y(MGLSampler * self, PyObject * value) {
    const GLMethods & gl = self->context->gl;

    if (value == Py_True) {
        gl.SamplerParameteri(self->sampler_obj, GL_TEXTURE_WRAP_T, GL_REPEAT);
        self->repeat_y = true;
        return 0;
    } else if (value == Py_False) {
        gl.SamplerParameteri(self->sampler_obj, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        self->repeat_y = false;
        return 0;
    } else {
        MGLError_Set("invalid value for texture_y");
        return -1;
    }
}

PyObject * MGLSampler_get_repeat_z(MGLSampler * self) {
    return PyBool_FromLong(self->repeat_z);
}

int MGLSampler_set_repeat_z(MGLSampler * self, PyObject * value) {
    const GLMethods & gl = self->context->gl;

    if (value == Py_True) {
        gl.SamplerParameteri(self->sampler_obj, GL_TEXTURE_WRAP_R, GL_REPEAT);
        self->repeat_z = true;
        return 0;
    } else if (value == Py_False) {
        gl.SamplerParameteri(self->sampler_obj, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        self->repeat_z = false;
        return 0;
    } else {
        MGLError_Set("invalid value for texture_z");
        return -1;
    }
}

PyObject * MGLSampler_get_filter(MGLSampler * self) {
    PyObject * res = PyTuple_New(2);
    PyTuple_SET_ITEM(res, 0, PyLong_FromLong(self->min_filter));
    PyTuple_SET_ITEM(res, 1, PyLong_FromLong(self->mag_filter));
    return res;
}

int MGLSampler_set_filter(MGLSampler * self, PyObject * value) {
    if (PyTuple_GET_SIZE(value) != 2) {
        MGLError_Set("invalid filter");
        return -1;
    }

    self->min_filter = PyLong_AsLong(PyTuple_GET_ITEM(value, 0));
    self->mag_filter = PyLong_AsLong(PyTuple_GET_ITEM(value, 1));

    const GLMethods & gl = self->context->gl;
    gl.SamplerParameteri(self->sampler_obj, GL_TEXTURE_MIN_FILTER, self->min_filter);
    gl.SamplerParameteri(self->sampler_obj, GL_TEXTURE_MAG_FILTER, self->mag_filter);

    return 0;
}

PyObject * MGLSampler_get_compare_func(MGLSampler * self) {
    return PyObject_CallMethod(helper, "compare_func_to_str", "(i)", self->compare_func);
}

int MGLSampler_set_compare_func(MGLSampler * self, PyObject * value) {
    PyObject * compare_func = PyObject_CallMethod(helper, "compare_func_from_str", "(O)", value);
    if (!compare_func) {
        return -1;
    }

    self->compare_func = PyLong_AsLong(compare_func);

    const GLMethods & gl = self->context->gl;

    if (self->compare_func == 0) {
        gl.SamplerParameteri(self->sampler_obj, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    } else {
        gl.SamplerParameteri(self->sampler_obj, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        gl.SamplerParameteri(self->sampler_obj, GL_TEXTURE_COMPARE_FUNC, self->compare_func);
    }

    return 0;
}

PyObject * MGLSampler_get_anisotropy(MGLSampler * self) {
    return PyFloat_FromDouble(self->anisotropy);
}

int MGLSampler_set_anisotropy(MGLSampler * self, PyObject * value) {
    self->anisotropy = (float)MGL_min(MGL_max(PyFloat_AsDouble(value), 1.0), self->context->max_anisotropy);

    const GLMethods & gl = self->context->gl;
    gl.SamplerParameterf(self->sampler_obj, GL_TEXTURE_MAX_ANISOTROPY, self->anisotropy);

    return 0;
}

PyObject * MGLSampler_get_border_color(MGLSampler * self) {
    PyObject * r = PyFloat_FromDouble(self->border_color[0]);
    PyObject * g = PyFloat_FromDouble(self->border_color[1]);
    PyObject * b = PyFloat_FromDouble(self->border_color[2]);
    PyObject * a = PyFloat_FromDouble(self->border_color[3]);
    return PyTuple_Pack(4, r, g, b, a);
}

int MGLSampler_set_border_color(MGLSampler * self, PyObject * value) {
    if (PyTuple_GET_SIZE(value) != 4) {
        MGLError_Set("border_color must be a 4-tuple not %d-tuple", PyTuple_GET_SIZE(value));
        return -1;
    }

    float r = (float)PyFloat_AsDouble(PyTuple_GET_ITEM(value, 0));
    float g = (float)PyFloat_AsDouble(PyTuple_GET_ITEM(value, 1));
    float b = (float)PyFloat_AsDouble(PyTuple_GET_ITEM(value, 2));
    float a = (float)PyFloat_AsDouble(PyTuple_GET_ITEM(value, 3));

    if (PyErr_Occurred()) {
        MGLError_Set("the border_color is invalid");
        return -1;
    }

    self->border_color[0] = r;
    self->border_color[1] = g;
    self->border_color[2] = b;
    self->border_color[3] = a;

    const GLMethods & gl = self->context->gl;
    gl.SamplerParameteri(self->sampler_obj, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    gl.SamplerParameteri(self->sampler_obj, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    gl.SamplerParameteri(self->sampler_obj, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    gl.SamplerParameterfv(self->sampler_obj, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&self->border_color);

    return 0;
}

PyObject * MGLSampler_get_min_lod(MGLSampler * self) {
    return PyFloat_FromDouble(self->min_lod);
}

int MGLSampler_set_min_lod(MGLSampler * self, PyObject * value) {
    self->min_lod = (float)PyFloat_AsDouble(value);

    const GLMethods & gl = self->context->gl;
    gl.SamplerParameterf(self->sampler_obj, GL_TEXTURE_MIN_LOD, self->min_lod);

    return 0;
}

PyObject * MGLSampler_get_max_lod(MGLSampler * self) {
    return PyFloat_FromDouble(self->max_lod);
}

int MGLSampler_set_max_lod(MGLSampler * self, PyObject * value) {
    self->max_lod = (float)PyFloat_AsDouble(value);

    const GLMethods & gl = self->context->gl;
    gl.SamplerParameterf(self->sampler_obj, GL_TEXTURE_MAX_LOD, self->max_lod);

    return 0;
}

PyObject * MGLContext_scope(MGLContext * self, PyObject * args) {
    MGLFramebuffer * framebuffer;
    PyObject * enable_flags;
    PyObject * textures;
    PyObject * uniform_buffers;
    PyObject * shader_storage_buffers;
    PyObject * samplers;

    int args_ok = PyArg_ParseTuple(
        args,
        "O!OOOOO",
        MGLFramebuffer_type,
        &framebuffer,
        &enable_flags,
        &textures,
        &uniform_buffers,
        &shader_storage_buffers,
        &samplers
    );

    if (!args_ok) {
        return 0;
    }

    int flags = MGL_INVALID;
    if (enable_flags != Py_None) {
        flags = PyLong_AsLong(enable_flags);
        if (PyErr_Occurred()) {
            MGLError_Set("invalid enable_flags");
            return 0;
        }
    }

    MGLScope * scope = PyObject_New(MGLScope, MGLScope_type);
    scope->released = false;

    Py_INCREF(self);
    scope->context = self;

    scope->enable_flags = flags;

    Py_INCREF(framebuffer);
    scope->framebuffer = framebuffer;

    Py_INCREF(self->bound_framebuffer);
    scope->old_framebuffer = self->bound_framebuffer;

    int num_textures = (int)PyTuple_Size(textures);
    int num_uniform_buffers = (int)PyTuple_Size(uniform_buffers);
    int num_shader_storage_buffers = (int)PyTuple_Size(shader_storage_buffers);

    scope->num_textures = num_textures;
    scope->textures = new int[scope->num_textures * 3];
    scope->num_buffers = num_uniform_buffers + num_shader_storage_buffers;
    scope->buffers = new int[scope->num_buffers * 3];

    scope->samplers = PySequence_Fast(samplers, "not iterable");

    for (int i = 0; i < num_textures; ++i) {
        PyObject * tup = PyTuple_GET_ITEM(textures, i);
        PyObject * item = PyTuple_GET_ITEM(tup, 0);

        int texture_type;
        int texture_obj;

        if (Py_TYPE(item) == MGLTexture_type) {
            MGLTexture * texture = (MGLTexture *)item;
            texture_type = texture->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
            texture_obj = texture->texture_obj;
        } else if (Py_TYPE(item) == MGLTexture3D_type) {
            MGLTexture3D * texture = (MGLTexture3D *)item;
            texture_type = GL_TEXTURE_3D;
            texture_obj = texture->texture_obj;
        } else if (Py_TYPE(item) == MGLTextureCube_type) {
            MGLTextureCube * texture = (MGLTextureCube *)item;
            texture_type = GL_TEXTURE_CUBE_MAP;
            texture_obj = texture->texture_obj;
        } else {
            MGLError_Set("invalid texture");
            return 0;
        }

        int binding = PyLong_AsLong(PyTuple_GET_ITEM(tup, 1));
        scope->textures[i * 3 + 0] = GL_TEXTURE0 + binding;
        scope->textures[i * 3 + 1] = texture_type;
        scope->textures[i * 3 + 2] = texture_obj;
    }

    for (int i = 0; i < num_uniform_buffers; ++i) {
        PyObject * tup = PyTuple_GET_ITEM(uniform_buffers, i);
        MGLBuffer * buffer = (MGLBuffer *)PyTuple_GET_ITEM(tup, 0);

        if (Py_TYPE(buffer) == MGLBuffer_type) {
            int binding = PyLong_AsLong(PyTuple_GET_ITEM(tup, 1));
            scope->buffers[i * 3 + 0] = GL_UNIFORM_BUFFER;
            scope->buffers[i * 3 + 1] = buffer->buffer_obj;
            scope->buffers[i * 3 + 2] = binding;
        } else {
            MGLError_Set("invalid buffer");
            return 0;
        }
    }

    int base = num_uniform_buffers * 3;

    for (int i = 0; i < num_shader_storage_buffers; ++i) {
        PyObject * tup = PyTuple_GET_ITEM(shader_storage_buffers, i);
        MGLBuffer * buffer = (MGLBuffer *)PyTuple_GET_ITEM(tup, 0);

        if (Py_TYPE(buffer) == MGLBuffer_type) {
            int binding = PyLong_AsLong(PyTuple_GET_ITEM(tup, 1));
            scope->buffers[base + i * 3 + 0] = GL_SHADER_STORAGE_BUFFER;
            scope->buffers[base + i * 3 + 1] = buffer->buffer_obj;
            scope->buffers[base + i * 3 + 2] = binding;
        } else {
            MGLError_Set("invalid buffer");
            return 0;
        }
    }
    Py_INCREF(scope);

    return (PyObject *)scope;
}

extern PyObject * MGLFramebuffer_use(MGLFramebuffer * self);

PyObject * MGLScope_begin(MGLScope * self) {
    const GLMethods & gl = self->context->gl;
    const int & flags = self->enable_flags;

    self->old_enable_flags = self->context->enable_flags;
    self->context->enable_flags = self->enable_flags;

    MGLFramebuffer_use(self->framebuffer);

    for (int i = 0; i < self->num_textures; ++i) {
        gl.ActiveTexture(self->textures[i * 3]);
        gl.BindTexture(self->textures[i * 3 + 1], self->textures[i * 3 + 2]);
    }

    for (int i = 0; i < self->num_buffers; ++i) {
        gl.BindBufferBase(self->buffers[i * 3], self->buffers[i * 3 + 2], self->buffers[i * 3 + 1]);
    }

    int num_samplers = (int)PySequence_Fast_GET_SIZE(self->samplers);
    for (int i = 0; i < num_samplers; ++i) {
        PyObject * pair = PySequence_Fast(PySequence_Fast_GET_ITEM(self->samplers, i), "not iterable");
        if (PySequence_Fast_GET_SIZE(pair) != 2) {
            return NULL;
        }
        PyObject * call = PyObject_CallMethod(PySequence_Fast_GET_ITEM(pair, 0), "use", "O", PySequence_Fast_GET_ITEM(pair, 1));
        Py_XDECREF(call);
        if (!call) {
            return NULL;
        }
    }

    if (flags & MGL_BLEND) {
        gl.Enable(GL_BLEND);
    } else {
        gl.Disable(GL_BLEND);
    }

    if (flags & MGL_DEPTH_TEST) {
        gl.Enable(GL_DEPTH_TEST);
    } else {
        gl.Disable(GL_DEPTH_TEST);
    }

    if (flags & MGL_CULL_FACE) {
        gl.Enable(GL_CULL_FACE);
    } else {
        gl.Disable(GL_CULL_FACE);
    }

    if (flags & MGL_RASTERIZER_DISCARD) {
        gl.Enable(GL_RASTERIZER_DISCARD);
    } else {
        gl.Disable(GL_RASTERIZER_DISCARD);
    }

    if (flags & MGL_PROGRAM_POINT_SIZE) {
        gl.Enable(GL_PROGRAM_POINT_SIZE);
    } else {
        gl.Disable(GL_PROGRAM_POINT_SIZE);
    }

    Py_RETURN_NONE;
}

PyObject * MGLScope_end(MGLScope * self) {
    const GLMethods & gl = self->context->gl;
    const int & flags = self->old_enable_flags;

    self->context->enable_flags = self->old_enable_flags;

    MGLFramebuffer_use(self->old_framebuffer);

    if (flags & MGL_BLEND) {
        gl.Enable(GL_BLEND);
    } else {
        gl.Disable(GL_BLEND);
    }

    if (flags & MGL_DEPTH_TEST) {
        gl.Enable(GL_DEPTH_TEST);
    } else {
        gl.Disable(GL_DEPTH_TEST);
    }

    if (flags & MGL_CULL_FACE) {
        gl.Enable(GL_CULL_FACE);
    } else {
        gl.Disable(GL_CULL_FACE);
    }

    if (flags & MGL_RASTERIZER_DISCARD) {
        gl.Enable(GL_RASTERIZER_DISCARD);
    } else {
        gl.Disable(GL_RASTERIZER_DISCARD);
    }

    if (flags & MGL_PROGRAM_POINT_SIZE) {
        gl.Enable(GL_PROGRAM_POINT_SIZE);
    } else {
        gl.Disable(GL_PROGRAM_POINT_SIZE);
    }

    Py_RETURN_NONE;
}

PyObject * MGLScope_release(MGLScope * self) {
    if (self->released) {
        Py_RETURN_NONE;
    }
    self->released = true;

    Py_DECREF(self->framebuffer);
    Py_DECREF(self->old_framebuffer);

    Py_DECREF(self->context);
    Py_DECREF(self);
    Py_RETURN_NONE;
}

PyObject * MGLContext_texture(MGLContext * self, PyObject * args) {
    int width;
    int height;

    int components;

    PyObject * data;

    int samples;
    int alignment;

    const char * dtype;
    Py_ssize_t dtype_size;
    int internal_format_override;

    int args_ok = PyArg_ParseTuple(
        args,
        "(II)IOIIs#I",
        &width,
        &height,
        &components,
        &data,
        &samples,
        &alignment,
        &dtype,
        &dtype_size,
        &internal_format_override
    );

    if (!args_ok) {
        return 0;
    }

    if (components < 1 || components > 4) {
        MGLError_Set("the components must be 1, 2, 3 or 4");
        return 0;
    }

    if ((samples & (samples - 1)) || samples > self->max_samples) {
        MGLError_Set("the number of samples is invalid");
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    if (data != Py_None && samples) {
        MGLError_Set("multisample textures are not writable directly");
        return 0;
    }

    MGLDataType * data_type = from_dtype(dtype, dtype_size);

    if (!data_type) {
        MGLError_Set("invalid dtype");
        return 0;
    }

    int expected_size = width * components * data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * height;

    Py_buffer buffer_view;

    if (data != Py_None) {
        int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
        if (get_buffer < 0) {
            // Propagate the default error
            return 0;
        }
    } else {
        buffer_view.len = expected_size;
        buffer_view.buf = 0;
    }

    if (buffer_view.len != expected_size) {
        MGLError_Set("data size mismatch %d != %d", buffer_view.len, expected_size);
        if (data != Py_None) {
            PyBuffer_Release(&buffer_view);
        }
        return 0;
    }

    int texture_target = samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    int pixel_type = data_type->gl_type;
    int base_format = data_type->base_format[components];
    int internal_format = internal_format_override ? internal_format_override : data_type->internal_format[components];

    const GLMethods & gl = self->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);

    MGLTexture * texture = PyObject_New(MGLTexture, MGLTexture_type);
    texture->released = false;
    texture->external = false;

    texture->texture_obj = 0;
    gl.GenTextures(1, (GLuint *)&texture->texture_obj);

    if (!texture->texture_obj) {
        MGLError_Set("cannot create texture");
        Py_DECREF(texture);
        return 0;
    }

    gl.BindTexture(texture_target, texture->texture_obj);

    if (samples) {
        gl.TexImage2DMultisample(texture_target, samples, internal_format, width, height, true);
    } else {
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.TexImage2D(texture_target, 0, internal_format, width, height, 0, base_format, pixel_type, buffer_view.buf);
        if (data_type->float_type) {
            gl.TexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            gl.TexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            gl.TexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            gl.TexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
    }

    if (data != Py_None) {
        PyBuffer_Release(&buffer_view);
    }

    texture->width = width;
    texture->height = height;
    texture->components = components;
    texture->samples = samples;
    texture->data_type = data_type;

    texture->max_level = 0;
    texture->compare_func = 0;
    texture->anisotropy = 1.0f;
    texture->depth = false;

    texture->min_filter = data_type->float_type ? GL_LINEAR : GL_NEAREST;
    texture->mag_filter = data_type->float_type ? GL_LINEAR : GL_NEAREST;

    texture->repeat_x = true;
    texture->repeat_y = true;

    Py_INCREF(self);
    texture->context = self;

    Py_INCREF(texture);

    PyObject * result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, (PyObject *)texture);
    PyTuple_SET_ITEM(result, 1, PyLong_FromLong(texture->texture_obj));
    return result;
}

PyObject * MGLContext_depth_texture(MGLContext * self, PyObject * args) {
    int width;
    int height;

    PyObject * data;

    int samples;
    int alignment;

    int args_ok = PyArg_ParseTuple(args, "(II)OII", &width, &height, &data, &samples, &alignment);
    if (!args_ok) {
        return 0;
    }

    if ((samples & (samples - 1)) || samples > self->max_samples) {
        MGLError_Set("the number of samples is invalid");
        return 0;
    }

    if (data != Py_None && samples) {
        MGLError_Set("multisample textures are not writable directly");
        return 0;
    }

    int expected_size = width * 4;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * height;

    Py_buffer buffer_view;

    if (data != Py_None) {
        int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
        if (get_buffer < 0) {
            // Propagate the default error
            return 0;
        }
    } else {
        buffer_view.len = expected_size;
        buffer_view.buf = 0;
    }

    if (buffer_view.len != expected_size) {
        MGLError_Set("data size mismatch %d != %d", buffer_view.len, expected_size);
        if (data != Py_None) {
            PyBuffer_Release(&buffer_view);
        }
        return 0;
    }

    int texture_target = samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    int pixel_type = GL_FLOAT;

    const GLMethods & gl = self->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);

    MGLTexture * texture = PyObject_New(MGLTexture, MGLTexture_type);
    texture->released = false;
    texture->external = false;

    texture->texture_obj = 0;
    gl.GenTextures(1, (GLuint *)&texture->texture_obj);

    if (!texture->texture_obj) {
        MGLError_Set("cannot create texture");
        Py_DECREF(texture);
        return 0;
    }

    gl.BindTexture(texture_target, texture->texture_obj);

    if (samples) {
        gl.TexImage2DMultisample(texture_target, samples, GL_DEPTH_COMPONENT24, width, height, true);
    } else {
        gl.TexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl.TexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.TexImage2D(texture_target, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, pixel_type, buffer_view.buf);
        gl.TexParameteri(texture_target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        gl.TexParameteri(texture_target, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    }

    if (data != Py_None) {
        PyBuffer_Release(&buffer_view);
    }

    texture->width = width;
    texture->height = height;
    texture->components = 1;
    texture->samples = samples;
    texture->data_type = from_dtype("f4", 2);

    texture->compare_func = GL_LEQUAL;
    texture->depth = true;

    texture->min_filter = GL_LINEAR;
    texture->mag_filter = GL_LINEAR;
    texture->max_level = 0;

    texture->repeat_x = false;
    texture->repeat_y = false;

    Py_INCREF(self);
    texture->context = self;

    Py_INCREF(texture);

    PyObject * result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, (PyObject *)texture);
    PyTuple_SET_ITEM(result, 1, PyLong_FromLong(texture->texture_obj));
    return result;
}

PyObject * MGLContext_external_texture(MGLContext * self, PyObject * args) {
    int glo;
    int width;
    int height;
    int components;
    int samples;
    const char * dtype;
    Py_ssize_t dtype_size;

    int args_ok = PyArg_ParseTuple(args, "I(II)IIs#", &glo, &width, &height, &components, &samples, &dtype, &dtype_size);
    if (!args_ok) {
        return 0;
    }

    MGLDataType * data_type = from_dtype(dtype, dtype_size);

    if (!data_type) {
        MGLError_Set("invalid dtype");
        return 0;
    }

    MGLTexture * texture = PyObject_New(MGLTexture, MGLTexture_type);
    texture->released = false;
    texture->external = true;

    texture->texture_obj = glo;
    texture->width = width;
    texture->height = height;
    texture->components = components;
    texture->samples = samples;
    texture->data_type = data_type;

    texture->max_level = 0;
    texture->compare_func = 0;
    texture->anisotropy = 1.0f;
    texture->depth = false;

    texture->min_filter = data_type->float_type ? GL_LINEAR : GL_NEAREST;
    texture->mag_filter = data_type->float_type ? GL_LINEAR : GL_NEAREST;

    texture->repeat_x = true;
    texture->repeat_y = true;

    Py_INCREF(self);
    texture->context = self;

    Py_INCREF(texture);

    PyObject * result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, (PyObject *)texture);
    PyTuple_SET_ITEM(result, 1, PyLong_FromLong(texture->texture_obj));
    return result;
}

PyObject * MGLTexture_read(MGLTexture * self, PyObject * args) {
    int level;
    int alignment;

    int args_ok = PyArg_ParseTuple(args, "II", &level, &alignment);
    if (!args_ok) {
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    if (level > self->max_level) {
        MGLError_Set("invalid level");
        return 0;
    }

    if (self->samples) {
        MGLError_Set("multisample textures cannot be read directly");
        return 0;
    }

    int width = self->width / (1 << level);
    int height = self->height / (1 << level);

    width = width > 1 ? width : 1;
    height = height > 1 ? height : 1;

    int expected_size = width * self->components * self->data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * height;

    PyObject * result = PyBytes_FromStringAndSize(0, expected_size);
    char * data = PyBytes_AS_STRING(result);

    int pixel_type = self->data_type->gl_type;
    int base_format = self->depth ? GL_DEPTH_COMPONENT : self->data_type->base_format[self->components];

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_2D, self->texture_obj);

    gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
    gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);

    // To determine the required size of pixels, use glGetTexLevelParameter to determine
    // the dimensions of the internal texture image, then scale the required number of pixels
    // by the storage required for each pixel, based on format and type. Be sure to take the
    // pixel storage parameters into account, especially GL_PACK_ALIGNMENT.

    // int pack = 0;
    // gl.GetIntegerv(GL_PACK_ALIGNMENT, &pack);
    // printf("GL_PACK_ALIGNMENT: %d\n", pack);

    // glGetTexLevelParameter with argument GL_TEXTURE_WIDTH
    // glGetTexLevelParameter with argument GL_TEXTURE_HEIGHT
    // glGetTexLevelParameter with argument GL_TEXTURE_INTERNAL_FORMAT

    // int level_width = 0;
    // int level_height = 0;
    // gl.GetTexLevelParameteriv(texture_target, 0, GL_TEXTURE_WIDTH, &level_width);
    // gl.GetTexLevelParameteriv(texture_target, 0, GL_TEXTURE_HEIGHT, &level_height);
    // printf("level_width: %d\n", level_width);
    // printf("level_height: %d\n", level_height);

    gl.GetTexImage(GL_TEXTURE_2D, level, base_format, pixel_type, data);

    return result;
}

PyObject * MGLTexture_read_into(MGLTexture * self, PyObject * args) {
    PyObject * data;
    int level;
    int alignment;
    Py_ssize_t write_offset;

    int args_ok = PyArg_ParseTuple(args, "OIIn", &data, &level, &alignment, &write_offset);
    if (!args_ok) {
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    if (level > self->max_level) {
        MGLError_Set("invalid level");
        return 0;
    }

    if (self->samples) {
        MGLError_Set("multisample textures cannot be read directly");
        return 0;
    }

    int width = self->width / (1 << level);
    int height = self->height / (1 << level);

    width = width > 1 ? width : 1;
    height = height > 1 ? height : 1;

    int expected_size = width * self->components * self->data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * height;

    int pixel_type = self->data_type->gl_type;
    int base_format = self->depth ? GL_DEPTH_COMPONENT : self->data_type->base_format[self->components];

    if (Py_TYPE(data) == MGLBuffer_type) {

        MGLBuffer * buffer = (MGLBuffer *)data;

        const GLMethods & gl = self->context->gl;

        gl.BindBuffer(GL_PIXEL_PACK_BUFFER, buffer->buffer_obj);
        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(GL_TEXTURE_2D, self->texture_obj);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.GetTexImage(GL_TEXTURE_2D, level, base_format, pixel_type, (void *)write_offset);
        gl.BindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    } else {

        Py_buffer buffer_view;

        int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_WRITABLE);
        if (get_buffer < 0) {
            // Propagate the default error
            return 0;
        }

        if (buffer_view.len < write_offset + expected_size) {
            MGLError_Set("the buffer is too small");
            PyBuffer_Release(&buffer_view);
            return 0;
        }

        char * ptr = (char *)buffer_view.buf + write_offset;

        const GLMethods & gl = self->context->gl;

        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(GL_TEXTURE_2D, self->texture_obj);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.GetTexImage(GL_TEXTURE_2D, level, base_format, pixel_type, ptr);

        PyBuffer_Release(&buffer_view);

    }

    Py_RETURN_NONE;
}

PyObject * MGLTexture_write(MGLTexture * self, PyObject * args) {
    PyObject * data;
    PyObject * viewport;
    int level;
    int alignment;

    int args_ok = PyArg_ParseTuple(args, "OOII", &data, &viewport, &level, &alignment);
    if (!args_ok) {
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    if (level > self->max_level) {
        MGLError_Set("invalid level");
        return 0;
    }

    if (self->samples) {
        MGLError_Set("multisample textures cannot be written directly");
        return 0;
    }

    int x = 0;
    int y = 0;
    int width = self->width / (1 << level);
    int height = self->height / (1 << level);

    width = width > 1 ? width : 1;
    height = height > 1 ? height : 1;

    Py_buffer buffer_view;

    if (viewport != Py_None) {
        if (Py_TYPE(viewport) != &PyTuple_Type) {
            MGLError_Set("the viewport must be a tuple not %s", Py_TYPE(viewport)->tp_name);
            return 0;
        }

        if (PyTuple_GET_SIZE(viewport) == 4) {

            x = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 0));
            y = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 1));
            width = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 2));
            height = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 3));

        } else if (PyTuple_GET_SIZE(viewport) == 2) {

            width = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 0));
            height = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 1));

        } else {

            MGLError_Set("the viewport size %d is invalid", PyTuple_GET_SIZE(viewport));
            return 0;

        }

        if (PyErr_Occurred()) {
            MGLError_Set("wrong values in the viewport");
            return 0;
        }

    }

    int expected_size = width * self->components * self->data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * height;

    int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    int pixel_type = self->data_type->gl_type;
    int format = self->data_type->base_format[self->components];

    if (Py_TYPE(data) == MGLBuffer_type) {

        MGLBuffer * buffer = (MGLBuffer *)data;

        const GLMethods & gl = self->context->gl;

        gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer->buffer_obj);
        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(texture_target, self->texture_obj);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.TexSubImage2D(texture_target, level, x, y, width, height, format, pixel_type, 0);
        gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    } else {

        int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
        if (get_buffer < 0) {
            // Propagate the default error
            return 0;
        }

        if (buffer_view.len != expected_size) {
            MGLError_Set("data size mismatch %d != %d", buffer_view.len, expected_size);
            if (data != Py_None) {
                PyBuffer_Release(&buffer_view);
            }
            return 0;
        }

        const GLMethods & gl = self->context->gl;

        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(texture_target, self->texture_obj);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.TexSubImage2D(texture_target, level, x, y, width, height, format, pixel_type, buffer_view.buf);

        PyBuffer_Release(&buffer_view);

    }

    Py_RETURN_NONE;
}

PyObject * MGLTexture_meth_bind(MGLTexture * self, PyObject * args) {
    int unit;
    int read;
    int write;
    int level;
    int format;

    int args_ok = PyArg_ParseTuple(args, "IppII", &unit, &read, &write, &level, &format);
    if (!args_ok) {
        return NULL;
    }

    int access = GL_READ_WRITE;
    if (read && !write) access = GL_READ_ONLY;
    else if (!read && write) access = GL_WRITE_ONLY;
    else if (!read && !write) {
        MGLError_Set("Illegal access mode. Read or write needs to be enabled.");
        return NULL;
    }

    int frmt = format ? format : self->data_type->internal_format[self->components];

    const GLMethods & gl = self->context->gl;
    gl.BindImageTexture(unit, self->texture_obj, level, 0, 0, access, frmt);
    Py_RETURN_NONE;
}

PyObject * MGLTexture_use(MGLTexture * self, PyObject * args) {
    int index;

    int args_ok = PyArg_ParseTuple(args, "I", &index);
    if (!args_ok) {
        return 0;
    }

    int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

    const GLMethods & gl = self->context->gl;
    gl.ActiveTexture(GL_TEXTURE0 + index);
    gl.BindTexture(texture_target, self->texture_obj);

    Py_RETURN_NONE;
}

PyObject * MGLTexture_build_mipmaps(MGLTexture * self, PyObject * args) {
    int base = 0;
    int max = 1000;

    int args_ok = PyArg_ParseTuple(args, "II", &base, &max);
    if (!args_ok) {
        return 0;
    }

    if (base > self->max_level) {
        MGLError_Set("invalid base");
        return 0;
    }

    int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(texture_target, self->texture_obj);

    gl.TexParameteri(texture_target, GL_TEXTURE_BASE_LEVEL, base);
    gl.TexParameteri(texture_target, GL_TEXTURE_MAX_LEVEL, max);

    gl.GenerateMipmap(texture_target);

    gl.TexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    gl.TexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    self->min_filter = GL_LINEAR_MIPMAP_LINEAR;
    self->mag_filter = GL_LINEAR;
    self->max_level = max;

    Py_RETURN_NONE;
}

PyObject * MGLTexture_release(MGLTexture * self) {
    if (self->released) {
        Py_RETURN_NONE;
    }
    self->released = true;

    const GLMethods & gl = self->context->gl;
    gl.DeleteTextures(1, (GLuint *)&self->texture_obj);

    Py_DECREF(self->context);
    Py_DECREF(self);
    Py_RETURN_NONE;
}

PyObject * MGLTexture_get_repeat_x(MGLTexture * self) {
    return PyBool_FromLong(self->repeat_x);
}

int MGLTexture_set_repeat_x(MGLTexture * self, PyObject * value) {
    int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(texture_target, self->texture_obj);

    if (value == Py_True) {
        gl.TexParameteri(texture_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        self->repeat_x = true;
        return 0;
    } else if (value == Py_False) {
        gl.TexParameteri(texture_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        self->repeat_x = false;
        return 0;
    } else {
        MGLError_Set("invalid value for texture_x");
        return -1;
    }
}

PyObject * MGLTexture_get_repeat_y(MGLTexture * self) {
    return PyBool_FromLong(self->repeat_y);
}

int MGLTexture_set_repeat_y(MGLTexture * self, PyObject * value) {
    int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(texture_target, self->texture_obj);

    if (value == Py_True) {
        gl.TexParameteri(texture_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
        self->repeat_y = true;
        return 0;
    } else if (value == Py_False) {
        gl.TexParameteri(texture_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        self->repeat_y = false;
        return 0;
    } else {
        MGLError_Set("invalid value for texture_y");
        return -1;
    }
}

PyObject * MGLTexture_get_filter(MGLTexture * self) {
    PyObject * res = PyTuple_New(2);
    PyTuple_SET_ITEM(res, 0, PyLong_FromLong(self->min_filter));
    PyTuple_SET_ITEM(res, 1, PyLong_FromLong(self->mag_filter));
    return res;
}

int MGLTexture_set_filter(MGLTexture * self, PyObject * value) {
    if (PyTuple_GET_SIZE(value) != 2) {
        MGLError_Set("invalid filter");
        return -1;
    }

    self->min_filter = PyLong_AsLong(PyTuple_GET_ITEM(value, 0));
    self->mag_filter = PyLong_AsLong(PyTuple_GET_ITEM(value, 1));

    int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(texture_target, self->texture_obj);
    gl.TexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, self->min_filter);
    gl.TexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, self->mag_filter);

    return 0;
}

PyObject * get_swizzle(MGLContext * context, int texture_target, int texture_obj, bool depth) {
    if (depth) {
        MGLError_Set("cannot get swizzle of depth textures");
        return NULL;
    }

    const GLMethods & gl = context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + context->default_texture_unit);
    gl.BindTexture(texture_target, texture_obj);

    int swizzle_r = 0;
    int swizzle_g = 0;
    int swizzle_b = 0;
    int swizzle_a = 0;

    gl.GetTexParameteriv(texture_target, GL_TEXTURE_SWIZZLE_R, &swizzle_r);
    gl.GetTexParameteriv(texture_target, GL_TEXTURE_SWIZZLE_G, &swizzle_g);
    gl.GetTexParameteriv(texture_target, GL_TEXTURE_SWIZZLE_B, &swizzle_b);
    gl.GetTexParameteriv(texture_target, GL_TEXTURE_SWIZZLE_A, &swizzle_a);

    return PyObject_CallMethod(helper, "swizzle_to_str", "(iiii)", swizzle_r, swizzle_g, swizzle_b, swizzle_a);
}

int set_swizzle(MGLContext * context, int texture_target, int texture_obj, bool depth, PyObject * value) {
    if (depth) {
        MGLError_Set("cannot set swizzle for depth textures");
        return -1;
    }

    PyObject * tup = PyObject_CallMethod(helper, "swizzle_from_str", "(O)", value);
    if (!tup) {
        return -1;
    }

    const GLMethods & gl = context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + context->default_texture_unit);
    gl.BindTexture(texture_target, texture_obj);

    gl.TexParameteri(texture_target, GL_TEXTURE_SWIZZLE_R, PyLong_AsLong(PyTuple_GetItem(tup, 0)));
    gl.TexParameteri(texture_target, GL_TEXTURE_SWIZZLE_G, PyLong_AsLong(PyTuple_GetItem(tup, 1)));
    gl.TexParameteri(texture_target, GL_TEXTURE_SWIZZLE_B, PyLong_AsLong(PyTuple_GetItem(tup, 2)));
    gl.TexParameteri(texture_target, GL_TEXTURE_SWIZZLE_A, PyLong_AsLong(PyTuple_GetItem(tup, 3)));

    Py_DECREF(tup);
    return 0;
}

PyObject * MGLTexture_get_swizzle(MGLTexture * self) {
    int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    return get_swizzle(self->context, texture_target, self->texture_obj, self->depth);
}

int MGLTexture_set_swizzle(MGLTexture * self, PyObject * value) {
    int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    return set_swizzle(self->context, texture_target, self->texture_obj, self->depth, value);
}

PyObject * MGLTexture_get_compare_func(MGLTexture * self) {
    if (!self->depth) {
        MGLError_Set("only depth textures have compare_func");
        return 0;
    }

    return PyObject_CallMethod(helper, "compare_func_to_str", "(i)", self->compare_func);
}

int MGLTexture_set_compare_func(MGLTexture * self, PyObject * value) {
    if (!self->depth) {
        MGLError_Set("only depth textures have compare_func");
        return -1;
    }

    PyObject * compare_func = PyObject_CallMethod(helper, "compare_func_from_str", "(O)", value);
    if (!compare_func) {
        return -1;
    }

    int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

    self->compare_func = PyLong_AsLong(compare_func);

    const GLMethods & gl = self->context->gl;
    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(texture_target, self->texture_obj);
    if (!self->compare_func) {
        gl.TexParameteri(texture_target, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    } else {
        gl.TexParameteri(texture_target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        gl.TexParameteri(texture_target, GL_TEXTURE_COMPARE_FUNC, self->compare_func);
    }

    return 0;
}

PyObject * MGLTexture_get_anisotropy(MGLTexture * self) {
    return PyFloat_FromDouble(self->anisotropy);
}

int MGLTexture_set_anisotropy(MGLTexture * self, PyObject * value) {
    self->anisotropy = (float)MGL_min(MGL_max(PyFloat_AsDouble(value), 1.0), self->context->max_anisotropy);
    int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(texture_target, self->texture_obj);
    gl.TexParameterf(texture_target, GL_TEXTURE_MAX_ANISOTROPY, self->anisotropy);

    return 0;
}

PyObject * MGLContext_texture3d(MGLContext * self, PyObject * args) {
    int width;
    int height;
    int depth;

    int components;

    PyObject * data;

    int alignment;

    const char * dtype;
    Py_ssize_t dtype_size;

    int args_ok = PyArg_ParseTuple(args, "(III)IOIs#", &width, &height, &depth, &components, &data, &alignment, &dtype, &dtype_size);
    if (!args_ok) {
        return 0;
    }

    if (components < 1 || components > 4) {
        MGLError_Set("the components must be 1, 2, 3 or 4");
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    MGLDataType * data_type = from_dtype(dtype, dtype_size);

    if (!data_type) {
        MGLError_Set("invalid dtype");
        return 0;
    }

    int expected_size = width * components * data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * height * depth;

    Py_buffer buffer_view;

    if (data != Py_None) {
        int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
        if (get_buffer < 0) {
            // Propagate the default error
            return 0;
        }
    } else {
        buffer_view.len = expected_size;
        buffer_view.buf = 0;
    }

    if (buffer_view.len != expected_size) {
        MGLError_Set("data size mismatch %d != %d", buffer_view.len, expected_size);
        if (data != Py_None) {
            PyBuffer_Release(&buffer_view);
        }
        return 0;
    }

    int pixel_type = data_type->gl_type;
    int base_format = data_type->base_format[components];
    int internal_format = data_type->internal_format[components];

    const GLMethods & gl = self->gl;

    MGLTexture3D * texture = PyObject_New(MGLTexture3D, MGLTexture3D_type);
    texture->released = false;

    texture->texture_obj = 0;
    gl.GenTextures(1, (GLuint *)&texture->texture_obj);

    if (!texture->texture_obj) {
        MGLError_Set("cannot create texture");
        Py_DECREF(texture);
        return 0;
    }

    gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_3D, texture->texture_obj);

    gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
    gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
    gl.TexImage3D(GL_TEXTURE_3D, 0, internal_format, width, height, depth, 0, base_format, pixel_type, buffer_view.buf);
    if (data_type->float_type) {
        gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    if (data != Py_None) {
        PyBuffer_Release(&buffer_view);
    }

    texture->width = width;
    texture->height = height;
    texture->depth = depth;
    texture->components = components;
    texture->data_type = data_type;

    texture->min_filter = data_type->float_type ? GL_LINEAR : GL_NEAREST;
    texture->mag_filter = data_type->float_type ? GL_LINEAR : GL_NEAREST;
    texture->max_level = 0;

    texture->repeat_x = true;
    texture->repeat_y = true;
    texture->repeat_z = true;

    Py_INCREF(self);
    texture->context = self;

    Py_INCREF(texture);

    PyObject * result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, (PyObject *)texture);
    PyTuple_SET_ITEM(result, 1, PyLong_FromLong(texture->texture_obj));
    return result;
}

PyObject * MGLTexture3D_read(MGLTexture3D * self, PyObject * args) {
    int alignment;

    int args_ok = PyArg_ParseTuple(args, "I", &alignment);
    if (!args_ok) {
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    int expected_size = self->width * self->components * self->data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * self->height * self->depth;

    PyObject * result = PyBytes_FromStringAndSize(0, expected_size);
    char * data = PyBytes_AS_STRING(result);

    int pixel_type = self->data_type->gl_type;
    int base_format = self->data_type->base_format[self->components];

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

    gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
    gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
    gl.GetTexImage(GL_TEXTURE_3D, 0, base_format, pixel_type, data);

    return result;
}

PyObject * MGLTexture3D_read_into(MGLTexture3D * self, PyObject * args) {
    PyObject * data;
    int alignment;
    Py_ssize_t write_offset;

    int args_ok = PyArg_ParseTuple(args, "OIn", &data, &alignment, &write_offset);
    if (!args_ok) {
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    int expected_size = self->width * self->components * self->data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * self->height * self->depth;

    int pixel_type = self->data_type->gl_type;
    int format = self->data_type->base_format[self->components];

    if (Py_TYPE(data) == MGLBuffer_type) {

        MGLBuffer * buffer = (MGLBuffer *)data;

        const GLMethods & gl = self->context->gl;

        gl.BindBuffer(GL_PIXEL_PACK_BUFFER, buffer->buffer_obj);
        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.GetTexImage(GL_TEXTURE_3D, 0, format, pixel_type, (void *)write_offset);
        gl.BindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    } else {

        Py_buffer buffer_view;

        int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_WRITABLE);
        if (get_buffer < 0) {
            // Propagate the default error
            return 0;
        }

        if (buffer_view.len < write_offset + expected_size) {
            MGLError_Set("the buffer is too small");
            PyBuffer_Release(&buffer_view);
            return 0;
        }

        char * ptr = (char *)buffer_view.buf + write_offset;

        const GLMethods & gl = self->context->gl;
        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.GetTexImage(GL_TEXTURE_3D, 0, format, pixel_type, ptr);

        PyBuffer_Release(&buffer_view);

    }

    Py_RETURN_NONE;
}

PyObject * MGLTexture3D_write(MGLTexture3D * self, PyObject * args) {
    PyObject * data;
    PyObject * viewport;
    int alignment;

    int args_ok = PyArg_ParseTuple(args, "OOI", &data, &viewport, &alignment);
    if (!args_ok) {
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    int x = 0;
    int y = 0;
    int z = 0;
    int width = self->width;
    int height = self->height;
    int depth = self->depth;

    Py_buffer buffer_view;

    if (viewport != Py_None) {
        if (Py_TYPE(viewport) != &PyTuple_Type) {
            MGLError_Set("the viewport must be a tuple not %s", Py_TYPE(viewport)->tp_name);
            return 0;
        }

        if (PyTuple_GET_SIZE(viewport) == 6) {

            x = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 0));
            y = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 1));
            z = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 2));
            width = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 3));
            height = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 4));
            depth = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 5));

        } else if (PyTuple_GET_SIZE(viewport) == 3) {

            width = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 0));
            height = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 1));
            depth = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 2));

        } else {

            MGLError_Set("the viewport size %d is invalid", PyTuple_GET_SIZE(viewport));
            return 0;

        }

        if (PyErr_Occurred()) {
            MGLError_Set("wrong values in the viewport");
            return 0;
        }

    }

    int expected_size = width * self->components * self->data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * height * depth;

    int pixel_type = self->data_type->gl_type;
    int format = self->data_type->base_format[self->components];

    if (Py_TYPE(data) == MGLBuffer_type) {

        MGLBuffer * buffer = (MGLBuffer *)data;

        const GLMethods & gl = self->context->gl;

        gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer->buffer_obj);
        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.TexSubImage3D(GL_TEXTURE_3D, 0, x, y, z, width, height, depth, format, pixel_type, 0);
        gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    } else {

        int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
        if (get_buffer < 0) {
            // Propagate the default error
            return 0;
        }

        if (buffer_view.len != expected_size) {
            MGLError_Set("data size mismatch %d != %d", buffer_view.len, expected_size);
            if (data != Py_None) {
                PyBuffer_Release(&buffer_view);
            }
            return 0;
        }

        const GLMethods & gl = self->context->gl;

        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.TexSubImage3D(GL_TEXTURE_3D, 0, x, y, z, width, height, depth, format, pixel_type, buffer_view.buf);

        PyBuffer_Release(&buffer_view);

    }

    Py_RETURN_NONE;
}

PyObject * MGLTexture3D_meth_bind(MGLTexture3D * self, PyObject * args) {
    int unit;
    int read;
    int write;
    int level;
    int format;

    int args_ok = PyArg_ParseTuple(args, "IppII", &unit, &read, &write, &level, &format);
    if (!args_ok) {
        return NULL;
    }

    int access = GL_READ_WRITE;
    if (read && !write) access = GL_READ_ONLY;
    else if (!read && write) access = GL_WRITE_ONLY;
    else if (!read && !write) {
        MGLError_Set("Illegal access mode. Read or write needs to be enabled.");
        return NULL;
    }

    int frmt = format ? format : self->data_type->internal_format[self->components];

    const GLMethods & gl = self->context->gl;
    // NOTE: 3D textures must be bound as layered to access the outside z=0
    gl.BindImageTexture(unit, self->texture_obj, level, GL_TRUE, 0, access, frmt);
    Py_RETURN_NONE;
}

PyObject * MGLTexture3D_use(MGLTexture3D * self, PyObject * args) {
    int index;

    int args_ok = PyArg_ParseTuple(args, "I", &index);
    if (!args_ok) {
        return 0;
    }

    const GLMethods & gl = self->context->gl;
    gl.ActiveTexture(GL_TEXTURE0 + index);
    gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

    Py_RETURN_NONE;
}

PyObject * MGLTexture3D_build_mipmaps(MGLTexture3D * self, PyObject * args) {
    int base = 0;
    int max = 1000;

    int args_ok = PyArg_ParseTuple(args, "II", &base, &max);
    if (!args_ok) {
        return 0;
    }

    if (base > self->max_level) {
        MGLError_Set("invalid base");
        return 0;
    }

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

    gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, base);
    gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, max);

    gl.GenerateMipmap(GL_TEXTURE_3D);

    gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    self->min_filter = GL_LINEAR_MIPMAP_LINEAR;
    self->mag_filter = GL_LINEAR;
    self->max_level = max;

    Py_RETURN_NONE;
}

PyObject * MGLTexture3D_release(MGLTexture3D * self) {
    if (self->released) {
        Py_RETURN_NONE;
    }
    self->released = true;

    const GLMethods & gl = self->context->gl;
    gl.DeleteTextures(1, (GLuint *)&self->texture_obj);

    Py_DECREF(self->context);
    Py_DECREF(self);
    Py_RETURN_NONE;
}

PyObject * MGLTexture3D_get_repeat_x(MGLTexture3D * self) {
    return PyBool_FromLong(self->repeat_x);
}

int MGLTexture3D_set_repeat_x(MGLTexture3D * self, PyObject * value) {

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

    if (value == Py_True) {
        gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        self->repeat_x = true;
        return 0;
    } else if (value == Py_False) {
        gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        self->repeat_x = false;
        return 0;
    } else {
        MGLError_Set("invalid value for texture_x");
        return -1;
    }
}

PyObject * MGLTexture3D_get_repeat_y(MGLTexture3D * self) {
    return PyBool_FromLong(self->repeat_y);
}

int MGLTexture3D_set_repeat_y(MGLTexture3D * self, PyObject * value) {

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

    if (value == Py_True) {
        gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        self->repeat_y = true;
        return 0;
    } else if (value == Py_False) {
        gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        self->repeat_y = false;
        return 0;
    } else {
        MGLError_Set("invalid value for texture_y");
        return -1;
    }
}

PyObject * MGLTexture3D_get_repeat_z(MGLTexture3D * self) {
    return PyBool_FromLong(self->repeat_z);
}

int MGLTexture3D_set_repeat_z(MGLTexture3D * self, PyObject * value) {

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

    if (value == Py_True) {
        gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        self->repeat_z = true;
        return 0;
    } else if (value == Py_False) {
        gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        self->repeat_z = false;
        return 0;
    } else {
        MGLError_Set("invalid value for texture_z");
        return -1;
    }
}

PyObject * MGLTexture3D_get_filter(MGLTexture3D * self) {
    PyObject * res = PyTuple_New(2);
    PyTuple_SET_ITEM(res, 0, PyLong_FromLong(self->min_filter));
    PyTuple_SET_ITEM(res, 1, PyLong_FromLong(self->mag_filter));
    return res;
}

int MGLTexture3D_set_filter(MGLTexture3D * self, PyObject * value) {
    if (PyTuple_GET_SIZE(value) != 2) {
        MGLError_Set("invalid filter");
        return -1;
    }

    self->min_filter = PyLong_AsLong(PyTuple_GET_ITEM(value, 0));
    self->mag_filter = PyLong_AsLong(PyTuple_GET_ITEM(value, 1));

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);
    gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, self->min_filter);
    gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, self->mag_filter);

    return 0;
}

PyObject * MGLTexture3D_get_swizzle(MGLTexture3D * self) {
    return get_swizzle(self->context, GL_TEXTURE_3D, self->texture_obj, false);
}

int MGLTexture3D_set_swizzle(MGLTexture3D * self, PyObject * value) {
    return set_swizzle(self->context, GL_TEXTURE_3D, self->texture_obj, false, value);
}

PyObject * MGLContext_texture_array(MGLContext * self, PyObject * args) {
    int width;
    int height;
    int layers;

    int components;

    PyObject * data;

    int alignment;

    const char * dtype;
    Py_ssize_t dtype_size;

    int args_ok = PyArg_ParseTuple(args, "(III)IOIs#", &width, &height, &layers, &components, &data, &alignment, &dtype, &dtype_size);
    if (!args_ok) {
        return 0;
    }

    if (components < 1 || components > 4) {
        MGLError_Set("the components must be 1, 2, 3 or 4");
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    MGLDataType * data_type = from_dtype(dtype, dtype_size);

    if (!data_type) {
        MGLError_Set("invalid dtype");
        return 0;
    }

    int expected_size = width * components * data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * height * layers;

    Py_buffer buffer_view;

    if (data != Py_None) {
        int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
        if (get_buffer < 0) {
            // Propagate the default error
            return 0;
        }
    } else {
        buffer_view.len = expected_size;
        buffer_view.buf = 0;
    }

    if (buffer_view.len != expected_size) {
        MGLError_Set("data size mismatch %d != %d", buffer_view.len, expected_size);
        if (data != Py_None) {
            PyBuffer_Release(&buffer_view);
        }
        return 0;
    }

    int pixel_type = data_type->gl_type;
    int base_format = data_type->base_format[components];
    int internal_format = data_type->internal_format[components];

    const GLMethods & gl = self->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);

    MGLTextureArray * texture = PyObject_New(MGLTextureArray, MGLTextureArray_type);
    texture->released = false;

    texture->texture_obj = 0;
    gl.GenTextures(1, (GLuint *)&texture->texture_obj);

    if (!texture->texture_obj) {
        MGLError_Set("cannot create texture");
        Py_DECREF(texture);
        return 0;
    }

    gl.BindTexture(GL_TEXTURE_2D_ARRAY, texture->texture_obj);

    gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
    gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
    gl.TexImage3D(GL_TEXTURE_2D_ARRAY, 0, internal_format, width, height, layers, 0, base_format, pixel_type, buffer_view.buf);
    if (data_type->float_type) {
        gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    if (data != Py_None) {
        PyBuffer_Release(&buffer_view);
    }

    texture->width = width;
    texture->height = height;
    texture->layers = layers;
    texture->components = components;
    texture->data_type = data_type;

    texture->min_filter = data_type->float_type ? GL_LINEAR : GL_NEAREST;
    texture->mag_filter = data_type->float_type ? GL_LINEAR : GL_NEAREST;

    texture->repeat_x = true;
    texture->repeat_y = true;
    texture->anisotropy = 1.0;

    Py_INCREF(self);
    texture->context = self;

    Py_INCREF(texture);

    PyObject * result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, (PyObject *)texture);
    PyTuple_SET_ITEM(result, 1, PyLong_FromLong(texture->texture_obj));
    return result;
}

PyObject * MGLTextureArray_read(MGLTextureArray * self, PyObject * args) {
    int alignment;

    int args_ok = PyArg_ParseTuple(args, "I", &alignment);
    if (!args_ok) {
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    int expected_size = self->width * self->components * self->data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * self->height * self->layers;

    PyObject * result = PyBytes_FromStringAndSize(0, expected_size);
    char * data = PyBytes_AS_STRING(result);

    int pixel_type = self->data_type->gl_type;
    int base_format = self->data_type->base_format[self->components];

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_2D_ARRAY, self->texture_obj);

    gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
    gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);

    // To determine the required size of pixels, use glGetTexLevelParameter to determine
    // the dimensions of the internal texture image, then scale the required number of pixels
    // by the storage required for each pixel, based on format and type. Be sure to take the
    // pixel storage parameters into account, especially GL_PACK_ALIGNMENT.

    // int pack = 0;
    // gl.GetIntegerv(GL_PACK_ALIGNMENT, &pack);
    // printf("GL_PACK_ALIGNMENT: %d\n", pack);

    // glGetTexLevelParameter with argument GL_TEXTURE_WIDTH
    // glGetTexLevelParameter with argument GL_TEXTURE_HEIGHT
    // glGetTexLevelParameter with argument GL_TEXTURE_INTERNAL_FORMAT

    // int level_width = 0;
    // int level_height = 0;
    // gl.GetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_WIDTH, &level_width);
    // gl.GetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_HEIGHT, &level_height);
    // printf("level_width: %d\n", level_width);
    // printf("level_height: %d\n", level_height);

    gl.GetTexImage(GL_TEXTURE_2D_ARRAY, 0, base_format, pixel_type, data);

    return result;
}

PyObject * MGLTextureArray_read_into(MGLTextureArray * self, PyObject * args) {
    PyObject * data;
    int alignment;
    Py_ssize_t write_offset;

    int args_ok = PyArg_ParseTuple(args, "OIn", &data, &alignment, &write_offset);
    if (!args_ok) {
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    int expected_size = self->width * self->components * self->data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * self->height * self->layers;

    int pixel_type = self->data_type->gl_type;
    int format = self->data_type->base_format[self->components];

    if (Py_TYPE(data) == MGLBuffer_type) {

        MGLBuffer * buffer = (MGLBuffer *)data;

        const GLMethods & gl = self->context->gl;

        gl.BindBuffer(GL_PIXEL_PACK_BUFFER, buffer->buffer_obj);
        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(GL_TEXTURE_2D_ARRAY, self->texture_obj);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.GetTexImage(GL_TEXTURE_2D_ARRAY, 0, format, pixel_type, (void *)write_offset);
        gl.BindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    } else {

        Py_buffer buffer_view;

        int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_WRITABLE);
        if (get_buffer < 0) {
            // Propagate the default error
            return 0;
        }

        if (buffer_view.len < write_offset + expected_size) {
            MGLError_Set("the buffer is too small");
            PyBuffer_Release(&buffer_view);
            return 0;
        }

        char * ptr = (char *)buffer_view.buf + write_offset;

        const GLMethods & gl = self->context->gl;

        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(GL_TEXTURE_2D_ARRAY, self->texture_obj);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.GetTexImage(GL_TEXTURE_2D_ARRAY, 0, format, pixel_type, ptr);

        PyBuffer_Release(&buffer_view);

    }

    Py_RETURN_NONE;
}

PyObject * MGLTextureArray_write(MGLTextureArray * self, PyObject * args) {
    PyObject * data;
    PyObject * viewport;
    int alignment;

    int args_ok = PyArg_ParseTuple(args, "OOI", &data, &viewport, &alignment);
    if (!args_ok) {
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    int x = 0;
    int y = 0;
    int z = 0;
    int width = self->width;
    int height = self->height;
    int layers = self->layers;

    Py_buffer buffer_view;

    if (viewport != Py_None) {
        if (Py_TYPE(viewport) != &PyTuple_Type) {
            MGLError_Set("the viewport must be a tuple not %s", Py_TYPE(viewport)->tp_name);
            return 0;
        }

        if (PyTuple_GET_SIZE(viewport) == 6) {

            x = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 0));
            y = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 1));
            z = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 2));
            width = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 3));
            height = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 4));
            layers = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 5));

        } else if (PyTuple_GET_SIZE(viewport) == 3) {

            width = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 0));
            height = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 1));
            layers = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 2));

        } else {

            MGLError_Set("the viewport size %d is invalid", PyTuple_GET_SIZE(viewport));
            return 0;

        }

        if (PyErr_Occurred()) {
            MGLError_Set("wrong values in the viewport");
            return 0;
        }

    }

    int expected_size = width * self->components * self->data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * height * layers;

    int pixel_type = self->data_type->gl_type;
    int format = self->data_type->base_format[self->components];

    if (Py_TYPE(data) == MGLBuffer_type) {

        MGLBuffer * buffer = (MGLBuffer *)data;

        const GLMethods & gl = self->context->gl;

        gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer->buffer_obj);
        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(GL_TEXTURE_2D_ARRAY, self->texture_obj);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.TexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, x, y, z, width, height, layers, format, pixel_type, 0);
        gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    } else {

        int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
        if (get_buffer < 0) {
            // Propagate the default error
            return 0;
        }

        if (buffer_view.len != expected_size) {
            MGLError_Set("data size mismatch %d != %d", buffer_view.len, expected_size);
            if (data != Py_None) {
                PyBuffer_Release(&buffer_view);
            }
            return 0;
        }

        const GLMethods & gl = self->context->gl;

        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(GL_TEXTURE_2D_ARRAY, self->texture_obj);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.TexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, x, y, z, width, height, layers, format, pixel_type, buffer_view.buf);

        PyBuffer_Release(&buffer_view);

    }

    Py_RETURN_NONE;
}

PyObject * MGLTextureArray_meth_bind(MGLTextureArray * self, PyObject * args) {
    int unit;
    int read;
    int write;
    int level;
    int format;

    int args_ok = PyArg_ParseTuple(args, "IppII", &unit, &read, &write, &level, &format);
    if (!args_ok) {
        return NULL;
    }

    int access = GL_READ_WRITE;
    if (read && !write) access = GL_READ_ONLY;
    else if (!read && write) access = GL_WRITE_ONLY;
    else if (!read && !write) {
        MGLError_Set("Illegal access mode. Read or write needs to be enabled.");
        return NULL;
    }

    int frmt = format ? format : self->data_type->internal_format[self->components];

    const GLMethods & gl = self->context->gl;
    // NOTE: Texture array must be bound as layered to expose all layers
    gl.BindImageTexture(unit, self->texture_obj, level, GL_TRUE, 0, access, frmt);
    Py_RETURN_NONE;
}

PyObject * MGLTextureArray_use(MGLTextureArray * self, PyObject * args) {
    int index;

    int args_ok = PyArg_ParseTuple(args, "I", &index);
    if (!args_ok) {
        return 0;
    }

    const GLMethods & gl = self->context->gl;
    gl.ActiveTexture(GL_TEXTURE0 + index);
    gl.BindTexture(GL_TEXTURE_2D_ARRAY, self->texture_obj);

    Py_RETURN_NONE;
}

PyObject * MGLTextureArray_build_mipmaps(MGLTextureArray * self, PyObject * args) {
    int base = 0;
    int max = 1000;

    int args_ok = PyArg_ParseTuple(args, "II", &base, &max);
    if (!args_ok) {
        return 0;
    }

    if (base > self->max_level) {
        MGLError_Set("invalid base");
        return 0;
    }

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_2D_ARRAY, self->texture_obj);

    gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, base);
    gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, max);

    gl.GenerateMipmap(GL_TEXTURE_2D_ARRAY);

    gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    self->min_filter = GL_LINEAR_MIPMAP_LINEAR;
    self->mag_filter = GL_LINEAR;
    self->max_level = max;

    Py_RETURN_NONE;
}

PyObject * MGLTextureArray_release(MGLTextureArray * self) {
    if (self->released) {
        Py_RETURN_NONE;
    }
    self->released = true;

    const GLMethods & gl = self->context->gl;
    gl.DeleteTextures(1, (GLuint *)&self->texture_obj);

    Py_DECREF(self->context);
    Py_DECREF(self);
    Py_RETURN_NONE;
}

PyObject * MGLTextureArray_get_repeat_x(MGLTextureArray * self) {
    return PyBool_FromLong(self->repeat_x);
}

int MGLTextureArray_set_repeat_x(MGLTextureArray * self, PyObject * value) {

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_2D_ARRAY, self->texture_obj);

    if (value == Py_True) {
        gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        self->repeat_x = true;
        return 0;
    } else if (value == Py_False) {
        gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        self->repeat_x = false;
        return 0;
    } else {
        MGLError_Set("invalid value for texture_x");
        return -1;
    }
}

PyObject * MGLTextureArray_get_repeat_y(MGLTextureArray * self) {
    return PyBool_FromLong(self->repeat_y);
}

int MGLTextureArray_set_repeat_y(MGLTextureArray * self, PyObject * value) {

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_2D_ARRAY, self->texture_obj);

    if (value == Py_True) {
        gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
        self->repeat_y = true;
        return 0;
    } else if (value == Py_False) {
        gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        self->repeat_y = false;
        return 0;
    } else {
        MGLError_Set("invalid value for texture_y");
        return -1;
    }
}

PyObject * MGLTextureArray_get_filter(MGLTextureArray * self) {
    PyObject * res = PyTuple_New(2);
    PyTuple_SET_ITEM(res, 0, PyLong_FromLong(self->min_filter));
    PyTuple_SET_ITEM(res, 1, PyLong_FromLong(self->mag_filter));
    return res;
}

int MGLTextureArray_set_filter(MGLTextureArray * self, PyObject * value) {
    if (PyTuple_GET_SIZE(value) != 2) {
        MGLError_Set("invalid filter");
        return -1;
    }

    self->min_filter = PyLong_AsLong(PyTuple_GET_ITEM(value, 0));
    self->mag_filter = PyLong_AsLong(PyTuple_GET_ITEM(value, 1));


    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_2D_ARRAY, self->texture_obj);
    gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, self->min_filter);
    gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, self->mag_filter);

    return 0;
}

PyObject * MGLTextureArray_get_swizzle(MGLTextureArray * self) {
    return get_swizzle(self->context, GL_TEXTURE_2D_ARRAY, self->texture_obj, false);
}

int MGLTextureArray_set_swizzle(MGLTextureArray * self, PyObject * value) {
    return set_swizzle(self->context, GL_TEXTURE_2D_ARRAY, self->texture_obj, false, value);
}

PyObject * MGLTextureArray_get_anisotropy(MGLTextureArray * self) {
    return PyFloat_FromDouble(self->anisotropy);
}

int MGLTextureArray_set_anisotropy(MGLTextureArray * self, PyObject * value) {
    self->anisotropy = (float)MGL_min(MGL_max(PyFloat_AsDouble(value), 1.0), self->context->max_anisotropy);

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_2D_ARRAY, self->texture_obj);
    gl.TexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY, self->anisotropy);

    return 0;
}

PyObject * MGLContext_texture_cube(MGLContext * self, PyObject * args) {
    int width;
    int height;

    int components;

    PyObject * data;

    int alignment;

    const char * dtype;
    Py_ssize_t dtype_size;
    int internal_format_override;

    int args_ok = PyArg_ParseTuple(
        args,
        "(II)IOIs#I",
        &width,
        &height,
        &components,
        &data,
        &alignment,
        &dtype,
        &dtype_size,
        &internal_format_override
    );

    if (!args_ok) {
        return 0;
    }

    if (components < 1 || components > 4) {
        MGLError_Set("the components must be 1, 2, 3 or 4");
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    MGLDataType * data_type = from_dtype(dtype, dtype_size);

    if (!data_type) {
        MGLError_Set("invalid dtype");
        return 0;
    }

    int expected_size = width * components * data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * height * 6;

    Py_buffer buffer_view;

    if (data != Py_None) {
        int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
        if (get_buffer < 0) {
            // Propagate the default error
            return 0;
        }
    } else {
        buffer_view.len = expected_size;
        buffer_view.buf = 0;
    }

    if (buffer_view.len != expected_size) {
        MGLError_Set("data size mismatch %d != %d", buffer_view.len, expected_size);
        if (data != Py_None) {
            PyBuffer_Release(&buffer_view);
        }
        return 0;
    }

    int pixel_type = data_type->gl_type;
    int base_format = data_type->base_format[components];
    int internal_format = internal_format_override ? internal_format_override : data_type->internal_format[components];

    const GLMethods & gl = self->gl;

    MGLTextureCube * texture = PyObject_New(MGLTextureCube, MGLTextureCube_type);
    texture->released = false;

    texture->texture_obj = 0;
    gl.GenTextures(1, (GLuint *)&texture->texture_obj);

    if (!texture->texture_obj) {
        MGLError_Set("cannot create texture");
        Py_DECREF(texture);
        return 0;
    }

    gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_CUBE_MAP, texture->texture_obj);

    if (data == Py_None) {
        expected_size = 0;
    }

    const char * ptr[6] = {
        (const char *)buffer_view.buf + expected_size * 0 / 6,
        (const char *)buffer_view.buf + expected_size * 1 / 6,
        (const char *)buffer_view.buf + expected_size * 2 / 6,
        (const char *)buffer_view.buf + expected_size * 3 / 6,
        (const char *)buffer_view.buf + expected_size * 4 / 6,
        (const char *)buffer_view.buf + expected_size * 5 / 6,
    };

    gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
    gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
    gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internal_format, width, height, 0, base_format, pixel_type, ptr[0]);
    gl.TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internal_format, width, height, 0, base_format, pixel_type, ptr[1]);
    gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internal_format, width, height, 0, base_format, pixel_type, ptr[2]);
    gl.TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internal_format, width, height, 0, base_format, pixel_type, ptr[3]);
    gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internal_format, width, height, 0, base_format, pixel_type, ptr[4]);
    gl.TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internal_format, width, height, 0, base_format, pixel_type, ptr[5]);
    if (data_type->float_type) {
        gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    if (data != Py_None) {
        PyBuffer_Release(&buffer_view);
    }

    texture->width = width;
    texture->height = height;
    texture->components = components;
    texture->data_type = data_type;

    texture->min_filter = data_type->float_type ? GL_LINEAR : GL_NEAREST;
    texture->mag_filter = data_type->float_type ? GL_LINEAR : GL_NEAREST;
    texture->max_level = 0;
    texture->anisotropy = 1.0;

    Py_INCREF(self);
    texture->context = self;

    Py_INCREF(texture);

    PyObject * result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, (PyObject *)texture);
    PyTuple_SET_ITEM(result, 1, PyLong_FromLong(texture->texture_obj));
    return result;
}

PyObject * MGLTextureCube_read(MGLTextureCube * self, PyObject * args) {
    int face;
    int alignment;

    int args_ok = PyArg_ParseTuple(args, "iI", &face, &alignment);
    if (!args_ok) {
        return 0;
    }

    if (face < 0 || face > 5) {
        MGLError_Set("the face must be 0, 1, 2, 3, 4 or 5");
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    int expected_size = self->width * self->components * self->data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * self->height;

    PyObject * result = PyBytes_FromStringAndSize(0, expected_size);
    char * data = PyBytes_AS_STRING(result);

    int pixel_type = self->data_type->gl_type;
    int format = self->data_type->base_format[self->components];

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);

    gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
    gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
    gl.GetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, format, pixel_type, data);

    return result;
}

PyObject * MGLTextureCube_read_into(MGLTextureCube * self, PyObject * args) {
    PyObject * data;
    int face;
    int alignment;
    Py_ssize_t write_offset;

    int args_ok = PyArg_ParseTuple(args, "OiIn", &data, &face, &alignment, &write_offset);
    if (!args_ok) {
        return 0;
    }

    if (face < 0 || face > 5) {
        MGLError_Set("the face must be 0, 1, 2, 3, 4 or 5");
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    int expected_size = self->width * self->components * self->data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * self->height;

    int pixel_type = self->data_type->gl_type;
    int format = self->data_type->base_format[self->components];

    if (Py_TYPE(data) == MGLBuffer_type) {

        MGLBuffer * buffer = (MGLBuffer *)data;

        const GLMethods & gl = self->context->gl;

        gl.BindBuffer(GL_PIXEL_PACK_BUFFER, buffer->buffer_obj);
        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.GetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, format, pixel_type, (char *)write_offset);
        gl.BindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    } else {

        Py_buffer buffer_view;

        int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_WRITABLE);
        if (get_buffer < 0) {
            // Propagate the default error
            return 0;
        }

        if (buffer_view.len < write_offset + expected_size) {
            MGLError_Set("the buffer is too small");
            PyBuffer_Release(&buffer_view);
            return 0;
        }

        char * ptr = (char *)buffer_view.buf + write_offset;

        const GLMethods & gl = self->context->gl;
        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.GetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, format, pixel_type, ptr);

        PyBuffer_Release(&buffer_view);

    }

    Py_RETURN_NONE;
}

PyObject * MGLTextureCube_write(MGLTextureCube * self, PyObject * args) {
    int face;
    PyObject * data;
    PyObject * viewport;
    int alignment;

    int args_ok = PyArg_ParseTuple(args, "iOOI", &face, &data, &viewport, &alignment);
    if (!args_ok) {
        return 0;
    }

    if (face < 0 || face > 5) {
        MGLError_Set("the face must be 0, 1, 2, 3, 4 or 5");
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    int x = 0;
    int y = 0;
    int width = self->width;
    int height = self->height;

    Py_buffer buffer_view;

    if (viewport != Py_None) {
        if (Py_TYPE(viewport) != &PyTuple_Type) {
            MGLError_Set("the viewport must be a tuple not %s", Py_TYPE(viewport)->tp_name);
            return 0;
        }

        if (PyTuple_GET_SIZE(viewport) == 4) {

            x = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 0));
            y = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 1));
            width = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 2));
            height = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 3));

        } else if (PyTuple_GET_SIZE(viewport) == 2) {

            width = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 0));
            height = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 1));

        } else {

            MGLError_Set("the viewport size %d is invalid", PyTuple_GET_SIZE(viewport));
            return 0;

        }

        if (PyErr_Occurred()) {
            MGLError_Set("wrong values in the viewport");
            return 0;
        }

    }

    int expected_size = width * self->components * self->data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * height;

    // GL_TEXTURE_CUBE_MAP_POSITIVE_X = GL_TEXTURE_CUBE_MAP_POSITIVE_X + 0
    // GL_TEXTURE_CUBE_MAP_NEGATIVE_X = GL_TEXTURE_CUBE_MAP_POSITIVE_X + 1
    // GL_TEXTURE_CUBE_MAP_POSITIVE_Y = GL_TEXTURE_CUBE_MAP_POSITIVE_X + 2
    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y = GL_TEXTURE_CUBE_MAP_POSITIVE_X + 3
    // GL_TEXTURE_CUBE_MAP_POSITIVE_Z = GL_TEXTURE_CUBE_MAP_POSITIVE_X + 4
    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z = GL_TEXTURE_CUBE_MAP_POSITIVE_X + 5

    int pixel_type = self->data_type->gl_type;
    int format = self->data_type->base_format[self->components];

    if (Py_TYPE(data) == MGLBuffer_type) {

        MGLBuffer * buffer = (MGLBuffer *)data;

        const GLMethods & gl = self->context->gl;

        gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer->buffer_obj);
        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.TexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, x, y, width, height, format, pixel_type, 0);
        gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    } else {

        int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
        if (get_buffer < 0) {
        // Propagate the default error
            return 0;
        }

        if (buffer_view.len != expected_size) {
            MGLError_Set("data size mismatch %d != %d", buffer_view.len, expected_size);
            PyBuffer_Release(&buffer_view);
            return 0;
        }

        const GLMethods & gl = self->context->gl;

        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);

        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.TexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, x, y, width, height, format, pixel_type, buffer_view.buf);

        PyBuffer_Release(&buffer_view);
    }

    Py_RETURN_NONE;
}

PyObject * MGLTextureCube_meth_bind(MGLTextureCube * self, PyObject * args) {
    int unit;
    int read;
    int write;
    int level;
    int format;

    int args_ok = PyArg_ParseTuple(args, "IppII", &unit, &read, &write, &level, &format);
    if (!args_ok) {
        return NULL;
    }

    int access = GL_READ_WRITE;
    if (read && !write) access = GL_READ_ONLY;
    else if (!read && write) access = GL_WRITE_ONLY;
    else if (!read && !write) {
        MGLError_Set("Illegal access mode. Read or write needs to be enabled.");
        return NULL;
    }

    int frmt = format ? format : self->data_type->internal_format[self->components];

    const GLMethods & gl = self->context->gl;
    // NOTE: Texture cube must be bound as layered to expose all layers
    gl.BindImageTexture(unit, self->texture_obj, level, GL_TRUE, 0, access, frmt);
    Py_RETURN_NONE;
}

PyObject * MGLTextureCube_use(MGLTextureCube * self, PyObject * args) {
    int index;

    int args_ok = PyArg_ParseTuple(args, "I", &index);
    if (!args_ok) {
        return 0;
    }

    const GLMethods & gl = self->context->gl;
    gl.ActiveTexture(GL_TEXTURE0 + index);
    gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);

    Py_RETURN_NONE;
}

PyObject * MGLTextureCube_release(MGLTextureCube * self) {
    if (self->released) {
        Py_RETURN_NONE;
    }
    self->released = true;

    // TODO: decref

    const GLMethods & gl = self->context->gl;
    gl.DeleteTextures(1, (GLuint *)&self->texture_obj);

    Py_DECREF(self);
    Py_RETURN_NONE;
}

PyObject * MGLTextureCube_get_filter(MGLTextureCube * self) {
    PyObject * res = PyTuple_New(2);
    PyTuple_SET_ITEM(res, 0, PyLong_FromLong(self->min_filter));
    PyTuple_SET_ITEM(res, 1, PyLong_FromLong(self->mag_filter));
    return res;
}

int MGLTextureCube_set_filter(MGLTextureCube * self, PyObject * value) {
    if (PyTuple_GET_SIZE(value) != 2) {
        MGLError_Set("invalid filter");
        return -1;
    }

    self->min_filter = PyLong_AsLong(PyTuple_GET_ITEM(value, 0));
    self->mag_filter = PyLong_AsLong(PyTuple_GET_ITEM(value, 1));

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);
    gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, self->min_filter);
    gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, self->mag_filter);

    return 0;
}

PyObject * MGLTextureCube_get_swizzle(MGLTextureCube * self) {
    return get_swizzle(self->context, GL_TEXTURE_CUBE_MAP, self->texture_obj, false);
}

int MGLTextureCube_set_swizzle(MGLTextureCube * self, PyObject * value) {
    return set_swizzle(self->context, GL_TEXTURE_CUBE_MAP, self->texture_obj, false, value);
}

PyObject * MGLTextureCube_get_anisotropy(MGLTextureCube * self) {
    return PyFloat_FromDouble(self->anisotropy);
}

int MGLTextureCube_set_anisotropy(MGLTextureCube * self, PyObject * value) {
    self->anisotropy = (float)MGL_min(MGL_max(PyFloat_AsDouble(value), 1.0), self->context->max_anisotropy);

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);
    gl.TexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, self->anisotropy);

    return 0;
}

PyObject * MGLContext_vertex_array(MGLContext * self, PyObject * args) {
    MGLProgram * program;
    PyObject * content;
    MGLBuffer * index_buffer;
    int index_element_size;
    int skip_errors;

    int args_ok = PyArg_ParseTuple(args, "O!OOIp", MGLProgram_type, &program, &content, &index_buffer, &index_element_size, &skip_errors);
    if (!args_ok) {
        return 0;
    }

    if (program->context != self) {
        MGLError_Set("the program belongs to a different context");
        return 0;
    }

    if (index_buffer != (MGLBuffer *)Py_None && index_buffer->context != self) {
        MGLError_Set("the index_buffer belongs to a different context");
        return 0;
    }

    int content_len = (int)PyTuple_GET_SIZE(content);

    // Allow empty vertextbuffers: https://github.com/moderngl/moderngl/issues/321
    // if (!content_len) {
    // 	MGLError_Set("the content must not be emtpy");
    // 	return 0;
    // }

    for (int i = 0; i < content_len; ++i) {
        PyObject * tuple = PyTuple_GET_ITEM(content, i);
        PyObject * buffer = PyTuple_GET_ITEM(tuple, 0);
        PyObject * format = PyTuple_GET_ITEM(tuple, 1);

        if (Py_TYPE(buffer) != MGLBuffer_type) {
            MGLError_Set("content[%d][0] must be a Buffer not %s", i, Py_TYPE(buffer)->tp_name);
            return 0;
        }

        if (Py_TYPE(format) != &PyUnicode_Type) {
            MGLError_Set("content[%d][1] must be a string not %s", i, Py_TYPE(format)->tp_name);
            return 0;
        }

        if (((MGLBuffer *)buffer)->context != self) {
            MGLError_Set("content[%d][0] belongs to a different context", i);
            return 0;
        }

        FormatIterator it = FormatIterator(PyUnicode_AsUTF8(format));
        FormatInfo format_info = it.info();

        if (!format_info.valid) {
            MGLError_Set("content[%d][1] is an invalid format", i);
            return 0;
        }

        int attributes_len = (int)PyTuple_GET_SIZE(tuple) - 2;

        if (!attributes_len) {
            MGLError_Set("content[%d][2] must not be empty", i);
            return 0;
        }

        if (attributes_len != format_info.nodes) {
            MGLError_Set("content[%d][1] and content[%d][2] size mismatch %d != %d", i, i, format_info.nodes, attributes_len);
            return 0;
        }
    }

    if (index_buffer != (MGLBuffer *)Py_None && Py_TYPE(index_buffer) != MGLBuffer_type) {
        MGLError_Set("the index_buffer must be a Buffer not %s", Py_TYPE(index_buffer)->tp_name);
        return 0;
    }

    if (index_element_size != 1 && index_element_size != 2 && index_element_size != 4) {
        MGLError_Set("index_element_size must be 1, 2, or 4, not %d", index_element_size);
        return 0;
    }

    const GLMethods & gl = self->gl;

    MGLVertexArray * array = PyObject_New(MGLVertexArray, MGLVertexArray_type);
    array->released = false;

    array->num_vertices = 0;
    array->num_instances = 1;

    Py_INCREF(program);
    array->program = program;

    array->vertex_array_obj = 0;
    gl.GenVertexArrays(1, (GLuint *)&array->vertex_array_obj);

    if (!array->vertex_array_obj) {
        MGLError_Set("cannot create vertex array");
        Py_DECREF(array);
        return 0;
    }

    gl.BindVertexArray(array->vertex_array_obj);

    Py_INCREF(index_buffer);
    array->index_buffer = index_buffer;
    array->index_element_size = index_element_size;

    const int element_types[5] = {0, GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, 0, GL_UNSIGNED_INT};
    array->index_element_type = element_types[index_element_size];

    if (index_buffer != (MGLBuffer *)Py_None) {
        array->num_vertices = (int)(index_buffer->size / index_element_size);
        gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->buffer_obj);
    } else {
        array->num_vertices = -1;
    }

    for (int i = 0; i < content_len; ++i) {
        PyObject * tuple = PyTuple_GET_ITEM(content, i);

        MGLBuffer * buffer = (MGLBuffer *)PyTuple_GET_ITEM(tuple, 0);
        const char * format = PyUnicode_AsUTF8(PyTuple_GET_ITEM(tuple, 1));

        FormatIterator it = FormatIterator(format);
        FormatInfo format_info = it.info();

        int buf_vertices = (int)(buffer->size / format_info.size);

        if (!format_info.divisor && array->index_buffer == (MGLBuffer *)Py_None && (!i || array->num_vertices > buf_vertices)) {
            array->num_vertices = buf_vertices;
        }

        gl.BindBuffer(GL_ARRAY_BUFFER, buffer->buffer_obj);

        char * ptr = 0;

        int attributes_len = (int)PyTuple_GET_SIZE(tuple) - 2;

        for (int j = 0; j < attributes_len; ++j) {
            FormatNode * node = it.next();

            while (!node->type) {
                ptr += node->size;
                node = it.next();
            }

            PyObject * attribute = PyTuple_GET_ITEM(tuple, j + 2);

            if (attribute == Py_None) {
                ptr += node->size;
                continue;
            }

            PyObject * attribute_location_py = PyObject_GetAttrString(attribute, "_location");
            PyObject * attribute_rows_length_py = PyObject_GetAttrString(attribute, "_rows_length");
            PyObject * attribute_scalar_type_py = PyObject_GetAttrString(attribute, "_scalar_type");
            if (!attribute_location_py || !attribute_rows_length_py || !attribute_scalar_type_py) {
                return NULL;
            }

            int attribute_location = PyLong_AsLong(attribute_location_py);
            int attribute_rows_length = PyLong_AsLong(attribute_rows_length_py);
            int attribute_scalar_type = PyLong_AsLong(attribute_scalar_type_py);

            for (int r = 0; r < attribute_rows_length; ++r) {
                int location = attribute_location + r;
                int count = node->count / attribute_rows_length;

                switch (attribute_scalar_type) {
                    case GL_FLOAT: gl.VertexAttribPointer(location, count, node->type, node->normalize, format_info.size, ptr); break;
                    case GL_DOUBLE: gl.VertexAttribLPointer(location, count, node->type, format_info.size, ptr); break;
                    case GL_INT: gl.VertexAttribIPointer(location, count, node->type, format_info.size, ptr); break;
                    case GL_UNSIGNED_INT: gl.VertexAttribIPointer(location, count, node->type, format_info.size, ptr); break;
                }

                gl.VertexAttribDivisor(location, format_info.divisor);

                gl.EnableVertexAttribArray(location);

                ptr += node->size / attribute_rows_length;
            }
        }
    }

    Py_INCREF(self);
    array->context = self;

    array->num_subroutines = 0;
    array->num_subroutines += array->program->num_vertex_shader_subroutines;
    array->num_subroutines += array->program->num_fragment_shader_subroutines;
    array->num_subroutines += array->program->num_geometry_shader_subroutines;
    array->num_subroutines += array->program->num_tess_evaluation_shader_subroutines;
    array->num_subroutines += array->program->num_tess_control_shader_subroutines;

    if (array->num_subroutines) {
        array->subroutines = new unsigned[array->num_subroutines];
    } else {
        array->subroutines = 0;
    }

    Py_INCREF(array);

    PyObject * result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, (PyObject *)array);
    PyTuple_SET_ITEM(result, 1, PyLong_FromLong(array->vertex_array_obj));
    return result;
}

inline void MGLVertexArray_SET_SUBROUTINES(MGLVertexArray * self, const GLMethods & gl);

PyObject * MGLVertexArray_render(MGLVertexArray * self, PyObject * args) {
    int mode;
    int vertices;
    int first;
    int instances;

    int args_ok = PyArg_ParseTuple(args, "IIII", &mode, &vertices, &first, &instances);
    if (!args_ok) {
        return 0;
    }

    if (vertices < 0) {
        if (self->num_vertices < 0) {
            MGLError_Set("cannot detect the number of vertices");
            return 0;
        }

        vertices = self->num_vertices;
    }

    if (instances < 0) {
        instances = self->num_instances;
    }

    const GLMethods & gl = self->context->gl;

    gl.UseProgram(self->program->program_obj);
    gl.BindVertexArray(self->vertex_array_obj);

    MGLVertexArray_SET_SUBROUTINES(self, gl);

    if (self->index_buffer != (MGLBuffer *)Py_None) {
        const void * ptr = (const void *)((GLintptr)first * self->index_element_size);
        gl.DrawElementsInstanced(mode, vertices, self->index_element_type, ptr, instances);
    } else {
        gl.DrawArraysInstanced(mode, first, vertices, instances);
    }

    Py_RETURN_NONE;
}

PyObject * MGLVertexArray_render_indirect(MGLVertexArray * self, PyObject * args) {
    MGLBuffer * buffer;
    int mode;
    int count;
    int first;

    int args_ok = PyArg_ParseTuple(args, "O!III", MGLBuffer_type, &buffer, &mode, &count, &first);
    if (!args_ok) {
        return 0;
    }

    if (count < 0) {
        count = (int)(buffer->size / 20 - first);
    }

    const GLMethods & gl = self->context->gl;

    gl.UseProgram(self->program->program_obj);
    gl.BindVertexArray(self->vertex_array_obj);
    gl.BindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer->buffer_obj);

    MGLVertexArray_SET_SUBROUTINES(self, gl);

    const void * ptr = (const void *)((GLintptr)first * 20);

    if (self->index_buffer != (MGLBuffer *)Py_None) {
        gl.MultiDrawElementsIndirect(mode, self->index_element_type, ptr, count, 20);
    } else {
        gl.MultiDrawArraysIndirect(mode, ptr, count, 20);
    }

    Py_RETURN_NONE;
}

PyObject * MGLVertexArray_transform(MGLVertexArray * self, PyObject * args) {
    PyObject * outputs;
    int mode;
    int vertices;
    int first;
    int instances;
    int buffer_offset;

    int args_ok = PyArg_ParseTuple(args, "O!IIIII", &PyList_Type, &outputs, &mode, &vertices, &first, &instances, &buffer_offset);
    if (!args_ok) {
        return 0;
    }

    if (!self->program->num_varyings) {
        MGLError_Set("the program has no varyings");
        return 0;
    }

    if (vertices < 0) {
        if (self->num_vertices < 0) {
            MGLError_Set("cannot detect the number of vertices");
            return 0;
        }

        vertices = self->num_vertices;
    }

    if (instances < 0) {
        instances = self->num_instances;
    }

    int output_mode = -1;

    // If a geo shader is present we need to sanity check the the rendering mode
    if (self->program->geometry_output > -1) {
        output_mode = self->program->geometry_output;

        // The rendering mode must match the input type in the geometry shader
        // points, lines, lines_adjacency, triangles, triangles_adjacency
        switch (self->program->geometry_input)
        {
        case GL_POINTS:
            if (mode != GL_POINTS) {
                MGLError_Set("Geometry shader expects POINTS as input. Change the transform mode.");
                return 0;
            }
            break;
        case GL_LINES:
            if(mode != GL_LINES && mode != GL_LINE_STRIP && mode != GL_LINE_LOOP && mode != GL_LINES_ADJACENCY) {
                MGLError_Set("Geometry shader expects LINES, LINE_STRIP, GL_LINE_LOOP or GL_LINES_ADJACENCY as input. Change the rendering mode.");
                return 0;
            }
            break;
        case GL_LINES_ADJACENCY:
            if(mode != GL_LINES_ADJACENCY && mode != GL_LINE_STRIP_ADJACENCY) {
                MGLError_Set("Geometry shader expects LINES_ADJACENCY or LINE_STRIP_ADJACENCY as input. Change the rendering mode.");
                return 0;
            }
            break;
        case GL_TRIANGLES:
            if(mode != GL_TRIANGLES && mode != GL_TRIANGLE_STRIP && mode != GL_TRIANGLE_FAN) {
                MGLError_Set("Geometry shader expects GL_TRIANGLES, GL_TRIANGLE_STRIP or GL_TRIANGLE_FAN as input. Change the rendering mode.");
                return 0;
            }
            break;
        case GL_TRIANGLES_ADJACENCY:
            if(mode != GL_TRIANGLES_ADJACENCY && mode != GL_TRIANGLE_STRIP_ADJACENCY) {
                MGLError_Set("Geometry shader expects GL_TRIANGLES_ADJACENCY or GL_TRIANGLE_STRIP_ADJACENCY as input. Change the rendering mode.");
                return 0;
            }
            break;
        default:
            MGLError_Set("Unexpected geometry shader input mode: %d", self->program->geometry_input);
            return 0;
            break;
        }
    } else {
        // If no geometry shader is present we need to determine the output mode by looking at the input
        switch (mode)
        {
        case GL_POINTS:
            output_mode = GL_POINTS;
            break;
        case GL_LINES:
        case GL_LINE_LOOP:
        case GL_LINE_STRIP:
        case GL_LINES_ADJACENCY:
        case GL_LINE_STRIP_ADJACENCY:
            output_mode = GL_LINES;
            break;
        case GL_TRIANGLES:
        case GL_TRIANGLE_STRIP:
        case GL_TRIANGLE_FAN:
        case GL_TRIANGLES_ADJACENCY:
        case GL_TRIANGLE_STRIP_ADJACENCY:
            output_mode = GL_TRIANGLES;
            break;
        default:
            MGLError_Set("Primitive mode not supported: %d", mode);
            return 0;
            break;
        }
    }

    const GLMethods & gl = self->context->gl;

    gl.UseProgram(self->program->program_obj);
    gl.BindVertexArray(self->vertex_array_obj);

    int num_outputs = (int)PyList_Size(outputs);
    for (int i = 0; i < num_outputs; ++i) {
        MGLBuffer * output = (MGLBuffer *)PyList_GET_ITEM(outputs, i);
        gl.BindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, i, output->buffer_obj, buffer_offset, output->size - buffer_offset);
    }

    gl.Enable(GL_RASTERIZER_DISCARD);
    gl.BeginTransformFeedback(output_mode);

    MGLVertexArray_SET_SUBROUTINES(self, gl);

    if (self->index_buffer != (MGLBuffer *)Py_None) {
        const void * ptr = (const void *)((GLintptr)first * self->index_element_size);
        gl.DrawElementsInstanced(mode, vertices, self->index_element_type, ptr, instances);
    } else {
        gl.DrawArraysInstanced(mode, first, vertices, instances);
    }

    gl.EndTransformFeedback();
    if (~self->context->enable_flags & MGL_RASTERIZER_DISCARD) {
        gl.Disable(GL_RASTERIZER_DISCARD);
    }
    gl.Flush();

    Py_RETURN_NONE;
}

PyObject * MGLVertexArray_bind(MGLVertexArray * self, PyObject * args) {
    int location;
    const char * type;
    MGLBuffer * buffer;
    const char * format;
    Py_ssize_t offset;
    int stride;
    int divisor;
    int normalize;

    int args_ok = PyArg_ParseTuple(args, "IsO!snIIp", &location, &type, MGLBuffer_type, &buffer, &format, &offset, &stride, &divisor, &normalize);
    if (!args_ok) {
        return 0;
    }

    FormatIterator it = FormatIterator(format);
    FormatInfo format_info = it.info();

    if (type[0] == 'f' && normalize) {
        MGLError_Set("invalid normalize");
        return 0;
    }

    if (!format_info.valid || format_info.divisor || format_info.nodes != 1) {
        MGLError_Set("invalid format");
        return 0;
    }

    FormatNode * node = it.next();

    if (!node->type) {
        MGLError_Set("invalid format");
        return 0;
    }

    char * ptr = (char *)offset;

    const GLMethods & gl = self->context->gl;

    gl.BindVertexArray(self->vertex_array_obj);
    gl.BindBuffer(GL_ARRAY_BUFFER, buffer->buffer_obj);

    switch (type[0]) {
        case 'f':
            gl.VertexAttribPointer(location, node->count, node->type, normalize, stride, ptr);
            break;
        case 'i':
            gl.VertexAttribIPointer(location, node->count, node->type, stride, ptr);
            break;
        case 'd':
            gl.VertexAttribLPointer(location, node->count, node->type, stride, ptr);
            break;
        default:
            MGLError_Set("invalid type");
            return 0;
    }

    gl.VertexAttribDivisor(location, divisor);

    gl.EnableVertexAttribArray(location);

    Py_RETURN_NONE;
}

PyObject * MGLVertexArray_release(MGLVertexArray * self) {
    if (self->released) {
        Py_RETURN_NONE;
    }
    self->released = true;

    const GLMethods & gl = self->context->gl;
    gl.DeleteVertexArrays(1, (GLuint *)&self->vertex_array_obj);

    Py_DECREF(self->program);
    Py_XDECREF(self->index_buffer);
    Py_DECREF(self);
    Py_RETURN_NONE;
}

int MGLVertexArray_set_index_buffer(MGLVertexArray * self, PyObject * value) {
    if (Py_TYPE(value) != MGLBuffer_type) {
        MGLError_Set("the index_buffer must be a Buffer not %s", Py_TYPE(value)->tp_name);
        return -1;
    }

    Py_INCREF(value);
    Py_DECREF(self->index_buffer);
    self->index_buffer = (MGLBuffer *)value;
    self->num_vertices = (int)(self->index_buffer->size / 4);

    return 0;
}

PyObject * MGLVertexArray_get_vertices(MGLVertexArray * self) {
    return PyLong_FromLong(self->num_vertices);
}

int MGLVertexArray_set_vertices(MGLVertexArray * self, PyObject * value) {
    int vertices = PyLong_AsUnsignedLong(value);

    if (PyErr_Occurred()) {
        MGLError_Set("invalid value for vertices");
        return -1;
    }

    self->num_vertices = vertices;

    return 0;
}

PyObject * MGLVertexArray_get_instances(MGLVertexArray * self) {
    return PyLong_FromLong(self->num_instances);
}

int MGLVertexArray_set_instances(MGLVertexArray * self, PyObject * value) {
    int instances = PyLong_AsUnsignedLong(value);

    if (PyErr_Occurred()) {
        MGLError_Set("invalid value for instances");
        return -1;
    }

    self->num_instances = instances;

    return 0;
}

int MGLVertexArray_set_subroutines(MGLVertexArray * self, PyObject * value) {
    if (PyTuple_GET_SIZE(value) != self->num_subroutines) {
        MGLError_Set("the number of subroutines is %d not %d", self->num_subroutines, PyTuple_GET_SIZE(value));
        return -1;
    }

    for (int i = 0; i < self->num_subroutines; ++i) {
        PyObject * obj = PyTuple_GET_ITEM(value, i);
        if (Py_TYPE(obj) == &PyLong_Type) {
            self->subroutines[i] = PyLong_AsUnsignedLong(obj);
        } else {
            PyObject * int_cast = PyNumber_Long(obj);
            if (!int_cast) {
                MGLError_Set("invalid values in subroutines");
                return -1;
            }
            self->subroutines[i] = PyLong_AsUnsignedLong(int_cast);
            Py_DECREF(int_cast);
        }
    }

    if (PyErr_Occurred()) {
        MGLError_Set("invalid values in subroutines");
        return -1;
    }

    return 0;
}

inline void MGLVertexArray_SET_SUBROUTINES(MGLVertexArray * self, const GLMethods & gl) {
        if (self->subroutines) {
        unsigned * subroutines = self->subroutines;

        if (self->program->num_vertex_shader_subroutines) {
            gl.UniformSubroutinesuiv(
                GL_VERTEX_SHADER,
                self->program->num_vertex_shader_subroutines,
                subroutines
            );
            subroutines += self->program->num_vertex_shader_subroutines;
        }

        if (self->program->num_fragment_shader_subroutines) {
            gl.UniformSubroutinesuiv(
                GL_FRAGMENT_SHADER,
                self->program->num_fragment_shader_subroutines,
                subroutines
            );
            subroutines += self->program->num_fragment_shader_subroutines;
        }

        if (self->program->num_geometry_shader_subroutines) {
            gl.UniformSubroutinesuiv(
                GL_GEOMETRY_SHADER,
                self->program->num_geometry_shader_subroutines,
                subroutines
            );
            subroutines += self->program->num_geometry_shader_subroutines;
        }

        if (self->program->num_tess_evaluation_shader_subroutines) {
            gl.UniformSubroutinesuiv(
                GL_TESS_EVALUATION_SHADER,
                self->program->num_tess_evaluation_shader_subroutines,
                subroutines
            );
            subroutines += self->program->num_tess_evaluation_shader_subroutines;
        }

        if (self->program->num_tess_control_shader_subroutines) {
            gl.UniformSubroutinesuiv(
                GL_TESS_CONTROL_SHADER,
                self->program->num_tess_control_shader_subroutines,
                subroutines
            );
        }
    }
}

PyObject * fmtdebug(PyObject * self, PyObject * args) {
    const char * str;

    if (!PyArg_ParseTuple(args, "s", &str)) {
        return 0;
    }

    FormatIterator it = FormatIterator(str);
    FormatInfo format_info = it.info();

    PyObject * nodes = PyList_New(0);

    if (format_info.valid) {
        while (FormatNode * node = it.next()) {
            PyObject * obj = PyTuple_New(4);
            PyTuple_SET_ITEM(obj, 0, PyLong_FromLong(node->size));
            PyTuple_SET_ITEM(obj, 1, PyLong_FromLong(node->count));
            PyTuple_SET_ITEM(obj, 2, PyLong_FromLong(node->type));
            PyTuple_SET_ITEM(obj, 3, PyBool_FromLong(node->normalize));
            PyList_Append(nodes, obj);
        }
    }

    PyObject * res = PyTuple_New(5);
    PyTuple_SET_ITEM(res, 0, PyLong_FromLong(format_info.size));
    PyTuple_SET_ITEM(res, 1, PyLong_FromLong(format_info.nodes));
    PyTuple_SET_ITEM(res, 2, PyLong_FromLong(format_info.divisor));
    PyTuple_SET_ITEM(res, 3, PyBool_FromLong(format_info.valid));
    PyTuple_SET_ITEM(res, 4, PyList_AsTuple(nodes));
    Py_DECREF(nodes);
    return res;
}

PyObject * create_context(PyObject * self, PyObject * args, PyObject * kwargs) {
    PyObject * backend;
    PyObject * backend_name = kwargs ? PyDict_GetItemString(kwargs, "backend") : NULL;

    PyObject * glcontext = PyImport_ImportModule("glcontext");
    if (!glcontext) {
        // Displayed to user: ModuleNotFoundError: No module named 'glcontext'
        return NULL;
    }

    // Use the specified backend
    if (backend_name) {
        backend = PyObject_CallMethod(glcontext, "get_backend_by_name", "O", backend_name);
        if (backend == Py_None || backend == NULL) {
            return NULL;
        }
    // Use default backend
    } else {
        backend = PyObject_CallMethod(glcontext, "default_backend", NULL);
        if (backend == Py_None || backend == NULL) {
            MGLError_Set("glcontext: Could not get a default backend");
            return NULL;
        }
    }

    MGLContext * ctx = PyObject_New(MGLContext, MGLContext_type);
    ctx->released = false;

    ctx->wireframe = false;

    // Ensure we have a callable
    if (!PyCallable_Check(backend)) {
        MGLError_Set("The returned glcontext is not a callable");
        return NULL;
    }
    // Create context by simply forwarding all arguments
    ctx->ctx = PyObject_Call(backend, args, kwargs);
    if (!ctx->ctx) {

        return NULL;
    }

    ctx->enter_func = PyObject_GetAttrString(ctx->ctx, "__enter__");
    if (!ctx->enter_func) {
        return NULL;
    }

    ctx->exit_func = PyObject_GetAttrString(ctx->ctx, "__exit__");
    if (!ctx->exit_func) {
        return NULL;
    }

    ctx->release_func = PyObject_GetAttrString(ctx->ctx, "release");
    if (!ctx->release_func) {
        return NULL;
    }

    // Map OpenGL functions
    void ** gl_function = (void **)&ctx->gl;
    for (int i = 0; GL_FUNCTIONS[i]; ++i) {
        PyObject * val = PyObject_CallMethod(ctx->ctx, "load", "s", GL_FUNCTIONS[i]);
        if (!val) {
            return NULL;
        }
        gl_function[i] = PyLong_AsVoidPtr(val);
        Py_DECREF(val);
    }

    const GLMethods & gl = ctx->gl;

    int major = 0;
    int minor = 0;

    gl.GetIntegerv(GL_MAJOR_VERSION, &major);
    gl.GetIntegerv(GL_MINOR_VERSION, &minor);

    ctx->version_code = major * 100 + minor * 10;

    // Load extensions
    int num_extensions = 0;
    gl.GetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
    ctx->extensions = PySet_New(NULL);

    for(int i = 0; i < num_extensions; i++) {
        const char * ext = (const char *)gl.GetStringi(GL_EXTENSIONS, i);
        PyObject * ext_name = PyUnicode_FromString(ext);
        PySet_Add(ctx->extensions, ext_name);
    }

    gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gl.Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    gl.Enable(GL_PRIMITIVE_RESTART);
    gl.PrimitiveRestartIndex(-1);

    ctx->max_samples = 0;
    gl.GetIntegerv(GL_MAX_SAMPLES, (GLint *)&ctx->max_samples);

    ctx->max_integer_samples = 0;
    gl.GetIntegerv(GL_MAX_INTEGER_SAMPLES, (GLint *)&ctx->max_integer_samples);

    ctx->max_color_attachments = 0;
    gl.GetIntegerv(GL_MAX_COLOR_ATTACHMENTS, (GLint *)&ctx->max_color_attachments);

    ctx->max_texture_units = 0;
    gl.GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint *)&ctx->max_texture_units);
    ctx->default_texture_unit = ctx->max_texture_units - 1;

    ctx->max_anisotropy = 0.0;
    gl.GetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, (GLfloat *)&ctx->max_anisotropy);

    int bound_framebuffer = 0;
    gl.GetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &bound_framebuffer);

    #ifdef __APPLE__
    if (PyObject_HasAttrString(ctx->ctx, "standalone") && PyObject_IsTrue(PyObject_GetAttrString(ctx->ctx, "standalone"))) {
        int renderbuffer = 0;
        gl.GenRenderbuffers(1, (GLuint *)&renderbuffer);
        gl.BindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
        gl.RenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, 4, 4);
        int framebuffer = 0;
        gl.GenFramebuffers(1, (GLuint *)&framebuffer);
        gl.BindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        gl.FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer);
        bound_framebuffer = framebuffer;
    }
    #endif

    {
        MGLFramebuffer * framebuffer = PyObject_New(MGLFramebuffer, MGLFramebuffer_type);
        framebuffer->released = false;

        framebuffer->framebuffer_obj = 0;

        framebuffer->draw_buffers_len = 1;
        framebuffer->draw_buffers = new unsigned[1];

        // According to glGet docs:
        // The initial value is GL_BACK if there are back buffers, otherwise it is GL_FRONT.

        // According to glDrawBuffer docs:
        // The symbolic constants GL_FRONT, GL_BACK, GL_LEFT, GL_RIGHT, and GL_FRONT_AND_BACK
        // are not allowed in the bufs array since they may refer to multiple buffers.

        // GL_COLOR_ATTACHMENT0 is causes error: 1282
        // This value is temporarily ignored

        // framebuffer->draw_buffers[0] = GL_COLOR_ATTACHMENT0;
        // framebuffer->draw_buffers[0] = GL_BACK_LEFT;

        gl.BindFramebuffer(GL_FRAMEBUFFER, 0);
        gl.GetIntegerv(GL_DRAW_BUFFER, (int *)&framebuffer->draw_buffers[0]);
        gl.BindFramebuffer(GL_FRAMEBUFFER, bound_framebuffer);

        framebuffer->color_mask = new bool[4];
        framebuffer->color_mask[0] = true;
        framebuffer->color_mask[1] = true;
        framebuffer->color_mask[2] = true;
        framebuffer->color_mask[3] = true;

        framebuffer->depth_mask = true;

        framebuffer->context = ctx;

        int scissor_box[4] = {};
        gl.GetIntegerv(GL_SCISSOR_BOX, scissor_box);

        framebuffer->viewport_x = scissor_box[0];
        framebuffer->viewport_y = scissor_box[1];
        framebuffer->viewport_width = scissor_box[2];
        framebuffer->viewport_height = scissor_box[3];

        framebuffer->scissor_enabled = false;
        framebuffer->scissor_x = scissor_box[0];
        framebuffer->scissor_y = scissor_box[1];
        framebuffer->scissor_width = scissor_box[2];
        framebuffer->scissor_height = scissor_box[3];

        framebuffer->width = scissor_box[2];
        framebuffer->height = scissor_box[3];
        framebuffer->dynamic = true;

        Py_INCREF(framebuffer);
        ctx->default_framebuffer = framebuffer;
    }

    Py_INCREF(ctx->default_framebuffer);
    ctx->bound_framebuffer = ctx->default_framebuffer;

    ctx->enable_flags = 0;
    ctx->front_face = GL_CCW;

    ctx->depth_func = GL_LEQUAL;
    ctx->blend_func_src = GL_SRC_ALPHA;
    ctx->blend_func_dst = GL_ONE_MINUS_SRC_ALPHA;

    ctx->wireframe = false;
    ctx->multisample = true;

    ctx->provoking_vertex = GL_LAST_VERTEX_CONVENTION;

    ctx->polygon_offset_factor = 0.0f;
    ctx->polygon_offset_units = 0.0f;

    gl.GetError(); // clear errors

    if (PyErr_Occurred()) {
        return 0;
    }

    Py_INCREF(ctx);

    PyObject * result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, (PyObject *)ctx);
    PyTuple_SET_ITEM(result, 1, PyLong_FromLong(ctx->version_code));
    return result;
}

void default_dealloc(PyObject * self) {
    Py_TYPE(self)->tp_free(self);
}

PyMethodDef module_methods[] = {
    {"create_context", (PyCFunction)create_context, METH_VARARGS | METH_KEYWORDS},
    {"fmtdebug", (PyCFunction)fmtdebug, METH_VARARGS},
    {},
};

PyMethodDef MGLBuffer_methods[] = {
    {"write", (PyCFunction)MGLBuffer_write, METH_VARARGS},
    {"read", (PyCFunction)MGLBuffer_read, METH_VARARGS},
    {"read_into", (PyCFunction)MGLBuffer_read_into, METH_VARARGS},
    {"write_chunks", (PyCFunction)MGLBuffer_write_chunks, METH_VARARGS},
    {"read_chunks", (PyCFunction)MGLBuffer_read_chunks, METH_VARARGS},
    {"read_chunks_into", (PyCFunction)MGLBuffer_read_chunks_into, METH_VARARGS},
    {"clear", (PyCFunction)MGLBuffer_clear, METH_VARARGS},
    {"orphan", (PyCFunction)MGLBuffer_orphan, METH_VARARGS},
    {"bind_to_uniform_block", (PyCFunction)MGLBuffer_bind_to_uniform_block, METH_VARARGS},
    {"bind_to_storage_buffer", (PyCFunction)MGLBuffer_bind_to_storage_buffer, METH_VARARGS},
    {"release", (PyCFunction)MGLBuffer_release, METH_NOARGS},
    {"size", (PyCFunction)MGLBuffer_size, METH_NOARGS},
    {},
};

PyMethodDef MGLComputeShader_methods[] = {
    {"run", (PyCFunction)MGLComputeShader_run, METH_VARARGS},
    {"release", (PyCFunction)MGLComputeShader_release, METH_VARARGS},
    {},
};

PyMethodDef MGLContext_methods[] = {
    {"enable_only", (PyCFunction)MGLContext_enable_only, METH_VARARGS},
    {"enable", (PyCFunction)MGLContext_enable, METH_VARARGS},
    {"disable", (PyCFunction)MGLContext_disable, METH_VARARGS},
    {"enable_direct", (PyCFunction)MGLContext_enable_direct, METH_VARARGS},
    {"disable_direct", (PyCFunction)MGLContext_disable_direct, METH_VARARGS},
    {"finish", (PyCFunction)MGLContext_finish, METH_NOARGS},
    {"copy_buffer", (PyCFunction)MGLContext_copy_buffer, METH_VARARGS},
    {"copy_framebuffer", (PyCFunction)MGLContext_copy_framebuffer, METH_VARARGS},
    {"detect_framebuffer", (PyCFunction)MGLContext_detect_framebuffer, METH_VARARGS},
    {"clear_samplers", (PyCFunction)MGLContext_clear_samplers, METH_VARARGS},
    {"buffer", (PyCFunction)MGLContext_buffer, METH_VARARGS},
    {"texture", (PyCFunction)MGLContext_texture, METH_VARARGS},
    {"texture3d", (PyCFunction)MGLContext_texture3d, METH_VARARGS},
    {"texture_array", (PyCFunction)MGLContext_texture_array, METH_VARARGS},
    {"texture_cube", (PyCFunction)MGLContext_texture_cube, METH_VARARGS},
    {"depth_texture", (PyCFunction)MGLContext_depth_texture, METH_VARARGS},
    {"external_texture", (PyCFunction)MGLContext_external_texture, METH_VARARGS},
    {"vertex_array", (PyCFunction)MGLContext_vertex_array, METH_VARARGS},
    {"program", (PyCFunction)MGLContext_program, METH_VARARGS},
    {"framebuffer", (PyCFunction)MGLContext_framebuffer, METH_VARARGS},
    {"renderbuffer", (PyCFunction)MGLContext_renderbuffer, METH_VARARGS},
    {"depth_renderbuffer", (PyCFunction)MGLContext_depth_renderbuffer, METH_VARARGS},
    {"compute_shader", (PyCFunction)MGLContext_compute_shader, METH_VARARGS},
    {"query", (PyCFunction)MGLContext_query, METH_VARARGS},
    {"scope", (PyCFunction)MGLContext_scope, METH_VARARGS},
    {"sampler", (PyCFunction)MGLContext_sampler, METH_NOARGS},
    {"__enter__", (PyCFunction)MGLContext_enter, METH_NOARGS},
    {"__exit__", (PyCFunction)MGLContext_exit, METH_VARARGS},
    {"release", (PyCFunction)MGLContext_release, METH_NOARGS},
    {"_get_ubo_binding", (PyCFunction)MGLContext_get_ubo_binding, METH_VARARGS},
    {"_set_ubo_binding", (PyCFunction)MGLContext_set_ubo_binding, METH_VARARGS},
    {"_write_uniform", (PyCFunction)MGLContext_write_uniform, METH_VARARGS},
    {"_read_uniform", (PyCFunction)MGLContext_read_uniform, METH_VARARGS},
    {},
};

PyGetSetDef MGLContext_getset[] = {
    {"line_width", (getter)MGLContext_get_line_width, (setter)MGLContext_set_line_width},
    {"point_size", (getter)MGLContext_get_point_size, (setter)MGLContext_set_point_size},
    {"depth_func", (getter)MGLContext_get_depth_func, (setter)MGLContext_set_depth_func},
    {"blend_func", (getter)MGLContext_get_blend_func, (setter)MGLContext_set_blend_func},
    {"blend_equation", (getter)MGLContext_get_blend_equation, (setter)MGLContext_set_blend_equation},
    {"multisample", (getter)MGLContext_get_multisample, (setter)MGLContext_set_multisample},
    {"provoking_vertex", (getter)MGLContext_get_provoking_vertex, (setter)MGLContext_set_provoking_vertex},
    {"polygon_offset", (getter)MGLContext_get_polygon_offset, (setter)MGLContext_set_polygon_offset},
    {"default_texture_unit", (getter)MGLContext_get_default_texture_unit, (setter)MGLContext_set_default_texture_unit},
    {"max_samples", (getter)MGLContext_get_max_samples, NULL},
    {"max_integer_samples", (getter)MGLContext_get_max_integer_samples, NULL},
    {"max_texture_units", (getter)MGLContext_get_max_texture_units, NULL},
    {"max_anisotropy", (getter)MGLContext_get_max_anisotropy, NULL},
    {"fbo", (getter)MGLContext_get_fbo, (setter)MGLContext_set_fbo},
    {"wireframe", (getter)MGLContext_get_wireframe, (setter)MGLContext_set_wireframe},
    {"front_face", (getter)MGLContext_get_front_face, (setter)MGLContext_set_front_face},
    {"cull_face", (getter)MGLContext_get_cull_face, (setter)MGLContext_set_cull_face},
    {"patch_vertices", (getter)MGLContext_get_patch_vertices, (setter)MGLContext_set_patch_vertices},
    {"extensions", (getter)MGLContext_get_extensions, NULL},
    {"info", (getter)MGLContext_get_info, NULL},
    {"error", (getter)MGLContext_get_error, NULL},
    {},
};

PyGetSetDef MGLFramebuffer_getset[] = {
    {"viewport", (getter)MGLFramebuffer_get_viewport, (setter)MGLFramebuffer_set_viewport},
    {"scissor", (getter)MGLFramebuffer_get_scissor, (setter)MGLFramebuffer_set_scissor},
    {"color_mask", (getter)MGLFramebuffer_get_color_mask, (setter)MGLFramebuffer_set_color_mask},
    {"depth_mask", (getter)MGLFramebuffer_get_depth_mask, (setter)MGLFramebuffer_set_depth_mask},
    {"bits", (getter)MGLFramebuffer_get_bits, NULL},
    {},
};

PyMethodDef MGLFramebuffer_methods[] = {
    {"clear", (PyCFunction)MGLFramebuffer_clear, METH_VARARGS},
    {"use", (PyCFunction)MGLFramebuffer_use, METH_NOARGS},
    {"read", (PyCFunction)MGLFramebuffer_read, METH_VARARGS},
    {"release", (PyCFunction)MGLFramebuffer_release, METH_NOARGS},
    {},
};

PyMethodDef MGLProgram_methods[] = {
    {"release", (PyCFunction)MGLProgram_release, METH_NOARGS},
    {},
};

PyGetSetDef MGLQuery_getset[] = {
    {"samples", (getter)MGLQuery_get_samples, NULL},
    {"primitives", (getter)MGLQuery_get_primitives, NULL},
    {"elapsed", (getter)MGLQuery_get_elapsed, NULL},
    {},
};

PyMethodDef MGLQuery_methods[] = {
    {"begin", (PyCFunction)MGLQuery_begin, METH_NOARGS},
    {"end", (PyCFunction)MGLQuery_end, METH_NOARGS},
    {"begin_render", (PyCFunction)MGLQuery_begin_render, METH_NOARGS},
    {"end_render", (PyCFunction)MGLQuery_end_render, METH_NOARGS},
    {},
};

PyMethodDef MGLRenderbuffer_methods[] = {
    {"release", (PyCFunction)MGLRenderbuffer_release, METH_NOARGS},
    {},
};

PyGetSetDef MGLSampler_getset[] = {
    {"repeat_x", (getter)MGLSampler_get_repeat_x, (setter)MGLSampler_set_repeat_x},
    {"repeat_y", (getter)MGLSampler_get_repeat_y, (setter)MGLSampler_set_repeat_y},
    {"repeat_z", (getter)MGLSampler_get_repeat_z, (setter)MGLSampler_set_repeat_z},
    {"filter", (getter)MGLSampler_get_filter, (setter)MGLSampler_set_filter},
    {"compare_func", (getter)MGLSampler_get_compare_func, (setter)MGLSampler_set_compare_func},
    {"anisotropy", (getter)MGLSampler_get_anisotropy, (setter)MGLSampler_set_anisotropy},
    {"border_color", (getter)MGLSampler_get_border_color, (setter)MGLSampler_set_border_color},
    {"min_lod", (getter)MGLSampler_get_min_lod, (setter)MGLSampler_set_min_lod},
    {"max_lod", (getter)MGLSampler_get_max_lod, (setter)MGLSampler_set_max_lod},
    {},
};

PyMethodDef MGLSampler_methods[] = {
    {"use", (PyCFunction)MGLSampler_use, METH_VARARGS},
    {"clear", (PyCFunction)MGLSampler_clear, METH_VARARGS},
    {"release", (PyCFunction)MGLSampler_release, METH_NOARGS},
    {},
};

PyMethodDef MGLScope_methods[] = {
    {"begin", (PyCFunction)MGLScope_begin, METH_NOARGS},
    {"end", (PyCFunction)MGLScope_end, METH_NOARGS},
    {"release", (PyCFunction)MGLScope_release, METH_NOARGS},
    {},
};

PyGetSetDef MGLTexture_getset[] = {
    {"repeat_x", (getter)MGLTexture_get_repeat_x, (setter)MGLTexture_set_repeat_x},
    {"repeat_y", (getter)MGLTexture_get_repeat_y, (setter)MGLTexture_set_repeat_y},
    {"filter", (getter)MGLTexture_get_filter, (setter)MGLTexture_set_filter},
    {"swizzle", (getter)MGLTexture_get_swizzle, (setter)MGLTexture_set_swizzle},
    {"compare_func", (getter)MGLTexture_get_compare_func, (setter)MGLTexture_set_compare_func},
    {"anisotropy", (getter)MGLTexture_get_anisotropy, (setter)MGLTexture_set_anisotropy},
    {},
};

PyMethodDef MGLTexture_methods[] = {
    {"write", (PyCFunction)MGLTexture_write, METH_VARARGS},
    {"bind", (PyCFunction)MGLTexture_meth_bind, METH_VARARGS},
    {"use", (PyCFunction)MGLTexture_use, METH_VARARGS},
    {"build_mipmaps", (PyCFunction)MGLTexture_build_mipmaps, METH_VARARGS},
    {"read", (PyCFunction)MGLTexture_read, METH_VARARGS},
    {"read_into", (PyCFunction)MGLTexture_read_into, METH_VARARGS},
    {"release", (PyCFunction)MGLTexture_release, METH_NOARGS},
    {},
};

PyGetSetDef MGLTexture3D_getset[] = {
    {"repeat_x", (getter)MGLTexture3D_get_repeat_x, (setter)MGLTexture3D_set_repeat_x},
    {"repeat_y", (getter)MGLTexture3D_get_repeat_y, (setter)MGLTexture3D_set_repeat_y},
    {"repeat_z", (getter)MGLTexture3D_get_repeat_z, (setter)MGLTexture3D_set_repeat_z},
    {"filter", (getter)MGLTexture3D_get_filter, (setter)MGLTexture3D_set_filter},
    {"swizzle", (getter)MGLTexture3D_get_swizzle, (setter)MGLTexture3D_set_swizzle},
    {},
};

PyMethodDef MGLTexture3D_methods[] = {
    {"write", (PyCFunction)MGLTexture3D_write, METH_VARARGS},
    {"bind", (PyCFunction)MGLTexture3D_meth_bind, METH_VARARGS},
    {"use", (PyCFunction)MGLTexture3D_use, METH_VARARGS},
    {"build_mipmaps", (PyCFunction)MGLTexture3D_build_mipmaps, METH_VARARGS},
    {"read", (PyCFunction)MGLTexture3D_read, METH_VARARGS},
    {"read_into", (PyCFunction)MGLTexture3D_read_into, METH_VARARGS},
    {"release", (PyCFunction)MGLTexture3D_release, METH_NOARGS},
    {},
};

PyGetSetDef MGLTextureArray_getset[] = {
    {"repeat_x", (getter)MGLTextureArray_get_repeat_x, (setter)MGLTextureArray_set_repeat_x},
    {"repeat_y", (getter)MGLTextureArray_get_repeat_y, (setter)MGLTextureArray_set_repeat_y},
    {"filter", (getter)MGLTextureArray_get_filter, (setter)MGLTextureArray_set_filter},
    {"swizzle", (getter)MGLTextureArray_get_swizzle, (setter)MGLTextureArray_set_swizzle},
    {"anisotropy", (getter)MGLTextureArray_get_anisotropy, (setter)MGLTextureArray_set_anisotropy},
    {},
};

PyMethodDef MGLTextureArray_methods[] = {
    {"write", (PyCFunction)MGLTextureArray_write, METH_VARARGS},
    {"bind", (PyCFunction)MGLTextureArray_meth_bind, METH_VARARGS},
    {"use", (PyCFunction)MGLTextureArray_use, METH_VARARGS},
    {"build_mipmaps", (PyCFunction)MGLTextureArray_build_mipmaps, METH_VARARGS},
    {"read", (PyCFunction)MGLTextureArray_read, METH_VARARGS},
    {"read_into", (PyCFunction)MGLTextureArray_read_into, METH_VARARGS},
    {"release", (PyCFunction)MGLTextureArray_release, METH_NOARGS},
    {},
};

PyGetSetDef MGLTextureCube_getset[] = {
    {"filter", (getter)MGLTextureCube_get_filter, (setter)MGLTextureCube_set_filter},
    {"swizzle", (getter)MGLTextureCube_get_swizzle, (setter)MGLTextureCube_set_swizzle},
    {"anisotropy", (getter)MGLTextureCube_get_anisotropy, (setter)MGLTextureCube_set_anisotropy},
    {},
};

PyMethodDef MGLTextureCube_methods[] = {
    {"write", (PyCFunction)MGLTextureCube_write, METH_VARARGS},
    {"use", (PyCFunction)MGLTextureCube_use, METH_VARARGS},
    {"bind", (PyCFunction)MGLTextureCube_meth_bind, METH_VARARGS},
    {"read", (PyCFunction)MGLTextureCube_read, METH_VARARGS},
    {"read_into", (PyCFunction)MGLTextureCube_read_into, METH_VARARGS},
    {"release", (PyCFunction)MGLTextureCube_release, METH_NOARGS},
    {},
};

PyMethodDef MGLVertexArray_methods[] = {
    {"render", (PyCFunction)MGLVertexArray_render, METH_VARARGS},
    {"render_indirect", (PyCFunction)MGLVertexArray_render_indirect, METH_VARARGS},
    {"transform", (PyCFunction)MGLVertexArray_transform, METH_VARARGS},
    {"bind", (PyCFunction)MGLVertexArray_bind, METH_VARARGS},
    {"release", (PyCFunction)MGLVertexArray_release, METH_NOARGS},
    {},
};

PyGetSetDef MGLVertexArray_getset[] = {
    {"index_buffer", NULL, (setter)MGLVertexArray_set_index_buffer},
    {"vertices", (getter)MGLVertexArray_get_vertices, (setter)MGLVertexArray_set_vertices},
    {"instances", (getter)MGLVertexArray_get_instances, (setter)MGLVertexArray_set_instances},
    {"subroutines", NULL, (setter)MGLVertexArray_set_subroutines},
    {},
};

PyType_Slot MGLBuffer_slots[] = {
    #if PY_VERSION_HEX >= 0x03090000
    {Py_bf_getbuffer, MGLBuffer_tp_as_buffer_get_view},
    {Py_bf_releasebuffer, MGLBuffer_tp_as_buffer_release_view},
    #endif
    {Py_tp_methods, MGLBuffer_methods},
    {Py_tp_dealloc, (void *)default_dealloc},
    {},
};

PyType_Slot MGLComputeShader_slots[] = {
    {Py_tp_methods, MGLComputeShader_methods},
    {Py_tp_dealloc, (void *)default_dealloc},
    {},
};

PyType_Slot MGLContext_slots[] = {
    {Py_tp_methods, MGLContext_methods},
    {Py_tp_getset, MGLContext_getset},
    {Py_tp_dealloc, (void *)default_dealloc},
    {},
};

PyType_Slot MGLFramebuffer_slots[] = {
    {Py_tp_methods, MGLFramebuffer_methods},
    {Py_tp_getset, MGLFramebuffer_getset},
    {Py_tp_dealloc, (void *)default_dealloc},
    {},
};

PyType_Slot MGLProgram_slots[] = {
    {Py_tp_methods, MGLProgram_methods},
    {Py_tp_dealloc, (void *)default_dealloc},
    {},
};

PyType_Slot MGLQuery_slots[] = {
    {Py_tp_methods, MGLQuery_methods},
    {Py_tp_getset, MGLQuery_getset},
    {Py_tp_dealloc, (void *)default_dealloc},
    {},
};

PyType_Slot MGLRenderbuffer_slots[] = {
    {Py_tp_methods, MGLRenderbuffer_methods},
    {Py_tp_dealloc, (void *)default_dealloc},
    {},
};

PyType_Slot MGLScope_slots[] = {
    {Py_tp_methods, MGLScope_methods},
    {Py_tp_dealloc, (void *)default_dealloc},
    {},
};

PyType_Slot MGLTexture_slots[] = {
    {Py_tp_methods, MGLTexture_methods},
    {Py_tp_getset, MGLTexture_getset},
    {Py_tp_dealloc, (void *)default_dealloc},
    {},
};

PyType_Slot MGLTextureArray_slots[] = {
    {Py_tp_methods, MGLTextureArray_methods},
    {Py_tp_getset, MGLTextureArray_getset},
    {Py_tp_dealloc, (void *)default_dealloc},
    {},
};

PyType_Slot MGLTextureCube_slots[] = {
    {Py_tp_methods, MGLTextureCube_methods},
    {Py_tp_getset, MGLTextureCube_getset},
    {Py_tp_dealloc, (void *)default_dealloc},
    {},
};

PyType_Slot MGLTexture3D_slots[] = {
    {Py_tp_methods, MGLTexture3D_methods},
    {Py_tp_getset, MGLTexture3D_getset},
    {Py_tp_dealloc, (void *)default_dealloc},
    {},
};

PyType_Slot MGLVertexArray_slots[] = {
    {Py_tp_methods, MGLVertexArray_methods},
    {Py_tp_getset, MGLVertexArray_getset},
    {Py_tp_dealloc, (void *)default_dealloc},
    {},
};

PyType_Slot MGLSampler_slots[] = {
    {Py_tp_methods, MGLSampler_methods},
    {Py_tp_getset, MGLSampler_getset},
    {Py_tp_dealloc, (void *)default_dealloc},
    {},
};

PyType_Spec MGLBuffer_spec = {"mgl.Buffer", sizeof(MGLBuffer), 0, Py_TPFLAGS_DEFAULT, MGLBuffer_slots};
PyType_Spec MGLComputeShader_spec = {"mgl.ComputeShader", sizeof(MGLComputeShader), 0, Py_TPFLAGS_DEFAULT, MGLComputeShader_slots};
PyType_Spec MGLContext_spec = {"mgl.Context", sizeof(MGLContext), 0, Py_TPFLAGS_DEFAULT, MGLContext_slots};
PyType_Spec MGLFramebuffer_spec = {"mgl.Framebuffer", sizeof(MGLFramebuffer), 0, Py_TPFLAGS_DEFAULT, MGLFramebuffer_slots};
PyType_Spec MGLProgram_spec = {"mgl.Program", sizeof(MGLProgram), 0, Py_TPFLAGS_DEFAULT, MGLProgram_slots};
PyType_Spec MGLQuery_spec = {"mgl.Query", sizeof(MGLQuery), 0, Py_TPFLAGS_DEFAULT, MGLQuery_slots};
PyType_Spec MGLRenderbuffer_spec = {"mgl.Renderbuffer", sizeof(MGLRenderbuffer), 0, Py_TPFLAGS_DEFAULT, MGLRenderbuffer_slots};
PyType_Spec MGLScope_spec = {"mgl.Scope", sizeof(MGLScope), 0, Py_TPFLAGS_DEFAULT, MGLScope_slots};
PyType_Spec MGLTexture_spec = {"mgl.Texture", sizeof(MGLTexture), 0, Py_TPFLAGS_DEFAULT, MGLTexture_slots};
PyType_Spec MGLTextureArray_spec = {"mgl.TextureArray", sizeof(MGLTextureArray), 0, Py_TPFLAGS_DEFAULT, MGLTextureArray_slots};
PyType_Spec MGLTextureCube_spec = {"mgl.TextureCube", sizeof(MGLTextureCube), 0, Py_TPFLAGS_DEFAULT, MGLTextureCube_slots};
PyType_Spec MGLTexture3D_spec = {"mgl.Texture3D", sizeof(MGLTexture3D), 0, Py_TPFLAGS_DEFAULT, MGLTexture3D_slots};
PyType_Spec MGLVertexArray_spec = {"mgl.VertexArray", sizeof(MGLVertexArray), 0, Py_TPFLAGS_DEFAULT, MGLVertexArray_slots};
PyType_Spec MGLSampler_spec = {"mgl.Sampler", sizeof(MGLSampler), 0, Py_TPFLAGS_DEFAULT, MGLSampler_slots};

PyModuleDef MGL_moduledef = {PyModuleDef_HEAD_INIT, "mgl", NULL, -1, module_methods};

extern "C" PyObject * PyInit_mgl() {
    PyObject * module = PyModule_Create(&MGL_moduledef);

    helper = PyImport_ImportModule("_moderngl");
    if (!helper) {
        return NULL;
    }

    moderngl_error = PyObject_GetAttrString(helper, "Error");

    MGLBuffer_type = (PyTypeObject *)PyType_FromSpec(&MGLBuffer_spec);
    MGLComputeShader_type = (PyTypeObject *)PyType_FromSpec(&MGLComputeShader_spec);
    MGLContext_type = (PyTypeObject *)PyType_FromSpec(&MGLContext_spec);
    MGLFramebuffer_type = (PyTypeObject *)PyType_FromSpec(&MGLFramebuffer_spec);
    MGLProgram_type = (PyTypeObject *)PyType_FromSpec(&MGLProgram_spec);
    MGLQuery_type = (PyTypeObject *)PyType_FromSpec(&MGLQuery_spec);
    MGLRenderbuffer_type = (PyTypeObject *)PyType_FromSpec(&MGLRenderbuffer_spec);
    MGLScope_type = (PyTypeObject *)PyType_FromSpec(&MGLScope_spec);
    MGLTexture_type = (PyTypeObject *)PyType_FromSpec(&MGLTexture_spec);
    MGLTextureArray_type = (PyTypeObject *)PyType_FromSpec(&MGLTextureArray_spec);
    MGLTextureCube_type = (PyTypeObject *)PyType_FromSpec(&MGLTextureCube_spec);
    MGLTexture3D_type = (PyTypeObject *)PyType_FromSpec(&MGLTexture3D_spec);
    MGLVertexArray_type = (PyTypeObject *)PyType_FromSpec(&MGLVertexArray_spec);
    MGLSampler_type = (PyTypeObject *)PyType_FromSpec(&MGLSampler_spec);

    PyObject * InvalidObject = PyObject_GetAttrString(helper, "InvalidObject");
    PyModule_AddObject(module, "InvalidObject", InvalidObject);
    Py_INCREF(InvalidObject);

    return module;
}