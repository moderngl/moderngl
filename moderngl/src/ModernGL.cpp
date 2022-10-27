#include "Types.hpp"

#include "BufferFormat.hpp"

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
	PyObject * backend;
	PyObject * backend_name = PyDict_GetItemString(kwargs, "backend");
	PyErr_Clear();

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

PyMethodDef MGL_module_methods[] = {
	{"strsize", (PyCFunction)strsize, METH_VARARGS, 0},
	{"create_context", (PyCFunction)create_context, METH_VARARGS | METH_KEYWORDS, 0},
	{"fmtdebug", (PyCFunction)fmtdebug, METH_VARARGS, 0},
	{0},
};

PyObject * MGLContext_buffer(MGLContext * self, PyObject * args);
PyObject * MGLBuffer_write(MGLBuffer * self, PyObject * args);
PyObject * MGLBuffer_read(MGLBuffer * self, PyObject * args);
PyObject * MGLBuffer_read_into(MGLBuffer * self, PyObject * args);
PyObject * MGLBuffer_write_chunks(MGLBuffer * self, PyObject * args);
PyObject * MGLBuffer_read_chunks(MGLBuffer * self, PyObject * args);
PyObject * MGLBuffer_read_chunks_into(MGLBuffer * self, PyObject * args);
PyObject * MGLBuffer_clear(MGLBuffer * self, PyObject * args);
PyObject * MGLBuffer_orphan(MGLBuffer * self, PyObject * args);
PyObject * MGLBuffer_bind_to_uniform_block(MGLBuffer * self, PyObject * args);
PyObject * MGLBuffer_bind_to_storage_buffer(MGLBuffer * self, PyObject * args);
PyObject * MGLBuffer_release(MGLBuffer * self);
PyObject * MGLBuffer_size(MGLBuffer * self);
int MGLBuffer_tp_as_buffer_get_view(MGLBuffer * self, Py_buffer * view, int flags);
void MGLBuffer_tp_as_buffer_release_view(MGLBuffer * self, Py_buffer * view);
void MGLBuffer_Invalidate(MGLBuffer * buffer);
PyObject * MGLContext_compute_shader(MGLContext * self, PyObject * args);
PyObject * MGLComputeShader_run(MGLComputeShader * self, PyObject * args);
PyObject * MGLComputeShader_release(MGLComputeShader * self);
void MGLComputeShader_Invalidate(MGLComputeShader * compute_shader);
PyObject * MGLContext_enable_only(MGLContext * self, PyObject * args);
PyObject * MGLContext_enable(MGLContext * self, PyObject * args);
PyObject * MGLContext_disable(MGLContext * self, PyObject * args);
PyObject * MGLContext_enable_direct(MGLContext * self, PyObject * args);
PyObject * MGLContext_disable_direct(MGLContext * self, PyObject * args);
PyObject * MGLContext_finish(MGLContext * self);
PyObject * MGLContext_copy_buffer(MGLContext * self, PyObject * args);
PyObject * MGLContext_copy_framebuffer(MGLContext * self, PyObject * args);
PyObject * MGLContext_detect_framebuffer(MGLContext * self, PyObject * args);
PyObject * MGLContext_clear_samplers(MGLContext * self, PyObject * args);
PyObject * MGLContext_enter(MGLContext * self);
PyObject * MGLContext_exit(MGLContext * self);
PyObject * MGLContext_release(MGLContext * self);
PyObject * MGLContext_get_ubo_binding(MGLContext * self, PyObject * args);
PyObject * MGLContext_set_ubo_binding(MGLContext * self, PyObject * args);
PyObject * MGLContext_read_uniform(MGLContext * self, PyObject * args);
PyObject * MGLContext_write_uniform(MGLContext * self, PyObject * args);
PyObject * MGLContext_get_line_width(MGLContext * self);
int MGLContext_set_line_width(MGLContext * self, PyObject * value);
PyObject * MGLContext_get_point_size(MGLContext * self);
int MGLContext_set_point_size(MGLContext * self, PyObject * value);
PyObject * MGLContext_get_blend_func(MGLContext * self);
int MGLContext_set_blend_func(MGLContext * self, PyObject * value);
PyObject * MGLContext_get_blend_equation(MGLContext * self);
int MGLContext_set_blend_equation(MGLContext * self, PyObject * value);
PyObject * MGLContext_get_depth_func(MGLContext * self);
int MGLContext_set_depth_func(MGLContext * self, PyObject * value);
PyObject * MGLContext_get_multisample(MGLContext * self);
int MGLContext_set_multisample(MGLContext * self, PyObject * value);
PyObject * MGLContext_get_provoking_vertex(MGLContext * self);
int MGLContext_set_provoking_vertex(MGLContext * self, PyObject * value);
PyObject * MGLContext_get_polygon_offset(MGLContext * self);
int MGLContext_set_polygon_offset(MGLContext * self, PyObject * value);
PyObject * MGLContext_get_default_texture_unit(MGLContext * self);
int MGLContext_set_default_texture_unit(MGLContext * self, PyObject * value);
PyObject * MGLContext_get_max_samples(MGLContext * self);
PyObject * MGLContext_get_max_integer_samples(MGLContext * self);
PyObject * MGLContext_get_max_texture_units(MGLContext * self);
PyObject * MGLContext_get_max_anisotropy(MGLContext * self);
MGLFramebuffer * MGLContext_get_fbo(MGLContext * self);
int MGLContext_set_fbo(MGLContext * self, PyObject * value);
PyObject * MGLContext_get_wireframe(MGLContext * self);
int MGLContext_set_wireframe(MGLContext * self, PyObject * value);
PyObject * MGLContext_get_front_face(MGLContext * self);
int MGLContext_set_front_face(MGLContext * self, PyObject * value);
PyObject * MGLContext_get_cull_face(MGLContext * self);
int MGLContext_set_cull_face(MGLContext * self, PyObject * value);
PyObject * MGLContext_get_patch_vertices(MGLContext * self);
int MGLContext_set_patch_vertices(MGLContext * self, PyObject * value);
PyObject * MGLContext_get_error(MGLContext * self, void * closure);
PyObject * MGLContext_get_version_code(MGLContext * self, void * closure);
PyObject * MGLContext_get_extensions(MGLContext * self, void * closure);
PyObject * MGLContext_get_info(MGLContext * self, void * closure);
void MGLContext_Invalidate(MGLContext * context);
PyObject * MGLContext_framebuffer(MGLContext * self, PyObject * args);
PyObject * MGLFramebuffer_release(MGLFramebuffer * self);
PyObject * MGLFramebuffer_clear(MGLFramebuffer * self, PyObject * args);
PyObject * MGLFramebuffer_use(MGLFramebuffer * self);
PyObject * MGLFramebuffer_read(MGLFramebuffer * self, PyObject * args);
PyObject * MGLFramebuffer_read_into(MGLFramebuffer * self, PyObject * args);
PyObject * MGLFramebuffer_get_viewport(MGLFramebuffer * self, void * closure);
int MGLFramebuffer_set_viewport(MGLFramebuffer * self, PyObject * value, void * closure);
PyObject * MGLFramebuffer_get_scissor(MGLFramebuffer * self, void * closure);
int MGLFramebuffer_set_scissor(MGLFramebuffer * self, PyObject * value, void * closure);
PyObject * MGLFramebuffer_get_color_mask(MGLFramebuffer * self, void * closure);
int MGLFramebuffer_set_color_mask(MGLFramebuffer * self, PyObject * value, void * closure);
PyObject * MGLFramebuffer_get_depth_mask(MGLFramebuffer * self, void * closure);
int MGLFramebuffer_set_depth_mask(MGLFramebuffer * self, PyObject * value, void * closure);
PyObject * MGLFramebuffer_get_bits(MGLFramebuffer * self, void * closure);
void MGLFramebuffer_Invalidate(MGLFramebuffer * framebuffer);
PyObject * MGLContext_program(MGLContext * self, PyObject * args);
PyObject * MGLProgram_release(MGLProgram * self);
void MGLProgram_Invalidate(MGLProgram * program);
PyObject * MGLContext_query(MGLContext * self, PyObject * args);
PyObject * MGLQuery_begin(MGLQuery * self, PyObject * args);
PyObject * MGLQuery_end(MGLQuery * self, PyObject * args);
PyObject * MGLQuery_begin_render(MGLQuery * self, PyObject * args);
PyObject * MGLQuery_end_render(MGLQuery * self, PyObject * args);
PyObject * MGLQuery_get_samples(MGLQuery * self);
PyObject * MGLQuery_get_primitives(MGLQuery * self);
PyObject * MGLQuery_get_elapsed(MGLQuery * self);
void MGLQuery_Invalidate(MGLQuery * query);
PyObject * MGLContext_renderbuffer(MGLContext * self, PyObject * args);
PyObject * MGLContext_depth_renderbuffer(MGLContext * self, PyObject * args);
PyObject * MGLRenderbuffer_release(MGLRenderbuffer * self);
void MGLRenderbuffer_Invalidate(MGLRenderbuffer * renderbuffer);
PyObject * MGLContext_sampler(MGLContext * self, PyObject * args);
PyObject * MGLSampler_use(MGLSampler * self, PyObject * args);
PyObject * MGLSampler_clear(MGLSampler * self, PyObject * args);
PyObject * MGLSampler_release(MGLSampler * self);
PyObject * MGLSampler_get_repeat_x(MGLSampler * self);
int MGLSampler_set_repeat_x(MGLSampler * self, PyObject * value);
PyObject * MGLSampler_get_repeat_y(MGLSampler * self);
int MGLSampler_set_repeat_y(MGLSampler * self, PyObject * value);
PyObject * MGLSampler_get_repeat_z(MGLSampler * self);
int MGLSampler_set_repeat_z(MGLSampler * self, PyObject * value);
PyObject * MGLSampler_get_filter(MGLSampler * self);
int MGLSampler_set_filter(MGLSampler * self, PyObject * value);
PyObject * MGLSampler_get_compare_func(MGLSampler * self);
int MGLSampler_set_compare_func(MGLSampler * self, PyObject * value);
PyObject * MGLSampler_get_anisotropy(MGLSampler * self);
int MGLSampler_set_anisotropy(MGLSampler * self, PyObject * value);
PyObject * MGLSampler_get_border_color(MGLSampler * self);
int MGLSampler_set_border_color(MGLSampler * self, PyObject * value);
PyObject * MGLSampler_get_min_lod(MGLSampler * self);
int MGLSampler_set_min_lod(MGLSampler * self, PyObject * value);
PyObject * MGLSampler_get_max_lod(MGLSampler * self);
int MGLSampler_set_max_lod(MGLSampler * self, PyObject * value);
void MGLSampler_Invalidate(MGLSampler * sampler);
PyObject * MGLContext_scope(MGLContext * self, PyObject * args);
PyObject * MGLScope_begin(MGLScope * self, PyObject * args);
PyObject * MGLScope_end(MGLScope * self, PyObject * args);
PyObject * MGLScope_release(MGLScope * self);
void MGLScope_Invalidate(MGLScope * scope);
PyObject * MGLContext_texture(MGLContext * self, PyObject * args);
PyObject * MGLContext_depth_texture(MGLContext * self, PyObject * args);
PyObject * MGLContext_external_texture(MGLContext * self, PyObject * args);
PyObject * MGLTexture_read(MGLTexture * self, PyObject * args);
PyObject * MGLTexture_read_into(MGLTexture * self, PyObject * args);
PyObject * MGLTexture_write(MGLTexture * self, PyObject * args);
PyObject * MGLTexture_meth_bind(MGLTexture * self, PyObject * args);
PyObject * MGLTexture_use(MGLTexture * self, PyObject * args);
PyObject * MGLTexture_build_mipmaps(MGLTexture * self, PyObject * args);
PyObject * MGLTexture_release(MGLTexture * self);
PyObject * MGLTexture_get_repeat_x(MGLTexture * self);
int MGLTexture_set_repeat_x(MGLTexture * self, PyObject * value);
PyObject * MGLTexture_get_repeat_y(MGLTexture * self);
int MGLTexture_set_repeat_y(MGLTexture * self, PyObject * value);
PyObject * MGLTexture_get_filter(MGLTexture * self);
int MGLTexture_set_filter(MGLTexture * self, PyObject * value);
PyObject * MGLTexture_get_swizzle(MGLTexture * self, void * closure);
int MGLTexture_set_swizzle(MGLTexture * self, PyObject * value, void * closure);
PyObject * MGLTexture_get_compare_func(MGLTexture * self);
int MGLTexture_set_compare_func(MGLTexture * self, PyObject * value);
PyObject * MGLTexture_get_anisotropy(MGLTexture * self);
int MGLTexture_set_anisotropy(MGLTexture * self, PyObject * value);
void MGLTexture_Invalidate(MGLTexture * texture);
PyObject * MGLContext_texture3d(MGLContext * self, PyObject * args);
PyObject * MGLTexture3D_read(MGLTexture3D * self, PyObject * args);
PyObject * MGLTexture3D_read_into(MGLTexture3D * self, PyObject * args);
PyObject * MGLTexture3D_write(MGLTexture3D * self, PyObject * args);
PyObject * MGLTexture3D_meth_bind(MGLTexture3D * self, PyObject * args);
PyObject * MGLTexture3D_use(MGLTexture3D * self, PyObject * args);
PyObject * MGLTexture3D_build_mipmaps(MGLTexture3D * self, PyObject * args);
PyObject * MGLTexture3D_release(MGLTexture3D * self);
PyObject * MGLTexture3D_get_repeat_x(MGLTexture3D * self);
int MGLTexture3D_set_repeat_x(MGLTexture3D * self, PyObject * value);
PyObject * MGLTexture3D_get_repeat_y(MGLTexture3D * self);
int MGLTexture3D_set_repeat_y(MGLTexture3D * self, PyObject * value);
PyObject * MGLTexture3D_get_repeat_z(MGLTexture3D * self);
int MGLTexture3D_set_repeat_z(MGLTexture3D * self, PyObject * value);
PyObject * MGLTexture3D_get_filter(MGLTexture3D * self);
int MGLTexture3D_set_filter(MGLTexture3D * self, PyObject * value);
PyObject * MGLTexture3D_get_swizzle(MGLTexture3D * self, void * closure);
int MGLTexture3D_set_swizzle(MGLTexture3D * self, PyObject * value, void * closure);
void MGLTexture3D_Invalidate(MGLTexture3D * texture);
PyObject * MGLContext_texture_array(MGLContext * self, PyObject * args);
PyObject * MGLTextureArray_read(MGLTextureArray * self, PyObject * args);
PyObject * MGLTextureArray_read_into(MGLTextureArray * self, PyObject * args);
PyObject * MGLTextureArray_write(MGLTextureArray * self, PyObject * args);
PyObject * MGLTextureArray_meth_bind(MGLTextureArray * self, PyObject * args);
PyObject * MGLTextureArray_use(MGLTextureArray * self, PyObject * args);
PyObject * MGLTextureArray_build_mipmaps(MGLTextureArray * self, PyObject * args);
PyObject * MGLTextureArray_release(MGLTextureArray * self);
PyObject * MGLTextureArray_get_repeat_x(MGLTextureArray * self);
int MGLTextureArray_set_repeat_x(MGLTextureArray * self, PyObject * value);
PyObject * MGLTextureArray_get_repeat_y(MGLTextureArray * self);
int MGLTextureArray_set_repeat_y(MGLTextureArray * self, PyObject * value);
PyObject * MGLTextureArray_get_filter(MGLTextureArray * self);
int MGLTextureArray_set_filter(MGLTextureArray * self, PyObject * value);
PyObject * MGLTextureArray_get_swizzle(MGLTextureArray * self, void * closure);
int MGLTextureArray_set_swizzle(MGLTextureArray * self, PyObject * value, void * closure);
PyObject * MGLTextureArray_get_anisotropy(MGLTextureArray * self);
int MGLTextureArray_set_anisotropy(MGLTextureArray * self, PyObject * value);
void MGLTextureArray_Invalidate(MGLTextureArray * texture);
PyObject * MGLContext_texture_cube(MGLContext * self, PyObject * args);
PyObject * MGLTextureCube_read(MGLTextureCube * self, PyObject * args);
PyObject * MGLTextureCube_read_into(MGLTextureCube * self, PyObject * args);
PyObject * MGLTextureCube_write(MGLTextureCube * self, PyObject * args);
PyObject * MGLTextureCube_meth_bind(MGLTextureCube * self, PyObject * args);
PyObject * MGLTextureCube_use(MGLTextureCube * self, PyObject * args);
PyObject * MGLTextureCube_release(MGLTextureCube * self);
PyObject * MGLTextureCube_get_filter(MGLTextureCube * self);
int MGLTextureCube_set_filter(MGLTextureCube * self, PyObject * value);
PyObject * MGLTextureCube_get_swizzle(MGLTextureCube * self, void * closure);
int MGLTextureCube_set_swizzle(MGLTextureCube * self, PyObject * value, void * closure);
PyObject * MGLTextureCube_get_anisotropy(MGLTextureCube * self);
int MGLTextureCube_set_anisotropy(MGLTextureCube * self, PyObject * value);
void MGLTextureCube_Invalidate(MGLTextureCube * texture);
PyObject * MGLContext_vertex_array(MGLContext * self, PyObject * args);
PyObject * MGLVertexArray_render(MGLVertexArray * self, PyObject * args);
PyObject * MGLVertexArray_render_indirect(MGLVertexArray * self, PyObject * args);
PyObject * MGLVertexArray_transform(MGLVertexArray * self, PyObject * args);
PyObject * MGLVertexArray_bind(MGLVertexArray * self, PyObject * args);
PyObject * MGLVertexArray_release(MGLVertexArray * self);
int MGLVertexArray_set_index_buffer(MGLVertexArray * self, PyObject * value, void * closure);
PyObject * MGLVertexArray_get_vertices(MGLVertexArray * self, void * closure);
int MGLVertexArray_set_vertices(MGLVertexArray * self, PyObject * value, void * closure);
PyObject * MGLVertexArray_get_instances(MGLVertexArray * self, void * closure);
int MGLVertexArray_set_instances(MGLVertexArray * self, PyObject * value, void * closure);
int MGLVertexArray_set_subroutines(MGLVertexArray * self, PyObject * value, void * closure);
void MGLVertexArray_Invalidate(MGLVertexArray * array);
void MGLVertexArray_Complete(MGLVertexArray * vertex_array);
void MGLVertexArray_SET_SUBROUTINES(MGLVertexArray * self, const GLMethods & gl);

