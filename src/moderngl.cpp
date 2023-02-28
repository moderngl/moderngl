#include <Python.h>
#include <structmember.h>

#include "gl_methods.hpp"

#define MGLError_Set(...) PyErr_Format(moderngl_error, __VA_ARGS__)

#define MGL_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MGL_MIN(a, b) (((a) < (b)) ? (a) : (b))

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

struct Viewport {
    int x, y, width, height;
};

int parse_viewport(PyObject * arg, Viewport * viewport) {
    PyObject * seq = PySequence_Fast(arg, "viewport or scissor not iterable");
    if (!seq) {
        return 0;
    }
    int size = (int)PySequence_Size(seq);
    if (size != 2 && size != 4) {
        PyErr_Format(PyExc_ValueError, "viewport or scissor size must be 2 or 4");
        return 0;
    }
    int temp[4];
    for (int i = 0; i < size; ++i) {
        PyObject * value = PyNumber_Long(PySequence_GetItem(seq, i));
        if (!value) {
            return 0;
        }
        temp[i] = PyLong_AsLong(value);
        if (PyErr_Occurred()) {
            return 0;
        }
        Py_DECREF(value);
    }
    if (size == 4) {
        *viewport = {temp[0], temp[1], temp[2], temp[3]};
    }
    if (size == 2) {
        *viewport = {temp[0], temp[1]};
    }
    return 1;
}

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
    MGLContext * extra;
    PyObject * members;
    int program_obj;
    int shader_obj;
    bool released;
};

struct MGLContext {
    PyObject_HEAD
    PyObject * ctx;
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
    MGLContext * extra;
    PyObject * size_tuple;
    int num_color_attachments;
    bool color_mask[64][4];
    int framebuffer_obj;
    Viewport viewport;
    bool scissor_enabled;
    Viewport scissor;

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
    MGLContext * extra;
    PyObject * members;
    int is_transform;
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

enum MGLQueryState {
    QUERY_INACTIVE,
    QUERY_ACTIVE,
    QUERY_CONDITIONAL_RENDER,
};

struct MGLQuery {
    PyObject_HEAD
    MGLContext * context;
    int query_obj[4];
    MGLQueryState state;
    bool ended;
    bool released;
};

struct MGLRenderbuffer {
    PyObject_HEAD
    MGLContext * context;
    MGLContext * extra;
    MGLDataType * data_type;
    PyObject * size_tuple;
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
    MGLContext * extra;
    MGLDataType * data_type;
    PyObject * size_tuple;
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

const char * framebuffer_status_text(int status) {
    if (status == GL_FRAMEBUFFER_COMPLETE) {
        return NULL;
    }

    switch (status) {
        case GL_FRAMEBUFFER_UNDEFINED:
            return "the framebuffer is not complete (UNDEFINED)";

        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            return "the framebuffer is not complete (INCOMPLETE_ATTACHMENT)";

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            return "the framebuffer is not complete (INCOMPLETE_MISSING_ATTACHMENT)";

        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            return "the framebuffer is not complete (INCOMPLETE_DRAW_BUFFER)";

        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            return "the framebuffer is not complete (INCOMPLETE_READ_BUFFER)";

        case GL_FRAMEBUFFER_UNSUPPORTED:
            return "the framebuffer is not complete (UNSUPPORTED)";

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            return "the framebuffer is not complete (INCOMPLETE_MULTISAMPLE)";

        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            return "the framebuffer is not complete (INCOMPLETE_LAYER_TARGETS)";
    }

    return "the framebuffer is not complete";
}

inline void clean_glsl_name(char * name, int & name_len) {
    if (name_len && name[name_len - 1] == ']') {
        name_len -= 1;
        while (name_len && name[name_len] != '[') {
            name_len -= 1;
        }
    }
    name[name_len] = 0;
}

inline int swizzle_from_char(char c) {
    switch (c) {
        case 'R':
        case 'r':
            return GL_RED;

        case 'G':
        case 'g':
            return GL_GREEN;

        case 'B':
        case 'b':
            return GL_BLUE;

        case 'A':
        case 'a':
            return GL_ALPHA;

        case '0':
            return GL_ZERO;

        case '1':
            return GL_ONE;
    }

    return -1;
}

inline char char_from_swizzle(int c) {
    switch (c) {
        case GL_RED:
            return 'R';

        case GL_GREEN:
            return 'G';

        case GL_BLUE:
            return 'B';

        case GL_ALPHA:
            return 'A';

        case GL_ZERO:
            return '0';

        case GL_ONE:
            return '1';
    }

    return '?';
}

inline int compare_func_from_string(const char * str) {
    if (!str[0] || (str[1] && str[2])) {
        return 0;
    }

    switch (str[0] * 256 + str[1]) {
        case ('<' * 256 + '='):
            return GL_LEQUAL;

        case ('<' * 256):
            return GL_LESS;

        case ('>' * 256 + '='):
            return GL_GEQUAL;

        case ('>' * 256):
            return GL_GREATER;

        case ('=' * 256 + '='):
            return GL_EQUAL;

        case ('!' * 256 + '='):
            return GL_NOTEQUAL;

        case ('0' * 256):
            return GL_NEVER;

        case ('1' * 256):
            return GL_ALWAYS;

        default:
            return 0;
    }
}

inline PyObject * compare_func_to_string(int func) {
    switch (func) {
        case GL_LEQUAL: {
            static PyObject * res_lequal = PyUnicode_FromString("<=");
            Py_INCREF(res_lequal);
            return res_lequal;
        }

        case GL_LESS: {
            static PyObject * res_less = PyUnicode_FromString("<");
            Py_INCREF(res_less);
            return res_less;
        }

        case GL_GEQUAL: {
            static PyObject * res_gequal = PyUnicode_FromString(">=");
            Py_INCREF(res_gequal);
            return res_gequal;
        }

        case GL_GREATER: {
            static PyObject * res_greater = PyUnicode_FromString(">");
            Py_INCREF(res_greater);
            return res_greater;
        }

        case GL_EQUAL: {
            static PyObject * res_equal = PyUnicode_FromString("==");
            Py_INCREF(res_equal);
            return res_equal;
        }

        case GL_NOTEQUAL: {
            static PyObject * res_notequal = PyUnicode_FromString("!=");
            Py_INCREF(res_notequal);
            return res_notequal;
        }

        case GL_NEVER: {
            static PyObject * res_never = PyUnicode_FromString("0");
            Py_INCREF(res_never);
            return res_never;
        }

        case GL_ALWAYS: {
            static PyObject * res_always = PyUnicode_FromString("1");
            Py_INCREF(res_always);
            return res_always;
        }

        default: {
            static PyObject * res_unk = PyUnicode_FromString("?");
            Py_INCREF(res_unk);
            return res_unk;
        }
    }
}

inline PyObject * tuple2(PyObject * a, PyObject * b) {
    PyObject * res = PyTuple_New(2);
    PyTuple_SET_ITEM(res, 0, a);
    PyTuple_SET_ITEM(res, 1, b);
    return res;
}

inline PyObject * tuple3(PyObject * a, PyObject * b, PyObject * c) {
    PyObject * res = PyTuple_New(3);
    PyTuple_SET_ITEM(res, 0, a);
    PyTuple_SET_ITEM(res, 1, b);
    PyTuple_SET_ITEM(res, 2, c);
    return res;
}

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

    // if (!dtype[0] || (dtype[1] && dtype[2])) {
    // 	return 0;
    // }

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

    int args_ok = PyArg_ParseTuple(
        args,
        "OIp",
        &data,
        &reserve,
        &dynamic
    );

    if (!args_ok) {
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

    int args_ok = PyArg_ParseTuple(
        args,
        "On",
        &data,
        &offset
    );

    if (!args_ok) {
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

    int args_ok = PyArg_ParseTuple(
        args,
        "nn",
        &size,
        &offset
    );

    if (!args_ok) {
        return 0;
    }

    if (size < 0) {
        size = self->size - offset;
    }

    if (offset < 0 || offset + size > self->size) {
        MGLError_Set("out of rangeoffset = %d or size = %d", offset, size);
        return 0;
    }

    const GLMethods & gl = self->context->gl;

    gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);
    void * map = gl.MapBufferRange(GL_ARRAY_BUFFER, offset, size, GL_MAP_READ_BIT);

    if (!map) {
        MGLError_Set("cannot map the buffer");
        return 0;
    }

    PyObject * data = PyBytes_FromStringAndSize((const char *)map, size);

    gl.UnmapBuffer(GL_ARRAY_BUFFER);

    return data;
}

PyObject * MGLBuffer_read_into(MGLBuffer * self, PyObject * args) {
    PyObject * data;
    Py_ssize_t size;
    Py_ssize_t offset;
    Py_ssize_t write_offset;

    int args_ok = PyArg_ParseTuple(
        args,
        "Onnn",
        &data,
        &size,
        &offset,
        &write_offset
    );

    if (!args_ok) {
        return 0;
    }

    if (size < 0) {
        size = self->size - offset;
    }

    if (offset < 0 || write_offset < 0 || offset + size > self->size) {
        MGLError_Set("out of range");
        return 0;
    }

    Py_buffer buffer_view;

    int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_WRITABLE);
    if (get_buffer < 0) {
        // Propagate the default error
        return 0;
    }

    if (buffer_view.len < write_offset + size) {
        MGLError_Set("the buffer is too small");
        PyBuffer_Release(&buffer_view);
        return 0;
    }

    const GLMethods & gl = self->context->gl;

    gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);
    void * map = gl.MapBufferRange(GL_ARRAY_BUFFER, offset, size, GL_MAP_READ_BIT);

    char * ptr = (char *)buffer_view.buf + write_offset;
    memcpy(ptr, map, size);

    gl.UnmapBuffer(GL_ARRAY_BUFFER);

    PyBuffer_Release(&buffer_view);
    Py_RETURN_NONE;
}

