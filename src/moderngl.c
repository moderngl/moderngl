#include <Python.h>
#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#define GLAPI __stdcall
#define THREAD_LOCAL __declspec(thread)
#else
#include <dlfcn.h>
#define GLAPI
#define THREAD_LOCAL __thread
#endif

#if PY_VERSION_HEX < 0x030C0000
#include <structmember.h>
#define Py_T_INT T_INT
#define Py_T_UINT T_UINT
#define Py_T_BOOL T_BOOL
#define Py_T_OBJECT_EX T_OBJECT_EX
#define Py_READONLY READONLY
#endif

#if PY_VERSION_HEX < 0x030D0000
#define PyMutex char
#define PyMutex_Lock(mutex) (void)(mutex)
#define PyMutex_Unlock(mutex) (void)(mutex)
#define Py_BEGIN_CRITICAL_SECTION(op) {
#define Py_END_CRITICAL_SECTION() }
#define PyUnicode_EqualToUTF8(str, value) (PyUnicode_CompareWithASCIIString(str, value) == 0)

static int PyDict_GetItemRef(PyObject * dict, PyObject * key, PyObject ** result) {
    *result = Py_XNewRef(PyDict_GetItemWithError(dict, key));
    return *result ? 1 : PyErr_Occurred() ? -1 : 0;
}
#endif

#include "gl.h"

enum {
    MAX_SAMPLERS = 16,
    MAX_BUFFERS = 8,
    MAX_ATTACHMENTS = 8,
    MAX_ATTRIBUTES = 64,
};

typedef struct GCHeader GCHeader;
typedef struct Context Context;

typedef struct ImageFormat {
    const char * name;
    uint32_t internal_format;
    uint32_t format;
    uint32_t type;
    uint32_t buffer;
    int32_t components;
    int32_t pixel_size;
    int32_t color;
    char clear_type;
} ImageFormat;

typedef struct VertexFormat {
    const char * name;
    uint32_t type;
    int32_t components;
    int32_t normalized;
    int32_t integer;
    int32_t size;
} VertexFormat;

typedef struct ShortVertexFormat {
    const char * name;
    const char * format;
} ShortVertexFormat;

typedef struct NamedValue {
    const char * name;
    uint32_t value;
} NamedValue;

typedef struct Region {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} Region;

typedef struct PolygonOffset {
    int8_t enabled;
    float factor;
    float units;
} PolygonOffset;

typedef union ClearValue {
    float clear_floats[4];
    int32_t clear_ints[4];
    uint32_t clear_uints[4];
} ClearValue;

typedef struct UniformBinding {
    uint32_t gltype;
    int32_t function;
    int32_t location;
    int32_t count;
    int32_t values;
    int32_t array;
    char type;
} UniformBinding;

typedef struct BlendState {
    int32_t enabled;
    uint32_t src;
    uint32_t dst;
    uint32_t equation;
    uint32_t src_alpha;
    uint32_t dst_alpha;
    uint32_t equation_alpha;
} BlendState;

typedef struct StencilState {
    int32_t enabled;
    int32_t ref;
    uint32_t read_mask;
    uint32_t write_mask;
    uint32_t func;
    uint32_t fail;
    uint32_t depth_fail;
    uint32_t depth_pass;
    uint32_t back_func;
    uint32_t back_fail;
    uint32_t back_depth_fail;
    uint32_t back_depth_pass;
} StencilState;

struct GCHeader {
    PyObject_HEAD
    GCHeader * gc_prev;
    GCHeader * gc_next;
};

typedef struct ModuleState {
    PyObject * context;
    PyObject * screen;
    PyTypeObject * Context_type;
    PyTypeObject * Program_type;
    PyTypeObject * Framebuffer_type;
    PyTypeObject * VertexArray_type;
    PyTypeObject * Texture_type;
    PyTypeObject * Buffer_type;
    PyTypeObject * Sampler_type;
    PyTypeObject * Resources_type;
    PyTypeObject * Pipeline_type;
    PyTypeObject * ReadRequest_type;
    PyTypeObject * HeadlessContext_type;
} ModuleState;

typedef struct HeadlessContext {
    PyObject_HEAD
    ModuleState * module_state;
    void * handles[4];
} HeadlessContext;

typedef struct Buffer {
    PyObject_HEAD
    GCHeader * gc_prev;
    GCHeader * gc_next;
    Context * ctx;
    uint32_t buffer;
    uint32_t target;
    int32_t size;
    int8_t index;
    int8_t external;
} Buffer;

typedef struct Attribute {
    Buffer * buffer;
    const VertexFormat * fmt;
    int32_t location;
    int32_t offset;
    int32_t stride;
    int32_t divisor;
} Attribute;

typedef struct Texture {
    PyObject_HEAD
    GCHeader * gc_prev;
    GCHeader * gc_next;
    Context * ctx;
    PyObject * size;
    PyObject * format;
    const ImageFormat * fmt;
    uint32_t texture;
    uint32_t target;
    uint32_t framebuffer;
    int32_t width;
    int32_t height;
    int32_t array;
    int8_t cubemap;
    int8_t multisampled;
    int8_t external;
    int32_t layer_count;
    int32_t level_count;
} Texture;

typedef struct Sampler {
    PyObject_HEAD
    GCHeader * gc_prev;
    GCHeader * gc_next;
    Context * ctx;
    Texture * texture;
    uint32_t sampler;
} Sampler;

typedef struct BufferRange {
    int32_t binding;
    Buffer * buffer;
    int32_t offset;
    int32_t size;
} BufferRange;

typedef struct SamplerSlot {
    int32_t unit;
    Sampler * sampler;
} SamplerSlot;

typedef struct StorageImageSlot {
    int32_t unit;
    Texture * texture;
    int32_t layer;
    int32_t level;
} StorageImageSlot;

typedef struct Resources {
    PyObject_HEAD
    Context * ctx;
    int32_t sampler_count;
    int32_t uniform_buffer_count;
    int32_t storage_buffer_count;
    int32_t storage_image_count;
    SamplerSlot samplers[MAX_SAMPLERS];
    BufferRange uniform_buffers[MAX_BUFFERS];
    BufferRange storage_buffers[MAX_BUFFERS];
    StorageImageSlot storage_images[MAX_BUFFERS];
} Resources;

typedef struct VertexArray {
    PyObject_HEAD
    GCHeader * gc_prev;
    GCHeader * gc_next;
    Context * ctx;
    PyObject * attributes;
    PyObject * index_buffer;
    uint32_t vertex_array;
    uint32_t index_type;
    int32_t index_size;
    int32_t vertices;
    uint32_t mode;
} VertexArray;

typedef struct Program {
    PyObject_HEAD
    GCHeader * gc_prev;
    GCHeader * gc_next;
    Context * ctx;
    PyObject * uniform_index;
    PyObject * samplers;
    PyObject * uniform_buffers;
    PyObject * storage_buffers;
    PyObject * storage_images;
    UniformBinding * uniform_bindings;
    uint32_t program;
    int32_t pipelines;
} Program;

typedef struct Framebuffer {
    PyObject_HEAD
    GCHeader * gc_prev;
    GCHeader * gc_next;
    Context * ctx;
    PyObject * attachments;
    PyObject * size;
    uint32_t framebuffer;
    int32_t width;
    int32_t height;
    int8_t multisampled;
    int32_t color_count;
    const ImageFormat * color_formats[MAX_ATTACHMENTS];
    const ImageFormat * depth_format;
    ClearValue clear_colors[MAX_ATTACHMENTS];
    float clear_depth;
    int32_t clear_stencil;
    Region viewport;
    int8_t has_scissor;
    Region scissor;
    int8_t external;
} Framebuffer;

typedef struct Pipeline {
    PyObject_HEAD
    GCHeader * gc_prev;
    GCHeader * gc_next;
    Context * ctx;
    Program * program;
    VertexArray * vertex_array;
    Framebuffer * framebuffer;
    PyObject * resources;
    char * uniform_data;
    int32_t * uniform_offsets;
    int32_t uniform_count;
    int32_t vertices;
    int32_t instances;
    int32_t first;
    int32_t first_instance;
    int32_t group_count[3];
    int8_t compute;
    uint32_t mode;
    int8_t has_viewport;
    Region viewport;
    int8_t has_scissor;
    Region scissor;
    PolygonOffset polygon_offset;
    int8_t depth_test;
    int8_t depth_write;
    uint32_t depth_func;
    uint32_t cull_face;
    BlendState blend;
    StencilState stencil;
} Pipeline;

typedef struct ReadRequest {
    PyObject_HEAD
    Context * ctx;
    PyObject * data;
    void * sync;
    uint32_t buffer;
    int32_t size;
} ReadRequest;

struct Context {
    PyObject_HEAD
    GCHeader * gc_prev;
    GCHeader * gc_next;
    ModuleState * module_state;
    int32_t max_vertex_attribs;
    PyObject * shaders;
    PyObject * info;
    PyObject * shader_cache;
    VertexArray * empty_vertex_array;
    uint32_t default_texture_unit;
    int32_t max_samplers;
    int32_t max_uniform_buffers;
    int32_t max_storage_buffers;
    int32_t max_storage_images;
    int32_t uniform_buffer_offset_alignment;
    int32_t storage_buffer_offset_alignment;
    int8_t depth_test;
    int8_t depth_write;
    uint32_t depth_func;
    uint32_t cull_face;
    uint32_t stencil_write_mask;
    Region viewport;
    int8_t scissor_test;
    Region scissor;
    PolygonOffset polygon_offset;
    Program * bound_program;
    Framebuffer * bound_framebuffer;
    VertexArray * bound_vertex_array;
};

static const NamedValue compare_funcs[] = {
    {"never", 0x0200},
    {"less", 0x0201},
    {"equal", 0x0202},
    {"lequal", 0x0203},
    {"greater", 0x0204},
    {"notequal", 0x0205},
    {"gequal", 0x0206},
    {"always", 0x0207},
    {NULL},
};

static const NamedValue stencil_ops[] = {
    {"keep", 0x1E00},
    {"zero", 0x0000},
    {"replace", 0x1E01},
    {"incr", 0x1E02},
    {"incr_wrap", 0x8507},
    {"decr", 0x1E03},
    {"decr_wrap", 0x8508},
    {"invert", 0x150A},
    {NULL},
};

static const NamedValue modes[] = {
    {"points", 0x0000},
    {"lines", 0x0001},
    {"line_loop", 0x0002},
    {"line_strip", 0x0003},
    {"triangles", 0x0004},
    {"triangle_strip", 0x0005},
    {"triangle_fan", 0x0006},
    {NULL},
};

static const NamedValue cull_faces[] = {
    {"front", 0x0404},
    {"back", 0x0405},
    {"front_and_back", 0x0408},
    {NULL},
};

static const NamedValue blend_factors[] = {
    {"zero", 0x0000},
    {"one", 0x0001},
    {"src_color", 0x0300},
    {"one_minus_src_color", 0x0301},
    {"dst_color", 0x0306},
    {"one_minus_dst_color", 0x0307},
    {"src_alpha", 0x0302},
    {"one_minus_src_alpha", 0x0303},
    {"dst_alpha", 0x0304},
    {"one_minus_dst_alpha", 0x0305},
    {"constant_color", 0x8001},
    {"one_minus_constant_color", 0x8002},
    {"constant_alpha", 0x8003},
    {"one_minus_constant_alpha", 0x8004},
    {"src_alpha_saturate", 0x0308},
    {NULL},
};

static const NamedValue blend_equations[] = {
    {"add", 0x8006},
    {"subtract", 0x800A},
    {"reverse_subtract", 0x800B},
    {"min", 0x8007},
    {"max", 0x8008},
    {NULL},
};

static const NamedValue filters[] = {
    {"nearest", 0x2600},
    {"linear", 0x2601},
    {"nearest_mipmap_nearest", 0x2700},
    {"linear_mipmap_nearest", 0x2701},
    {"nearest_mipmap_linear", 0x2702},
    {"linear_mipmap_linear", 0x2703},
    {NULL},
};

static const NamedValue wraps[] = {
    {"repeat", 0x2901},
    {"clamp_to_edge", 0x812F},
    {"mirrored_repeat", 0x8370},
    {NULL},
};

static const NamedValue buffer_usages[] = {
    {"stream_draw", 0x88E0},
    {"stream_read", 0x88E1},
    {"stream_copy", 0x88E2},
    {"static_draw", 0x88E4},
    {"static_read", 0x88E5},
    {"static_copy", 0x88E6},
    {"dynamic_draw", 0x88E8},
    {"dynamic_read", 0x88E9},
    {"dynamic_copy", 0x88EA},
    {NULL},
};

static const NamedValue uniform_types[] = {
    {"float", 0x1406},
    {"vec2", 0x8B50},
    {"vec3", 0x8B51},
    {"vec4", 0x8B52},
    {"int", 0x1404},
    {"ivec2", 0x8B53},
    {"ivec3", 0x8B54},
    {"ivec4", 0x8B55},
    {"uint", 0x1405},
    {"uvec2", 0x8DC6},
    {"uvec3", 0x8DC7},
    {"uvec4", 0x8DC8},
    {"bool", 0x8B56},
    {"bvec2", 0x8B57},
    {"bvec3", 0x8B58},
    {"bvec4", 0x8B59},
    {"mat2", 0x8B5A},
    {"mat2x3", 0x8B65},
    {"mat2x4", 0x8B66},
    {"mat3x2", 0x8B67},
    {"mat3", 0x8B5B},
    {"mat3x4", 0x8B68},
    {"mat4x2", 0x8B69},
    {"mat4x3", 0x8B6A},
    {"mat4", 0x8B5C},
    {NULL},
};

static const NamedValue sampler_types[] = {
    {"sampler1D", 0x8B5D},
    {"sampler2D", 0x8B5E},
    {"sampler3D", 0x8B5F},
    {"samplerCube", 0x8B60},
    {"sampler1DShadow", 0x8B61},
    {"sampler2DShadow", 0x8B62},
    {"sampler2DRect", 0x8B63},
    {"sampler2DRectShadow", 0x8B64},
    {"sampler1DArray", 0x8DC0},
    {"sampler2DArray", 0x8DC1},
    {"samplerBuffer", 0x8DC2},
    {"sampler1DArrayShadow", 0x8DC3},
    {"sampler2DArrayShadow", 0x8DC4},
    {"samplerCubeShadow", 0x8DC5},
    {"isampler1D", 0x8DC9},
    {"isampler2D", 0x8DCA},
    {"isampler3D", 0x8DCB},
    {"isamplerCube", 0x8DCC},
    {"isampler2DRect", 0x8DCD},
    {"isampler1DArray", 0x8DCE},
    {"isampler2DArray", 0x8DCF},
    {"isamplerBuffer", 0x8DD0},
    {"usampler1D", 0x8DD1},
    {"usampler2D", 0x8DD2},
    {"usampler3D", 0x8DD3},
    {"usamplerCube", 0x8DD4},
    {"usampler2DRect", 0x8DD5},
    {"usampler1DArray", 0x8DD6},
    {"usampler2DArray", 0x8DD7},
    {"usamplerBuffer", 0x8DD8},
    {"sampler2DMS", 0x9108},
    {"isampler2DMS", 0x9109},
    {"usampler2DMS", 0x910A},
    {"sampler2DMSArray", 0x910B},
    {"isampler2DMSArray", 0x910C},
    {"usampler2DMSArray", 0x910D},
    {NULL},
};

static const VertexFormat vertex_formats[] = {
    {"uint8x2", 0x1401, 2, 0, 1, 2},
    {"uint8x4", 0x1401, 4, 0, 1, 4},
    {"sint8x2", 0x1400, 2, 0, 1, 2},
    {"sint8x4", 0x1400, 4, 0, 1, 4},
    {"unorm8x2", 0x1401, 2, 1, 0, 2},
    {"unorm8x4", 0x1401, 4, 1, 0, 4},
    {"snorm8x2", 0x1400, 2, 1, 0, 2},
    {"snorm8x4", 0x1400, 4, 1, 0, 4},
    {"uint16x2", 0x1403, 2, 0, 1, 4},
    {"uint16x4", 0x1403, 4, 0, 1, 8},
    {"sint16x2", 0x1402, 2, 0, 1, 4},
    {"sint16x4", 0x1402, 4, 0, 1, 8},
    {"unorm16x2", 0x1403, 2, 1, 0, 4},
    {"unorm16x4", 0x1403, 4, 1, 0, 8},
    {"snorm16x2", 0x1402, 2, 1, 0, 4},
    {"snorm16x4", 0x1402, 4, 1, 0, 8},
    {"float16x2", 0x140B, 2, 0, 0, 4},
    {"float16x4", 0x140B, 4, 0, 0, 8},
    {"float32", 0x1406, 1, 0, 0, 4},
    {"float32x2", 0x1406, 2, 0, 0, 8},
    {"float32x3", 0x1406, 3, 0, 0, 12},
    {"float32x4", 0x1406, 4, 0, 0, 16},
    {"uint32", 0x1405, 1, 0, 1, 4},
    {"uint32x2", 0x1405, 2, 0, 1, 8},
    {"uint32x3", 0x1405, 3, 0, 1, 12},
    {"uint32x4", 0x1405, 4, 0, 1, 16},
    {"sint32", 0x1404, 1, 0, 1, 4},
    {"sint32x2", 0x1404, 2, 0, 1, 8},
    {"sint32x3", 0x1404, 3, 0, 1, 12},
    {"sint32x4", 0x1404, 4, 0, 1, 16},
    {NULL},
};

static const ShortVertexFormat short_vertex_formats[] = {
    {"2u1", "uint8x2"},
    {"4u1", "uint8x4"},
    {"2i1", "sint8x2"},
    {"4i1", "sint8x4"},
    {"2nu1", "unorm8x2"},
    {"4nu1", "unorm8x4"},
    {"2ni1", "snorm8x2"},
    {"4ni1", "snorm8x4"},
    {"2u2", "uint16x2"},
    {"4u2", "uint16x4"},
    {"2i2", "sint16x2"},
    {"4i2", "sint16x4"},
    {"2nu2", "unorm16x2"},
    {"4nu2", "unorm16x4"},
    {"2ni2", "snorm16x2"},
    {"4ni2", "snorm16x4"},
    {"2h", "float16x2"},
    {"4h", "float16x4"},
    {"1f", "float32"},
    {"2f", "float32x2"},
    {"3f", "float32x3"},
    {"4f", "float32x4"},
    {"1u", "uint32"},
    {"2u", "uint32x2"},
    {"3u", "uint32x3"},
    {"4u", "uint32x4"},
    {"1i", "sint32"},
    {"2i", "sint32x2"},
    {"3i", "sint32x3"},
    {"4i", "sint32x4"},
    {NULL},
};

static const ImageFormat image_formats[] = {
    {"r8unorm", 0x8229, 0x1903, 0x1401, 0x1800, 1, 1, 1, 'f'},
    {"rg8unorm", 0x822B, 0x8227, 0x1401, 0x1800, 2, 2, 1, 'f'},
    {"rgba8unorm", 0x8058, 0x1908, 0x1401, 0x1800, 4, 4, 1, 'f'},
    {"bgra8unorm", 0x8058, 0x80E1, 0x1401, 0x1800, 4, 4, 1, 'f'},
    {"r8snorm", 0x8F94, 0x1903, 0x1400, 0x1800, 1, 1, 1, 'f'},
    {"rg8snorm", 0x8F95, 0x8227, 0x1400, 0x1800, 2, 2, 1, 'f'},
    {"rgba8snorm", 0x8F97, 0x1908, 0x1400, 0x1800, 4, 4, 1, 'f'},
    {"r8uint", 0x8232, 0x8D94, 0x1401, 0x1800, 1, 1, 1, 'u'},
    {"rg8uint", 0x8238, 0x8228, 0x1401, 0x1800, 2, 2, 1, 'u'},
    {"rgba8uint", 0x8D7C, 0x8D99, 0x1401, 0x1800, 4, 4, 1, 'u'},
    {"r16uint", 0x8234, 0x8D94, 0x1403, 0x1800, 1, 2, 1, 'u'},
    {"rg16uint", 0x823A, 0x8228, 0x1403, 0x1800, 2, 4, 1, 'u'},
    {"rgba16uint", 0x8D76, 0x8D99, 0x1403, 0x1800, 4, 8, 1, 'u'},
    {"r32uint", 0x8236, 0x8D94, 0x1405, 0x1800, 1, 4, 1, 'u'},
    {"rg32uint", 0x823C, 0x8228, 0x1405, 0x1800, 2, 8, 1, 'u'},
    {"rgba32uint", 0x8D70, 0x8D99, 0x1405, 0x1800, 4, 16, 1, 'u'},
    {"r8sint", 0x8231, 0x8D94, 0x1400, 0x1800, 1, 1, 1, 'i'},
    {"rg8sint", 0x8237, 0x8228, 0x1400, 0x1800, 2, 2, 1, 'i'},
    {"rgba8sint", 0x8D8E, 0x8D99, 0x1400, 0x1800, 4, 4, 1, 'i'},
    {"r16sint", 0x8233, 0x8D94, 0x1402, 0x1800, 1, 2, 1, 'i'},
    {"rg16sint", 0x8239, 0x8228, 0x1402, 0x1800, 2, 4, 1, 'i'},
    {"rgba16sint", 0x8D88, 0x8D99, 0x1402, 0x1800, 4, 8, 1, 'i'},
    {"r32sint", 0x8235, 0x8D94, 0x1404, 0x1800, 1, 4, 1, 'i'},
    {"rg32sint", 0x823B, 0x8228, 0x1404, 0x1800, 2, 8, 1, 'i'},
    {"rgba32sint", 0x8D82, 0x8D99, 0x1404, 0x1800, 4, 16, 1, 'i'},
    {"r16float", 0x822D, 0x1903, 0x140B, 0x1800, 1, 2, 1, 'f'},
    {"rg16float", 0x822F, 0x8227, 0x140B, 0x1800, 2, 4, 1, 'f'},
    {"rgba16float", 0x881A, 0x1908, 0x140B, 0x1800, 4, 8, 1, 'f'},
    {"r32float", 0x822E, 0x1903, 0x1406, 0x1800, 1, 4, 1, 'f'},
    {"rg32float", 0x8230, 0x8227, 0x1406, 0x1800, 2, 8, 1, 'f'},
    {"rgba32float", 0x8814, 0x1908, 0x1406, 0x1800, 4, 16, 1, 'f'},
    {"rgb10a2unorm", 0x8059, 0x1908, 0x8368, 0x1800, 4, 4, 1, 'f'},
    {"depth16unorm", 0x81A5, 0x1902, 0x1403, 0x1801, 1, 2, 0, 'f'},
    {"depth24plus", 0x81A6, 0x1902, 0x1405, 0x1801, 1, 4, 0, 'f'},
    {"depth24plus-stencil8", 0x88F0, 0x84F9, 0x84FA, 0x84F9, 2, 4, 0, 'x'},
    {"depth32float", 0x8CAC, 0x1902, 0x1406, 0x1801, 1, 4, 0, 'f'},
    {NULL},
};

static PyObject * release_object(Context * self, PyObject * arg);
static PyObject * release_context(Context * self);
static PyObject * Context_get_screen(Context * self, void * closure);

static PyObject * meth_init(PyObject * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"loader", NULL};

    PyObject * loader = Py_None;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", keywords, &loader)) {
        return NULL;
    }

    PyMutex_Lock(&gl_mutex);
    int ok = load_gl(loader);
    PyMutex_Unlock(&gl_mutex);

    if (!ok) {
        return NULL;
    }

    Py_RETURN_NONE;
}

static int get_named_value(const NamedValue * table, const char * name, uint32_t * value) {
    for (const NamedValue * it = table; it->name; ++it) {
        if (!strcmp(name, it->name)) {
            *value = it->value;
            return 1;
        }
    }
    return 0;
}

static const char * get_value_name(const NamedValue * table, uint32_t value) {
    for (const NamedValue * it = table; it->name; ++it) {
        if (it->value == value) {
            return it->name;
        }
    }
    return NULL;
}

static int parse_named_value(const NamedValue * table, PyObject * obj, uint32_t * value, const char * what) {
    const char * name = PyUnicode_Check(obj) ? PyUnicode_AsUTF8(obj) : NULL;
    if (!name || !get_named_value(table, name, value)) {
        PyErr_Clear();
        PyErr_Format(PyExc_ValueError, "invalid %s %R", what, obj);
        return 0;
    }
    return 1;
}

static int parse_optional(const NamedValue * table, PyObject * obj, uint32_t * value, const char * what) {
    return obj == Py_None || parse_named_value(table, obj, value, what);
}

static const ImageFormat * get_image_format(const char * name) {
    for (const ImageFormat * fmt = image_formats; fmt->name; ++fmt) {
        if (!strcmp(name, fmt->name)) {
            return fmt;
        }
    }
    return NULL;
}

static const VertexFormat * get_vertex_format(const char * name) {
    for (const VertexFormat * fmt = vertex_formats; fmt->name; ++fmt) {
        if (!strcmp(name, fmt->name)) {
            return fmt;
        }
    }
    return NULL;
}

static const ShortVertexFormat * get_short_vertex_format(const char * name) {
    for (const ShortVertexFormat * fmt = short_vertex_formats; fmt->name; ++fmt) {
        if (!strcmp(name, fmt->name)) {
            return fmt;
        }
    }
    return NULL;
}

static int32_t least_one(int32_t value) {
    return value > 1 ? value : 1;
}

static int32_t count_mipmaps(int32_t width, int32_t height) {
    int32_t size = width > height ? width : height;
    int32_t levels = 0;
    while (size) {
        levels += 1;
        size /= 2;
    }
    return levels;
}

static int to_int_pair(int32_t * x, int32_t * y, PyObject * obj, int32_t default_x, int32_t default_y) {
    if (obj == Py_None) {
        *x = default_x;
        *y = default_y;
        return 1;
    }
    if (!PyTuple_Check(obj) || PyTuple_Size(obj) != 2) {
        return 0;
    }
    *x = (int32_t)PyLong_AsLong(PyTuple_GetItem(obj, 0));
    *y = (int32_t)PyLong_AsLong(PyTuple_GetItem(obj, 1));
    if (PyErr_Occurred()) {
        PyErr_Clear();
        return 0;
    }
    return 1;
}

static int32_t get_limit(uint32_t pname, int32_t low, int32_t high) {
    int32_t value = low;
    glGetIntegerv(pname, &value);
    if (value < low) {
        return low;
    }
    if (value > high) {
        return high;
    }
    return value;
}

static void track_object(Context * ctx, GCHeader * obj) {
    Py_BEGIN_CRITICAL_SECTION(ctx);
    obj->gc_prev = ctx->gc_prev;
    obj->gc_next = (GCHeader *)ctx;
    obj->gc_prev->gc_next = obj;
    ctx->gc_prev = obj;
    Py_INCREF(obj);
    Py_END_CRITICAL_SECTION();
}

static int untrack_object(Context * ctx, GCHeader * obj) {
    int tracked = 0;
    Py_BEGIN_CRITICAL_SECTION(ctx);
    if (obj->gc_prev) {
        obj->gc_prev->gc_next = obj->gc_next;
        obj->gc_next->gc_prev = obj->gc_prev;
        obj->gc_prev = NULL;
        obj->gc_next = NULL;
        tracked = 1;
    }
    Py_END_CRITICAL_SECTION();
    return tracked;
}

static int parse_blend(PyObject * args, PyObject * kwargs, BlendState * res) {
    static char * keywords[] = {"src", "dst", "equation", "src_alpha", "dst_alpha", "equation_alpha", NULL};

    PyObject * src_arg = Py_None;
    PyObject * dst_arg = Py_None;
    PyObject * equation_arg = Py_None;
    PyObject * src_alpha_arg = Py_None;
    PyObject * dst_alpha_arg = Py_None;
    PyObject * equation_alpha_arg = Py_None;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "|OO$OOOO", keywords,
        &src_arg, &dst_arg, &equation_arg,
        &src_alpha_arg, &dst_alpha_arg, &equation_alpha_arg
    );

    if (!args_ok) {
        return 0;
    }

    memset(res, 0, sizeof(BlendState));

    if (src_arg == Py_None && dst_arg == Py_None) {
        return 1;
    }

    if (src_arg == Py_None || dst_arg == Py_None) {
        PyErr_Format(PyExc_TypeError, "both src and dst are required");
        return 0;
    }

    res->enabled = 1;
    res->equation = GL_FUNC_ADD;

    if (!parse_named_value(blend_factors, src_arg, &res->src, "blend factor")) {
        return 0;
    }

    if (!parse_named_value(blend_factors, dst_arg, &res->dst, "blend factor")) {
        return 0;
    }

    if (!parse_optional(blend_equations, equation_arg, &res->equation, "blend equation")) {
        return 0;
    }

    res->src_alpha = res->src;
    res->dst_alpha = res->dst;
    res->equation_alpha = res->equation;

    if (!parse_optional(blend_factors, src_alpha_arg, &res->src_alpha, "blend factor")) {
        return 0;
    }

    if (!parse_optional(blend_factors, dst_alpha_arg, &res->dst_alpha, "blend factor")) {
        return 0;
    }

    if (!parse_optional(blend_equations, equation_alpha_arg, &res->equation_alpha, "blend equation")) {
        return 0;
    }

    return 1;
}

static void apply_blend(BlendState * state) {
    if (!state->enabled) {
        glDisable(GL_BLEND);
        return;
    }
    glEnable(GL_BLEND);
    glBlendFuncSeparate(state->src, state->dst, state->src_alpha, state->dst_alpha);
    glBlendEquationSeparate(state->equation, state->equation_alpha);
}

static int parse_stencil(PyObject * args, PyObject * kwargs, StencilState * res) {
    static char * keywords[] = {
        "func", "ref", "read_mask", "write_mask", "fail", "depth_fail", "depth_pass", "back_func", "back_fail",
        "back_depth_fail", "back_depth_pass", NULL,
    };

    PyObject * func_arg = Py_None;
    PyObject * fail_arg = Py_None;
    PyObject * depth_fail_arg = Py_None;
    PyObject * depth_pass_arg = Py_None;
    PyObject * back_func_arg = Py_None;
    PyObject * back_fail_arg = Py_None;
    PyObject * back_depth_fail_arg = Py_None;
    PyObject * back_depth_pass_arg = Py_None;

    memset(res, 0, sizeof(StencilState));
    res->read_mask = 0xFF;
    res->write_mask = 0xFF;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "|Oi$IIOOOOOOO", keywords,
        &func_arg, &res->ref, &res->read_mask, &res->write_mask,
        &fail_arg, &depth_fail_arg, &depth_pass_arg,
        &back_func_arg, &back_fail_arg, &back_depth_fail_arg, &back_depth_pass_arg
    );

    if (!args_ok) {
        return 0;
    }

    if (func_arg == Py_None) {
        res->enabled = 0;
        return 1;
    }

    res->enabled = 1;
    res->fail = GL_KEEP;
    res->depth_fail = GL_KEEP;
    res->depth_pass = GL_KEEP;

    if (!parse_named_value(compare_funcs, func_arg, &res->func, "stencil func")) {
        return 0;
    }

    if (!parse_optional(stencil_ops, fail_arg, &res->fail, "stencil op")) {
        return 0;
    }

    if (!parse_optional(stencil_ops, depth_fail_arg, &res->depth_fail, "stencil op")) {
        return 0;
    }

    if (!parse_optional(stencil_ops, depth_pass_arg, &res->depth_pass, "stencil op")) {
        return 0;
    }

    res->back_func = res->func;
    res->back_fail = res->fail;
    res->back_depth_fail = res->depth_fail;
    res->back_depth_pass = res->depth_pass;

    if (!parse_optional(compare_funcs, back_func_arg, &res->back_func, "stencil func")) {
        return 0;
    }

    if (!parse_optional(stencil_ops, back_fail_arg, &res->back_fail, "stencil op")) {
        return 0;
    }

    if (!parse_optional(stencil_ops, back_depth_fail_arg, &res->back_depth_fail, "stencil op")) {
        return 0;
    }

    if (!parse_optional(stencil_ops, back_depth_pass_arg, &res->back_depth_pass, "stencil op")) {
        return 0;
    }

    return 1;
}