void default_dealloc(PyObject * self) {
    Py_TYPE(self)->tp_free(self);
}

PyMethodDef MGLBuffer_methods[] = {
	{"write", (PyCFunction)MGLBuffer_write, METH_VARARGS, 0},
	{"read", (PyCFunction)MGLBuffer_read, METH_VARARGS, 0},
	{"read_into", (PyCFunction)MGLBuffer_read_into, METH_VARARGS, 0},
	{"write_chunks", (PyCFunction)MGLBuffer_write_chunks, METH_VARARGS, 0},
	{"read_chunks", (PyCFunction)MGLBuffer_read_chunks, METH_VARARGS, 0},
	{"read_chunks_into", (PyCFunction)MGLBuffer_read_chunks_into, METH_VARARGS, 0},
	{"clear", (PyCFunction)MGLBuffer_clear, METH_VARARGS, 0},
	{"orphan", (PyCFunction)MGLBuffer_orphan, METH_VARARGS, 0},
	{"bind_to_uniform_block", (PyCFunction)MGLBuffer_bind_to_uniform_block, METH_VARARGS, 0},
	{"bind_to_storage_buffer", (PyCFunction)MGLBuffer_bind_to_storage_buffer, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLBuffer_release, METH_NOARGS, 0},
	{"size", (PyCFunction)MGLBuffer_size, METH_NOARGS, 0},
	{0},
};

