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

enum {
    GL_ACTIVE_UNIFORMS = 0x8B86,
    GL_ACTIVE_UNIFORM_BLOCKS = 0x8A36,
    GL_ALWAYS = 0x0207,
    GL_ARRAY_BUFFER = 0x8892,
    GL_BACK = 0x0405,
    GL_BLEND = 0x0BE2,
    GL_BOOL = 0x8B56,
    GL_BOOL_VEC2 = 0x8B57,
    GL_BOOL_VEC3 = 0x8B58,
    GL_BOOL_VEC4 = 0x8B59,
    GL_BUFFER_SIZE = 0x8764,
    GL_COLOR = 0x1800,
    GL_COLOR_ATTACHMENT0 = 0x8CE0,
    GL_COLOR_BUFFER_BIT = 0x00004000,
    GL_COMPARE_REF_TO_TEXTURE = 0x884E,
    GL_COMPILE_STATUS = 0x8B81,
    GL_COPY_READ_BUFFER = 0x8F36,
    GL_COPY_WRITE_BUFFER = 0x8F37,
    GL_CULL_FACE = 0x0B44,
    GL_DEPTH = 0x1801,
    GL_DEPTH_ATTACHMENT = 0x8D00,
    GL_DEPTH_BUFFER_BIT = 0x00000100,
    GL_DEPTH_STENCIL = 0x84F9,
    GL_DEPTH_STENCIL_ATTACHMENT = 0x821A,
    GL_DEPTH_TEST = 0x0B71,
    GL_DRAW_FRAMEBUFFER = 0x8CA9,
    GL_ELEMENT_ARRAY_BUFFER = 0x8893,
    GL_FALSE = 0,
    GL_FLOAT = 0x1406,
    GL_FLOAT_MAT2 = 0x8B5A,
    GL_FLOAT_MAT2x3 = 0x8B65,
    GL_FLOAT_MAT2x4 = 0x8B66,
    GL_FLOAT_MAT3 = 0x8B5B,
    GL_FLOAT_MAT3x2 = 0x8B67,
    GL_FLOAT_MAT3x4 = 0x8B68,
    GL_FLOAT_MAT4 = 0x8B5C,
    GL_FLOAT_MAT4x2 = 0x8B69,
    GL_FLOAT_MAT4x3 = 0x8B6A,
    GL_FLOAT_VEC2 = 0x8B50,
    GL_FLOAT_VEC3 = 0x8B51,
    GL_FLOAT_VEC4 = 0x8B52,
    GL_FRAGMENT_SHADER = 0x8B30,
    GL_FRAMEBUFFER = 0x8D40,
    GL_FRAMEBUFFER_COMPLETE = 0x8CD5,
    GL_FRONT = 0x0404,
    GL_FUNC_ADD = 0x8006,
    GL_INT = 0x1404,
    GL_INT_VEC2 = 0x8B53,
    GL_INT_VEC3 = 0x8B54,
    GL_INT_VEC4 = 0x8B55,
    GL_KEEP = 0x1E00,
    GL_LESS = 0x0201,
    GL_LINEAR = 0x2601,
    GL_LINK_STATUS = 0x8B82,
    GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0x8B4D,
    GL_MAX_SAMPLES = 0x8D57,
    GL_MAX_UNIFORM_BLOCK_SIZE = 0x8A30,
    GL_MAX_UNIFORM_BUFFER_BINDINGS = 0x8A2F,
    GL_MAX_VERTEX_ATTRIBS = 0x8869,
    GL_NEAREST = 0x2600,
    GL_NONE = 0,
    GL_ONE = 1,
    GL_PIXEL_PACK_BUFFER = 0x88EB,
    GL_PIXEL_UNPACK_BUFFER = 0x88EC,
    GL_PRIMITIVE_RESTART = 0x8F9D,
    GL_PROGRAM_POINT_SIZE = 0x8642,
    GL_READ_FRAMEBUFFER = 0x8CA8,
    GL_RENDERBUFFER = 0x8D41,
    GL_RENDERER = 0x1F01,
    GL_SHADING_LANGUAGE_VERSION = 0x8B8C,
    GL_STENCIL_BUFFER_BIT = 0x00000400,
    GL_STENCIL_TEST = 0x0B90,
    GL_TEXTURE0 = 0x84C0,
    GL_TEXTURE_2D = 0x0DE1,
    GL_TEXTURE_2D_ARRAY = 0x8C1A,
    GL_TEXTURE_COMPARE_FUNC = 0x884D,
    GL_TEXTURE_COMPARE_MODE = 0x884C,
    GL_TEXTURE_CUBE_MAP = 0x8513,
    GL_TEXTURE_CUBE_MAP_POSITIVE_X = 0x8515,
    GL_TEXTURE_CUBE_MAP_SEAMLESS = 0x884F,
    GL_TEXTURE_LOD_BIAS = 0x8501,
    GL_TEXTURE_MAG_FILTER = 0x2800,
    GL_TEXTURE_MAX_ANISOTROPY = 0x84FE,
    GL_TEXTURE_MAX_LEVEL = 0x813D,
    GL_TEXTURE_MAX_LOD = 0x813B,
    GL_TEXTURE_MIN_FILTER = 0x2801,
    GL_TEXTURE_MIN_LOD = 0x813A,
    GL_TEXTURE_WRAP_R = 0x8072,
    GL_TEXTURE_WRAP_S = 0x2802,
    GL_TEXTURE_WRAP_T = 0x2803,
    GL_TRUE = 1,
    GL_UNIFORM_BLOCK_BINDING = 0x8A3F,
    GL_UNIFORM_BUFFER = 0x8A11,
    GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0x8A34,
    GL_UNSIGNED_BYTE = 0x1401,
    GL_UNSIGNED_INT = 0x1405,
    GL_UNSIGNED_INT_VEC2 = 0x8DC6,
    GL_UNSIGNED_INT_VEC3 = 0x8DC7,
    GL_UNSIGNED_INT_VEC4 = 0x8DC8,
    GL_UNSIGNED_SHORT = 0x1403,
    GL_VENDOR = 0x1F00,
    GL_VERSION = 0x1F02,
    GL_VERTEX_SHADER = 0x8B31,
    GL_VIEWPORT = 0x0BA2,
    GL_ZERO = 0,
};

static const uint32_t GL_INVALID_INDEX = 0xFFFFFFFF;

static THREAD_LOCAL int gl_loaded;
static PyMutex gl_mutex;

#ifdef EXTERN_GL

extern void glActiveTexture(uint32_t texture) __asm__("moderngl_glActiveTexture");
extern void glAttachShader(uint32_t program, uint32_t shader) __asm__("moderngl_glAttachShader");
extern void glBindBuffer(uint32_t target, uint32_t buffer) __asm__("moderngl_glBindBuffer");
extern void glBindBufferBase(uint32_t target, uint32_t index, uint32_t buffer) __asm__("moderngl_glBindBufferBase");
extern void glBindBufferRange(uint32_t target, uint32_t index, uint32_t buffer, intptr_t offset, intptr_t size) __asm__("moderngl_glBindBufferRange");
extern void glBindFramebuffer(uint32_t target, uint32_t framebuffer) __asm__("moderngl_glBindFramebuffer");
extern void glBindRenderbuffer(uint32_t target, uint32_t renderbuffer) __asm__("moderngl_glBindRenderbuffer");
extern void glBindSampler(uint32_t unit, uint32_t sampler) __asm__("moderngl_glBindSampler");
extern void glBindTexture(uint32_t target, uint32_t texture) __asm__("moderngl_glBindTexture");
extern void glBindVertexArray(uint32_t array) __asm__("moderngl_glBindVertexArray");
extern void glBlendEquationSeparate(uint32_t mode_rgb, uint32_t mode_alpha) __asm__("moderngl_glBlendEquationSeparate");
extern void glBlendFuncSeparate(uint32_t src_rgb, uint32_t dst_rgb, uint32_t src_alpha, uint32_t dst_alpha) __asm__("moderngl_glBlendFuncSeparate");
extern void glBlitFramebuffer(int32_t src_x0, int32_t src_y0, int32_t src_x1, int32_t src_y1, int32_t dst_x0, int32_t dst_y0, int32_t dst_x1, int32_t dst_y1, uint32_t mask, uint32_t filter) __asm__("moderngl_glBlitFramebuffer");
extern void glBufferData(uint32_t target, intptr_t size, const void * data, uint32_t usage) __asm__("moderngl_glBufferData");
extern void glBufferSubData(uint32_t target, intptr_t offset, intptr_t size, const void * data) __asm__("moderngl_glBufferSubData");
extern uint32_t glCheckFramebufferStatus(uint32_t target) __asm__("moderngl_glCheckFramebufferStatus");
extern void glClearBufferfi(uint32_t buffer, int32_t drawbuffer, float depth, int32_t stencil) __asm__("moderngl_glClearBufferfi");
extern void glClearBufferfv(uint32_t buffer, int32_t drawbuffer, const float * value) __asm__("moderngl_glClearBufferfv");
extern void glClearBufferiv(uint32_t buffer, int32_t drawbuffer, const int32_t * value) __asm__("moderngl_glClearBufferiv");
extern void glClearBufferuiv(uint32_t buffer, int32_t drawbuffer, const uint32_t * value) __asm__("moderngl_glClearBufferuiv");
extern void glCompileShader(uint32_t shader) __asm__("moderngl_glCompileShader");
extern void glCopyBufferSubData(uint32_t read_target, uint32_t write_target, intptr_t read_offset, intptr_t write_offset, intptr_t size) __asm__("moderngl_glCopyBufferSubData");
extern uint32_t glCreateProgram(void) __asm__("moderngl_glCreateProgram");
extern uint32_t glCreateShader(uint32_t type) __asm__("moderngl_glCreateShader");
extern void glCullFace(uint32_t mode) __asm__("moderngl_glCullFace");
extern void glDeleteBuffers(int32_t n, const uint32_t * buffers) __asm__("moderngl_glDeleteBuffers");
extern void glDeleteFramebuffers(int32_t n, const uint32_t * framebuffers) __asm__("moderngl_glDeleteFramebuffers");
extern void glDeleteProgram(uint32_t program) __asm__("moderngl_glDeleteProgram");
extern void glDeleteRenderbuffers(int32_t n, const uint32_t * renderbuffers) __asm__("moderngl_glDeleteRenderbuffers");
extern void glDeleteSamplers(int32_t n, const uint32_t * samplers) __asm__("moderngl_glDeleteSamplers");
extern void glDeleteShader(uint32_t shader) __asm__("moderngl_glDeleteShader");
extern void glDeleteTextures(int32_t n, const uint32_t * textures) __asm__("moderngl_glDeleteTextures");
extern void glDeleteVertexArrays(int32_t n, const uint32_t * arrays) __asm__("moderngl_glDeleteVertexArrays");
extern void glDepthFunc(uint32_t func) __asm__("moderngl_glDepthFunc");
extern void glDepthMask(uint8_t flag) __asm__("moderngl_glDepthMask");
extern void glDetachShader(uint32_t program, uint32_t shader) __asm__("moderngl_glDetachShader");
extern void glDisable(uint32_t cap) __asm__("moderngl_glDisable");
extern void glDrawArraysInstanced(uint32_t mode, int32_t first, int32_t count, int32_t instance_count) __asm__("moderngl_glDrawArraysInstanced");
extern void glDrawBuffers(int32_t n, const uint32_t * bufs) __asm__("moderngl_glDrawBuffers");
extern void glDrawElementsInstanced(uint32_t mode, int32_t count, uint32_t type, const void * indices, int32_t instance_count) __asm__("moderngl_glDrawElementsInstanced");
extern void glEnable(uint32_t cap) __asm__("moderngl_glEnable");
extern void glEnableVertexAttribArray(uint32_t index) __asm__("moderngl_glEnableVertexAttribArray");
extern void glFlush(void) __asm__("moderngl_glFlush");
extern void glFramebufferRenderbuffer(uint32_t target, uint32_t attachment, uint32_t renderbuffer_target, uint32_t renderbuffer) __asm__("moderngl_glFramebufferRenderbuffer");
extern void glFramebufferTexture2D(uint32_t target, uint32_t attachment, uint32_t texture_target, uint32_t texture, int32_t level) __asm__("moderngl_glFramebufferTexture2D");
extern void glFramebufferTextureLayer(uint32_t target, uint32_t attachment, uint32_t texture, int32_t level, int32_t layer) __asm__("moderngl_glFramebufferTextureLayer");
extern void glGenBuffers(int32_t n, uint32_t * buffers) __asm__("moderngl_glGenBuffers");
extern void glGenFramebuffers(int32_t n, uint32_t * framebuffers) __asm__("moderngl_glGenFramebuffers");
extern void glGenRenderbuffers(int32_t n, uint32_t * renderbuffers) __asm__("moderngl_glGenRenderbuffers");
extern void glGenSamplers(int32_t n, uint32_t * samplers) __asm__("moderngl_glGenSamplers");
extern void glGenTextures(int32_t n, uint32_t * textures) __asm__("moderngl_glGenTextures");
extern void glGenVertexArrays(int32_t n, uint32_t * arrays) __asm__("moderngl_glGenVertexArrays");
extern void glGenerateMipmap(uint32_t target) __asm__("moderngl_glGenerateMipmap");
extern void glGetActiveUniform(uint32_t program, uint32_t index, int32_t buf_size, int32_t * length, int32_t * size, uint32_t * type, char * name) __asm__("moderngl_glGetActiveUniform");
extern void glGetActiveUniformBlockName(uint32_t program, uint32_t index, int32_t buf_size, int32_t * length, char * name) __asm__("moderngl_glGetActiveUniformBlockName");
extern void glGetActiveUniformBlockiv(uint32_t program, uint32_t index, uint32_t pname, int32_t * params) __asm__("moderngl_glGetActiveUniformBlockiv");
extern void glGetBufferParameteriv(uint32_t target, uint32_t pname, int32_t * params) __asm__("moderngl_glGetBufferParameteriv");
extern void glGetBufferSubData(uint32_t target, intptr_t offset, intptr_t size, void * data) __asm__("moderngl_glGetBufferSubData");
extern void glGetIntegerv(uint32_t pname, int32_t * data) __asm__("moderngl_glGetIntegerv");
extern void glGetProgramInfoLog(uint32_t program, int32_t buf_size, int32_t * length, char * log) __asm__("moderngl_glGetProgramInfoLog");
extern void glGetProgramiv(uint32_t program, uint32_t pname, int32_t * params) __asm__("moderngl_glGetProgramiv");
extern void glGetShaderInfoLog(uint32_t shader, int32_t buf_size, int32_t * length, char * log) __asm__("moderngl_glGetShaderInfoLog");
extern void glGetShaderiv(uint32_t shader, uint32_t pname, int32_t * params) __asm__("moderngl_glGetShaderiv");
extern const uint8_t * glGetString(uint32_t name) __asm__("moderngl_glGetString");
extern uint32_t glGetUniformBlockIndex(uint32_t program, const char * name) __asm__("moderngl_glGetUniformBlockIndex");
extern int32_t glGetUniformLocation(uint32_t program, const char * name) __asm__("moderngl_glGetUniformLocation");
extern void glGetUniformfv(uint32_t program, int32_t location, float * params) __asm__("moderngl_glGetUniformfv");
extern void glGetUniformiv(uint32_t program, int32_t location, int32_t * params) __asm__("moderngl_glGetUniformiv");
extern void glGetUniformuiv(uint32_t program, int32_t location, uint32_t * params) __asm__("moderngl_glGetUniformuiv");
extern void glLinkProgram(uint32_t program) __asm__("moderngl_glLinkProgram");
extern void glPrimitiveRestartIndex(uint32_t index) __asm__("moderngl_glPrimitiveRestartIndex");
extern void glReadBuffer(uint32_t src) __asm__("moderngl_glReadBuffer");
extern void glReadPixels(int32_t x, int32_t y, int32_t width, int32_t height, uint32_t format, uint32_t type, void * pixels) __asm__("moderngl_glReadPixels");
extern void glRenderbufferStorageMultisample(uint32_t target, int32_t samples, uint32_t internal_format, int32_t width, int32_t height) __asm__("moderngl_glRenderbufferStorageMultisample");
extern void glSamplerParameterf(uint32_t sampler, uint32_t pname, float param) __asm__("moderngl_glSamplerParameterf");
extern void glSamplerParameteri(uint32_t sampler, uint32_t pname, int32_t param) __asm__("moderngl_glSamplerParameteri");
extern void glShaderSource(uint32_t shader, int32_t count, const char * const * string, const int32_t * length) __asm__("moderngl_glShaderSource");
extern void glStencilFunc(uint32_t func, int32_t ref, uint32_t mask) __asm__("moderngl_glStencilFunc");
extern void glStencilFuncSeparate(uint32_t face, uint32_t func, int32_t ref, uint32_t mask) __asm__("moderngl_glStencilFuncSeparate");
extern void glStencilMask(uint32_t mask) __asm__("moderngl_glStencilMask");
extern void glStencilOp(uint32_t fail, uint32_t depth_fail, uint32_t depth_pass) __asm__("moderngl_glStencilOp");
extern void glStencilOpSeparate(uint32_t face, uint32_t fail, uint32_t depth_fail, uint32_t depth_pass) __asm__("moderngl_glStencilOpSeparate");
extern void glTexImage2D(uint32_t target, int32_t level, int32_t internal_format, int32_t width, int32_t height, int32_t border, uint32_t format, uint32_t type, const void * pixels) __asm__("moderngl_glTexImage2D");
extern void glTexImage3D(uint32_t target, int32_t level, int32_t internal_format, int32_t width, int32_t height, int32_t depth, int32_t border, uint32_t format, uint32_t type, const void * pixels) __asm__("moderngl_glTexImage3D");
extern void glTexParameteri(uint32_t target, uint32_t pname, int32_t param) __asm__("moderngl_glTexParameteri");
extern void glTexSubImage2D(uint32_t target, int32_t level, int32_t x, int32_t y, int32_t width, int32_t height, uint32_t format, uint32_t type, const void * pixels) __asm__("moderngl_glTexSubImage2D");
extern void glTexSubImage3D(uint32_t target, int32_t level, int32_t x, int32_t y, int32_t z, int32_t width, int32_t height, int32_t depth, uint32_t format, uint32_t type, const void * pixels) __asm__("moderngl_glTexSubImage3D");
extern void glUniform1fv(int32_t location, int32_t count, const float * value) __asm__("moderngl_glUniform1fv");
extern void glUniform1i(int32_t location, int32_t value) __asm__("moderngl_glUniform1i");
extern void glUniform1iv(int32_t location, int32_t count, const int32_t * value) __asm__("moderngl_glUniform1iv");
extern void glUniform1uiv(int32_t location, int32_t count, const uint32_t * value) __asm__("moderngl_glUniform1uiv");
extern void glUniform2fv(int32_t location, int32_t count, const float * value) __asm__("moderngl_glUniform2fv");
extern void glUniform2iv(int32_t location, int32_t count, const int32_t * value) __asm__("moderngl_glUniform2iv");
extern void glUniform2uiv(int32_t location, int32_t count, const uint32_t * value) __asm__("moderngl_glUniform2uiv");
extern void glUniform3fv(int32_t location, int32_t count, const float * value) __asm__("moderngl_glUniform3fv");
extern void glUniform3iv(int32_t location, int32_t count, const int32_t * value) __asm__("moderngl_glUniform3iv");
extern void glUniform3uiv(int32_t location, int32_t count, const uint32_t * value) __asm__("moderngl_glUniform3uiv");
extern void glUniform4fv(int32_t location, int32_t count, const float * value) __asm__("moderngl_glUniform4fv");
extern void glUniform4iv(int32_t location, int32_t count, const int32_t * value) __asm__("moderngl_glUniform4iv");
extern void glUniform4uiv(int32_t location, int32_t count, const uint32_t * value) __asm__("moderngl_glUniform4uiv");
extern void glUniformBlockBinding(uint32_t program, uint32_t index, uint32_t binding) __asm__("moderngl_glUniformBlockBinding");
extern void glUniformMatrix2fv(int32_t location, int32_t count, uint8_t transpose, const float * value) __asm__("moderngl_glUniformMatrix2fv");
extern void glUniformMatrix2x3fv(int32_t location, int32_t count, uint8_t transpose, const float * value) __asm__("moderngl_glUniformMatrix2x3fv");
extern void glUniformMatrix2x4fv(int32_t location, int32_t count, uint8_t transpose, const float * value) __asm__("moderngl_glUniformMatrix2x4fv");
extern void glUniformMatrix3fv(int32_t location, int32_t count, uint8_t transpose, const float * value) __asm__("moderngl_glUniformMatrix3fv");
extern void glUniformMatrix3x2fv(int32_t location, int32_t count, uint8_t transpose, const float * value) __asm__("moderngl_glUniformMatrix3x2fv");
extern void glUniformMatrix3x4fv(int32_t location, int32_t count, uint8_t transpose, const float * value) __asm__("moderngl_glUniformMatrix3x4fv");
extern void glUniformMatrix4fv(int32_t location, int32_t count, uint8_t transpose, const float * value) __asm__("moderngl_glUniformMatrix4fv");
extern void glUniformMatrix4x2fv(int32_t location, int32_t count, uint8_t transpose, const float * value) __asm__("moderngl_glUniformMatrix4x2fv");
extern void glUniformMatrix4x3fv(int32_t location, int32_t count, uint8_t transpose, const float * value) __asm__("moderngl_glUniformMatrix4x3fv");
extern void glUseProgram(uint32_t program) __asm__("moderngl_glUseProgram");
extern void glVertexAttribDivisor(uint32_t index, uint32_t divisor) __asm__("moderngl_glVertexAttribDivisor");
extern void glVertexAttribIPointer(uint32_t index, int32_t size, uint32_t type, int32_t stride, const void * pointer) __asm__("moderngl_glVertexAttribIPointer");
extern void glVertexAttribPointer(uint32_t index, int32_t size, uint32_t type, uint8_t normalized, int32_t stride, const void * pointer) __asm__("moderngl_glVertexAttribPointer");
extern void glViewport(int32_t x, int32_t y, int32_t width, int32_t height) __asm__("moderngl_glViewport");