static void apply_stencil(Context * ctx, StencilState * state) {
    if (!state->enabled) {
        glDisable(GL_STENCIL_TEST);
        return;
    }
    glEnable(GL_STENCIL_TEST);
    glStencilFuncSeparate(GL_FRONT, state->func, state->ref, state->read_mask);
    glStencilFuncSeparate(GL_BACK, state->back_func, state->ref, state->read_mask);
    glStencilOpSeparate(GL_FRONT, state->fail, state->depth_fail, state->depth_pass);
    glStencilOpSeparate(GL_BACK, state->back_fail, state->back_depth_fail, state->back_depth_pass);
    glStencilMask(state->write_mask);
    ctx->stencil_write_mask = state->write_mask;
}

static uint32_t bound_framebuffer_object(Context * ctx) {
    return ctx->bound_framebuffer ? ctx->bound_framebuffer->framebuffer : 0;
}

static uint32_t bound_vertex_array_object(Context * ctx) {
    return ctx->bound_vertex_array ? ctx->bound_vertex_array->vertex_array : 0;
}

static uint32_t bound_program_object(Context * ctx) {
    return ctx->bound_program ? ctx->bound_program->program : 0;
}

static void apply_viewport(Context * ctx, Region * viewport) {
    if (memcmp(&ctx->viewport, viewport, sizeof(Region))) {
        ctx->viewport = *viewport;
        glViewport(viewport->x, viewport->y, viewport->width, viewport->height);
    }
}

static void apply_scissor(Context * ctx, int8_t enabled, Region * scissor) {
    if (ctx->scissor_test != enabled) {
        ctx->scissor_test = enabled;
        if (enabled) {
            glEnable(GL_SCISSOR_TEST);
        } else {
            glDisable(GL_SCISSOR_TEST);
        }
    }
    if (enabled && memcmp(&ctx->scissor, scissor, sizeof(Region))) {
        ctx->scissor = *scissor;
        glScissor(scissor->x, scissor->y, scissor->width, scissor->height);
    }
}

static void suspend_scissor(Context * ctx) {
    if (ctx->scissor_test) {
        glDisable(GL_SCISSOR_TEST);
    }
}

static void resume_scissor(Context * ctx) {
    if (ctx->scissor_test) {
        glEnable(GL_SCISSOR_TEST);
    }
}

static void apply_polygon_offset(Context * ctx, PolygonOffset * state) {
    ctx->polygon_offset = *state;
    if (state->enabled) {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(state->factor, state->units);
    } else {
        glDisable(GL_POLYGON_OFFSET_FILL);
    }
}

static int parse_polygon_offset(PyObject * value, PolygonOffset * state) {
    if (value == Py_None) {
        state->enabled = 0;
        state->factor = 0.0f;
        state->units = 0.0f;
        return 1;
    }
    if (!PyArg_ParseTuple(value, "ff", &state->factor, &state->units)) {
        PyErr_Clear();
        PyErr_Format(PyExc_TypeError, "the polygon_offset must be a tuple of 2 floats or None");
        return 0;
    }
    state->enabled = 1;
    return 1;
}

static PyObject * polygon_offset_value(PolygonOffset * state) {
    if (!state->enabled) {
        Py_RETURN_NONE;
    }
    return Py_BuildValue("(ff)", state->factor, state->units);
}

static int parse_rect(PyObject * value, Region * region, const char * name) {
    if (!value || !PyArg_ParseTuple(value, "iiii", &region->x, &region->y, &region->width, &region->height)) {
        PyErr_Clear();
        PyErr_Format(PyExc_TypeError, "the %s must be a tuple of 4 ints or None", name);
        return 0;
    }
    return 1;
}

static void bind_framebuffer(Context * ctx, Framebuffer * framebuffer) {
    if (ctx->bound_framebuffer != framebuffer) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->framebuffer);
        Py_XSETREF(ctx->bound_framebuffer, (Framebuffer *)Py_NewRef(framebuffer));
    }
}

static void restore_framebuffer(Context * ctx) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, bound_framebuffer_object(ctx));
}

static void restore_vertex_array(Context * ctx) {
    glBindVertexArray(bound_vertex_array_object(ctx));
}

static int parse_buffer_range(Context * ctx, PyObject * obj, BufferRange * res, int storage) {
    PyObject * buffer = obj;
    res->offset = 0;
    res->size = -1;
    if (PyTuple_Check(obj)) {
        if (!PyArg_ParseTuple(obj, "Oii", &buffer, &res->offset, &res->size)) {
            PyErr_Clear();
            buffer = NULL;
        }
    }
    if (!buffer || Py_TYPE(buffer) != ctx->module_state->Buffer_type) {
        PyErr_Format(PyExc_TypeError, "expected a Buffer or a (buffer, offset, size) view");
        return 0;
    }
    res->buffer = (Buffer *)buffer;
    if (res->size < 0) {
        res->size = res->buffer->size - res->offset;
    }
    if (res->buffer->index) {
        PyErr_Format(PyExc_TypeError, "index buffers can only be used as index buffers");
        return 0;
    }
    if (res->offset < 0 || res->size <= 0 || res->offset + res->size > res->buffer->size) {
        PyErr_Format(PyExc_ValueError, "invalid size or offset");
        return 0;
    }
    int32_t alignment = storage ? ctx->storage_buffer_offset_alignment : ctx->uniform_buffer_offset_alignment;
    if (res->offset % alignment) {
        PyErr_Format(PyExc_ValueError, "the offset must be a multiple of %d", alignment);
        return 0;
    }
    return 1;
}

static uint32_t attachment_point(const ImageFormat * fmt) {
    if (fmt->buffer == GL_DEPTH) {
        return GL_DEPTH_ATTACHMENT;
    }
    if (fmt->buffer == GL_DEPTH_STENCIL) {
        return GL_DEPTH_STENCIL_ATTACHMENT;
    }
    return GL_COLOR_ATTACHMENT0;
}

static void attach_texture(uint32_t attachment, Texture * texture, int32_t layer, int32_t level) {
    if (texture->multisampled) {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, texture->texture);
    } else if (texture->cubemap) {
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, texture->texture, level
        );
    } else if (texture->array) {
        glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, texture->texture, level, layer);
    } else {
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->texture, level);
    }
}

static uint32_t face_framebuffer(Texture * texture, int32_t layer, int32_t level) {
    uint32_t framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    attach_texture(attachment_point(texture->fmt), texture, layer, level);

    uint32_t buffer = texture->fmt->color ? GL_COLOR_ATTACHMENT0 : GL_NONE;
    glReadBuffer(buffer);
    glDrawBuffers(1, &buffer);

    uint32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    restore_framebuffer(texture->ctx);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        glDeleteFramebuffers(1, &framebuffer);
        PyErr_Format(PyExc_RuntimeError, "incomplete framebuffer 0x%x", status);
        return 0;
    }

    return framebuffer;
}

static uint32_t texture_framebuffer(Texture * texture) {
    if (texture->array || texture->cubemap) {
        PyErr_Format(PyExc_TypeError, "layered textures have no framebuffer");
        return 0;
    }
    if (!texture->framebuffer) {
        texture->framebuffer = face_framebuffer(texture, 0, 0);
    }
    return texture->framebuffer;
}

static int parse_region(Texture * texture, int32_t level, PyObject * size_arg, PyObject * offset_arg, Region * region) {
    int32_t level_width = least_one(texture->width >> level);
    int32_t level_height = least_one(texture->height >> level);

    if (!to_int_pair(&region->width, &region->height, size_arg, level_width, level_height)) {
        PyErr_Format(PyExc_TypeError, "the size must be a tuple of 2 ints");
        return 0;
    }

    if (!to_int_pair(&region->x, &region->y, offset_arg, 0, 0)) {
        PyErr_Format(PyExc_TypeError, "the offset must be a tuple of 2 ints");
        return 0;
    }

    if (size_arg == Py_None && offset_arg != Py_None) {
        PyErr_Format(PyExc_ValueError, "the size is required when the offset is not None");
        return 0;
    }

    if (region->width <= 0 || region->height <= 0 || region->x < 0 || region->y < 0) {
        PyErr_Format(PyExc_ValueError, "invalid size or offset");
        return 0;
    }

    if (region->x + region->width > level_width || region->y + region->height > level_height) {
        PyErr_Format(PyExc_ValueError, "invalid size or offset");
        return 0;
    }

    return 1;
}

static int32_t region_bytes(Texture * texture, Region * region) {
    return ((region->width * texture->fmt->pixel_size + 3) & ~3) * region->height;
}

static Framebuffer * new_framebuffer(
    Context * ctx, uint32_t framebuffer, PyObject * attachments, int32_t width, int32_t height, int8_t multisampled
) {
    Framebuffer * res = PyObject_New(Framebuffer, ctx->module_state->Framebuffer_type);
    res->ctx = (Context *)Py_NewRef(ctx);
    res->attachments = attachments;
    res->size = Py_BuildValue("(ii)", width, height);
    res->framebuffer = framebuffer;
    res->width = width;
    res->height = height;
    res->multisampled = multisampled;
    res->gc_prev = NULL;
    res->gc_next = NULL;
    res->color_count = 0;
    memset(res->color_formats, 0, sizeof(res->color_formats));
    res->depth_format = NULL;
    memset(res->clear_colors, 0, sizeof(res->clear_colors));
    res->clear_depth = 1.0f;
    res->clear_stencil = 0;
    res->viewport.x = 0;
    res->viewport.y = 0;
    res->viewport.width = width;
    res->viewport.height = height;
    res->has_scissor = 0;
    memset(&res->scissor, 0, sizeof(res->scissor));
    res->external = 0;
    return res;
}

static void add_slot(Framebuffer * self, const ImageFormat * fmt) {
    if (fmt->color) {
        self->color_formats[self->color_count] = fmt;
        self->color_count += 1;
    } else {
        self->depth_format = fmt;
    }
}

static int parse_clear_color(const ImageFormat * fmt, PyObject * value, ClearValue * res) {
    PyObject * values = fmt->components == 1 ? PyTuple_Pack(1, value) : PySequence_Tuple(value);
    if (!values) {
        PyErr_Clear();
        PyErr_Format(PyExc_TypeError, "the clear color of a %d component format must be a tuple", fmt->components);
        return 0;
    }

    if (PyTuple_Size(values) != fmt->components) {
        Py_DECREF(values);
        PyErr_Format(PyExc_ValueError, "the clear color must have %d components", fmt->components);
        return 0;
    }

    for (int32_t i = 0; i < fmt->components; ++i) {
        PyObject * item = PyTuple_GetItem(values, i);
        if (fmt->clear_type == 'f') {
            res->clear_floats[i] = (float)PyFloat_AsDouble(item);
        } else if (fmt->clear_type == 'i') {
            res->clear_ints[i] = (int32_t)PyLong_AsLong(item);
        } else {
            res->clear_uints[i] = (uint32_t)PyLong_AsUnsignedLong(item);
        }
    }

    Py_DECREF(values);

    if (PyErr_Occurred()) {
        PyErr_Clear();
        PyErr_Format(PyExc_TypeError, "invalid clear color");
        return 0;
    }

    return 1;
}

static PyObject * clear_color_object(const ImageFormat * fmt, ClearValue * value) {
    if (fmt->components == 1) {
        if (fmt->clear_type == 'f') {
            return PyFloat_FromDouble(value->clear_floats[0]);
        }
        if (fmt->clear_type == 'i') {
            return PyLong_FromLong(value->clear_ints[0]);
        }
        return PyLong_FromUnsignedLong(value->clear_uints[0]);
    }

    PyObject * res = PyTuple_New(fmt->components);
    for (int32_t i = 0; i < fmt->components; ++i) {
        if (fmt->clear_type == 'f') {
            PyTuple_SetItem(res, i, PyFloat_FromDouble(value->clear_floats[i]));
        } else if (fmt->clear_type == 'i') {
            PyTuple_SetItem(res, i, PyLong_FromLong(value->clear_ints[i]));
        } else {
            PyTuple_SetItem(res, i, PyLong_FromUnsignedLong(value->clear_uints[i]));
        }
    }
    return res;
}

static void clear_slot(Framebuffer * self, int32_t index) {
    if (index < self->color_count) {
        ClearValue * value = &self->clear_colors[index];
        char clear_type = self->color_formats[index]->clear_type;
        if (clear_type == 'f') {
            glClearBufferfv(GL_COLOR, index, value->clear_floats);
        } else if (clear_type == 'i') {
            glClearBufferiv(GL_COLOR, index, value->clear_ints);
        } else {
            glClearBufferuiv(GL_COLOR, index, value->clear_uints);
        }
        return;
    }

    if (self->depth_format->buffer == GL_DEPTH) {
        glClearBufferfv(GL_DEPTH, 0, &self->clear_depth);
        return;
    }

    glClearBufferfi(GL_DEPTH_STENCIL, 0, self->clear_depth, self->clear_stencil);
}

static int parse_clear_attachments(PyObject * attachments, int32_t slot_count, int32_t * indices, int32_t * count) {
    PyObject * seq = PySequence_Fast(attachments, "attachments must be a list of ints");
    if (!seq) {
        return 0;
    }

    *count = (int32_t)PySequence_Fast_GET_SIZE(seq);

    if (*count > slot_count) {
        Py_DECREF(seq);
        PyErr_Format(PyExc_ValueError, "too many attachments");
        return 0;
    }

    for (int32_t i = 0; i < *count; ++i) {
        PyObject * item = PySequence_Fast_GET_ITEM(seq, i);
        int32_t index = PyLong_CheckExact(item) ? (int32_t)PyLong_AsLong(item) : slot_count;
        if (index < 0) {
            index += slot_count;
        }
        if (index < 0 || index >= slot_count) {
            Py_DECREF(seq);
            PyErr_Clear();
            PyErr_Format(PyExc_ValueError, "invalid attachment at %d", i);
            return 0;
        }
        indices[i] = index;
    }

    Py_DECREF(seq);
    return 1;
}

static int parse_mode(VertexArray * self, const char * mode_arg, uint32_t * mode) {
    *mode = self->mode;
    if (mode_arg && !get_named_value(modes, mode_arg, mode)) {
        PyErr_Format(PyExc_ValueError, "invalid mode \"%s\"", mode_arg);
        return 0;
    }
    return 1;
}

static PyObject * load_shader(PyObject * shaders, PyObject * name, int32_t depth) {
    if (depth > 32) {
        PyErr_Format(PyExc_ValueError, "too deep include chain at \"%U\"", name);
        return NULL;
    }

    PyObject * source = NULL;
    int found = PyDict_GetItemRef(shaders, name, &source);
    if (found <= 0) {
        if (!found) {
            PyErr_Format(PyExc_KeyError, "shader \"%U\" is not in ctx.shaders", name);
        }
        return NULL;
    }

    if (!PyUnicode_Check(source)) {
        Py_DECREF(source);
        PyErr_Format(PyExc_TypeError, "shader \"%U\" must be a str", name);
        return NULL;
    }

    PyObject * needle = PyUnicode_FromString("#include");
    Py_ssize_t has_include = PyUnicode_Find(source, needle, 0, PyUnicode_GetLength(source), 1);
    Py_DECREF(needle);
    if (has_include == -1) {
        return source;
    }

    PyObject * lines = PyUnicode_Splitlines(source, 1);
    Py_DECREF(source);
    PyObject * parts = PyList_New(0);
    int32_t count = (int32_t)PyList_Size(lines);

    for (int32_t i = 0; i < count; ++i) {
        PyObject * line = PyList_GetItem(lines, i);
        const char * text = PyUnicode_AsUTF8(line);

        while (*text == ' ' || *text == '\t') {
            text += 1;
        }

        if (strncmp(text, "#include", 8)) {
            PyList_Append(parts, line);
            continue;
        }

        const char * start = strchr(text, '"');
        const char * end = start ? strchr(start + 1, '"') : NULL;

        if (!end) {
            PyErr_Format(PyExc_ValueError, "invalid include in \"%U\" at line %d", name, i + 1);
            Py_DECREF(parts);
            Py_DECREF(lines);
            return NULL;
        }

        PyObject * include_name = PyUnicode_FromStringAndSize(start + 1, end - start - 1);
        PyObject * include = load_shader(shaders, include_name, depth + 1);
        Py_DECREF(include_name);

        if (!include) {
            Py_DECREF(parts);
            Py_DECREF(lines);
            return NULL;
        }

        PyList_Append(parts, include);
        Py_DECREF(include);

        PyObject * line_directive = PyUnicode_FromFormat("\n#line %d\n", i + 2);
        PyList_Append(parts, line_directive);
        Py_DECREF(line_directive);
    }

    Py_DECREF(lines);

    PyObject * empty = PyUnicode_FromString("");
    PyObject * res = PyUnicode_Join(empty, parts);
    Py_DECREF(empty);
    Py_DECREF(parts);
    return res;
}

static uint32_t compile_shader(PyObject * cache, uint32_t type, PyObject * shaders, PyObject * name) {
    PyObject * source = load_shader(shaders, name, 0);
    if (!source) {
        return 0;
    }

    PyObject * key = Py_BuildValue("(IN)", type, source);
    PyObject * cached = PyDict_GetItemWithError(cache, key);
    if (cached) {
        Py_DECREF(key);
        return (uint32_t)PyLong_AsUnsignedLong(cached);
    }

    uint32_t shader = glCreateShader(type);
    const char * text = PyUnicode_AsUTF8(source);
    glShaderSource(shader, 1, &text, NULL);
    glCompileShader(shader);

    int32_t compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        char log[4096] = {0};
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        glDeleteShader(shader);
        Py_DECREF(key);
        const char * stage = type == GL_VERTEX_SHADER ? "Vertex" : type == GL_COMPUTE_SHADER ? "Compute" : "Fragment";
        PyErr_Format(PyExc_ValueError, "%s Shader Error in \"%U\"\n\n%s", stage, name, log);
        return 0;
    }

    PyObject * value = PyLong_FromUnsignedLong(shader);
    PyDict_SetItem(cache, key, value);
    Py_DECREF(value);
    Py_DECREF(key);
    return shader;
}

static uint32_t compile_program(
    PyObject * cache, PyObject * shaders, PyObject * vertex_shader, PyObject * fragment_shader
) {
    uint32_t vs = compile_shader(cache, GL_VERTEX_SHADER, shaders, vertex_shader);
    if (!vs) {
        return 0;
    }

    uint32_t fs = compile_shader(cache, GL_FRAGMENT_SHADER, shaders, fragment_shader);
    if (!fs) {
        return 0;
    }

    uint32_t program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDetachShader(program, vs);
    glDetachShader(program, fs);

    int32_t linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[4096] = {0};
        glGetProgramInfoLog(program, sizeof(log), NULL, log);
        glDeleteProgram(program);
        PyErr_Format(PyExc_ValueError, "Linker Error in \"%U\" and \"%U\"\n\n%s", vertex_shader, fragment_shader, log);
        return 0;
    }

    return program;
}

static uint32_t compile_compute_program(PyObject * cache, PyObject * shaders, PyObject * compute_shader) {
    uint32_t cs = compile_shader(cache, GL_COMPUTE_SHADER, shaders, compute_shader);
    if (!cs) {
        return 0;
    }

    uint32_t program = glCreateProgram();
    glAttachShader(program, cs);
    glLinkProgram(program);
    glDetachShader(program, cs);

    int32_t linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[4096] = {0};
        glGetProgramInfoLog(program, sizeof(log), NULL, log);
        glDeleteProgram(program);
        PyErr_Format(PyExc_ValueError, "Linker Error in \"%U\"\n\n%s", compute_shader, log);
        return 0;
    }

    return program;
}

static int get_uniform_type(uint32_t gltype, int32_t * function, int32_t * items, char * type) {
    switch (gltype) {
        case GL_INT: *function = 0; *items = 1; *type = 'i'; return 1;
        case GL_INT_VEC2: *function = 1; *items = 2; *type = 'i'; return 1;
        case GL_INT_VEC3: *function = 2; *items = 3; *type = 'i'; return 1;
        case GL_INT_VEC4: *function = 3; *items = 4; *type = 'i'; return 1;
        case GL_BOOL: *function = 0; *items = 1; *type = 'i'; return 1;
        case GL_BOOL_VEC2: *function = 1; *items = 2; *type = 'i'; return 1;
        case GL_BOOL_VEC3: *function = 2; *items = 3; *type = 'i'; return 1;
        case GL_BOOL_VEC4: *function = 3; *items = 4; *type = 'i'; return 1;
        case GL_UNSIGNED_INT: *function = 4; *items = 1; *type = 'u'; return 1;
        case GL_UNSIGNED_INT_VEC2: *function = 5; *items = 2; *type = 'u'; return 1;
        case GL_UNSIGNED_INT_VEC3: *function = 6; *items = 3; *type = 'u'; return 1;
        case GL_UNSIGNED_INT_VEC4: *function = 7; *items = 4; *type = 'u'; return 1;
        case GL_FLOAT: *function = 8; *items = 1; *type = 'f'; return 1;
        case GL_FLOAT_VEC2: *function = 9; *items = 2; *type = 'f'; return 1;
        case GL_FLOAT_VEC3: *function = 10; *items = 3; *type = 'f'; return 1;
        case GL_FLOAT_VEC4: *function = 11; *items = 4; *type = 'f'; return 1;
        case GL_FLOAT_MAT2: *function = 12; *items = 4; *type = 'f'; return 1;
        case GL_FLOAT_MAT2x3: *function = 13; *items = 6; *type = 'f'; return 1;
        case GL_FLOAT_MAT2x4: *function = 14; *items = 8; *type = 'f'; return 1;
        case GL_FLOAT_MAT3x2: *function = 15; *items = 6; *type = 'f'; return 1;
        case GL_FLOAT_MAT3: *function = 16; *items = 9; *type = 'f'; return 1;
        case GL_FLOAT_MAT3x4: *function = 17; *items = 12; *type = 'f'; return 1;
        case GL_FLOAT_MAT4x2: *function = 18; *items = 8; *type = 'f'; return 1;
        case GL_FLOAT_MAT4x3: *function = 19; *items = 12; *type = 'f'; return 1;
        case GL_FLOAT_MAT4: *function = 20; *items = 16; *type = 'f'; return 1;
    }
    return 0;
}

static void upload_uniform(UniformBinding * b, void * ptr) {
    switch (b->function) {
        case 0: glUniform1iv(b->location, b->count, ptr); break;
        case 1: glUniform2iv(b->location, b->count, ptr); break;
        case 2: glUniform3iv(b->location, b->count, ptr); break;
        case 3: glUniform4iv(b->location, b->count, ptr); break;
        case 4: glUniform1uiv(b->location, b->count, ptr); break;
        case 5: glUniform2uiv(b->location, b->count, ptr); break;
        case 6: glUniform3uiv(b->location, b->count, ptr); break;
        case 7: glUniform4uiv(b->location, b->count, ptr); break;
        case 8: glUniform1fv(b->location, b->count, ptr); break;
        case 9: glUniform2fv(b->location, b->count, ptr); break;
        case 10: glUniform3fv(b->location, b->count, ptr); break;
        case 11: glUniform4fv(b->location, b->count, ptr); break;
        case 12: glUniformMatrix2fv(b->location, b->count, GL_FALSE, ptr); break;
        case 13: glUniformMatrix2x3fv(b->location, b->count, GL_FALSE, ptr); break;
        case 14: glUniformMatrix2x4fv(b->location, b->count, GL_FALSE, ptr); break;
        case 15: glUniformMatrix3x2fv(b->location, b->count, GL_FALSE, ptr); break;
        case 16: glUniformMatrix3fv(b->location, b->count, GL_FALSE, ptr); break;
        case 17: glUniformMatrix3x4fv(b->location, b->count, GL_FALSE, ptr); break;
        case 18: glUniformMatrix4x2fv(b->location, b->count, GL_FALSE, ptr); break;
        case 19: glUniformMatrix4x3fv(b->location, b->count, GL_FALSE, ptr); break;
        case 20: glUniformMatrix4fv(b->location, b->count, GL_FALSE, ptr); break;
    }
}

static int flatten(PyObject * obj, PyObject * res) {
    if (PyLong_Check(obj) || PyFloat_Check(obj)) {
        return !PyList_Append(res, obj);
    }
    PyObject * seq = PySequence_Fast(obj, "");
    if (!seq) {
        return 0;
    }
    int32_t count = (int32_t)PySequence_Fast_GET_SIZE(seq);
    for (int32_t i = 0; i < count; ++i) {
        if (!flatten(PySequence_Fast_GET_ITEM(seq, i), res)) {
            Py_DECREF(seq);
            return 0;
        }
    }
    Py_DECREF(seq);
    return 1;
}

static int pack_uniform(UniformBinding * b, PyObject * name, PyObject * value, char * out) {
    int32_t size = b->values * 4;

    if (PyObject_CheckBuffer(value)) {
        Py_buffer view = {0};
        if (PyObject_GetBuffer(value, &view, PyBUF_SIMPLE)) {
            return 0;
        }
        if (view.len != size) {
            PyBuffer_Release(&view);
            PyErr_Format(PyExc_ValueError, "uniform \"%U\" needs %d bytes, got %d", name, size, (int32_t)view.len);
            return 0;
        }
        memcpy(out, view.buf, size);
        PyBuffer_Release(&view);
        return 1;
    }

    PyObject * values = PyList_New(0);
    if (PyUnicode_Check(value) || !flatten(value, values)) {
        Py_DECREF(values);
        PyErr_Clear();
        PyErr_Format(PyExc_TypeError, "uniform \"%U\" must be a number or a nested sequence of numbers", name);
        return 0;
    }

    if (PyList_Size(values) != b->values) {
        PyErr_Format(PyExc_ValueError, "uniform \"%U\" needs %d values, got %d", name, b->values, (int32_t)PyList_Size(values));
        Py_DECREF(values);
        return 0;
    }

    char temp[1024] = {0};
    char * dst = size <= 1024 ? temp : (char *)PyMem_Malloc(size);

    for (int32_t i = 0; i < b->values; ++i) {
        PyObject * item = PyList_GetItem(values, i);
        if (b->type == 'f') {
            ((float *)dst)[i] = (float)PyFloat_AsDouble(item);
        } else if (PyFloat_Check(item)) {
            PyErr_Format(PyExc_TypeError, "uniform \"%U\" is an integer uniform, got a float", name);
        } else if (b->type == 'i') {
            ((int32_t *)dst)[i] = (int32_t)PyLong_AsLong(item);
        } else {
            ((uint32_t *)dst)[i] = (uint32_t)PyLong_AsUnsignedLong(item);
        }
        if (PyErr_Occurred()) {
            break;
        }
    }

    Py_DECREF(values);

    if (!PyErr_Occurred()) {
        memcpy(out, dst, size);
    }

    if (dst != temp) {
        PyMem_Free(dst);
    }

    return !PyErr_Occurred();
}

static int set_uniform(Program * self, int32_t index, PyObject * name, PyObject * value) {
    UniformBinding * b = &self->uniform_bindings[index];
    int32_t size = b->values * 4;
    char temp[1024] = {0};
    char * dst = size <= 1024 ? temp : (char *)PyMem_Malloc(size);
    int ok = pack_uniform(b, name, value, dst);
    if (ok) {
        upload_uniform(b, dst);
    }
    if (dst != temp) {
        PyMem_Free(dst);
    }
    return ok;
}

static int parse_uniform_type(PyObject * obj, uint32_t * gltype, int32_t * count, int32_t * array) {
    const char * str = PyUnicode_Check(obj) ? PyUnicode_AsUTF8(obj) : NULL;
    if (!str) {
        return 0;
    }
    char base[16] = {0};
    const char * bracket = strchr(str, '[');
    int32_t length = bracket ? (int32_t)(bracket - str) : (int32_t)strlen(str);
    if (length >= 16) {
        return 0;
    }
    memcpy(base, str, length);
    *count = 1;
    *array = bracket != NULL;
    if (bracket) {
        char * end = NULL;
        long size = strtol(bracket + 1, &end, 10);
        if (end == bracket + 1 || strcmp(end, "]") || size < 1 || size > 4096) {
            return 0;
        }
        *count = (int32_t)size;
    }
    return get_named_value(uniform_types, base, gltype);
}

static int get_binding(PyObject * bindings, const char * name, int32_t limit, int32_t * binding) {
    PyObject * value = bindings ? PyDict_GetItemString(bindings, name) : NULL;
    if (!value || !PyLong_CheckExact(value)) {
        return 0;
    }
    *binding = (int32_t)PyLong_AsLong(value);
    return *binding >= 0 && *binding < limit;
}

static ReadRequest * new_read_request(Context * ctx, uint32_t buffer, int32_t size) {
    ReadRequest * res = PyObject_New(ReadRequest, ctx->module_state->ReadRequest_type);
    res->ctx = (Context *)Py_NewRef(ctx);
    res->data = NULL;
    res->sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    res->buffer = buffer;
    res->size = size;
    glFlush();
    return res;
}

static int direct_read(void) {
#ifdef EXTERN_GL
    return 0;
#else
    return 1;
#endif
}

static ReadRequest * finished_read_request(Context * ctx, PyObject * data) {
    ReadRequest * res = PyObject_New(ReadRequest, ctx->module_state->ReadRequest_type);
    res->ctx = (Context *)Py_NewRef(ctx);
    res->data = data;
    res->sync = NULL;
    res->buffer = 0;
    res->size = (int32_t)PyBytes_Size(data);
    return res;
}

static uint32_t new_staging_buffer(uint32_t target, int32_t size) {
    uint32_t buffer = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, (intptr_t)size, NULL, GL_STREAM_READ);
    return buffer;
}