PyMethodDef MGLComputeShader_methods[] = {
	{"run", (PyCFunction)MGLComputeShader_run, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLComputeShader_release, METH_VARARGS, 0},
	{0},
};

PyMethodDef MGLContext_methods[] = {
	{"enable_only", (PyCFunction)MGLContext_enable_only, METH_VARARGS, 0},
	{"enable", (PyCFunction)MGLContext_enable, METH_VARARGS, 0},
	{"disable", (PyCFunction)MGLContext_disable, METH_VARARGS, 0},
	{"enable_direct", (PyCFunction)MGLContext_enable_direct, METH_VARARGS, 0},
	{"disable_direct", (PyCFunction)MGLContext_disable_direct, METH_VARARGS, 0},
	{"finish", (PyCFunction)MGLContext_finish, METH_NOARGS, 0},
	{"copy_buffer", (PyCFunction)MGLContext_copy_buffer, METH_VARARGS, 0},
	{"copy_framebuffer", (PyCFunction)MGLContext_copy_framebuffer, METH_VARARGS, 0},
	{"detect_framebuffer", (PyCFunction)MGLContext_detect_framebuffer, METH_VARARGS, 0},
	{"clear_samplers", (PyCFunction)MGLContext_clear_samplers, METH_VARARGS, 0},

	{"buffer", (PyCFunction)MGLContext_buffer, METH_VARARGS, 0},
	{"texture", (PyCFunction)MGLContext_texture, METH_VARARGS, 0},
	{"texture3d", (PyCFunction)MGLContext_texture3d, METH_VARARGS, 0},
	{"texture_array", (PyCFunction)MGLContext_texture_array, METH_VARARGS, 0},
	{"texture_cube", (PyCFunction)MGLContext_texture_cube, METH_VARARGS, 0},
	{"depth_texture", (PyCFunction)MGLContext_depth_texture, METH_VARARGS, 0},
	{"external_texture", (PyCFunction)MGLContext_external_texture, METH_VARARGS, 0},
	{"vertex_array", (PyCFunction)MGLContext_vertex_array, METH_VARARGS, 0},
	{"program", (PyCFunction)MGLContext_program, METH_VARARGS, 0},
	{"framebuffer", (PyCFunction)MGLContext_framebuffer, METH_VARARGS, 0},
	{"renderbuffer", (PyCFunction)MGLContext_renderbuffer, METH_VARARGS, 0},
	{"depth_renderbuffer", (PyCFunction)MGLContext_depth_renderbuffer, METH_VARARGS, 0},
	{"compute_shader", (PyCFunction)MGLContext_compute_shader, METH_VARARGS, 0},
	{"query", (PyCFunction)MGLContext_query, METH_VARARGS, 0},
	{"scope", (PyCFunction)MGLContext_scope, METH_VARARGS, 0},
	{"sampler", (PyCFunction)MGLContext_sampler, METH_VARARGS, 0},

	{"__enter__", (PyCFunction)MGLContext_enter, METH_NOARGS, 0},
	{"__exit__", (PyCFunction)MGLContext_exit, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLContext_release, METH_NOARGS, 0},

	{"_get_ubo_binding", (PyCFunction)MGLContext_get_ubo_binding, METH_VARARGS, 0},
	{"_set_ubo_binding", (PyCFunction)MGLContext_set_ubo_binding, METH_VARARGS, 0},
	{"_write_uniform", (PyCFunction)MGLContext_write_uniform, METH_VARARGS, 0},
	{"_read_uniform", (PyCFunction)MGLContext_read_uniform, METH_VARARGS, 0},
	{0},
};