PyObject * MGLBuffer_write_chunks(MGLBuffer * self, PyObject * args) {
    PyObject * data;
    Py_ssize_t start;
    Py_ssize_t step;
    Py_ssize_t count;

    int args_ok = PyArg_ParseTuple(
        args,
        "Onnn",
        &data,
        &start,
        &step,
        &count
    );

    if (!args_ok) {
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

    int args_ok = PyArg_ParseTuple(
        args,
        "nnnn",
        &chunk_size,
        &start,
        &step,
        &count
    );

    if (!args_ok) {
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

    int args_ok = PyArg_ParseTuple(
        args,
        "Onnnnn",
        &data,
        &chunk_size,
        &start,
        &step,
        &count,
        &write_offset
    );

    if (!args_ok) {
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

    int args_ok = PyArg_ParseTuple(
        args,
        "nnO",
        &size,
        &offset,
        &chunk
    );

    if (!args_ok) {
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

    int args_ok = PyArg_ParseTuple(
        args,
        "n",
        &size
    );

    if (!args_ok) {
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

    int args_ok = PyArg_ParseTuple(
        args,
        "Inn",
        &binding,
        &offset,
        &size
    );

    if (!args_ok) {
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

    int args_ok = PyArg_ParseTuple(
        args,
        "Inn",
        &binding,
        &offset,
        &size
    );

    if (!args_ok) {
        return 0;
    }

    if (size < 0) {
        size = self->size - offset;
    }

    const GLMethods & gl = self->context->gl;
    gl.BindBufferRange(GL_SHADER_STORAGE_BUFFER, binding, self->buffer_obj, offset, size);
    Py_RETURN_NONE;
}

PyObject * MGLBuffer_release(MGLBuffer * self, PyObject * args) {
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

PyObject * MGLBuffer_size(MGLBuffer * self, PyObject * args) {
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

MGLComputeShader * MGLContext_compute_shader(MGLContext * self, PyObject * args, PyObject * kwargs) {
    const char * keywords[] = {"source", NULL};
    PyObject * source;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", (char **)keywords, &source)) {
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
    int num_storage_blocks = 0;

    gl.GetProgramiv(program_obj, GL_ACTIVE_UNIFORMS, &num_uniforms);
    gl.GetProgramiv(program_obj, GL_ACTIVE_UNIFORM_BLOCKS, &num_uniform_blocks);
    gl.GetProgramInterfaceiv(program_obj, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &num_storage_blocks);

    PyObject * members_dict = PyDict_New();

    for (int i = 0; i < num_uniforms; ++i) {
        int type = 0;
        int array_length = 0;
        int name_len = 0;
        char name[256];

        gl.GetActiveUniform(program_obj, i, 256, &name_len, &array_length, (GLenum *)&type, name);
        int location = gl.GetUniformLocation(program_obj, name);

        clean_glsl_name(name, name_len);

        if (location < 0) {
            continue;
        }

        PyObject * item = PyObject_CallMethod(
            helper, "make_uniform", "(siiiiO)",
            name, type, program_obj, location, array_length, self
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

        clean_glsl_name(name, name_len);

        PyObject * item = PyObject_CallMethod(
            helper, "make_uniform_block", "(siiiO)",
            name, program_obj, index, size, self
        );

        PyDict_SetItemString(members_dict, name, item);
        Py_DECREF(item);
    }

    for(int i = 0; i < num_storage_blocks; ++i) {
        int name_len = 0;
        char name[256];

        gl.GetProgramResourceName(program_obj, GL_SHADER_STORAGE_BLOCK, i, 256, &name_len, name);
        clean_glsl_name(name, name_len);

        PyObject * item = PyObject_CallMethod(
            helper, "make_storage_block", "(siiO)",
            name, program_obj, i, self
        );

        PyDict_SetItemString(members_dict, name, item);
        Py_DECREF(item);
    }

    compute_shader->members = members_dict;

    Py_INCREF(compute_shader);
    return compute_shader;
}

PyObject * MGLComputeShader_run(MGLComputeShader * self, PyObject * args, PyObject * kwargs) {
    const char * keywords[] = {"group_x", "group_y", "group_z", NULL};

    unsigned x = 1, y = 1, z = 1;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|III", (char **)keywords, &x, &y, &z)) {
        return 0;
    }

    const GLMethods & gl = self->context->gl;

    gl.UseProgram(self->program_obj);
    gl.DispatchCompute(x, y, z);
    Py_RETURN_NONE;
}

PyObject * MGLComputeShader_run_indirect(MGLComputeShader * self, PyObject * args, PyObject * kwargs) {
    const char * keywords[] = {"buffer", "offset", NULL};

    MGLBuffer * buffer;
    Py_ssize_t offset = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!|n", (char **)keywords, MGLBuffer_type, &buffer, &offset)) {
        return 0;
    }

    const GLMethods & gl = self->context->gl;

    gl.UseProgram(self->program_obj);
    gl.BindBuffer(GL_DISPATCH_INDIRECT_BUFFER, buffer->buffer_obj);
    gl.DispatchComputeIndirect((GLintptr)offset);
    Py_RETURN_NONE;
}

PyObject * MGLComputeShader_getitem(MGLComputeShader * self, PyObject * key) {
    PyObject * res = PyDict_GetItem(self->members, key);
    Py_XINCREF(res);
    return res;
}

int MGLComputeShader_setitem(MGLComputeShader * self, PyObject * key, PyObject * value) {
    PyObject * res = PyDict_GetItem(self->members, key);
    if (!res) {
        return -1;
    }
    return PyObject_SetAttrString(res, "value", value);
}

PyObject * MGLComputeShader_release(MGLComputeShader * self, PyObject * args) {
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

MGLFramebuffer * MGLContext_framebuffer(MGLContext * self, PyObject * args) {
    PyObject * color_attachments;
    PyObject * depth_attachment;

    if (!PyArg_ParseTuple(args, "OO", &color_attachments, &depth_attachment)) {
        return NULL;
    }

    PyObject * tuple = PyObject_CallMethod(helper, "framebuffer_attachments", "(OO)", color_attachments, depth_attachment);
    if (!tuple) {
        return NULL;
    }

    int width = PyLong_AsLong(PyTuple_GetItem(tuple, 0));
    int height = PyLong_AsLong(PyTuple_GetItem(tuple, 1));
    int samples = PyLong_AsLong(PyTuple_GetItem(tuple, 2));
    color_attachments = PyTuple_GetItem(tuple, 3);
    depth_attachment = PyTuple_GetItem(tuple, 4);
    PyObject * color_mask = PyTuple_GetItem(tuple, 5);

    const GLMethods & gl = self->gl;

    MGLFramebuffer * framebuffer = PyObject_New(MGLFramebuffer, MGLFramebuffer_type);
    framebuffer->released = false;

    framebuffer->num_color_attachments = (int)PyList_Size(color_attachments);

    framebuffer->framebuffer_obj = 0;
    gl.GenFramebuffers(1, (GLuint *)&framebuffer->framebuffer_obj);

    if (!framebuffer->framebuffer_obj) {
        MGLError_Set("cannot create framebuffer");
        Py_DECREF(framebuffer);
        return NULL;
    }

    gl.BindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebuffer_obj);

    for (int i = 0; i < framebuffer->num_color_attachments; ++i) {
        PyObject * item = PyList_GetItem(color_attachments, i);

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

    unsigned draw_buffers[64] = {};
    for (int i = 0; i < framebuffer->num_color_attachments; ++i) {
        draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    gl.DrawBuffers(framebuffer->num_color_attachments, draw_buffers);
    gl.ReadBuffer(framebuffer->num_color_attachments ? GL_COLOR_ATTACHMENT0 : GL_NONE);

    int status = gl.CheckFramebufferStatus(GL_FRAMEBUFFER);
    gl.BindFramebuffer(GL_FRAMEBUFFER, self->bound_framebuffer->framebuffer_obj);

    if (const char * incomplete = framebuffer_status_text(status)) {
        MGLError_Set("%s", incomplete);
        return NULL;
    }

    memset(framebuffer->color_mask, 0, sizeof(framebuffer->color_mask));
    memcpy(framebuffer->color_mask, PyBytes_AsString(color_mask), PyBytes_Size(color_mask));
    framebuffer->depth_mask = (depth_attachment != Py_None);

    framebuffer->viewport = {0, 0, width, height};
    framebuffer->dynamic = false;

    framebuffer->scissor_enabled = false;
    framebuffer->scissor = {0, 0, width, height};

    framebuffer->extra = NULL;
    framebuffer->size_tuple = Py_BuildValue("(ii)", width, height);
    framebuffer->width = width;
    framebuffer->height = height;
    framebuffer->samples = samples;

    Py_INCREF(self);
    framebuffer->context = self;

    Py_INCREF(framebuffer);
    return framebuffer;
}

PyObject * MGLContext_empty_framebuffer(MGLContext * self, PyObject * args) {
    int width;
    int height;
    int layers = 0;
    int samples = 0;

    if (!PyArg_ParseTuple(args, "(II)|II", &width, &height, &layers, &samples)) {
        return 0;
    }

    const GLMethods & gl = self->gl;

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
    gl.DrawBuffer(GL_NONE);
    gl.ReadBuffer(GL_NONE);

    gl.FramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, width);
    gl.FramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, height);

    if (layers) {
        gl.FramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_LAYERS, layers);
    }

    if (samples) {
        gl.FramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES, samples);
    }

    int status = gl.CheckFramebufferStatus(GL_FRAMEBUFFER);
    gl.BindFramebuffer(GL_FRAMEBUFFER, self->bound_framebuffer->framebuffer_obj);

    if (const char * incomplete = framebuffer_status_text(status)) {
        MGLError_Set("%s", incomplete);
        return NULL;
    }

    memset(framebuffer->color_mask, 0, sizeof(framebuffer->color_mask));
    framebuffer->depth_mask = false;

    framebuffer->viewport = {0, 0, width, height};
    framebuffer->dynamic = false;

    framebuffer->scissor_enabled = false;
    framebuffer->scissor = {0, 0, width, height};

    framebuffer->extra = NULL;
    framebuffer->size_tuple = Py_BuildValue("(ii)", width, height);
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

PyObject * MGLFramebuffer_release(MGLFramebuffer * self, PyObject * args) {
    if (self->released) {
        Py_RETURN_NONE;
    }
    self->released = true;

    if (self->framebuffer_obj) {
        self->context->gl.DeleteFramebuffers(1, (GLuint *)&self->framebuffer_obj);
        Py_DECREF(self->context);
    }

    Py_DECREF(self);
    Py_RETURN_NONE;
}

PyObject * MGLFramebuffer_clear(MGLFramebuffer * self, PyObject * args, PyObject * kwargs) {
    const char * keywords[] = {"red", "green", "blue", "alpha", "viewport", "depth", NULL};
    float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f, depth = 1.0f;
    PyObject * viewport = Py_None;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|fffffO", (char **)keywords, &r, &g, &b, &a, &depth, &viewport)) {
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

    gl.ClearColor(r, g, b, a);
    gl.ClearDepthf(depth);

    for (int i = 0; i < self->num_color_attachments; ++i) {
        bool (*mask)[4] = self->color_mask;
        gl.ColorMaski(i, mask[i][0], mask[i][1], mask[i][2], mask[i][3]);
    }

    gl.DepthMask(self->depth_mask);

    // Respect the passed in viewport even with scissor enabled
    if (viewport != Py_None) {
        gl.Enable(GL_SCISSOR_TEST);
        gl.Scissor(x, y, width, height);
        gl.Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // restore scissor if enabled
        if (self->scissor_enabled) {
            gl.Scissor(
                self->scissor.x, self->scissor.y,
                self->scissor.width, self->scissor.height
            );
        } else {
            gl.Disable(GL_SCISSOR_TEST);
        }
    } else {
        // clear with scissor if enabled
        if (self->scissor_enabled) {
            gl.Enable(GL_SCISSOR_TEST);
            gl.Scissor(
                self->scissor.x, self->scissor.y,
                self->scissor.width, self->scissor.height
            );
        }
        gl.Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }

    gl.BindFramebuffer(GL_FRAMEBUFFER, self->context->bound_framebuffer->framebuffer_obj);

    Py_RETURN_NONE;
}

PyObject * MGLFramebuffer_use(MGLFramebuffer * self, PyObject * args) {
    const GLMethods & gl = self->context->gl;

    gl.BindFramebuffer(GL_FRAMEBUFFER, self->framebuffer_obj);

    if (self->viewport.width && self->viewport.height) {
        gl.Viewport(self->viewport.x, self->viewport.y, self->viewport.width, self->viewport.height);
    }

    if (self->scissor_enabled) {
        gl.Enable(GL_SCISSOR_TEST);
        gl.Scissor(self->scissor.x, self->scissor.y, self->scissor.width, self->scissor.height);
    } else {
        gl.Disable(GL_SCISSOR_TEST);
    }

    for (int i = 0; i < self->num_color_attachments; ++i) {
        bool (*mask)[4] = self->color_mask;
        gl.ColorMaski(i, mask[i][0], mask[i][1], mask[i][2], mask[i][3]);
    }

    gl.DepthMask(self->depth_mask);

    Py_INCREF(self);
    Py_DECREF(self->context->bound_framebuffer);
    self->context->bound_framebuffer = self;

    Py_RETURN_NONE;
}

PyObject * MGLFramebuffer_read(MGLFramebuffer * self, PyObject * args, PyObject * kwargs) {
    const char * keywords[] = {"viewport", "components", "attachment", "alignment", "clamp", "dtype", NULL};

    PyObject * viewport = Py_None;
    int components = 3;
    int attachment = 0;
    int alignment = 1;
    int clamp = false;

    const char * dtype = "f1";
    Py_ssize_t dtype_size = 2;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args,
        kwargs,
        "|OIIIps#",
        (char **)keywords,
        &viewport,
        &components,
        &attachment,
        &alignment,
        &clamp,
        &dtype,
        &dtype_size
    );

    if (!args_ok) {
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

    bool read_depth = false;

    if (attachment == -1) {
        components = 1;
        read_depth = true;
    }

    int expected_size = width * components * data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * height;

    int pixel_type = data_type->gl_type;
    int base_format = read_depth ? GL_DEPTH_COMPONENT : data_type->base_format[components];

    PyObject * result = PyBytes_FromStringAndSize(0, expected_size);
    char * data = PyBytes_AS_STRING(result);

    const GLMethods & gl = self->context->gl;

    if (clamp) {
        gl.ClampColor(GL_CLAMP_READ_COLOR, GL_TRUE);
    } else {
        gl.ClampColor(GL_CLAMP_READ_COLOR, GL_FIXED_ONLY);
    }

    gl.BindFramebuffer(GL_FRAMEBUFFER, self->framebuffer_obj);
    // if (self->framebuffer_obj) {
    gl.ReadBuffer(read_depth ? GL_NONE : (GL_COLOR_ATTACHMENT0 + attachment));
    // } else {
    // gl.ReadBuffer(GL_BACK_LEFT);
    // gl.ReadBuffer(self->draw_buffers[0]);
    // }
    gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
    gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
    gl.ReadPixels(x, y, width, height, base_format, pixel_type, data);
    gl.BindFramebuffer(GL_FRAMEBUFFER, self->context->bound_framebuffer->framebuffer_obj);

    return result;
}

PyObject * MGLFramebuffer_read_into(MGLFramebuffer * self, PyObject * args, PyObject * kwargs) {
    const char * keywords[] = {"data", "viewport", "components", "attachment", "alignment", "clamp", "dtype", "write_offset", NULL};

    PyObject * data;
    PyObject * viewport = Py_None;
    int components = 3;
    int attachment = 0;
    int alignment = 1;
    int clamp = false;
    const char * dtype = "f1";
    Py_ssize_t dtype_size = 2;
    Py_ssize_t write_offset = 0;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args,
        kwargs,
        "O|OIIIps#n",
        (char **)keywords,
        &data,
        &viewport,
        &components,
        &attachment,
        &alignment,
        &clamp,
        &dtype,
        &dtype_size,
        &write_offset
    );

    if (!args_ok) {
        return 0;
    }

    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
        MGLError_Set("the alignment must be 1, 2, 4 or 8");
        return 0;
    }

    const GLMethods & gl = self->context->gl;

    if (clamp) {
        gl.ClampColor(GL_CLAMP_READ_COLOR, GL_TRUE);
    } else {
        gl.ClampColor(GL_CLAMP_READ_COLOR, GL_FIXED_ONLY);
    }

    MGLDataType * data_type = from_dtype(dtype, dtype_size);

    if (!data_type) {
        MGLError_Set("invalid dtype");
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

    bool read_depth = false;

    if (attachment == -1) {
        components = 1;
        read_depth = true;
    }

    int expected_size = width * components * data_type->size;
    expected_size = (expected_size + alignment - 1) / alignment * alignment;
    expected_size = expected_size * height;

    int pixel_type = data_type->gl_type;
    int base_format = read_depth ? GL_DEPTH_COMPONENT : data_type->base_format[components];

    if (Py_TYPE(data) == MGLBuffer_type) {

        MGLBuffer * buffer = (MGLBuffer *)data;

        gl.BindBuffer(GL_PIXEL_PACK_BUFFER, buffer->buffer_obj);
        gl.BindFramebuffer(GL_FRAMEBUFFER, self->framebuffer_obj);
        gl.ReadBuffer(read_depth ? GL_NONE : (GL_COLOR_ATTACHMENT0 + attachment));
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.ReadPixels(x, y, width, height, base_format, pixel_type, (void *)write_offset);
        gl.BindFramebuffer(GL_FRAMEBUFFER, self->context->bound_framebuffer->framebuffer_obj);
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

        gl.BindFramebuffer(GL_FRAMEBUFFER, self->framebuffer_obj);
        gl.ReadBuffer(read_depth ? GL_NONE : (GL_COLOR_ATTACHMENT0 + attachment));
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.ReadPixels(x, y, width, height, base_format, pixel_type, ptr);
        gl.BindFramebuffer(GL_FRAMEBUFFER, self->context->bound_framebuffer->framebuffer_obj);

        PyBuffer_Release(&buffer_view);
    }

    return PyLong_FromLong(expected_size);
}

PyObject * MGLFramebuffer_get_viewport(MGLFramebuffer * self, void * closure) {
    return Py_BuildValue("(iiii)", self->viewport.x, self->viewport.y, self->viewport.width, self->viewport.height);
}

int MGLFramebuffer_set_viewport(MGLFramebuffer * self, PyObject * value, void * closure) {
    Viewport viewport = {};
    if (!parse_viewport(value, &viewport)) {
        return -1;
    }

    self->viewport = viewport;

    if (self->framebuffer_obj == self->context->bound_framebuffer->framebuffer_obj) {
        const GLMethods & gl = self->context->gl;
        gl.Viewport(self->viewport.x, self->viewport.y, self->viewport.width, self->viewport.height);
    }

    return 0;
}

PyObject * MGLFramebuffer_get_scissor(MGLFramebuffer * self, void * closure) {
    return Py_BuildValue("(iiii)", self->scissor.x, self->scissor.y, self->scissor.width, self->scissor.height);
}

int MGLFramebuffer_set_scissor(MGLFramebuffer * self, PyObject * value, void * closure) {
    if (value == Py_None) {
        self->scissor_enabled = false;
        self->scissor = {0, 0, self->width, self->height};
    } else {
        Viewport scissor = {};
        if (!parse_viewport(value, &scissor)) {
            return -1;
        }

        self->scissor_enabled = true;
        self->scissor = scissor;
    }

    if (self->framebuffer_obj == self->context->bound_framebuffer->framebuffer_obj) {
        const GLMethods & gl = self->context->gl;

        if (self->scissor_enabled) {
            gl.Enable(GL_SCISSOR_TEST);
        } else {
            gl.Disable(GL_SCISSOR_TEST);
        }

        gl.Scissor(self->scissor.x, self->scissor.y, self->scissor.width, self->scissor.height);
    }

    return 0;
}

PyObject * MGLFramebuffer_get_color_mask(MGLFramebuffer * self, void * closure) {
    PyObject * res = PyList_New(self->num_color_attachments);
    for (int i = 0; i < self->num_color_attachments; ++i) {
        PyList_SetItem(res, i, Py_BuildValue(
            "(OOOO)",
            self->color_mask[i][0] ? Py_True : Py_False,
            self->color_mask[i][1] ? Py_True : Py_False,
            self->color_mask[i][2] ? Py_True : Py_False,
            self->color_mask[i][3] ? Py_True : Py_False
        ));
    }

    return res;
}

int MGLFramebuffer_set_color_mask(MGLFramebuffer * self, PyObject * value, void * closure) {
    if (!PySequence_Check(value)) {
        PyErr_Format(PyExc_ValueError, "invalid color_mask");
        return -1;
    }
    if (!PySequence_Check(PySequence_GetItem(value, 0))) {
        PyObject * wrapped = Py_BuildValue("(N)", value);
        int res = MGLFramebuffer_set_color_mask(self, wrapped, closure);
        Py_DECREF(wrapped);
        return res;
    }
    for (int i = 0; i < self->num_color_attachments; ++i) {
        PyObject * item = PySequence_GetItem(value, i);
        if (!PySequence_Check(item) || PySequence_Size(item) != 4) {
            PyErr_Format(PyExc_ValueError, "invalid color_mask");
            return -1;
        }
        self->color_mask[i][0] = !!PyObject_IsTrue(PySequence_GetItem(item, 0));
        self->color_mask[i][1] = !!PyObject_IsTrue(PySequence_GetItem(item, 1));
        self->color_mask[i][2] = !!PyObject_IsTrue(PySequence_GetItem(item, 2));
        self->color_mask[i][3] = !!PyObject_IsTrue(PySequence_GetItem(item, 3));
        if (PyErr_Occurred()) {
            return -1;
        }
    }

    if (self->framebuffer_obj == self->context->bound_framebuffer->framebuffer_obj) {
        const GLMethods & gl = self->context->gl;

        for (int i = 0; i < self->num_color_attachments; ++i) {
            bool (*mask)[4] = self->color_mask;
            gl.ColorMaski(i, mask[i][0], mask[i][1], mask[i][2], mask[i][3]);
        }
    }

    return 0;
}

PyObject * MGLFramebuffer_get_depth_mask(MGLFramebuffer * self, void * closure) {
    return PyBool_FromLong(self->depth_mask);
}

int MGLFramebuffer_set_depth_mask(MGLFramebuffer * self, PyObject * value, void * closure) {
    self->depth_mask = !!PyObject_IsTrue(value);

    if (self->framebuffer_obj == self->context->bound_framebuffer->framebuffer_obj) {
        const GLMethods & gl = self->context->gl;
        gl.DepthMask(self->depth_mask);
    }

    return 0;
}

PyObject * MGLFramebuffer_get_bits(MGLFramebuffer * self, void * closure) {
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

    PyObject * red_obj = PyLong_FromLong(red_bits);
    PyObject * green_obj = PyLong_FromLong(green_bits);
    PyObject * blue_obj = PyLong_FromLong(blue_bits);
    PyObject * alpha_obj = PyLong_FromLong(alpha_bits);
    PyObject * depth_obj = PyLong_FromLong(depth_bits);
    PyObject * stencil_obj = PyLong_FromLong(stencil_bits);

    PyObject * result = PyDict_New();

    PyDict_SetItemString(result, "red", red_obj);
    PyDict_SetItemString(result, "green", green_obj);
    PyDict_SetItemString(result, "blue", blue_obj);
    PyDict_SetItemString(result, "alpha", alpha_obj);
    PyDict_SetItemString(result, "depth", depth_obj);
    PyDict_SetItemString(result, "stencil", stencil_obj);

    Py_DECREF(red_obj);
    Py_DECREF(green_obj);
    Py_DECREF(blue_obj);
    Py_DECREF(alpha_obj);
    Py_DECREF(depth_obj);
    Py_DECREF(stencil_obj);

    return result;
}

MGLProgram * MGLContext_program(MGLContext * self, PyObject * args, PyObject * kwargs) {
    const char * keywords[] = {
        "vertex_shader",
        "fragment_shader",
        "geometry_shader",
        "tess_control_shader",
        "tess_evaluation_shader",
        "varyings",
        "fragment_outputs",
        "varyings_capture_mode",
        NULL,
    };

    static PyObject * empty_tuple = PyTuple_New(0); // TODO: move

    PyObject * shaders[5] = {Py_None, Py_None, Py_None, Py_None, Py_None};
    PyObject * outputs = empty_tuple;
    PyObject * fragment_outputs = Py_None;
    const char * varyings_capture_mode = "interleaved";

    int args_ok = PyArg_ParseTuple(
        args,
        "|OOOOOOOs",
        &shaders[0],
        &shaders[1],
        &shaders[2],
        &shaders[3],
        &shaders[4],
        &outputs,
        &fragment_outputs,
        &varyings_capture_mode
    );

    if (!args_ok) {
        return 0;
    }

    int interleaved = !strcmp(varyings_capture_mode, "interleaved");

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
    program->extra = NULL;

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

        int shader_obj = gl.CreateShader(SHADER_TYPE[i]);

        if (!shader_obj) {
            MGLError_Set("cannot create shader");
            return 0;
        }

        if (PyUnicode_CheckExact(shaders[i])) {
            const char * source_str = PyUnicode_AsUTF8(shaders[i]);
            gl.ShaderSource(shader_obj, 1, &source_str, NULL);
            gl.CompileShader(shader_obj);
        } else if (PyBytes_CheckExact(shaders[i])) {
            unsigned * spv = (unsigned *)PyBytes_AsString(shaders[i]);
            if (spv[0] == 0x07230203) {
                int spv_length = (int)PyBytes_Size(shaders[i]);
                gl.ShaderBinary(1, (unsigned *)&shader_obj, GL_SHADER_BINARY_FORMAT_SPIR_V, spv, spv_length);
                gl.SpecializeShader(shader_obj, "main", 0, NULL, NULL);
            } else {
                const char * source_str = PyBytes_AsString(shaders[i]);
                gl.ShaderSource(shader_obj, 1, &source_str, NULL);
                gl.CompileShader(shader_obj);
            }
        } else {
            MGLError_Set("wrong shader source type");
            return NULL;
        }

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

    if (fragment_outputs != Py_None) {
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

        clean_glsl_name(name, name_len);

        PyObject * item = PyObject_CallMethod(
            helper, "make_attribute", "(siiii)",
            name, type, program->program_obj, location, array_length
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

        clean_glsl_name(name, name_len);

        if (location < 0) {
            continue;
        }

        PyObject * item = PyObject_CallMethod(
            helper, "make_uniform", "(siiiiO)",
            name, type, program->program_obj, location, array_length, self
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

        clean_glsl_name(name, name_len);

        PyObject * item = PyObject_CallMethod(
            helper, "make_uniform_block", "(siiiO)",
            name, program->program_obj, index, size, self
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

    program->is_transform = shaders[1] == Py_None;
    program->members = members_dict;

    Py_INCREF(program);
    return program;
}

PyObject * MGLProgram_release(MGLProgram * self, PyObject * args) {
    if (self->released) {
        Py_RETURN_NONE;
    }
    self->released = true;

    const GLMethods & gl = self->context->gl;
    gl.DeleteProgram(self->program_obj);

    Py_DECREF(self);
    Py_RETURN_NONE;
}

PyObject * MGLProgram_getitem(MGLProgram * self, PyObject * key) {
    PyObject * res = PyDict_GetItem(self->members, key);
    Py_XINCREF(res);
    return res;
}

int MGLProgram_setitem(MGLProgram * self, PyObject * key, PyObject * value) {
    PyObject * res = PyDict_GetItem(self->members, key);
    if (!res) {
        return -1;
    }
    return PyObject_SetAttrString(res, "value", value);
}

PyObject * MGLContext_query(MGLContext * self, PyObject * args) {
    int samples_passed;
    int any_samples_passed;
    int time_elapsed;
    int primitives_generated;

    int args_ok = PyArg_ParseTuple(
        args,
        "pppp",
        &samples_passed,
        &any_samples_passed,
        &time_elapsed,
        &primitives_generated
    );

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
    query->query_obj[SAMPLES_PASSED] = 0;
    query->query_obj[ANY_SAMPLES_PASSED] = 0;
    query->query_obj[TIME_ELAPSED] = 0;
    query->query_obj[PRIMITIVES_GENERATED] = 0;
    query->released = false;

    Py_INCREF(self);
    query->context = self;

    query->state = QUERY_INACTIVE;
    query->ended = false;

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

    return (PyObject *)query;
}

PyObject * MGLQuery_begin(MGLQuery * self, PyObject * args) {
    if (self->state != QUERY_INACTIVE) {
        MGLError_Set(self->state == QUERY_ACTIVE ? "this query is already running" : "this query is in conditional render mode");
        return NULL;
    }

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

    self->state = QUERY_ACTIVE;
    Py_RETURN_NONE;
}

PyObject * MGLQuery_end(MGLQuery * self, PyObject * args) {
    if (self->state != QUERY_ACTIVE) {
        MGLError_Set(self->state == QUERY_INACTIVE ? "this query was not started" : "this query is in conditional render mode");
        return NULL;
    }

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

    self->state = QUERY_INACTIVE;
    self->ended = true;
    Py_RETURN_NONE;
}

PyObject * MGLQuery_begin_render(MGLQuery * self, PyObject * args) {
    if (self->state != QUERY_INACTIVE) {
        MGLError_Set(self->state == QUERY_ACTIVE ? "this query was not stopped" : "this query is already in conditional render mode");
        return NULL;
    }

    const GLMethods & gl = self->context->gl;

    if (self->query_obj[ANY_SAMPLES_PASSED]) {
        gl.BeginConditionalRender(self->query_obj[ANY_SAMPLES_PASSED], GL_QUERY_NO_WAIT);
    } else if (self->query_obj[SAMPLES_PASSED]) {
        gl.BeginConditionalRender(self->query_obj[SAMPLES_PASSED], GL_QUERY_NO_WAIT);
    } else {
        MGLError_Set("no samples");
        return NULL;
    }

    self->state = QUERY_CONDITIONAL_RENDER;
    Py_RETURN_NONE;
}

PyObject * MGLQuery_end_render(MGLQuery * self, PyObject * args) {
    if (self->state != QUERY_CONDITIONAL_RENDER) {
        MGLError_Set("this query is not in conditional render mode");
        return NULL;
    }

    const GLMethods & gl = self->context->gl;
    gl.EndConditionalRender();
    self->state = QUERY_INACTIVE;
    Py_RETURN_NONE;
}

PyObject * MGLQuery_get_samples(MGLQuery * self) {
    if (!self->query_obj[SAMPLES_PASSED]) {
        MGLError_Set("query created without the samples_passed flag");
        return NULL;
    }

    if (self->state == QUERY_ACTIVE) {
        MGLError_Set("this query was not stopped");
        return NULL;
    }

    const GLMethods & gl = self->context->gl;

    unsigned samples = 0;
    if (self->ended) {
        gl.GetQueryObjectuiv(self->query_obj[SAMPLES_PASSED], GL_QUERY_RESULT, &samples);
    }

    return PyLong_FromUnsignedLong(samples);
}

PyObject * MGLQuery_get_primitives(MGLQuery * self) {
    if (!self->query_obj[PRIMITIVES_GENERATED]) {
        MGLError_Set("query created without the primitives_generated flag");
        return NULL;
    }

    if (self->state == QUERY_ACTIVE) {
        MGLError_Set("this query was not stopped");
        return NULL;
    }

    const GLMethods & gl = self->context->gl;

    unsigned primitives = 0;
    if (self->ended) {
        gl.GetQueryObjectuiv(self->query_obj[PRIMITIVES_GENERATED], GL_QUERY_RESULT, &primitives);
    }

    return PyLong_FromUnsignedLong(primitives);
}

PyObject * MGLQuery_get_elapsed(MGLQuery * self) {
    if (!self->query_obj[TIME_ELAPSED]) {
        MGLError_Set("query created without the time_elapsed flag");
        return NULL;
    }

    if (self->state == QUERY_ACTIVE) {
        MGLError_Set("this query was not stopped");
        return NULL;
    }

    const GLMethods & gl = self->context->gl;

    unsigned elapsed = 0;
    if (self->ended) {
        gl.GetQueryObjectuiv(self->query_obj[TIME_ELAPSED], GL_QUERY_RESULT, &elapsed);
    }

    return PyLong_FromUnsignedLong(elapsed);
}

MGLRenderbuffer * MGLContext_renderbuffer(MGLContext * self, PyObject * args, PyObject * kwargs) {
    const char * keywords[] = {"size", "components", "samples", "dtype", NULL};

    int width;
    int height;
    int components = 4;
    int samples = 0;
    const char * dtype = "f1";
    Py_ssize_t dtype_size = 2;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args,
        kwargs,
        "(II)|IIs#",
        (char **)keywords,
        &width,
        &height,
        &components,
        &samples,
        &dtype,
        &dtype_size
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

    renderbuffer->extra = NULL;
    renderbuffer->size_tuple = Py_BuildValue("(ii)", width, height);
    renderbuffer->width = width;
    renderbuffer->height = height;
    renderbuffer->components = components;
    renderbuffer->samples = samples;
    renderbuffer->data_type = data_type;
    renderbuffer->depth = false;

    Py_INCREF(self);
    renderbuffer->context = self;

    Py_INCREF(renderbuffer);
    return renderbuffer;
}

MGLRenderbuffer * MGLContext_depth_renderbuffer(MGLContext * self, PyObject * args, PyObject * kwargs) {
    const char * keywords[] = {"size", "samples", NULL};

    int width;
    int height;
    int samples = 0;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args,
        kwargs,
        "(II)I",
        (char **)keywords,
        &width,
        &height,
        &samples
    );

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

    renderbuffer->extra = NULL;
    renderbuffer->size_tuple = Py_BuildValue("(ii)", width, height);
    renderbuffer->width = width;
    renderbuffer->height = height;
    renderbuffer->components = 1;
    renderbuffer->samples = samples;
    renderbuffer->data_type = from_dtype("f4", 2);
    renderbuffer->depth = true;

    Py_INCREF(self);
    renderbuffer->context = self;

    Py_INCREF(renderbuffer);
    return renderbuffer;
}

PyObject * MGLRenderbuffer_release(MGLRenderbuffer * self, PyObject * args) {
    if (self->released) {
        Py_RETURN_NONE;
    }
    self->released = true;

    const GLMethods & gl = self->context->gl;
    gl.DeleteRenderbuffers(1, (GLuint *)&self->renderbuffer_obj);

    Py_DECREF(self);
    Py_RETURN_NONE;
}

PyObject * MGLContext_sampler(MGLContext * self, PyObject * args) {
    int args_ok = PyArg_ParseTuple(
        args,
        ""
    );

    if (!args_ok) {
        return 0;
    }

    const GLMethods & gl = self->gl;

    MGLSampler * sampler = PyObject_New(MGLSampler, MGLSampler_type);
    sampler->released = false;

    gl.GenSamplers(1, (GLuint *)&sampler->sampler_obj);

    sampler->min_filter = GL_LINEAR;
    sampler->mag_filter = GL_LINEAR;
    sampler->anisotropy = 0.0;
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

PyObject * MGLContext_memory_barrier(MGLContext * self, PyObject * args) {
    unsigned barriers = GL_ALL_BARRIER_BITS;
    int by_region = false;

    if (!PyArg_ParseTuple(args, "|Ip", &barriers, &by_region)) {
        return 0;
    }

    if (by_region && !self->gl.MemoryBarrierByRegion) {
        by_region = false;
    }

    if (by_region) {
        self->gl.MemoryBarrierByRegion(barriers);
    } else if (self->gl.MemoryBarrier) {
        self->gl.MemoryBarrier(barriers);
    }

    Py_RETURN_NONE;
}

PyObject * MGLSampler_use(MGLSampler * self, PyObject * args) {
    int index;

    int args_ok = PyArg_ParseTuple(
        args,
        "I",
        &index
    );

    if (!args_ok) {
        return 0;
    }

    const GLMethods & gl = self->context->gl;
    gl.BindSampler(index, self->sampler_obj);

    Py_RETURN_NONE;
}

PyObject * MGLSampler_clear(MGLSampler * self, PyObject * args) {
    int index;

    int args_ok = PyArg_ParseTuple(
        args,
        "I",
        &index
    );

    if (!args_ok) {
        return 0;
    }

    const GLMethods & gl = self->context->gl;
    gl.BindSampler(index, 0);

    Py_RETURN_NONE;
}

PyObject * MGLSampler_release(MGLSampler * self, PyObject * args) {
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
    return compare_func_to_string(self->compare_func);
}

int MGLSampler_set_compare_func(MGLSampler * self, PyObject * value) {
    const char * func = PyUnicode_AsUTF8(value);
    self->compare_func = compare_func_from_string(func);

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
    if (self->context->max_anisotropy == 0) return 0;
    self->anisotropy = (float)MGL_MIN(MGL_MAX(PyFloat_AsDouble(value), 1.0), self->context->max_anisotropy);

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

PyObject * MGLScope_begin(MGLScope * self, PyObject * args) {
    int args_ok = PyArg_ParseTuple(
        args,
        ""
    );

    if (!args_ok) {
        return 0;
    }

    const GLMethods & gl = self->context->gl;
    const int & flags = self->enable_flags;

    self->old_enable_flags = self->context->enable_flags;
    self->context->enable_flags = self->enable_flags;

    Py_XDECREF(MGLFramebuffer_use(self->framebuffer, NULL));

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

PyObject * MGLScope_end(MGLScope * self, PyObject * args) {
    int args_ok = PyArg_ParseTuple(
        args,
        ""
    );

    if (!args_ok) {
        return 0;
    }

    const GLMethods & gl = self->context->gl;
    const int & flags = self->old_enable_flags;

    self->context->enable_flags = self->old_enable_flags;

    Py_XDECREF(MGLFramebuffer_use(self->old_framebuffer, NULL));

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

PyObject * MGLScope_release(MGLScope * self, PyObject * args) {
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

    texture->extra = NULL;
    texture->size_tuple = Py_BuildValue("(ii)", width, height);
    texture->width = width;
    texture->height = height;
    texture->components = components;
    texture->samples = samples;
    texture->data_type = data_type;

    texture->max_level = 0;
    texture->compare_func = 0;
    texture->anisotropy = 0.0;
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

    int args_ok = PyArg_ParseTuple(
        args,
        "(II)OII",
        &width,
        &height,
        &data,
        &samples,
        &alignment
    );

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

    texture->extra = NULL;
    texture->size_tuple = Py_BuildValue("(ii)", width, height);
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

    int args_ok = PyArg_ParseTuple(
        args,
        "I(II)IIs#",
        &glo,
        &width,
        &height,
        &components,
        &samples,
        &dtype,
        &dtype_size
    );

    if (!args_ok) {
        return NULL;
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
    texture->extra = NULL;
    texture->size_tuple = Py_BuildValue("(ii)", width, height);
    texture->width = width;
    texture->height = height;
    texture->components = components;
    texture->samples = samples;
    texture->data_type = data_type;

    texture->max_level = 0;
    texture->compare_func = 0;
    texture->anisotropy = 0.0;
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

    int args_ok = PyArg_ParseTuple(
        args,
        "II",
        &level,
        &alignment
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "OIIn",
        &data,
        &level,
        &alignment,
        &write_offset
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "OOII",
        &data,
        &viewport,
        &level,
        &alignment
    );

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

    int pixel_type = self->data_type->gl_type;
    int format = self->depth ? GL_DEPTH_COMPONENT : self->data_type->base_format[self->components];

    if (Py_TYPE(data) == MGLBuffer_type) {

        MGLBuffer * buffer = (MGLBuffer *)data;

        const GLMethods & gl = self->context->gl;

        gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer->buffer_obj);
        gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
        gl.BindTexture(GL_TEXTURE_2D, self->texture_obj);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.TexSubImage2D(GL_TEXTURE_2D, level, x, y, width, height, format, pixel_type, 0);
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
        gl.BindTexture(GL_TEXTURE_2D, self->texture_obj);
        gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
        gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        gl.TexSubImage2D(GL_TEXTURE_2D, level, x, y, width, height, format, pixel_type, buffer_view.buf);

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

    int args_ok = PyArg_ParseTuple(
        args,
        "IppII",
        &unit,
        &read,
        &write,
        &level,
        &format
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "I",
        &index
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "II",
        &base,
        &max
    );

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

PyObject * MGLTexture_get_handle(MGLTexture * self, PyObject * args) {
    int resident = true;

    if(!PyArg_ParseTuple(args, "|p", &resident)) {
        return NULL;
    }

    const GLMethods & gl = self->context->gl;

    unsigned long long handle = gl.GetTextureHandleARB(self->texture_obj);
    if (resident) {
        gl.MakeTextureHandleResidentARB(handle);
    } else {
        gl.MakeTextureHandleNonResidentARB(handle);
    }

    return PyLong_FromUnsignedLongLong(handle);
}

PyObject * MGLTexture_release(MGLTexture * self, PyObject * args) {
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

PyObject * MGLTexture_get_swizzle(MGLTexture * self, void * closure) {

    if (self->depth) {
        MGLError_Set("cannot get swizzle of depth textures");
        return 0;
    }

    int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(texture_target, self->texture_obj);

    int swizzle_r = 0;
    int swizzle_g = 0;
    int swizzle_b = 0;
    int swizzle_a = 0;

    gl.GetTexParameteriv(texture_target, GL_TEXTURE_SWIZZLE_R, &swizzle_r);
    gl.GetTexParameteriv(texture_target, GL_TEXTURE_SWIZZLE_G, &swizzle_g);
    gl.GetTexParameteriv(texture_target, GL_TEXTURE_SWIZZLE_B, &swizzle_b);
    gl.GetTexParameteriv(texture_target, GL_TEXTURE_SWIZZLE_A, &swizzle_a);

    char swizzle[5] = {
        char_from_swizzle(swizzle_r),
        char_from_swizzle(swizzle_g),
        char_from_swizzle(swizzle_b),
        char_from_swizzle(swizzle_a),
        0,
    };

    return PyUnicode_FromStringAndSize(swizzle, 4);
}

int MGLTexture_set_swizzle(MGLTexture * self, PyObject * value, void * closure) {
    const char * swizzle = PyUnicode_AsUTF8(value);

    if (self->depth) {
        MGLError_Set("cannot set swizzle for depth textures");
        return -1;
    }

    if (!swizzle[0]) {
        MGLError_Set("the swizzle is empty");
        return -1;
    }

    int tex_swizzle[4] = {-1, -1, -1, -1};

    for (int i = 0; swizzle[i]; ++i) {
        if (i > 3) {
            MGLError_Set("the swizzle is too long");
            return -1;
        }

        tex_swizzle[i] = swizzle_from_char(swizzle[i]);

        if (tex_swizzle[i] == -1) {
            MGLError_Set("'%c' is not a valid swizzle parameter", swizzle[i]);
            return -1;
        }
    }

    int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(texture_target, self->texture_obj);

    gl.TexParameteri(texture_target, GL_TEXTURE_SWIZZLE_R, tex_swizzle[0]);
    if (tex_swizzle[1] != -1) {
        gl.TexParameteri(texture_target, GL_TEXTURE_SWIZZLE_G, tex_swizzle[1]);
        if (tex_swizzle[2] != -1) {
            gl.TexParameteri(texture_target, GL_TEXTURE_SWIZZLE_B, tex_swizzle[2]);
            if (tex_swizzle[3] != -1) {
                gl.TexParameteri(texture_target, GL_TEXTURE_SWIZZLE_A, tex_swizzle[3]);
            }
        }
    }

    return 0;
}

PyObject * MGLTexture_get_compare_func(MGLTexture * self) {
    if (!self->depth) {
        MGLError_Set("only depth textures have compare_func");
        return 0;
    }

    return compare_func_to_string(self->compare_func);
}

int MGLTexture_set_compare_func(MGLTexture * self, PyObject * value) {
    if (!self->depth) {
        MGLError_Set("only depth textures have compare_func");
        return -1;
    }

    int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    const char * func = PyUnicode_AsUTF8(value);

    if (PyErr_Occurred()) {
        return -1;
    }

    self->compare_func = compare_func_from_string(func);

    const GLMethods & gl = self->context->gl;
    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(texture_target, self->texture_obj);
    if (self->compare_func == 0) {
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
    if (self->context->max_anisotropy == 0) return 0;
    self->anisotropy = (float)MGL_MIN(MGL_MAX(PyFloat_AsDouble(value), 1.0), self->context->max_anisotropy);
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

    int args_ok = PyArg_ParseTuple(
        args,
        "(III)IOIs#",
        &width,
        &height,
        &depth,
        &components,
        &data,
        &alignment,
        &dtype,
        &dtype_size
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

    int args_ok = PyArg_ParseTuple(
        args,
        "I",
        &alignment
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "OIn",
        &data,
        &alignment,
        &write_offset
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "OOI",
        &data,
        &viewport,
        &alignment
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "IppII",
        &unit,
        &read,
        &write,
        &level,
        &format
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "I",
        &index
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "II",
        &base,
        &max
    );

    if (!args_ok) {
        return 0;
    }

    if (base > self->max_level) {
        MGLError_Set("invalid base");
        return 0;
    }

    int texture_target = GL_TEXTURE_3D;

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
PyObject * MGLTexture3D_get_handle(MGLTexture3D * self, PyObject * args) {
    int resident = true;

    if(!PyArg_ParseTuple(args, "|p", &resident)) {
        return NULL;
    }

    const GLMethods & gl = self->context->gl;

    unsigned long long handle = gl.GetTextureHandleARB(self->texture_obj);
    if (resident) {
        gl.MakeTextureHandleResidentARB(handle);
    } else {
        gl.MakeTextureHandleNonResidentARB(handle);
    }

    return PyLong_FromUnsignedLongLong(handle);
}

PyObject * MGLTexture3D_release(MGLTexture3D * self, PyObject * args) {
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

PyObject * MGLTexture3D_get_swizzle(MGLTexture3D * self, void * closure) {

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

    int swizzle_r = 0;
    int swizzle_g = 0;
    int swizzle_b = 0;
    int swizzle_a = 0;

    gl.GetTexParameteriv(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_R, &swizzle_r);
    gl.GetTexParameteriv(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_G, &swizzle_g);
    gl.GetTexParameteriv(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_B, &swizzle_b);
    gl.GetTexParameteriv(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_A, &swizzle_a);

    char swizzle[5] = {
        char_from_swizzle(swizzle_r),
        char_from_swizzle(swizzle_g),
        char_from_swizzle(swizzle_b),
        char_from_swizzle(swizzle_a),
        0,
    };

    return PyUnicode_FromStringAndSize(swizzle, 4);
}

int MGLTexture3D_set_swizzle(MGLTexture3D * self, PyObject * value, void * closure) {
    const char * swizzle = PyUnicode_AsUTF8(value);

    if (!swizzle[0]) {
        MGLError_Set("the swizzle is empty");
        return -1;
    }

    int tex_swizzle[4] = {-1, -1, -1, -1};

    for (int i = 0; swizzle[i]; ++i) {
        if (i > 3) {
            MGLError_Set("the swizzle is too long");
            return -1;
        }

        tex_swizzle[i] = swizzle_from_char(swizzle[i]);

        if (tex_swizzle[i] == -1) {
            MGLError_Set("'%c' is not a valid swizzle parameter", swizzle[i]);
            return -1;
        }
    }


    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

    gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_R, tex_swizzle[0]);
    if (tex_swizzle[1] != -1) {
        gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_G, tex_swizzle[1]);
        if (tex_swizzle[2] != -1) {
            gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_B, tex_swizzle[2]);
            if (tex_swizzle[3] != -1) {
                gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_A, tex_swizzle[3]);
            }
        }
    }

    return 0;
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

    int args_ok = PyArg_ParseTuple(
        args,
        "(III)IOIs#",
        &width,
        &height,
        &layers,
        &components,
        &data,
        &alignment,
        &dtype,
        &dtype_size
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
    texture->anisotropy = 0.0;

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

    int args_ok = PyArg_ParseTuple(
        args,
        "I",
        &alignment
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "OIn",
        &data,
        &alignment,
        &write_offset
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "OOI",
        &data,
        &viewport,
        &alignment
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "IppII",
        &unit,
        &read,
        &write,
        &level,
        &format
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "I",
        &index
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "II",
        &base,
        &max
    );

    if (!args_ok) {
        return 0;
    }

    if (base > self->max_level) {
        MGLError_Set("invalid base");
        return 0;
    }

    int texture_target = GL_TEXTURE_2D_ARRAY;

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
PyObject * MGLTextureArray_get_handle(MGLTextureArray * self, PyObject * args) {
    int resident = true;

    if(!PyArg_ParseTuple(args, "|p", &resident)) {
        return NULL;
    }

    const GLMethods & gl = self->context->gl;

    unsigned long long handle = gl.GetTextureHandleARB(self->texture_obj);
    if (resident) {
        gl.MakeTextureHandleResidentARB(handle);
    } else {
        gl.MakeTextureHandleNonResidentARB(handle);
    }

    return PyLong_FromUnsignedLongLong(handle);
}

PyObject * MGLTextureArray_release(MGLTextureArray * self, PyObject * args) {
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

PyObject * MGLTextureArray_get_swizzle(MGLTextureArray * self, void * closure) {

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_2D_ARRAY, self->texture_obj);

    int swizzle_r = 0;
    int swizzle_g = 0;
    int swizzle_b = 0;
    int swizzle_a = 0;

    gl.GetTexParameteriv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_R, &swizzle_r);
    gl.GetTexParameteriv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_G, &swizzle_g);
    gl.GetTexParameteriv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_B, &swizzle_b);
    gl.GetTexParameteriv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_A, &swizzle_a);

    char swizzle[5] = {
        char_from_swizzle(swizzle_r),
        char_from_swizzle(swizzle_g),
        char_from_swizzle(swizzle_b),
        char_from_swizzle(swizzle_a),
        0,
    };

    return PyUnicode_FromStringAndSize(swizzle, 4);
}

int MGLTextureArray_set_swizzle(MGLTextureArray * self, PyObject * value, void * closure) {
    const char * swizzle = PyUnicode_AsUTF8(value);

    if (!swizzle[0]) {
        MGLError_Set("the swizzle is empty");
        return -1;
    }

    int tex_swizzle[4] = {-1, -1, -1, -1};

    for (int i = 0; swizzle[i]; ++i) {
        if (i > 3) {
            MGLError_Set("the swizzle is too long");
            return -1;
        }

        tex_swizzle[i] = swizzle_from_char(swizzle[i]);

        if (tex_swizzle[i] == -1) {
            MGLError_Set("'%c' is not a valid swizzle parameter", swizzle[i]);
            return -1;
        }
    }


    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_2D_ARRAY, self->texture_obj);

    gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_R, tex_swizzle[0]);
    if (tex_swizzle[1] != -1) {
        gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_G, tex_swizzle[1]);
        if (tex_swizzle[2] != -1) {
            gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_B, tex_swizzle[2]);
            if (tex_swizzle[3] != -1) {
                gl.TexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_A, tex_swizzle[3]);
            }
        }
    }

    return 0;
}

PyObject * MGLTextureArray_get_anisotropy(MGLTextureArray * self) {
    return PyFloat_FromDouble(self->anisotropy);
}

int MGLTextureArray_set_anisotropy(MGLTextureArray * self, PyObject * value) {
    if (self->context->max_anisotropy == 0) return 0;
    self->anisotropy = (float)MGL_MIN(MGL_MAX(PyFloat_AsDouble(value), 1.0), self->context->max_anisotropy);

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
    texture->anisotropy = 0.0;

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

    int args_ok = PyArg_ParseTuple(
        args,
        "iI",
        &face,
        &alignment
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "OiIn",
        &data,
        &face,
        &alignment,
        &write_offset
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "iOOI",
        &face,
        &data,
        &viewport,
        &alignment
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "IppII",
        &unit,
        &read,
        &write,
        &level,
        &format
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "I",
        &index
    );

    if (!args_ok) {
        return 0;
    }

    const GLMethods & gl = self->context->gl;
    gl.ActiveTexture(GL_TEXTURE0 + index);
    gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);

    Py_RETURN_NONE;
}
PyObject * MGLTextureCube_get_handle(MGLTextureCube * self, PyObject * args) {
    int resident = true;

    if(!PyArg_ParseTuple(args, "|p", &resident)) {
        return NULL;
    }

    const GLMethods & gl = self->context->gl;

    unsigned long long handle = gl.GetTextureHandleARB(self->texture_obj);
    if (resident) {
        gl.MakeTextureHandleResidentARB(handle);
    } else {
        gl.MakeTextureHandleNonResidentARB(handle);
    }

    return PyLong_FromUnsignedLongLong(handle);
}

PyObject * MGLTextureCube_build_mipmaps(MGLTextureCube * self, PyObject * args) {
    int base = 0;
    int max = 1000;

    int args_ok = PyArg_ParseTuple(
        args,
        "II",
        &base,
        &max
    );

    if (!args_ok) {
        return 0;
    }

    if (base > self->max_level) {
        MGLError_Set("invalid base");
        return 0;
    }

    int texture_target = GL_TEXTURE_CUBE_MAP;

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

PyObject * MGLTextureCube_release(MGLTextureCube * self, PyObject * args) {
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

PyObject * MGLTextureCube_get_swizzle(MGLTextureCube * self, void * closure) {

    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);

    int swizzle_r = 0;
    int swizzle_g = 0;
    int swizzle_b = 0;
    int swizzle_a = 0;

    gl.GetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_R, &swizzle_r);
    gl.GetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_G, &swizzle_g);
    gl.GetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_B, &swizzle_b);
    gl.GetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_A, &swizzle_a);

    char swizzle[5] = {
        char_from_swizzle(swizzle_r),
        char_from_swizzle(swizzle_g),
        char_from_swizzle(swizzle_b),
        char_from_swizzle(swizzle_a),
        0,
    };

    return PyUnicode_FromStringAndSize(swizzle, 4);
}

int MGLTextureCube_set_swizzle(MGLTextureCube * self, PyObject * value, void * closure) {
    const char * swizzle = PyUnicode_AsUTF8(value);

    if (!swizzle[0]) {
        MGLError_Set("the swizzle is empty");
        return -1;
    }

    int tex_swizzle[4] = {-1, -1, -1, -1};

    for (int i = 0; swizzle[i]; ++i) {
        if (i > 3) {
            MGLError_Set("the swizzle is too long");
            return -1;
        }

        tex_swizzle[i] = swizzle_from_char(swizzle[i]);

        if (tex_swizzle[i] == -1) {
            MGLError_Set("'%c' is not a valid swizzle parameter", swizzle[i]);
            return -1;
        }
    }


    const GLMethods & gl = self->context->gl;

    gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
    gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);

    gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_R, tex_swizzle[0]);
    if (tex_swizzle[1] != -1) {
        gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_G, tex_swizzle[1]);
        if (tex_swizzle[2] != -1) {
            gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_B, tex_swizzle[2]);
            if (tex_swizzle[3] != -1) {
                gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_A, tex_swizzle[3]);
            }
        }
    }

    return 0;
}