static ReadRequest * read_buffer(Buffer * self, PyObject * size_arg, int32_t offset) {
    if (offset < 0 || offset > self->size) {
        PyErr_Format(PyExc_ValueError, "invalid offset");
        return NULL;
    }

    if (size_arg != Py_None && !PyLong_CheckExact(size_arg)) {
        PyErr_Format(PyExc_TypeError, "the size must be an int");
        return NULL;
    }

    int32_t size = self->size - offset;
    if (size_arg != Py_None) {
        size = (int32_t)PyLong_AsLong(size_arg);
    }

    if (size < 0 || size + offset > self->size) {
        PyErr_Format(PyExc_ValueError, "invalid size");
        return NULL;
    }

    if (direct_read()) {
        PyObject * data = PyBytes_FromStringAndSize(NULL, size);
        glBindBuffer(GL_COPY_READ_BUFFER, self->buffer);
        glGetBufferSubData(GL_COPY_READ_BUFFER, (intptr_t)offset, (intptr_t)size, PyBytes_AsString(data));
        glBindBuffer(GL_COPY_READ_BUFFER, 0);
        return finished_read_request(self->ctx, data);
    }

    uint32_t staging = new_staging_buffer(GL_COPY_WRITE_BUFFER, size);
    glBindBuffer(GL_COPY_READ_BUFFER, self->buffer);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, (intptr_t)offset, 0, (intptr_t)size);
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    return new_read_request(self->ctx, staging, size);
}

static PyObject * Buffer_meth_write(Buffer * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"data", "offset", NULL};

    PyObject * data = NULL;
    int32_t offset = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|i", keywords, &data, &offset)) {
        return NULL;
    }

    if (offset < 0 || offset > self->size) {
        PyErr_Format(PyExc_ValueError, "invalid offset");
        return NULL;
    }

    PyObject * mem = PyMemoryView_GetContiguous(data, PyBUF_READ, 'C');
    if (!mem) {
        return NULL;
    }

    Py_buffer view = {0};
    if (PyObject_GetBuffer(mem, &view, PyBUF_SIMPLE)) {
        Py_DECREF(mem);
        return NULL;
    }

    int32_t data_size = (int32_t)view.len;

    if (data_size + offset > self->size) {
        PyBuffer_Release(&view);
        Py_DECREF(mem);
        PyErr_Format(PyExc_ValueError, "invalid size");
        return NULL;
    }

    if (data_size) {
        if (self->target == GL_ELEMENT_ARRAY_BUFFER) {
            glBindVertexArray(0);
        }
        glBindBuffer(self->target, self->buffer);
        glBufferSubData(self->target, (intptr_t)offset, (intptr_t)data_size, view.buf);
        glBindBuffer(self->target, 0);
        if (self->target == GL_ELEMENT_ARRAY_BUFFER) {
            restore_vertex_array(self->ctx);
        }
    }

    PyBuffer_Release(&view);
    Py_DECREF(mem);
    Py_RETURN_NONE;
}

static PyObject * Buffer_meth_read(Buffer * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"size", "offset", NULL};

    PyObject * size_arg = Py_None;
    int32_t offset = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|Oi", keywords, &size_arg, &offset)) {
        return NULL;
    }

    return (PyObject *)read_buffer(self, size_arg, offset);
}

static PyObject * Buffer_meth_bind(Buffer * self, PyObject * args, PyObject * kwargs) {
    Py_ssize_t nargs = PyTuple_Size(args);

    if (nargs < 1 || !PyUnicode_Check(PyTuple_GetItem(args, 0))) {
        PyErr_Format(PyExc_TypeError, "bind(layout, *locations, offset=0)");
        return NULL;
    }

    int32_t base_offset = 0;
    if (kwargs) {
        PyObject * offset_arg = PyDict_GetItemString(kwargs, "offset");
        if (PyDict_Size(kwargs) != (offset_arg ? 1 : 0)) {
            PyErr_Format(PyExc_TypeError, "bind() only takes offset as keyword argument");
            return NULL;
        }
        if (offset_arg) {
            base_offset = (int32_t)PyLong_AsLong(offset_arg);
            if (PyErr_Occurred()) {
                return NULL;
            }
        }
    }

    PyObject * nodes = PyUnicode_Split(PyTuple_GetItem(args, 0), NULL, -1);
    int32_t node_count = (int32_t)PyList_Size(nodes);
    int32_t divisor = 0;

    if (node_count && !strcmp(PyUnicode_AsUTF8(PyList_GetItem(nodes, node_count - 1)), "/i")) {
        divisor = 1;
        node_count -= 1;
    }

    int32_t location_count = (int32_t)nargs - 1;
    int32_t stride = 0;
    int32_t attribute_count = 0;

    for (int32_t i = 0; i < node_count; ++i) {
        const char * node = PyUnicode_AsUTF8(PyList_GetItem(nodes, i));
        size_t length = strlen(node);

        if (length > 1 && node[length - 1] == 'x') {
            int32_t padding = atoi(node);
            if (padding <= 0) {
                Py_DECREF(nodes);
                PyErr_Format(PyExc_ValueError, "invalid padding \"%s\"", node);
                return NULL;
            }
            stride += padding;
            continue;
        }

        const ShortVertexFormat * fmt = get_short_vertex_format(node);
        if (!fmt) {
            Py_DECREF(nodes);
            PyErr_Format(PyExc_ValueError, "invalid vertex format \"%s\"", node);
            return NULL;
        }

        stride += get_vertex_format(fmt->format)->size;
        attribute_count += 1;
    }

    if (attribute_count != location_count) {
        Py_DECREF(nodes);
        PyErr_Format(PyExc_ValueError, "the layout has %d attributes but %d locations were given", attribute_count, location_count);
        return NULL;
    }

    PyObject * res = PyList_New(0);
    int32_t offset = base_offset;
    int32_t index = 0;

    for (int32_t i = 0; i < node_count; ++i) {
        const char * node = PyUnicode_AsUTF8(PyList_GetItem(nodes, i));
        size_t length = strlen(node);

        if (length > 1 && node[length - 1] == 'x') {
            offset += atoi(node);
            continue;
        }

        const ShortVertexFormat * fmt = get_short_vertex_format(node);
        PyObject * location = PyTuple_GetItem(args, 1 + index);

        if (!PyLong_CheckExact(location)) {
            Py_DECREF(nodes);
            Py_DECREF(res);
            PyErr_Format(PyExc_TypeError, "the locations must be ints");
            return NULL;
        }

        if (PyLong_AsLong(location) >= 0) {
            PyObject * item = Py_BuildValue("(OOsiii)", self, location, fmt->format, offset, stride, divisor);
            PyList_Append(res, item);
            Py_DECREF(item);
        }

        offset += get_vertex_format(fmt->format)->size;
        index += 1;
    }

    Py_DECREF(nodes);
    PyObject * tuple = PyList_AsTuple(res);
    Py_DECREF(res);
    return tuple;
}

static PyObject * Buffer_meth_view(Buffer * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"offset", "size", NULL};

    int32_t offset = 0;
    PyObject * size_arg = Py_None;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|iO", keywords, &offset, &size_arg)) {
        return NULL;
    }

    if (size_arg != Py_None && !PyLong_CheckExact(size_arg)) {
        PyErr_Format(PyExc_TypeError, "the size must be an int");
        return NULL;
    }

    int32_t size = size_arg != Py_None ? (int32_t)PyLong_AsLong(size_arg) : self->size - offset;

    if (offset < 0 || size <= 0 || offset + size > self->size) {
        PyErr_Format(PyExc_ValueError, "invalid size or offset");
        return NULL;
    }

    return Py_BuildValue("(Oii)", self, offset, size);
}

static PyObject * Buffer_meth_use(Buffer * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"binding", "offset", "size", NULL};

    int32_t binding = 0;
    int32_t offset = 0;
    PyObject * size_arg = Py_None;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "i|iO", keywords, &binding, &offset, &size_arg)) {
        return NULL;
    }

    if (self->index) {
        PyErr_Format(PyExc_TypeError, "index buffers can only be used as index buffers");
        return NULL;
    }

    if (binding < 0 || binding >= self->ctx->max_uniform_buffers) {
        PyErr_Format(PyExc_ValueError, "invalid binding %d", binding);
        return NULL;
    }

    if (size_arg != Py_None && !PyLong_CheckExact(size_arg)) {
        PyErr_Format(PyExc_TypeError, "the size must be an int");
        return NULL;
    }

    int32_t size = self->size - offset;
    if (size_arg != Py_None) {
        size = (int32_t)PyLong_AsLong(size_arg);
    }

    if (offset < 0 || size <= 0 || offset + size > self->size) {
        PyErr_Format(PyExc_ValueError, "invalid size or offset");
        return NULL;
    }

    if (offset % self->ctx->uniform_buffer_offset_alignment) {
        PyErr_Format(PyExc_ValueError, "the offset must be a multiple of %d", self->ctx->uniform_buffer_offset_alignment);
        return NULL;
    }

    glBindBufferRange(GL_UNIFORM_BUFFER, binding, self->buffer, (intptr_t)offset, (intptr_t)size);
    Py_RETURN_NONE;
}

static ReadRequest * read_texture(
    Texture * self, PyObject * size_arg, PyObject * offset_arg, int32_t layer, int32_t level
) {
    if (self->multisampled) {
        PyErr_Format(PyExc_TypeError, "cannot read multisampled textures");
        return NULL;
    }

    if (!self->fmt->color) {
        PyErr_Format(PyExc_TypeError, "cannot read depth or stencil textures");
        return NULL;
    }

    if (layer < 0 || layer >= self->layer_count || level < 0 || level >= self->level_count) {
        PyErr_Format(PyExc_ValueError, "invalid layer or level");
        return NULL;
    }

    Region region = {0};
    if (!parse_region(self, level, size_arg, offset_arg, &region)) {
        return NULL;
    }

    int32_t size = region_bytes(self, &region);

    int temporary = self->array || self->cubemap || level;
    uint32_t framebuffer = temporary ? face_framebuffer(self, layer, level) : texture_framebuffer(self);
    if (!framebuffer) {
        return NULL;
    }

    int direct = direct_read();
    PyObject * data = direct ? PyBytes_FromStringAndSize(NULL, size) : NULL;
    if (direct) {
        memset(PyBytes_AsString(data), 0, (size_t)size);
    }
    uint32_t staging = direct ? 0 : new_staging_buffer(GL_PIXEL_PACK_BUFFER, size);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glReadPixels(
        region.x, region.y, region.width, region.height, self->fmt->format,
        self->fmt->type, direct ? PyBytes_AsString(data) : NULL
    );
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    if (temporary) {
        glDeleteFramebuffers(1, &framebuffer);
    }

    if (direct) {
        return finished_read_request(self->ctx, data);
    }

    return new_read_request(self->ctx, staging, size);
}

static PyObject * Texture_meth_write(Texture * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"data", "size", "offset", "layer", "level", NULL};

    PyObject * data = NULL;
    PyObject * size_arg = Py_None;
    PyObject * offset_arg = Py_None;
    PyObject * layer_arg = Py_None;
    int32_t level = 0;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "O|OOOi", keywords,
        &data, &size_arg, &offset_arg, &layer_arg, &level
    );

    if (!args_ok) {
        return NULL;
    }

    if (layer_arg != Py_None && !PyLong_CheckExact(layer_arg)) {
        PyErr_Format(PyExc_TypeError, "the layer must be an int or None");
        return NULL;
    }

    int32_t layer = 0;
    if (layer_arg != Py_None) {
        layer = (int32_t)PyLong_AsLong(layer_arg);
    }

    if (layer < 0 || layer >= self->layer_count) {
        PyErr_Format(PyExc_ValueError, "invalid layer");
        return NULL;
    }

    if (level < 0 || level >= self->level_count) {
        PyErr_Format(PyExc_ValueError, "invalid level");
        return NULL;
    }

    Region region = {0};
    if (!parse_region(self, level, size_arg, offset_arg, &region)) {
        return NULL;
    }

    int32_t x = region.x;
    int32_t y = region.y;
    int32_t width = region.width;
    int32_t height = region.height;

    if (!self->cubemap && !self->array && layer_arg != Py_None) {
        PyErr_Format(PyExc_TypeError, "the texture is not layered");
        return NULL;
    }

    if (!self->fmt->color) {
        PyErr_Format(PyExc_TypeError, "cannot write to depth or stencil textures");
        return NULL;
    }

    if (self->multisampled) {
        PyErr_Format(PyExc_TypeError, "cannot write to multisampled textures");
        return NULL;
    }

    int32_t padded_row = (width * self->fmt->pixel_size + 3) & ~3;
    int32_t stride = padded_row * height;
    int32_t expected_size = stride;

    if (layer_arg == Py_None) {
        expected_size *= self->layer_count;
    }

    PyObject * mem = PyMemoryView_GetContiguous(data, PyBUF_READ, 'C');
    if (!mem) {
        return NULL;
    }

    Py_buffer view = {0};
    if (PyObject_GetBuffer(mem, &view, PyBUF_SIMPLE)) {
        Py_DECREF(mem);
        return NULL;
    }

    int32_t data_size = (int32_t)view.len;

    if (data_size != expected_size) {
        PyBuffer_Release(&view);
        Py_DECREF(mem);
        PyErr_Format(PyExc_ValueError, "invalid data size, expected %d, got %d", expected_size, data_size);
        return NULL;
    }

    const ImageFormat * fmt = self->fmt;
    char * ptr = (char *)view.buf;

    glActiveTexture(self->ctx->default_texture_unit);
    glBindTexture(self->target, self->texture);

    if (self->cubemap) {
        if (layer_arg != Py_None) {
            uint32_t face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer;
            glTexSubImage2D(face, level, x, y, width, height, fmt->format, fmt->type, ptr);
        } else {
            for (int32_t i = 0; i < 6; ++i) {
                uint32_t face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
                glTexSubImage2D(face, level, x, y, width, height, fmt->format, fmt->type, ptr + stride * i);
            }
        }
    } else if (self->array) {
        if (layer_arg != Py_None) {
            glTexSubImage3D(self->target, level, x, y, layer, width, height, 1, fmt->format, fmt->type, ptr);
        } else {
            glTexSubImage3D(self->target, level, x, y, 0, width, height, self->array, fmt->format, fmt->type, ptr);
        }
    } else {
        glTexSubImage2D(self->target, level, x, y, width, height, fmt->format, fmt->type, ptr);
    }

    PyBuffer_Release(&view);
    Py_DECREF(mem);
    Py_RETURN_NONE;
}

static PyObject * Texture_meth_read(Texture * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"size", "offset", "layer", "level", NULL};

    PyObject * size_arg = Py_None;
    PyObject * offset_arg = Py_None;
    int32_t layer = 0;
    int32_t level = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOii", keywords, &size_arg, &offset_arg, &layer, &level)) {
        return NULL;
    }

    return (PyObject *)read_texture(self, size_arg, offset_arg, layer, level);
}

static PyObject * storage_image_view(Texture * texture, int32_t layer, int32_t level) {
    if (layer < 0) {
        return Py_BuildValue("(OOi)", texture, Py_None, level);
    }
    return Py_BuildValue("(Oii)", texture, layer, level);
}

static int parse_storage_image(Context * ctx, PyObject * obj, StorageImageSlot * res) {
    PyObject * texture = obj;
    PyObject * layer = Py_None;
    res->level = 0;
    if (PyTuple_Check(obj) && !PyArg_ParseTuple(obj, "OOi", &texture, &layer, &res->level)) {
        PyErr_Clear();
        texture = NULL;
    }
    int is_texture = texture && Py_TYPE(texture) == ctx->module_state->Texture_type;
    if (!is_texture || (layer != Py_None && !PyLong_CheckExact(layer))) {
        PyErr_Format(PyExc_TypeError, "expected a Texture or a texture.storage_image(...)");
        return 0;
    }
    res->texture = (Texture *)texture;
    res->layer = layer != Py_None ? (int32_t)PyLong_AsLong(layer) : -1;
    if (res->texture->multisampled || !res->texture->fmt->color || res->texture->fmt->format == GL_BGRA) {
        PyErr_Format(PyExc_ValueError, "storage images cannot be multisampled, depth or bgra8unorm");
        return 0;
    }
    if (res->layer >= res->texture->layer_count || res->level < 0 || res->level >= res->texture->level_count) {
        PyErr_Format(PyExc_ValueError, "invalid layer or level");
        return 0;
    }
    return 1;
}

static PyObject * Texture_meth_storage_image(Texture * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"layer", "level", "face", NULL};

    PyObject * layer_arg = Py_None;
    int32_t level = 0;
    PyObject * face_arg = Py_None;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|$OiO", keywords, &layer_arg, &level, &face_arg)) {
        return NULL;
    }

    if (self->cubemap && layer_arg != Py_None) {
        PyErr_Format(PyExc_TypeError, "cubemaps take a face, not a layer");
        return NULL;
    }

    if (!self->cubemap && face_arg != Py_None) {
        PyErr_Format(PyExc_TypeError, "only cubemaps take a face");
        return NULL;
    }

    if (!self->array && layer_arg != Py_None) {
        PyErr_Format(PyExc_TypeError, "only array textures take a layer");
        return NULL;
    }

    PyObject * index_arg = self->cubemap ? face_arg : layer_arg;

    if (index_arg != Py_None && !PyLong_CheckExact(index_arg)) {
        PyErr_Format(PyExc_TypeError, "the layer and face must be ints");
        return NULL;
    }

    int32_t layer = index_arg != Py_None ? (int32_t)PyLong_AsLong(index_arg) : -1;

    if (layer >= self->layer_count || (index_arg != Py_None && layer < 0)) {
        PyErr_Format(PyExc_ValueError, self->cubemap ? "invalid face" : "invalid layer");
        return NULL;
    }

    if (level < 0 || level >= self->level_count) {
        PyErr_Format(PyExc_ValueError, "invalid level");
        return NULL;
    }

    return storage_image_view(self, layer, level);
}

static PyObject * Texture_meth_attachment(Texture * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"layer", "level", "face", NULL};

    PyObject * layer_arg = Py_None;
    int32_t level = 0;
    PyObject * face_arg = Py_None;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|$OiO", keywords, &layer_arg, &level, &face_arg)) {
        return NULL;
    }

    if (self->cubemap && layer_arg != Py_None) {
        PyErr_Format(PyExc_TypeError, "cubemaps take a face, not a layer");
        return NULL;
    }

    if (!self->cubemap && face_arg != Py_None) {
        PyErr_Format(PyExc_TypeError, "only cubemaps take a face");
        return NULL;
    }

    if (!self->array && layer_arg != Py_None) {
        PyErr_Format(PyExc_TypeError, "only array textures take a layer");
        return NULL;
    }

    PyObject * index_arg = self->cubemap ? face_arg : layer_arg;

    if (index_arg != Py_None && !PyLong_CheckExact(index_arg)) {
        PyErr_Format(PyExc_TypeError, "the layer and face must be ints");
        return NULL;
    }

    int32_t layer = index_arg != Py_None ? (int32_t)PyLong_AsLong(index_arg) : 0;

    if (layer < 0 || layer >= self->layer_count) {
        PyErr_Format(PyExc_ValueError, self->cubemap ? "invalid face" : "invalid layer");
        return NULL;
    }

    if (level < 0 || level >= self->level_count) {
        PyErr_Format(PyExc_ValueError, "invalid level");
        return NULL;
    }

    return Py_BuildValue("(Oii)", self, layer, level);
}

static PyObject * Texture_meth_mipmaps(Texture * self, PyObject * args) {
    if (self->multisampled) {
        PyErr_Format(PyExc_TypeError, "cannot build mipmaps for multisampled textures");
        return NULL;
    }

    glActiveTexture(self->ctx->default_texture_unit);
    glBindTexture(self->target, self->texture);
    glGenerateMipmap(self->target);
    Py_RETURN_NONE;
}

static PyObject * Sampler_meth_use(Sampler * self, PyObject * arg) {
    int32_t unit = (int32_t)PyLong_AsLong(arg);
    if (PyErr_Occurred()) {
        return NULL;
    }

    if (unit < 0 || unit >= self->ctx->max_samplers) {
        PyErr_Format(PyExc_ValueError, "invalid unit %d", unit);
        return NULL;
    }

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(self->texture->target, self->texture->texture);
    glBindSampler(unit, self->sampler);
    Py_RETURN_NONE;
}

static PyObject * Resources_meth_use(Resources * self, PyObject * args) {
    for (int32_t i = 0; i < self->sampler_count; ++i) {
        SamplerSlot * slot = &self->samplers[i];
        glActiveTexture(GL_TEXTURE0 + slot->unit);
        glBindTexture(slot->sampler->texture->target, slot->sampler->texture->texture);
        glBindSampler(slot->unit, slot->sampler->sampler);
    }
    for (int32_t i = 0; i < self->uniform_buffer_count; ++i) {
        BufferRange * range = &self->uniform_buffers[i];
        glBindBufferRange(
            GL_UNIFORM_BUFFER, range->binding, range->buffer->buffer, (intptr_t)range->offset, (intptr_t)range->size
        );
    }
    for (int32_t i = 0; i < self->storage_buffer_count; ++i) {
        BufferRange * range = &self->storage_buffers[i];
        glBindBufferRange(
            GL_SHADER_STORAGE_BUFFER, range->binding, range->buffer->buffer,
            (intptr_t)range->offset, (intptr_t)range->size
        );
    }
    for (int32_t i = 0; i < self->storage_image_count; ++i) {
        StorageImageSlot * slot = &self->storage_images[i];
        Texture * texture = slot->texture;
        uint8_t layered = slot->layer < 0 && (texture->array || texture->cubemap);
        int32_t layer = slot->layer < 0 ? 0 : slot->layer;
        glBindImageTexture(
            slot->unit, texture->texture, slot->level, layered, layer, GL_READ_WRITE, texture->fmt->internal_format
        );
    }
    Py_RETURN_NONE;
}

static PyObject * Resources_get_samplers(Resources * self, void * closure) {
    PyObject * res = PyDict_New();
    for (int32_t i = 0; i < self->sampler_count; ++i) {
        PyObject * unit = PyLong_FromLong(self->samplers[i].unit);
        PyDict_SetItem(res, unit, (PyObject *)self->samplers[i].sampler);
        Py_DECREF(unit);
    }
    return res;
}

static PyObject * get_buffer_ranges(BufferRange * ranges, int32_t count) {
    PyObject * res = PyDict_New();
    for (int32_t i = 0; i < count; ++i) {
        BufferRange * range = &ranges[i];
        PyObject * binding = PyLong_FromLong(range->binding);
        PyObject * view = Py_BuildValue("(Oii)", range->buffer, range->offset, range->size);
        PyDict_SetItem(res, binding, view);
        Py_DECREF(binding);
        Py_DECREF(view);
    }
    return res;
}

static PyObject * Resources_get_uniform_buffers(Resources * self, void * closure) {
    return get_buffer_ranges(self->uniform_buffers, self->uniform_buffer_count);
}

static PyObject * Resources_get_storage_buffers(Resources * self, void * closure) {
    return get_buffer_ranges(self->storage_buffers, self->storage_buffer_count);
}

static PyObject * Resources_get_storage_images(Resources * self, void * closure) {
    PyObject * res = PyDict_New();
    for (int32_t i = 0; i < self->storage_image_count; ++i) {
        StorageImageSlot * slot = &self->storage_images[i];
        PyObject * unit = PyLong_FromLong(slot->unit);
        PyObject * view = storage_image_view(slot->texture, slot->layer, slot->level);
        PyDict_SetItem(res, unit, view);
        Py_DECREF(unit);
        Py_DECREF(view);
    }
    return res;
}

static PyObject * draw_vertex_array(
    VertexArray * self, uint32_t mode, int32_t first, int32_t vertices, int32_t instances, int32_t first_instance
) {
    if (!self->ctx->bound_program) {
        PyErr_Format(PyExc_RuntimeError, "no program is bound, call program.use() first");
        return NULL;
    }

    if (vertices < 0 || instances < 0 || first < 0 || first_instance < 0) {
        PyErr_Format(PyExc_ValueError, "vertices, instances, first and first_instance must not be negative");
        return NULL;
    }

    if (first_instance && !glDrawArraysInstancedBaseInstance) {
        PyErr_Format(PyExc_RuntimeError, "first_instance is not supported, it needs OpenGL 4.2");
        return NULL;
    }

    if (!self->index_type) {
        if (first_instance) {
            glDrawArraysInstancedBaseInstance(mode, first, vertices, instances, first_instance);
        } else {
            glDrawArraysInstanced(mode, first, vertices, instances);
        }
        Py_RETURN_NONE;
    }

    void * offset = (void *)(intptr_t)(first * self->index_size);
    if (first_instance) {
        glDrawElementsInstancedBaseVertexBaseInstance(
            mode, vertices, self->index_type, offset, instances, 0, first_instance
        );
    } else {
        glDrawElementsInstanced(mode, vertices, self->index_type, offset, instances);
    }
    Py_RETURN_NONE;
}

static PyObject * VertexArray_meth_use(VertexArray * self, PyObject * args) {
    Context * ctx = self->ctx;
    if (ctx->bound_vertex_array != self) {
        glBindVertexArray(self->vertex_array);
        Py_XSETREF(ctx->bound_vertex_array, (VertexArray *)Py_NewRef(self));
    }
    Py_RETURN_NONE;
}

static PyObject * VertexArray_meth_render(VertexArray * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"mode", "vertices", "instances", "first", "first_instance", NULL};

    const char * mode_arg = NULL;
    PyObject * vertices_arg = Py_None;
    int32_t instances = 1;
    int32_t first = 0;
    int32_t first_instance = 0;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "|$zOiii", keywords,
        &mode_arg, &vertices_arg, &instances, &first, &first_instance
    );

    if (!args_ok) {
        return NULL;
    }

    if (vertices_arg != Py_None && !PyLong_CheckExact(vertices_arg)) {
        PyErr_Format(PyExc_TypeError, "vertices must be an int or None");
        return NULL;
    }

    int32_t vertices = vertices_arg != Py_None ? (int32_t)PyLong_AsLong(vertices_arg) : self->vertices;

    Context * ctx = self->ctx;
    if (ctx->bound_vertex_array != self) {
        glBindVertexArray(self->vertex_array);
        Py_XSETREF(ctx->bound_vertex_array, (VertexArray *)Py_NewRef(self));
    }

    uint32_t mode = 0;
    if (!parse_mode(self, mode_arg, &mode)) {
        return NULL;
    }

    return draw_vertex_array(self, mode, first, vertices, instances, first_instance);
}

static PyObject * VertexArray_meth_draw(VertexArray * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"vertices", "first", "instances", "first_instance", "mode", NULL};

    int32_t vertices = 0;
    int32_t first = 0;
    int32_t instances = 1;
    int32_t first_instance = 0;
    const char * mode_arg = NULL;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "i|i$iiz", keywords,
        &vertices, &first, &instances, &first_instance, &mode_arg
    );

    if (!args_ok) {
        return NULL;
    }

    if (self->ctx->bound_vertex_array != self) {
        PyErr_Format(PyExc_RuntimeError, "the vertex array is not bound, call vertex_array.use() first");
        return NULL;
    }

    uint32_t mode = 0;
    if (!parse_mode(self, mode_arg, &mode)) {
        return NULL;
    }

    return draw_vertex_array(self, mode, first, vertices, instances, first_instance);
}

static PyObject * VertexArray_meth_draw_indirect(VertexArray * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"buffer", "count", "offset", "mode", NULL};

    Buffer * buffer = NULL;
    int32_t count = 1;
    int32_t offset = 0;
    const char * mode_arg = NULL;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "O!|ii$z", keywords,
        self->ctx->module_state->Buffer_type, &buffer, &count, &offset, &mode_arg
    );

    if (!args_ok) {
        return NULL;
    }

    if (!glMultiDrawArraysIndirect) {
        PyErr_Format(PyExc_RuntimeError, "draw_indirect is not supported, it needs OpenGL 4.3");
        return NULL;
    }

    if (!self->ctx->bound_program) {
        PyErr_Format(PyExc_RuntimeError, "no program is bound, call program.use() first");
        return NULL;
    }

    if (buffer->index) {
        PyErr_Format(PyExc_TypeError, "index buffers can only be used as index buffers");
        return NULL;
    }

    uint32_t mode = 0;
    if (!parse_mode(self, mode_arg, &mode)) {
        return NULL;
    }

    int32_t stride = self->index_type ? 20 : 16;
    if (count < 0 || offset < 0 || offset % 4 || offset + count * stride > buffer->size) {
        PyErr_Format(PyExc_ValueError, "invalid count or offset, %d commands of %d bytes from offset %d do not fit in %d bytes", count, stride, offset, buffer->size);
        return NULL;
    }

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer->buffer);
    if (self->index_type) {
        glMultiDrawElementsIndirect(mode, self->index_type, (void *)(intptr_t)offset, count, 0);
    } else {
        glMultiDrawArraysIndirect(mode, (void *)(intptr_t)offset, count, 0);
    }
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    Py_RETURN_NONE;
}

static PyObject * VertexArray_get_mode(VertexArray * self, void * closure) {
    return PyUnicode_FromString(get_value_name(modes, self->mode));
}

static int VertexArray_set_mode(VertexArray * self, PyObject * value, void * closure) {
    uint32_t mode = 0;
    if (!parse_named_value(modes, value, &mode, "mode")) {
        return -1;
    }
    self->mode = mode;
    return 0;
}

static PyObject * Program_meth_use(Program * self, PyObject * args) {
    Context * ctx = self->ctx;
    if (ctx->bound_program != self) {
        glUseProgram(self->program);
        Py_XSETREF(ctx->bound_program, (Program *)Py_NewRef(self));
    }
    Py_RETURN_NONE;
}

static int Program_ass_subscript(Program * self, PyObject * key, PyObject * value) {
    if (!value) {
        PyErr_Format(PyExc_TypeError, "uniforms cannot be deleted");
        return -1;
    }
    if (self->ctx->bound_program != self) {
        PyErr_Format(PyExc_RuntimeError, "the program is not bound, call program.use() first");
        return -1;
    }
    PyObject * index = PyDict_GetItemWithError(self->uniform_index, key);
    if (!index) {
        if (!PyErr_Occurred()) {
            PyErr_SetObject(PyExc_KeyError, key);
        }
        return -1;
    }
    return set_uniform(self, (int32_t)PyLong_AsLong(index), key, value) ? 0 : -1;
}

static PyObject * Framebuffer_meth_use(Framebuffer * self, PyObject * args) {
    bind_framebuffer(self->ctx, self);
    apply_viewport(self->ctx, &self->viewport);
    apply_scissor(self->ctx, self->has_scissor, &self->scissor);
    Py_RETURN_NONE;
}

static PyObject * Framebuffer_meth_clear(Framebuffer * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"attachments", NULL};

    PyObject * attachments = Py_None;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", keywords, &attachments)) {
        return NULL;
    }

    int32_t slot_count = self->color_count + (self->depth_format ? 1 : 0);
    int8_t depth_write = self->ctx->depth_write;
    int8_t stencil_masked = self->ctx->stencil_write_mask != 0xFF;

    int32_t count = slot_count;
    int32_t indices[MAX_ATTACHMENTS + 1] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    if (attachments != Py_None && !parse_clear_attachments(attachments, slot_count, indices, &count)) {
        return NULL;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, self->framebuffer);
    suspend_scissor(self->ctx);
    if (!depth_write) {
        glDepthMask(GL_TRUE);
    }
    if (stencil_masked) {
        glStencilMask(0xFF);
    }
    for (int32_t i = 0; i < count; ++i) {
        clear_slot(self, indices[i]);
    }
    if (!depth_write) {
        glDepthMask(GL_FALSE);
    }
    if (stencil_masked) {
        glStencilMask(self->ctx->stencil_write_mask);
    }
    resume_scissor(self->ctx);
    restore_framebuffer(self->ctx);
    Py_RETURN_NONE;
}