PyGetSetDef MGLContext_getset[] = {
	{(char *)"line_width", (getter)MGLContext_get_line_width, (setter)MGLContext_set_line_width, 0, 0},
	{(char *)"point_size", (getter)MGLContext_get_point_size, (setter)MGLContext_set_point_size, 0, 0},

	{(char *)"depth_func", (getter)MGLContext_get_depth_func, (setter)MGLContext_set_depth_func, 0, 0},
	{(char *)"blend_func", (getter)MGLContext_get_blend_func, (setter)MGLContext_set_blend_func, 0, 0},
	{(char *)"blend_equation", (getter)MGLContext_get_blend_equation, (setter)MGLContext_set_blend_equation, 0, 0},
	{(char *)"multisample", (getter)MGLContext_get_multisample, (setter)MGLContext_set_multisample, 0, 0},

	{(char *)"provoking_vertex", (getter)MGLContext_get_provoking_vertex, (setter)MGLContext_set_provoking_vertex, 0, 0},
	{(char *)"polygon_offset", (getter)MGLContext_get_polygon_offset, (setter)MGLContext_set_polygon_offset, 0, 0},

	{(char *)"default_texture_unit", (getter)MGLContext_get_default_texture_unit, (setter)MGLContext_set_default_texture_unit, 0, 0},
	{(char *)"max_samples", (getter)MGLContext_get_max_samples, 0, 0, 0},
	{(char *)"max_integer_samples", (getter)MGLContext_get_max_integer_samples, 0, 0, 0},
	{(char *)"max_texture_units", (getter)MGLContext_get_max_texture_units, 0, 0, 0},
	{(char *)"max_anisotropy", (getter)MGLContext_get_max_anisotropy, 0, 0, 0},

	{(char *)"fbo", (getter)MGLContext_get_fbo, (setter)MGLContext_set_fbo, 0, 0},

	{(char *)"wireframe", (getter)MGLContext_get_wireframe, (setter)MGLContext_set_wireframe, 0, 0},
	{(char *)"front_face", (getter)MGLContext_get_front_face, (setter)MGLContext_set_front_face, 0, 0},
	{(char *)"cull_face", (getter)MGLContext_get_cull_face, (setter)MGLContext_set_cull_face, 0, 0},

	{(char *)"patch_vertices", (getter)MGLContext_get_patch_vertices, (setter)MGLContext_set_patch_vertices, 0, 0},

	{(char *)"extensions", (getter)MGLContext_get_extensions, 0, 0, 0},
	{(char *)"info", (getter)MGLContext_get_info, 0, 0, 0},
	{(char *)"error", (getter)MGLContext_get_error, 0, 0, 0},
	{0},
};