static int load_gl(PyObject * loader) {
    gl_loaded = 1;
    return 1;
}

static PyObject * meth_get_proc_address(PyObject * self, PyObject * arg) {
    return PyLong_FromLong(0);
}

static void * default_loader_load(const char * name) {
    return NULL;
}

static int headless_create(void ** handles) {
    PyErr_Format(PyExc_RuntimeError, "headless contexts are not available in the browser");
    return 0;
}

static void headless_release(void ** handles) {
}

#else

static void GLAPI gl_trap(void) {
    Py_FatalError("OpenGL called from a thread without a moderngl context");
}

static THREAD_LOCAL void (GLAPI * glActiveTexture)(uint32_t texture) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glAttachShader)(uint32_t program, uint32_t shader) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glBindBuffer)(uint32_t target, uint32_t buffer) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glBindBufferBase)(uint32_t target, uint32_t index, uint32_t buffer) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glBindBufferRange)(uint32_t target, uint32_t index, uint32_t buffer, intptr_t offset, intptr_t size) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glBindFramebuffer)(uint32_t target, uint32_t framebuffer) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glBindRenderbuffer)(uint32_t target, uint32_t renderbuffer) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glBindSampler)(uint32_t unit, uint32_t sampler) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glBindTexture)(uint32_t target, uint32_t texture) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glBindVertexArray)(uint32_t array) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glBlendEquationSeparate)(uint32_t mode_rgb, uint32_t mode_alpha) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glBlendFuncSeparate)(uint32_t src_rgb, uint32_t dst_rgb, uint32_t src_alpha, uint32_t dst_alpha) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glBlitFramebuffer)(int32_t src_x0, int32_t src_y0, int32_t src_x1, int32_t src_y1, int32_t dst_x0, int32_t dst_y0, int32_t dst_x1, int32_t dst_y1, uint32_t mask, uint32_t filter) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glBufferData)(uint32_t target, intptr_t size, const void * data, uint32_t usage) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glBufferSubData)(uint32_t target, intptr_t offset, intptr_t size, const void * data) = (void *)gl_trap;
static THREAD_LOCAL uint32_t (GLAPI * glCheckFramebufferStatus)(uint32_t target) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glClearBufferfi)(uint32_t buffer, int32_t drawbuffer, float depth, int32_t stencil) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glClearBufferfv)(uint32_t buffer, int32_t drawbuffer, const float * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glClearBufferiv)(uint32_t buffer, int32_t drawbuffer, const int32_t * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glClearBufferuiv)(uint32_t buffer, int32_t drawbuffer, const uint32_t * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glCompileShader)(uint32_t shader) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glCopyBufferSubData)(uint32_t read_target, uint32_t write_target, intptr_t read_offset, intptr_t write_offset, intptr_t size) = (void *)gl_trap;
static THREAD_LOCAL uint32_t (GLAPI * glCreateProgram)(void) = (void *)gl_trap;
static THREAD_LOCAL uint32_t (GLAPI * glCreateShader)(uint32_t type) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glCullFace)(uint32_t mode) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDeleteBuffers)(int32_t n, const uint32_t * buffers) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDeleteFramebuffers)(int32_t n, const uint32_t * framebuffers) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDeleteProgram)(uint32_t program) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDeleteRenderbuffers)(int32_t n, const uint32_t * renderbuffers) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDeleteSamplers)(int32_t n, const uint32_t * samplers) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDeleteShader)(uint32_t shader) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDeleteTextures)(int32_t n, const uint32_t * textures) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDeleteVertexArrays)(int32_t n, const uint32_t * arrays) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDepthFunc)(uint32_t func) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDepthMask)(uint8_t flag) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDetachShader)(uint32_t program, uint32_t shader) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDisable)(uint32_t cap) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDrawArraysInstanced)(uint32_t mode, int32_t first, int32_t count, int32_t instance_count) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDrawBuffers)(int32_t n, const uint32_t * bufs) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDrawElementsInstanced)(uint32_t mode, int32_t count, uint32_t type, const void * indices, int32_t instance_count) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glEnable)(uint32_t cap) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glEnableVertexAttribArray)(uint32_t index) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glFlush)(void) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glFramebufferRenderbuffer)(uint32_t target, uint32_t attachment, uint32_t renderbuffer_target, uint32_t renderbuffer) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glFramebufferTexture2D)(uint32_t target, uint32_t attachment, uint32_t texture_target, uint32_t texture, int32_t level) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glFramebufferTextureLayer)(uint32_t target, uint32_t attachment, uint32_t texture, int32_t level, int32_t layer) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGenBuffers)(int32_t n, uint32_t * buffers) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGenFramebuffers)(int32_t n, uint32_t * framebuffers) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGenRenderbuffers)(int32_t n, uint32_t * renderbuffers) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGenSamplers)(int32_t n, uint32_t * samplers) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGenTextures)(int32_t n, uint32_t * textures) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGenVertexArrays)(int32_t n, uint32_t * arrays) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGenerateMipmap)(uint32_t target) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGetActiveUniform)(uint32_t program, uint32_t index, int32_t buf_size, int32_t * length, int32_t * size, uint32_t * type, char * name) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGetActiveUniformBlockName)(uint32_t program, uint32_t index, int32_t buf_size, int32_t * length, char * name) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGetActiveUniformBlockiv)(uint32_t program, uint32_t index, uint32_t pname, int32_t * params) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGetBufferParameteriv)(uint32_t target, uint32_t pname, int32_t * params) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGetBufferSubData)(uint32_t target, intptr_t offset, intptr_t size, void * data) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGetIntegerv)(uint32_t pname, int32_t * data) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGetProgramInfoLog)(uint32_t program, int32_t buf_size, int32_t * length, char * log) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGetProgramiv)(uint32_t program, uint32_t pname, int32_t * params) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGetShaderInfoLog)(uint32_t shader, int32_t buf_size, int32_t * length, char * log) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGetShaderiv)(uint32_t shader, uint32_t pname, int32_t * params) = (void *)gl_trap;
static THREAD_LOCAL const uint8_t * (GLAPI * glGetString)(uint32_t name) = (void *)gl_trap;
static THREAD_LOCAL uint32_t (GLAPI * glGetUniformBlockIndex)(uint32_t program, const char * name) = (void *)gl_trap;
static THREAD_LOCAL int32_t (GLAPI * glGetUniformLocation)(uint32_t program, const char * name) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGetUniformfv)(uint32_t program, int32_t location, float * params) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGetUniformiv)(uint32_t program, int32_t location, int32_t * params) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glGetUniformuiv)(uint32_t program, int32_t location, uint32_t * params) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glLinkProgram)(uint32_t program) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glPrimitiveRestartIndex)(uint32_t index) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glReadBuffer)(uint32_t src) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glReadPixels)(int32_t x, int32_t y, int32_t width, int32_t height, uint32_t format, uint32_t type, void * pixels) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glRenderbufferStorageMultisample)(uint32_t target, int32_t samples, uint32_t internal_format, int32_t width, int32_t height) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glSamplerParameterf)(uint32_t sampler, uint32_t pname, float param) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glSamplerParameteri)(uint32_t sampler, uint32_t pname, int32_t param) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glShaderSource)(uint32_t shader, int32_t count, const char * const * string, const int32_t * length) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glStencilFunc)(uint32_t func, int32_t ref, uint32_t mask) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glStencilFuncSeparate)(uint32_t face, uint32_t func, int32_t ref, uint32_t mask) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glStencilMask)(uint32_t mask) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glStencilOp)(uint32_t fail, uint32_t depth_fail, uint32_t depth_pass) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glStencilOpSeparate)(uint32_t face, uint32_t fail, uint32_t depth_fail, uint32_t depth_pass) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glTexImage2D)(uint32_t target, int32_t level, int32_t internal_format, int32_t width, int32_t height, int32_t border, uint32_t format, uint32_t type, const void * pixels) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glTexImage3D)(uint32_t target, int32_t level, int32_t internal_format, int32_t width, int32_t height, int32_t depth, int32_t border, uint32_t format, uint32_t type, const void * pixels) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glTexParameteri)(uint32_t target, uint32_t pname, int32_t param) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glTexSubImage2D)(uint32_t target, int32_t level, int32_t x, int32_t y, int32_t width, int32_t height, uint32_t format, uint32_t type, const void * pixels) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glTexSubImage3D)(uint32_t target, int32_t level, int32_t x, int32_t y, int32_t z, int32_t width, int32_t height, int32_t depth, uint32_t format, uint32_t type, const void * pixels) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniform1fv)(int32_t location, int32_t count, const float * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniform1i)(int32_t location, int32_t value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniform1iv)(int32_t location, int32_t count, const int32_t * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniform1uiv)(int32_t location, int32_t count, const uint32_t * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniform2fv)(int32_t location, int32_t count, const float * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniform2iv)(int32_t location, int32_t count, const int32_t * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniform2uiv)(int32_t location, int32_t count, const uint32_t * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniform3fv)(int32_t location, int32_t count, const float * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniform3iv)(int32_t location, int32_t count, const int32_t * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniform3uiv)(int32_t location, int32_t count, const uint32_t * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniform4fv)(int32_t location, int32_t count, const float * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniform4iv)(int32_t location, int32_t count, const int32_t * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniform4uiv)(int32_t location, int32_t count, const uint32_t * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniformBlockBinding)(uint32_t program, uint32_t index, uint32_t binding) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniformMatrix2fv)(int32_t location, int32_t count, uint8_t transpose, const float * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniformMatrix2x3fv)(int32_t location, int32_t count, uint8_t transpose, const float * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniformMatrix2x4fv)(int32_t location, int32_t count, uint8_t transpose, const float * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniformMatrix3fv)(int32_t location, int32_t count, uint8_t transpose, const float * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniformMatrix3x2fv)(int32_t location, int32_t count, uint8_t transpose, const float * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniformMatrix3x4fv)(int32_t location, int32_t count, uint8_t transpose, const float * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniformMatrix4fv)(int32_t location, int32_t count, uint8_t transpose, const float * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniformMatrix4x2fv)(int32_t location, int32_t count, uint8_t transpose, const float * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUniformMatrix4x3fv)(int32_t location, int32_t count, uint8_t transpose, const float * value) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glUseProgram)(uint32_t program) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glVertexAttribDivisor)(uint32_t index, uint32_t divisor) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glVertexAttribIPointer)(uint32_t index, int32_t size, uint32_t type, int32_t stride, const void * pointer) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glVertexAttribPointer)(uint32_t index, int32_t size, uint32_t type, uint8_t normalized, int32_t stride, const void * pointer) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glViewport)(int32_t x, int32_t y, int32_t width, int32_t height) = (void *)gl_trap;