static PyObject * Framebuffer_get_clear_color(Framebuffer * self, void * closure) {
    if (self->color_count == 0) {
        Py_RETURN_NONE;
    }

    if (self->color_count == 1) {
        return clear_color_object(self->color_formats[0], &self->clear_colors[0]);
    }

    PyObject * res = PyTuple_New(self->color_count);
    for (int32_t i = 0; i < self->color_count; ++i) {
        PyTuple_SetItem(res, i, clear_color_object(self->color_formats[i], &self->clear_colors[i]));
    }
    return res;
}

static int Framebuffer_set_clear_color(Framebuffer * self, PyObject * value, void * closure) {
    if (self->color_count == 0) {
        return 0;
    }

    ClearValue clear_colors[MAX_ATTACHMENTS] = {0};

    if (self->color_count == 1) {
        if (!parse_clear_color(self->color_formats[0], value, &clear_colors[0])) {
            return -1;
        }
        self->clear_colors[0] = clear_colors[0];
        return 0;
    }

    PyObject * values = PySequence_Tuple(value);
    if (!values) {
        PyErr_Clear();
        PyErr_Format(PyExc_TypeError, "the clear color must be a tuple of %d items", self->color_count);
        return -1;
    }

    if (PyTuple_Size(values) != self->color_count) {
        Py_DECREF(values);
        PyErr_Format(PyExc_ValueError, "the clear color must have %d items", self->color_count);
        return -1;
    }

    for (int32_t i = 0; i < self->color_count; ++i) {
        if (!parse_clear_color(self->color_formats[i], PyTuple_GetItem(values, i), &clear_colors[i])) {
            Py_DECREF(values);
            return -1;
        }
    }

    Py_DECREF(values);
    memcpy(self->clear_colors, clear_colors, sizeof(clear_colors));
    return 0;
}

static PyObject * Framebuffer_get_clear_depth(Framebuffer * self, void * closure) {
    return PyFloat_FromDouble(self->clear_depth);
}

static int Framebuffer_set_clear_depth(Framebuffer * self, PyObject * value, void * closure) {
    float clear_depth = (float)PyFloat_AsDouble(value);
    if (PyErr_Occurred()) {
        PyErr_Clear();
        PyErr_Format(PyExc_TypeError, "the clear depth must be a float");
        return -1;
    }
    self->clear_depth = clear_depth;
    return 0;
}

static PyObject * Framebuffer_get_clear_stencil(Framebuffer * self, void * closure) {
    return PyLong_FromLong(self->clear_stencil);
}

static int Framebuffer_set_clear_stencil(Framebuffer * self, PyObject * value, void * closure) {
    int32_t clear_stencil = (int32_t)PyLong_AsLong(value);
    if (PyErr_Occurred()) {
        PyErr_Clear();
        PyErr_Format(PyExc_TypeError, "the clear stencil must be an int");
        return -1;
    }
    self->clear_stencil = clear_stencil;
    return 0;
}

static PyObject * Framebuffer_get_viewport(Framebuffer * self, void * closure) {
    return Py_BuildValue("(iiii)", self->viewport.x, self->viewport.y, self->viewport.width, self->viewport.height);
}

static int Framebuffer_set_viewport(Framebuffer * self, PyObject * value, void * closure) {
    int32_t x = 0;
    int32_t y = 0;
    int32_t width = 0;
    int32_t height = 0;

    if (!PyArg_ParseTuple(value, "iiii", &x, &y, &width, &height)) {
        PyErr_Clear();
        PyErr_Format(PyExc_TypeError, "the viewport must be a tuple of 4 ints");
        return -1;
    }

    self->viewport.x = x;
    self->viewport.y = y;
    self->viewport.width = width;
    self->viewport.height = height;
    if (self->ctx->bound_framebuffer == self) {
        apply_viewport(self->ctx, &self->viewport);
    }
    return 0;
}

static PyObject * Framebuffer_get_scissor(Framebuffer * self, void * closure) {
    if (!self->has_scissor) {
        Py_RETURN_NONE;
    }
    return Py_BuildValue("(iiii)", self->scissor.x, self->scissor.y, self->scissor.width, self->scissor.height);
}

static int Framebuffer_set_scissor(Framebuffer * self, PyObject * value, void * closure) {
    Region scissor = {0};
    if (value != Py_None && !parse_rect(value, &scissor, "scissor")) {
        return -1;
    }
    self->has_scissor = value != Py_None;
    self->scissor = scissor;
    if (self->ctx->bound_framebuffer == self) {
        apply_scissor(self->ctx, self->has_scissor, &self->scissor);
    }
    return 0;
}

static PyObject * Framebuffer_get_size(Framebuffer * self, void * closure) {
    return Py_NewRef(self->size);
}

static int Framebuffer_set_size(Framebuffer * self, PyObject * value, void * closure) {
    if (!self->external) {
        PyErr_Format(PyExc_TypeError, "only external framebuffers can be resized");
        return -1;
    }

    int32_t width = 0;
    int32_t height = 0;

    if (!PyArg_ParseTuple(value, "ii", &width, &height) || width <= 0 || height <= 0) {
        PyErr_Clear();
        PyErr_Format(PyExc_TypeError, "the size must be a tuple of 2 positive ints");
        return -1;
    }

    self->width = width;
    self->height = height;
    Py_XSETREF(self->size, Py_BuildValue("(ii)", width, height));
    self->viewport.x = 0;
    self->viewport.y = 0;
    self->viewport.width = width;
    self->viewport.height = height;
    if (self->ctx->bound_framebuffer == self) {
        apply_viewport(self->ctx, &self->viewport);
    }
    return 0;
}

static void bind_pipeline(Pipeline * self) {
    Context * ctx = self->ctx;

    if (self->compute) {
        Py_DECREF(Program_meth_use(self->program, NULL));
        Py_DECREF(Resources_meth_use((Resources *)self->resources, NULL));
        for (int32_t i = 0; i < self->uniform_count; ++i) {
            upload_uniform(&self->program->uniform_bindings[i], self->uniform_data + self->uniform_offsets[i]);
        }
        return;
    }

    bind_framebuffer(ctx, self->framebuffer);
    Region full = {0, 0, self->framebuffer->width, self->framebuffer->height};
    apply_viewport(ctx, self->has_viewport ? &self->viewport : &full);
    apply_scissor(ctx, self->has_scissor, &self->scissor);
    apply_polygon_offset(ctx, &self->polygon_offset);
    Py_DECREF(Program_meth_use(self->program, NULL));
    Py_DECREF(VertexArray_meth_use(self->vertex_array, NULL));

    Py_DECREF(Resources_meth_use((Resources *)self->resources, NULL));

    ctx->depth_test = self->depth_test;
    ctx->depth_write = self->depth_write;
    ctx->depth_func = self->depth_func;
    ctx->cull_face = self->cull_face;

    if (self->depth_test) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    glDepthMask(self->depth_write ? GL_TRUE : GL_FALSE);
    glDepthFunc(self->depth_func);

    if (self->cull_face) {
        glEnable(GL_CULL_FACE);
        glCullFace(self->cull_face);
    } else {
        glDisable(GL_CULL_FACE);
    }

    apply_blend(&self->blend);
    apply_stencil(ctx, &self->stencil);

    for (int32_t i = 0; i < self->uniform_count; ++i) {
        upload_uniform(&self->program->uniform_bindings[i], self->uniform_data + self->uniform_offsets[i]);
    }
}

static PyObject * Pipeline_meth_use(Pipeline * self, PyObject * args) {
    bind_pipeline(self);
    Py_RETURN_NONE;
}

static PyObject * Pipeline_meth_run(Pipeline * self, PyObject * args) {
    bind_pipeline(self);

    if (self->compute) {
        glDispatchCompute(self->group_count[0], self->group_count[1], self->group_count[2]);
        Py_RETURN_NONE;
    }

    PyObject * res = draw_vertex_array(
        self->vertex_array, self->mode, self->first, self->vertices, self->instances, self->first_instance
    );
    apply_viewport(self->ctx, &self->framebuffer->viewport);
    apply_scissor(self->ctx, self->framebuffer->has_scissor, &self->framebuffer->scissor);
    return res;
}

static PyObject * Pipeline_get_group_count(Pipeline * self, void * closure) {
    return Py_BuildValue("(iii)", self->group_count[0], self->group_count[1], self->group_count[2]);
}

static int Pipeline_set_group_count(Pipeline * self, PyObject * value, void * closure) {
    int32_t x = 0;
    int32_t y = 0;
    int32_t z = 0;
    if (!value || !PyArg_ParseTuple(value, "iii", &x, &y, &z) || x < 0 || y < 0 || z < 0) {
        PyErr_Clear();
        PyErr_Format(PyExc_TypeError, "the group_count must be a tuple of 3 non-negative ints");
        return -1;
    }
    self->group_count[0] = x;
    self->group_count[1] = y;
    self->group_count[2] = z;
    return 0;
}

static int Pipeline_ass_subscript(Pipeline * self, PyObject * key, PyObject * value) {
    if (!value) {
        PyErr_Format(PyExc_TypeError, "uniforms cannot be deleted");
        return -1;
    }
    PyObject * index = PyDict_GetItemWithError(self->program->uniform_index, key);
    if (!index) {
        if (!PyErr_Occurred()) {
            PyErr_SetObject(PyExc_KeyError, key);
        }
        return -1;
    }
    int32_t i = (int32_t)PyLong_AsLong(index);
    UniformBinding * b = &self->program->uniform_bindings[i];
    return pack_uniform(b, key, value, self->uniform_data + self->uniform_offsets[i]) ? 0 : -1;
}

static PyObject * Pipeline_get_viewport(Pipeline * self, void * closure) {
    if (!self->has_viewport) {
        Py_RETURN_NONE;
    }
    return Py_BuildValue("(iiii)", self->viewport.x, self->viewport.y, self->viewport.width, self->viewport.height);
}

static int Pipeline_set_viewport(Pipeline * self, PyObject * value, void * closure) {
    if (value == Py_None) {
        self->has_viewport = 0;
        return 0;
    }
    Region viewport = {0};
    if (!value || !PyArg_ParseTuple(value, "iiii", &viewport.x, &viewport.y, &viewport.width, &viewport.height)) {
        PyErr_Clear();
        PyErr_Format(PyExc_TypeError, "the viewport must be a tuple of 4 ints or None");
        return -1;
    }
    self->has_viewport = 1;
    self->viewport = viewport;
    return 0;
}

static PyObject * Pipeline_get_scissor(Pipeline * self, void * closure) {
    if (!self->has_scissor) {
        Py_RETURN_NONE;
    }
    return Py_BuildValue("(iiii)", self->scissor.x, self->scissor.y, self->scissor.width, self->scissor.height);
}

static int Pipeline_set_scissor(Pipeline * self, PyObject * value, void * closure) {
    Region scissor = {0};
    if (value != Py_None && !parse_rect(value, &scissor, "scissor")) {
        return -1;
    }
    self->has_scissor = value != Py_None;
    self->scissor = scissor;
    return 0;
}

static int read_request_ready(ReadRequest * self, uint64_t timeout) {
    uint32_t status = glClientWaitSync(self->sync, GL_SYNC_FLUSH_COMMANDS_BIT, timeout);
    if (status == GL_WAIT_FAILED) {
        PyErr_Format(PyExc_RuntimeError, "the read failed");
        return -1;
    }
    return status == GL_ALREADY_SIGNALED || status == GL_CONDITION_SATISFIED;
}

static PyObject * ReadRequest_meth_is_ready(ReadRequest * self, PyObject * args) {
    if (self->data) {
        Py_RETURN_TRUE;
    }
    int ready = read_request_ready(self, 0);
    if (ready < 0) {
        return NULL;
    }
    return PyBool_FromLong(ready);
}

static PyObject * ReadRequest_meth_wait(ReadRequest * self, PyObject * args) {
    if (self->data) {
        return Py_NewRef(self->data);
    }

    int ready = read_request_ready(self, 0);

#ifdef EXTERN_GL
    if (!ready) {
        PyErr_Format(PyExc_RuntimeError, "the read is not ready yet, wait() cannot block in the browser, poll is_ready() once per frame");
        return NULL;
    }
#else
    while (!ready) {
        ready = read_request_ready(self, 1000000000);
    }
#endif

    if (ready < 0) {
        return NULL;
    }

    self->data = PyBytes_FromStringAndSize(NULL, self->size);
    glBindBuffer(GL_COPY_READ_BUFFER, self->buffer);
    glGetBufferSubData(GL_COPY_READ_BUFFER, 0, (intptr_t)self->size, PyBytes_AsString(self->data));
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glDeleteBuffers(1, &self->buffer);
    glDeleteSync(self->sync);
    self->buffer = 0;
    self->sync = NULL;
    return Py_NewRef(self->data);
}

static PyObject * HeadlessContext_meth_load_opengl_function(HeadlessContext * self, PyObject * arg) {
    const char * name = PyUnicode_AsUTF8(arg);
    if (!name) {
        return NULL;
    }
    return PyLong_FromVoidPtr(default_loader_load(name));
}

static PyObject * HeadlessContext_meth_release(HeadlessContext * self, PyObject * args) {
    if (!self->handles[0]) {
        Py_RETURN_NONE;
    }
    if (self->module_state->context) {
        Py_DECREF(release_context((Context *)self->module_state->context));
    }
    headless_release(self->handles);
    memset(self->handles, 0, sizeof(self->handles));
    Py_RETURN_NONE;
}

static Buffer * Context_meth_buffer(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"data", "size", "index", "usage", NULL};

    PyObject * data = Py_None;
    PyObject * size_arg = Py_None;
    int index = 0;
    const char * usage_arg = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O$Opz", keywords, &data, &size_arg, &index, &usage_arg)) {
        return NULL;
    }

    if (size_arg != Py_None && !PyLong_CheckExact(size_arg)) {
        PyErr_Format(PyExc_TypeError, "the size must be an int");
        return NULL;
    }

    if (data == Py_None && size_arg == Py_None) {
        PyErr_Format(PyExc_ValueError, "data or size is required");
        return NULL;
    }

    if (data != Py_None && size_arg != Py_None) {
        PyErr_Format(PyExc_ValueError, "data and size are exclusive");
        return NULL;
    }

    uint32_t target = index ? GL_ELEMENT_ARRAY_BUFFER : GL_COPY_WRITE_BUFFER;

    if (!usage_arg) {
        usage_arg = "static_draw";
    }

    uint32_t usage = 0;
    if (!get_named_value(buffer_usages, usage_arg, &usage)) {
        PyErr_Format(PyExc_ValueError, "invalid usage \"%s\"", usage_arg);
        return NULL;
    }

    int32_t size = 0;
    if (size_arg != Py_None) {
        size = (int32_t)PyLong_AsLong(size_arg);
    }

    if (data != Py_None) {
        Py_buffer view = {0};
        if (PyObject_GetBuffer(data, &view, PyBUF_FULL_RO)) {
            return NULL;
        }
        size = (int32_t)view.len;
        PyBuffer_Release(&view);
    }

    if (size <= 0) {
        PyErr_Format(PyExc_ValueError, "invalid size");
        return NULL;
    }

    if (target == GL_ELEMENT_ARRAY_BUFFER) {
        glBindVertexArray(0);
    }

    uint32_t buffer = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, (intptr_t)size, NULL, usage);
    glBindBuffer(target, 0);

    if (target == GL_ELEMENT_ARRAY_BUFFER) {
        restore_vertex_array(self);
    }

    Buffer * res = PyObject_New(Buffer, self->module_state->Buffer_type);
    res->gc_prev = NULL;
    res->gc_next = NULL;
    res->ctx = (Context *)Py_NewRef(self);
    res->buffer = buffer;
    res->target = target;
    res->size = size;
    res->index = (int8_t)index;
    res->external = 0;

    if (data != Py_None) {
        PyObject * write_args = PyTuple_Pack(1, data);
        PyObject * temp = Buffer_meth_write(res, write_args, NULL);
        Py_DECREF(write_args);
        if (!temp) {
            glDeleteBuffers(1, &res->buffer);
            Py_DECREF(res);
            return NULL;
        }
        Py_DECREF(temp);
    }

    track_object(self, (GCHeader *)res);
    return res;
}

static Texture * Context_meth_texture(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"size", "format", "data", "multisampled", "array", "levels", "cubemap", NULL};

    int32_t width = 0;
    int32_t height = 0;
    const char * format = "rgba8unorm";
    PyObject * data = Py_None;
    int multisampled = 0;
    int32_t array = 0;
    int32_t levels = 1;
    int cubemap = 0;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "(ii)|sO$piip", keywords,
        &width, &height, &format, &data,
        &multisampled, &array, &levels, &cubemap
    );

    if (!args_ok) {
        return NULL;
    }

    const ImageFormat * fmt = get_image_format(format);
    if (!fmt) {
        PyErr_Format(PyExc_ValueError, "invalid texture format \"%s\"", format);
        return NULL;
    }

    if (width <= 0 || height <= 0) {
        PyErr_Format(PyExc_ValueError, "invalid size");
        return NULL;
    }

    int32_t max_levels = count_mipmaps(width, height);
    if (levels <= 0) {
        levels = max_levels;
    }

    if (array < 0) {
        PyErr_Format(PyExc_ValueError, "array must not be negative");
        return NULL;
    }
    if (levels > max_levels) {
        PyErr_Format(PyExc_ValueError, "too many levels");
        return NULL;
    }
    if (cubemap && array) {
        PyErr_Format(PyExc_TypeError, "cubemap arrays are not supported");
        return NULL;
    }
    if (cubemap && width != height) {
        PyErr_Format(PyExc_ValueError, "cubemaps must be square");
        return NULL;
    }
    if (multisampled && (array || cubemap)) {
        PyErr_Format(PyExc_TypeError, "multisampled array or cubemap textures are not supported");
        return NULL;
    }
    if (data != Py_None && multisampled) {
        PyErr_Format(PyExc_ValueError, "cannot write to multisampled textures");
        return NULL;
    }
    uint32_t target = cubemap ? GL_TEXTURE_CUBE_MAP : array ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;

    uint32_t image = 0;
    if (multisampled) {
        glGenRenderbuffers(1, &image);
        glBindRenderbuffer(GL_RENDERBUFFER, image);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, fmt->internal_format, width, height);
    } else {
        glGenTextures(1, &image);
        glActiveTexture(self->default_texture_unit);
        glBindTexture(target, image);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, levels - 1);
        for (int32_t level = 0; level < levels; ++level) {
            int32_t w = least_one(width >> level);
            int32_t h = least_one(height >> level);
            if (cubemap) {
                for (int32_t i = 0; i < 6; ++i) {
                    uint32_t face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
                    glTexImage2D(face, level, fmt->internal_format, w, h, 0, fmt->format, fmt->type, NULL);
                }
            } else if (array) {
                glTexImage3D(target, level, fmt->internal_format, w, h, array, 0, fmt->format, fmt->type, NULL);
            } else {
                glTexImage2D(target, level, fmt->internal_format, w, h, 0, fmt->format, fmt->type, NULL);
            }
        }
    }

    Texture * res = PyObject_New(Texture, self->module_state->Texture_type);
    res->gc_prev = NULL;
    res->gc_next = NULL;
    res->ctx = (Context *)Py_NewRef(self);
    res->size = Py_BuildValue("(ii)", width, height);
    res->format = PyUnicode_FromString(fmt->name);
    res->fmt = fmt;
    res->texture = image;
    res->target = target;
    res->framebuffer = 0;
    res->width = width;
    res->height = height;
    res->array = array;
    res->cubemap = (int8_t)cubemap;
    res->multisampled = (int8_t)multisampled;
    res->external = 0;
    res->layer_count = (array ? array : 1) * (cubemap ? 6 : 1);
    res->level_count = levels;

    if (data != Py_None) {
        PyObject * write_args = PyTuple_Pack(1, data);
        PyObject * temp = Texture_meth_write(res, write_args, NULL);
        Py_DECREF(write_args);
        if (!temp) {
            glDeleteTextures(1, &res->texture);
            Py_DECREF(res);
            return NULL;
        }
        Py_DECREF(temp);
    }

    track_object(self, (GCHeader *)res);
    return res;
}

static Sampler * Context_meth_sampler(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {
        "texture", "min_filter", "mag_filter", "wrap_x", "wrap_y", "wrap_z", "min_lod", "max_lod", "lod_bias",
        "compare_func", "anisotropy", NULL,
    };

    Texture * texture = NULL;
    const char * min_filter_arg = NULL;
    const char * mag_filter_arg = "linear";
    const char * wrap_x_arg = "repeat";
    const char * wrap_y_arg = "repeat";
    const char * wrap_z_arg = "repeat";
    float min_lod = -1000.0f;
    float max_lod = 1000.0f;
    float lod_bias = 0.0f;
    const char * compare_func_arg = NULL;
    float anisotropy = 1.0f;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "O!|$zssssfffzf", keywords,
        self->module_state->Texture_type, &texture,
        &min_filter_arg, &mag_filter_arg, &wrap_x_arg, &wrap_y_arg, &wrap_z_arg,
        &min_lod, &max_lod, &lod_bias, &compare_func_arg, &anisotropy
    );

    if (!args_ok) {
        return NULL;
    }

    if (texture->multisampled) {
        PyErr_Format(PyExc_TypeError, "multisampled textures cannot be sampled");
        return NULL;
    }

    if (!min_filter_arg) {
        min_filter_arg = texture->level_count > 1 ? "linear_mipmap_linear" : "linear";
    }

    uint32_t min_filter = 0;
    uint32_t mag_filter = 0;
    uint32_t wraps_value[3] = {0};
    uint32_t compare_func = 0;

    if (!get_named_value(filters, min_filter_arg, &min_filter)) {
        PyErr_Format(PyExc_ValueError, "invalid min_filter \"%s\"", min_filter_arg);
        return NULL;
    }

    if (!get_named_value(filters, mag_filter_arg, &mag_filter) || mag_filter > GL_LINEAR) {
        PyErr_Format(PyExc_ValueError, "invalid mag_filter \"%s\", must be \"nearest\" or \"linear\"", mag_filter_arg);
        return NULL;
    }

    const char * wrap_args[3] = {wrap_x_arg, wrap_y_arg, wrap_z_arg};
    const char * wrap_names[3] = {"wrap_x", "wrap_y", "wrap_z"};

    for (int32_t i = 0; i < 3; ++i) {
        if (!get_named_value(wraps, wrap_args[i], &wraps_value[i])) {
            PyErr_Format(PyExc_ValueError, "invalid %s \"%s\"", wrap_names[i], wrap_args[i]);
            return NULL;
        }
    }

    if (compare_func_arg && !get_named_value(compare_funcs, compare_func_arg, &compare_func)) {
        PyErr_Format(PyExc_ValueError, "invalid compare_func \"%s\"", compare_func_arg);
        return NULL;
    }

    uint32_t sampler = 0;
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, min_filter);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, mag_filter);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, wraps_value[0]);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, wraps_value[1]);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, wraps_value[2]);
    glSamplerParameterf(sampler, GL_TEXTURE_MIN_LOD, min_lod);
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_LOD, max_lod);

    if (lod_bias != 0.0f) {
        glSamplerParameterf(sampler, GL_TEXTURE_LOD_BIAS, lod_bias);
    }

    if (compare_func) {
        glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, compare_func);
    }

    if (anisotropy != 1.0f) {
        glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
    }

    Sampler * res = PyObject_New(Sampler, self->module_state->Sampler_type);
    res->gc_prev = NULL;
    res->gc_next = NULL;
    res->ctx = (Context *)Py_NewRef(self);
    res->texture = (Texture *)Py_NewRef(texture);
    res->sampler = sampler;
    track_object(self, (GCHeader *)res);
    return res;
}

static Resources * Context_meth_resources(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"samplers", "uniform_buffers", "storage_buffers", "storage_images", NULL};

    PyObject * samplers = NULL;
    PyObject * uniform_buffers = NULL;
    PyObject * storage_buffers = NULL;
    PyObject * storage_images = NULL;

    PyObject * empty = PyTuple_New(0);
    int args_ok = PyArg_ParseTupleAndKeywords(
        empty, kwargs, "|$O!O!O!O!", keywords,
        &PyDict_Type, &samplers, &PyDict_Type, &uniform_buffers,
        &PyDict_Type, &storage_buffers, &PyDict_Type, &storage_images
    );
    Py_DECREF(empty);

    if (!args_ok) {
        return NULL;
    }

    Sampler * sampler_slots[MAX_SAMPLERS] = {0};
    BufferRange buffer_slots[MAX_BUFFERS] = {0};
    BufferRange storage_slots[MAX_BUFFERS] = {0};
    StorageImageSlot image_slots[MAX_BUFFERS] = {0};

    for (int32_t i = 0; i < PyTuple_Size(args); ++i) {
        Resources * base = (Resources *)PyTuple_GetItem(args, i);
        if (Py_TYPE(base) != self->module_state->Resources_type) {
            PyErr_Format(PyExc_TypeError, "positional arguments must be Resources");
            return NULL;
        }
        for (int32_t j = 0; j < base->sampler_count; ++j) {
            int32_t unit = base->samplers[j].unit;
            if (sampler_slots[unit]) {
                PyErr_Format(PyExc_ValueError, "sampler unit %d is set by more than one Resources, override it with samplers=", unit);
                return NULL;
            }
            sampler_slots[unit] = base->samplers[j].sampler;
        }
        for (int32_t j = 0; j < base->uniform_buffer_count; ++j) {
            int32_t binding = base->uniform_buffers[j].binding;
            if (buffer_slots[binding].buffer) {
                PyErr_Format(PyExc_ValueError, "uniform buffer binding %d is set by more than one Resources, override it with uniform_buffers=", binding);
                return NULL;
            }
            buffer_slots[binding] = base->uniform_buffers[j];
        }
        for (int32_t j = 0; j < base->storage_buffer_count; ++j) {
            int32_t binding = base->storage_buffers[j].binding;
            if (storage_slots[binding].buffer) {
                PyErr_Format(PyExc_ValueError, "storage buffer binding %d is set by more than one Resources, override it with storage_buffers=", binding);
                return NULL;
            }
            storage_slots[binding] = base->storage_buffers[j];
        }
        for (int32_t j = 0; j < base->storage_image_count; ++j) {
            int32_t unit = base->storage_images[j].unit;
            if (image_slots[unit].texture) {
                PyErr_Format(PyExc_ValueError, "storage image unit %d is set by more than one Resources, override it with storage_images=", unit);
                return NULL;
            }
            image_slots[unit] = base->storage_images[j];
        }
    }

    PyObject * key = NULL;
    PyObject * value = NULL;
    Py_ssize_t pos = 0;

    while (samplers && PyDict_Next(samplers, &pos, &key, &value)) {
        int32_t unit = PyLong_CheckExact(key) ? (int32_t)PyLong_AsLong(key) : -1;
        if (unit < 0 || unit >= self->max_samplers) {
            PyErr_Clear();
            PyErr_Format(PyExc_ValueError, "invalid sampler unit %R, must be between 0 and %d", key, self->max_samplers - 1);
            return NULL;
        }
        if (Py_TYPE(value) != self->module_state->Sampler_type) {
            PyErr_Format(PyExc_TypeError, "sampler unit %d must be a Sampler", unit);
            return NULL;
        }
        sampler_slots[unit] = (Sampler *)value;
    }

    pos = 0;
    while (uniform_buffers && PyDict_Next(uniform_buffers, &pos, &key, &value)) {
        int32_t binding = PyLong_CheckExact(key) ? (int32_t)PyLong_AsLong(key) : -1;
        if (binding < 0 || binding >= self->max_uniform_buffers) {
            PyErr_Clear();
            PyErr_Format(PyExc_ValueError, "invalid uniform buffer binding %R, must be between 0 and %d", key, self->max_uniform_buffers - 1);
            return NULL;
        }
        BufferRange range = {0};
        if (!parse_buffer_range(self, value, &range, 0)) {
            return NULL;
        }
        range.binding = binding;
        buffer_slots[binding] = range;
    }

    pos = 0;
    while (storage_buffers && PyDict_Next(storage_buffers, &pos, &key, &value)) {
        int32_t binding = PyLong_CheckExact(key) ? (int32_t)PyLong_AsLong(key) : -1;
        if (binding < 0 || binding >= self->max_storage_buffers) {
            PyErr_Clear();
            if (!self->max_storage_buffers) {
                PyErr_Format(PyExc_RuntimeError, "storage buffers are not supported, they need OpenGL 4.3");
                return NULL;
            }
            PyErr_Format(PyExc_ValueError, "invalid storage buffer binding %R, must be between 0 and %d", key, self->max_storage_buffers - 1);
            return NULL;
        }
        BufferRange range = {0};
        if (!parse_buffer_range(self, value, &range, 1)) {
            return NULL;
        }
        range.binding = binding;
        storage_slots[binding] = range;
    }

    pos = 0;
    while (storage_images && PyDict_Next(storage_images, &pos, &key, &value)) {
        int32_t unit = PyLong_CheckExact(key) ? (int32_t)PyLong_AsLong(key) : -1;
        if (unit < 0 || unit >= self->max_storage_images) {
            PyErr_Clear();
            if (!self->max_storage_images) {
                PyErr_Format(PyExc_RuntimeError, "storage images are not supported, they need OpenGL 4.2");
                return NULL;
            }
            PyErr_Format(PyExc_ValueError, "invalid storage image unit %R, must be between 0 and %d", key, self->max_storage_images - 1);
            return NULL;
        }
        StorageImageSlot slot = {0};
        if (!parse_storage_image(self, value, &slot)) {
            return NULL;
        }
        slot.unit = unit;
        image_slots[unit] = slot;
    }

    Resources * res = PyObject_New(Resources, self->module_state->Resources_type);
    res->ctx = (Context *)Py_NewRef(self);
    res->sampler_count = 0;
    res->uniform_buffer_count = 0;
    res->storage_buffer_count = 0;
    res->storage_image_count = 0;

    for (int32_t i = 0; i < MAX_SAMPLERS; ++i) {
        if (sampler_slots[i]) {
            res->samplers[res->sampler_count].unit = i;
            res->samplers[res->sampler_count].sampler = (Sampler *)Py_NewRef(sampler_slots[i]);
            res->sampler_count += 1;
        }
    }

    for (int32_t i = 0; i < MAX_BUFFERS; ++i) {
        if (buffer_slots[i].buffer) {
            res->uniform_buffers[res->uniform_buffer_count] = buffer_slots[i];
            Py_INCREF(buffer_slots[i].buffer);
            res->uniform_buffer_count += 1;
        }
        if (storage_slots[i].buffer) {
            res->storage_buffers[res->storage_buffer_count] = storage_slots[i];
            Py_INCREF(storage_slots[i].buffer);
            res->storage_buffer_count += 1;
        }
        if (image_slots[i].texture) {
            res->storage_images[res->storage_image_count] = image_slots[i];
            Py_INCREF(image_slots[i].texture);
            res->storage_image_count += 1;
        }
    }

    return res;
}