PyGetSetDef MGLFramebuffer_getset[] = {
	{(char *)"viewport", (getter)MGLFramebuffer_get_viewport, (setter)MGLFramebuffer_set_viewport, 0, 0},
	{(char *)"scissor", (getter)MGLFramebuffer_get_scissor, (setter)MGLFramebuffer_set_scissor, 0, 0},
	{(char *)"color_mask", (getter)MGLFramebuffer_get_color_mask, (setter)MGLFramebuffer_set_color_mask, 0, 0},
	{(char *)"depth_mask", (getter)MGLFramebuffer_get_depth_mask, (setter)MGLFramebuffer_set_depth_mask, 0, 0},

	{(char *)"bits", (getter)MGLFramebuffer_get_bits, 0, 0, 0},
	{0},
};

PyMethodDef MGLFramebuffer_methods[] = {
	{"clear", (PyCFunction)MGLFramebuffer_clear, METH_VARARGS, 0},
	{"use", (PyCFunction)MGLFramebuffer_use, METH_NOARGS, 0},
	{"read", (PyCFunction)MGLFramebuffer_read, METH_VARARGS, 0},
	{"read_into", (PyCFunction)MGLFramebuffer_read_into, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLFramebuffer_release, METH_NOARGS, 0},
	{0},
};

