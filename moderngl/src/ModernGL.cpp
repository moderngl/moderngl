#include "Types.hpp"
#include "Error.hpp"

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

	MGLContext * ctx = (MGLContext *)MGLContext_Type.tp_alloc(&MGLContext_Type, 0);

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
		MGLFramebuffer * framebuffer = (MGLFramebuffer *)MGLFramebuffer_Type.tp_alloc(&MGLFramebuffer_Type, 0);

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

bool MGL_InitializeModule(PyObject * module) {
	{
		if (PyType_Ready(&MGLAttribute_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register Attribute in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLAttribute_Type);

		PyModule_AddObject(module, "Attribute", (PyObject *)&MGLAttribute_Type);
	}

	{
		if (PyType_Ready(&MGLBuffer_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register Buffer in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLBuffer_Type);

		PyModule_AddObject(module, "Buffer", (PyObject *)&MGLBuffer_Type);
	}

	{
		if (PyType_Ready(&MGLComputeShader_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register ComputeShader in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLComputeShader_Type);

		PyModule_AddObject(module, "ComputeShader", (PyObject *)&MGLComputeShader_Type);
	}

	{
		if (PyType_Ready(&MGLContext_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register Context in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLContext_Type);

		PyModule_AddObject(module, "Context", (PyObject *)&MGLContext_Type);
	}

	{
		if (PyType_Ready(&MGLFramebuffer_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register Framebuffer in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLFramebuffer_Type);

		PyModule_AddObject(module, "Framebuffer", (PyObject *)&MGLFramebuffer_Type);
	}

	{
		if (PyType_Ready(&MGLInvalidObject_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register InvalidObject in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLInvalidObject_Type);

		PyModule_AddObject(module, "InvalidObject", (PyObject *)&MGLInvalidObject_Type);
	}

	{
		if (PyType_Ready(&MGLProgram_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register Program in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLProgram_Type);

		PyModule_AddObject(module, "Program", (PyObject *)&MGLProgram_Type);
	}

	{
		if (PyType_Ready(&MGLQuery_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register Query in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLQuery_Type);

		PyModule_AddObject(module, "Query", (PyObject *)&MGLQuery_Type);
	}

	{
		if (PyType_Ready(&MGLRenderbuffer_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register Renderbuffer in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLRenderbuffer_Type);

		PyModule_AddObject(module, "Renderbuffer", (PyObject *)&MGLRenderbuffer_Type);
	}

	{
		if (PyType_Ready(&MGLScope_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register Scope in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLScope_Type);

		PyModule_AddObject(module, "Scope", (PyObject *)&MGLScope_Type);
	}

	{
		if (PyType_Ready(&MGLTexture_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register Texture in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLTexture_Type);

		PyModule_AddObject(module, "Texture", (PyObject *)&MGLTexture_Type);
	}

	{
		if (PyType_Ready(&MGLTextureArray_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register TextureArray in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLTextureArray_Type);

		PyModule_AddObject(module, "TextureArray", (PyObject *)&MGLTextureArray_Type);
	}

	{
		if (PyType_Ready(&MGLTextureCube_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register TextureCube in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLTextureCube_Type);

		PyModule_AddObject(module, "TextureCube", (PyObject *)&MGLTextureCube_Type);
	}

	{
		if (PyType_Ready(&MGLTexture3D_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register Texture3D in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLTexture3D_Type);

		PyModule_AddObject(module, "Texture3D", (PyObject *)&MGLTexture3D_Type);
	}

	{
		if (PyType_Ready(&MGLUniform_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register Uniform in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLUniform_Type);

		PyModule_AddObject(module, "Uniform", (PyObject *)&MGLUniform_Type);
	}

	{
		if (PyType_Ready(&MGLUniformBlock_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register UniformBlock in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLUniformBlock_Type);

		PyModule_AddObject(module, "UniformBlock", (PyObject *)&MGLUniformBlock_Type);
	}

	{
		if (PyType_Ready(&MGLVertexArray_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register VertexArray in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLVertexArray_Type);

		PyModule_AddObject(module, "VertexArray", (PyObject *)&MGLVertexArray_Type);
	}

	{
		if (PyType_Ready(&MGLSampler_Type) < 0) {
			PyErr_Format(PyExc_ImportError, "Cannot register Sampler in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}

		Py_INCREF(&MGLSampler_Type);

		PyModule_AddObject(module, "Sampler", (PyObject *)&MGLSampler_Type);
	}

	return true;
}

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

	if (!MGL_InitializeModule(module)) {
		return 0;
	}

	return module;
}