typedef struct GLFunction {
    const char * name;
    void ** ptr;
} GLFunction;

static void * gl_library;
static THREAD_LOCAL PyObject * gl_loader;
static void * (GLAPI * get_proc_address)(const char * name);

#ifdef _WIN32

static int default_loader_init(void) {
    gl_library = (void *)LoadLibraryA("opengl32.dll");
    if (!gl_library) {
        PyErr_Format(PyExc_RuntimeError, "cannot load opengl32.dll");
        return 0;
    }
    void * (GLAPI * get_current_context)(void) = NULL;
    *(void **)&get_current_context = (void *)GetProcAddress((HMODULE)gl_library, "wglGetCurrentContext");
    *(void **)&get_proc_address = (void *)GetProcAddress((HMODULE)gl_library, "wglGetProcAddress");
    if (!get_current_context || !get_proc_address || !get_current_context()) {
        PyErr_Format(PyExc_RuntimeError, "no current OpenGL context");
        return 0;
    }
    return 1;
}

static void * default_loader_load(const char * name) {
    void * res = get_proc_address(name);
    if ((intptr_t)res >= -1 && (intptr_t)res <= 3) {
        res = (void *)GetProcAddress((HMODULE)gl_library, name);
    }
    return res;
}

static int headless_create(void ** handles) {
    HMODULE opengl32 = LoadLibraryA("opengl32.dll");
    if (!opengl32) {
        PyErr_Format(PyExc_RuntimeError, "cannot load opengl32.dll");
        return 0;
    }
    HGLRC (WINAPI * wglCreateContext)(HDC hdc) = NULL;
    BOOL (WINAPI * wglMakeCurrent)(HDC hdc, HGLRC context) = NULL;
    BOOL (WINAPI * wglDeleteContext)(HGLRC context) = NULL;
    PROC (WINAPI * wglGetProcAddress)(LPCSTR name) = NULL;
    HGLRC (WINAPI * wglCreateContextAttribsARB)(HDC hdc, HGLRC share, const int * attribs) = NULL;
    *(void **)&wglCreateContext = (void *)GetProcAddress(opengl32, "wglCreateContext");
    *(void **)&wglMakeCurrent = (void *)GetProcAddress(opengl32, "wglMakeCurrent");
    *(void **)&wglDeleteContext = (void *)GetProcAddress(opengl32, "wglDeleteContext");
    *(void **)&wglGetProcAddress = (void *)GetProcAddress(opengl32, "wglGetProcAddress");

    WNDCLASSA wc = {0};
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = GetModuleHandleA(NULL);
    wc.lpszClassName = "moderngl_headless";
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA("moderngl_headless", NULL, 0, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
    if (!hwnd) {
        PyErr_Format(PyExc_RuntimeError, "cannot create a hidden window");
        return 0;
    }
    HDC hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    SetPixelFormat(hdc, ChoosePixelFormat(hdc, &pfd), &pfd);

    HGLRC temp = wglCreateContext(hdc);
    if (!temp || !wglMakeCurrent(hdc, temp)) {
        PyErr_Format(PyExc_RuntimeError, "cannot create an OpenGL context");
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return 0;
    }

    *(void **)&wglCreateContextAttribsARB = (void *)wglGetProcAddress("wglCreateContextAttribsARB");
    const int attribs[] = {0x2091, 3, 0x2092, 3, 0x9126, 1, 0};
    HGLRC context = wglCreateContextAttribsARB ? wglCreateContextAttribsARB(hdc, NULL, attribs) : NULL;
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(temp);

    if (!context || !wglMakeCurrent(hdc, context)) {
        PyErr_Format(PyExc_RuntimeError, "cannot create an OpenGL 3.3 core context");
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return 0;
    }

    handles[0] = (void *)opengl32;
    handles[1] = (void *)hwnd;
    handles[2] = (void *)hdc;
    handles[3] = (void *)context;
    return default_loader_init();
}

static void headless_release(void ** handles) {
    BOOL (WINAPI * wglMakeCurrent)(HDC hdc, HGLRC context) = NULL;
    BOOL (WINAPI * wglDeleteContext)(HGLRC context) = NULL;
    *(void **)&wglMakeCurrent = (void *)GetProcAddress((HMODULE)handles[0], "wglMakeCurrent");
    *(void **)&wglDeleteContext = (void *)GetProcAddress((HMODULE)handles[0], "wglDeleteContext");
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext((HGLRC)handles[3]);
    ReleaseDC((HWND)handles[1], (HDC)handles[2]);
    DestroyWindow((HWND)handles[1]);
}

#elif defined(__APPLE__)

static int default_loader_init(void) {
    gl_library = dlopen("/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_LAZY | RTLD_LOCAL);
    if (!gl_library) {
        PyErr_Format(PyExc_RuntimeError, "cannot load the OpenGL framework");
        return 0;
    }
    void * (* get_current_context)(void) = NULL;
    *(void **)&get_current_context = dlsym(gl_library, "CGLGetCurrentContext");
    if (!get_current_context || !get_current_context()) {
        PyErr_Format(PyExc_RuntimeError, "no current OpenGL context");
        return 0;
    }
    return 1;
}

static void * default_loader_load(const char * name) {
    return dlsym(gl_library, name);
}

static int headless_create(void ** handles) {
    void * cgl = dlopen("/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_LAZY | RTLD_LOCAL);
    if (!cgl) {
        PyErr_Format(PyExc_RuntimeError, "cannot load the OpenGL framework");
        return 0;
    }
    int (* CGLChoosePixelFormat)(const int * attribs, void ** pixel_format, int * count) = NULL;
    int (* CGLCreateContext)(void * pixel_format, void * share, void ** context) = NULL;
    int (* CGLDestroyPixelFormat)(void * pixel_format) = NULL;
    int (* CGLSetCurrentContext)(void * context) = NULL;
    *(void **)&CGLChoosePixelFormat = dlsym(cgl, "CGLChoosePixelFormat");
    *(void **)&CGLCreateContext = dlsym(cgl, "CGLCreateContext");
    *(void **)&CGLDestroyPixelFormat = dlsym(cgl, "CGLDestroyPixelFormat");
    *(void **)&CGLSetCurrentContext = dlsym(cgl, "CGLSetCurrentContext");

    const int attribs[] = {99, 0x3200, 0};
    void * pixel_format = NULL;
    void * context = NULL;
    int count = 0;
    if (CGLChoosePixelFormat(attribs, &pixel_format, &count) || !pixel_format) {
        PyErr_Format(PyExc_RuntimeError, "cannot choose an OpenGL 3.3 core pixel format");
        return 0;
    }
    int error = CGLCreateContext(pixel_format, NULL, &context);
    CGLDestroyPixelFormat(pixel_format);
    if (error || CGLSetCurrentContext(context)) {
        PyErr_Format(PyExc_RuntimeError, "cannot create an OpenGL 3.3 core context");
        return 0;
    }

    handles[0] = cgl;
    handles[1] = context;
    return default_loader_init();
}

static void headless_release(void ** handles) {
    int (* CGLSetCurrentContext)(void * context) = NULL;
    int (* CGLDestroyContext)(void * context) = NULL;
    *(void **)&CGLSetCurrentContext = dlsym(handles[0], "CGLSetCurrentContext");
    *(void **)&CGLDestroyContext = dlsym(handles[0], "CGLDestroyContext");
    CGLSetCurrentContext(NULL);
    CGLDestroyContext(handles[1]);
}

#else

static int default_loader_init(void) {
    void * (* get_current_context)(void) = NULL;
    void * egl = dlopen("libEGL.so.1", RTLD_LAZY | RTLD_LOCAL);
    if (egl) {
        *(void **)&get_current_context = dlsym(egl, "eglGetCurrentContext");
        if (get_current_context && get_current_context()) {
            gl_library = egl;
            *(void **)&get_proc_address = dlsym(egl, "eglGetProcAddress");
            return 1;
        }
        dlclose(egl);
    }
    gl_library = dlopen("libGL.so.1", RTLD_LAZY | RTLD_LOCAL);
    if (!gl_library) {
        PyErr_Format(PyExc_RuntimeError, "cannot load libGL.so.1");
        return 0;
    }
    *(void **)&get_current_context = dlsym(gl_library, "glXGetCurrentContext");
    *(void **)&get_proc_address = dlsym(gl_library, "glXGetProcAddressARB");
    if (!get_current_context || !get_proc_address || !get_current_context()) {
        PyErr_Format(PyExc_RuntimeError, "no current OpenGL context");
        return 0;
    }
    return 1;
}

static void * default_loader_load(const char * name) {
    void * res = get_proc_address(name);
    if (!res) {
        res = dlsym(gl_library, name);
    }
    return res;
}


static int headless_create(void ** handles) {
    void * egl = dlopen("libEGL.so.1", RTLD_LAZY | RTLD_LOCAL);
    if (!egl) {
        PyErr_Format(PyExc_RuntimeError, "cannot load libEGL.so.1");
        return 0;
    }
    void * (* eglGetProcAddress)(const char * name) = NULL;
    void * (* eglGetDisplay)(void * native_display) = NULL;
    uint32_t (* eglInitialize)(void * display, int32_t * major, int32_t * minor) = NULL;
    uint32_t (* eglBindAPI)(uint32_t api) = NULL;
    uint32_t (* eglChooseConfig)(void * display, const int32_t * attribs, void ** configs, int32_t size, int32_t * count) = NULL;
    void * (* eglCreateContext)(void * display, void * config, void * share, const int32_t * attribs) = NULL;
    uint32_t (* eglMakeCurrent)(void * display, void * draw, void * read, void * context) = NULL;
    void * (* eglGetPlatformDisplayEXT)(uint32_t platform, void * native_display, const int32_t * attribs) = NULL;
    *(void **)&eglGetProcAddress = dlsym(egl, "eglGetProcAddress");
    *(void **)&eglGetDisplay = dlsym(egl, "eglGetDisplay");
    *(void **)&eglInitialize = dlsym(egl, "eglInitialize");
    *(void **)&eglBindAPI = dlsym(egl, "eglBindAPI");
    *(void **)&eglChooseConfig = dlsym(egl, "eglChooseConfig");
    *(void **)&eglCreateContext = dlsym(egl, "eglCreateContext");
    *(void **)&eglMakeCurrent = dlsym(egl, "eglMakeCurrent");
    *(void **)&eglGetPlatformDisplayEXT = eglGetProcAddress("eglGetPlatformDisplayEXT");

    void * display = eglGetPlatformDisplayEXT ? eglGetPlatformDisplayEXT(0x31DD, NULL, NULL) : NULL;
    if (!display || !eglInitialize(display, NULL, NULL)) {
        display = eglGetDisplay(NULL);
    }
    if (!display || !eglInitialize(display, NULL, NULL)) {
        PyErr_Format(PyExc_RuntimeError, "cannot initialize an EGL display");
        return 0;
    }

    const int32_t config_attribs[] = {0x3033, 0, 0x3040, 0x0008, 0x3038};
    const int32_t context_attribs[] = {0x3098, 3, 0x30FB, 3, 0x30FD, 1, 0x3038};
    void * config = NULL;
    int32_t count = 0;
    eglBindAPI(0x30A2);
    if (!eglChooseConfig(display, config_attribs, &config, 1, &count) || !count) {
        PyErr_Format(PyExc_RuntimeError, "cannot choose an EGL config");
        return 0;
    }
    void * context = eglCreateContext(display, config, NULL, context_attribs);
    if (!context || !eglMakeCurrent(display, NULL, NULL, context)) {
        PyErr_Format(PyExc_RuntimeError, "cannot create an OpenGL 3.3 core context");
        return 0;
    }

    handles[0] = egl;
    handles[1] = display;
    handles[2] = context;
    return default_loader_init();
}

static void headless_release(void ** handles) {
    uint32_t (* eglMakeCurrent)(void * display, void * draw, void * read, void * context) = NULL;
    uint32_t (* eglDestroyContext)(void * display, void * context) = NULL;
    *(void **)&eglMakeCurrent = dlsym(handles[0], "eglMakeCurrent");
    *(void **)&eglDestroyContext = dlsym(handles[0], "eglDestroyContext");
    eglMakeCurrent(handles[1], NULL, NULL, NULL);
    eglDestroyContext(handles[1], handles[2]);
}

#endif

static int load_gl(PyObject * loader) {
    const GLFunction gl_functions[] = {
        {"glActiveTexture", (void **)&glActiveTexture},
        {"glAttachShader", (void **)&glAttachShader},
        {"glBindBuffer", (void **)&glBindBuffer},
        {"glBindBufferBase", (void **)&glBindBufferBase},
        {"glBindBufferRange", (void **)&glBindBufferRange},
        {"glBindFramebuffer", (void **)&glBindFramebuffer},
        {"glBindRenderbuffer", (void **)&glBindRenderbuffer},
        {"glBindSampler", (void **)&glBindSampler},
        {"glBindTexture", (void **)&glBindTexture},
        {"glBindVertexArray", (void **)&glBindVertexArray},
        {"glBlendEquationSeparate", (void **)&glBlendEquationSeparate},
        {"glBlendFuncSeparate", (void **)&glBlendFuncSeparate},
        {"glBlitFramebuffer", (void **)&glBlitFramebuffer},
        {"glBufferData", (void **)&glBufferData},
        {"glBufferSubData", (void **)&glBufferSubData},
        {"glCheckFramebufferStatus", (void **)&glCheckFramebufferStatus},
        {"glClearBufferfi", (void **)&glClearBufferfi},
        {"glClearBufferfv", (void **)&glClearBufferfv},
        {"glClearBufferiv", (void **)&glClearBufferiv},
        {"glClearBufferuiv", (void **)&glClearBufferuiv},
        {"glCompileShader", (void **)&glCompileShader},
        {"glCopyBufferSubData", (void **)&glCopyBufferSubData},
        {"glCreateProgram", (void **)&glCreateProgram},
        {"glCreateShader", (void **)&glCreateShader},
        {"glCullFace", (void **)&glCullFace},
        {"glDeleteBuffers", (void **)&glDeleteBuffers},
        {"glDeleteFramebuffers", (void **)&glDeleteFramebuffers},
        {"glDeleteProgram", (void **)&glDeleteProgram},
        {"glDeleteRenderbuffers", (void **)&glDeleteRenderbuffers},
        {"glDeleteSamplers", (void **)&glDeleteSamplers},
        {"glDeleteShader", (void **)&glDeleteShader},
        {"glDeleteTextures", (void **)&glDeleteTextures},
        {"glDeleteVertexArrays", (void **)&glDeleteVertexArrays},
        {"glDepthFunc", (void **)&glDepthFunc},
        {"glDepthMask", (void **)&glDepthMask},
        {"glDetachShader", (void **)&glDetachShader},
        {"glDisable", (void **)&glDisable},
        {"glDrawArraysInstanced", (void **)&glDrawArraysInstanced},
        {"glDrawBuffers", (void **)&glDrawBuffers},
        {"glDrawElementsInstanced", (void **)&glDrawElementsInstanced},
        {"glEnable", (void **)&glEnable},
        {"glEnableVertexAttribArray", (void **)&glEnableVertexAttribArray},
        {"glFlush", (void **)&glFlush},
        {"glFramebufferRenderbuffer", (void **)&glFramebufferRenderbuffer},
        {"glFramebufferTexture2D", (void **)&glFramebufferTexture2D},
        {"glFramebufferTextureLayer", (void **)&glFramebufferTextureLayer},
        {"glGenBuffers", (void **)&glGenBuffers},
        {"glGenFramebuffers", (void **)&glGenFramebuffers},
        {"glGenRenderbuffers", (void **)&glGenRenderbuffers},
        {"glGenSamplers", (void **)&glGenSamplers},
        {"glGenTextures", (void **)&glGenTextures},
        {"glGenVertexArrays", (void **)&glGenVertexArrays},
        {"glGenerateMipmap", (void **)&glGenerateMipmap},
        {"glGetActiveUniform", (void **)&glGetActiveUniform},
        {"glGetActiveUniformBlockName", (void **)&glGetActiveUniformBlockName},
        {"glGetActiveUniformBlockiv", (void **)&glGetActiveUniformBlockiv},
        {"glGetBufferParameteriv", (void **)&glGetBufferParameteriv},
        {"glGetBufferSubData", (void **)&glGetBufferSubData},
        {"glGetIntegerv", (void **)&glGetIntegerv},
        {"glGetProgramInfoLog", (void **)&glGetProgramInfoLog},
        {"glGetProgramiv", (void **)&glGetProgramiv},
        {"glGetShaderInfoLog", (void **)&glGetShaderInfoLog},
        {"glGetShaderiv", (void **)&glGetShaderiv},
        {"glGetString", (void **)&glGetString},
        {"glGetUniformBlockIndex", (void **)&glGetUniformBlockIndex},
        {"glGetUniformLocation", (void **)&glGetUniformLocation},
        {"glGetUniformfv", (void **)&glGetUniformfv},
        {"glGetUniformiv", (void **)&glGetUniformiv},
        {"glGetUniformuiv", (void **)&glGetUniformuiv},
        {"glLinkProgram", (void **)&glLinkProgram},
        {"glPrimitiveRestartIndex", (void **)&glPrimitiveRestartIndex},
        {"glReadBuffer", (void **)&glReadBuffer},
        {"glReadPixels", (void **)&glReadPixels},
        {"glRenderbufferStorageMultisample", (void **)&glRenderbufferStorageMultisample},
        {"glSamplerParameterf", (void **)&glSamplerParameterf},
        {"glSamplerParameteri", (void **)&glSamplerParameteri},
        {"glShaderSource", (void **)&glShaderSource},
        {"glStencilFunc", (void **)&glStencilFunc},
        {"glStencilFuncSeparate", (void **)&glStencilFuncSeparate},
        {"glStencilMask", (void **)&glStencilMask},
        {"glStencilOp", (void **)&glStencilOp},
        {"glStencilOpSeparate", (void **)&glStencilOpSeparate},
        {"glTexImage2D", (void **)&glTexImage2D},
        {"glTexImage3D", (void **)&glTexImage3D},
        {"glTexParameteri", (void **)&glTexParameteri},
        {"glTexSubImage2D", (void **)&glTexSubImage2D},
        {"glTexSubImage3D", (void **)&glTexSubImage3D},
        {"glUniform1fv", (void **)&glUniform1fv},
        {"glUniform1i", (void **)&glUniform1i},
        {"glUniform1iv", (void **)&glUniform1iv},
        {"glUniform1uiv", (void **)&glUniform1uiv},
        {"glUniform2fv", (void **)&glUniform2fv},
        {"glUniform2iv", (void **)&glUniform2iv},
        {"glUniform2uiv", (void **)&glUniform2uiv},
        {"glUniform3fv", (void **)&glUniform3fv},
        {"glUniform3iv", (void **)&glUniform3iv},
        {"glUniform3uiv", (void **)&glUniform3uiv},
        {"glUniform4fv", (void **)&glUniform4fv},
        {"glUniform4iv", (void **)&glUniform4iv},
        {"glUniform4uiv", (void **)&glUniform4uiv},
        {"glUniformBlockBinding", (void **)&glUniformBlockBinding},
        {"glUniformMatrix2fv", (void **)&glUniformMatrix2fv},
        {"glUniformMatrix2x3fv", (void **)&glUniformMatrix2x3fv},
        {"glUniformMatrix2x4fv", (void **)&glUniformMatrix2x4fv},
        {"glUniformMatrix3fv", (void **)&glUniformMatrix3fv},
        {"glUniformMatrix3x2fv", (void **)&glUniformMatrix3x2fv},
        {"glUniformMatrix3x4fv", (void **)&glUniformMatrix3x4fv},
        {"glUniformMatrix4fv", (void **)&glUniformMatrix4fv},
        {"glUniformMatrix4x2fv", (void **)&glUniformMatrix4x2fv},
        {"glUniformMatrix4x3fv", (void **)&glUniformMatrix4x3fv},
        {"glUseProgram", (void **)&glUseProgram},
        {"glVertexAttribDivisor", (void **)&glVertexAttribDivisor},
        {"glVertexAttribIPointer", (void **)&glVertexAttribIPointer},
        {"glVertexAttribPointer", (void **)&glVertexAttribPointer},
        {"glViewport", (void **)&glViewport},
        {NULL},
    };

    PyObject * load_function = NULL;

    if (loader == Py_None) {
        if (!default_loader_init()) {
            return 0;
        }
    } else {
        load_function = PyObject_GetAttrString(loader, "load_opengl_function");
        if (!load_function) {
            PyErr_Clear();
            PyErr_Format(PyExc_ValueError, "the loader must have a load_opengl_function(name) method");
            return 0;
        }
    }

    void * functions[sizeof(gl_functions) / sizeof(GLFunction)] = {0};
    PyObject * missing = PyList_New(0);

    for (int32_t i = 0; gl_functions[i].name; ++i) {
        const char * name = gl_functions[i].name;
        if (!load_function) {
            functions[i] = default_loader_load(name);
        } else {
            PyObject * res = PyObject_CallFunction(load_function, "s", name);
            if (!res) {
                Py_DECREF(load_function);
                Py_DECREF(missing);
                return 0;
            }
            int valid = PyLong_Check(res);
            functions[i] = valid ? PyLong_AsVoidPtr(res) : NULL;
            Py_DECREF(res);
            if (!valid || PyErr_Occurred()) {
                PyErr_Clear();
                PyErr_Format(PyExc_TypeError, "load_opengl_function must return an int");
                Py_DECREF(load_function);
                Py_DECREF(missing);
                return 0;
            }
        }
        if (!functions[i]) {
            PyObject * item = PyUnicode_FromString(name);
            PyList_Append(missing, item);
            Py_DECREF(item);
        }
    }

    Py_XDECREF(load_function);

    if (PyList_Size(missing)) {
        PyErr_Format(PyExc_RuntimeError, "cannot load OpenGL functions %R", missing);
        Py_DECREF(missing);
        return 0;
    }

    Py_DECREF(missing);

    for (int32_t i = 0; gl_functions[i].name; ++i) {
        *gl_functions[i].ptr = functions[i];
    }

    Py_XSETREF(gl_loader, loader == Py_None ? NULL : Py_NewRef(loader));
    gl_loaded = 1;
    return 1;
}

static PyObject * meth_get_proc_address(PyObject * self, PyObject * arg) {
    const char * name = PyUnicode_AsUTF8(arg);
    if (!name) {
        return NULL;
    }

    if (!gl_loaded) {
        PyErr_Format(PyExc_RuntimeError, "OpenGL is not loaded, call moderngl.init() or create the context first");
        return NULL;
    }

    if (gl_loader) {
        return PyObject_CallMethod(gl_loader, "load_opengl_function", "s", name);
    }

    return PyLong_FromVoidPtr(default_loader_load(name));
}

#endif

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

static const int32_t max_buffer_bindings = 8;
static const int32_t max_sampler_bindings = 16;

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
    PyTypeObject * HeadlessContext_type;
} ModuleState;

typedef struct HeadlessContext {
    PyObject_HEAD
    void * handles[4];
} HeadlessContext;

typedef struct Buffer {
    PyObject_HEAD
    GCHeader * gc_prev;
    GCHeader * gc_next;
    Context * ctx;
    PyObject * kind;
    uint32_t buffer;
    uint32_t target;
    int32_t size;
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
    int32_t samples;
    int32_t array;
    int8_t cubemap;
    int8_t renderbuffer;
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

typedef struct Resources {
    PyObject_HEAD
    Context * ctx;
    int32_t sampler_count;
    int32_t uniform_buffer_count;
    SamplerSlot samplers[16];
    BufferRange uniform_buffers[8];
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
    PyObject * uniforms;
    PyObject * uniform_index;
    PyObject * samplers;
    PyObject * uniform_buffers;
    UniformBinding * uniform_bindings;
    uint32_t program;
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
    int32_t samples;
    int32_t color_count;
    const ImageFormat * color_formats[8];
    const ImageFormat * depth_format;
    ClearValue clear_colors[8];
    float clear_depth;
    int32_t clear_stencil;
    Region viewport;
} Framebuffer;

typedef struct Pipeline {
    PyObject_HEAD
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
    uint32_t mode;
    int8_t has_viewport;
    Region viewport;
    int8_t depth_test;
    int8_t depth_write;
    uint32_t depth_func;
    uint32_t cull_face;
    BlendState blend;
    StencilState stencil;
} Pipeline;

struct Context {
    PyObject_HEAD
    GCHeader * gc_prev;
    GCHeader * gc_next;
    ModuleState * module_state;
    int32_t max_samples;
    int32_t max_vertex_attribs;
    PyObject * shaders;
    PyObject * info;
    PyObject * shader_cache;
    VertexArray * empty_vertex_array;
    uint32_t default_texture_unit;
    int32_t max_samplers;
    int32_t max_uniform_buffers;
    int32_t uniform_buffer_offset_alignment;
    int8_t depth_test;
    int8_t depth_write;
    uint32_t depth_func;
    uint32_t cull_face;
    uint32_t stencil_write_mask;
    Region viewport;
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
        args,
        kwargs,
        "|OO$OOOO",
        keywords,
        &src_arg,
        &dst_arg,
        &equation_arg,
        &src_alpha_arg,
        &dst_alpha_arg,
        &equation_alpha_arg
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

    if (equation_arg != Py_None && !parse_named_value(blend_equations, equation_arg, &res->equation, "blend equation")) {
        return 0;
    }

    res->src_alpha = res->src;
    res->dst_alpha = res->dst;
    res->equation_alpha = res->equation;

    if (src_alpha_arg != Py_None && !parse_named_value(blend_factors, src_alpha_arg, &res->src_alpha, "blend factor")) {
        return 0;
    }

    if (dst_alpha_arg != Py_None && !parse_named_value(blend_factors, dst_alpha_arg, &res->dst_alpha, "blend factor")) {
        return 0;
    }

    if (equation_alpha_arg != Py_None && !parse_named_value(blend_equations, equation_alpha_arg, &res->equation_alpha, "blend equation")) {
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
        "func",
        "ref",
        "read_mask",
        "write_mask",
        "fail",
        "depth_fail",
        "depth_pass",
        "back_func",
        "back_fail",
        "back_depth_fail",
        "back_depth_pass",
        NULL,
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
        args,
        kwargs,
        "|Oi$IIOOOOOOO",
        keywords,
        &func_arg,
        &res->ref,
        &res->read_mask,
        &res->write_mask,
        &fail_arg,
        &depth_fail_arg,
        &depth_pass_arg,
        &back_func_arg,
        &back_fail_arg,
        &back_depth_fail_arg,
        &back_depth_pass_arg
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

    if (fail_arg != Py_None && !parse_named_value(stencil_ops, fail_arg, &res->fail, "stencil op")) {
        return 0;
    }

    if (depth_fail_arg != Py_None && !parse_named_value(stencil_ops, depth_fail_arg, &res->depth_fail, "stencil op")) {
        return 0;
    }

    if (depth_pass_arg != Py_None && !parse_named_value(stencil_ops, depth_pass_arg, &res->depth_pass, "stencil op")) {
        return 0;
    }

    res->back_func = res->func;
    res->back_fail = res->fail;
    res->back_depth_fail = res->depth_fail;
    res->back_depth_pass = res->depth_pass;

    if (back_func_arg != Py_None && !parse_named_value(compare_funcs, back_func_arg, &res->back_func, "stencil func")) {
        return 0;
    }

    if (back_fail_arg != Py_None && !parse_named_value(stencil_ops, back_fail_arg, &res->back_fail, "stencil op")) {
        return 0;
    }

    if (back_depth_fail_arg != Py_None && !parse_named_value(stencil_ops, back_depth_fail_arg, &res->back_depth_fail, "stencil op")) {
        return 0;
    }

    if (back_depth_pass_arg != Py_None && !parse_named_value(stencil_ops, back_depth_pass_arg, &res->back_depth_pass, "stencil op")) {
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

static int parse_buffer_kind(const char * kind, uint32_t * target, const char ** default_usage) {
    if (!strcmp(kind, "vertex_buffer")) {
        *target = GL_ARRAY_BUFFER;
        *default_usage = "static_draw";
    } else if (!strcmp(kind, "index_buffer")) {
        *target = GL_ELEMENT_ARRAY_BUFFER;
        *default_usage = "static_draw";
    } else if (!strcmp(kind, "uniform_buffer")) {
        *target = GL_UNIFORM_BUFFER;
        *default_usage = "dynamic_draw";
    } else if (!strcmp(kind, "copy_buffer")) {
        *target = GL_COPY_WRITE_BUFFER;
        *default_usage = "dynamic_copy";
    } else {
        PyErr_Format(PyExc_ValueError, "invalid kind \"%s\", must be \"vertex_buffer\", \"index_buffer\", \"uniform_buffer\" or \"copy_buffer\"", kind);
        return 0;
    }
    return 1;
}

static int parse_buffer_range(Context * ctx, PyObject * obj, BufferRange * res) {
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
    if (res->buffer->target != GL_UNIFORM_BUFFER) {
        PyErr_Format(PyExc_TypeError, "only uniform buffers can be bound to uniform buffer slots");
        return 0;
    }
    if (res->offset < 0 || res->size <= 0 || res->offset + res->size > res->buffer->size) {
        PyErr_Format(PyExc_ValueError, "invalid size or offset");
        return 0;
    }
    if (res->offset % ctx->uniform_buffer_offset_alignment) {
        PyErr_Format(PyExc_ValueError, "the offset must be a multiple of %d", ctx->uniform_buffer_offset_alignment);
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
    if (texture->renderbuffer) {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, texture->texture);
    } else if (texture->cubemap) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, texture->texture, level);
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

static Framebuffer * new_framebuffer(Context * ctx, uint32_t framebuffer, PyObject * attachments, int32_t width, int32_t height, int32_t samples) {
    Framebuffer * res = PyObject_New(Framebuffer, ctx->module_state->Framebuffer_type);
    res->ctx = (Context *)Py_NewRef(ctx);
    res->attachments = attachments;
    res->size = Py_BuildValue("(ii)", width, height);
    res->framebuffer = framebuffer;
    res->width = width;
    res->height = height;
    res->samples = samples;
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

    const char * text = PyUnicode_AsUTF8(PyTuple_GetItem(key, 1));
    uint32_t shader = glCreateShader(type);
    glShaderSource(shader, 1, &text, NULL);
    glCompileShader(shader);

    int32_t compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        char log[4096] = {0};
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        glDeleteShader(shader);
        Py_DECREF(key);
        const char * title = type == GL_VERTEX_SHADER ? "Vertex Shader Error" : "Fragment Shader Error";
        PyErr_Format(PyExc_ValueError, "%s in \"%U\"\n\n%s", title, name, log);
        return 0;
    }

    PyObject * value = PyLong_FromUnsignedLong(shader);
    PyDict_SetItem(cache, key, value);
    Py_DECREF(value);
    Py_DECREF(key);
    return shader;
}

static uint32_t compile_program(PyObject * cache, PyObject * shaders, PyObject * vertex_shader, PyObject * fragment_shader) {
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

static PyObject * read_uniform_element(Program * self, UniformBinding * b, int32_t location) {
    if (location < 0) {
        Py_RETURN_NONE;
    }
    int32_t items = b->values / b->count;
    int32_t raw[16] = {0};
    if (b->type == 'f') {
        glGetUniformfv(self->program, location, (float *)raw);
    } else if (b->type == 'i') {
        glGetUniformiv(self->program, location, raw);
    } else {
        glGetUniformuiv(self->program, location, (uint32_t *)raw);
    }
    int boolean = b->gltype == GL_BOOL || b->gltype == GL_BOOL_VEC2 || b->gltype == GL_BOOL_VEC3 || b->gltype == GL_BOOL_VEC4;
    PyObject * values = PyTuple_New(items);
    for (int32_t i = 0; i < items; ++i) {
        PyObject * item = NULL;
        if (b->type == 'f') {
            item = PyFloat_FromDouble(((float *)raw)[i]);
        } else if (boolean) {
            item = PyBool_FromLong(raw[i]);
        } else if (b->type == 'i') {
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
    if (b->function < 12) {
        return values;
    }
    static const int32_t columns[] = {2, 2, 2, 3, 3, 3, 4, 4, 4};
    int32_t cols = columns[b->function - 12];
    int32_t rows = items / cols;
    PyObject * res = PyTuple_New(cols);
    for (int32_t i = 0; i < cols; ++i) {
        PyTuple_SetItem(res, i, PyTuple_GetSlice(values, i * rows, i * rows + rows));
    }
    Py_DECREF(values);
    return res;
}

static PyObject * read_uniforms(Program * self) {
    PyObject * res = PyDict_New();
    PyObject * key = NULL;
    PyObject * value = NULL;
    Py_ssize_t pos = 0;
    while (PyDict_Next(self->uniform_index, &pos, &key, &value)) {
        UniformBinding * b = &self->uniform_bindings[PyLong_AsLong(value)];
        if (!b->array) {
            PyObject * item = read_uniform_element(self, b, b->location);
            PyDict_SetItem(res, key, item);
            Py_DECREF(item);
            continue;
        }
        PyObject * list = PyList_New(b->count);
        for (int32_t i = 0; i < b->count; ++i) {
            char name[300] = {0};
            snprintf(name, sizeof(name), "%s[%d]", PyUnicode_AsUTF8(key), i);
            int32_t location = b->location < 0 ? -1 : glGetUniformLocation(self->program, name);
            PyList_SetItem(list, i, read_uniform_element(self, b, location));
        }
        PyDict_SetItem(res, key, list);
        Py_DECREF(list);
    }
    return res;
}

static PyObject * read_samplers(Program * self) {
    PyObject * res = PyDict_New();
    PyObject * key = NULL;
    PyObject * value = NULL;
    Py_ssize_t pos = 0;
    while (PyDict_Next(self->samplers, &pos, &key, &value)) {
        const char * name = PyUnicode_Check(key) ? PyUnicode_AsUTF8(key) : NULL;
        int32_t location = name ? glGetUniformLocation(self->program, name) : -1;
        int32_t unit = 0;
        PyObject * item = NULL;
        if (location >= 0) {
            glGetUniformiv(self->program, location, &unit);
            item = PyLong_FromLong(unit);
        } else {
            item = Py_NewRef(Py_None);
        }
        PyDict_SetItem(res, key, item);
        Py_DECREF(item);
    }
    return res;
}

static PyObject * read_uniform_buffers(Program * self) {
    PyObject * res = PyDict_New();
    PyObject * key = NULL;
    PyObject * value = NULL;
    Py_ssize_t pos = 0;
    while (PyDict_Next(self->uniform_buffers, &pos, &key, &value)) {
        const char * name = PyUnicode_Check(key) ? PyUnicode_AsUTF8(key) : NULL;
        uint32_t block = name ? glGetUniformBlockIndex(self->program, name) : GL_INVALID_INDEX;
        int32_t binding = 0;
        PyObject * item = NULL;
        if (block != GL_INVALID_INDEX) {
            glGetActiveUniformBlockiv(self->program, block, GL_UNIFORM_BLOCK_BINDING, &binding);
            item = PyLong_FromLong(binding);
        } else {
            item = Py_NewRef(Py_None);
        }
        PyDict_SetItem(res, key, item);
        Py_DECREF(item);
    }
    return res;
}

static int read_buffer(Buffer * self, PyObject * size_arg, int32_t offset, PyObject * memory, PyObject ** res) {
    if (offset < 0 || offset > self->size) {
        PyErr_Format(PyExc_ValueError, "invalid offset");
        return 0;
    }

    if (size_arg != Py_None && !PyLong_CheckExact(size_arg)) {
        PyErr_Format(PyExc_TypeError, "the size must be an int");
        return 0;
    }

    int32_t size = self->size - offset;
    if (size_arg != Py_None) {
        size = (int32_t)PyLong_AsLong(size_arg);
    }

    if (size < 0 || size + offset > self->size) {
        PyErr_Format(PyExc_ValueError, "invalid size");
        return 0;
    }

    Py_buffer view = {0};
    if (memory) {
        if (PyObject_GetBuffer(memory, &view, PyBUF_WRITABLE | PyBUF_C_CONTIGUOUS)) {
            return 0;
        }
        if (view.len < size) {
            PyBuffer_Release(&view);
            PyErr_Format(PyExc_ValueError, "the memory is too small, %d bytes required", size);
            return 0;
        }
    } else {
        *res = PyBytes_FromStringAndSize(NULL, size);
    }

    if (self->target == GL_ELEMENT_ARRAY_BUFFER) {
        glBindVertexArray(0);
    }

    glBindBuffer(self->target, self->buffer);
    glGetBufferSubData(self->target, (intptr_t)offset, (intptr_t)size, memory ? view.buf : PyBytes_AsString(*res));
    glBindBuffer(self->target, 0);

    if (self->target == GL_ELEMENT_ARRAY_BUFFER) {
        restore_vertex_array(self->ctx);
    }

    if (memory) {
        PyBuffer_Release(&view);
    }

    return 1;
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

    PyObject * res = NULL;
    if (!read_buffer(self, size_arg, offset, NULL, &res)) {
        return NULL;
    }
    return res;
}

static PyObject * Buffer_meth_read_into(Buffer * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"memory", "size", "offset", NULL};

    PyObject * memory = NULL;
    PyObject * size_arg = Py_None;
    int32_t offset = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|Oi", keywords, &memory, &size_arg, &offset)) {
        return NULL;
    }

    PyObject * res = NULL;
    if (!read_buffer(self, size_arg, offset, memory, &res)) {
        return NULL;
    }
    Py_RETURN_NONE;
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

    if (self->target == GL_UNIFORM_BUFFER && offset % self->ctx->uniform_buffer_offset_alignment) {
        PyErr_Format(PyExc_ValueError, "the offset must be a multiple of %d", self->ctx->uniform_buffer_offset_alignment);
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

    if (self->target != GL_UNIFORM_BUFFER) {
        PyErr_Format(PyExc_TypeError, "only uniform buffers can be used");
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

static int read_texture(Texture * self, PyObject * size_arg, PyObject * offset_arg, int32_t layer, int32_t level, PyObject * memory, PyObject ** res) {
    if (self->samples > 1) {
        PyErr_Format(PyExc_TypeError, "cannot read multisampled textures");
        return 0;
    }

    if (!self->fmt->color) {
        PyErr_Format(PyExc_TypeError, "cannot read depth or stencil textures");
        return 0;
    }

    if (layer < 0 || layer >= self->layer_count || level < 0 || level >= self->level_count) {
        PyErr_Format(PyExc_ValueError, "invalid layer or level");
        return 0;
    }

    Region region = {0};
    if (!parse_region(self, level, size_arg, offset_arg, &region)) {
        return 0;
    }

    int32_t size = region_bytes(self, &region);

    Py_buffer view = {0};
    if (memory) {
        if (PyObject_GetBuffer(memory, &view, PyBUF_WRITABLE | PyBUF_C_CONTIGUOUS)) {
            return 0;
        }
        if (view.len < size) {
            PyBuffer_Release(&view);
            PyErr_Format(PyExc_ValueError, "the memory is too small, %d bytes required", size);
            return 0;
        }
    } else {
        *res = PyBytes_FromStringAndSize(NULL, size);
        memset(PyBytes_AsString(*res), 0, size);
    }

    int temporary = self->array || self->cubemap || level;
    uint32_t framebuffer = temporary ? face_framebuffer(self, layer, level) : texture_framebuffer(self);
    if (!framebuffer) {
        if (memory) {
            PyBuffer_Release(&view);
        } else {
            Py_CLEAR(*res);
        }
        return 0;
    }

    void * ptr = memory ? view.buf : PyBytes_AsString(*res);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glReadPixels(region.x, region.y, region.width, region.height, self->fmt->format, self->fmt->type, ptr);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    if (temporary) {
        glDeleteFramebuffers(1, &framebuffer);
    }

    if (memory) {
        PyBuffer_Release(&view);
    }

    return 1;
}

static PyObject * Texture_meth_write(Texture * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"data", "size", "offset", "layer", "level", NULL};

    PyObject * data = NULL;
    PyObject * size_arg = Py_None;
    PyObject * offset_arg = Py_None;
    PyObject * layer_arg = Py_None;
    int32_t level = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OOOi", keywords, &data, &size_arg, &offset_arg, &layer_arg, &level)) {
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

    if (self->renderbuffer) {
        PyErr_Format(PyExc_TypeError, "cannot write to renderbuffers");
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

    PyObject * res = NULL;
    if (!read_texture(self, size_arg, offset_arg, layer, level, NULL, &res)) {
        return NULL;
    }
    return res;
}

static PyObject * Texture_meth_read_into(Texture * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"memory", "size", "offset", "layer", "level", NULL};

    PyObject * memory = NULL;
    PyObject * size_arg = Py_None;
    PyObject * offset_arg = Py_None;
    int32_t layer = 0;
    int32_t level = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OOii", keywords, &memory, &size_arg, &offset_arg, &layer, &level)) {
        return NULL;
    }

    PyObject * res = NULL;
    if (!read_texture(self, size_arg, offset_arg, layer, level, memory, &res)) {
        return NULL;
    }
    Py_RETURN_NONE;
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
    if (self->renderbuffer) {
        PyErr_Format(PyExc_TypeError, "cannot build mipmaps for renderbuffers");
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
        glBindBufferRange(GL_UNIFORM_BUFFER, range->binding, range->buffer->buffer, (intptr_t)range->offset, (intptr_t)range->size);
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

static PyObject * Resources_get_uniform_buffers(Resources * self, void * closure) {
    PyObject * res = PyDict_New();
    for (int32_t i = 0; i < self->uniform_buffer_count; ++i) {
        BufferRange * range = &self->uniform_buffers[i];
        PyObject * binding = PyLong_FromLong(range->binding);
        PyObject * view = Py_BuildValue("(Oii)", range->buffer, range->offset, range->size);
        PyDict_SetItem(res, binding, view);
        Py_DECREF(binding);
        Py_DECREF(view);
    }
    return res;
}

static PyObject * draw_vertex_array(VertexArray * self, uint32_t mode, int32_t first, int32_t vertices, int32_t instances) {
    if (!self->ctx->bound_program) {
        PyErr_Format(PyExc_RuntimeError, "no program is bound, call program.use() first");
        return NULL;
    }

    if (vertices < 0 || instances < 0 || first < 0) {
        PyErr_Format(PyExc_ValueError, "vertices, instances and first must not be negative");
        return NULL;
    }

    if (!self->index_type) {
        glDrawArraysInstanced(mode, first, vertices, instances);
        Py_RETURN_NONE;
    }

    uint32_t restart_index = self->index_size == 1 ? 0xFF : self->index_size == 2 ? 0xFFFF : 0xFFFFFFFF;
    void * offset = (void *)(intptr_t)(first * self->index_size);
    glPrimitiveRestartIndex(restart_index);
    glDrawElementsInstanced(mode, vertices, self->index_type, offset, instances);
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
    static char * keywords[] = {"mode", "vertices", "instances", "first", NULL};

    const char * mode_arg = NULL;
    PyObject * vertices_arg = Py_None;
    int32_t instances = 1;
    int32_t first = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|$zOii", keywords, &mode_arg, &vertices_arg, &instances, &first)) {
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

    return draw_vertex_array(self, mode, first, vertices, instances);
}

static PyObject * VertexArray_meth_draw(VertexArray * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"vertices", "first", "instances", "mode", NULL};

    int32_t vertices = 0;
    int32_t first = 0;
    int32_t instances = 1;
    const char * mode_arg = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "i|i$iz", keywords, &vertices, &first, &instances, &mode_arg)) {
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

    return draw_vertex_array(self, mode, first, vertices, instances);
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


static PyObject * Program_meth_debug(Program * self, PyObject * args) {
    PyObject * uniforms = read_uniforms(self);
    PyObject * samplers = read_samplers(self);
    PyObject * uniform_buffers = read_uniform_buffers(self);
    PyObject * res = Py_BuildValue("{sOsOsO}", "uniforms", uniforms, "samplers", samplers, "uniform_buffers", uniform_buffers);
    Py_DECREF(uniforms);
    Py_DECREF(samplers);
    Py_DECREF(uniform_buffers);
    return res;
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
    int32_t indices[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    if (attachments != Py_None && !parse_clear_attachments(attachments, slot_count, indices, &count)) {
        return NULL;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, self->framebuffer);
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

    ClearValue clear_colors[8] = {0};

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

static PyObject * Framebuffer_get_size(Framebuffer * self, void * closure) {
    return Py_NewRef(self->size);
}

static int Framebuffer_set_size(Framebuffer * self, PyObject * value, void * closure) {
    if (PyTuple_Size(self->attachments)) {
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

static PyObject * Pipeline_meth_run(Pipeline * self, PyObject * args) {
    Context * ctx = self->ctx;

    bind_framebuffer(ctx, self->framebuffer);
    Region full = {0, 0, self->framebuffer->width, self->framebuffer->height};
    apply_viewport(ctx, self->has_viewport ? &self->viewport : &full);
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

    PyObject * res = draw_vertex_array(self->vertex_array, self->mode, self->first, self->vertices, self->instances);
    apply_viewport(ctx, &self->framebuffer->viewport);
    return res;
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
    return pack_uniform(&self->program->uniform_bindings[i], key, value, self->uniform_data + self->uniform_offsets[i]) ? 0 : -1;
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
    headless_release(self->handles);
    memset(self->handles, 0, sizeof(self->handles));
    Py_RETURN_NONE;
}

static PyObject * Context_meth_release(Context * self, PyObject * arg);
static PyObject * Context_get_screen(Context * self, void * closure);

static Buffer * Context_meth_buffer(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"data", "size", "kind", "usage", NULL};

    PyObject * data = Py_None;
    PyObject * size_arg = Py_None;
    const char * kind = "vertex_buffer";
    const char * usage_arg = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O$Osz", keywords, &data, &size_arg, &kind, &usage_arg)) {
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

    uint32_t target = 0;
    const char * default_usage = NULL;

    if (!parse_buffer_kind(kind, &target, &default_usage)) {
        return NULL;
    }

    if (!usage_arg) {
        usage_arg = default_usage;
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
    res->kind = PyUnicode_InternFromString(kind);
    res->size = size;

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
    static char * keywords[] = {"size", "format", "data", "samples", "array", "levels", "renderbuffer", "cubemap", NULL};

    int32_t width = 0;
    int32_t height = 0;
    const char * format = "rgba8unorm";
    PyObject * data = Py_None;
    int32_t samples = 1;
    int32_t array = 0;
    int32_t levels = 1;
    PyObject * renderbuffer_arg = Py_None;
    int cubemap = 0;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args,
        kwargs,
        "(ii)|sO$iiiOp",
        keywords,
        &width,
        &height,
        &format,
        &data,
        &samples,
        &array,
        &levels,
        &renderbuffer_arg,
        &cubemap
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

    if (renderbuffer_arg != Py_True && renderbuffer_arg != Py_False && renderbuffer_arg != Py_None) {
        PyErr_Format(PyExc_TypeError, "renderbuffer must be True, False or None");
        return NULL;
    }
    if (samples > 1 && renderbuffer_arg == Py_False) {
        PyErr_Format(PyExc_TypeError, "multisampled textures must be renderbuffers");
        return NULL;
    }
    if (samples < 1 || (samples & (samples - 1)) || samples > 16) {
        PyErr_Format(PyExc_ValueError, "samples must be 1, 2, 4, 8 or 16");
        return NULL;
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
    if (samples > 1 && (array || cubemap)) {
        PyErr_Format(PyExc_TypeError, "multisampled array or cubemap textures are not supported");
        return NULL;
    }
    if (renderbuffer_arg == Py_True && (array || cubemap)) {
        PyErr_Format(PyExc_TypeError, "array or cubemap textures cannot be renderbuffers");
        return NULL;
    }
    if (data != Py_None && samples > 1) {
        PyErr_Format(PyExc_ValueError, "cannot write to multisampled textures");
        return NULL;
    }
    if (data != Py_None && renderbuffer_arg == Py_True) {
        PyErr_Format(PyExc_ValueError, "cannot write to renderbuffers");
        return NULL;
    }

    int8_t renderbuffer = samples > 1 || renderbuffer_arg == Py_True;
    uint32_t target = cubemap ? GL_TEXTURE_CUBE_MAP : array ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;

    if (samples > self->max_samples) {
        samples = self->max_samples;
    }

    uint32_t image = 0;
    if (renderbuffer) {
        glGenRenderbuffers(1, &image);
        glBindRenderbuffer(GL_RENDERBUFFER, image);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples > 1 ? samples : 0, fmt->internal_format, width, height);
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
    res->samples = samples;
    res->array = array;
    res->cubemap = (int8_t)cubemap;
    res->renderbuffer = renderbuffer;
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
        "texture",
        "min_filter",
        "mag_filter",
        "wrap_x",
        "wrap_y",
        "wrap_z",
        "min_lod",
        "max_lod",
        "lod_bias",
        "compare_func",
        "anisotropy",
        NULL,
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
        args,
        kwargs,
        "O!|$zssssfffzf",
        keywords,
        self->module_state->Texture_type,
        &texture,
        &min_filter_arg,
        &mag_filter_arg,
        &wrap_x_arg,
        &wrap_y_arg,
        &wrap_z_arg,
        &min_lod,
        &max_lod,
        &lod_bias,
        &compare_func_arg,
        &anisotropy
    );

    if (!args_ok) {
        return NULL;
    }

    if (texture->renderbuffer) {
        PyErr_Format(PyExc_TypeError, "renderbuffers cannot be sampled");
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
    static char * keywords[] = {"samplers", "uniform_buffers", NULL};

    PyObject * samplers = NULL;
    PyObject * uniform_buffers = NULL;

    PyObject * empty = PyTuple_New(0);
    int args_ok = PyArg_ParseTupleAndKeywords(empty, kwargs, "|$O!O!", keywords, &PyDict_Type, &samplers, &PyDict_Type, &uniform_buffers);
    Py_DECREF(empty);

    if (!args_ok) {
        return NULL;
    }

    Sampler * sampler_slots[16] = {0};
    BufferRange buffer_slots[8] = {0};

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
        if (!parse_buffer_range(self, value, &range)) {
            return NULL;
        }
        range.binding = binding;
        buffer_slots[binding] = range;
    }

    Resources * res = PyObject_New(Resources, self->module_state->Resources_type);
    res->ctx = (Context *)Py_NewRef(self);
    res->sampler_count = 0;
    res->uniform_buffer_count = 0;

    for (int32_t i = 0; i < 16; ++i) {
        if (sampler_slots[i]) {
            res->samplers[res->sampler_count].unit = i;
            res->samplers[res->sampler_count].sampler = (Sampler *)Py_NewRef(sampler_slots[i]);
            res->sampler_count += 1;
        }
    }

    for (int32_t i = 0; i < 8; ++i) {
        if (buffer_slots[i].buffer) {
            res->uniform_buffers[res->uniform_buffer_count] = buffer_slots[i];
            Py_INCREF(buffer_slots[i].buffer);
            res->uniform_buffer_count += 1;
        }
    }

    return res;
}

static VertexArray * Context_meth_vertex_array(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"attributes", "index_buffer", "index_size", "vertices", "mode", NULL};

    PyObject * attributes_arg = NULL;
    PyObject * index_buffer = Py_None;
    int32_t index_size = 4;
    PyObject * vertices_arg = Py_None;
    const char * mode_arg = "triangles";

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O$iOs", keywords, &attributes_arg, &index_buffer, &index_size, &vertices_arg, &mode_arg)) {
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

    if (index_buffer != Py_None && ((Buffer *)index_buffer)->target != GL_ELEMENT_ARRAY_BUFFER) {
        PyErr_Format(PyExc_TypeError, "the index buffer must be an index_buffer");
        return NULL;
    }

    if (index_size != 1 && index_size != 2 && index_size != 4) {
        PyErr_Format(PyExc_ValueError, "the index size must be 1, 2 or 4");
        return NULL;
    }

    PyObject * attributes = PySequence_Tuple(attributes_arg);
    if (!attributes) {
        return NULL;
    }

    int32_t count = (int32_t)PyTuple_Size(attributes);

    if (count > 64) {
        PyErr_Format(PyExc_ValueError, "too many attributes");
        Py_DECREF(attributes);
        return NULL;
    }

    Attribute parsed[64] = {0};
    uint8_t used[64] = {0};
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

        if (!PyTuple_Check(item) || !PyArg_ParseTuple(item, "O!isiii", self->module_state->Buffer_type, &a->buffer, &a->location, &format, &a->offset, &a->stride, &a->divisor)) {
            PyErr_Clear();
            PyErr_Format(PyExc_TypeError, "attribute %d must be a (buffer, location, format, offset, stride, divisor) tuple", i);
            Py_DECREF(attributes);
            return NULL;
        }

        a->fmt = get_vertex_format(format);

        if (a->buffer->target != GL_ARRAY_BUFFER) {
            PyErr_Format(PyExc_TypeError, "attribute %d buffer must be a vertex_buffer", i);
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
            glVertexAttribIPointer(a->location, a->fmt->components, a->fmt->type, a->stride, (void *)(intptr_t)a->offset);
        } else {
            glVertexAttribPointer(a->location, a->fmt->components, a->fmt->type, a->fmt->normalized, a->stride, (void *)(intptr_t)a->offset);
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
        index_type = index_size == 1 ? GL_UNSIGNED_BYTE : index_size == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
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

static Program * Context_meth_program(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"vertex_shader", "fragment_shader", "samplers", "uniform_buffers", "uniforms", NULL};

    PyObject * vertex_shader = NULL;
    PyObject * fragment_shader = NULL;
    PyObject * samplers = NULL;
    PyObject * uniform_buffers = NULL;
    PyObject * declared = NULL;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args,
        kwargs,
        "UU|$O!O!O!",
        keywords,
        &vertex_shader,
        &fragment_shader,
        &PyDict_Type,
        &samplers,
        &PyDict_Type,
        &uniform_buffers,
        &PyDict_Type,
        &declared
    );

    if (!args_ok) {
        return NULL;
    }

    int32_t uniform_count = declared ? (int32_t)PyDict_Size(declared) : 0;
    UniformBinding * bindings = (UniformBinding *)PyMem_Malloc(sizeof(UniformBinding) * (uniform_count + 1));
    PyObject * uniforms = PyDict_New();
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
        program = compile_program(self->shader_cache, self->shaders, vertex_shader, fragment_shader);
    }

    if (!program) {
        PyMem_Free(bindings);
        Py_DECREF(uniforms);
        Py_DECREF(uniform_index);
        return NULL;
    }

    glUseProgram(program);

    pos = 0;
    index = 0;
    while (declared && PyDict_Next(declared, &pos, &key, &value)) {
        UniformBinding * b = &bindings[index];
        b->location = glGetUniformLocation(program, PyUnicode_AsUTF8(key));
        PyObject * location = PyLong_FromLong(b->location);
        PyDict_SetItem(uniforms, key, location);
        Py_DECREF(location);
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

    glUseProgram(bound_program_object(self));

    if (PyErr_Occurred()) {
        PyMem_Free(bindings);
        Py_DECREF(uniforms);
        Py_DECREF(uniform_index);
        glDeleteProgram(program);
        return NULL;
    }

    Program * res = PyObject_New(Program, self->module_state->Program_type);
    res->gc_prev = NULL;
    res->gc_next = NULL;
    res->ctx = (Context *)Py_NewRef(self);
    res->uniforms = uniforms;
    res->uniform_index = uniform_index;
    res->samplers = samplers ? PyDict_Copy(samplers) : PyDict_New();
    res->uniform_buffers = uniform_buffers ? PyDict_Copy(uniform_buffers) : PyDict_New();
    res->uniform_bindings = bindings;
    res->program = program;

    track_object(self, (GCHeader *)res);
    return res;
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
    int32_t samples = 0;
    int32_t color_count = 0;

    for (int32_t i = 0; i < count; ++i) {
        PyObject * item = PyTuple_GetItem(seq, i);
        Texture * texture = NULL;
        int32_t layer = 0;
        int32_t level = 0;

        if (Py_TYPE(item) == self->module_state->Texture_type) {
            texture = (Texture *)item;
            if (texture->array || texture->cubemap) {
                PyErr_Format(PyExc_TypeError, "attachment %d is a layered texture, use texture.attachment()", i);
                Py_DECREF(attachments);
                Py_DECREF(seq);
                return NULL;
            }
        } else if (PyTuple_Check(item) && PyTuple_Size(item) == 3 && Py_TYPE(PyTuple_GetItem(item, 0)) == self->module_state->Texture_type) {
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
            samples = texture->samples;
        }

        if (level_width != width || level_height != height) {
            PyErr_Format(PyExc_ValueError, "attachment %d has a different size", i);
            Py_DECREF(attachments);
            Py_DECREF(seq);
            return NULL;
        }

        if (texture->samples != samples) {
            PyErr_Format(PyExc_ValueError, "attachment %d has a different sample count", i);
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

    if (color_count > 8) {
        Py_DECREF(attachments);
        PyErr_Format(PyExc_ValueError, "too many color attachments");
        return NULL;
    }

    uint32_t framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    Framebuffer * res = new_framebuffer(self, framebuffer, attachments, width, height, samples);

    uint32_t draw_buffers[8] = {0};

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

static Pipeline * Context_meth_pipeline(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {
        "vertex_shader",
        "fragment_shader",
        "samplers",
        "uniform_buffers",
        "uniforms",
        "vertex_array",
        "framebuffer",
        "resources",
        "vertices",
        "instances",
        "first",
        "mode",
        "viewport",
        "depth_test",
        "depth_write",
        "depth_func",
        "cull_face",
        "blend",
        "stencil",
        NULL,
    };

    PyObject * vertex_shader = NULL;
    PyObject * fragment_shader = NULL;
    PyObject * samplers = Py_None;
    PyObject * uniform_buffers = Py_None;
    PyObject * uniforms = Py_None;
    VertexArray * vertex_array = NULL;
    Framebuffer * framebuffer = NULL;
    PyObject * resources_arg = NULL;
    PyObject * vertices_arg = Py_None;
    int32_t instances = 1;
    int32_t first = 0;
    const char * mode_arg = NULL;
    PyObject * viewport_arg = Py_None;
    int depth_test = 0;
    int depth_write = 1;
    const char * depth_func_arg = "less";
    const char * cull_face_arg = NULL;
    PyObject * blend_arg = Py_None;
    PyObject * stencil_arg = Py_None;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args,
        kwargs,
        "UU|$OOOO!O!OOiizOppszOO",
        keywords,
        &vertex_shader,
        &fragment_shader,
        &samplers,
        &uniform_buffers,
        &uniforms,
        self->module_state->VertexArray_type,
        &vertex_array,
        self->module_state->Framebuffer_type,
        &framebuffer,
        &resources_arg,
        &vertices_arg,
        &instances,
        &first,
        &mode_arg,
        &viewport_arg,
        &depth_test,
        &depth_write,
        &depth_func_arg,
        &cull_face_arg,
        &blend_arg,
        &stencil_arg
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

    if (vertices < 0 || instances < 0 || first < 0) {
        PyErr_Format(PyExc_ValueError, "vertices, instances and first must not be negative");
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

    if (resources_arg && resources_arg != Py_None && Py_TYPE(resources_arg) != self->module_state->Resources_type) {
        PyErr_Format(PyExc_TypeError, "resources must be a Resources or None, combine them with ctx.resources(a, b)");
        return NULL;
    }

    PyObject * empty_args = PyTuple_New(0);
    PyObject * resources = resources_arg && resources_arg != Py_None ? Py_NewRef(resources_arg) : (PyObject *)Context_meth_resources(self, empty_args, NULL);
    Py_DECREF(empty_args);
    Resources * merged = (Resources *)resources;

    int8_t sampler_units[16] = {0};
    int8_t buffer_bindings[8] = {0};

    for (int32_t j = 0; j < merged->sampler_count; ++j) {
        sampler_units[merged->samplers[j].unit] = 1;
    }
    for (int32_t j = 0; j < merged->uniform_buffer_count; ++j) {
        buffer_bindings[merged->uniform_buffers[j].binding] = 1;
    }

    PyObject * program_args = PyTuple_Pack(2, vertex_shader, fragment_shader);
    PyObject * program_kwargs = PyDict_New();
    if (samplers != Py_None) {
        PyDict_SetItemString(program_kwargs, "samplers", samplers);
    }
    if (uniform_buffers != Py_None) {
        PyDict_SetItemString(program_kwargs, "uniform_buffers", uniform_buffers);
    }
    if (uniforms != Py_None) {
        PyDict_SetItemString(program_kwargs, "uniforms", uniforms);
    }
    Program * program = Context_meth_program(self, program_args, program_kwargs);
    Py_DECREF(program_args);
    Py_DECREF(program_kwargs);

    if (!program) {
        Py_DECREF(resources);
        return NULL;
    }

    PyObject * key = NULL;
    PyObject * value = NULL;
    Py_ssize_t pos = 0;

    while (PyDict_Next(program->samplers, &pos, &key, &value)) {
        const char * name = PyUnicode_Check(key) ? PyUnicode_AsUTF8(key) : NULL;
        if (name && glGetUniformLocation(program->program, name) >= 0 && !sampler_units[PyLong_AsLong(value)]) {
            PyErr_Format(PyExc_ValueError, "sampler \"%s\" has no resource at unit %d", name, (int32_t)PyLong_AsLong(value));
            break;
        }
    }

    pos = 0;
    while (!PyErr_Occurred() && PyDict_Next(program->uniform_buffers, &pos, &key, &value)) {
        const char * name = PyUnicode_Check(key) ? PyUnicode_AsUTF8(key) : NULL;
        if (name && glGetUniformBlockIndex(program->program, name) != GL_INVALID_INDEX && !buffer_bindings[PyLong_AsLong(value)]) {
            PyErr_Format(PyExc_ValueError, "uniform block \"%s\" has no resource at binding %d", name, (int32_t)PyLong_AsLong(value));
            break;
        }
    }

    if (PyErr_Occurred()) {
        Py_DECREF(Context_meth_release(self, (PyObject *)program));
        Py_DECREF(program);
        Py_DECREF(resources);
        return NULL;
    }

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
    res->ctx = (Context *)Py_NewRef(self);
    res->program = program;
    res->vertex_array = (VertexArray *)Py_NewRef(vertex_array);
    res->framebuffer = (Framebuffer *)Py_NewRef(framebuffer);
    res->resources = resources;
    res->uniform_data = uniform_data;
    res->uniform_offsets = uniform_offsets;
    res->uniform_count = uniform_count;
    res->vertices = vertices;
    res->instances = instances;
    res->first = first;
    res->mode = mode;
    res->has_viewport = 0;
    res->depth_test = (int8_t)depth_test;
    res->depth_write = (int8_t)depth_write;
    res->depth_func = depth_func;
    res->cull_face = cull_face;
    res->blend = blend;
    res->stencil = stencil;

    if (Pipeline_set_viewport(res, viewport_arg, NULL)) {
        Py_DECREF(res);
        return NULL;
    }

    return res;
}

static Buffer * Context_meth_external_buffer(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"buffer", "size", "kind", NULL};

    uint32_t buffer = 0;
    PyObject * size_arg = Py_None;
    const char * kind = "vertex_buffer";

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "I|O$s", keywords, &buffer, &size_arg, &kind)) {
        return NULL;
    }

    if (size_arg != Py_None && !PyLong_CheckExact(size_arg)) {
        PyErr_Format(PyExc_TypeError, "the size must be an int or None");
        return NULL;
    }

    uint32_t target = 0;
    const char * default_usage = NULL;

    if (!parse_buffer_kind(kind, &target, &default_usage)) {
        return NULL;
    }

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
    res->kind = PyUnicode_InternFromString(kind);
    res->size = size;
    return res;
}

static Texture * Context_meth_external_texture(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"texture", "size", "format", "samples", "array", "levels", "renderbuffer", "cubemap", NULL};

    uint32_t image = 0;
    int32_t width = 0;
    int32_t height = 0;
    const char * format = "rgba8unorm";
    int32_t samples = 1;
    int32_t array = 0;
    int32_t levels = 1;
    int renderbuffer = 0;
    int cubemap = 0;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args,
        kwargs,
        "I(ii)|s$iiipp",
        keywords,
        &image,
        &width,
        &height,
        &format,
        &samples,
        &array,
        &levels,
        &renderbuffer,
        &cubemap
    );

    if (!args_ok) {
        return NULL;
    }

    const ImageFormat * fmt = get_image_format(format);
    if (!fmt) {
        PyErr_Format(PyExc_ValueError, "invalid texture format \"%s\"", format);
        return NULL;
    }

    if (width <= 0 || height <= 0 || samples < 1 || array < 0 || levels < 1 || (cubemap && array)) {
        PyErr_Format(PyExc_ValueError, "invalid size, samples, array, levels or cubemap");
        return NULL;
    }

    if (samples > 1 && !renderbuffer) {
        PyErr_Format(PyExc_TypeError, "multisampled textures must be renderbuffers");
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
    res->samples = samples;
    res->array = array;
    res->cubemap = (int8_t)cubemap;
    res->renderbuffer = (int8_t)renderbuffer;
    res->layer_count = (array ? array : 1) * (cubemap ? 6 : 1);
    res->level_count = levels;
    return res;
}

static Framebuffer * create_external_framebuffer(Context * self, uint32_t framebuffer, int32_t width, int32_t height, PyObject * formats, int32_t samples) {
    PyObject * seq = PySequence_Tuple(formats);
    if (!seq) {
        return NULL;
    }

    int32_t count = (int32_t)PyTuple_Size(seq);

    if (count == 0 || count > 9) {
        Py_DECREF(seq);
        PyErr_Format(PyExc_ValueError, "invalid number of formats");
        return NULL;
    }

    if (width <= 0 || height <= 0) {
        Py_DECREF(seq);
        PyErr_Format(PyExc_ValueError, "invalid size");
        return NULL;
    }

    const ImageFormat * slot_formats[9] = {0};
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

    if (color_count > 8) {
        PyErr_Format(PyExc_ValueError, "too many color formats");
        return NULL;
    }

    PyObject * attachments = PyTuple_New(0);
    Framebuffer * res = new_framebuffer(self, framebuffer, attachments, width, height, samples);

    for (int32_t i = 0; i < count; ++i) {
        add_slot(res, slot_formats[i]);
    }

    return res;
}

static Framebuffer * Context_meth_external_framebuffer(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"framebuffer", "size", "formats", "samples", NULL};

    uint32_t framebuffer = 0;
    int32_t width = 0;
    int32_t height = 0;
    PyObject * formats = NULL;
    int32_t samples = 1;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "I(ii)|O$i", keywords, &framebuffer, &width, &height, &formats, &samples)) {
        return NULL;
    }

    if (!formats) {
        formats = Py_BuildValue("(ss)", "rgba8unorm", "depth24plus-stencil8");
        Framebuffer * res = create_external_framebuffer(self, framebuffer, width, height, formats, samples);
        Py_DECREF(formats);
        return res;
    }

    return create_external_framebuffer(self, framebuffer, width, height, formats, samples);
}

static PyObject * Context_meth_copy_buffer(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"src", "dst", "size", "src_offset", "dst_offset", NULL};

    Buffer * src = NULL;
    Buffer * dst = NULL;
    PyObject * size_arg = Py_None;
    int32_t src_offset = 0;
    int32_t dst_offset = 0;

    int args_ok = PyArg_ParseTupleAndKeywords(
        args,
        kwargs,
        "O!O!|O$ii",
        keywords,
        self->module_state->Buffer_type,
        &src,
        self->module_state->Buffer_type,
        &dst,
        &size_arg,
        &src_offset,
        &dst_offset
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

    if (src_offset < 0 || dst_offset < 0 || size <= 0 || src_offset + size > src->size || dst_offset + size > dst->size) {
        PyErr_Format(PyExc_ValueError, "invalid size or offset");
        return NULL;
    }

    glBindBuffer(GL_COPY_READ_BUFFER, src->buffer);
    glBindBuffer(GL_COPY_WRITE_BUFFER, dst->buffer);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, (intptr_t)src_offset, (intptr_t)dst_offset, (intptr_t)size);
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
        args,
        kwargs,
        "O!O|O$OOOsp",
        keywords,
        self->module_state->Texture_type,
        &src,
        &dst,
        &size_arg,
        &src_offset_arg,
        &dst_offset_arg,
        &dst_size_arg,
        &filter_arg,
        &flip_y
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
    int32_t dst_samples = dst_texture ? dst_texture->samples : dst_framebuffer->samples;
    int32_t dst_width = dst_texture ? dst_texture->width : dst_framebuffer->width;
    int32_t dst_height = dst_texture ? dst_texture->height : dst_framebuffer->height;

    if (dst_texture == src) {
        PyErr_Format(PyExc_ValueError, "src and dst are the same");
        return NULL;
    }

    if (dst_samples > 1) {
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

    if (src->samples > 1 && src->fmt != dst_fmt) {
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

    if (dst_texture && (dst_region.x < 0 || dst_region.y < 0 || dst_region.x + dst_region.width > dst_width || dst_region.y + dst_region.height > dst_height)) {
        PyErr_Format(PyExc_ValueError, "invalid size or offset");
        return NULL;
    }

    int scaled = dst_region.width != src_region.width || dst_region.height != src_region.height;

    if (src->samples > 1 && (scaled || flip_y || src_region.x != dst_region.x || src_region.y != dst_region.y)) {
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
    glBlitFramebuffer(sx0, sy0, sx1, sy1, dx0, dy0, dx1, dy1, mask, filter);
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
        args,
        kwargs,
        "O!O!|O$iOii",
        keywords,
        self->module_state->Buffer_type,
        &src,
        self->module_state->Texture_type,
        &dst,
        &size_arg,
        &src_offset,
        &dst_offset_arg,
        &dst_layer,
        &dst_level
    );

    if (!args_ok) {
        return NULL;
    }

    if (src->target == GL_ELEMENT_ARRAY_BUFFER) {
        PyErr_Format(PyExc_TypeError, "index buffers cannot be copied to textures");
        return NULL;
    }

    if (dst->renderbuffer) {
        PyErr_Format(PyExc_TypeError, "cannot copy to renderbuffers");
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
        glTexSubImage3D(dst->target, dst_level, region.x, region.y, dst_layer, region.width, region.height, 1, fmt->format, fmt->type, ptr);
    } else {
        glTexSubImage2D(dst->target, dst_level, region.x, region.y, region.width, region.height, fmt->format, fmt->type, ptr);
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
        args,
        kwargs,
        "O!O!|O$Oi",
        keywords,
        self->module_state->Texture_type,
        &src,
        self->module_state->Buffer_type,
        &dst,
        &size_arg,
        &src_offset_arg,
        &dst_offset
    );

    if (!args_ok) {
        return NULL;
    }

    if (dst->target == GL_ELEMENT_ARRAY_BUFFER) {
        PyErr_Format(PyExc_TypeError, "textures cannot be copied to index buffers");
        return NULL;
    }

    if (src->samples > 1) {
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
    glEnable(GL_PRIMITIVE_RESTART);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    Py_CLEAR(self->bound_program);
    Py_CLEAR(self->bound_vertex_array);
    Py_XSETREF(self->bound_framebuffer, (Framebuffer *)Context_get_screen(self, NULL));

    glUseProgram(0);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    self->viewport = self->bound_framebuffer->viewport;
    glViewport(self->viewport.x, self->viewport.y, self->viewport.width, self->viewport.height);
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

static PyObject * Context_meth_release(Context * self, PyObject * arg) {
    ModuleState * module_state = self->module_state;

    if (PyUnicode_CheckExact(arg) && PyUnicode_EqualToUTF8(arg, "all")) {
        GCHeader * it = self->gc_next;
        while (it != (GCHeader *)self) {
            GCHeader * next = it->gc_next;
            Py_DECREF(Context_meth_release(self, (PyObject *)it));
            it = next;
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
            glDeleteBuffers(1, &buffer->buffer);
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
        if (untrack_object(self, (GCHeader *)texture)) {
            if (texture->renderbuffer) {
                glDeleteRenderbuffers(1, &texture->texture);
            } else {
                glDeleteTextures(1, &texture->texture);
            }
            Py_DECREF(texture);
        }
        Py_RETURN_NONE;
    }

    if (type == module_state->Framebuffer_type) {
        Framebuffer * framebuffer = (Framebuffer *)arg;
        if (untrack_object(self, (GCHeader *)framebuffer)) {
            if (self->bound_framebuffer == framebuffer) {
                Py_XSETREF(self->bound_framebuffer, (Framebuffer *)Context_get_screen(self, NULL));
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                apply_viewport(self, &self->bound_framebuffer->viewport);
            }
            glDeleteFramebuffers(1, &framebuffer->framebuffer);
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
            Py_DECREF(vertex_array);
        }
        Py_RETURN_NONE;
    }

    if (type == module_state->Sampler_type) {
        Sampler * sampler = (Sampler *)arg;
        if (untrack_object(self, (GCHeader *)sampler)) {
            glDeleteSamplers(1, &sampler->sampler);
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
            Py_DECREF(program);
        }
        Py_RETURN_NONE;
    }

    if (type == module_state->Pipeline_type) {
        return Context_meth_release(self, (PyObject *)((Pipeline *)arg)->program);
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

static PyObject * Context_get_screen(Context * self, void * closure) {
    ModuleState * module_state = self->module_state;

    if (module_state->screen) {
        return Py_NewRef(module_state->screen);
    }

    int32_t viewport[4] = {0};
    glGetIntegerv(GL_VIEWPORT, viewport);

    PyObject * formats = Py_BuildValue("(ss)", "rgba8unorm", "depth24plus-stencil8");
    Framebuffer * res = create_external_framebuffer(self, 0, least_one(viewport[2]), least_one(viewport[3]), formats, 1);
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
        res->max_samples = get_limit(GL_MAX_SAMPLES, 1, 16);
        res->max_vertex_attribs = get_limit(GL_MAX_VERTEX_ATTRIBS, 8, 64);
        res->shaders = PyDict_New();
        res->shader_cache = PyDict_New();
        res->empty_vertex_array = NULL;
        glEnable(GL_PRIMITIVE_RESTART);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        int32_t max_texture_units = get_limit(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 2, 1024);
        res->default_texture_unit = GL_TEXTURE0 + max_texture_units - 1;
        res->max_samplers = max_texture_units - 1 < max_sampler_bindings ? max_texture_units - 1 : max_sampler_bindings;
        res->max_uniform_buffers = get_limit(GL_MAX_UNIFORM_BUFFER_BINDINGS, 1, max_buffer_bindings);
        res->uniform_buffer_offset_alignment = 1;
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &res->uniform_buffer_offset_alignment);
        res->info = Py_BuildValue(
            "{szszszszsisisisisisi}",
            "vendor", (const char *)glGetString(GL_VENDOR),
            "renderer", (const char *)glGetString(GL_RENDERER),
            "version", (const char *)glGetString(GL_VERSION),
            "glsl", (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION),
            "max_samples", res->max_samples,
            "max_vertex_attribs", res->max_vertex_attribs,
            "max_samplers", res->max_samplers,
            "max_uniform_buffers", res->max_uniform_buffers,
            "max_uniform_block_size", get_limit(GL_MAX_UNIFORM_BLOCK_SIZE, 0x4000, 0x40000000),
            "uniform_buffer_offset_alignment", res->uniform_buffer_offset_alignment
        );
        res->depth_test = 0;
        res->depth_write = 1;
        res->depth_func = GL_LESS;
        res->cull_face = 0;
        res->stencil_write_mask = 0xFF;
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
        args,
        kwargs,
        "(ddd)|(ddd)(ddd)dddddp",
        keywords,
        &eye.x,
        &eye.y,
        &eye.z,
        &target.x,
        &target.y,
        &target.z,
        &up.x,
        &up.y,
        &up.z,
        &fov,
        &aspect,
        &znear,
        &zfar,
        &size,
        &clip
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
    Py_DECREF(self->kind);
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
    Py_DECREF(self->uniforms);
    Py_DECREF(self->uniform_index);
    Py_DECREF(self->samplers);
    Py_DECREF(self->uniform_buffers);
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

static void Pipeline_dealloc(Pipeline * self) {
    PyTypeObject * type = Py_TYPE(self);
    PyMem_Free(self->uniform_data);
    PyMem_Free(self->uniform_offsets);
    Py_DECREF(self->program);
    Py_DECREF(self->vertex_array);
    Py_DECREF(self->framebuffer);
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

static PyMethodDef HeadlessContext_methods[] = {
    {"load_opengl_function", (PyCFunction)HeadlessContext_meth_load_opengl_function, METH_O, NULL},
    {"release", (PyCFunction)HeadlessContext_meth_release, METH_NOARGS, NULL},
    {0},
};

static PyMethodDef Buffer_methods[] = {
    {"write", (PyCFunction)Buffer_meth_write, METH_VARARGS | METH_KEYWORDS, NULL},
    {"read", (PyCFunction)Buffer_meth_read, METH_VARARGS | METH_KEYWORDS, NULL},
    {"read_into", (PyCFunction)Buffer_meth_read_into, METH_VARARGS | METH_KEYWORDS, NULL},
    {"bind", (PyCFunction)Buffer_meth_bind, METH_VARARGS | METH_KEYWORDS, NULL},
    {"use", (PyCFunction)Buffer_meth_use, METH_VARARGS | METH_KEYWORDS, NULL},
    {"view", (PyCFunction)Buffer_meth_view, METH_VARARGS | METH_KEYWORDS, NULL},
    {0},
};

static PyMemberDef Buffer_members[] = {
    {"kind", Py_T_OBJECT_EX, offsetof(Buffer, kind), Py_READONLY, NULL},
    {"size", Py_T_INT, offsetof(Buffer, size), Py_READONLY, NULL},
    {"glo", Py_T_UINT, offsetof(Buffer, buffer), Py_READONLY, NULL},
    {0},
};

static PyMethodDef Texture_methods[] = {
    {"write", (PyCFunction)Texture_meth_write, METH_VARARGS | METH_KEYWORDS, NULL},
    {"read", (PyCFunction)Texture_meth_read, METH_VARARGS | METH_KEYWORDS, NULL},
    {"read_into", (PyCFunction)Texture_meth_read_into, METH_VARARGS | METH_KEYWORDS, NULL},
    {"attachment", (PyCFunction)Texture_meth_attachment, METH_VARARGS | METH_KEYWORDS, NULL},
    {"mipmaps", (PyCFunction)Texture_meth_mipmaps, METH_NOARGS, NULL},
    {0},
};

static PyMemberDef Texture_members[] = {
    {"size", Py_T_OBJECT_EX, offsetof(Texture, size), Py_READONLY, NULL},
    {"format", Py_T_OBJECT_EX, offsetof(Texture, format), Py_READONLY, NULL},
    {"samples", Py_T_INT, offsetof(Texture, samples), Py_READONLY, NULL},
    {"array", Py_T_INT, offsetof(Texture, array), Py_READONLY, NULL},
    {"cubemap", Py_T_BOOL, offsetof(Texture, cubemap), Py_READONLY, NULL},
    {"renderbuffer", Py_T_BOOL, offsetof(Texture, renderbuffer), Py_READONLY, NULL},
    {"levels", Py_T_INT, offsetof(Texture, level_count), Py_READONLY, NULL},
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
    {0},
};

static PyMethodDef VertexArray_methods[] = {
    {"use", (PyCFunction)VertexArray_meth_use, METH_NOARGS, NULL},
    {"render", (PyCFunction)VertexArray_meth_render, METH_VARARGS | METH_KEYWORDS, NULL},
    {"draw", (PyCFunction)VertexArray_meth_draw, METH_VARARGS | METH_KEYWORDS, NULL},
    {0},
};

static PyMemberDef VertexArray_members[] = {
    {"vertices", Py_T_INT, offsetof(VertexArray, vertices), 0, NULL},
    {"attributes", Py_T_OBJECT_EX, offsetof(VertexArray, attributes), Py_READONLY, NULL},
    {"index_buffer", Py_T_OBJECT_EX, offsetof(VertexArray, index_buffer), Py_READONLY, NULL},
    {"index_size", Py_T_INT, offsetof(VertexArray, index_size), Py_READONLY, NULL},
    {"glo", Py_T_UINT, offsetof(VertexArray, vertex_array), Py_READONLY, NULL},
    {0},
};

static PyGetSetDef VertexArray_getset[] = {
    {"mode", (getter)VertexArray_get_mode, (setter)VertexArray_set_mode, NULL, NULL},
    {0},
};

static PyMethodDef Program_methods[] = {
    {"use", (PyCFunction)Program_meth_use, METH_NOARGS, NULL},
    {"debug", (PyCFunction)Program_meth_debug, METH_NOARGS, NULL},
    {0},
};

static PyMemberDef Program_members[] = {
    {"uniforms", Py_T_OBJECT_EX, offsetof(Program, uniforms), Py_READONLY, NULL},
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
    {"samples", Py_T_INT, offsetof(Framebuffer, samples), Py_READONLY, NULL},
    {"glo", Py_T_UINT, offsetof(Framebuffer, framebuffer), Py_READONLY, NULL},
    {0},
};

static PyGetSetDef Framebuffer_getset[] = {
    {"viewport", (getter)Framebuffer_get_viewport, (setter)Framebuffer_set_viewport, NULL, NULL},
    {"size", (getter)Framebuffer_get_size, (setter)Framebuffer_set_size, NULL, NULL},
    {"clear_color", (getter)Framebuffer_get_clear_color, (setter)Framebuffer_set_clear_color, NULL, NULL},
    {"clear_depth", (getter)Framebuffer_get_clear_depth, (setter)Framebuffer_set_clear_depth, NULL, NULL},
    {"clear_stencil", (getter)Framebuffer_get_clear_stencil, (setter)Framebuffer_set_clear_stencil, NULL, NULL},
    {0},
};

static PyMethodDef Pipeline_methods[] = {
    {"run", (PyCFunction)Pipeline_meth_run, METH_NOARGS, NULL},
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
    {0},
};

static PyGetSetDef Pipeline_getset[] = {
    {"viewport", (getter)Pipeline_get_viewport, (setter)Pipeline_set_viewport, NULL, NULL},
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
    {"source", (PyCFunction)Context_meth_source, METH_O, NULL},
    {"start_frame", (PyCFunction)Context_meth_start_frame, METH_NOARGS, NULL},
    {"end_frame", (PyCFunction)Context_meth_end_frame, METH_NOARGS, NULL},
    {"present", (PyCFunction)Context_meth_present, METH_O, NULL},
    {"copy_buffer", (PyCFunction)Context_meth_copy_buffer, METH_VARARGS | METH_KEYWORDS, NULL},
    {"copy_texture", (PyCFunction)Context_meth_copy_texture, METH_VARARGS | METH_KEYWORDS, NULL},
    {"copy_buffer_to_texture", (PyCFunction)Context_meth_copy_buffer_to_texture, METH_VARARGS | METH_KEYWORDS, NULL},
    {"copy_texture_to_buffer", (PyCFunction)Context_meth_copy_texture_to_buffer, METH_VARARGS | METH_KEYWORDS, NULL},
    {"program", (PyCFunction)Context_meth_program, METH_VARARGS | METH_KEYWORDS, NULL},
    {"release", (PyCFunction)Context_meth_release, METH_O, NULL},
    {"reset_state", (PyCFunction)Context_meth_reset_state, METH_NOARGS, NULL},
    {"sampler", (PyCFunction)Context_meth_sampler, METH_VARARGS | METH_KEYWORDS, NULL},
    {"resources", (PyCFunction)Context_meth_resources, METH_VARARGS | METH_KEYWORDS, NULL},
    {"pipeline", (PyCFunction)Context_meth_pipeline, METH_VARARGS | METH_KEYWORDS, NULL},
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
    {0},
};

static PyType_Slot HeadlessContext_slots[] = {
    {Py_tp_methods, HeadlessContext_methods},
    {Py_tp_dealloc, (void *)HeadlessContext_dealloc},
    {0},
};

static PyType_Slot Buffer_slots[] = {
    {Py_tp_methods, Buffer_methods},
    {Py_tp_members, Buffer_members},
    {Py_tp_dealloc, (void *)Buffer_dealloc},
    {0},
};

static PyType_Slot Texture_slots[] = {
    {Py_tp_methods, Texture_methods},
    {Py_tp_members, Texture_members},
    {Py_tp_dealloc, (void *)Texture_dealloc},
    {0},
};

static PyType_Slot Sampler_slots[] = {
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

static PyType_Slot Program_slots[] = {
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

static PyType_Slot Context_slots[] = {
    {Py_tp_methods, Context_methods},
    {Py_tp_getset, Context_getset},
    {Py_tp_members, Context_members},
    {Py_tp_dealloc, (void *)Context_dealloc},
    {0},
};

static PyType_Spec HeadlessContext_spec = {
    "moderngl.HeadlessContext",
    sizeof(HeadlessContext),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_DISALLOW_INSTANTIATION,
    HeadlessContext_slots,
};

static PyType_Spec Buffer_spec = {
    "moderngl.Buffer",
    sizeof(Buffer),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_DISALLOW_INSTANTIATION,
    Buffer_slots,
};

static PyType_Spec Texture_spec = {
    "moderngl.Texture",
    sizeof(Texture),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_DISALLOW_INSTANTIATION,
    Texture_slots,
};

static PyType_Spec Sampler_spec = {
    "moderngl.Sampler",
    sizeof(Sampler),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_DISALLOW_INSTANTIATION,
    Sampler_slots,
};

static PyType_Spec Resources_spec = {
    "moderngl.Resources",
    sizeof(Resources),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_DISALLOW_INSTANTIATION,
    Resources_slots,
};

static PyType_Spec VertexArray_spec = {
    "moderngl.VertexArray",
    sizeof(VertexArray),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_DISALLOW_INSTANTIATION,
    VertexArray_slots,
};

static PyType_Spec Program_spec = {
    "moderngl.Program",
    sizeof(Program),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_DISALLOW_INSTANTIATION,
    Program_slots,
};

static PyType_Spec Framebuffer_spec = {
    "moderngl.Framebuffer",
    sizeof(Framebuffer),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_DISALLOW_INSTANTIATION,
    Framebuffer_slots,
};

static PyType_Spec Pipeline_spec = {
    "moderngl.Pipeline",
    sizeof(Pipeline),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_DISALLOW_INSTANTIATION,
    Pipeline_slots,
};

static PyType_Spec Context_spec = {
    "moderngl.Context",
    sizeof(Context),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_DISALLOW_INSTANTIATION,
    Context_slots,
};

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
    if (!(state->HeadlessContext_type = add_type(self, &HeadlessContext_spec))) {
        return -1;
    }

#ifdef EXTERN_GL
    if (PyModule_Add(self, "_extern_gl", PyUnicode_FromString(EXTERN_GL))) {
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
    Py_CLEAR(state->HeadlessContext_type);
    return 0;
}

static void module_free(void * self) {
    module_clear((PyObject *)self);
}

static PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "moderngl",
    NULL,
    sizeof(ModuleState),
    module_methods,
    module_slots,
    module_traverse,
    module_clear,
    module_free,
};

extern PyObject * PyInit_moderngl() {
    return PyModuleDef_Init(&module_def);
}