PyMethodDef MGLProgram_methods[] = {
	{"release", (PyCFunction)MGLProgram_release, METH_NOARGS, 0},
	{0},
};

PyGetSetDef MGLQuery_getset[] = {
	{(char *)"samples", (getter)MGLQuery_get_samples, 0, 0, 0},
	{(char *)"primitives", (getter)MGLQuery_get_primitives, 0, 0, 0},
	{(char *)"elapsed", (getter)MGLQuery_get_elapsed, 0, 0, 0},
	{0},
};

PyMethodDef MGLQuery_methods[] = {
	{"begin", (PyCFunction)MGLQuery_begin, METH_VARARGS, 0},
	{"end", (PyCFunction)MGLQuery_end, METH_VARARGS, 0},
	{"begin_render", (PyCFunction)MGLQuery_begin_render, METH_VARARGS, 0},
	{"end_render", (PyCFunction)MGLQuery_end_render, METH_VARARGS, 0},
	// {"release", (PyCFunction)MGLQuery_release, METH_NOARGS, 0},
	{0},
};

PyMethodDef MGLRenderbuffer_methods[] = {
	{"release", (PyCFunction)MGLRenderbuffer_release, METH_NOARGS, 0},
	{0},
};

PyGetSetDef MGLSampler_getset[] = {
	{(char *)"repeat_x", (getter)MGLSampler_get_repeat_x, (setter)MGLSampler_set_repeat_x, 0, 0},
	{(char *)"repeat_y", (getter)MGLSampler_get_repeat_y, (setter)MGLSampler_set_repeat_y, 0, 0},
	{(char *)"repeat_z", (getter)MGLSampler_get_repeat_z, (setter)MGLSampler_set_repeat_z, 0, 0},
	{(char *)"filter", (getter)MGLSampler_get_filter, (setter)MGLSampler_set_filter, 0, 0},
	{(char *)"compare_func", (getter)MGLSampler_get_compare_func, (setter)MGLSampler_set_compare_func, 0, 0},
	{(char *)"anisotropy", (getter)MGLSampler_get_anisotropy, (setter)MGLSampler_set_anisotropy, 0, 0},
	{(char *)"border_color", (getter)MGLSampler_get_border_color, (setter)MGLSampler_set_border_color, 0, 0},
	{(char *)"min_lod", (getter)MGLSampler_get_min_lod, (setter)MGLSampler_set_min_lod, 0, 0},
	{(char *)"max_lod", (getter)MGLSampler_get_max_lod, (setter)MGLSampler_set_max_lod, 0, 0},
	{0},
};

