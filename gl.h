enum {
    GL_ACTIVE_RESOURCES = 0x92F5,
    GL_ACTIVE_UNIFORMS = 0x8B86,
    GL_ACTIVE_UNIFORM_BLOCKS = 0x8A36,
    GL_ALL_BARRIER_BITS = 0xFFFFFFFF,
    GL_ALREADY_SIGNALED = 0x911A,
    GL_ALWAYS = 0x0207,
    GL_ARRAY_BUFFER = 0x8892,
    GL_BACK = 0x0405,
    GL_BGRA = 0x80E1,
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
    GL_COMPUTE_SHADER = 0x91B9,
    GL_CONDITION_SATISFIED = 0x911C,
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
    GL_DRAW_INDIRECT_BUFFER = 0x8F3F,
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
    GL_MAJOR_VERSION = 0x821B,
    GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0x8B4D,
    GL_MAX_IMAGE_UNITS = 0x8F38,
    GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS = 0x90DD,
    GL_MAX_UNIFORM_BLOCK_SIZE = 0x8A30,
    GL_MAX_UNIFORM_BUFFER_BINDINGS = 0x8A2F,
    GL_MAX_VERTEX_ATTRIBS = 0x8869,
    GL_MINOR_VERSION = 0x821C,
    GL_NEAREST = 0x2600,
    GL_NONE = 0,
    GL_ONE = 1,
    GL_PIXEL_PACK_BUFFER = 0x88EB,
    GL_PIXEL_UNPACK_BUFFER = 0x88EC,
    GL_PRIMITIVE_RESTART = 0x8F9D,
    GL_PROGRAM_POINT_SIZE = 0x8642,
    GL_R8 = 0x8229,
    GL_READ_FRAMEBUFFER = 0x8CA8,
    GL_READ_ONLY = 0x88B8,
    GL_READ_WRITE = 0x88BA,
    GL_RENDERBUFFER = 0x8D41,
    GL_RENDERER = 0x1F01,
    GL_SHADER_STORAGE_BLOCK = 0x92E6,
    GL_SHADER_STORAGE_BUFFER = 0x90D2,
    GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT = 0x90DF,
    GL_SHADING_LANGUAGE_VERSION = 0x8B8C,
    GL_STENCIL_BUFFER_BIT = 0x00000400,
    GL_STENCIL_TEST = 0x0B90,
    GL_STREAM_READ = 0x88E1,
    GL_SYNC_FLUSH_COMMANDS_BIT = 0x00000001,
    GL_SYNC_GPU_COMMANDS_COMPLETE = 0x9117,
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
    GL_WAIT_FAILED = 0x911D,
    GL_ZERO = 0,
};

static const uint32_t GL_INVALID_INDEX = 0xFFFFFFFF;

static THREAD_LOCAL int gl_loaded;
#ifdef EXTERN_GL
static const char * gl_backend = "webgl";
#else
static const char * gl_backend = "gl";
#endif
static PyMutex gl_mutex;