static VertexArray * Context_meth_vertex_array(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"attributes", "index_buffer", "short_index", "vertices", "mode", NULL};

    PyObject * attributes_arg = NULL;
    PyObject * index_buffer = Py_None;
    int short_index = 0;
    PyObject * vertices_arg = Py_None;
    const char * mode_arg = "triangles";

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "O|O$pOs", keywords,
        &attributes_arg, &index_buffer, &short_index, &vertices_arg, &mode_arg
    );

    if (!args_ok) {
        return NULL;
    }

    uint32_t mode = 0;
    if (!get_named_value(modes, mode_arg, &mode)) {
        PyErr_Format(PyExc_ValueError, "invalid mode \"%s\"", mode_arg);
        return NULL;
    }

    if (vertices_arg != Py_None && !PyLong_CheckExact(vertices_arg)) {
        PyErr_Format(PyExc_TypeError, "vertices must be an int or None");
        return NULL;
    }

    if (index_buffer != Py_None && Py_TYPE(index_buffer) != self->module_state->Buffer_type) {
        PyErr_Format(PyExc_TypeError, "the index buffer must be a Buffer or None");
        return NULL;
    }

    if (index_buffer != Py_None && !((Buffer *)index_buffer)->index) {
        PyErr_Format(PyExc_TypeError, "the index buffer must be an index_buffer");
        return NULL;
    }

    int32_t index_size = short_index ? 2 : 4;

    PyObject * attributes = PySequence_Tuple(attributes_arg);
    if (!attributes) {
        return NULL;
    }

    int32_t count = (int32_t)PyTuple_Size(attributes);

    if (count > MAX_ATTRIBUTES) {
        PyErr_Format(PyExc_ValueError, "too many attributes");
        Py_DECREF(attributes);
        return NULL;
    }

    Attribute parsed[MAX_ATTRIBUTES] = {0};
    uint8_t used[MAX_ATTRIBUTES] = {0};
    int32_t vertices = -1;

    for (int32_t i = 0; i < count; ++i) {
        PyObject * item = PyTuple_GetItem(attributes, i);
        Attribute * a = &parsed[i];
        const char * format = NULL;

        if (PyTuple_Check(item) && PyTuple_Size(item) && PyTuple_Check(PyTuple_GetItem(item, 0))) {
            PyErr_Format(PyExc_TypeError, "attribute %d is a tuple of attributes, did you mean *buffer.bind(...)?", i);
            Py_DECREF(attributes);
            return NULL;
        }

        int parsed = PyTuple_Check(item) && PyArg_ParseTuple(
            item, "O!isiii", self->module_state->Buffer_type,
            &a->buffer, &a->location, &format, &a->offset, &a->stride, &a->divisor
        );
        if (!parsed) {
            PyErr_Clear();
            PyErr_Format(PyExc_TypeError, "attribute %d must be a (buffer, location, format, offset, stride, divisor) tuple", i);
            Py_DECREF(attributes);
            return NULL;
        }

        a->fmt = get_vertex_format(format);

        if (a->buffer->index) {
            PyErr_Format(PyExc_TypeError, "attribute %d buffer is an index buffer", i);
            Py_DECREF(attributes);
            return NULL;
        }

        if (!a->fmt) {
            PyErr_Format(PyExc_ValueError, "attribute %d has an invalid format \"%s\"", i, format);
            Py_DECREF(attributes);
            return NULL;
        }

        if (a->location < 0 || a->location >= self->max_vertex_attribs) {
            PyErr_Format(PyExc_ValueError, "attribute %d has an invalid location %d", i, a->location);
            Py_DECREF(attributes);
            return NULL;
        }

        if (used[a->location]) {
            PyErr_Format(PyExc_ValueError, "attribute %d has a duplicate location %d", i, a->location);
            Py_DECREF(attributes);
            return NULL;
        }

        if (a->offset < 0 || a->stride < 0 || a->divisor < 0) {
            PyErr_Format(PyExc_ValueError, "attribute %d has an invalid offset, stride or divisor", i);
            Py_DECREF(attributes);
            return NULL;
        }

        used[a->location] = 1;

        if (a->divisor) {
            continue;
        }

        int32_t size = a->fmt->size;
        int32_t step = a->stride ? a->stride : size;
        int32_t available = a->buffer->size - a->offset < size ? 0 : (a->buffer->size - a->offset - size) / step + 1;
        if (vertices < 0 || available < vertices) {
            vertices = available;
        }
    }

    if (index_buffer != Py_None) {
        vertices = ((Buffer *)index_buffer)->size / index_size;
    }

    if (vertices < 0) {
        vertices = 0;
    }

    if (vertices_arg != Py_None) {
        vertices = (int32_t)PyLong_AsLong(vertices_arg);
    }

    uint32_t vertex_array = 0;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    for (int32_t i = 0; i < count; ++i) {
        Attribute * a = &parsed[i];
        glBindBuffer(GL_ARRAY_BUFFER, a->buffer->buffer);
        if (a->fmt->integer) {
            glVertexAttribIPointer(
                a->location, a->fmt->components, a->fmt->type, a->stride, (void *)(intptr_t)a->offset
            );
        } else {
            glVertexAttribPointer(
                a->location, a->fmt->components, a->fmt->type,
                a->fmt->normalized, a->stride, (void *)(intptr_t)a->offset
            );
        }
        glVertexAttribDivisor(a->location, a->divisor);
        glEnableVertexAttribArray(a->location);
    }

    if (index_buffer != Py_None) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ((Buffer *)index_buffer)->buffer);
    }

    restore_vertex_array(self);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    uint32_t index_type = 0;
    if (index_buffer != Py_None) {
        index_type = short_index ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    }

    VertexArray * res = PyObject_New(VertexArray, self->module_state->VertexArray_type);
    res->gc_prev = NULL;
    res->gc_next = NULL;
    res->ctx = (Context *)Py_NewRef(self);
    res->attributes = attributes;
    res->index_buffer = Py_NewRef(index_buffer);
    res->vertex_array = vertex_array;
    res->index_type = index_type;
    res->index_size = index_size;
    res->vertices = vertices;
    res->mode = mode;
    track_object(self, (GCHeader *)res);
    return res;
}

static Program * create_program(
    Context * self, PyObject * vertex_shader, PyObject * fragment_shader, PyObject * compute_shader,
    PyObject * samplers, PyObject * uniform_buffers, PyObject * storage_buffers, PyObject * storage_images,
    PyObject * declared
) {
    if (compute_shader && !glDispatchCompute) {
        PyErr_Format(PyExc_RuntimeError, "compute shaders are not supported, they need OpenGL 4.3");
        return NULL;
    }

    int32_t uniform_count = declared ? (int32_t)PyDict_Size(declared) : 0;

    UniformBinding * bindings = (UniformBinding *)PyMem_Malloc(sizeof(UniformBinding) * (uniform_count + 1));
    PyObject * uniform_index = PyDict_New();

    PyObject * key = NULL;
    PyObject * value = NULL;
    Py_ssize_t pos = 0;
    int32_t index = 0;
    while (declared && PyDict_Next(declared, &pos, &key, &value)) {
        UniformBinding * b = &bindings[index];
        int32_t items = 0;
        if (!PyUnicode_Check(key) || !parse_uniform_type(value, &b->gltype, &b->count, &b->array)) {
            PyErr_Format(PyExc_ValueError, "invalid uniform type %R for %R", value, key);
            break;
        }
        get_uniform_type(b->gltype, &b->function, &items, &b->type);
        b->values = b->count * items;
        b->location = -1;
        PyObject * i = PyLong_FromLong(index);
        PyDict_SetItem(uniform_index, key, i);
        Py_DECREF(i);
        index += 1;
    }

    uint32_t program = 0;

    if (!PyErr_Occurred()) {
        if (compute_shader) {
            program = compile_compute_program(self->shader_cache, self->shaders, compute_shader);
        } else {
            program = compile_program(self->shader_cache, self->shaders, vertex_shader, fragment_shader);
        }
    }

    if (!program) {
        PyMem_Free(bindings);
        Py_DECREF(uniform_index);
        return NULL;
    }

    glUseProgram(program);

    pos = 0;
    index = 0;
    while (declared && PyDict_Next(declared, &pos, &key, &value)) {
        UniformBinding * b = &bindings[index];
        b->location = glGetUniformLocation(program, PyUnicode_AsUTF8(key));
        index += 1;
    }

    int32_t active_uniforms = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &active_uniforms);

    for (int32_t i = 0; i < active_uniforms; ++i) {
        char name[256] = {0};
        int32_t length = 0;
        int32_t size = 0;
        uint32_t gltype = 0;
        glGetActiveUniform(program, i, 256, &length, &size, &gltype, name);
        int32_t location = glGetUniformLocation(program, name);

        int32_t array = length > 3 && !strcmp(name + length - 3, "[0]");
        if (array) {
            name[length - 3] = 0;
        }

        if (location < 0) {
            continue;
        }

        int32_t function = 0;
        int32_t items = 0;
        char type = 0;
        PyObject * found = PyDict_GetItemString(uniform_index, name);

        int32_t image = gltype >= 0x904C && gltype <= 0x906C;

        if (image) {
            if (found) {
                PyErr_Format(PyExc_ValueError, "storage image \"%s\" must be in storage_images, not in uniforms", name);
                break;
            }
            for (int32_t e = 0; e < size; ++e) {
                char element[300] = {0};
                snprintf(element, sizeof(element), array ? "%s[%d]" : "%s", name, e);
                int32_t binding = 0;
                if (!get_binding(storage_images, element, self->max_storage_images, &binding)) {
                    PyErr_Format(PyExc_ValueError, "storage image \"%s\" needs a unit between 0 and %d in storage_images", element, self->max_storage_images - 1);
                    break;
                }
                glUniform1i(glGetUniformLocation(program, element), binding);
            }
            if (PyErr_Occurred()) {
                break;
            }
            continue;
        }

        if (!get_uniform_type(gltype, &function, &items, &type)) {
            if (!get_value_name(sampler_types, gltype)) {
                PyErr_Format(PyExc_ValueError, "uniform \"%s\" has an unsupported type 0x%04X", name, gltype);
                break;
            }
            if (found) {
                PyErr_Format(PyExc_ValueError, "sampler \"%s\" must be in samplers, not in uniforms", name);
                break;
            }
            for (int32_t e = 0; e < size; ++e) {
                char element[300] = {0};
                snprintf(element, sizeof(element), array ? "%s[%d]" : "%s", name, e);
                int32_t binding = 0;
                if (!get_binding(samplers, element, self->max_samplers, &binding)) {
                    PyErr_Format(PyExc_ValueError, "sampler \"%s\" needs a unit between 0 and %d in samplers", element, self->max_samplers - 1);
                    break;
                }
                glUniform1i(glGetUniformLocation(program, element), binding);
            }
            if (PyErr_Occurred()) {
                break;
            }
            continue;
        }

        if (!found) {
            continue;
        }

        UniformBinding * b = &bindings[PyLong_AsLong(found)];
        if (b->gltype != gltype) {
            PyErr_Format(PyExc_ValueError, "uniform \"%s\" has a different type in the shader", name);
            break;
        }
        if (size > b->count) {
            PyErr_Format(PyExc_ValueError, "uniform \"%s\" has %d elements in the shader, declared %d", name, size, b->count);
            break;
        }
    }

    int32_t active_blocks = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &active_blocks);

    for (int32_t i = 0; i < active_blocks && !PyErr_Occurred(); ++i) {
        char name[256] = {0};
        glGetActiveUniformBlockName(program, i, 256, NULL, name);
        if (PyDict_GetItemString(uniform_index, name)) {
            PyErr_Format(PyExc_ValueError, "uniform block \"%s\" must be in uniform_buffers, not in uniforms", name);
            break;
        }
        int32_t binding = 0;
        if (!get_binding(uniform_buffers, name, self->max_uniform_buffers, &binding)) {
            PyErr_Format(PyExc_ValueError, "uniform block \"%s\" needs a binding between 0 and %d in uniform_buffers", name, self->max_uniform_buffers - 1);
            break;
        }
        glUniformBlockBinding(program, i, binding);
    }

    int32_t active_storage_blocks = 0;
    if (glGetProgramInterfaceiv) {
        glGetProgramInterfaceiv(program, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &active_storage_blocks);
    }

    for (int32_t i = 0; i < active_storage_blocks && !PyErr_Occurred(); ++i) {
        char name[256] = {0};
        glGetProgramResourceName(program, GL_SHADER_STORAGE_BLOCK, i, 256, NULL, name);
        int32_t binding = 0;
        if (!get_binding(storage_buffers, name, self->max_storage_buffers, &binding)) {
            PyErr_Format(PyExc_ValueError, "storage block \"%s\" needs a binding between 0 and %d in storage_buffers", name, self->max_storage_buffers - 1);
            break;
        }
        glShaderStorageBlockBinding(program, i, binding);
    }

    glUseProgram(bound_program_object(self));

    if (PyErr_Occurred()) {
        PyMem_Free(bindings);
        Py_DECREF(uniform_index);
        glDeleteProgram(program);
        return NULL;
    }

    Program * res = PyObject_New(Program, self->module_state->Program_type);
    res->gc_prev = NULL;
    res->gc_next = NULL;
    res->ctx = (Context *)Py_NewRef(self);
    res->uniform_index = uniform_index;
    res->samplers = samplers ? PyDict_Copy(samplers) : PyDict_New();
    res->uniform_buffers = uniform_buffers ? PyDict_Copy(uniform_buffers) : PyDict_New();
    res->storage_buffers = storage_buffers ? PyDict_Copy(storage_buffers) : PyDict_New();
    res->storage_images = storage_images ? PyDict_Copy(storage_images) : PyDict_New();
    res->uniform_bindings = bindings;
    res->program = program;
    res->pipelines = 0;

    track_object(self, (GCHeader *)res);
    return res;
}

static Program * Context_meth_program(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {
        "vertex_shader", "fragment_shader", "samplers", "uniform_buffers", "storage_buffers", "storage_images",
        "uniforms", NULL,
    };

    PyObject * vertex_shader = NULL;
    PyObject * fragment_shader = NULL;
    PyObject * samplers = NULL;
    PyObject * uniform_buffers = NULL;
    PyObject * storage_buffers = NULL;
    PyObject * storage_images = NULL;
    PyObject * declared = NULL;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "UU|$O!O!O!O!O!", keywords,
        &vertex_shader, &fragment_shader,
        &PyDict_Type, &samplers, &PyDict_Type, &uniform_buffers,
        &PyDict_Type, &storage_buffers, &PyDict_Type, &storage_images,
        &PyDict_Type, &declared
    );

    if (!args_ok) {
        return NULL;
    }

    return create_program(
        self, vertex_shader, fragment_shader, NULL, samplers, uniform_buffers, storage_buffers, storage_images, declared
    );
}

static Framebuffer * Context_meth_framebuffer(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"attachments", NULL};

    PyObject * attachments_arg = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &attachments_arg)) {
        return NULL;
    }

    PyObject * seq = PySequence_Tuple(attachments_arg);
    if (!seq) {
        return NULL;
    }

    int32_t count = (int32_t)PyTuple_Size(seq);

    if (count == 0) {
        Py_DECREF(seq);
        PyErr_Format(PyExc_ValueError, "the framebuffer has no attachments");
        return NULL;
    }

    PyObject * attachments = PyTuple_New(count);
    int32_t width = 0;
    int32_t height = 0;
    int8_t multisampled = 0;
    int32_t color_count = 0;

    for (int32_t i = 0; i < count; ++i) {
        PyObject * item = PyTuple_GetItem(seq, i);
        Texture * texture = NULL;
        int32_t layer = 0;
        int32_t level = 0;
        int is_view = PyTuple_Check(item) && PyTuple_Size(item) == 3;

        if (Py_TYPE(item) == self->module_state->Texture_type) {
            texture = (Texture *)item;
            if (texture->array || texture->cubemap) {
                PyErr_Format(PyExc_TypeError, "attachment %d is a layered texture, use texture.attachment()", i);
                Py_DECREF(attachments);
                Py_DECREF(seq);
                return NULL;
            }
        } else if (is_view && Py_TYPE(PyTuple_GetItem(item, 0)) == self->module_state->Texture_type) {
            texture = (Texture *)PyTuple_GetItem(item, 0);
            layer = (int32_t)PyLong_AsLong(PyTuple_GetItem(item, 1));
            level = (int32_t)PyLong_AsLong(PyTuple_GetItem(item, 2));
        }

        if (!texture || PyErr_Occurred()) {
            PyErr_Clear();
            PyErr_Format(PyExc_TypeError, "attachment %d must be a Texture or a (texture, layer, level) tuple", i);
            Py_DECREF(attachments);
            Py_DECREF(seq);
            return NULL;
        }

        if (layer < 0 || layer >= texture->layer_count || level < 0 || level >= texture->level_count) {
            PyErr_Format(PyExc_ValueError, "attachment %d has an invalid layer or level", i);
            Py_DECREF(attachments);
            Py_DECREF(seq);
            return NULL;
        }

        int32_t level_width = least_one(texture->width >> level);
        int32_t level_height = least_one(texture->height >> level);

        if (i == 0) {
            width = level_width;
            height = level_height;
            multisampled = texture->multisampled;
        }

        if (level_width != width || level_height != height) {
            PyErr_Format(PyExc_ValueError, "attachment %d has a different size", i);
            Py_DECREF(attachments);
            Py_DECREF(seq);
            return NULL;
        }

        if (texture->multisampled != multisampled) {
            PyErr_Format(PyExc_ValueError, "attachment %d is %smultisampled unlike the others", i, multisampled ? "not " : "");
            Py_DECREF(attachments);
            Py_DECREF(seq);
            return NULL;
        }

        if (!texture->fmt->color && i != count - 1) {
            PyErr_Format(PyExc_ValueError, "attachment %d is a depth attachment, it must be the last one", i);
            Py_DECREF(attachments);
            Py_DECREF(seq);
            return NULL;
        }

        if (texture->fmt->color) {
            color_count += 1;
        }

        PyTuple_SetItem(attachments, i, Py_BuildValue("(Oii)", texture, layer, level));
    }

    Py_DECREF(seq);

    if (color_count > MAX_ATTACHMENTS) {
        Py_DECREF(attachments);
        PyErr_Format(PyExc_ValueError, "too many color attachments");
        return NULL;
    }

    uint32_t framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    Framebuffer * res = new_framebuffer(self, framebuffer, attachments, width, height, multisampled);

    uint32_t draw_buffers[MAX_ATTACHMENTS] = {0};

    for (int32_t i = 0; i < count; ++i) {
        PyObject * item = PyTuple_GetItem(attachments, i);
        Texture * texture = (Texture *)PyTuple_GetItem(item, 0);
        int32_t layer = (int32_t)PyLong_AsLong(PyTuple_GetItem(item, 1));
        int32_t level = (int32_t)PyLong_AsLong(PyTuple_GetItem(item, 2));
        add_slot(res, texture->fmt);
        if (texture->fmt->color) {
            attach_texture(GL_COLOR_ATTACHMENT0 + i, texture, layer, level);
            draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
        } else {
            attach_texture(attachment_point(texture->fmt), texture, layer, level);
        }
    }

    if (color_count) {
        glDrawBuffers(color_count, draw_buffers);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
    } else {
        uint32_t none = GL_NONE;
        glDrawBuffers(1, &none);
        glReadBuffer(GL_NONE);
    }

    uint32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    restore_framebuffer(self);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        glDeleteFramebuffers(1, &res->framebuffer);
        Py_DECREF(res);
        PyErr_Format(PyExc_RuntimeError, "incomplete framebuffer 0x%x", status);
        return NULL;
    }

    track_object(self, (GCHeader *)res);
    return res;
}

static int check_resources(Context * self, Program * program, Resources * merged) {
    int8_t sampler_units[MAX_SAMPLERS] = {0};
    int8_t buffer_bindings[MAX_BUFFERS] = {0};
    int8_t storage_bindings[MAX_BUFFERS] = {0};
    int8_t image_units[MAX_BUFFERS] = {0};

    for (int32_t j = 0; j < merged->storage_image_count; ++j) {
        image_units[merged->storage_images[j].unit] = 1;
    }
    for (int32_t j = 0; j < merged->sampler_count; ++j) {
        sampler_units[merged->samplers[j].unit] = 1;
    }
    for (int32_t j = 0; j < merged->uniform_buffer_count; ++j) {
        buffer_bindings[merged->uniform_buffers[j].binding] = 1;
    }
    for (int32_t j = 0; j < merged->storage_buffer_count; ++j) {
        storage_bindings[merged->storage_buffers[j].binding] = 1;
    }

    PyObject * key = NULL;
    PyObject * value = NULL;
    Py_ssize_t pos = 0;

    while (PyDict_Next(program->samplers, &pos, &key, &value)) {
        const char * name = PyUnicode_Check(key) ? PyUnicode_AsUTF8(key) : NULL;
        int active = name && glGetUniformLocation(program->program, name) >= 0;
        if (active && !sampler_units[PyLong_AsLong(value)]) {
            PyErr_Format(PyExc_ValueError, "sampler \"%s\" has no resource at unit %d", name, (int32_t)PyLong_AsLong(value));
            return 0;
        }
    }

    pos = 0;
    while (PyDict_Next(program->uniform_buffers, &pos, &key, &value)) {
        const char * name = PyUnicode_Check(key) ? PyUnicode_AsUTF8(key) : NULL;
        int active = name && glGetUniformBlockIndex(program->program, name) != GL_INVALID_INDEX;
        if (active && !buffer_bindings[PyLong_AsLong(value)]) {
            PyErr_Format(PyExc_ValueError, "uniform block \"%s\" has no resource at binding %d", name, (int32_t)PyLong_AsLong(value));
            return 0;
        }
    }

    pos = 0;
    while (glGetProgramResourceIndex && PyDict_Next(program->storage_buffers, &pos, &key, &value)) {
        const char * name = PyUnicode_Check(key) ? PyUnicode_AsUTF8(key) : NULL;
        int active = name && glGetProgramResourceIndex(program->program, GL_SHADER_STORAGE_BLOCK, name) != GL_INVALID_INDEX;
        if (active && !storage_bindings[PyLong_AsLong(value)]) {
            PyErr_Format(PyExc_ValueError, "storage block \"%s\" has no resource at binding %d", name, (int32_t)PyLong_AsLong(value));
            return 0;
        }
    }

    pos = 0;
    while (PyDict_Next(program->storage_images, &pos, &key, &value)) {
        const char * name = PyUnicode_Check(key) ? PyUnicode_AsUTF8(key) : NULL;
        int active = name && glGetUniformLocation(program->program, name) >= 0;
        if (active && !image_units[PyLong_AsLong(value)]) {
            PyErr_Format(PyExc_ValueError, "storage image \"%s\" has no resource at unit %d", name, (int32_t)PyLong_AsLong(value));
            return 0;
        }
    }

    return 1;
}

static PyObject * empty_resources(Context * ctx) {
    PyObject * args = PyTuple_New(0);
    PyObject * res = (PyObject *)Context_meth_resources(ctx, args, NULL);
    Py_DECREF(args);
    return res;
}

static Program * build_program(
    Context * self, PyObject * vertex_shader, PyObject * fragment_shader, PyObject * compute_shader,
    PyObject * samplers, PyObject * uniform_buffers, PyObject * storage_buffers, PyObject * storage_images,
    PyObject * uniforms, PyObject * resources_arg, PyObject ** resources_out
) {
    PyObject * dicts[] = {samplers, uniform_buffers, storage_buffers, storage_images, uniforms};
    const char * names[] = {"samplers", "uniform_buffers", "storage_buffers", "storage_images", "uniforms"};
    for (int32_t i = 0; i < 5; ++i) {
        if (dicts[i] != Py_None && !PyDict_Check(dicts[i])) {
            PyErr_Format(PyExc_TypeError, "%s must be a dict or None", names[i]);
            return NULL;
        }
    }

    if (resources_arg && resources_arg != Py_None && Py_TYPE(resources_arg) != self->module_state->Resources_type) {
        PyErr_Format(PyExc_TypeError, "resources must be a Resources or None, combine them with ctx.resources(a, b)");
        return NULL;
    }

    Program * program = create_program(
        self, vertex_shader, fragment_shader, compute_shader,
        samplers != Py_None ? samplers : NULL, uniform_buffers != Py_None ? uniform_buffers : NULL,
        storage_buffers != Py_None ? storage_buffers : NULL, storage_images != Py_None ? storage_images : NULL,
        uniforms != Py_None ? uniforms : NULL
    );

    if (!program) {
        return NULL;
    }

    PyObject * resources = resources_arg && resources_arg != Py_None ? Py_NewRef(resources_arg) : empty_resources(self);

    if (!check_resources(self, program, (Resources *)resources)) {
        Py_DECREF(release_object(self, (PyObject *)program));
        Py_DECREF(program);
        Py_DECREF(resources);
        return NULL;
    }

    *resources_out = resources;
    return program;
}

static Pipeline * new_pipeline(Context * self, Program * program, PyObject * resources) {
    int32_t uniform_count = (int32_t)PyDict_Size(program->uniform_index);
    int32_t * uniform_offsets = (int32_t *)PyMem_Malloc(sizeof(int32_t) * (uniform_count + 1));
    int32_t data_size = 0;
    for (int32_t i = 0; i < uniform_count; ++i) {
        uniform_offsets[i] = data_size;
        data_size += program->uniform_bindings[i].values * 4;
    }
    char * uniform_data = (char *)PyMem_Malloc(data_size + 1);
    memset(uniform_data, 0, data_size + 1);

    Pipeline * res = PyObject_New(Pipeline, self->module_state->Pipeline_type);
    res->gc_prev = NULL;
    res->gc_next = NULL;
    res->ctx = (Context *)Py_NewRef(self);
    res->program = program;
    program->pipelines += 1;
    res->vertex_array = NULL;
    res->framebuffer = NULL;
    res->resources = resources;
    res->uniform_data = uniform_data;
    res->uniform_offsets = uniform_offsets;
    res->uniform_count = uniform_count;
    res->vertices = 0;
    res->instances = 1;
    res->first = 0;
    res->first_instance = 0;
    res->group_count[0] = 1;
    res->group_count[1] = 1;
    res->group_count[2] = 1;
    res->compute = 0;
    res->mode = 0;
    res->has_viewport = 0;
    res->has_scissor = 0;
    memset(&res->scissor, 0, sizeof(res->scissor));
    memset(&res->polygon_offset, 0, sizeof(res->polygon_offset));
    res->depth_test = 0;
    res->depth_write = 1;
    res->depth_func = GL_LESS;
    res->cull_face = 0;
    memset(&res->blend, 0, sizeof(res->blend));
    memset(&res->stencil, 0, sizeof(res->stencil));
    return res;
}

static void release_pipeline_program(Context * self, Pipeline * pipeline) {
    pipeline->program->pipelines -= 1;
    if (!pipeline->program->pipelines) {
        Py_DECREF(release_object(self, (PyObject *)pipeline->program));
    }
}

static Pipeline * Context_meth_compute(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {
        "compute_shader", "samplers", "uniform_buffers", "storage_buffers", "storage_images", "uniforms", "resources",
        "group_count", NULL,
    };

    PyObject * compute_shader = NULL;
    PyObject * samplers = Py_None;
    PyObject * uniform_buffers = Py_None;
    PyObject * storage_buffers = Py_None;
    PyObject * storage_images = Py_None;
    PyObject * uniforms = Py_None;
    PyObject * resources_arg = NULL;
    PyObject * group_count = NULL;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "U|$OOOOOOO", keywords,
        &compute_shader,
        &samplers, &uniform_buffers, &storage_buffers, &storage_images, &uniforms,
        &resources_arg, &group_count
    );

    if (!args_ok) {
        return NULL;
    }

    PyObject * resources = NULL;
    Program * program = build_program(
        self, NULL, NULL, compute_shader,
        samplers, uniform_buffers, storage_buffers, storage_images, uniforms,
        resources_arg, &resources
    );
    if (!program) {
        return NULL;
    }

    Pipeline * res = new_pipeline(self, program, resources);
    res->compute = 1;
    res->vertex_array = (VertexArray *)Py_NewRef(Py_None);
    res->framebuffer = (Framebuffer *)Py_NewRef(Py_None);

    if (group_count && Pipeline_set_group_count(res, group_count, NULL)) {
        release_pipeline_program(self, res);
        Py_DECREF(res);
        return NULL;
    }

    track_object(self, (GCHeader *)res);
    return res;
}

static PyObject * Context_meth_memory_barrier(Context * self, PyObject * args) {
    if (!glMemoryBarrier) {
        PyErr_Format(PyExc_RuntimeError, "memory barriers are not supported, they need OpenGL 4.2");
        return NULL;
    }
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    Py_RETURN_NONE;
}