PyMethodDef MGLSampler_methods[] = {
	{"use", (PyCFunction)MGLSampler_use, METH_VARARGS, 0},
	{"clear", (PyCFunction)MGLSampler_clear, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLSampler_release, METH_NOARGS, 0},
	{0},
};

PyMethodDef MGLScope_methods[] = {
	{"begin", (PyCFunction)MGLScope_begin, METH_VARARGS, 0},
	{"end", (PyCFunction)MGLScope_end, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLScope_release, METH_NOARGS, 0},
	{0},
};

PyGetSetDef MGLScope_getset[] = {
	{0},
};

PyGetSetDef MGLTexture_getset[] = {
	{(char *)"repeat_x", (getter)MGLTexture_get_repeat_x, (setter)MGLTexture_set_repeat_x, 0, 0},
	{(char *)"repeat_y", (getter)MGLTexture_get_repeat_y, (setter)MGLTexture_set_repeat_y, 0, 0},
	{(char *)"filter", (getter)MGLTexture_get_filter, (setter)MGLTexture_set_filter, 0, 0},
	{(char *)"swizzle", (getter)MGLTexture_get_swizzle, (setter)MGLTexture_set_swizzle, 0, 0},
	{(char *)"compare_func", (getter)MGLTexture_get_compare_func, (setter)MGLTexture_set_compare_func, 0, 0},
	{(char *)"anisotropy", (getter)MGLTexture_get_anisotropy, (setter)MGLTexture_set_anisotropy, 0, 0},
	{0},
};

PyMethodDef MGLTexture_methods[] = {
	{"write", (PyCFunction)MGLTexture_write, METH_VARARGS, 0},
	{"bind", (PyCFunction)MGLTexture_meth_bind, METH_VARARGS, 0},
	{"use", (PyCFunction)MGLTexture_use, METH_VARARGS, 0},
	{"build_mipmaps", (PyCFunction)MGLTexture_build_mipmaps, METH_VARARGS, 0},
	{"read", (PyCFunction)MGLTexture_read, METH_VARARGS, 0},
	{"read_into", (PyCFunction)MGLTexture_read_into, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLTexture_release, METH_NOARGS, 0},
	{0},
};

PyGetSetDef MGLTexture3D_getset[] = {
	{(char *)"repeat_x", (getter)MGLTexture3D_get_repeat_x, (setter)MGLTexture3D_set_repeat_x, 0, 0},
	{(char *)"repeat_y", (getter)MGLTexture3D_get_repeat_y, (setter)MGLTexture3D_set_repeat_y, 0, 0},
	{(char *)"repeat_z", (getter)MGLTexture3D_get_repeat_z, (setter)MGLTexture3D_set_repeat_z, 0, 0},
	{(char *)"filter", (getter)MGLTexture3D_get_filter, (setter)MGLTexture3D_set_filter, 0, 0},
	{(char *)"swizzle", (getter)MGLTexture3D_get_swizzle, (setter)MGLTexture3D_set_swizzle, 0, 0},
	{0},
};