#ifndef EXTERN_GL

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
static THREAD_LOCAL void * (GLAPI * glFenceSync)(uint32_t condition, uint32_t flags) = (void *)gl_trap;
static THREAD_LOCAL uint32_t (GLAPI * glClientWaitSync)(void * sync, uint32_t flags, uint64_t timeout) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDeleteSync)(void * sync) = (void *)gl_trap;
static THREAD_LOCAL void (GLAPI * glDispatchCompute)(uint32_t x, uint32_t y, uint32_t z) = NULL;
static THREAD_LOCAL void (GLAPI * glMemoryBarrier)(uint32_t barriers) = NULL;
static THREAD_LOCAL void (GLAPI * glBindImageTexture)(uint32_t unit, uint32_t texture, int32_t level, uint8_t layered, int32_t layer, uint32_t access, uint32_t format) = NULL;
static THREAD_LOCAL void (GLAPI * glShaderStorageBlockBinding)(uint32_t program, uint32_t index, uint32_t binding) = NULL;
static THREAD_LOCAL void (GLAPI * glGetProgramInterfaceiv)(uint32_t program, uint32_t program_interface, uint32_t pname, int32_t * params) = NULL;
static THREAD_LOCAL void (GLAPI * glGetProgramResourceName)(uint32_t program, uint32_t program_interface, uint32_t index, int32_t size, int32_t * length, char * name) = NULL;
static THREAD_LOCAL uint32_t (GLAPI * glGetProgramResourceIndex)(uint32_t program, uint32_t program_interface, const char * name) = NULL;
static THREAD_LOCAL void (GLAPI * glDrawArraysInstancedBaseInstance)(uint32_t mode, int32_t first, int32_t count, int32_t instances, uint32_t base_instance) = NULL;
static THREAD_LOCAL void (GLAPI * glDrawElementsInstancedBaseVertexBaseInstance)(uint32_t mode, int32_t count, uint32_t type, const void * indices, int32_t instances, int32_t base_vertex, uint32_t base_instance) = NULL;
static THREAD_LOCAL void (GLAPI * glMultiDrawArraysIndirect)(uint32_t mode, const void * indirect, int32_t count, int32_t stride) = NULL;
static THREAD_LOCAL void (GLAPI * glMultiDrawElementsIndirect)(uint32_t mode, uint32_t type, const void * indirect, int32_t count, int32_t stride) = NULL;
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
        dlclose(egl);
        return 0;
    }

    const int32_t config_attribs[] = {0x3033, 0, 0x3040, 0x0008, 0x3038};
    const int32_t context_attribs[] = {0x3098, 3, 0x30FB, 3, 0x30FD, 1, 0x3038};
    void * config = NULL;
    int32_t count = 0;
    eglBindAPI(0x30A2);
    if (!eglChooseConfig(display, config_attribs, &config, 1, &count) || !count) {
        PyErr_Format(PyExc_RuntimeError, "cannot choose an EGL config");
        dlclose(egl);
        return 0;
    }
    void * context = eglCreateContext(display, config, NULL, context_attribs);
    if (!context || !eglMakeCurrent(display, NULL, NULL, context)) {
        PyErr_Format(PyExc_RuntimeError, "cannot create an OpenGL 3.3 core context");
        dlclose(egl);
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
        {"glFenceSync", (void **)&glFenceSync},
        {"glClientWaitSync", (void **)&glClientWaitSync},
        {"glDeleteSync", (void **)&glDeleteSync},
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

    const GLFunction optional_functions[] = {
        {"glDispatchCompute", (void **)&glDispatchCompute},
        {"glMemoryBarrier", (void **)&glMemoryBarrier},
        {"glShaderStorageBlockBinding", (void **)&glShaderStorageBlockBinding},
        {"glGetProgramInterfaceiv", (void **)&glGetProgramInterfaceiv},
        {"glGetProgramResourceName", (void **)&glGetProgramResourceName},
        {"glGetProgramResourceIndex", (void **)&glGetProgramResourceIndex},
        {"glDrawArraysInstancedBaseInstance", (void **)&glDrawArraysInstancedBaseInstance},
        {"glDrawElementsInstancedBaseVertexBaseInstance", (void **)&glDrawElementsInstancedBaseVertexBaseInstance},
        {"glMultiDrawArraysIndirect", (void **)&glMultiDrawArraysIndirect},
        {"glMultiDrawElementsIndirect", (void **)&glMultiDrawElementsIndirect},
        {"glBindImageTexture", (void **)&glBindImageTexture},
        {NULL},
    };

    const int32_t optional_versions[] = {43, 42, 43, 43, 43, 43, 42, 42, 43, 43, 42};

    void * optional[sizeof(optional_functions) / sizeof(GLFunction)] = {0};

    for (int32_t i = 0; optional_functions[i].name; ++i) {
        const char * name = optional_functions[i].name;
        if (!load_function) {
            optional[i] = default_loader_load(name);
            continue;
        }
        PyObject * res = PyObject_CallFunction(load_function, "s", name);
        optional[i] = res && PyLong_Check(res) ? PyLong_AsVoidPtr(res) : NULL;
        Py_XDECREF(res);
        PyErr_Clear();
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

    int32_t major = 0;
    int32_t minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    for (int32_t i = 0; optional_functions[i].name; ++i) {
        *optional_functions[i].ptr = major * 10 + minor >= optional_versions[i] ? optional[i] : NULL;
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

#else

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
extern void * glFenceSync(uint32_t condition, uint32_t flags) __asm__("moderngl_glFenceSync");
extern uint32_t glClientWaitSync(void * sync, uint32_t flags, uint64_t timeout) __asm__("moderngl_glClientWaitSync");
extern void glDeleteSync(void * sync) __asm__("moderngl_glDeleteSync");
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

static void (* glDispatchCompute)(uint32_t x, uint32_t y, uint32_t z) = NULL;
static void (* glMemoryBarrier)(uint32_t barriers) = NULL;
static void (* glShaderStorageBlockBinding)(uint32_t program, uint32_t index, uint32_t binding) = NULL;
static void (* glGetProgramInterfaceiv)(uint32_t program, uint32_t program_interface, uint32_t pname, int32_t * params) = NULL;
static void (* glGetProgramResourceName)(uint32_t program, uint32_t program_interface, uint32_t index, int32_t size, int32_t * length, char * name) = NULL;
static uint32_t (* glGetProgramResourceIndex)(uint32_t program, uint32_t program_interface, const char * name) = NULL;
static void (* glDrawArraysInstancedBaseInstance)(uint32_t mode, int32_t first, int32_t count, int32_t instances, uint32_t base_instance) = NULL;
static void (* glDrawElementsInstancedBaseVertexBaseInstance)(uint32_t mode, int32_t count, uint32_t type, const void * indices, int32_t instances, int32_t base_vertex, uint32_t base_instance) = NULL;
static void (* glMultiDrawArraysIndirect)(uint32_t mode, const void * indirect, int32_t count, int32_t stride) = NULL;
static void (* glMultiDrawElementsIndirect)(uint32_t mode, uint32_t type, const void * indirect, int32_t count, int32_t stride) = NULL;
static void (* glBindImageTexture)(uint32_t unit, uint32_t texture, int32_t level, uint8_t layered, int32_t layer, uint32_t access, uint32_t format) = NULL;

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


// moderngl.js begin, generated by embed_js.py
static const char webgl_js[] =
    "(wasm, gl) => {\n"
    "  gl.getExtension('EXT_color_buffer_float');\n"
    "  gl.getExtension('EXT_float_blend');\n"
    "  const decoder = new TextDecoder();\n"
    "  const encoder = new TextEncoder();\n"
    "\n"
    "  const objects = new Map();\n"
    "  const locations = new Map();\n"
    "  const strings = new Map();\n"
    "  let nextId = 1;\n"
    "  let packBuffer = 0;\n"
    "  let unpackBuffer = 0;\n"
    "\n"
    "  const add = (obj) => {\n"
    "    const id = nextId++;\n"
    "    objects.set(id, obj);\n"
    "    return id;\n"
    "  };\n"
    "\n"
    "  const get = (id) => objects.get(id) \?\? null;\n"
    "\n"
    "  const genObjects = (n, ptr, create) => {\n"
    "    for (let i = 0; i < n; ++i) {\n"
    "      wasm.HEAPU32[(ptr >> 2) + i] = add(create());\n"
    "    }\n"
    "  };\n"
    "\n"
    "  const deleteObjects = (n, ptr, destroy) => {\n"
    "    for (let i = 0; i < n; ++i) {\n"
    "      const id = wasm.HEAPU32[(ptr >> 2) + i];\n"
    "      destroy(get(id));\n"
    "      objects.delete(id);\n"
    "    }\n"
    "  };\n"
    "\n"
    "  const getString = (ptr) => {\n"
    "    const end = wasm.HEAPU8.indexOf(0, ptr);\n"
    "    return decoder.decode(wasm.HEAPU8.subarray(ptr, end));\n"
    "  };\n"
    "\n"
    "  const setString = (ptr, bufSize, lengthPtr, text) => {\n"
    "    const raw = encoder.encode(text).subarray(0, bufSize - 1);\n"
    "    wasm.HEAPU8.set(raw, ptr);\n"
    "    wasm.HEAPU8[ptr + raw.length] = 0;\n"
    "    if (lengthPtr) {\n"
    "      wasm.HEAP32[lengthPtr >> 2] = raw.length;\n"
    "    }\n"
    "  };\n"
    "\n"
    "  const setValues = (heap, ptr, value) => {\n"
    "    const values = typeof value === 'object' \? Array.from(value, Number) : [Number(value)];\n"
    "    heap.set(values, ptr >> 2);\n"
    "  };\n"
    "\n"
    "  const setIntegers = (ptr, value) => {\n"
    "    const values = typeof value === 'object' \? Array.from(value, Number) : [Number(value)];\n"
    "    wasm.HEAP32.set(values.map((x) => Math.min(x, 0x7fffffff)), ptr >> 2);\n"
    "  };\n"
    "\n"
    "  const pixelHeap = (type) => {\n"
    "    switch (type) {\n"
    "      case 0x1400: return [wasm.HEAP8, 0];\n"
    "      case 0x1401: return [wasm.HEAPU8, 0];\n"
    "      case 0x1402: return [wasm.HEAP16, 1];\n"
    "      case 0x1403: return [wasm.HEAPU16, 1];\n"
    "      case 0x1404: return [wasm.HEAP32, 2];\n"
    "      case 0x1405: return [wasm.HEAPU32, 2];\n"
    "      case 0x1406: return [wasm.HEAPF32, 2];\n"
    "      case 0x140B: return [wasm.HEAPU16, 1];\n"
    "      case 0x8368: return [wasm.HEAPU32, 2];\n"
    "      case 0x84FA: return [wasm.HEAPU32, 2];\n"
    "    }\n"
    "  };\n"
    "\n"
    "  const ignoredCaps = new Set([0x8F9D, 0x8642, 0x884F]);\n"
    "\n"
    "  const shaderHeader = `#version 300 es\n"
    "precision highp float;\n"
    "precision highp int;\n"
    "precision highp sampler3D;\n"
    "precision highp sampler2DArray;\n"
    "precision highp sampler2DShadow;\n"
    "precision highp sampler2DArrayShadow;\n"
    "precision highp samplerCubeShadow;\n"
    "precision highp isampler2D;\n"
    "precision highp isampler3D;\n"
    "precision highp isamplerCube;\n"
    "precision highp isampler2DArray;\n"
    "precision highp usampler2D;\n"
    "precision highp usampler3D;\n"
    "precision highp usamplerCube;\n"
    "precision highp usampler2DArray;`;\n"
    "\n"
    "  return {\n"
    "    moderngl_glActiveTexture: (texture) => gl.activeTexture(texture),\n"
    "    moderngl_glAttachShader: (program, shader) => gl.attachShader(get(program), get(shader)),\n"
    "    moderngl_glBindBuffer: (target, buffer) => {\n"
    "      if (target === 0x88EB) {\n"
    "        packBuffer = buffer;\n"
    "      }\n"
    "      if (target === 0x88EC) {\n"
    "        unpackBuffer = buffer;\n"
    "      }\n"
    "      gl.bindBuffer(target, get(buffer));\n"
    "    },\n"
    "    moderngl_glBindBufferBase: (target, index, buffer) => gl.bindBufferBase(target, index, get(buffer)),\n"
    "    moderngl_glBindBufferRange: (target, index, buffer, offset, size) => gl.bindBufferRange(target, index, get(buffer), offset, size),\n"
    "    moderngl_glBindFramebuffer: (target, framebuffer) => gl.bindFramebuffer(target, get(framebuffer)),\n"
    "    moderngl_glBindRenderbuffer: (target, renderbuffer) => gl.bindRenderbuffer(target, get(renderbuffer)),\n"
    "    moderngl_glBindSampler: (unit, sampler) => gl.bindSampler(unit, get(sampler)),\n"
    "    moderngl_glBindTexture: (target, texture) => gl.bindTexture(target, get(texture)),\n"
    "    moderngl_glBindVertexArray: (array) => gl.bindVertexArray(get(array)),\n"
    "    moderngl_glBlendEquationSeparate: (modeRgb, modeAlpha) => gl.blendEquationSeparate(modeRgb, modeAlpha),\n"
    "    moderngl_glBlendFuncSeparate: (srcRgb, dstRgb, srcAlpha, dstAlpha) => gl.blendFuncSeparate(srcRgb, dstRgb, srcAlpha, dstAlpha),\n"
    "    moderngl_glBlitFramebuffer: (sx0, sy0, sx1, sy1, dx0, dy0, dx1, dy1, mask, filter) => gl.blitFramebuffer(sx0, sy0, sx1, sy1, dx0, dy0, dx1, dy1, mask, filter),\n"
    "    moderngl_glBufferData: (target, size, data, usage) => gl.bufferData(target, size, usage),\n"
    "    moderngl_glBufferSubData: (target, offset, size, data) => gl.bufferSubData(target, offset, wasm.HEAPU8, data, size),\n"
    "    moderngl_glCheckFramebufferStatus: (target) => gl.checkFramebufferStatus(target),\n"
    "    moderngl_glClearBufferfi: (buffer, drawbuffer, depth, stencil) => gl.clearBufferfi(buffer, drawbuffer, depth, stencil),\n"
    "    moderngl_glClearBufferfv: (buffer, drawbuffer, value) => gl.clearBufferfv(buffer, drawbuffer, wasm.HEAPF32, value >> 2),\n"
    "    moderngl_glClearBufferiv: (buffer, drawbuffer, value) => gl.clearBufferiv(buffer, drawbuffer, wasm.HEAP32, value >> 2),\n"
    "    moderngl_glClearBufferuiv: (buffer, drawbuffer, value) => gl.clearBufferuiv(buffer, drawbuffer, wasm.HEAPU32, value >> 2),\n"
    "    moderngl_glClientWaitSync: (sync, flags, timeout) => gl.clientWaitSync(get(sync), flags, 0),\n"
    "    moderngl_glCompileShader: (shader) => gl.compileShader(get(shader)),\n"
    "    moderngl_glCopyBufferSubData: (readTarget, writeTarget, readOffset, writeOffset, size) => gl.copyBufferSubData(readTarget, writeTarget, readOffset, writeOffset, size),\n"
    "    moderngl_glCreateProgram: () => add(gl.createProgram()),\n"
    "    moderngl_glCreateShader: (type) => add(gl.createShader(type)),\n"
    "    moderngl_glCullFace: (mode) => gl.cullFace(mode),\n"
    "    moderngl_glDeleteBuffers: (n, buffers) => deleteObjects(n, buffers, (x) => gl.deleteBuffer(x)),\n"
    "    moderngl_glDeleteFramebuffers: (n, framebuffers) => deleteObjects(n, framebuffers, (x) => gl.deleteFramebuffer(x)),\n"
    "    moderngl_glDeleteProgram: (program) => {\n"
    "      for (const [key, id] of locations) {\n"
    "        if (key.startsWith(`${program}:`)) {\n"
    "          objects.delete(id);\n"
    "          locations.delete(key);\n"
    "        }\n"
    "      }\n"
    "      gl.deleteProgram(get(program));\n"
    "      objects.delete(program);\n"
    "    },\n"
    "    moderngl_glDeleteRenderbuffers: (n, renderbuffers) => deleteObjects(n, renderbuffers, (x) => gl.deleteRenderbuffer(x)),\n"
    "    moderngl_glDeleteSamplers: (n, samplers) => deleteObjects(n, samplers, (x) => gl.deleteSampler(x)),\n"
    "    moderngl_glDeleteShader: (shader) => {\n"
    "      gl.deleteShader(get(shader));\n"
    "      objects.delete(shader);\n"
    "    },\n"
    "    moderngl_glDeleteSync: (sync) => {\n"
    "      gl.deleteSync(get(sync));\n"
    "      objects.delete(sync);\n"
    "    },\n"
    "    moderngl_glDeleteTextures: (n, textures) => deleteObjects(n, textures, (x) => gl.deleteTexture(x)),\n"
    "    moderngl_glDeleteVertexArrays: (n, arrays) => deleteObjects(n, arrays, (x) => gl.deleteVertexArray(x)),\n"
    "    moderngl_glDepthFunc: (func) => gl.depthFunc(func),\n"
    "    moderngl_glDepthMask: (flag) => gl.depthMask(!!flag),\n"
    "    moderngl_glDetachShader: (program, shader) => gl.detachShader(get(program), get(shader)),\n"
    "    moderngl_glDisable: (cap) => ignoredCaps.has(cap) || gl.disable(cap),\n"
    "    moderngl_glDrawArraysInstanced: (mode, first, count, instances) => gl.drawArraysInstanced(mode, first, count, instances),\n"
    "    moderngl_glDrawBuffers: (n, bufs) => gl.drawBuffers(wasm.HEAPU32.subarray(bufs >> 2, (bufs >> 2) + n)),\n"
    "    moderngl_glDrawElementsInstanced: (mode, count, type, offset, instances) => gl.drawElementsInstanced(mode, count, type, offset, instances),\n"
    "    moderngl_glEnable: (cap) => ignoredCaps.has(cap) || gl.enable(cap),\n"
    "    moderngl_glEnableVertexAttribArray: (index) => gl.enableVertexAttribArray(index),\n"
    "    moderngl_glFenceSync: (condition, flags) => add(gl.fenceSync(condition, flags)),\n"
    "    moderngl_glFlush: () => gl.flush(),\n"
    "    moderngl_glFramebufferRenderbuffer: (target, attachment, renderbufferTarget, renderbuffer) => gl.framebufferRenderbuffer(target, attachment, renderbufferTarget, get(renderbuffer)),\n"
    "    moderngl_glFramebufferTexture2D: (target, attachment, textureTarget, texture, level) => gl.framebufferTexture2D(target, attachment, textureTarget, get(texture), level),\n"
    "    moderngl_glFramebufferTextureLayer: (target, attachment, texture, level, layer) => gl.framebufferTextureLayer(target, attachment, get(texture), level, layer),\n"
    "    moderngl_glGenBuffers: (n, buffers) => genObjects(n, buffers, () => gl.createBuffer()),\n"
    "    moderngl_glGenFramebuffers: (n, framebuffers) => genObjects(n, framebuffers, () => gl.createFramebuffer()),\n"
    "    moderngl_glGenRenderbuffers: (n, renderbuffers) => genObjects(n, renderbuffers, () => gl.createRenderbuffer()),\n"
    "    moderngl_glGenSamplers: (n, samplers) => genObjects(n, samplers, () => gl.createSampler()),\n"
    "    moderngl_glGenTextures: (n, textures) => genObjects(n, textures, () => gl.createTexture()),\n"
    "    moderngl_glGenVertexArrays: (n, arrays) => genObjects(n, arrays, () => gl.createVertexArray()),\n"
    "    moderngl_glGenerateMipmap: (target) => gl.generateMipmap(target),\n"
    "    moderngl_glGetActiveUniform: (program, index, bufSize, length, size, type, name) => {\n"
    "      const info = gl.getActiveUniform(get(program), index);\n"
    "      setString(name, bufSize, length, info.name);\n"
    "      wasm.HEAP32[size >> 2] = info.size;\n"
    "      wasm.HEAPU32[type >> 2] = info.type;\n"
    "    },\n"
    "    moderngl_glGetActiveUniformBlockName: (program, index, bufSize, length, name) => {\n"
    "      setString(name, bufSize, length, gl.getActiveUniformBlockName(get(program), index));\n"
    "    },\n"
    "    moderngl_glGetActiveUniformBlockiv: (program, index, pname, params) => {\n"
    "      setIntegers(params, gl.getActiveUniformBlockParameter(get(program), index, pname));\n"
    "    },\n"
    "    moderngl_glGetBufferParameteriv: (target, pname, params) => setIntegers(params, gl.getBufferParameter(target, pname)),\n"
    "    moderngl_glGetBufferSubData: (target, offset, size, data) => gl.getBufferSubData(target, offset, wasm.HEAPU8, data, size),\n"
    "    moderngl_glGetIntegerv: (pname, data) => setIntegers(data, gl.getParameter(pname)),\n"
    "    moderngl_glGetProgramInfoLog: (program, bufSize, length, log) => setString(log, bufSize, length, gl.getProgramInfoLog(get(program))),\n"
    "    moderngl_glGetProgramiv: (program, pname, params) => setIntegers(params, gl.getProgramParameter(get(program), pname)),\n"
    "    moderngl_glGetShaderInfoLog: (shader, bufSize, length, log) => setString(log, bufSize, length, gl.getShaderInfoLog(get(shader))),\n"
    "    moderngl_glGetShaderiv: (shader, pname, params) => setIntegers(params, gl.getShaderParameter(get(shader), pname)),\n"
    "    moderngl_glGetString: (name) => {\n"
    "      if (!strings.has(name)) {\n"
    "        const raw = encoder.encode(gl.getParameter(name));\n"
    "        const ptr = wasm._malloc(raw.length + 1);\n"
    "        wasm.HEAPU8.set(raw, ptr);\n"
    "        wasm.HEAPU8[ptr + raw.length] = 0;\n"
    "        strings.set(name, ptr);\n"
    "      }\n"
    "      return strings.get(name);\n"
    "    },\n"
    "    moderngl_glGetUniformBlockIndex: (program, name) => gl.getUniformBlockIndex(get(program), getString(name)),\n"
    "    moderngl_glGetUniformLocation: (program, name) => {\n"
    "      const key = `${program}:${getString(name)}`;\n"
    "      if (!locations.has(key)) {\n"
    "        const location = gl.getUniformLocation(get(program), getString(name));\n"
    "        locations.set(key, location === null \? -1 : add(location));\n"
    "      }\n"
    "      return locations.get(key);\n"
    "    },\n"
    "    moderngl_glGetUniformfv: (program, location, params) => setValues(wasm.HEAPF32, params, gl.getUniform(get(program), get(location))),\n"
    "    moderngl_glGetUniformiv: (program, location, params) => setValues(wasm.HEAP32, params, gl.getUniform(get(program), get(location))),\n"
    "    moderngl_glGetUniformuiv: (program, location, params) => setValues(wasm.HEAPU32, params, gl.getUniform(get(program), get(location))),\n"
    "    moderngl_glLinkProgram: (program) => gl.linkProgram(get(program)),\n"
    "    moderngl_glPrimitiveRestartIndex: (index) => {},\n"
    "    moderngl_glReadBuffer: (src) => gl.readBuffer(src),\n"
    "    moderngl_glReadPixels: (x, y, width, height, format, type, pixels) => {\n"
    "      if (packBuffer) {\n"
    "        gl.readPixels(x, y, width, height, format, type, pixels);\n"
    "        return;\n"
    "      }\n"
    "      const [heap, shift] = pixelHeap(type);\n"
    "      gl.readPixels(x, y, width, height, format, type, heap, pixels >> shift);\n"
    "    },\n"
    "    moderngl_glRenderbufferStorageMultisample: (target, samples, internalFormat, width, height) => gl.renderbufferStorageMultisample(target, samples, internalFormat, width, height),\n"
    "    moderngl_glSamplerParameterf: (sampler, pname, param) => gl.samplerParameterf(get(sampler), pname, param),\n"
    "    moderngl_glSamplerParameteri: (sampler, pname, param) => gl.samplerParameteri(get(sampler), pname, param),\n"
    "    moderngl_glShaderSource: (shader, count, string, length) => {\n"
    "      const source = getString(wasm.HEAPU32[string >> 2]).replace(/^\\s*#version 330 core/, shaderHeader);\n"
    "      gl.shaderSource(get(shader), source);\n"
    "    },\n"
    "    moderngl_glStencilFunc: (func, ref, mask) => gl.stencilFunc(func, ref, mask),\n"
    "    moderngl_glStencilFuncSeparate: (face, func, ref, mask) => gl.stencilFuncSeparate(face, func, ref, mask),\n"
    "    moderngl_glStencilMask: (mask) => gl.stencilMask(mask),\n"
    "    moderngl_glStencilOp: (fail, depthFail, depthPass) => gl.stencilOp(fail, depthFail, depthPass),\n"
    "    moderngl_glStencilOpSeparate: (face, fail, depthFail, depthPass) => gl.stencilOpSeparate(face, fail, depthFail, depthPass),\n"
    "    moderngl_glTexImage2D: (target, level, internalFormat, width, height, border, format, type, pixels) => {\n"
    "      gl.texImage2D(target, level, internalFormat, width, height, border, format, type, null);\n"
    "    },\n"
    "    moderngl_glTexImage3D: (target, level, internalFormat, width, height, depth, border, format, type, pixels) => {\n"
    "      gl.texImage3D(target, level, internalFormat, width, height, depth, border, format, type, null);\n"
    "    },\n"
    "    moderngl_glTexParameteri: (target, pname, param) => gl.texParameteri(target, pname, param),\n"
    "    moderngl_glTexSubImage2D: (target, level, x, y, width, height, format, type, pixels) => {\n"
    "      if (unpackBuffer) {\n"
    "        gl.texSubImage2D(target, level, x, y, width, height, format, type, pixels);\n"
    "        return;\n"
    "      }\n"
    "      const [heap, shift] = pixelHeap(type);\n"
    "      gl.texSubImage2D(target, level, x, y, width, height, format, type, heap, pixels >> shift);\n"
    "    },\n"
    "    moderngl_glTexSubImage3D: (target, level, x, y, z, width, height, depth, format, type, pixels) => {\n"
    "      if (unpackBuffer) {\n"
    "        gl.texSubImage3D(target, level, x, y, z, width, height, depth, format, type, pixels);\n"
    "        return;\n"
    "      }\n"
    "      const [heap, shift] = pixelHeap(type);\n"
    "      gl.texSubImage3D(target, level, x, y, z, width, height, depth, format, type, heap, pixels >> shift);\n"
    "    },\n"
    "    moderngl_glUniform1fv: (location, count, value) => gl.uniform1fv(get(location), wasm.HEAPF32, value >> 2, count),\n"
    "    moderngl_glUniform1i: (location, value) => gl.uniform1i(get(location), value),\n"
    "    moderngl_glUniform1iv: (location, count, value) => gl.uniform1iv(get(location), wasm.HEAP32, value >> 2, count),\n"
    "    moderngl_glUniform1uiv: (location, count, value) => gl.uniform1uiv(get(location), wasm.HEAPU32, value >> 2, count),\n"
    "    moderngl_glUniform2fv: (location, count, value) => gl.uniform2fv(get(location), wasm.HEAPF32, value >> 2, count * 2),\n"
    "    moderngl_glUniform2iv: (location, count, value) => gl.uniform2iv(get(location), wasm.HEAP32, value >> 2, count * 2),\n"
    "    moderngl_glUniform2uiv: (location, count, value) => gl.uniform2uiv(get(location), wasm.HEAPU32, value >> 2, count * 2),\n"
    "    moderngl_glUniform3fv: (location, count, value) => gl.uniform3fv(get(location), wasm.HEAPF32, value >> 2, count * 3),\n"
    "    moderngl_glUniform3iv: (location, count, value) => gl.uniform3iv(get(location), wasm.HEAP32, value >> 2, count * 3),\n"
    "    moderngl_glUniform3uiv: (location, count, value) => gl.uniform3uiv(get(location), wasm.HEAPU32, value >> 2, count * 3),\n"
    "    moderngl_glUniform4fv: (location, count, value) => gl.uniform4fv(get(location), wasm.HEAPF32, value >> 2, count * 4),\n"
    "    moderngl_glUniform4iv: (location, count, value) => gl.uniform4iv(get(location), wasm.HEAP32, value >> 2, count * 4),\n"
    "    moderngl_glUniform4uiv: (location, count, value) => gl.uniform4uiv(get(location), wasm.HEAPU32, value >> 2, count * 4),\n"
    "    moderngl_glUniformBlockBinding: (program, index, binding) => gl.uniformBlockBinding(get(program), index, binding),\n"
    "    moderngl_glUniformMatrix2fv: (location, count, transpose, value) => gl.uniformMatrix2fv(get(location), false, wasm.HEAPF32, value >> 2, count * 4),\n"
    "    moderngl_glUniformMatrix2x3fv: (location, count, transpose, value) => gl.uniformMatrix2x3fv(get(location), false, wasm.HEAPF32, value >> 2, count * 6),\n"
    "    moderngl_glUniformMatrix2x4fv: (location, count, transpose, value) => gl.uniformMatrix2x4fv(get(location), false, wasm.HEAPF32, value >> 2, count * 8),\n"
    "    moderngl_glUniformMatrix3fv: (location, count, transpose, value) => gl.uniformMatrix3fv(get(location), false, wasm.HEAPF32, value >> 2, count * 9),\n"
    "    moderngl_glUniformMatrix3x2fv: (location, count, transpose, value) => gl.uniformMatrix3x2fv(get(location), false, wasm.HEAPF32, value >> 2, count * 6),\n"
    "    moderngl_glUniformMatrix3x4fv: (location, count, transpose, value) => gl.uniformMatrix3x4fv(get(location), false, wasm.HEAPF32, value >> 2, count * 12),\n"
    "    moderngl_glUniformMatrix4fv: (location, count, transpose, value) => gl.uniformMatrix4fv(get(location), false, wasm.HEAPF32, value >> 2, count * 16),\n"
    "    moderngl_glUniformMatrix4x2fv: (location, count, transpose, value) => gl.uniformMatrix4x2fv(get(location), false, wasm.HEAPF32, value >> 2, count * 8),\n"
    "    moderngl_glUniformMatrix4x3fv: (location, count, transpose, value) => gl.uniformMatrix4x3fv(get(location), false, wasm.HEAPF32, value >> 2, count * 12),\n"
    "    moderngl_glUseProgram: (program) => gl.useProgram(get(program)),\n"
    "    moderngl_glVertexAttribDivisor: (index, divisor) => gl.vertexAttribDivisor(index, divisor),\n"
    "    moderngl_glVertexAttribIPointer: (index, size, type, stride, offset) => gl.vertexAttribIPointer(index, size, type, stride, offset),\n"
    "    moderngl_glVertexAttribPointer: (index, size, type, normalized, stride, offset) => gl.vertexAttribPointer(index, size, type, !!normalized, stride, offset),\n"
    "    moderngl_glViewport: (x, y, width, height) => gl.viewport(x, y, width, height),\n"
    "  };\n"
    "}\n"
;
// moderngl.js end

#endif
