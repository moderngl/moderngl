#include "Types.hpp"

#include "InlineMethods.hpp"

PyObject * MGLComputeShader_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLComputeShader * self = (MGLComputeShader *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLComputeShader_tp_dealloc(MGLComputeShader * self) {
	Py_TYPE(self)->tp_free((PyObject *)self);
}

int MGLComputeShader_tp_init(MGLComputeShader * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.ComputeShader manually");
	return -1;
}

PyObject * MGLComputeShader_run(MGLComputeShader * self, PyObject * args) {
	unsigned x;
	unsigned y;
	unsigned z;

	int args_ok = PyArg_ParseTuple(
		args,
		"III",
		&x,
		&y,
		&z
	);

	if (!args_ok) {
		return 0;
	}

	const GLMethods & gl = self->context->gl;

	gl.UseProgram(self->program_obj);
	gl.DispatchCompute(x, y, z);

	Py_RETURN_NONE;
}

PyMethodDef MGLComputeShader_tp_methods[] = {
	{"run", (PyCFunction)MGLComputeShader_run, METH_VARARGS, 0},
	{0},
};

PyObject * MGLComputeShader_get_uniforms(MGLComputeShader * self, void * closure) {
	Py_INCREF(self->uniforms);
	return self->uniforms;
}

PyObject * MGLComputeShader_get_uniform_blocks(MGLComputeShader * self, void * closure) {
	Py_INCREF(self->uniform_blocks);
	return self->uniform_blocks;
}

PyObject * MGLComputeShader_get_subroutines(MGLComputeShader * self, void * closure) {
	Py_INCREF(self->subroutines);
	return self->subroutines;
}

PyObject * MGLComputeShader_get_subroutine_uniforms(MGLComputeShader * self, void * closure) {
	Py_INCREF(self->subroutine_uniforms);
	return self->subroutine_uniforms;
}

PyObject * MGLComputeShader_get_source(MGLComputeShader * self, void * closure) {
	Py_INCREF(self->source);
	return self->source;
}

MGLContext * MGLComputeShader_get_context(MGLComputeShader * self, void * closure) {
	Py_INCREF(self->context);
	return self->context;
}

PyObject * MGLComputeShader_get_glo(MGLComputeShader * self, void * closure) {
	return PyLong_FromLong(self->program_obj);
}

PyGetSetDef MGLComputeShader_tp_getseters[] = {
	{(char *)"uniforms", (getter)MGLComputeShader_get_uniforms, 0, 0, 0},
	{(char *)"uniform_blocks", (getter)MGLComputeShader_get_uniform_blocks, 0, 0, 0},
	{(char *)"subroutines", (getter)MGLComputeShader_get_subroutines, 0, 0, 0},
	{(char *)"subroutine_uniforms", (getter)MGLComputeShader_get_subroutine_uniforms, 0, 0, 0},
	{(char *)"source", (getter)MGLComputeShader_get_source, 0, 0, 0},
	{(char *)"context", (getter)MGLComputeShader_get_context, 0, 0, 0},
	{(char *)"glo", (getter)MGLComputeShader_get_glo, 0, 0, 0},
	{0},
};

PyTypeObject MGLComputeShader_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.ComputeShader",                                    // tp_name
	sizeof(MGLComputeShader),                               // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLComputeShader_tp_dealloc,                // tp_dealloc
	0,                                                      // tp_print
	0,                                                      // tp_getattr
	0,                                                      // tp_setattr
	0,                                                      // tp_reserved
	0,                                                      // tp_repr
	0,                                                      // tp_as_number
	0,                                                      // tp_as_sequence
	0,                                                      // tp_as_mapping
	0,                                                      // tp_hash
	0,                                                      // tp_call
	0,                                                      // tp_str
	0,                                                      // tp_getattro
	0,                                                      // tp_setattro
	0,                                                      // tp_as_buffer
	Py_TPFLAGS_DEFAULT,                                     // tp_flags
	0,                                                      // tp_doc
	0,                                                      // tp_traverse
	0,                                                      // tp_clear
	0,                                                      // tp_richcompare
	0,                                                      // tp_weaklistoffset
	0,                                                      // tp_iter
	0,                                                      // tp_iternext
	MGLComputeShader_tp_methods,                            // tp_methods
	0,                                                      // tp_members
	MGLComputeShader_tp_getseters,                          // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLComputeShader_tp_init,                     // tp_init
	0,                                                      // tp_alloc
	MGLComputeShader_tp_new,                                // tp_new
};