PyMethodDef MGLTexture3D_methods[] = {
	{"write", (PyCFunction)MGLTexture3D_write, METH_VARARGS, 0},
	{"bind", (PyCFunction)MGLTexture3D_meth_bind, METH_VARARGS, 0},
	{"use", (PyCFunction)MGLTexture3D_use, METH_VARARGS, 0},
	{"build_mipmaps", (PyCFunction)MGLTexture3D_build_mipmaps, METH_VARARGS, 0},
	{"read", (PyCFunction)MGLTexture3D_read, METH_VARARGS, 0},
	{"read_into", (PyCFunction)MGLTexture3D_read_into, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLTexture3D_release, METH_NOARGS, 0},
	{0},
};

PyGetSetDef MGLTextureArray_getset[] = {
	{(char *)"repeat_x", (getter)MGLTextureArray_get_repeat_x, (setter)MGLTextureArray_set_repeat_x, 0, 0},
	{(char *)"repeat_y", (getter)MGLTextureArray_get_repeat_y, (setter)MGLTextureArray_set_repeat_y, 0, 0},
	{(char *)"filter", (getter)MGLTextureArray_get_filter, (setter)MGLTextureArray_set_filter, 0, 0},
	{(char *)"swizzle", (getter)MGLTextureArray_get_swizzle, (setter)MGLTextureArray_set_swizzle, 0, 0},
	{(char *)"anisotropy", (getter)MGLTextureArray_get_anisotropy, (setter)MGLTextureArray_set_anisotropy, 0, 0},
	{0},
};

PyMethodDef MGLTextureArray_methods[] = {
	{"write", (PyCFunction)MGLTextureArray_write, METH_VARARGS, 0},
	{"bind", (PyCFunction)MGLTextureArray_meth_bind, METH_VARARGS, 0},
	{"use", (PyCFunction)MGLTextureArray_use, METH_VARARGS, 0},
	{"build_mipmaps", (PyCFunction)MGLTextureArray_build_mipmaps, METH_VARARGS, 0},
	{"read", (PyCFunction)MGLTextureArray_read, METH_VARARGS, 0},
	{"read_into", (PyCFunction)MGLTextureArray_read_into, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLTextureArray_release, METH_NOARGS, 0},
	{0},
};

PyGetSetDef MGLTextureCube_getset[] = {
	{(char *)"filter", (getter)MGLTextureCube_get_filter, (setter)MGLTextureCube_set_filter, 0, 0},
	{(char *)"swizzle", (getter)MGLTextureCube_get_swizzle, (setter)MGLTextureCube_set_swizzle, 0, 0},
	{(char *)"anisotropy", (getter)MGLTextureCube_get_anisotropy, (setter)MGLTextureCube_set_anisotropy, 0, 0},
	{0},
};

PyMethodDef MGLTextureCube_methods[] = {
	{"write", (PyCFunction)MGLTextureCube_write, METH_VARARGS, 0},
	{"use", (PyCFunction)MGLTextureCube_use, METH_VARARGS, 0},
	{"bind", (PyCFunction)MGLTextureCube_meth_bind, METH_VARARGS, 0},
//	{"build_mipmaps", (PyCFunction)MGLTextureCube_build_mipmaps, METH_VARARGS, 0},
	{"read", (PyCFunction)MGLTextureCube_read, METH_VARARGS, 0},
	{"read_into", (PyCFunction)MGLTextureCube_read_into, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLTextureCube_release, METH_NOARGS, 0},
	{0},
};

PyMethodDef MGLVertexArray_methods[] = {
	{"render", (PyCFunction)MGLVertexArray_render, METH_VARARGS, 0},
	{"render_indirect", (PyCFunction)MGLVertexArray_render_indirect, METH_VARARGS, 0},
	{"transform", (PyCFunction)MGLVertexArray_transform, METH_VARARGS, 0},
	{"bind", (PyCFunction)MGLVertexArray_bind, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLVertexArray_release, METH_NOARGS, 0},
	{0},
};

PyGetSetDef MGLVertexArray_getset[] = {
	{(char *)"index_buffer", 0, (setter)MGLVertexArray_set_index_buffer, 0, 0},
	{(char *)"vertices", (getter)MGLVertexArray_get_vertices, (setter)MGLVertexArray_set_vertices, 0, 0},
	{(char *)"instances", (getter)MGLVertexArray_get_instances, (setter)MGLVertexArray_set_instances, 0, 0},
	{(char *)"subroutines", 0, (setter)MGLVertexArray_set_subroutines, 0, 0},
	{0},
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
    {Py_tp_getset, MGLScope_getset},
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