static Pipeline * Context_meth_pipeline(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {
        "vertex_shader", "fragment_shader", "samplers", "uniform_buffers", "storage_buffers", "storage_images",
        "uniforms", "vertex_array", "framebuffer", "resources", "vertices", "instances", "first", "first_instance",
        "mode", "viewport", "scissor", "polygon_offset", "depth_test", "depth_write", "depth_func", "cull_face",
        "blend", "stencil", NULL,
    };

    PyObject * vertex_shader = NULL;
    PyObject * fragment_shader = NULL;
    PyObject * samplers = Py_None;
    PyObject * uniform_buffers = Py_None;
    PyObject * storage_buffers = Py_None;
    PyObject * storage_images = Py_None;
    PyObject * uniforms = Py_None;
    VertexArray * vertex_array = NULL;
    Framebuffer * framebuffer = NULL;
    PyObject * resources_arg = NULL;
    PyObject * vertices_arg = Py_None;
    int32_t instances = 1;
    int32_t first = 0;
    int32_t first_instance = 0;
    const char * mode_arg = NULL;
    PyObject * viewport_arg = Py_None;
    PyObject * scissor_arg = Py_None;
    PyObject * polygon_offset_arg = Py_None;
    int depth_test = 0;
    int depth_write = 1;
    const char * depth_func_arg = "less";
    const char * cull_face_arg = NULL;
    PyObject * blend_arg = Py_None;
    PyObject * stencil_arg = Py_None;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "UU|$OOOOOO!O!OOiiizOOOppszOO", keywords,
        &vertex_shader, &fragment_shader, &samplers, &uniform_buffers, &storage_buffers, &storage_images, &uniforms,
        self->module_state->VertexArray_type, &vertex_array, self->module_state->Framebuffer_type, &framebuffer,
        &resources_arg, &vertices_arg, &instances, &first, &first_instance, &mode_arg,
        &viewport_arg, &scissor_arg, &polygon_offset_arg,
        &depth_test, &depth_write, &depth_func_arg, &cull_face_arg, &blend_arg, &stencil_arg
    );

    if (!args_ok) {
        return NULL;
    }

    if (!vertex_array || !framebuffer) {
        PyErr_Format(PyExc_TypeError, "vertex_array and framebuffer are required");
        return NULL;
    }

    if (vertices_arg != Py_None && !PyLong_CheckExact(vertices_arg)) {
        PyErr_Format(PyExc_TypeError, "vertices must be an int or None");
        return NULL;
    }

    int32_t vertices = vertices_arg != Py_None ? (int32_t)PyLong_AsLong(vertices_arg) : vertex_array->vertices;

    if (vertices < 0 || instances < 0 || first < 0 || first_instance < 0) {
        PyErr_Format(PyExc_ValueError, "vertices, instances, first and first_instance must not be negative");
        return NULL;
    }

    uint32_t mode = 0;
    if (!parse_mode(vertex_array, mode_arg, &mode)) {
        return NULL;
    }

    uint32_t depth_func = 0;
    if (!get_named_value(compare_funcs, depth_func_arg, &depth_func)) {
        PyErr_Format(PyExc_ValueError, "invalid depth_func \"%s\"", depth_func_arg);
        return NULL;
    }

    uint32_t cull_face = 0;
    if (cull_face_arg && !get_named_value(cull_faces, cull_face_arg, &cull_face)) {
        PyErr_Format(PyExc_ValueError, "invalid cull_face \"%s\"", cull_face_arg);
        return NULL;
    }

    PolygonOffset polygon_offset = {0};
    if (!parse_polygon_offset(polygon_offset_arg, &polygon_offset)) {
        return NULL;
    }

    BlendState blend = {0};
    StencilState stencil = {0};
    PyObject * empty = PyTuple_New(0);

    if (blend_arg != Py_None && (!PyDict_Check(blend_arg) || !parse_blend(empty, blend_arg, &blend))) {
        Py_DECREF(empty);
        if (!PyErr_Occurred()) {
            PyErr_Format(PyExc_TypeError, "blend must be a dict or None");
        }
        return NULL;
    }

    if (stencil_arg != Py_None && (!PyDict_Check(stencil_arg) || !parse_stencil(empty, stencil_arg, &stencil))) {
        Py_DECREF(empty);
        if (!PyErr_Occurred()) {
            PyErr_Format(PyExc_TypeError, "stencil must be a dict or None");
        }
        return NULL;
    }

    Py_DECREF(empty);

    PyObject * resources = NULL;
    Program * program = build_program(
        self, vertex_shader, fragment_shader, NULL,
        samplers, uniform_buffers, storage_buffers, storage_images, uniforms,
        resources_arg, &resources
    );
    if (!program) {
        return NULL;
    }

    Pipeline * res = new_pipeline(self, program, resources);
    res->vertex_array = (VertexArray *)Py_NewRef(vertex_array);
    res->framebuffer = (Framebuffer *)Py_NewRef(framebuffer);
    res->vertices = vertices;
    res->instances = instances;
    res->first = first;
    res->first_instance = first_instance;
    res->mode = mode;
    res->depth_test = (int8_t)depth_test;
    res->depth_write = (int8_t)depth_write;
    res->depth_func = depth_func;
    res->cull_face = cull_face;
    res->blend = blend;
    res->stencil = stencil;
    res->polygon_offset = polygon_offset;

    if (Pipeline_set_viewport(res, viewport_arg, NULL) || Pipeline_set_scissor(res, scissor_arg, NULL)) {
        release_pipeline_program(self, res);
        Py_DECREF(res);
        return NULL;
    }

    track_object(self, (GCHeader *)res);
    return res;
}

static int same_framebuffer_layout(Framebuffer * a, Framebuffer * b) {
    if (a->color_count != b->color_count || a->depth_format != b->depth_format || a->multisampled != b->multisampled) {
        return 0;
    }
    for (int32_t i = 0; i < a->color_count; ++i) {
        if (a->color_formats[i] != b->color_formats[i]) {
            return 0;
        }
    }
    return 1;
}

typedef struct VertexLayout {
    const VertexFormat * fmt;
    int32_t stride;
    int32_t divisor;
} VertexLayout;

static void vertex_layout(VertexArray * vertex_array, VertexLayout * layout) {
    int32_t count = (int32_t)PyTuple_Size(vertex_array->attributes);
    for (int32_t i = 0; i < count; ++i) {
        PyObject * item = PyTuple_GetItem(vertex_array->attributes, i);
        int32_t location = (int32_t)PyLong_AsLong(PyTuple_GetItem(item, 1));
        const VertexFormat * fmt = get_vertex_format(PyUnicode_AsUTF8(PyTuple_GetItem(item, 2)));
        int32_t stride = (int32_t)PyLong_AsLong(PyTuple_GetItem(item, 4));
        layout[location].fmt = fmt;
        layout[location].stride = stride ? stride : fmt->size;
        layout[location].divisor = (int32_t)PyLong_AsLong(PyTuple_GetItem(item, 5));
    }
}

static int same_vertex_layout(VertexArray * a, VertexArray * b) {
    VertexLayout layout_a[MAX_ATTRIBUTES] = {0};
    VertexLayout layout_b[MAX_ATTRIBUTES] = {0};
    vertex_layout(a, layout_a);
    vertex_layout(b, layout_b);
    return !memcmp(layout_a, layout_b, sizeof(layout_a));
}

static Pipeline * Pipeline_meth_variant(Pipeline * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"vertex_array", "framebuffer", "resources", NULL};

    Context * ctx = self->ctx;
    VertexArray * vertex_array = NULL;
    Framebuffer * framebuffer = NULL;
    PyObject * resources_arg = NULL;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "|$O!O!O", keywords,
        ctx->module_state->VertexArray_type, &vertex_array,
        ctx->module_state->Framebuffer_type, &framebuffer,
        &resources_arg
    );

    if (!args_ok) {
        return NULL;
    }

    if (self->compute && (vertex_array || framebuffer || !resources_arg)) {
        PyErr_Format(PyExc_TypeError, "compute variants take resources only");
        return NULL;
    }

    if (!self->compute && (!vertex_array || !framebuffer || !resources_arg)) {
        PyErr_Format(PyExc_TypeError, "vertex_array, framebuffer and resources are required");
        return NULL;
    }

    if (resources_arg != Py_None && Py_TYPE(resources_arg) != ctx->module_state->Resources_type) {
        PyErr_Format(PyExc_TypeError, "resources must be a Resources or None, combine them with ctx.resources(a, b)");
        return NULL;
    }

    if (!self->compute && !same_framebuffer_layout(self->framebuffer, framebuffer)) {
        PyErr_Format(PyExc_ValueError, "the framebuffer must have the same formats and multisampling as the pipeline's framebuffer");
        return NULL;
    }

    if (!self->compute && !same_vertex_layout(self->vertex_array, vertex_array)) {
        PyErr_Format(PyExc_ValueError, "the vertex array must have the same locations, formats, strides and divisors as the pipeline's vertex array");
        return NULL;
    }

    PyObject * resources = resources_arg != Py_None ? Py_NewRef(resources_arg) : empty_resources(ctx);

    if (!check_resources(ctx, self->program, (Resources *)resources)) {
        Py_DECREF(resources);
        return NULL;
    }

    Pipeline * res = new_pipeline(ctx, (Program *)Py_NewRef(self->program), resources);
    res->compute = self->compute;
    res->vertex_array = (VertexArray *)Py_NewRef(self->compute ? Py_None : (PyObject *)vertex_array);
    res->framebuffer = (Framebuffer *)Py_NewRef(self->compute ? Py_None : (PyObject *)framebuffer);
    res->vertices = self->compute ? 0 : vertex_array->vertices;
    res->instances = self->instances;
    res->first = self->first;
    res->first_instance = self->first_instance;
    memcpy(res->group_count, self->group_count, sizeof(res->group_count));
    res->mode = self->mode;
    res->has_viewport = self->has_viewport;
    res->viewport = self->viewport;
    res->has_scissor = self->has_scissor;
    res->scissor = self->scissor;
    res->polygon_offset = self->polygon_offset;
    res->depth_test = self->depth_test;
    res->depth_write = self->depth_write;
    res->depth_func = self->depth_func;
    res->cull_face = self->cull_face;
    res->blend = self->blend;
    res->stencil = self->stencil;
    track_object(ctx, (GCHeader *)res);
    return res;
}

static Buffer * Context_meth_external_buffer(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"buffer", "size", "index", NULL};

    uint32_t buffer = 0;
    PyObject * size_arg = Py_None;
    int index = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "I|O$p", keywords, &buffer, &size_arg, &index)) {
        return NULL;
    }

    if (size_arg != Py_None && !PyLong_CheckExact(size_arg)) {
        PyErr_Format(PyExc_TypeError, "the size must be an int or None");
        return NULL;
    }

    uint32_t target = index ? GL_ELEMENT_ARRAY_BUFFER : GL_COPY_WRITE_BUFFER;

    int32_t size = 0;
    if (size_arg != Py_None) {
        size = (int32_t)PyLong_AsLong(size_arg);
    } else {
        glBindBuffer(GL_COPY_READ_BUFFER, buffer);
        glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);
        glBindBuffer(GL_COPY_READ_BUFFER, 0);
    }

    if (size <= 0) {
        PyErr_Format(PyExc_ValueError, "invalid size");
        return NULL;
    }

    Buffer * res = PyObject_New(Buffer, self->module_state->Buffer_type);
    res->gc_prev = NULL;
    res->gc_next = NULL;
    res->ctx = (Context *)Py_NewRef(self);
    res->buffer = buffer;
    res->target = target;
    res->size = size;
    res->index = (int8_t)index;
    res->external = 1;
    track_object(self, (GCHeader *)res);
    return res;
}

static Texture * Context_meth_external_texture(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"texture", "size", "format", "multisampled", "array", "levels", "cubemap", NULL};

    uint32_t image = 0;
    int32_t width = 0;
    int32_t height = 0;
    const char * format = "rgba8unorm";
    int multisampled = 0;
    int32_t array = 0;
    int32_t levels = 1;
    int cubemap = 0;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "I(ii)|s$piip", keywords,
        &image, &width, &height, &format,
        &multisampled, &array, &levels, &cubemap
    );

    if (!args_ok) {
        return NULL;
    }

    const ImageFormat * fmt = get_image_format(format);
    if (!fmt) {
        PyErr_Format(PyExc_ValueError, "invalid texture format \"%s\"", format);
        return NULL;
    }

    if (width <= 0 || height <= 0 || array < 0 || levels < 1 || (cubemap && array)) {
        PyErr_Format(PyExc_ValueError, "invalid size, array, levels or cubemap");
        return NULL;
    }

    if (multisampled && (array || cubemap)) {
        PyErr_Format(PyExc_TypeError, "multisampled array or cubemap textures are not supported");
        return NULL;
    }

    Texture * res = PyObject_New(Texture, self->module_state->Texture_type);
    res->gc_prev = NULL;
    res->gc_next = NULL;
    res->ctx = (Context *)Py_NewRef(self);
    res->size = Py_BuildValue("(ii)", width, height);
    res->format = PyUnicode_FromString(fmt->name);
    res->fmt = fmt;
    res->texture = image;
    res->target = cubemap ? GL_TEXTURE_CUBE_MAP : array ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
    res->framebuffer = 0;
    res->width = width;
    res->height = height;
    res->array = array;
    res->cubemap = (int8_t)cubemap;
    res->multisampled = (int8_t)multisampled;
    res->external = 1;
    res->layer_count = (array ? array : 1) * (cubemap ? 6 : 1);
    res->level_count = levels;
    track_object(self, (GCHeader *)res);
    return res;
}

static Framebuffer * create_external_framebuffer(
    Context * self, uint32_t framebuffer, int32_t width, int32_t height, PyObject * formats, int8_t multisampled
) {
    PyObject * seq = PySequence_Tuple(formats);
    if (!seq) {
        return NULL;
    }

    int32_t count = (int32_t)PyTuple_Size(seq);

    if (count == 0 || count > MAX_ATTACHMENTS + 1) {
        Py_DECREF(seq);
        PyErr_Format(PyExc_ValueError, "invalid number of formats");
        return NULL;
    }

    if (width <= 0 || height <= 0) {
        Py_DECREF(seq);
        PyErr_Format(PyExc_ValueError, "invalid size");
        return NULL;
    }

    const ImageFormat * slot_formats[MAX_ATTACHMENTS + 1] = {0};
    int32_t color_count = 0;

    for (int32_t i = 0; i < count; ++i) {
        PyObject * name = PyTuple_GetItem(seq, i);
        const char * format = PyUnicode_Check(name) ? PyUnicode_AsUTF8(name) : NULL;
        const ImageFormat * fmt = format ? get_image_format(format) : NULL;

        if (!fmt) {
            Py_DECREF(seq);
            PyErr_Format(PyExc_ValueError, "invalid format at %d", i);
            return NULL;
        }

        if (!fmt->color && i != count - 1) {
            Py_DECREF(seq);
            PyErr_Format(PyExc_ValueError, "format %d is a depth format, it must be the last one", i);
            return NULL;
        }

        if (fmt->color) {
            color_count += 1;
        }

        slot_formats[i] = fmt;
    }

    Py_DECREF(seq);

    if (color_count > MAX_ATTACHMENTS) {
        PyErr_Format(PyExc_ValueError, "too many color formats");
        return NULL;
    }

    PyObject * attachments = PyTuple_New(0);
    Framebuffer * res = new_framebuffer(self, framebuffer, attachments, width, height, multisampled);

    for (int32_t i = 0; i < count; ++i) {
        add_slot(res, slot_formats[i]);
    }

    res->external = 1;
    return res;
}

static Framebuffer * Context_meth_external_framebuffer(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"framebuffer", "size", "formats", "multisampled", NULL};

    uint32_t framebuffer = 0;
    int32_t width = 0;
    int32_t height = 0;
    PyObject * formats = NULL;
    int multisampled = 0;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "I(ii)|O$p", keywords,
        &framebuffer, &width, &height, &formats, &multisampled
    );

    if (!args_ok) {
        return NULL;
    }

    PyObject * default_formats = formats ? NULL : Py_BuildValue("(ss)", "rgba8unorm", "depth24plus-stencil8");
    Framebuffer * res = create_external_framebuffer(
        self, framebuffer, width, height, formats ? formats : default_formats, (int8_t)multisampled
    );
    Py_XDECREF(default_formats);
    if (res) {
        track_object(self, (GCHeader *)res);
    }
    return res;
}

static PyObject * Context_meth_copy_buffer(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"src", "dst", "size", "src_offset", "dst_offset", NULL};

    Buffer * src = NULL;
    Buffer * dst = NULL;
    PyObject * size_arg = Py_None;
    int32_t src_offset = 0;
    int32_t dst_offset = 0;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "O!O!|O$ii", keywords,
        self->module_state->Buffer_type, &src, self->module_state->Buffer_type, &dst,
        &size_arg, &src_offset, &dst_offset
    );

    if (!args_ok) {
        return NULL;
    }

    if (size_arg != Py_None && !PyLong_CheckExact(size_arg)) {
        PyErr_Format(PyExc_TypeError, "the size must be an int");
        return NULL;
    }

    if ((src->target == GL_ELEMENT_ARRAY_BUFFER) != (dst->target == GL_ELEMENT_ARRAY_BUFFER)) {
        PyErr_Format(PyExc_TypeError, "index buffers can only be copied to index buffers");
        return NULL;
    }

    int32_t size = src->size - src_offset;
    if (size_arg != Py_None) {
        size = (int32_t)PyLong_AsLong(size_arg);
    }

    int fits = src_offset + size <= src->size && dst_offset + size <= dst->size;
    if (src_offset < 0 || dst_offset < 0 || size <= 0 || !fits) {
        PyErr_Format(PyExc_ValueError, "invalid size or offset");
        return NULL;
    }

    glBindBuffer(GL_COPY_READ_BUFFER, src->buffer);
    glBindBuffer(GL_COPY_WRITE_BUFFER, dst->buffer);
    glCopyBufferSubData(
        GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, (intptr_t)src_offset, (intptr_t)dst_offset, (intptr_t)size
    );
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    Py_RETURN_NONE;
}

static PyObject * Context_meth_copy_texture(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"src", "dst", "size", "src_offset", "dst_offset", "dst_size", "filter", "flip_y", NULL};

    Texture * src = NULL;
    PyObject * dst = NULL;
    PyObject * size_arg = Py_None;
    PyObject * src_offset_arg = Py_None;
    PyObject * dst_offset_arg = Py_None;
    PyObject * dst_size_arg = Py_None;
    const char * filter_arg = "nearest";
    int flip_y = 0;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "O!O|O$OOOsp", keywords,
        self->module_state->Texture_type, &src, &dst,
        &size_arg, &src_offset_arg, &dst_offset_arg, &dst_size_arg,
        &filter_arg, &flip_y
    );

    if (!args_ok) {
        return NULL;
    }

    Texture * dst_texture = NULL;
    Framebuffer * dst_framebuffer = NULL;

    if (Py_TYPE(dst) == self->module_state->Texture_type) {
        dst_texture = (Texture *)dst;
    } else if (Py_TYPE(dst) == self->module_state->Framebuffer_type) {
        dst_framebuffer = (Framebuffer *)dst;
    } else {
        PyErr_Format(PyExc_TypeError, "dst must be a Texture or a Framebuffer");
        return NULL;
    }

    if (dst_framebuffer && !dst_framebuffer->color_count) {
        PyErr_Format(PyExc_TypeError, "the framebuffer has no color attachment");
        return NULL;
    }

    if (dst_framebuffer && !src->fmt->color) {
        PyErr_Format(PyExc_TypeError, "only color textures can be copied to framebuffers");
        return NULL;
    }

    const ImageFormat * dst_fmt = dst_texture ? dst_texture->fmt : dst_framebuffer->color_formats[0];
    int8_t dst_multisampled = dst_texture ? dst_texture->multisampled : dst_framebuffer->multisampled;
    int32_t dst_width = dst_texture ? dst_texture->width : dst_framebuffer->width;
    int32_t dst_height = dst_texture ? dst_texture->height : dst_framebuffer->height;

    if (dst_texture == src) {
        PyErr_Format(PyExc_ValueError, "src and dst are the same");
        return NULL;
    }

    if (dst_multisampled) {
        PyErr_Format(PyExc_TypeError, "cannot copy to multisampled targets");
        return NULL;
    }

    if (src->fmt->color != dst_fmt->color) {
        PyErr_Format(PyExc_TypeError, "cannot copy between color and depth textures");
        return NULL;
    }

    if (src->fmt->color && src->fmt->clear_type != dst_fmt->clear_type) {
        PyErr_Format(PyExc_TypeError, "cannot copy between float, int and uint textures");
        return NULL;
    }

    if (!src->fmt->color && src->fmt != dst_fmt) {
        PyErr_Format(PyExc_TypeError, "depth and stencil formats must match");
        return NULL;
    }

    if (src->multisampled && src->fmt != dst_fmt) {
        PyErr_Format(PyExc_TypeError, "multisampled formats must match");
        return NULL;
    }

    Region src_region = {0};
    if (!parse_region(src, 0, size_arg, src_offset_arg, &src_region)) {
        return NULL;
    }

    Region dst_region = {0};

    if (!to_int_pair(&dst_region.x, &dst_region.y, dst_offset_arg, 0, 0)) {
        PyErr_Format(PyExc_TypeError, "the offset must be a tuple of 2 ints");
        return NULL;
    }

    if (!to_int_pair(&dst_region.width, &dst_region.height, dst_size_arg, src_region.width, src_region.height)) {
        PyErr_Format(PyExc_TypeError, "the dst_size must be a tuple of 2 ints");
        return NULL;
    }

    uint32_t filter = 0;
    if (!get_named_value(filters, filter_arg, &filter) || filter > GL_LINEAR) {
        PyErr_Format(PyExc_ValueError, "invalid filter \"%s\", must be \"nearest\" or \"linear\"", filter_arg);
        return NULL;
    }

    if (filter == GL_LINEAR && !src->fmt->color) {
        PyErr_Format(PyExc_TypeError, "depth and stencil copies must use the nearest filter");
        return NULL;
    }

    if (dst_region.width <= 0 || dst_region.height <= 0) {
        PyErr_Format(PyExc_ValueError, "invalid dst_size");
        return NULL;
    }

    int outside_x = dst_region.x < 0 || dst_region.x + dst_region.width > dst_width;
    int outside_y = dst_region.y < 0 || dst_region.y + dst_region.height > dst_height;
    if (dst_texture && (outside_x || outside_y)) {
        PyErr_Format(PyExc_ValueError, "invalid size or offset");
        return NULL;
    }

    int scaled = dst_region.width != src_region.width || dst_region.height != src_region.height;

    if (src->multisampled && (scaled || flip_y || src_region.x != dst_region.x || src_region.y != dst_region.y)) {
        PyErr_Format(PyExc_ValueError, "multisampled copies must have matching offsets and sizes, resolve first");
        return NULL;
    }

    uint32_t read_framebuffer = texture_framebuffer(src);
    if (!read_framebuffer) {
        return NULL;
    }

    uint32_t draw_framebuffer = dst_texture ? texture_framebuffer(dst_texture) : dst_framebuffer->framebuffer;
    if (dst_texture && !draw_framebuffer) {
        return NULL;
    }

    uint32_t mask = GL_COLOR_BUFFER_BIT;
    if (src->fmt->buffer == GL_DEPTH) {
        mask = GL_DEPTH_BUFFER_BIT;
    }
    if (src->fmt->buffer == GL_DEPTH_STENCIL) {
        mask = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
    }

    int32_t sx0 = src_region.x;
    int32_t sy0 = src_region.y;
    int32_t sx1 = src_region.x + src_region.width;
    int32_t sy1 = src_region.y + src_region.height;
    int32_t dx0 = dst_region.x;
    int32_t dy0 = flip_y ? dst_region.y + dst_region.height : dst_region.y;
    int32_t dx1 = dst_region.x + dst_region.width;
    int32_t dy1 = flip_y ? dst_region.y : dst_region.y + dst_region.height;

    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_framebuffer);
    suspend_scissor(self);
    glBlitFramebuffer(sx0, sy0, sx1, sy1, dx0, dy0, dx1, dy1, mask, filter);
    resume_scissor(self);
    restore_framebuffer(self);
    Py_RETURN_NONE;
}

static PyObject * Context_meth_copy_buffer_to_texture(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"src", "dst", "size", "src_offset", "dst_offset", "dst_layer", "dst_level", NULL};

    Buffer * src = NULL;
    Texture * dst = NULL;
    PyObject * size_arg = Py_None;
    int32_t src_offset = 0;
    PyObject * dst_offset_arg = Py_None;
    int32_t dst_layer = 0;
    int32_t dst_level = 0;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "O!O!|O$iOii", keywords,
        self->module_state->Buffer_type, &src, self->module_state->Texture_type, &dst,
        &size_arg, &src_offset, &dst_offset_arg, &dst_layer, &dst_level
    );

    if (!args_ok) {
        return NULL;
    }

    if (src->target == GL_ELEMENT_ARRAY_BUFFER) {
        PyErr_Format(PyExc_TypeError, "index buffers cannot be copied to textures");
        return NULL;
    }

    if (dst->multisampled) {
        PyErr_Format(PyExc_TypeError, "cannot copy to multisampled textures");
        return NULL;
    }

    if (!dst->fmt->color) {
        PyErr_Format(PyExc_TypeError, "cannot copy to depth or stencil textures");
        return NULL;
    }

    if (dst_layer < 0 || dst_layer >= dst->layer_count || dst_level < 0 || dst_level >= dst->level_count) {
        PyErr_Format(PyExc_ValueError, "invalid dst_layer or dst_level");
        return NULL;
    }

    Region region = {0};
    if (!parse_region(dst, dst_level, size_arg, dst_offset_arg, &region)) {
        return NULL;
    }

    int32_t size = region_bytes(dst, &region);

    if (src_offset < 0 || src_offset + size > src->size) {
        PyErr_Format(PyExc_ValueError, "invalid src_offset, %d bytes required", size);
        return NULL;
    }

    const ImageFormat * fmt = dst->fmt;
    void * ptr = (void *)(intptr_t)src_offset;

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, src->buffer);
    glActiveTexture(self->default_texture_unit);
    glBindTexture(dst->target, dst->texture);

    if (dst->cubemap) {
        uint32_t face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + dst_layer;
        glTexSubImage2D(face, dst_level, region.x, region.y, region.width, region.height, fmt->format, fmt->type, ptr);
    } else if (dst->array) {
        glTexSubImage3D(
            dst->target, dst_level, region.x, region.y, dst_layer,
            region.width, region.height, 1, fmt->format, fmt->type, ptr
        );
    } else {
        glTexSubImage2D(
            dst->target, dst_level, region.x, region.y, region.width, region.height, fmt->format, fmt->type, ptr
        );
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    Py_RETURN_NONE;
}

static PyObject * Context_meth_copy_texture_to_buffer(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"src", "dst", "size", "src_offset", "dst_offset", NULL};

    Texture * src = NULL;
    Buffer * dst = NULL;
    PyObject * size_arg = Py_None;
    PyObject * src_offset_arg = Py_None;
    int32_t dst_offset = 0;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "O!O!|O$Oi", keywords,
        self->module_state->Texture_type, &src, self->module_state->Buffer_type, &dst,
        &size_arg, &src_offset_arg, &dst_offset
    );

    if (!args_ok) {
        return NULL;
    }

    if (dst->target == GL_ELEMENT_ARRAY_BUFFER) {
        PyErr_Format(PyExc_TypeError, "textures cannot be copied to index buffers");
        return NULL;
    }

    if (src->multisampled) {
        PyErr_Format(PyExc_TypeError, "cannot copy multisampled textures to buffers");
        return NULL;
    }

    if (!src->fmt->color) {
        PyErr_Format(PyExc_TypeError, "cannot copy depth or stencil textures to buffers");
        return NULL;
    }

    Region region = {0};
    if (!parse_region(src, 0, size_arg, src_offset_arg, &region)) {
        return NULL;
    }

    int32_t size = region_bytes(src, &region);

    if (dst_offset < 0 || dst_offset + size > dst->size) {
        PyErr_Format(PyExc_ValueError, "invalid dst_offset, %d bytes required", size);
        return NULL;
    }

    uint32_t framebuffer = texture_framebuffer(src);
    if (!framebuffer) {
        return NULL;
    }

    const ImageFormat * fmt = src->fmt;
    void * ptr = (void *)(intptr_t)dst_offset;

    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, dst->buffer);
    glReadPixels(region.x, region.y, region.width, region.height, fmt->format, fmt->type, ptr);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    Py_RETURN_NONE;
}

static PyObject * Context_meth_blend(Context * self, PyObject * args, PyObject * kwargs) {
    BlendState state = {0};
    if (!parse_blend(args, kwargs, &state)) {
        return NULL;
    }
    apply_blend(&state);
    Py_RETURN_NONE;
}

static PyObject * Context_meth_stencil(Context * self, PyObject * args, PyObject * kwargs) {
    StencilState state = {0};
    if (!parse_stencil(args, kwargs, &state)) {
        return NULL;
    }
    apply_stencil(self, &state);
    Py_RETURN_NONE;
}

static PyObject * Context_meth_reset_state(Context * self, PyObject * args) {
    self->depth_test = 0;
    self->depth_write = 1;
    self->depth_func = GL_LESS;
    self->cull_face = 0;
    self->stencil_write_mask = 0xFF;

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glDisable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilMask(0xFFFFFFFF);
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(0.0f, 0.0f);
    self->scissor_test = 0;
    self->scissor.width = -1;
    memset(&self->polygon_offset, 0, sizeof(self->polygon_offset));

    Py_CLEAR(self->bound_program);
    Py_CLEAR(self->bound_vertex_array);
    Py_XSETREF(self->bound_framebuffer, (Framebuffer *)Context_get_screen(self, NULL));

    glUseProgram(0);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    self->viewport = self->bound_framebuffer->viewport;
    glViewport(self->viewport.x, self->viewport.y, self->viewport.width, self->viewport.height);
    apply_scissor(self, self->bound_framebuffer->has_scissor, &self->bound_framebuffer->scissor);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    for (int32_t i = 0; i < self->max_uniform_buffers; ++i) {
        glBindBufferBase(GL_UNIFORM_BUFFER, i, 0);
    }

    for (int32_t i = 0; i < self->max_samplers; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glBindSampler(i, 0);
    }

    for (int32_t i = 0; i < self->max_storage_images; ++i) {
        glBindImageTexture(i, 0, 0, 0, 0, GL_READ_ONLY, GL_R8);
    }

    glActiveTexture(GL_TEXTURE0);
    Py_RETURN_NONE;
}

static PyObject * Context_meth_start_frame(Context * self, PyObject * args) {
    return Context_meth_reset_state(self, args);
}

static PyObject * Context_meth_end_frame(Context * self, PyObject * args) {
    glFlush();
    Py_RETURN_NONE;
}

static PyObject * Context_meth_present(Context * self, PyObject * arg) {
    PyObject * screen = Context_get_screen(self, NULL);
    PyObject * args = PyTuple_Pack(2, arg, screen);
    PyObject * res = Context_meth_copy_texture(self, args, NULL);
    Py_DECREF(args);
    Py_DECREF(screen);
    if (!res) {
        return NULL;
    }
    Py_DECREF(res);
    return Context_meth_end_frame(self, NULL);
}

static PyObject * release_object(Context * self, PyObject * arg) {
    ModuleState * module_state = self->module_state;

    if (PyUnicode_CheckExact(arg) && PyUnicode_EqualToUTF8(arg, "shader_cache")) {
        PyObject * key = NULL;
        PyObject * value = NULL;
        Py_ssize_t pos = 0;
        while (PyDict_Next(self->shader_cache, &pos, &key, &value)) {
            glDeleteShader((uint32_t)PyLong_AsUnsignedLong(value));
        }
        PyDict_Clear(self->shader_cache);
        Py_RETURN_NONE;
    }

    if (PyUnicode_CheckExact(arg) && PyUnicode_EqualToUTF8(arg, "all")) {
        while (self->gc_next != (GCHeader *)self) {
            Py_DECREF(release_object(self, (PyObject *)self->gc_next));
        }
        PyObject * key = NULL;
        PyObject * value = NULL;
        Py_ssize_t pos = 0;
        while (PyDict_Next(self->shader_cache, &pos, &key, &value)) {
            glDeleteShader((uint32_t)PyLong_AsUnsignedLong(value));
        }
        PyDict_Clear(self->shader_cache);
        Py_RETURN_NONE;
    }

    PyTypeObject * type = Py_TYPE(arg);

    if (type == module_state->Buffer_type) {
        Buffer * buffer = (Buffer *)arg;
        if (untrack_object(self, (GCHeader *)buffer)) {
            if (!buffer->external) {
                glDeleteBuffers(1, &buffer->buffer);
            }
            buffer->buffer = 0;
            Py_DECREF(buffer);
        }
        Py_RETURN_NONE;
    }

    if (type == module_state->Texture_type) {
        Texture * texture = (Texture *)arg;
        if (texture->framebuffer) {
            glDeleteFramebuffers(1, &texture->framebuffer);
            texture->framebuffer = 0;
        }
        if (!untrack_object(self, (GCHeader *)texture)) {
            Py_RETURN_NONE;
        }
        if (!texture->external && texture->multisampled) {
            glDeleteRenderbuffers(1, &texture->texture);
        }
        if (!texture->external && !texture->multisampled) {
            glDeleteTextures(1, &texture->texture);
        }
        texture->texture = 0;
        Py_DECREF(texture);
        Py_RETURN_NONE;
    }

    if (type == module_state->Framebuffer_type) {
        Framebuffer * framebuffer = (Framebuffer *)arg;
        if (untrack_object(self, (GCHeader *)framebuffer)) {
            if (self->bound_framebuffer == framebuffer) {
                Py_XSETREF(self->bound_framebuffer, (Framebuffer *)Context_get_screen(self, NULL));
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                apply_viewport(self, &self->bound_framebuffer->viewport);
                apply_scissor(self, self->bound_framebuffer->has_scissor, &self->bound_framebuffer->scissor);
            }
            if (!framebuffer->external) {
                glDeleteFramebuffers(1, &framebuffer->framebuffer);
            }
            framebuffer->framebuffer = 0;
            Py_DECREF(framebuffer);
        }
        Py_RETURN_NONE;
    }

    if (type == module_state->VertexArray_type) {
        VertexArray * vertex_array = (VertexArray *)arg;
        if (untrack_object(self, (GCHeader *)vertex_array)) {
            if (self->bound_vertex_array == vertex_array) {
                Py_CLEAR(self->bound_vertex_array);
                glBindVertexArray(0);
            }
            glDeleteVertexArrays(1, &vertex_array->vertex_array);
            vertex_array->vertex_array = 0;
            Py_DECREF(vertex_array);
        }
        Py_RETURN_NONE;
    }

    if (type == module_state->Sampler_type) {
        Sampler * sampler = (Sampler *)arg;
        if (untrack_object(self, (GCHeader *)sampler)) {
            glDeleteSamplers(1, &sampler->sampler);
            sampler->sampler = 0;
            Py_DECREF(sampler);
        }
        Py_RETURN_NONE;
    }

    if (type == module_state->Program_type) {
        Program * program = (Program *)arg;
        if (untrack_object(self, (GCHeader *)program)) {
            if (self->bound_program == program) {
                Py_CLEAR(self->bound_program);
                glUseProgram(0);
            }
            glDeleteProgram(program->program);
            program->program = 0;
            Py_DECREF(program);
        }
        Py_RETURN_NONE;
    }

    if (type == module_state->Pipeline_type) {
        Pipeline * pipeline = (Pipeline *)arg;
        if (untrack_object(self, (GCHeader *)pipeline)) {
            release_pipeline_program(self, pipeline);
            Py_DECREF(pipeline);
        }
        Py_RETURN_NONE;
    }

    PyErr_Format(PyExc_TypeError, "cannot release %R", arg);
    return NULL;
}