void MGLComputeShader_Compile(MGLComputeShader * compute_shader) {
	const char * source_str = PyUnicode_AsUTF8(compute_shader->source);

	const GLMethods & gl = compute_shader->context->gl;

	int shader_obj = gl.CreateShader(GL_COMPUTE_SHADER);

	if (!shader_obj) {
		MGLError_Set("cannot create the shader object");
		return;
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
		return;
	}

	compute_shader->shader_obj = shader_obj;

	int program_obj = gl.CreateProgram();

	if (!program_obj) {
		MGLError_Set("cannot create program");
		return;
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
		return;
	}

	compute_shader->program_obj = program_obj;

	// Fill

	PyObject * uniforms = PyDict_New();

	int num_uniforms = 0;
	gl.GetProgramiv(program_obj, GL_ACTIVE_UNIFORMS, &num_uniforms);

	for (int i = 0; i < num_uniforms; ++i) {
		MGLUniform * uniform = (MGLUniform *)MGLUniform_Type.tp_alloc(&MGLUniform_Type, 0);

		uniform->context = compute_shader->context;

		int name_len = 0;
		char name[256];

		gl.GetActiveUniform(program_obj, i, 256, &name_len, &uniform->array_length, (GLenum *)&uniform->type, name);

		uniform->location = gl.GetUniformLocation(program_obj, name);

		clean_glsl_name(name, name_len);

		// Skip uniforms from uniform buffers

		if (uniform->location < 0) {
			Py_DECREF((PyObject *)uniform);
			continue;
		}

		uniform->number = i;
		uniform->program_obj = program_obj;
		uniform->name = PyUnicode_FromStringAndSize(name, name_len);

		MGLUniform_Complete(uniform, gl);

		PyDict_SetItem(uniforms, uniform->name, (PyObject *)uniform);
		Py_DECREF(uniform);
	}

	compute_shader->uniforms = uniforms;
	Py_INCREF(uniforms);

	PyObject * uniform_blocks = PyDict_New();

	int num_uniform_blocks = 0;
	gl.GetProgramiv(program_obj, GL_ACTIVE_UNIFORM_BLOCKS, &num_uniform_blocks);

	for (int i = 0; i < num_uniform_blocks; ++i) {
		MGLUniformBlock * uniform_block = (MGLUniformBlock *)MGLUniformBlock_Type.tp_alloc(&MGLUniformBlock_Type, 0);

		int name_len = 0;
		char name[256];

		gl.GetActiveUniformBlockName(program_obj, i, 256, &name_len, name);
		uniform_block->index = gl.GetUniformBlockIndex(program_obj, name);

		gl.GetActiveUniformBlockiv(program_obj, uniform_block->index, GL_UNIFORM_BLOCK_DATA_SIZE, &uniform_block->size);

		clean_glsl_name(name, name_len);

		uniform_block->gl = &gl;
		uniform_block->program_obj = program_obj;
		uniform_block->name = PyUnicode_FromStringAndSize(name, name_len);

		MGLUniformBlock_Complete(uniform_block, gl);

		PyDict_SetItem(uniform_blocks, uniform_block->name, (PyObject *)uniform_block);
		Py_DECREF(uniform_block);
	}

	compute_shader->uniform_blocks = uniform_blocks;
	Py_INCREF(uniform_blocks);
}