PyObject * MGLTextureCube_get_anisotropy(MGLTextureCube * self) {
    return PyFloat_FromDouble(self->anisotropy);
}

int MGLTextureCube_set_anisotropy(MGLTextureCube * self, PyObject * value) {
    if (self->context->max_anisotropy == 0) return 0;
    self->anisotropy = (float)MGL_MIN(MGL_MAX(PyFloat_AsDouble(value), 1.0), self->context->max_anisotropy);

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

    int args_ok = PyArg_ParseTuple(
        args,
        "O!OOIp",
        MGLProgram_type,
        &program,
        &content,
        &index_buffer,
        &index_element_size,
        &skip_errors
    );

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

            PyObject * attribute_location_py = PyObject_GetAttrString(attribute, "location");
            PyObject * attribute_rows_length_py = PyObject_GetAttrString(attribute, "rows_length");
            PyObject * attribute_scalar_type_py = PyObject_GetAttrString(attribute, "scalar_type");
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

    int args_ok = PyArg_ParseTuple(
        args,
        "IIII",
        &mode,
        &vertices,
        &first,
        &instances
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "O!III",
        MGLBuffer_type,
        &buffer,
        &mode,
        &count,
        &first
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "O!IIIII",
        &PyList_Type,
        &outputs,
        &mode,
        &vertices,
        &first,
        &instances,
        &buffer_offset
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "IsO!snIIp",
        &location,
        &type,
        MGLBuffer_type,
        &buffer,
        &format,
        &offset,
        &stride,
        &divisor,
        &normalize
    );

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

PyObject * MGLVertexArray_release(MGLVertexArray * self, PyObject * args) {
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

int MGLVertexArray_set_index_buffer(MGLVertexArray * self, PyObject * value, void * closure) {
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

PyObject * MGLVertexArray_get_vertices(MGLVertexArray * self, void * closure) {
    return PyLong_FromLong(self->num_vertices);
}

int MGLVertexArray_set_vertices(MGLVertexArray * self, PyObject * value, void * closure) {
    int vertices = PyLong_AsUnsignedLong(value);

    if (PyErr_Occurred()) {
        MGLError_Set("invalid value for vertices");
        return -1;
    }

    self->num_vertices = vertices;

    return 0;
}

PyObject * MGLVertexArray_get_instances(MGLVertexArray * self, void * closure) {
    return PyLong_FromLong(self->num_instances);
}

int MGLVertexArray_set_instances(MGLVertexArray * self, PyObject * value, void * closure) {
    int instances = PyLong_AsUnsignedLong(value);

    if (PyErr_Occurred()) {
        MGLError_Set("invalid value for instances");
        return -1;
    }

    self->num_instances = instances;

    return 0;
}

int MGLVertexArray_set_subroutines(MGLVertexArray * self, PyObject * value, void * closure) {
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

PyObject * MGLContext_enable_only(MGLContext * self, PyObject * args) {
    int flags;

    int args_ok = PyArg_ParseTuple(
        args,
        "i",
        &flags
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "i",
        &flags
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "i",
        &flags
    );

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

    int args_ok = PyArg_ParseTuple(
        args,
        "i",
        &value
    );

    if (!args_ok) {
        return 0;
    }

    self->gl.Enable(value);
    Py_RETURN_NONE;
}

PyObject * MGLContext_disable_direct(MGLContext * self, PyObject * args) {
    int value;

    int args_ok = PyArg_ParseTuple(
        args,
        "i",
        &value
    );

    if (!args_ok) {
        return 0;
    }

    self->gl.Disable(value);
    Py_RETURN_NONE;
}

PyObject * MGLContext_finish(MGLContext * self, PyObject * args) {
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

    int args_ok = PyArg_ParseTuple(
        args,
        "OO!",
        &dst,
        MGLFramebuffer_type,
        &src
    );

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

        if (dst_framebuffer->num_color_attachments != src->num_color_attachments) {
            MGLError_Set("Destination and source framebuffers have different number of color attachments!");
            return 0;
        }

        int num_color_attachments = dst_framebuffer->num_color_attachments;

        gl.BindFramebuffer(GL_READ_FRAMEBUFFER, src->framebuffer_obj);
        gl.BindFramebuffer(GL_DRAW_FRAMEBUFFER, dst_framebuffer->framebuffer_obj);

        for (int i = 0; i < num_color_attachments; ++i) {
            gl.ReadBuffer(GL_COLOR_ATTACHMENT0 + i);
            gl.DrawBuffer(GL_COLOR_ATTACHMENT0 + i);
            gl.BlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
        unsigned draw_buffers[64] = {};
        for (int i = 0; i < dst_framebuffer->num_color_attachments; ++i) {
            draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
        }

        gl.DrawBuffers(dst_framebuffer->num_color_attachments, draw_buffers);
        gl.ReadBuffer(src->num_color_attachments ? GL_COLOR_ATTACHMENT0 : GL_NONE);

        gl.BindFramebuffer(GL_FRAMEBUFFER, self->bound_framebuffer->framebuffer_obj);

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

MGLFramebuffer * MGLContext_detect_framebuffer(MGLContext * self, PyObject * args) {
    PyObject * glo;

    if (!PyArg_ParseTuple(args, "O", &glo)) {
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
        Py_INCREF(self->default_framebuffer);
        return self->default_framebuffer;
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

    framebuffer->num_color_attachments = num_color_attachments;
    memset(framebuffer->color_mask, 1, sizeof(framebuffer->color_mask));
    framebuffer->depth_mask = true;

    framebuffer->context = self;

    framebuffer->viewport = {0, 0, width, height};

    framebuffer->scissor_enabled = false;
    framebuffer->scissor = {0, 0, width, height};

    framebuffer->extra = NULL;
    framebuffer->size_tuple = Py_BuildValue("(ii)", width, height);
    framebuffer->width = width;
    framebuffer->height = height;
    framebuffer->dynamic = true;

    gl.BindFramebuffer(GL_FRAMEBUFFER, bound_framebuffer);

    Py_INCREF(framebuffer);
    return framebuffer;
}

PyObject * MGLContext_clear_samplers(MGLContext * self, PyObject * args) {
    int start;
    int end;

    int args_ok = PyArg_ParseTuple(
        args,
        "ii",
        &start,
        &end
    );

    if (!args_ok) {
        return 0;
    }

    start = MGL_MAX(start, 0);
    if (end == -1) {
        end = self->max_texture_units;
    } else {
        end = MGL_MIN(end, self->max_texture_units);
    }

    const GLMethods & gl = self->gl;

    for(int i = start; i < end; i++) {
        gl.BindSampler(i, 0);
    }

    Py_RETURN_NONE;
}

PyObject * MGLContext_enter(MGLContext * self, PyObject * args) {
    return PyObject_CallMethod(self->ctx, "__enter__", NULL);
}

PyObject * MGLContext_exit(MGLContext * self, PyObject * args) {
    return PyObject_CallMethod(self->ctx, "__exit__", NULL);
}

PyObject * MGLContext_release(MGLContext * self, PyObject * args) {
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

PyObject * MGLContext_get_storage_block_binding(MGLContext * self, PyObject * args) {
    int program_obj;
    int index;
    if (!PyArg_ParseTuple(args, "II", &program_obj, &index)) {
        return NULL;
    }
    int binding = 0;
    GLenum prop = GL_BUFFER_BINDING;
    self->gl.GetProgramResourceiv(program_obj, GL_SHADER_STORAGE_BLOCK, index, 1, &prop, 1, NULL, &binding);
    return PyLong_FromLong(binding);
}

PyObject * MGLContext_set_storage_block_binding(MGLContext * self, PyObject * args) {
    int program_obj;
    int index;
    int binding;
    if (!PyArg_ParseTuple(args, "III", &program_obj, &index, &binding)) {
        return NULL;
    }
    self->gl.ShaderStorageBlockBinding(program_obj, index, binding);
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

PyObject * MGLContext_set_uniform_handle(MGLContext * self, PyObject * args) {
    int program_obj;
    int location;
    unsigned long long handle;

    if (!PyArg_ParseTuple(args, "IIK", &program_obj, &location, &handle)) {
        return NULL;
    }

    self->gl.ProgramUniformHandleui64ARB(program_obj, location, handle);
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
    return compare_func_to_string(self->depth_func);
}

int MGLContext_set_depth_func(MGLContext * self, PyObject * value) {
    const char * func = PyUnicode_AsUTF8(value);

    if (PyErr_Occurred()) {
        return -1;
    }

    int depth_func = compare_func_from_string(func);

    if (!depth_func) {
        return -1;
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
        static PyObject * res_cw = PyUnicode_FromString("cw");
        Py_INCREF(res_cw);
        return res_cw;
    }
    static PyObject * res_ccw = PyUnicode_FromString("ccw");
    Py_INCREF(res_ccw);
    return res_ccw;
}

int MGLContext_set_front_face(MGLContext * self, PyObject * value) {
    const char * str = PyUnicode_AsUTF8(value);

    if (!strcmp(str, "cw")) {
        self->front_face = GL_CW;
    } else if (!strcmp(str, "ccw")) {
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
        static PyObject * res_cw = PyUnicode_FromString("front");
        Py_INCREF(res_cw);
        return res_cw;
    }
    else if (self->front_face == GL_BACK) {
        static PyObject * res_cw = PyUnicode_FromString("back");
        Py_INCREF(res_cw);
        return res_cw;
    }
    static PyObject * res_ccw = PyUnicode_FromString("front_and_back");
    Py_INCREF(res_ccw);
    return res_ccw;
}

int MGLContext_set_cull_face(MGLContext * self, PyObject * value) {
    const char * str = PyUnicode_AsUTF8(value);

    if (!strcmp(str, "front")) {
        self->cull_face = GL_FRONT;
    } else if (!strcmp(str, "back")) {
        self->cull_face = GL_BACK;
    } else if (!strcmp(str, "front_and_back")) {
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

    if (!patch_vertices) {
        return -1;
    }

    self->gl.PatchParameteri(GL_PATCH_VERTICES, patch_vertices);

    return 0;
}

PyObject * MGLContext_get_error(MGLContext * self, void * closure) {
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

PyObject * MGLContext_get_version_code(MGLContext * self, void * closure) {
    return PyLong_FromLong(self->version_code);
}

PyObject * MGLContext_get_extensions(MGLContext * self, void * closure) {
    Py_INCREF(self->extensions);
    return self->extensions;
}

PyObject * MGLContext_get_context(MGLContext * self, void * closure) {
    Py_INCREF(self->ctx);
    return self->ctx;
}

void set_key(PyObject * dict, const char * key, PyObject * value) {
    PyDict_SetItemString(dict, key, value);
    Py_DECREF(value);
}

void set_info_str(MGLContext * self, PyObject * info, const char * name, GLenum param) {
    const char * ptr = (const char *)self->gl.GetString(param);
    set_key(info, name, PyUnicode_FromString(ptr ? ptr : ""));
}

void set_info_bool(MGLContext * self, PyObject * info, const char * name, GLenum param) {
    int value = 0;
    self->gl.GetBooleanv(param, (unsigned char *)&value);
    PyDict_SetItemString(info, name, value ? Py_True : Py_False);
}

void set_info_float(MGLContext * self, PyObject * info, const char * name, GLenum param) {
    float value = 0.0f;
    self->gl.GetFloatv(param, &value);
    set_key(info, name, PyFloat_FromDouble(value));
}

void set_info_int(MGLContext * self, PyObject * info, const char * name, GLenum param) {
    int value = 0;
    self->gl.GetIntegerv(param, &value);
    set_key(info, name, PyLong_FromLong(value));
}

void set_info_int64(MGLContext * self, PyObject * info, const char * name, GLenum param) {
    long long value = 0;
    if (self->gl.GetInteger64v) {
        self->gl.GetInteger64v(param, (GLint64 *)&value);
    }
    set_key(info, name, PyLong_FromLongLong(value));
}

void set_info_float_range(MGLContext * self, PyObject * info, const char * name, GLenum param) {
    float value[2] = {};
    self->gl.GetFloatv(param, value);
    set_key(info, name, Py_BuildValue("(ff)", value[0], value[1]));
}

void set_info_int_range(MGLContext * self, PyObject * info, const char * name, GLenum param) {
    int value[2] = {};
    self->gl.GetIntegerv(param, value);
    set_key(info, name, Py_BuildValue("(ii)", value[0], value[1]));
}

void set_info_int_xyz(MGLContext * self, PyObject * info, const char * name, GLenum param) {
    int value[3] = {};
    if (self->gl.GetIntegeri_v) {
        self->gl.GetIntegeri_v(param, 0, &value[0]);
        self->gl.GetIntegeri_v(param, 1, &value[1]);
        self->gl.GetIntegeri_v(param, 2, &value[2]);
    }
    set_key(info, name, Py_BuildValue("(iii)", value[0], value[1], value[2]));
}

PyObject * MGLContext_get_info(MGLContext * self) {
    PyObject * info = PyDict_New();

    set_info_str(self, info, "GL_VENDOR", GL_VENDOR);
    set_info_str(self, info, "GL_RENDERER", GL_RENDERER);
    set_info_str(self, info, "GL_VERSION", GL_VERSION);
    set_info_float_range(self, info, "GL_POINT_SIZE_RANGE", GL_POINT_SIZE_RANGE);
    set_info_float_range(self, info, "GL_SMOOTH_LINE_WIDTH_RANGE", GL_SMOOTH_LINE_WIDTH_RANGE);
    set_info_float_range(self, info, "GL_ALIASED_LINE_WIDTH_RANGE", GL_ALIASED_LINE_WIDTH_RANGE);
    set_info_float(self, info, "GL_POINT_FADE_THRESHOLD_SIZE", GL_POINT_FADE_THRESHOLD_SIZE);
    set_info_float(self, info, "GL_POINT_SIZE_GRANULARITY", GL_POINT_SIZE_GRANULARITY);
    set_info_float(self, info, "GL_SMOOTH_LINE_WIDTH_GRANULARITY", GL_SMOOTH_LINE_WIDTH_GRANULARITY);
    set_info_float(self, info, "GL_MIN_PROGRAM_TEXEL_OFFSET", GL_MIN_PROGRAM_TEXEL_OFFSET);
    set_info_float(self, info, "GL_MAX_PROGRAM_TEXEL_OFFSET", GL_MAX_PROGRAM_TEXEL_OFFSET);
    set_info_int(self, info, "GL_MINOR_VERSION", GL_MINOR_VERSION);
    set_info_int(self, info, "GL_MAJOR_VERSION", GL_MAJOR_VERSION);
    set_info_int(self, info, "GL_SAMPLE_BUFFERS", GL_SAMPLE_BUFFERS);
    set_info_int(self, info, "GL_SUBPIXEL_BITS", GL_SUBPIXEL_BITS);
    set_info_int(self, info, "GL_CONTEXT_PROFILE_MASK", GL_CONTEXT_PROFILE_MASK);
    set_info_int(self, info, "GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT", GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT);
    set_info_bool(self, info, "GL_DOUBLEBUFFER", GL_DOUBLEBUFFER);
    set_info_bool(self, info, "GL_STEREO", GL_STEREO);
    set_info_int_range(self, info, "GL_MAX_VIEWPORT_DIMS", GL_MAX_VIEWPORT_DIMS);
    set_info_int(self, info, "GL_MAX_3D_TEXTURE_SIZE", GL_MAX_3D_TEXTURE_SIZE);
    set_info_int(self, info, "GL_MAX_ARRAY_TEXTURE_LAYERS", GL_MAX_ARRAY_TEXTURE_LAYERS);
    set_info_int(self, info, "GL_MAX_CLIP_DISTANCES", GL_MAX_CLIP_DISTANCES);
    set_info_int(self, info, "GL_MAX_COLOR_ATTACHMENTS", GL_MAX_COLOR_ATTACHMENTS);
    set_info_int(self, info, "GL_MAX_COLOR_TEXTURE_SAMPLES", GL_MAX_COLOR_TEXTURE_SAMPLES);
    set_info_int(self, info, "GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS", GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS);
    set_info_int(self, info, "GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS", GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS);
    set_info_int(self, info, "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
    set_info_int(self, info, "GL_MAX_COMBINED_UNIFORM_BLOCKS", GL_MAX_COMBINED_UNIFORM_BLOCKS);
    set_info_int(self, info, "GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS", GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS);
    set_info_int(self, info, "GL_MAX_CUBE_MAP_TEXTURE_SIZE", GL_MAX_CUBE_MAP_TEXTURE_SIZE);
    set_info_int(self, info, "GL_MAX_DEPTH_TEXTURE_SAMPLES", GL_MAX_DEPTH_TEXTURE_SAMPLES);
    set_info_int(self, info, "GL_MAX_DRAW_BUFFERS", GL_MAX_DRAW_BUFFERS);
    set_info_int(self, info, "GL_MAX_DUAL_SOURCE_DRAW_BUFFERS", GL_MAX_DUAL_SOURCE_DRAW_BUFFERS);
    set_info_int(self, info, "GL_MAX_ELEMENTS_INDICES", GL_MAX_ELEMENTS_INDICES);
    set_info_int(self, info, "GL_MAX_ELEMENTS_VERTICES", GL_MAX_ELEMENTS_VERTICES);
    set_info_int(self, info, "GL_MAX_FRAGMENT_INPUT_COMPONENTS", GL_MAX_FRAGMENT_INPUT_COMPONENTS);
    set_info_int(self, info, "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS", GL_MAX_FRAGMENT_UNIFORM_COMPONENTS);
    set_info_int(self, info, "GL_MAX_FRAGMENT_UNIFORM_VECTORS", GL_MAX_FRAGMENT_UNIFORM_VECTORS);
    set_info_int(self, info, "GL_MAX_FRAGMENT_UNIFORM_BLOCKS", GL_MAX_FRAGMENT_UNIFORM_BLOCKS);
    set_info_int(self, info, "GL_MAX_GEOMETRY_INPUT_COMPONENTS", GL_MAX_GEOMETRY_INPUT_COMPONENTS);
    set_info_int(self, info, "GL_MAX_GEOMETRY_OUTPUT_COMPONENTS", GL_MAX_GEOMETRY_OUTPUT_COMPONENTS);
    set_info_int(self, info, "GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS", GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS);
    set_info_int(self, info, "GL_MAX_GEOMETRY_UNIFORM_BLOCKS", GL_MAX_GEOMETRY_UNIFORM_BLOCKS);
    set_info_int(self, info, "GL_MAX_GEOMETRY_UNIFORM_COMPONENTS", GL_MAX_GEOMETRY_UNIFORM_COMPONENTS);
    set_info_int(self, info, "GL_MAX_GEOMETRY_OUTPUT_VERTICES", GL_MAX_GEOMETRY_OUTPUT_VERTICES);
    set_info_int(self, info, "GL_MAX_INTEGER_SAMPLES", GL_MAX_INTEGER_SAMPLES);
    set_info_int(self, info, "GL_MAX_SAMPLES", GL_MAX_SAMPLES);
    set_info_int(self, info, "GL_MAX_RECTANGLE_TEXTURE_SIZE", GL_MAX_RECTANGLE_TEXTURE_SIZE);
    set_info_int(self, info, "GL_MAX_RENDERBUFFER_SIZE", GL_MAX_RENDERBUFFER_SIZE);
    set_info_int(self, info, "GL_MAX_SAMPLE_MASK_WORDS", GL_MAX_SAMPLE_MASK_WORDS);
    set_info_int(self, info, "GL_MAX_TEXTURE_BUFFER_SIZE", GL_MAX_TEXTURE_BUFFER_SIZE);
    set_info_int(self, info, "GL_MAX_TEXTURE_IMAGE_UNITS", GL_MAX_TEXTURE_IMAGE_UNITS);
    set_info_int(self, info, "GL_MAX_TEXTURE_LOD_BIAS", GL_MAX_TEXTURE_LOD_BIAS);
    set_info_int(self, info, "GL_MAX_TEXTURE_SIZE", GL_MAX_TEXTURE_SIZE);
    set_info_int(self, info, "GL_MAX_UNIFORM_BUFFER_BINDINGS", GL_MAX_UNIFORM_BUFFER_BINDINGS);
    set_info_int(self, info, "GL_MAX_UNIFORM_BLOCK_SIZE", GL_MAX_UNIFORM_BLOCK_SIZE);
    set_info_int(self, info, "GL_MAX_VARYING_VECTORS", GL_MAX_VARYING_VECTORS);
    set_info_int(self, info, "GL_MAX_VERTEX_ATTRIBS", GL_MAX_VERTEX_ATTRIBS);
    set_info_int(self, info, "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS", GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
    set_info_int(self, info, "GL_MAX_VERTEX_UNIFORM_COMPONENTS", GL_MAX_VERTEX_UNIFORM_COMPONENTS);
    set_info_int(self, info, "GL_MAX_VERTEX_UNIFORM_VECTORS", GL_MAX_VERTEX_UNIFORM_VECTORS);
    set_info_int(self, info, "GL_MAX_VERTEX_OUTPUT_COMPONENTS", GL_MAX_VERTEX_OUTPUT_COMPONENTS);
    set_info_int(self, info, "GL_MAX_VERTEX_UNIFORM_BLOCKS", GL_MAX_VERTEX_UNIFORM_BLOCKS);
    set_info_int64(self, info, "GL_MAX_SERVER_WAIT_TIMEOUT", GL_MAX_SERVER_WAIT_TIMEOUT);

    if (self->version_code >= 410) {
        set_info_int_range(self, info, "GL_VIEWPORT_BOUNDS_RANGE", GL_VIEWPORT_BOUNDS_RANGE);
        set_info_int(self, info, "GL_VIEWPORT_SUBPIXEL_BITS", GL_VIEWPORT_SUBPIXEL_BITS);
        set_info_int(self, info, "GL_MAX_VIEWPORTS", GL_MAX_VIEWPORTS);
    }

    if (self->version_code >= 420) {
        set_info_int(self, info, "GL_MIN_MAP_BUFFER_ALIGNMENT", GL_MIN_MAP_BUFFER_ALIGNMENT);
        set_info_int(self, info, "GL_MAX_COMBINED_ATOMIC_COUNTERS", GL_MAX_COMBINED_ATOMIC_COUNTERS);
        set_info_int(self, info, "GL_MAX_FRAGMENT_ATOMIC_COUNTERS", GL_MAX_FRAGMENT_ATOMIC_COUNTERS);
        set_info_int(self, info, "GL_MAX_GEOMETRY_ATOMIC_COUNTERS", GL_MAX_GEOMETRY_ATOMIC_COUNTERS);
        set_info_int(self, info, "GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS", GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS);
        set_info_int(self, info, "GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS", GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS);
        set_info_int(self, info, "GL_MAX_VERTEX_ATOMIC_COUNTERS", GL_MAX_VERTEX_ATOMIC_COUNTERS);
    }

    if (self->version_code >= 430) {
        set_info_int_xyz(self, info, "GL_MAX_COMPUTE_WORK_GROUP_COUNT", GL_MAX_COMPUTE_WORK_GROUP_COUNT);
        set_info_int_xyz(self, info, "GL_MAX_COMPUTE_WORK_GROUP_SIZE", GL_MAX_COMPUTE_WORK_GROUP_SIZE);
        set_info_int(self, info, "GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET", GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET);
        set_info_int(self, info, "GL_MAX_VERTEX_ATTRIB_BINDINGS", GL_MAX_VERTEX_ATTRIB_BINDINGS);
        set_info_int(self, info, "GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS", GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
        set_info_int(self, info, "GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS", GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS);
        set_info_int(self, info, "GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS", GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS);
        set_info_int(self, info, "GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS", GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS);
        set_info_int(self, info, "GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS", GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS);
        set_info_int(self, info, "GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS", GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS);
        set_info_int(self, info, "GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS", GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS);
        set_info_int(self, info, "GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS", GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS);
        set_info_int(self, info, "GL_MAX_COMPUTE_UNIFORM_COMPONENTS", GL_MAX_COMPUTE_UNIFORM_COMPONENTS);
        set_info_int(self, info, "GL_MAX_COMPUTE_ATOMIC_COUNTERS", GL_MAX_COMPUTE_ATOMIC_COUNTERS);
        set_info_int(self, info, "GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS", GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS);
        set_info_int(self, info, "GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS", GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS);
        set_info_int(self, info, "GL_MAX_COMPUTE_UNIFORM_BLOCKS", GL_MAX_COMPUTE_UNIFORM_BLOCKS);
        set_info_int(self, info, "GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS", GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS);
        set_info_int(self, info, "GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS", GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS);
        set_info_int(self, info, "GL_MAX_FRAMEBUFFER_WIDTH", GL_MAX_FRAMEBUFFER_WIDTH);
        set_info_int(self, info, "GL_MAX_FRAMEBUFFER_HEIGHT", GL_MAX_FRAMEBUFFER_HEIGHT);
        set_info_int(self, info, "GL_MAX_FRAMEBUFFER_LAYERS", GL_MAX_FRAMEBUFFER_LAYERS);
        set_info_int(self, info, "GL_MAX_FRAMEBUFFER_SAMPLES", GL_MAX_FRAMEBUFFER_SAMPLES);
        set_info_int(self, info, "GL_MAX_UNIFORM_LOCATIONS", GL_MAX_UNIFORM_LOCATIONS);
        set_info_int64(self, info, "GL_MAX_ELEMENT_INDEX", GL_MAX_ELEMENT_INDEX);
        set_info_int64(self, info, "GL_MAX_SHADER_STORAGE_BLOCK_SIZE", GL_MAX_SHADER_STORAGE_BLOCK_SIZE);
    }

    return info;
}

PyObject * strsize(PyObject * self, PyObject * args) {
    const char * str;

    int args_ok = PyArg_ParseTuple(
        args,
        "s",
        &str
    );

    if (!args_ok) {
        return 0;
    }

    char first_chr = *str++;
    if (first_chr < '1' || first_chr > '9') {
        return 0;
    }

    long long value = first_chr - '0';

    while (char chr = *str++) {
        if (chr < '0' || chr > '9') {
            switch (chr) {
                case 'G':
                    value *= 1024;

                case 'M':
                    value *= 1024;

                case 'K':
                    value *= 1024;

                    if (*str++ != 'B') {
                        return 0;
                    }

                case 'B':
                    if (*str++) {
                        return 0;
                    }

                case 0:
                    break;

                default:
                    return 0;
            }
            break;
        }

        value = value * 10 + chr - '0';
    }

    return PyLong_FromLongLong(value);
}

PyObject * fmtdebug(PyObject * self, PyObject * args) {
    const char * str;

    int args_ok = PyArg_ParseTuple(
        args,
        "s",
        &str
    );

    if (!args_ok) {
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
    PyObject * context = PyDict_GetItemString(kwargs, "context");

    if (!context) {
        PyObject * glcontext = PyImport_ImportModule("glcontext");
        if (!glcontext) {
            // Displayed to user: ModuleNotFoundError: No module named 'glcontext'
            return NULL;
        }

        PyObject * backend = NULL;
        PyObject * backend_name = PyDict_GetItemString(kwargs, "backend");

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

        // Ensure we have a callable
        if (!PyCallable_Check(backend)) {
            MGLError_Set("The returned glcontext is not a callable");
            return NULL;
        }
        // Create context by simply forwarding all arguments
        context = PyObject_Call(backend, args, kwargs);
        if (!context) {
            return NULL;
        }
    } else {
        Py_INCREF(context);
    }

    MGLContext * ctx = PyObject_New(MGLContext, MGLContext_type);
    ctx->released = false;
    ctx->wireframe = false;
    ctx->ctx = context;

    ctx->gl = load_gl_methods(context);
    if (PyErr_Occurred()) {
        return NULL;
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

    if (gl.PrimitiveRestartIndex) {
        gl.Enable(GL_PRIMITIVE_RESTART);
        gl.PrimitiveRestartIndex(-1);
    } else {
        gl.Enable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    }

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

        framebuffer->num_color_attachments = 1;

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
        gl.BindFramebuffer(GL_FRAMEBUFFER, bound_framebuffer);

        memset(framebuffer->color_mask, 1, sizeof(framebuffer->color_mask));
        framebuffer->depth_mask = true;

        framebuffer->context = ctx;

        int scissor_box[4] = {};
        gl.GetIntegerv(GL_SCISSOR_BOX, scissor_box);

        framebuffer->viewport = {scissor_box[0], scissor_box[1], scissor_box[2], scissor_box[3]};

        framebuffer->scissor_enabled = false;
        framebuffer->scissor = {scissor_box[0], scissor_box[1], scissor_box[2], scissor_box[3]};

        framebuffer->extra = NULL;
        framebuffer->size_tuple = Py_BuildValue("(ii)", scissor_box[2], scissor_box[3]);
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

PyObject * return_self(PyObject * self) {
    return self;
}

PyMethodDef MGL_module_methods[] = {
    {(char *)"strsize", (PyCFunction)strsize, METH_VARARGS},
    {(char *)"create_context", (PyCFunction)create_context, METH_VARARGS | METH_KEYWORDS},
    {(char *)"fmtdebug", (PyCFunction)fmtdebug, METH_VARARGS},
    {},
};

PyMethodDef MGLBuffer_methods[] = {
    {(char *)"write", (PyCFunction)MGLBuffer_write, METH_VARARGS},
    {(char *)"read", (PyCFunction)MGLBuffer_read, METH_VARARGS},
    {(char *)"read_into", (PyCFunction)MGLBuffer_read_into, METH_VARARGS},
    {(char *)"write_chunks", (PyCFunction)MGLBuffer_write_chunks, METH_VARARGS},
    {(char *)"read_chunks", (PyCFunction)MGLBuffer_read_chunks, METH_VARARGS},
    {(char *)"read_chunks_into", (PyCFunction)MGLBuffer_read_chunks_into, METH_VARARGS},
    {(char *)"clear", (PyCFunction)MGLBuffer_clear, METH_VARARGS},
    {(char *)"orphan", (PyCFunction)MGLBuffer_orphan, METH_VARARGS},
    {(char *)"bind_to_uniform_block", (PyCFunction)MGLBuffer_bind_to_uniform_block, METH_VARARGS},
    {(char *)"bind_to_storage_buffer", (PyCFunction)MGLBuffer_bind_to_storage_buffer, METH_VARARGS},
    {(char *)"release", (PyCFunction)MGLBuffer_release, METH_NOARGS},
    {(char *)"size", (PyCFunction)MGLBuffer_size, METH_NOARGS},
    {},
};

PyMethodDef MGLComputeShader_methods[] = {
    {(char *)"run", (PyCFunction)MGLComputeShader_run, METH_VARARGS | METH_KEYWORDS},
    {(char *)"run_indirect", (PyCFunction)MGLComputeShader_run_indirect, METH_VARARGS | METH_KEYWORDS},
    {(char *)"release", (PyCFunction)MGLComputeShader_release, METH_VARARGS | METH_KEYWORDS},
    {},
};

PyGetSetDef MGLComputeShader_getset[] = {
    {(char *)"mglo", (getter)return_self, NULL},
    {},
};

PyMemberDef MGLComputeShader_members[] = {
    {(char *)"extra", T_OBJECT_EX, offsetof(MGLTexture, extra), 0},
    {(char *)"glo", T_INT, offsetof(MGLComputeShader, program_obj), READONLY},
    {(char *)"_members", T_OBJECT, offsetof(MGLComputeShader, members), READONLY},
    {},
};

PyMethodDef MGLContext_methods[] = {
    {(char *)"enable_only", (PyCFunction)MGLContext_enable_only, METH_VARARGS},
    {(char *)"enable", (PyCFunction)MGLContext_enable, METH_VARARGS},
    {(char *)"disable", (PyCFunction)MGLContext_disable, METH_VARARGS},
    {(char *)"enable_direct", (PyCFunction)MGLContext_enable_direct, METH_VARARGS},
    {(char *)"disable_direct", (PyCFunction)MGLContext_disable_direct, METH_VARARGS},
    {(char *)"finish", (PyCFunction)MGLContext_finish, METH_NOARGS},
    {(char *)"copy_buffer", (PyCFunction)MGLContext_copy_buffer, METH_VARARGS},
    {(char *)"copy_framebuffer", (PyCFunction)MGLContext_copy_framebuffer, METH_VARARGS},
    {(char *)"detect_framebuffer", (PyCFunction)MGLContext_detect_framebuffer, METH_VARARGS},
    {(char *)"clear_samplers", (PyCFunction)MGLContext_clear_samplers, METH_VARARGS},

    {(char *)"buffer", (PyCFunction)MGLContext_buffer, METH_VARARGS},
    {(char *)"texture", (PyCFunction)MGLContext_texture, METH_VARARGS},
    {(char *)"texture3d", (PyCFunction)MGLContext_texture3d, METH_VARARGS},
    {(char *)"texture_array", (PyCFunction)MGLContext_texture_array, METH_VARARGS},
    {(char *)"texture_cube", (PyCFunction)MGLContext_texture_cube, METH_VARARGS},
    {(char *)"depth_texture", (PyCFunction)MGLContext_depth_texture, METH_VARARGS},
    {(char *)"external_texture", (PyCFunction)MGLContext_external_texture, METH_VARARGS},
    {(char *)"vertex_array", (PyCFunction)MGLContext_vertex_array, METH_VARARGS},
    {(char *)"program", (PyCFunction)MGLContext_program, METH_VARARGS},
    {(char *)"framebuffer", (PyCFunction)MGLContext_framebuffer, METH_VARARGS},
    {(char *)"empty_framebuffer", (PyCFunction)MGLContext_empty_framebuffer, METH_VARARGS},
    {(char *)"renderbuffer", (PyCFunction)MGLContext_renderbuffer, METH_VARARGS | METH_KEYWORDS},
    {(char *)"depth_renderbuffer", (PyCFunction)MGLContext_depth_renderbuffer, METH_VARARGS | METH_KEYWORDS},
    {(char *)"compute_shader", (PyCFunction)MGLContext_compute_shader, METH_VARARGS | METH_KEYWORDS},
    {(char *)"query", (PyCFunction)MGLContext_query, METH_VARARGS},
    {(char *)"scope", (PyCFunction)MGLContext_scope, METH_VARARGS},
    {(char *)"sampler", (PyCFunction)MGLContext_sampler, METH_VARARGS},
    {(char *)"memory_barrier", (PyCFunction)MGLContext_memory_barrier, METH_VARARGS},

    {(char *)"__enter__", (PyCFunction)MGLContext_enter, METH_NOARGS},
    {(char *)"__exit__", (PyCFunction)MGLContext_exit, METH_VARARGS},
    {(char *)"release", (PyCFunction)MGLContext_release, METH_NOARGS},

    {(char *)"_get_ubo_binding", (PyCFunction)MGLContext_get_ubo_binding, METH_VARARGS},
    {(char *)"_set_ubo_binding", (PyCFunction)MGLContext_set_ubo_binding, METH_VARARGS},
    {(char *)"_get_storage_block_binding", (PyCFunction)MGLContext_get_storage_block_binding, METH_VARARGS},
    {(char *)"_set_storage_block_binding", (PyCFunction)MGLContext_set_storage_block_binding, METH_VARARGS},
    {(char *)"_write_uniform", (PyCFunction)MGLContext_write_uniform, METH_VARARGS},
    {(char *)"_read_uniform", (PyCFunction)MGLContext_read_uniform, METH_VARARGS},
    {(char *)"_set_uniform_handle", (PyCFunction)MGLContext_set_uniform_handle, METH_VARARGS},
    {},
};

PyGetSetDef MGLContext_getset[] = {
    {(char *)"line_width", (getter)MGLContext_get_line_width, (setter)MGLContext_set_line_width},
    {(char *)"point_size", (getter)MGLContext_get_point_size, (setter)MGLContext_set_point_size},

    {(char *)"depth_func", (getter)MGLContext_get_depth_func, (setter)MGLContext_set_depth_func},
    {(char *)"blend_func", (getter)MGLContext_get_blend_func, (setter)MGLContext_set_blend_func},
    {(char *)"blend_equation", (getter)MGLContext_get_blend_equation, (setter)MGLContext_set_blend_equation},
    {(char *)"multisample", (getter)MGLContext_get_multisample, (setter)MGLContext_set_multisample},

    {(char *)"provoking_vertex", (getter)MGLContext_get_provoking_vertex, (setter)MGLContext_set_provoking_vertex},
    {(char *)"polygon_offset", (getter)MGLContext_get_polygon_offset, (setter)MGLContext_set_polygon_offset},

    {(char *)"default_texture_unit", (getter)MGLContext_get_default_texture_unit, (setter)MGLContext_set_default_texture_unit},
    {(char *)"max_samples", (getter)MGLContext_get_max_samples, NULL},
    {(char *)"max_integer_samples", (getter)MGLContext_get_max_integer_samples, NULL},
    {(char *)"max_texture_units", (getter)MGLContext_get_max_texture_units, NULL},
    {(char *)"max_anisotropy", (getter)MGLContext_get_max_anisotropy, NULL},

    {(char *)"fbo", (getter)MGLContext_get_fbo, (setter)MGLContext_set_fbo},

    {(char *)"wireframe", (getter)MGLContext_get_wireframe, (setter)MGLContext_set_wireframe},
    {(char *)"front_face", (getter)MGLContext_get_front_face, (setter)MGLContext_set_front_face},
    {(char *)"cull_face", (getter)MGLContext_get_cull_face, (setter)MGLContext_set_cull_face},

    {(char *)"patch_vertices", (getter)MGLContext_get_patch_vertices, (setter)MGLContext_set_patch_vertices},

    {(char *)"extensions", (getter)MGLContext_get_extensions, NULL},
    {(char *)"info", (getter)MGLContext_get_info, NULL},
    {(char *)"error", (getter)MGLContext_get_error, NULL},

    {(char *)"_context", (getter)MGLContext_get_context, NULL},
    {},
};

PyGetSetDef MGLFramebuffer_getset[] = {
    {(char *)"mglo", (getter)return_self, NULL},
    {(char *)"viewport", (getter)MGLFramebuffer_get_viewport, (setter)MGLFramebuffer_set_viewport},
    {(char *)"scissor", (getter)MGLFramebuffer_get_scissor, (setter)MGLFramebuffer_set_scissor},
    {(char *)"color_mask", (getter)MGLFramebuffer_get_color_mask, (setter)MGLFramebuffer_set_color_mask},
    {(char *)"depth_mask", (getter)MGLFramebuffer_get_depth_mask, (setter)MGLFramebuffer_set_depth_mask},
    {(char *)"bits", (getter)MGLFramebuffer_get_bits, NULL},
    {},
};

PyMethodDef MGLFramebuffer_methods[] = {
    {(char *)"clear", (PyCFunction)MGLFramebuffer_clear, METH_VARARGS | METH_KEYWORDS},
    {(char *)"use", (PyCFunction)MGLFramebuffer_use, METH_NOARGS},
    {(char *)"read", (PyCFunction)MGLFramebuffer_read, METH_VARARGS | METH_KEYWORDS},
    {(char *)"read_into", (PyCFunction)MGLFramebuffer_read_into, METH_VARARGS | METH_KEYWORDS},
    {(char *)"release", (PyCFunction)MGLFramebuffer_release, METH_NOARGS},
    {},
};

PyMemberDef MGLFramebuffer_members[] = {
    {(char *)"size", T_OBJECT, offsetof(MGLFramebuffer, size_tuple), READONLY},
    {(char *)"extra", T_OBJECT_EX, offsetof(MGLFramebuffer, extra), 0},
    {(char *)"glo", T_INT, offsetof(MGLFramebuffer, framebuffer_obj), READONLY},
    {(char *)"width", T_INT, offsetof(MGLFramebuffer, width), READONLY},
    {(char *)"height", T_INT, offsetof(MGLFramebuffer, height), READONLY},
    {(char *)"samples", T_INT, offsetof(MGLFramebuffer, samples), READONLY},
    {},
};

PyMethodDef MGLProgram_methods[] = {
    {(char *)"release", (PyCFunction)MGLProgram_release, METH_NOARGS},
    {},
};

PyGetSetDef MGLProgram_getset[] = {
    {(char *)"mglo", (getter)return_self, NULL},
    {},
};

PyMemberDef MGLProgram_members[] = {
    {(char *)"extra", T_OBJECT_EX, offsetof(MGLTexture, extra), 0},
    {(char *)"glo", T_INT, offsetof(MGLProgram, program_obj), READONLY},
    {(char *)"_members", T_OBJECT, offsetof(MGLProgram, members), READONLY},
    {(char *)"is_transform", T_BOOL, offsetof(MGLProgram, is_transform), READONLY},
    {},
};

PyGetSetDef MGLQuery_getset[] = {
    {(char *)"samples", (getter)MGLQuery_get_samples, NULL},
    {(char *)"primitives", (getter)MGLQuery_get_primitives, NULL},
    {(char *)"elapsed", (getter)MGLQuery_get_elapsed, NULL},
    {},
};

PyMethodDef MGLQuery_methods[] = {
    {(char *)"begin", (PyCFunction)MGLQuery_begin, METH_NOARGS},
    {(char *)"end", (PyCFunction)MGLQuery_end, METH_NOARGS},
    {(char *)"begin_render", (PyCFunction)MGLQuery_begin_render, METH_NOARGS},
    {(char *)"end_render", (PyCFunction)MGLQuery_end_render, METH_NOARGS},
    // {(char *)"release", (PyCFunction)MGLQuery_release, METH_NOARGS},
    {},
};

PyMethodDef MGLRenderbuffer_methods[] = {
    {(char *)"release", (PyCFunction)MGLRenderbuffer_release, METH_NOARGS},
    {},
};

PyGetSetDef MGLRenderbuffer_getset[] = {
    {(char *)"mglo", (getter)return_self, NULL},
    {},
};

PyMemberDef MGLRenderbuffer_members[] = {
    {(char *)"size", T_OBJECT, offsetof(MGLRenderbuffer, size_tuple), READONLY},
    {(char *)"extra", T_OBJECT_EX, offsetof(MGLRenderbuffer, extra), 0},
    {(char *)"glo", T_INT, offsetof(MGLRenderbuffer, renderbuffer_obj), READONLY},
    {(char *)"width", T_INT, offsetof(MGLRenderbuffer, width), READONLY},
    {(char *)"height", T_INT, offsetof(MGLRenderbuffer, height), READONLY},
    {(char *)"samples", T_INT, offsetof(MGLRenderbuffer, samples), READONLY},
    {},
};

PyGetSetDef MGLSampler_getset[] = {
    {(char *)"repeat_x", (getter)MGLSampler_get_repeat_x, (setter)MGLSampler_set_repeat_x},
    {(char *)"repeat_y", (getter)MGLSampler_get_repeat_y, (setter)MGLSampler_set_repeat_y},
    {(char *)"repeat_z", (getter)MGLSampler_get_repeat_z, (setter)MGLSampler_set_repeat_z},
    {(char *)"filter", (getter)MGLSampler_get_filter, (setter)MGLSampler_set_filter},
    {(char *)"compare_func", (getter)MGLSampler_get_compare_func, (setter)MGLSampler_set_compare_func},
    {(char *)"anisotropy", (getter)MGLSampler_get_anisotropy, (setter)MGLSampler_set_anisotropy},
    {(char *)"border_color", (getter)MGLSampler_get_border_color, (setter)MGLSampler_set_border_color},
    {(char *)"min_lod", (getter)MGLSampler_get_min_lod, (setter)MGLSampler_set_min_lod},
    {(char *)"max_lod", (getter)MGLSampler_get_max_lod, (setter)MGLSampler_set_max_lod},
    {},
};

PyMethodDef MGLSampler_methods[] = {
    {(char *)"use", (PyCFunction)MGLSampler_use, METH_VARARGS},
    {(char *)"clear", (PyCFunction)MGLSampler_clear, METH_VARARGS},
    {(char *)"release", (PyCFunction)MGLSampler_release, METH_NOARGS},
    {},
};

PyMethodDef MGLScope_methods[] = {
    {(char *)"begin", (PyCFunction)MGLScope_begin, METH_VARARGS},
    {(char *)"end", (PyCFunction)MGLScope_end, METH_VARARGS},
    {(char *)"release", (PyCFunction)MGLScope_release, METH_NOARGS},
    {},
};

PyGetSetDef MGLTexture_getset[] = {
    {(char *)"repeat_x", (getter)MGLTexture_get_repeat_x, (setter)MGLTexture_set_repeat_x},
    {(char *)"repeat_y", (getter)MGLTexture_get_repeat_y, (setter)MGLTexture_set_repeat_y},
    {(char *)"filter", (getter)MGLTexture_get_filter, (setter)MGLTexture_set_filter},
    {(char *)"swizzle", (getter)MGLTexture_get_swizzle, (setter)MGLTexture_set_swizzle},
    {(char *)"compare_func", (getter)MGLTexture_get_compare_func, (setter)MGLTexture_set_compare_func},
    {(char *)"anisotropy", (getter)MGLTexture_get_anisotropy, (setter)MGLTexture_set_anisotropy},
    {},
};

PyMethodDef MGLTexture_methods[] = {
    {(char *)"write", (PyCFunction)MGLTexture_write, METH_VARARGS},
    {(char *)"bind", (PyCFunction)MGLTexture_meth_bind, METH_VARARGS},
    {(char *)"use", (PyCFunction)MGLTexture_use, METH_VARARGS},
    {(char *)"build_mipmaps", (PyCFunction)MGLTexture_build_mipmaps, METH_VARARGS},
    {(char *)"read", (PyCFunction)MGLTexture_read, METH_VARARGS},
    {(char *)"read_into", (PyCFunction)MGLTexture_read_into, METH_VARARGS},
    {(char *)"get_handle", (PyCFunction)MGLTexture_get_handle, METH_VARARGS},
    {(char *)"release", (PyCFunction)MGLTexture_release, METH_NOARGS},
    {},
};

PyMemberDef MGLTexture_members[] = {
    {(char *)"size", T_OBJECT, offsetof(MGLTexture, size_tuple), READONLY},
    {(char *)"extra", T_OBJECT_EX, offsetof(MGLTexture, extra), 0},
    {(char *)"glo", T_INT, offsetof(MGLTexture, texture_obj), READONLY},
    {(char *)"width", T_INT, offsetof(MGLTexture, width), READONLY},
    {(char *)"height", T_INT, offsetof(MGLTexture, height), READONLY},
    {(char *)"samples", T_INT, offsetof(MGLTexture, samples), READONLY},
    {},
};

PyGetSetDef MGLTexture3D_getset[] = {
    {(char *)"repeat_x", (getter)MGLTexture3D_get_repeat_x, (setter)MGLTexture3D_set_repeat_x},
    {(char *)"repeat_y", (getter)MGLTexture3D_get_repeat_y, (setter)MGLTexture3D_set_repeat_y},
    {(char *)"repeat_z", (getter)MGLTexture3D_get_repeat_z, (setter)MGLTexture3D_set_repeat_z},
    {(char *)"filter", (getter)MGLTexture3D_get_filter, (setter)MGLTexture3D_set_filter},
    {(char *)"swizzle", (getter)MGLTexture3D_get_swizzle, (setter)MGLTexture3D_set_swizzle},
    {},
};

PyMethodDef MGLTexture3D_methods[] = {
    {(char *)"write", (PyCFunction)MGLTexture3D_write, METH_VARARGS},
    {(char *)"bind", (PyCFunction)MGLTexture3D_meth_bind, METH_VARARGS},
    {(char *)"use", (PyCFunction)MGLTexture3D_use, METH_VARARGS},
    {(char *)"build_mipmaps", (PyCFunction)MGLTexture3D_build_mipmaps, METH_VARARGS},
    {(char *)"read", (PyCFunction)MGLTexture3D_read, METH_VARARGS},
    {(char *)"read_into", (PyCFunction)MGLTexture3D_read_into, METH_VARARGS},
    {(char *)"get_handle", (PyCFunction)MGLTexture3D_get_handle, METH_VARARGS},
    {(char *)"release", (PyCFunction)MGLTexture3D_release, METH_NOARGS},
    {},
};

PyGetSetDef MGLTextureArray_getset[] = {
    {(char *)"repeat_x", (getter)MGLTextureArray_get_repeat_x, (setter)MGLTextureArray_set_repeat_x},
    {(char *)"repeat_y", (getter)MGLTextureArray_get_repeat_y, (setter)MGLTextureArray_set_repeat_y},
    {(char *)"filter", (getter)MGLTextureArray_get_filter, (setter)MGLTextureArray_set_filter},
    {(char *)"swizzle", (getter)MGLTextureArray_get_swizzle, (setter)MGLTextureArray_set_swizzle},
    {(char *)"anisotropy", (getter)MGLTextureArray_get_anisotropy, (setter)MGLTextureArray_set_anisotropy},
    {},
};

PyMethodDef MGLTextureArray_methods[] = {
    {(char *)"write", (PyCFunction)MGLTextureArray_write, METH_VARARGS},
    {(char *)"bind", (PyCFunction)MGLTextureArray_meth_bind, METH_VARARGS},
    {(char *)"use", (PyCFunction)MGLTextureArray_use, METH_VARARGS},
    {(char *)"build_mipmaps", (PyCFunction)MGLTextureArray_build_mipmaps, METH_VARARGS},
    {(char *)"read", (PyCFunction)MGLTextureArray_read, METH_VARARGS},
    {(char *)"read_into", (PyCFunction)MGLTextureArray_read_into, METH_VARARGS},
    {(char *)"get_handle", (PyCFunction)MGLTextureArray_get_handle, METH_VARARGS},
    {(char *)"release", (PyCFunction)MGLTextureArray_release, METH_NOARGS},
    {},
};

PyGetSetDef MGLTextureCube_getset[] = {
    {(char *)"filter", (getter)MGLTextureCube_get_filter, (setter)MGLTextureCube_set_filter},
    {(char *)"swizzle", (getter)MGLTextureCube_get_swizzle, (setter)MGLTextureCube_set_swizzle},
    {(char *)"anisotropy", (getter)MGLTextureCube_get_anisotropy, (setter)MGLTextureCube_set_anisotropy},
    {},
};

PyMethodDef MGLTextureCube_methods[] = {
    {(char *)"write", (PyCFunction)MGLTextureCube_write, METH_VARARGS},
    {(char *)"use", (PyCFunction)MGLTextureCube_use, METH_VARARGS},
    {(char *)"bind", (PyCFunction)MGLTextureCube_meth_bind, METH_VARARGS},
	{(char *)"build_mipmaps", (PyCFunction)MGLTextureCube_build_mipmaps, METH_VARARGS},
    {(char *)"read", (PyCFunction)MGLTextureCube_read, METH_VARARGS},
    {(char *)"read_into", (PyCFunction)MGLTextureCube_read_into, METH_VARARGS},
    {(char *)"get_handle", (PyCFunction)MGLTextureCube_get_handle, METH_VARARGS},
    {(char *)"release", (PyCFunction)MGLTextureCube_release, METH_NOARGS},
    {},
};

PyMethodDef MGLVertexArray_methods[] = {
    {(char *)"render", (PyCFunction)MGLVertexArray_render, METH_VARARGS},
    {(char *)"render_indirect", (PyCFunction)MGLVertexArray_render_indirect, METH_VARARGS},
    {(char *)"transform", (PyCFunction)MGLVertexArray_transform, METH_VARARGS},
    {(char *)"bind", (PyCFunction)MGLVertexArray_bind, METH_VARARGS},
    {(char *)"release", (PyCFunction)MGLVertexArray_release, METH_NOARGS},
    {},
};

PyGetSetDef MGLVertexArray_getset[] = {
    {(char *)"index_buffer", NULL, (setter)MGLVertexArray_set_index_buffer},
    {(char *)"vertices", (getter)MGLVertexArray_get_vertices, (setter)MGLVertexArray_set_vertices},
    {(char *)"instances", (getter)MGLVertexArray_get_instances, (setter)MGLVertexArray_set_instances},
    {(char *)"subroutines", NULL, (setter)MGLVertexArray_set_subroutines},
    {},
};

PyType_Slot MGLBuffer_slots[] = {
    #if PY_VERSION_HEX >= 0x03090000
    {Py_bf_getbuffer, (void *)MGLBuffer_tp_as_buffer_get_view},
    {Py_bf_releasebuffer, (void *)MGLBuffer_tp_as_buffer_release_view},
    #endif
    {Py_tp_methods, MGLBuffer_methods},
    {Py_tp_dealloc, (void *)default_dealloc},
    {},
};

PyType_Slot MGLComputeShader_slots[] = {
    {Py_tp_methods, MGLComputeShader_methods},
    {Py_tp_members, MGLComputeShader_members},
    {Py_tp_getset, MGLComputeShader_getset},
    {Py_mp_subscript, MGLComputeShader_getitem},
    {Py_mp_ass_subscript, MGLComputeShader_setitem},
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
    {Py_tp_members, MGLFramebuffer_members},
    {Py_tp_dealloc, (void *)default_dealloc},
    {},
};

PyType_Slot MGLProgram_slots[] = {
    {Py_tp_methods, MGLProgram_methods},
    {Py_tp_getset, MGLProgram_getset},
    {Py_tp_members, MGLProgram_members},
    {Py_mp_subscript, MGLProgram_getitem},
    {Py_mp_ass_subscript, MGLProgram_setitem},
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
    {Py_tp_members, MGLRenderbuffer_members},
    {Py_tp_getset, MGLRenderbuffer_getset},
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
    {Py_tp_members, MGLTexture_members},
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

PyModuleDef MGL_moduledef = {
    PyModuleDef_HEAD_INIT,
    "mgl",
    0,
    -1,
    MGL_module_methods,
    0,
    0,
    0,
    0,
};

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

    PyModule_AddObject(module, "Buffer", (PyObject *)MGLBuffer_type);
    PyModule_AddObject(module, "ComputeShader", (PyObject *)MGLComputeShader_type);
    PyModule_AddObject(module, "Context", (PyObject *)MGLContext_type);
    PyModule_AddObject(module, "Framebuffer", (PyObject *)MGLFramebuffer_type);
    PyModule_AddObject(module, "Program", (PyObject *)MGLProgram_type);
    PyModule_AddObject(module, "Query", (PyObject *)MGLQuery_type);
    PyModule_AddObject(module, "Renderbuffer", (PyObject *)MGLRenderbuffer_type);
    PyModule_AddObject(module, "Scope", (PyObject *)MGLScope_type);
    PyModule_AddObject(module, "Texture", (PyObject *)MGLTexture_type);
    PyModule_AddObject(module, "TextureArray", (PyObject *)MGLTextureArray_type);
    PyModule_AddObject(module, "TextureCube", (PyObject *)MGLTextureCube_type);
    PyModule_AddObject(module, "Texture3D", (PyObject *)MGLTexture3D_type);
    PyModule_AddObject(module, "VertexArray", (PyObject *)MGLVertexArray_type);
    PyModule_AddObject(module, "Sampler", (PyObject *)MGLSampler_type);

    PyObject * InvalidObject = PyObject_GetAttrString(helper, "InvalidObject");
    PyModule_AddObject(module, "InvalidObject", InvalidObject);
    Py_INCREF(InvalidObject);

    return module;
}