static PyObject * Context_meth_source(Context * self, PyObject * arg) {
    if (!PyUnicode_Check(arg)) {
        PyErr_Format(PyExc_TypeError, "the shader name must be a str");
        return NULL;
    }
    return load_shader(self->shaders, arg, 0);
}

static PyObject * Context_meth_gc(Context * self, PyObject * args) {
    PyObject * res = PyList_New(0);
    Py_BEGIN_CRITICAL_SECTION(self);
    for (GCHeader * it = self->gc_next; it != (GCHeader *)self; it = it->gc_next) {
        PyList_Append(res, (PyObject *)it);
    }
    Py_END_CRITICAL_SECTION();
    return res;
}

static PyObject * release_context(Context * self) {
    ModuleState * module_state = self->module_state;
    PyObject * all = PyUnicode_FromString("all");
    Py_DECREF(release_object(self, all));
    Py_DECREF(all);
    glDeleteVertexArrays(1, &self->empty_vertex_array->vertex_array);
    self->empty_vertex_array->vertex_array = 0;
    Py_CLEAR(self->bound_program);
    Py_CLEAR(self->bound_vertex_array);
    Py_CLEAR(self->bound_framebuffer);
    Py_CLEAR(module_state->screen);
    if (module_state->context == (PyObject *)self) {
        Py_CLEAR(module_state->context);
    }
    Py_RETURN_NONE;
}

static PyObject * Context_meth_release(Context * self, PyObject * args) {
    PyObject * arg = NULL;
    if (!PyArg_ParseTuple(args, "|O", &arg)) {
        return NULL;
    }
    if (!arg) {
        return release_context(self);
    }
    return release_object(self, arg);
}

static PyObject * Context_get_screen(Context * self, void * closure) {
    ModuleState * module_state = self->module_state;

    if (module_state->screen) {
        return Py_NewRef(module_state->screen);
    }

    int32_t viewport[4] = {0};
    glGetIntegerv(GL_VIEWPORT, viewport);

    PyObject * formats = Py_BuildValue("(ss)", "rgba8unorm", "depth24plus-stencil8");
    Framebuffer * res = create_external_framebuffer(
        self, 0, least_one(viewport[2]), least_one(viewport[3]), formats, 0
    );
    Py_DECREF(formats);

    if (!res) {
        return NULL;
    }

    module_state->screen = Py_NewRef(res);
    return (PyObject *)res;
}

static PyObject * Context_get_bound_program(Context * self, void * closure) {
    return Py_NewRef(self->bound_program ? (PyObject *)self->bound_program : Py_None);
}

static PyObject * Context_get_bound_framebuffer(Context * self, void * closure) {
    return Py_NewRef(self->bound_framebuffer ? (PyObject *)self->bound_framebuffer : Py_None);
}

static PyObject * Context_get_bound_vertex_array(Context * self, void * closure) {
    return Py_NewRef(self->bound_vertex_array ? (PyObject *)self->bound_vertex_array : Py_None);
}

static PyObject * Context_get_depth_test(Context * self, void * closure) {
    return PyBool_FromLong(self->depth_test);
}

static int Context_set_depth_test(Context * self, PyObject * value, void * closure) {
    int flag = PyObject_IsTrue(value);
    if (flag < 0) {
        return -1;
    }
    self->depth_test = (int8_t)flag;
    if (flag) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    return 0;
}

static PyObject * Context_get_depth_write(Context * self, void * closure) {
    return PyBool_FromLong(self->depth_write);
}

static int Context_set_depth_write(Context * self, PyObject * value, void * closure) {
    int flag = PyObject_IsTrue(value);
    if (flag < 0) {
        return -1;
    }
    self->depth_write = (int8_t)flag;
    glDepthMask(flag ? GL_TRUE : GL_FALSE);
    return 0;
}

static PyObject * Context_get_depth_func(Context * self, void * closure) {
    return PyUnicode_FromString(get_value_name(compare_funcs, self->depth_func));
}

static int Context_set_depth_func(Context * self, PyObject * value, void * closure) {
    uint32_t depth_func = 0;
    if (!parse_named_value(compare_funcs, value, &depth_func, "depth_func")) {
        return -1;
    }
    self->depth_func = depth_func;
    glDepthFunc(depth_func);
    return 0;
}

static PyObject * Context_get_cull_face(Context * self, void * closure) {
    if (!self->cull_face) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(get_value_name(cull_faces, self->cull_face));
}

static int Context_set_cull_face(Context * self, PyObject * value, void * closure) {
    if (value == Py_None) {
        self->cull_face = 0;
        glDisable(GL_CULL_FACE);
        return 0;
    }
    uint32_t cull_face = 0;
    if (!parse_named_value(cull_faces, value, &cull_face, "cull_face")) {
        return -1;
    }
    self->cull_face = cull_face;
    glEnable(GL_CULL_FACE);
    glCullFace(cull_face);
    return 0;
}

static PyObject * Context_get_polygon_offset(Context * self, void * closure) {
    return polygon_offset_value(&self->polygon_offset);
}

static int Context_set_polygon_offset(Context * self, PyObject * value, void * closure) {
    PolygonOffset state = {0};
    if (!parse_polygon_offset(value, &state)) {
        return -1;
    }
    apply_polygon_offset(self, &state);
    return 0;
}

enum {
    DEBUG_SCISSOR_BOX = 0x0C10,
    DEBUG_DEPTH_WRITEMASK = 0x0B72,
    DEBUG_DEPTH_FUNC = 0x0B74,
    DEBUG_CULL_FACE_MODE = 0x0B45,
    DEBUG_POLYGON_OFFSET_FACTOR = 0x8038,
    DEBUG_POLYGON_OFFSET_UNITS = 0x2A00,
    DEBUG_BLEND_DST_RGB = 0x80C8,
    DEBUG_BLEND_SRC_RGB = 0x80C9,
    DEBUG_BLEND_DST_ALPHA = 0x80CA,
    DEBUG_BLEND_SRC_ALPHA = 0x80CB,
    DEBUG_BLEND_EQUATION_RGB = 0x8009,
    DEBUG_BLEND_EQUATION_ALPHA = 0x883D,
    DEBUG_STENCIL_FUNC = 0x0B92,
    DEBUG_STENCIL_VALUE_MASK = 0x0B93,
    DEBUG_STENCIL_FAIL = 0x0B94,
    DEBUG_STENCIL_PASS_DEPTH_FAIL = 0x0B95,
    DEBUG_STENCIL_PASS_DEPTH_PASS = 0x0B96,
    DEBUG_STENCIL_REF = 0x0B97,
    DEBUG_STENCIL_WRITEMASK = 0x0B98,
    DEBUG_STENCIL_BACK_FUNC = 0x8800,
    DEBUG_STENCIL_BACK_FAIL = 0x8801,
    DEBUG_STENCIL_BACK_PASS_DEPTH_FAIL = 0x8802,
    DEBUG_STENCIL_BACK_PASS_DEPTH_PASS = 0x8803,
    DEBUG_CURRENT_PROGRAM = 0x8B8D,
    DEBUG_VERTEX_ARRAY_BINDING = 0x85B5,
    DEBUG_DRAW_FRAMEBUFFER_BINDING = 0x8CA6,
    DEBUG_ACTIVE_TEXTURE = 0x84E0,
    DEBUG_TEXTURE_BINDING_2D = 0x8069,
    DEBUG_TEXTURE_BINDING_2D_ARRAY = 0x8C1D,
    DEBUG_TEXTURE_BINDING_CUBE_MAP = 0x8514,
    DEBUG_SAMPLER_BINDING = 0x8919,
    DEBUG_UNIFORM_BUFFER_BINDING = 0x8A28,
    DEBUG_UNIFORM_BUFFER_START = 0x8A29,
    DEBUG_UNIFORM_BUFFER_SIZE = 0x8A2A,
};

static void debug_set(PyObject * dict, const char * key, PyObject * value) {
    PyDict_SetItemString(dict, key, value);
    Py_DECREF(value);
}

static int32_t debug_int(uint32_t pname) {
    int32_t value = 0;
    glGetIntegerv(pname, &value);
    return value;
}

static PyObject * debug_name(const NamedValue * table, uint32_t pname) {
    const char * name = get_value_name(table, (uint32_t)debug_int(pname));
    return name ? PyUnicode_FromString(name) : Py_NewRef(Py_None);
}

static uint32_t debug_glo(Context * ctx, PyObject * obj) {
    ModuleState * state = ctx->module_state;
    PyTypeObject * type = Py_TYPE(obj);
    if (type == state->Program_type) {
        return ((Program *)obj)->program;
    }
    if (type == state->VertexArray_type) {
        return ((VertexArray *)obj)->vertex_array;
    }
    if (type == state->Framebuffer_type) {
        return ((Framebuffer *)obj)->framebuffer;
    }
    if (type == state->Buffer_type) {
        return ((Buffer *)obj)->buffer;
    }
    if (type == state->Sampler_type) {
        return ((Sampler *)obj)->sampler;
    }
    if (type == state->Texture_type && !((Texture *)obj)->multisampled) {
        return ((Texture *)obj)->texture;
    }
    return 0;
}

static PyObject * debug_find(Context * ctx, PyTypeObject * type, int32_t glo) {
    if (!glo) {
        Py_RETURN_NONE;
    }
    VertexArray * empty = ctx->empty_vertex_array;
    if (type == ctx->module_state->VertexArray_type && empty && empty->vertex_array == (uint32_t)glo) {
        return Py_NewRef(empty);
    }
    PyObject * res = NULL;
    Py_BEGIN_CRITICAL_SECTION(ctx);
    for (GCHeader * it = ctx->gc_next; it != (GCHeader *)ctx; it = it->gc_next) {
        if (Py_TYPE(it) == type && debug_glo(ctx, (PyObject *)it) == (uint32_t)glo) {
            res = Py_NewRef(it);
            break;
        }
    }
    Py_END_CRITICAL_SECTION();
    return res ? res : Py_NewRef(Py_None);
}

static PyObject * debug_uniform_value(uint32_t program, uint32_t gltype, int32_t location) {
    int32_t function = 0;
    int32_t items = 0;
    char type = 0;
    if (!get_uniform_type(gltype, &function, &items, &type)) {
        Py_RETURN_NONE;
    }
    int32_t raw[16] = {0};
    if (type == 'f') {
        glGetUniformfv(program, location, (float *)raw);
    } else if (type == 'i') {
        glGetUniformiv(program, location, raw);
    } else {
        glGetUniformuiv(program, location, (uint32_t *)raw);
    }
    int is_bool = gltype == GL_BOOL || gltype == GL_BOOL_VEC2 || gltype == GL_BOOL_VEC3 || gltype == GL_BOOL_VEC4;
    PyObject * values = PyTuple_New(items);
    for (int32_t i = 0; i < items; ++i) {
        PyObject * item = NULL;
        if (type == 'f') {
            item = PyFloat_FromDouble(((float *)raw)[i]);
        } else if (is_bool) {
            item = PyBool_FromLong(raw[i]);
        } else if (type == 'i') {
            item = PyLong_FromLong(raw[i]);
        } else {
            item = PyLong_FromUnsignedLong(((uint32_t *)raw)[i]);
        }
        PyTuple_SetItem(values, i, item);
    }
    if (items == 1) {
        PyObject * res = Py_NewRef(PyTuple_GetItem(values, 0));
        Py_DECREF(values);
        return res;
    }
    if (function < 12) {
        return values;
    }
    static const int32_t columns[] = {2, 2, 2, 3, 3, 3, 4, 4, 4};
    int32_t cols = columns[function - 12];
    int32_t rows = items / cols;
    PyObject * res = PyTuple_New(cols);
    for (int32_t i = 0; i < cols; ++i) {
        PyTuple_SetItem(res, i, PyTuple_GetSlice(values, i * rows, i * rows + rows));
    }
    Py_DECREF(values);
    return res;
}

static void debug_units(uint32_t program, PyObject * target, const char * name, int32_t size, int32_t array) {
    for (int32_t i = 0; i < size; ++i) {
        char element[300] = {0};
        snprintf(element, sizeof(element), array ? "%s[%d]" : "%s", name, i);
        int32_t location = glGetUniformLocation(program, element);
        if (location < 0) {
            continue;
        }
        int32_t unit = 0;
        glGetUniformiv(program, location, &unit);
        PyObject * value = PyLong_FromLong(unit);
        PyDict_SetItemString(target, element, value);
        Py_DECREF(value);
    }
}

static void debug_inactive(PyObject * declared, PyObject * active, PyObject * inactive) {
    PyObject * key = NULL;
    PyObject * value = NULL;
    Py_ssize_t pos = 0;
    while (declared && PyDict_Next(declared, &pos, &key, &value)) {
        if (!PyDict_Contains(active, key)) {
            PyList_Append(inactive, key);
        }
    }
}

static void debug_program(Context * self, PyObject * res, uint32_t program_glo) {
    PyObject * found = debug_find(self, self->module_state->Program_type, (int32_t)program_glo);
    Program * program = found != Py_None ? (Program *)found : NULL;
    PyObject * uniforms = PyDict_New();
    PyObject * samplers = PyDict_New();
    PyObject * uniform_buffers = PyDict_New();
    PyObject * storage_buffers = PyDict_New();
    PyObject * storage_images = PyDict_New();
    PyObject * inactive = PyList_New(0);

    int32_t active_uniforms = 0;
    int32_t active_blocks = 0;
    if (program_glo) {
        glGetProgramiv(program_glo, GL_ACTIVE_UNIFORMS, &active_uniforms);
        glGetProgramiv(program_glo, GL_ACTIVE_UNIFORM_BLOCKS, &active_blocks);
    }

    for (int32_t i = 0; i < active_uniforms; ++i) {
        char name[256] = {0};
        int32_t length = 0;
        int32_t size = 0;
        uint32_t gltype = 0;
        glGetActiveUniform(program_glo, i, 256, &length, &size, &gltype, name);
        int32_t array = length > 3 && !strcmp(name + length - 3, "[0]");
        if (array) {
            name[length - 3] = 0;
        }
        int32_t location = glGetUniformLocation(program_glo, name);
        if (location < 0) {
            continue;
        }
        if (gltype >= 0x904C && gltype <= 0x906C) {
            debug_units(program_glo, storage_images, name, size, array);
            continue;
        }
        if (get_value_name(sampler_types, gltype)) {
            debug_units(program_glo, samplers, name, size, array);
            continue;
        }
        if (!array) {
            debug_set(uniforms, name, debug_uniform_value(program_glo, gltype, location));
            continue;
        }
        PyObject * list = PyList_New(0);
        for (int32_t e = 0; e < size; ++e) {
            char element[300] = {0};
            snprintf(element, sizeof(element), "%s[%d]", name, e);
            int32_t element_location = glGetUniformLocation(program_glo, element);
            if (element_location < 0) {
                break;
            }
            PyObject * value = debug_uniform_value(program_glo, gltype, element_location);
            PyList_Append(list, value);
            Py_DECREF(value);
        }
        debug_set(uniforms, name, list);
    }

    for (int32_t i = 0; i < active_blocks; ++i) {
        char name[256] = {0};
        int32_t binding = 0;
        glGetActiveUniformBlockName(program_glo, i, 256, NULL, name);
        glGetActiveUniformBlockiv(program_glo, i, GL_UNIFORM_BLOCK_BINDING, &binding);
        debug_set(uniform_buffers, name, PyLong_FromLong(binding));
    }

    if (program) {
        PyObject * key = NULL;
        PyObject * value = NULL;
        Py_ssize_t pos = 0;
        while (PyDict_Next(program->storage_buffers, &pos, &key, &value)) {
            const char * name = PyUnicode_Check(key) ? PyUnicode_AsUTF8(key) : NULL;
            if (!name || !glGetProgramResourceIndex) {
                continue;
            }
            if (glGetProgramResourceIndex(program_glo, GL_SHADER_STORAGE_BLOCK, name) != GL_INVALID_INDEX) {
                PyDict_SetItem(storage_buffers, key, value);
            }
        }
        debug_inactive(program->uniform_index, uniforms, inactive);
        debug_inactive(program->samplers, samplers, inactive);
        debug_inactive(program->uniform_buffers, uniform_buffers, inactive);
        debug_inactive(program->storage_buffers, storage_buffers, inactive);
        debug_inactive(program->storage_images, storage_images, inactive);
    }

    debug_set(res, "program", found);
    debug_set(res, "uniforms", uniforms);
    debug_set(res, "samplers", samplers);
    debug_set(res, "uniform_buffers", uniform_buffers);
    debug_set(res, "storage_buffers", storage_buffers);
    debug_set(res, "storage_images", storage_images);
    debug_set(res, "inactive", inactive);
}

static void debug_state(Context * self, PyObject * res) {
    int32_t box[4] = {0};
    glGetIntegerv(GL_VIEWPORT, box);
    debug_set(res, "viewport", Py_BuildValue("(iiii)", box[0], box[1], box[2], box[3]));

    if (glIsEnabled(GL_SCISSOR_TEST)) {
        glGetIntegerv(DEBUG_SCISSOR_BOX, box);
        debug_set(res, "scissor", Py_BuildValue("(iiii)", box[0], box[1], box[2], box[3]));
    } else {
        debug_set(res, "scissor", Py_NewRef(Py_None));
    }

    debug_set(res, "depth_test", PyBool_FromLong(glIsEnabled(GL_DEPTH_TEST)));
    debug_set(res, "depth_write", PyBool_FromLong(debug_int(DEBUG_DEPTH_WRITEMASK)));
    debug_set(res, "depth_func", debug_name(compare_funcs, DEBUG_DEPTH_FUNC));
    PyObject * cull = glIsEnabled(GL_CULL_FACE) ? debug_name(cull_faces, DEBUG_CULL_FACE_MODE) : Py_NewRef(Py_None);
    debug_set(res, "cull_face", cull);

    if (glIsEnabled(GL_POLYGON_OFFSET_FILL)) {
        float factor = 0.0f;
        float units = 0.0f;
        glGetFloatv(DEBUG_POLYGON_OFFSET_FACTOR, &factor);
        glGetFloatv(DEBUG_POLYGON_OFFSET_UNITS, &units);
        debug_set(res, "polygon_offset", Py_BuildValue("(ff)", factor, units));
    } else {
        debug_set(res, "polygon_offset", Py_NewRef(Py_None));
    }

    if (glIsEnabled(GL_BLEND)) {
        debug_set(res, "blend", Py_BuildValue(
            "{sNsNsNsNsNsN}",
            "src", debug_name(blend_factors, DEBUG_BLEND_SRC_RGB),
            "dst", debug_name(blend_factors, DEBUG_BLEND_DST_RGB),
            "equation", debug_name(blend_equations, DEBUG_BLEND_EQUATION_RGB),
            "src_alpha", debug_name(blend_factors, DEBUG_BLEND_SRC_ALPHA),
            "dst_alpha", debug_name(blend_factors, DEBUG_BLEND_DST_ALPHA),
            "equation_alpha", debug_name(blend_equations, DEBUG_BLEND_EQUATION_ALPHA)
        ));
    } else {
        debug_set(res, "blend", Py_NewRef(Py_None));
    }

    if (glIsEnabled(GL_STENCIL_TEST)) {
        debug_set(res, "stencil", Py_BuildValue(
            "{sNsisIsIsNsNsNsNsNsNsN}",
            "func", debug_name(compare_funcs, DEBUG_STENCIL_FUNC),
            "ref", debug_int(DEBUG_STENCIL_REF),
            "read_mask", (uint32_t)debug_int(DEBUG_STENCIL_VALUE_MASK),
            "write_mask", (uint32_t)debug_int(DEBUG_STENCIL_WRITEMASK),
            "fail", debug_name(stencil_ops, DEBUG_STENCIL_FAIL),
            "depth_fail", debug_name(stencil_ops, DEBUG_STENCIL_PASS_DEPTH_FAIL),
            "depth_pass", debug_name(stencil_ops, DEBUG_STENCIL_PASS_DEPTH_PASS),
            "back_func", debug_name(compare_funcs, DEBUG_STENCIL_BACK_FUNC),
            "back_fail", debug_name(stencil_ops, DEBUG_STENCIL_BACK_FAIL),
            "back_depth_fail", debug_name(stencil_ops, DEBUG_STENCIL_BACK_PASS_DEPTH_FAIL),
            "back_depth_pass", debug_name(stencil_ops, DEBUG_STENCIL_BACK_PASS_DEPTH_PASS)
        ));
    } else {
        debug_set(res, "stencil", Py_NewRef(Py_None));
    }
}

static PyObject * debug_units_bound(Context * self) {
    ModuleState * state = self->module_state;
    PyObject * samplers = PyDict_New();
    PyObject * uniform_buffers = PyDict_New();
    int32_t active_texture = debug_int(DEBUG_ACTIVE_TEXTURE);

    for (int32_t i = 0; i < self->max_samplers; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        int32_t texture = debug_int(DEBUG_TEXTURE_BINDING_2D);
        if (!texture) {
            texture = debug_int(DEBUG_TEXTURE_BINDING_2D_ARRAY);
        }
        if (!texture) {
            texture = debug_int(DEBUG_TEXTURE_BINDING_CUBE_MAP);
        }
        int32_t sampler = debug_int(DEBUG_SAMPLER_BINDING);
        if (!texture && !sampler) {
            continue;
        }
        PyObject * value = debug_find(self, state->Sampler_type, sampler);
        if (value != Py_None && ((Sampler *)value)->texture->texture != (uint32_t)texture) {
            Py_SETREF(value, Py_NewRef(Py_None));
        }
        if (value == Py_None) {
            Py_SETREF(value, debug_find(self, state->Texture_type, texture));
        }
        PyObject * unit = PyLong_FromLong(i);
        PyDict_SetItem(samplers, unit, value);
        Py_DECREF(unit);
        Py_DECREF(value);
    }

    glActiveTexture((uint32_t)active_texture);

    for (int32_t i = 0; i < self->max_uniform_buffers; ++i) {
        int32_t buffer = 0;
        int32_t offset = 0;
        int32_t size = 0;
        glGetIntegeri_v(DEBUG_UNIFORM_BUFFER_BINDING, i, &buffer);
        if (!buffer) {
            continue;
        }
        glGetIntegeri_v(DEBUG_UNIFORM_BUFFER_START, i, &offset);
        glGetIntegeri_v(DEBUG_UNIFORM_BUFFER_SIZE, i, &size);
        PyObject * found = debug_find(self, state->Buffer_type, buffer);
        PyObject * value = found;
        if (found != Py_None) {
            value = Py_BuildValue("(Oii)", found, offset, size ? size : ((Buffer *)found)->size);
            Py_DECREF(found);
        }
        PyObject * binding = PyLong_FromLong(i);
        PyDict_SetItem(uniform_buffers, binding, value);
        Py_DECREF(binding);
        Py_DECREF(value);
    }

    return Py_BuildValue("{sNsN}", "samplers", samplers, "uniform_buffers", uniform_buffers);
}

static PyObject * Context_meth_debug(Context * self, PyObject * args) {
    ModuleState * state = self->module_state;
    PyObject * res = PyDict_New();
    uint32_t program = (uint32_t)debug_int(DEBUG_CURRENT_PROGRAM);
    debug_set(res, "program", Py_NewRef(Py_None));
    debug_set(res, "vertex_array", debug_find(self, state->VertexArray_type, debug_int(DEBUG_VERTEX_ARRAY_BINDING)));
    debug_set(res, "framebuffer", debug_find(self, state->Framebuffer_type, debug_int(DEBUG_DRAW_FRAMEBUFFER_BINDING)));
    debug_state(self, res);
    debug_program(self, res, program);
    debug_set(res, "units", debug_units_bound(self));
    return res;
}

static Context * create_context(PyObject * self) {
    ModuleState * module_state = (ModuleState *)PyModule_GetState(self);
    Context * res = NULL;

    PyMutex_Lock(&gl_mutex);
    int ok = gl_loaded || load_gl(Py_None);
    PyMutex_Unlock(&gl_mutex);

    if (!ok) {
        return NULL;
    }

    Py_BEGIN_CRITICAL_SECTION(self);
    if (module_state->context) {
        PyErr_Format(PyExc_RuntimeError, "the context is already created");
    } else {
        res = PyObject_New(Context, module_state->Context_type);
        res->gc_prev = (GCHeader *)res;
        res->gc_next = (GCHeader *)res;
        res->module_state = module_state;
        res->max_vertex_attribs = get_limit(GL_MAX_VERTEX_ATTRIBS, 8, MAX_ATTRIBUTES);
        res->shaders = PyDict_New();
        res->shader_cache = PyDict_New();
        res->empty_vertex_array = NULL;
        glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        int32_t max_texture_units = get_limit(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 2, 1024);
        res->default_texture_unit = GL_TEXTURE0 + max_texture_units - 1;
        res->max_samplers = max_texture_units - 1 < MAX_SAMPLERS ? max_texture_units - 1 : MAX_SAMPLERS;
        res->max_uniform_buffers = get_limit(GL_MAX_UNIFORM_BUFFER_BINDINGS, 1, MAX_BUFFERS);
        res->max_storage_buffers = 0;
        if (glDispatchCompute) {
            res->max_storage_buffers = get_limit(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, 0, MAX_BUFFERS);
        }
        res->max_storage_images = 0;
        if (glBindImageTexture) {
            res->max_storage_images = get_limit(GL_MAX_IMAGE_UNITS, 0, MAX_BUFFERS);
        }
        res->uniform_buffer_offset_alignment = 1;
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &res->uniform_buffer_offset_alignment);
        res->storage_buffer_offset_alignment = 1;
        if (glDispatchCompute) {
            glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &res->storage_buffer_offset_alignment);
        }
        res->info = Py_BuildValue(
            "{szszszszszsisisisisisisisOsOsOsO}",
            "backend", gl_backend,
            "vendor", (const char *)glGetString(GL_VENDOR),
            "renderer", (const char *)glGetString(GL_RENDERER),
            "version", (const char *)glGetString(GL_VERSION),
            "glsl", (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION),
            "max_vertex_attribs", res->max_vertex_attribs,
            "max_samplers", res->max_samplers,
            "max_uniform_buffers", res->max_uniform_buffers,
            "max_uniform_block_size", get_limit(GL_MAX_UNIFORM_BLOCK_SIZE, 0x4000, 0x40000000),
            "uniform_buffer_offset_alignment", res->uniform_buffer_offset_alignment,
            "max_storage_buffers", res->max_storage_buffers,
            "max_storage_images", res->max_storage_images,
            "compute", glDispatchCompute ? Py_True : Py_False,
            "storage_images", glBindImageTexture ? Py_True : Py_False,
            "first_instance", glDrawArraysInstancedBaseInstance ? Py_True : Py_False,
            "draw_indirect", glMultiDrawArraysIndirect ? Py_True : Py_False
        );
        res->depth_test = 0;
        res->depth_write = 1;
        res->depth_func = GL_LESS;
        res->cull_face = 0;
        res->stencil_write_mask = 0xFF;
        glDisable(GL_SCISSOR_TEST);
        glDisable(GL_POLYGON_OFFSET_FILL);
        res->scissor_test = 0;
        memset(&res->scissor, 0, sizeof(res->scissor));
        res->scissor.width = -1;
        memset(&res->polygon_offset, 0, sizeof(res->polygon_offset));
        res->bound_program = NULL;
        res->bound_framebuffer = NULL;
        res->bound_vertex_array = NULL;
        module_state->context = Py_NewRef(res);
    }
    Py_END_CRITICAL_SECTION();

    if (res) {
        PyObject * screen = Context_get_screen(res, NULL);
        if (!screen) {
            return NULL;
        }
        res->bound_framebuffer = (Framebuffer *)screen;
        res->viewport = res->bound_framebuffer->viewport;
        glViewport(res->viewport.x, res->viewport.y, res->viewport.width, res->viewport.height);
        PyObject * empty_args = Py_BuildValue("([])");
        res->empty_vertex_array = Context_meth_vertex_array(res, empty_args, NULL);
        Py_DECREF(empty_args);
        untrack_object(res, (GCHeader *)res->empty_vertex_array);
        Py_DECREF(res->empty_vertex_array);
    }

    return res;
}

static PyObject * meth_get_context(PyObject * self, PyObject * args) {
    ModuleState * module_state = (ModuleState *)PyModule_GetState(self);
    PyObject * res = NULL;

    Py_BEGIN_CRITICAL_SECTION(self);
    if (module_state->context) {
        res = Py_NewRef(module_state->context);
    }
    Py_END_CRITICAL_SECTION();

    if (res) {
        return res;
    }

    return (PyObject *)create_context(self);
}

static PyObject * meth_create_context(PyObject * self, PyObject * args) {
    if (PyErr_WarnEx(PyExc_DeprecationWarning, "create_context() is deprecated, use get_context()", 1) < 0) {
        return NULL;
    }
    return (PyObject *)create_context(self);
}

static PyObject * meth_headless_context(PyObject * self, PyObject * args) {
    ModuleState * module_state = (ModuleState *)PyModule_GetState(self);
    void * handles[4] = {0};

    PyMutex_Lock(&gl_mutex);
    int ok = headless_create(handles);
    PyMutex_Unlock(&gl_mutex);

    if (!ok) {
        return NULL;
    }

    HeadlessContext * res = PyObject_New(HeadlessContext, module_state->HeadlessContext_type);
    res->module_state = module_state;
    memcpy(res->handles, handles, sizeof(handles));
    return (PyObject *)res;
}

typedef struct vec3 {
    double x, y, z;
} vec3;

static vec3 sub(const vec3 a, const vec3 b) {
    vec3 res;
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    res.z = a.z - b.z;
    return res;
}

static vec3 normalize(const vec3 a) {
    const double l = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    vec3 res;
    res.x = a.x / l;
    res.y = a.y / l;
    res.z = a.z / l;
    return res;
}

static vec3 cross(const vec3 a, const vec3 b) {
    vec3 res;
    res.x = a.y * b.z - a.z * b.y;
    res.y = a.z * b.x - a.x * b.z;
    res.z = a.x * b.y - a.y * b.x;
    return res;
}

static double dot(const vec3 a, const vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static PyObject * meth_camera(PyObject * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"eye", "target", "up", "fov", "aspect", "near", "far", "size", "clip", NULL};

    vec3 eye;
    vec3 target = {0.0, 0.0, 0.0};
    vec3 up = {0.0, 0.0, 1.0};
    double fov = 60.0;
    double aspect = 1.0;
    double znear = 0.1;
    double zfar = 1000.0;
    double size = 1.0;
    int clip = 0;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "(ddd)|(ddd)(ddd)dddddp", keywords,
        &eye.x, &eye.y, &eye.z,
        &target.x, &target.y, &target.z,
        &up.x, &up.y, &up.z,
        &fov, &aspect, &znear, &zfar, &size, &clip
    );

    if (!args_ok) {
        return NULL;
    }

    const vec3 f = normalize(sub(target, eye));
    const vec3 s = normalize(cross(f, up));
    const vec3 u = cross(s, f);
    const vec3 t = {-dot(s, eye), -dot(u, eye), -dot(f, eye)};

    if (!fov) {
        const double r1 = size;
        const double r2 = r1 * aspect;
        const double r3 = clip ? 1.0 / (zfar - znear) : 2.0 / (zfar - znear);
        const double r4 = clip ? znear / (zfar - znear) : (zfar + znear) / (zfar - znear);

        float res[] = {
            (float)(s.x / r2), (float)(u.x / r1), (float)(r3 * f.x), 0.0f,
            (float)(s.y / r2), (float)(u.y / r1), (float)(r3 * f.y), 0.0f,
            (float)(s.z / r2), (float)(u.z / r1), (float)(r3 * f.z), 0.0f,
            (float)(t.x / r2), (float)(t.y / r1), (float)(r3 * t.z - r4), 1.0f,
        };

        return PyBytes_FromStringAndSize((char *)res, 64);
    }

    const double r1 = tan(fov * 0.008726646259971647884618453842);
    const double r2 = r1 * aspect;
    const double r3 = clip ? zfar / (zfar - znear) : (zfar + znear) / (zfar - znear);
    const double r4 = clip ? (zfar * znear) / (zfar - znear) : (2.0 * zfar * znear) / (zfar - znear);

    float res[] = {
        (float)(s.x / r2), (float)(u.x / r1), (float)(r3 * f.x), (float)f.x,
        (float)(s.y / r2), (float)(u.y / r1), (float)(r3 * f.y), (float)f.y,
        (float)(s.z / r2), (float)(u.z / r1), (float)(r3 * f.z), (float)f.z,
        (float)(t.x / r2), (float)(t.y / r1), (float)(r3 * t.z - r4), (float)t.z,
    };

    return PyBytes_FromStringAndSize((char *)res, 64);
}

static void Buffer_dealloc(Buffer * self) {
    PyTypeObject * type = Py_TYPE(self);
    Py_DECREF(self->ctx);
    PyObject_Del(self);
    Py_DECREF(type);
}

static void Texture_dealloc(Texture * self) {
    PyTypeObject * type = Py_TYPE(self);
    Py_DECREF(self->size);
    Py_DECREF(self->format);
    Py_DECREF(self->ctx);
    PyObject_Del(self);
    Py_DECREF(type);
}

static void Sampler_dealloc(Sampler * self) {
    PyTypeObject * type = Py_TYPE(self);
    Py_DECREF(self->texture);
    Py_DECREF(self->ctx);
    PyObject_Del(self);
    Py_DECREF(type);
}

static void Resources_dealloc(Resources * self) {
    PyTypeObject * type = Py_TYPE(self);
    for (int32_t i = 0; i < self->sampler_count; ++i) {
        Py_DECREF(self->samplers[i].sampler);
    }
    for (int32_t i = 0; i < self->uniform_buffer_count; ++i) {
        Py_DECREF(self->uniform_buffers[i].buffer);
    }
    for (int32_t i = 0; i < self->storage_buffer_count; ++i) {
        Py_DECREF(self->storage_buffers[i].buffer);
    }
    for (int32_t i = 0; i < self->storage_image_count; ++i) {
        Py_DECREF(self->storage_images[i].texture);
    }
    Py_DECREF(self->ctx);
    PyObject_Del(self);
    Py_DECREF(type);
}

static void VertexArray_dealloc(VertexArray * self) {
    PyTypeObject * type = Py_TYPE(self);
    Py_DECREF(self->attributes);
    Py_DECREF(self->index_buffer);
    Py_DECREF(self->ctx);
    PyObject_Del(self);
    Py_DECREF(type);
}

static void Program_dealloc(Program * self) {
    PyTypeObject * type = Py_TYPE(self);
    PyMem_Free(self->uniform_bindings);
    Py_DECREF(self->uniform_index);
    Py_DECREF(self->samplers);
    Py_DECREF(self->uniform_buffers);
    Py_DECREF(self->storage_buffers);
    Py_DECREF(self->storage_images);
    Py_DECREF(self->ctx);
    PyObject_Del(self);
    Py_DECREF(type);
}

static void Framebuffer_dealloc(Framebuffer * self) {
    PyTypeObject * type = Py_TYPE(self);
    Py_DECREF(self->attachments);
    Py_DECREF(self->size);
    Py_DECREF(self->ctx);
    PyObject_Del(self);
    Py_DECREF(type);
}

static void ReadRequest_dealloc(ReadRequest * self) {
    PyTypeObject * type = Py_TYPE(self);
    if (self->sync) {
        glDeleteBuffers(1, &self->buffer);
        glDeleteSync(self->sync);
    }
    Py_XDECREF(self->data);
    Py_DECREF(self->ctx);
    PyObject_Del(self);
    Py_DECREF(type);
}

static void Pipeline_dealloc(Pipeline * self) {
    PyTypeObject * type = Py_TYPE(self);
    PyMem_Free(self->uniform_data);
    PyMem_Free(self->uniform_offsets);
    Py_DECREF(self->program);
    Py_XDECREF(self->vertex_array);
    Py_XDECREF(self->framebuffer);
    Py_DECREF(self->resources);
    Py_DECREF(self->ctx);
    PyObject_Del(self);
    Py_DECREF(type);
}

static void Context_dealloc(Context * self) {
    PyTypeObject * type = Py_TYPE(self);
    Py_XDECREF(self->bound_program);
    Py_XDECREF(self->bound_framebuffer);
    Py_XDECREF(self->bound_vertex_array);
    Py_DECREF(self->shaders);
    Py_DECREF(self->info);
    Py_DECREF(self->shader_cache);
    Py_XDECREF(self->empty_vertex_array);
    PyObject_Del(self);
    Py_DECREF(type);
}

static void HeadlessContext_dealloc(HeadlessContext * self) {
    PyTypeObject * type = Py_TYPE(self);
    PyObject_Del(self);
    Py_DECREF(type);
}

typedef struct TrackedObject {
    PyObject_HEAD
    GCHeader * gc_prev;
    GCHeader * gc_next;
    Context * ctx;
} TrackedObject;

static PyObject * get_released(TrackedObject * self, void * closure) {
    PyObject * obj = (PyObject *)self;
    int owned_by_context = obj == (PyObject *)self->ctx->empty_vertex_array || obj == self->ctx->module_state->screen;
    return PyBool_FromLong(!self->gc_prev && !owned_by_context);
}

static PyMethodDef HeadlessContext_methods[] = {
    {"load_opengl_function", (PyCFunction)HeadlessContext_meth_load_opengl_function, METH_O, NULL},
    {"release", (PyCFunction)HeadlessContext_meth_release, METH_NOARGS, NULL},
    {0},
};

static PyMethodDef Buffer_methods[] = {
    {"write", (PyCFunction)Buffer_meth_write, METH_VARARGS | METH_KEYWORDS, NULL},
    {"read", (PyCFunction)Buffer_meth_read, METH_VARARGS | METH_KEYWORDS, NULL},
    {"bind", (PyCFunction)Buffer_meth_bind, METH_VARARGS | METH_KEYWORDS, NULL},
    {"use", (PyCFunction)Buffer_meth_use, METH_VARARGS | METH_KEYWORDS, NULL},
    {"view", (PyCFunction)Buffer_meth_view, METH_VARARGS | METH_KEYWORDS, NULL},
    {0},
};

static PyMemberDef Buffer_members[] = {
    {"index", Py_T_BOOL, offsetof(Buffer, index), Py_READONLY, NULL},
    {"size", Py_T_INT, offsetof(Buffer, size), Py_READONLY, NULL},
    {"glo", Py_T_UINT, offsetof(Buffer, buffer), Py_READONLY, NULL},
    {0},
};

static PyMethodDef Texture_methods[] = {
    {"write", (PyCFunction)Texture_meth_write, METH_VARARGS | METH_KEYWORDS, NULL},
    {"read", (PyCFunction)Texture_meth_read, METH_VARARGS | METH_KEYWORDS, NULL},
    {"attachment", (PyCFunction)Texture_meth_attachment, METH_VARARGS | METH_KEYWORDS, NULL},
    {"storage_image", (PyCFunction)Texture_meth_storage_image, METH_VARARGS | METH_KEYWORDS, NULL},
    {"mipmaps", (PyCFunction)Texture_meth_mipmaps, METH_NOARGS, NULL},
    {0},
};

static PyMemberDef Texture_members[] = {
    {"size", Py_T_OBJECT_EX, offsetof(Texture, size), Py_READONLY, NULL},
    {"format", Py_T_OBJECT_EX, offsetof(Texture, format), Py_READONLY, NULL},
    {"array", Py_T_INT, offsetof(Texture, array), Py_READONLY, NULL},
    {"cubemap", Py_T_BOOL, offsetof(Texture, cubemap), Py_READONLY, NULL},
    {"levels", Py_T_INT, offsetof(Texture, level_count), Py_READONLY, NULL},
    {"multisampled", Py_T_BOOL, offsetof(Texture, multisampled), Py_READONLY, NULL},
    {"glo", Py_T_UINT, offsetof(Texture, texture), Py_READONLY, NULL},
    {0},
};

static PyMethodDef Sampler_methods[] = {
    {"use", (PyCFunction)Sampler_meth_use, METH_O, NULL},
    {0},
};

static PyMemberDef Sampler_members[] = {
    {"texture", Py_T_OBJECT_EX, offsetof(Sampler, texture), Py_READONLY, NULL},
    {"glo", Py_T_UINT, offsetof(Sampler, sampler), Py_READONLY, NULL},
    {0},
};

static PyMethodDef Resources_methods[] = {
    {"use", (PyCFunction)Resources_meth_use, METH_NOARGS, NULL},
    {0},
};

static PyGetSetDef Resources_getset[] = {
    {"samplers", (getter)Resources_get_samplers, NULL, NULL, NULL},
    {"uniform_buffers", (getter)Resources_get_uniform_buffers, NULL, NULL, NULL},
    {"storage_buffers", (getter)Resources_get_storage_buffers, NULL, NULL, NULL},
    {"storage_images", (getter)Resources_get_storage_images, NULL, NULL, NULL},
    {0},
};

static PyMethodDef VertexArray_methods[] = {
    {"use", (PyCFunction)VertexArray_meth_use, METH_NOARGS, NULL},
    {"render", (PyCFunction)VertexArray_meth_render, METH_VARARGS | METH_KEYWORDS, NULL},
    {"draw", (PyCFunction)VertexArray_meth_draw, METH_VARARGS | METH_KEYWORDS, NULL},
    {"draw_indirect", (PyCFunction)VertexArray_meth_draw_indirect, METH_VARARGS | METH_KEYWORDS, NULL},
    {0},
};

static PyMemberDef VertexArray_members[] = {
    {"vertices", Py_T_INT, offsetof(VertexArray, vertices), 0, NULL},
    {"attributes", Py_T_OBJECT_EX, offsetof(VertexArray, attributes), Py_READONLY, NULL},
    {"index_buffer", Py_T_OBJECT_EX, offsetof(VertexArray, index_buffer), Py_READONLY, NULL},
    {"glo", Py_T_UINT, offsetof(VertexArray, vertex_array), Py_READONLY, NULL},
    {0},
};

static PyGetSetDef VertexArray_getset[] = {
    {"released", (getter)get_released, NULL, NULL, NULL},
    {"mode", (getter)VertexArray_get_mode, (setter)VertexArray_set_mode, NULL, NULL},
    {0},
};

static PyMethodDef Program_methods[] = {
    {"use", (PyCFunction)Program_meth_use, METH_NOARGS, NULL},
    {0},
};

static PyMemberDef Program_members[] = {
    {"glo", Py_T_UINT, offsetof(Program, program), Py_READONLY, NULL},
    {0},
};

static PyMethodDef Framebuffer_methods[] = {
    {"clear", (PyCFunction)Framebuffer_meth_clear, METH_VARARGS | METH_KEYWORDS, NULL},
    {"use", (PyCFunction)Framebuffer_meth_use, METH_NOARGS, NULL},
    {0},
};

static PyMemberDef Framebuffer_members[] = {
    {"attachments", Py_T_OBJECT_EX, offsetof(Framebuffer, attachments), Py_READONLY, NULL},
    {"multisampled", Py_T_BOOL, offsetof(Framebuffer, multisampled), Py_READONLY, NULL},
    {"glo", Py_T_UINT, offsetof(Framebuffer, framebuffer), Py_READONLY, NULL},
    {0},
};

static PyGetSetDef Framebuffer_getset[] = {
    {"released", (getter)get_released, NULL, NULL, NULL},
    {"viewport", (getter)Framebuffer_get_viewport, (setter)Framebuffer_set_viewport, NULL, NULL},
    {"scissor", (getter)Framebuffer_get_scissor, (setter)Framebuffer_set_scissor, NULL, NULL},
    {"size", (getter)Framebuffer_get_size, (setter)Framebuffer_set_size, NULL, NULL},
    {"clear_color", (getter)Framebuffer_get_clear_color, (setter)Framebuffer_set_clear_color, NULL, NULL},
    {"clear_depth", (getter)Framebuffer_get_clear_depth, (setter)Framebuffer_set_clear_depth, NULL, NULL},
    {"clear_stencil", (getter)Framebuffer_get_clear_stencil, (setter)Framebuffer_set_clear_stencil, NULL, NULL},
    {0},
};

static PyGetSetDef Texture_getset[] = {
    {"released", (getter)get_released, NULL, NULL, NULL},
    {0},
};

static PyMethodDef Pipeline_methods[] = {
    {"run", (PyCFunction)Pipeline_meth_run, METH_NOARGS, NULL},
    {"variant", (PyCFunction)Pipeline_meth_variant, METH_VARARGS | METH_KEYWORDS, NULL},
    {"use", (PyCFunction)Pipeline_meth_use, METH_NOARGS, NULL},
    {0},
};

static PyMethodDef ReadRequest_methods[] = {
    {"is_ready", (PyCFunction)ReadRequest_meth_is_ready, METH_NOARGS, NULL},
    {"wait", (PyCFunction)ReadRequest_meth_wait, METH_NOARGS, NULL},
    {0},
};

static PyMemberDef ReadRequest_members[] = {
    {"size", Py_T_INT, offsetof(ReadRequest, size), Py_READONLY, NULL},
    {0},
};

static PyMemberDef Pipeline_members[] = {
    {"program", Py_T_OBJECT_EX, offsetof(Pipeline, program), Py_READONLY, NULL},
    {"vertex_array", Py_T_OBJECT_EX, offsetof(Pipeline, vertex_array), Py_READONLY, NULL},
    {"framebuffer", Py_T_OBJECT_EX, offsetof(Pipeline, framebuffer), Py_READONLY, NULL},
    {"resources", Py_T_OBJECT_EX, offsetof(Pipeline, resources), Py_READONLY, NULL},
    {"vertices", Py_T_INT, offsetof(Pipeline, vertices), 0, NULL},
    {"instances", Py_T_INT, offsetof(Pipeline, instances), 0, NULL},
    {"first", Py_T_INT, offsetof(Pipeline, first), 0, NULL},
    {"first_instance", Py_T_INT, offsetof(Pipeline, first_instance), 0, NULL},
    {0},
};

static PyGetSetDef Pipeline_getset[] = {
    {"released", (getter)get_released, NULL, NULL, NULL},
    {"viewport", (getter)Pipeline_get_viewport, (setter)Pipeline_set_viewport, NULL, NULL},
    {"scissor", (getter)Pipeline_get_scissor, (setter)Pipeline_set_scissor, NULL, NULL},
    {"group_count", (getter)Pipeline_get_group_count, (setter)Pipeline_set_group_count, NULL, NULL},
    {0},
};

static PyMethodDef Context_methods[] = {
    {"buffer", (PyCFunction)Context_meth_buffer, METH_VARARGS | METH_KEYWORDS, NULL},
    {"texture", (PyCFunction)Context_meth_texture, METH_VARARGS | METH_KEYWORDS, NULL},
    {"framebuffer", (PyCFunction)Context_meth_framebuffer, METH_VARARGS | METH_KEYWORDS, NULL},
    {"vertex_array", (PyCFunction)Context_meth_vertex_array, METH_VARARGS | METH_KEYWORDS, NULL},
    {"external_framebuffer", (PyCFunction)Context_meth_external_framebuffer, METH_VARARGS | METH_KEYWORDS, NULL},
    {"external_buffer", (PyCFunction)Context_meth_external_buffer, METH_VARARGS | METH_KEYWORDS, NULL},
    {"external_texture", (PyCFunction)Context_meth_external_texture, METH_VARARGS | METH_KEYWORDS, NULL},
    {"gc", (PyCFunction)Context_meth_gc, METH_NOARGS, NULL},
    {"debug", (PyCFunction)Context_meth_debug, METH_NOARGS, NULL},
    {"source", (PyCFunction)Context_meth_source, METH_O, NULL},
    {"start_frame", (PyCFunction)Context_meth_start_frame, METH_NOARGS, NULL},
    {"end_frame", (PyCFunction)Context_meth_end_frame, METH_NOARGS, NULL},
    {"present", (PyCFunction)Context_meth_present, METH_O, NULL},
    {"copy_buffer", (PyCFunction)Context_meth_copy_buffer, METH_VARARGS | METH_KEYWORDS, NULL},
    {"copy_texture", (PyCFunction)Context_meth_copy_texture, METH_VARARGS | METH_KEYWORDS, NULL},
    {"copy_buffer_to_texture", (PyCFunction)Context_meth_copy_buffer_to_texture, METH_VARARGS | METH_KEYWORDS, NULL},
    {"copy_texture_to_buffer", (PyCFunction)Context_meth_copy_texture_to_buffer, METH_VARARGS | METH_KEYWORDS, NULL},
    {"program", (PyCFunction)Context_meth_program, METH_VARARGS | METH_KEYWORDS, NULL},
    {"release", (PyCFunction)Context_meth_release, METH_VARARGS, NULL},
    {"reset_state", (PyCFunction)Context_meth_reset_state, METH_NOARGS, NULL},
    {"sampler", (PyCFunction)Context_meth_sampler, METH_VARARGS | METH_KEYWORDS, NULL},
    {"resources", (PyCFunction)Context_meth_resources, METH_VARARGS | METH_KEYWORDS, NULL},
    {"pipeline", (PyCFunction)Context_meth_pipeline, METH_VARARGS | METH_KEYWORDS, NULL},
    {"compute", (PyCFunction)Context_meth_compute, METH_VARARGS | METH_KEYWORDS, NULL},
    {"memory_barrier", (PyCFunction)Context_meth_memory_barrier, METH_NOARGS, NULL},
    {"blend", (PyCFunction)Context_meth_blend, METH_VARARGS | METH_KEYWORDS, NULL},
    {"stencil", (PyCFunction)Context_meth_stencil, METH_VARARGS | METH_KEYWORDS, NULL},
    {0},
};

static PyMemberDef Context_members[] = {
    {"shaders", Py_T_OBJECT_EX, offsetof(Context, shaders), Py_READONLY, NULL},
    {"info", Py_T_OBJECT_EX, offsetof(Context, info), Py_READONLY, NULL},
    {"empty_vertex_array", Py_T_OBJECT_EX, offsetof(Context, empty_vertex_array), Py_READONLY, NULL},
    {0},
};

static PyGetSetDef Context_getset[] = {
    {"bound_program", (getter)Context_get_bound_program, NULL, NULL, NULL},
    {"bound_framebuffer", (getter)Context_get_bound_framebuffer, NULL, NULL, NULL},
    {"bound_vertex_array", (getter)Context_get_bound_vertex_array, NULL, NULL, NULL},
    {"screen", (getter)Context_get_screen, NULL, NULL, NULL},
    {"depth_test", (getter)Context_get_depth_test, (setter)Context_set_depth_test, NULL, NULL},
    {"depth_write", (getter)Context_get_depth_write, (setter)Context_set_depth_write, NULL, NULL},
    {"depth_func", (getter)Context_get_depth_func, (setter)Context_set_depth_func, NULL, NULL},
    {"cull_face", (getter)Context_get_cull_face, (setter)Context_set_cull_face, NULL, NULL},
    {"polygon_offset", (getter)Context_get_polygon_offset, (setter)Context_set_polygon_offset, NULL, NULL},
    {0},
};

static PyType_Slot HeadlessContext_slots[] = {
    {Py_tp_methods, HeadlessContext_methods},
    {Py_tp_dealloc, (void *)HeadlessContext_dealloc},
    {0},
};

static PyGetSetDef Buffer_getset[] = {
    {"released", (getter)get_released, NULL, NULL, NULL},
    {0},
};

static PyType_Slot Buffer_slots[] = {
    {Py_tp_getset, Buffer_getset},
    {Py_tp_methods, Buffer_methods},
    {Py_tp_members, Buffer_members},
    {Py_tp_dealloc, (void *)Buffer_dealloc},
    {0},
};

static PyType_Slot Texture_slots[] = {
    {Py_tp_methods, Texture_methods},
    {Py_tp_members, Texture_members},
    {Py_tp_getset, Texture_getset},
    {Py_tp_dealloc, (void *)Texture_dealloc},
    {0},
};

static PyGetSetDef Sampler_getset[] = {
    {"released", (getter)get_released, NULL, NULL, NULL},
    {0},
};

static PyType_Slot Sampler_slots[] = {
    {Py_tp_getset, Sampler_getset},
    {Py_tp_methods, Sampler_methods},
    {Py_tp_members, Sampler_members},
    {Py_tp_dealloc, (void *)Sampler_dealloc},
    {0},
};

static PyType_Slot Resources_slots[] = {
    {Py_tp_methods, Resources_methods},
    {Py_tp_getset, Resources_getset},
    {Py_tp_dealloc, (void *)Resources_dealloc},
    {0},
};

static PyType_Slot VertexArray_slots[] = {
    {Py_tp_methods, VertexArray_methods},
    {Py_tp_getset, VertexArray_getset},
    {Py_tp_members, VertexArray_members},
    {Py_tp_dealloc, (void *)VertexArray_dealloc},
    {0},
};

static PyGetSetDef Program_getset[] = {
    {"released", (getter)get_released, NULL, NULL, NULL},
    {0},
};

static PyType_Slot Program_slots[] = {
    {Py_tp_getset, Program_getset},
    {Py_tp_methods, Program_methods},
    {Py_tp_members, Program_members},
    {Py_mp_ass_subscript, (void *)Program_ass_subscript},
    {Py_tp_dealloc, (void *)Program_dealloc},
    {0},
};

static PyType_Slot Framebuffer_slots[] = {
    {Py_tp_methods, Framebuffer_methods},
    {Py_tp_getset, Framebuffer_getset},
    {Py_tp_members, Framebuffer_members},
    {Py_tp_dealloc, (void *)Framebuffer_dealloc},
    {0},
};

static PyType_Slot Pipeline_slots[] = {
    {Py_tp_methods, Pipeline_methods},
    {Py_tp_getset, Pipeline_getset},
    {Py_tp_members, Pipeline_members},
    {Py_mp_ass_subscript, (void *)Pipeline_ass_subscript},
    {Py_tp_dealloc, (void *)Pipeline_dealloc},
    {0},
};

static PyType_Slot ReadRequest_slots[] = {
    {Py_tp_methods, ReadRequest_methods},
    {Py_tp_members, ReadRequest_members},
    {Py_tp_dealloc, (void *)ReadRequest_dealloc},
    {0},
};

static PyType_Slot Context_slots[] = {
    {Py_tp_methods, Context_methods},
    {Py_tp_getset, Context_getset},
    {Py_tp_members, Context_members},
    {Py_tp_dealloc, (void *)Context_dealloc},
    {0},
};

#define TYPE_FLAGS (Py_TPFLAGS_DEFAULT | Py_TPFLAGS_DISALLOW_INSTANTIATION)
#define TYPE_SPEC(name) {"moderngl." #name, sizeof(name), 0, TYPE_FLAGS, name##_slots}

static PyType_Spec HeadlessContext_spec = TYPE_SPEC(HeadlessContext);
static PyType_Spec Buffer_spec = TYPE_SPEC(Buffer);
static PyType_Spec Texture_spec = TYPE_SPEC(Texture);
static PyType_Spec Sampler_spec = TYPE_SPEC(Sampler);
static PyType_Spec Resources_spec = TYPE_SPEC(Resources);
static PyType_Spec VertexArray_spec = TYPE_SPEC(VertexArray);
static PyType_Spec Program_spec = TYPE_SPEC(Program);
static PyType_Spec Framebuffer_spec = TYPE_SPEC(Framebuffer);
static PyType_Spec Pipeline_spec = TYPE_SPEC(Pipeline);
static PyType_Spec ReadRequest_spec = TYPE_SPEC(ReadRequest);
static PyType_Spec Context_spec = TYPE_SPEC(Context);

static PyTypeObject * add_type(PyObject * self, PyType_Spec * spec) {
    PyTypeObject * type = (PyTypeObject *)PyType_FromModuleAndSpec(self, spec, NULL);
    if (!type) {
        return NULL;
    }
    if (PyModule_AddType(self, type)) {
        Py_DECREF(type);
        return NULL;
    }
    return type;
}

static int module_exec(PyObject * self) {
    ModuleState * state = (ModuleState *)PyModule_GetState(self);

    if (!(state->Context_type = add_type(self, &Context_spec))) {
        return -1;
    }
    if (!(state->Program_type = add_type(self, &Program_spec))) {
        return -1;
    }
    if (!(state->Framebuffer_type = add_type(self, &Framebuffer_spec))) {
        return -1;
    }
    if (!(state->VertexArray_type = add_type(self, &VertexArray_spec))) {
        return -1;
    }
    if (!(state->Texture_type = add_type(self, &Texture_spec))) {
        return -1;
    }
    if (!(state->Buffer_type = add_type(self, &Buffer_spec))) {
        return -1;
    }
    if (!(state->Sampler_type = add_type(self, &Sampler_spec))) {
        return -1;
    }
    if (!(state->Resources_type = add_type(self, &Resources_spec))) {
        return -1;
    }
    if (!(state->Pipeline_type = add_type(self, &Pipeline_spec))) {
        return -1;
    }
    if (!(state->ReadRequest_type = add_type(self, &ReadRequest_spec))) {
        return -1;
    }
    if (!(state->HeadlessContext_type = add_type(self, &HeadlessContext_spec))) {
        return -1;
    }
    if (PyModule_AddStringConstant(self, "__version__", "6.0.0b3")) {
        return -1;
    }

#ifdef EXTERN_GL
    if (PyModule_Add(self, "_extern_gl", PyUnicode_FromString(webgl_js))) {
        return -1;
    }
#endif

    return 0;
}

static PyModuleDef_Slot module_slots[] = {
    {Py_mod_exec, (void *)module_exec},
#if PY_VERSION_HEX >= 0x030D0000
    {Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED},
    {Py_mod_gil, Py_MOD_GIL_NOT_USED},
#elif PY_VERSION_HEX >= 0x030C0000
    {Py_mod_multiple_interpreters, Py_MOD_MULTIPLE_INTERPRETERS_SUPPORTED},
#endif
    {0},
};

static PyMethodDef module_methods[] = {
    {"init", (PyCFunction)meth_init, METH_VARARGS | METH_KEYWORDS, NULL},
    {"get_proc_address", (PyCFunction)meth_get_proc_address, METH_O, NULL},
    {"create_context", (PyCFunction)meth_create_context, METH_NOARGS, NULL},
    {"get_context", (PyCFunction)meth_get_context, METH_NOARGS, NULL},
    {"headless_context", (PyCFunction)meth_headless_context, METH_NOARGS, NULL},
    {"camera", (PyCFunction)meth_camera, METH_VARARGS | METH_KEYWORDS, NULL},
    {0},
};

static int module_traverse(PyObject * self, visitproc visit, void * arg) {
    ModuleState * state = (ModuleState *)PyModule_GetState(self);
    Py_VISIT(state->context);
    Py_VISIT(state->screen);
    Py_VISIT(state->Context_type);
    Py_VISIT(state->Program_type);
    Py_VISIT(state->Framebuffer_type);
    Py_VISIT(state->VertexArray_type);
    Py_VISIT(state->Texture_type);
    Py_VISIT(state->Buffer_type);
    Py_VISIT(state->Sampler_type);
    Py_VISIT(state->Resources_type);
    Py_VISIT(state->Pipeline_type);
    Py_VISIT(state->ReadRequest_type);
    Py_VISIT(state->HeadlessContext_type);
    return 0;
}

static int module_clear(PyObject * self) {
    ModuleState * state = (ModuleState *)PyModule_GetState(self);
    Py_CLEAR(state->screen);
    Py_CLEAR(state->context);
    Py_CLEAR(state->Context_type);
    Py_CLEAR(state->Program_type);
    Py_CLEAR(state->Framebuffer_type);
    Py_CLEAR(state->VertexArray_type);
    Py_CLEAR(state->Texture_type);
    Py_CLEAR(state->Buffer_type);
    Py_CLEAR(state->Sampler_type);
    Py_CLEAR(state->Resources_type);
    Py_CLEAR(state->Pipeline_type);
    Py_CLEAR(state->ReadRequest_type);
    Py_CLEAR(state->HeadlessContext_type);
    return 0;
}

static void module_free(void * self) {
    module_clear((PyObject *)self);
}

static PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT, "moderngl", NULL, sizeof(ModuleState), module_methods, module_slots,
    module_traverse, module_clear, module_free,
};

extern PyObject * PyInit_moderngl() {
    return PyModuleDef_Init(&module_def);
}
