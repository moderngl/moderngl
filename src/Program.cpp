#include "Program.hpp"

#include "Error.hpp"
#include "InvalidObject.hpp"
#include "Shader.hpp"
#include "Uniform.hpp"
#include "UniformBlock.hpp"
#include "Attribute.hpp"
#include "Subroutine.hpp"
#include "SubroutineUniform.hpp"
#include "ProgramStage.hpp"
#include "Varying.hpp"

#include "InlineMethods.hpp"

PyObject * MGLProgram_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLProgram * self = (MGLProgram *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLProgram_tp_dealloc(MGLProgram * self) {
	MGLProgram_Type.tp_free((PyObject *)self);
}

int MGLProgram_tp_init(MGLProgram * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.Program manually");
	return -1;
}

PyObject * MGLProgram_release(MGLProgram * self) {
	MGLProgram_Invalidate(self);
	Py_RETURN_NONE;
}

PyMethodDef MGLProgram_tp_methods[] = {
	{"release", (PyCFunction)MGLProgram_release, METH_NOARGS, 0},
	{0},
};

PyObject * MGLProgram_get_shaders(MGLProgram * self, void * closure) {
	Py_INCREF(self->shaders);
	return self->shaders;
}

PyObject * MGLProgram_get_varyings(MGLProgram * self, void * closure) {
	Py_INCREF(self->varyings);
	return self->varyings;
}

PyObject * MGLProgram_get_uniforms(MGLProgram * self, void * closure) {
	Py_INCREF(self->uniforms);
	return self->uniforms;
}

PyObject * MGLProgram_get_uniform_blocks(MGLProgram * self, void * closure) {
	Py_INCREF(self->uniform_blocks);
	return self->uniform_blocks;
}

PyObject * MGLProgram_get_attributes(MGLProgram * self, void * closure) {
	Py_INCREF(self->attributes);
	return self->attributes;
}

PyObject * MGLProgram_get_geometry_input(MGLProgram * self, void * closure) {
	Py_INCREF(self->geometry_input->wrapper);
	return (PyObject *)self->geometry_input->wrapper;
}

PyObject * MGLProgram_get_geometry_output(MGLProgram * self, void * closure) {
	Py_INCREF(self->geometry_output->wrapper);
	return (PyObject *)self->geometry_output->wrapper;
}

PyObject * MGLProgram_get_geometry_vertices(MGLProgram * self, void * closure) {
	return PyLong_FromLong(self->geometry_vertices);
}

PyObject * MGLProgram_get_vertex_shader(MGLProgram * self, void * closure) {
	if (self->vertex_shader) {
		Py_INCREF(self->vertex_shader);
		return (PyObject *)self->vertex_shader;
	} else {
		Py_RETURN_NONE;
	}
}

PyObject * MGLProgram_get_fragment_shader(MGLProgram * self, void * closure) {
	if (self->fragment_shader) {
		Py_INCREF(self->fragment_shader);
		return (PyObject *)self->fragment_shader;
	} else {
		Py_RETURN_NONE;
	}
}

PyObject * MGLProgram_get_geometry_shader(MGLProgram * self, void * closure) {
	if (self->geometry_shader) {
		Py_INCREF(self->geometry_shader);
		return (PyObject *)self->geometry_shader;
	} else {
		Py_RETURN_NONE;
	}
}

PyObject * MGLProgram_get_tess_evaluation_shader(MGLProgram * self, void * closure) {
	if (self->tess_evaluation_shader) {
		Py_INCREF(self->tess_evaluation_shader);
		return (PyObject *)self->tess_evaluation_shader;
	} else {
		Py_RETURN_NONE;
	}
}

PyObject * MGLProgram_get_tess_control_shader(MGLProgram * self, void * closure) {
	if (self->tess_control_shader) {
		Py_INCREF(self->tess_control_shader);
		return (PyObject *)self->tess_control_shader;
	} else {
		Py_RETURN_NONE;
	}
}

MGLContext * MGLProgram_get_context(MGLProgram * self, void * closure) {
	Py_INCREF(self->context);
	return self->context;
}

PyObject * MGLProgram_get_glo(MGLProgram * self, void * closure) {
	return PyLong_FromLong(self->program_obj);
}

PyGetSetDef MGLProgram_tp_getseters[] = {
	{(char *)"uniforms", (getter)MGLProgram_get_uniforms, 0, 0, 0},
	{(char *)"uniform_blocks", (getter)MGLProgram_get_uniform_blocks, 0, 0, 0},
	{(char *)"attributes", (getter)MGLProgram_get_attributes, 0, 0, 0},
	{(char *)"varyings", (getter)MGLProgram_get_varyings, 0, 0, 0},

	{(char *)"geometry_input", (getter)MGLProgram_get_geometry_input, 0, 0, 0},
	{(char *)"geometry_output", (getter)MGLProgram_get_geometry_output, 0, 0, 0},
	{(char *)"geometry_vertices", (getter)MGLProgram_get_geometry_vertices, 0, 0, 0},

	{(char *)"vertex_shader", (getter)MGLProgram_get_vertex_shader, 0, 0, 0},
	{(char *)"fragment_shader", (getter)MGLProgram_get_fragment_shader, 0, 0, 0},
	{(char *)"geometry_shader", (getter)MGLProgram_get_geometry_shader, 0, 0, 0},
	{(char *)"tess_evaluation_shader", (getter)MGLProgram_get_tess_evaluation_shader, 0, 0, 0},
	{(char *)"tess_control_shader", (getter)MGLProgram_get_tess_control_shader, 0, 0, 0},
	{(char *)"context", (getter)MGLProgram_get_context, 0, 0, 0},
	{(char *)"glo", (getter)MGLProgram_get_glo, 0, 0, 0},
	{0},
};

PyTypeObject MGLProgram_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.Program",                                          // tp_name
	sizeof(MGLProgram),                                     // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLProgram_tp_dealloc,                      // tp_dealloc
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
	MGLProgram_tp_methods,                                  // tp_methods
	0,                                                      // tp_members
	MGLProgram_tp_getseters,                                // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLProgram_tp_init,                           // tp_init
	0,                                                      // tp_alloc
	MGLProgram_tp_new,                                      // tp_new
};

void MGLProgram_Invalidate(MGLProgram * program) {
	if (Py_TYPE(program) == &MGLInvalidObject_Type) {
		return;
	}

	const GLMethods & gl = program->context->gl;

	gl.DeleteProgram(program->program_obj);

	{
		MGLUniform * uniform = 0;
		Py_ssize_t pos = 0;

		while (PyDict_Next(program->uniforms, &pos, 0, (PyObject **)&uniform)) {
			MGLUniform_Invalidate(uniform);
		}

		Py_DECREF(program->uniforms);
	}

	{
		MGLAttribute * attribute = 0;
		Py_ssize_t pos = 0;

		while (PyDict_Next(program->attributes, &pos, 0, (PyObject **)&attribute)) {
			MGLAttribute_Invalidate(attribute);
		}

		Py_DECREF(program->attributes);
	}

	{
		MGLVarying * varying = 0;
		Py_ssize_t pos = 0;

		while (PyDict_Next(program->varyings, &pos, 0, (PyObject **)&varying)) {
			MGLVarying_Invalidate(varying);
		}

		Py_DECREF(program->varyings);
	}

	{
		int shaders_len = (int)PyTuple_GET_SIZE(program->shaders);

		for (int i = 0; i < shaders_len; ++i) {
			MGLShader * shader = (MGLShader *)PyTuple_GET_ITEM(program->shaders, i);
			if (Py_REFCNT(shader) == 2) {
				MGLShader_Invalidate(shader);
			}
		}

		Py_DECREF(program->shaders);
	}

	Py_XDECREF(program->geometry_input);
	Py_XDECREF(program->geometry_output);

	Py_DECREF(program->context);

	Py_TYPE(program) = &MGLInvalidObject_Type;

	Py_DECREF(program);
}

void MGLProgram_Compile(MGLProgram * program, PyObject * outputs) {
	const GLMethods & gl = program->context->gl;

	int obj = gl.CreateProgram();

	if (!obj) {
		MGLError_Set("cannot create program");
		return;
	}

	MGLShader * shaders[NUM_SHADER_SLOTS] = {};

	int num_shaders = (int)PyTuple_GET_SIZE(program->shaders);

	for (int i = 0; i < num_shaders; ++i) {
		MGLShader * shader = (MGLShader *)PyTuple_GET_ITEM(program->shaders, i);
		shaders[shader->shader_slot] = shader;

		gl.AttachShader(obj, shader->shader_obj);
	}

	int outputs_len = (int)PyTuple_GET_SIZE(outputs);

	if (outputs_len) {
		const char ** varyings_array = new const char * [outputs_len];

		for (int i = 0; i < outputs_len; ++i) {
			varyings_array[i] = PyUnicode_AsUTF8(PyTuple_GET_ITEM(outputs, i));
		}

		gl.TransformFeedbackVaryings(obj, outputs_len, varyings_array, GL_INTERLEAVED_ATTRIBS);

		delete[] varyings_array;
	}

	gl.LinkProgram(obj);

	int linked = GL_FALSE;
	gl.GetProgramiv(obj, GL_LINK_STATUS, &linked);

	if (!linked) {
		const char * message = "GLSL Linker failed";
		const char * title = "Program";
		const char * underline = "=======";

		int log_len = 0;
		gl.GetProgramiv(obj, GL_INFO_LOG_LENGTH, &log_len);

		char * log = new char[log_len];
		gl.GetProgramInfoLog(obj, log_len, &log_len, log);

		gl.DeleteProgram(obj);

		MGLError_Set("%s\n\n%s\n%s\n%s\n", message, title, underline, log);

		delete[] log;
		return;
	}

	program->program_obj = obj;

	int location_base = 0;

	if (shaders[VERTEX_SHADER_SLOT]) {
		MGLProgramStage * program_stage = (MGLProgramStage *)MGLProgramStage_tp_new(&MGLProgramStage_Type, 0, 0);
		MGLProgramStage_Complete(program_stage, GL_VERTEX_SHADER, obj, location_base, gl);
		program->vertex_shader = program_stage;
	} else {
		program->vertex_shader = 0;
	}

	if (shaders[FRAGMENT_SHADER_SLOT]) {
		MGLProgramStage * program_stage = (MGLProgramStage *)MGLProgramStage_tp_new(&MGLProgramStage_Type, 0, 0);
		MGLProgramStage_Complete(program_stage, GL_FRAGMENT_SHADER, obj, location_base, gl);
		program->fragment_shader = program_stage;
	} else {
		program->fragment_shader = 0;
	}

	if (shaders[GEOMETRY_SHADER_SLOT]) {
		MGLProgramStage * program_stage = (MGLProgramStage *)MGLProgramStage_tp_new(&MGLProgramStage_Type, 0, 0);
		MGLProgramStage_Complete(program_stage, GL_GEOMETRY_SHADER, obj, location_base, gl);
		program->geometry_shader = program_stage;
	} else {
		program->geometry_shader = 0;
	}

	if (shaders[TESS_EVALUATION_SHADER_SLOT]) {
		MGLProgramStage * program_stage = (MGLProgramStage *)MGLProgramStage_tp_new(&MGLProgramStage_Type, 0, 0);
		MGLProgramStage_Complete(program_stage, GL_TESS_EVALUATION_SHADER, obj, location_base, gl);
		program->tess_evaluation_shader = program_stage;
	} else {
		program->tess_evaluation_shader = 0;
	}

	if (shaders[TESS_CONTROL_SHADER_SLOT]) {
		MGLProgramStage * program_stage = (MGLProgramStage *)MGLProgramStage_tp_new(&MGLProgramStage_Type, 0, 0);
		MGLProgramStage_Complete(program_stage, GL_TESS_CONTROL_SHADER, obj, location_base, gl);
		program->tess_control_shader = program_stage;
	} else {
		program->tess_control_shader = 0;
	}

	program->num_vertex_shader_subroutines = 0;
	program->num_fragment_shader_subroutines = 0;
	program->num_geometry_shader_subroutines = 0;
	program->num_tess_evaluation_shader_subroutines = 0;
	program->num_tess_control_shader_subroutines = 0;

	if (program->context->version_code >= 400) {
		if (shaders[VERTEX_SHADER_SLOT]) {
			gl.GetProgramStageiv(
				obj,
				GL_VERTEX_SHADER,
				GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
				&program->num_vertex_shader_subroutines
			);
		}

		if (shaders[FRAGMENT_SHADER_SLOT]) {
			gl.GetProgramStageiv(
				obj,
				GL_FRAGMENT_SHADER,
				GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
				&program->num_fragment_shader_subroutines
			);
		}

		if (shaders[GEOMETRY_SHADER_SLOT]) {
			gl.GetProgramStageiv(
				obj,
				GL_GEOMETRY_SHADER,
				GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
				&program->num_geometry_shader_subroutines
			);
		}

		if (shaders[TESS_EVALUATION_SHADER_SLOT]) {
			gl.GetProgramStageiv(
				obj,
				GL_TESS_EVALUATION_SHADER,
				GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
				&program->num_tess_evaluation_shader_subroutines
			);
		}

		if (shaders[TESS_CONTROL_SHADER_SLOT]) {
			gl.GetProgramStageiv(
				obj,
				GL_TESS_CONTROL_SHADER,
				GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
				&program->num_tess_control_shader_subroutines
			);
		}
	}

	PyObject * uniforms = PyDict_New();

	int num_uniforms = 0;
	gl.GetProgramiv(obj, GL_ACTIVE_UNIFORMS, &num_uniforms);

	for (int i = 0; i < num_uniforms; ++i) {
		MGLUniform * uniform = (MGLUniform *)MGLUniform_tp_new(&MGLUniform_Type, 0, 0);

		uniform->context = program->context;

		int name_len = 0;
		char name[256];

		gl.GetActiveUniform(obj, i, 256, &name_len, &uniform->array_length, (GLenum *)&uniform->type, name);

		uniform->location = gl.GetUniformLocation(obj, name);

		clean_glsl_name(name, name_len);

		// Skip uniforms from uniform buffers

		if (uniform->location < 0) {
			Py_DECREF((PyObject *)uniform);
			continue;
		}

		uniform->number = i;
		uniform->program_obj = obj;
		uniform->name = PyUnicode_FromStringAndSize(name, name_len);

		MGLUniform_Complete(uniform, gl);

		PyDict_SetItem(uniforms, uniform->name, (PyObject *)uniform);
		Py_DECREF(uniform);
	}

	program->uniforms = uniforms;
	Py_INCREF(uniforms);

	PyObject * uniform_blocks = PyDict_New();

	int num_uniform_blocks = 0;
	gl.GetProgramiv(obj, GL_ACTIVE_UNIFORM_BLOCKS, &num_uniform_blocks);

	for (int i = 0; i < num_uniform_blocks; ++i) {
		MGLUniformBlock * uniform_block = (MGLUniformBlock *)MGLUniformBlock_tp_new(&MGLUniformBlock_Type, 0, 0);

		int name_len = 0;
		char name[256];

		gl.GetActiveUniformBlockName(obj, i, 256, &name_len, name);
		uniform_block->index = gl.GetUniformBlockIndex(obj, name);

		gl.GetActiveUniformBlockiv(obj, uniform_block->index, GL_UNIFORM_BLOCK_DATA_SIZE, &uniform_block->size);

		clean_glsl_name(name, name_len);

		uniform_block->gl = &gl;
		uniform_block->program_obj = obj;
		uniform_block->name = PyUnicode_FromStringAndSize(name, name_len);

		MGLUniformBlock_Complete(uniform_block, gl);

		PyDict_SetItem(uniform_blocks, uniform_block->name, (PyObject *)uniform_block);
		Py_DECREF(uniform_block);
	}

	program->uniform_blocks = uniform_blocks;
	Py_INCREF(uniform_blocks);

	PyObject * attributes = PyDict_New();

	int num_attributes = 0;
	gl.GetProgramiv(obj, GL_ACTIVE_ATTRIBUTES, &num_attributes);

	for (int i = 0; i < num_attributes; ++i) {
		MGLAttribute * attribute = (MGLAttribute *)MGLAttribute_tp_new(&MGLAttribute_Type, 0, 0);

		attribute->context = program->context;

		int name_len = 0;
		char name[256];

		gl.GetActiveAttrib(obj, i, 256, &name_len, &attribute->array_length, (GLenum *)&attribute->type, name);

		attribute->location = gl.GetAttribLocation(obj, name);

		clean_glsl_name(name, name_len);

		attribute->number = i;
		attribute->program_obj = obj;
		attribute->name = PyUnicode_FromStringAndSize(name, name_len);

		MGLAttribute_Complete(attribute, gl);

		PyDict_SetItem(attributes, attribute->name, (PyObject *)attribute);
		Py_DECREF(attribute);
	}

	program->attributes = attributes;
	Py_INCREF(attributes);

	PyObject * varyings = PyDict_New();

	gl.GetProgramiv(obj, GL_TRANSFORM_FEEDBACK_VARYINGS, &program->num_varyings);

	for (int i = 0; i < program->num_varyings; ++i) {
		MGLVarying * varying = (MGLVarying *)MGLVarying_tp_new(&MGLVarying_Type, 0, 0);

		int name_len = 0;
		char name[256];

		gl.GetTransformFeedbackVarying(obj, i, 256, &name_len, &varying->array_length, (GLenum *)&varying->type, name);

		varying->number = i;
		varying->name = PyUnicode_FromStringAndSize(name, name_len);

		MGLVarying_Complete(varying, gl);

		PyDict_SetItem(varyings, varying->name, (PyObject *)varying);
		Py_DECREF(varying);
	}

	program->varyings = varyings;
	Py_INCREF(varyings);

	if (shaders[GEOMETRY_SHADER_SLOT]) {

		int geometry_in = 0;
		int geometry_out = 0;
		program->geometry_vertices = 0;

		gl.GetProgramiv(obj, GL_GEOMETRY_INPUT_TYPE, &geometry_in);
		gl.GetProgramiv(obj, GL_GEOMETRY_OUTPUT_TYPE, &geometry_out);
		gl.GetProgramiv(obj, GL_GEOMETRY_VERTICES_OUT, &program->geometry_vertices);

		switch (geometry_in) {
			case GL_TRIANGLES:
				program->geometry_input = MGL_TRIANGLES;
				break;

			case GL_TRIANGLE_STRIP:
				program->geometry_input = MGL_TRIANGLE_STRIP;
				break;

			case GL_TRIANGLE_FAN:
				program->geometry_input = MGL_TRIANGLE_FAN;
				break;

			case GL_LINES:
				program->geometry_input = MGL_LINES;
				break;

			case GL_LINE_STRIP:
				program->geometry_input = MGL_LINE_STRIP;
				break;

			case GL_LINE_LOOP:
				program->geometry_input = MGL_LINE_LOOP;
				break;

			case GL_POINTS:
				program->geometry_input = MGL_POINTS;
				break;

			case GL_LINE_STRIP_ADJACENCY:
				program->geometry_input = MGL_LINE_STRIP_ADJACENCY;
				break;

			case GL_LINES_ADJACENCY:
				program->geometry_input = MGL_LINES_ADJACENCY;
				break;

			case GL_TRIANGLE_STRIP_ADJACENCY:
				program->geometry_input = MGL_TRIANGLE_STRIP_ADJACENCY;
				break;

			case GL_TRIANGLES_ADJACENCY:
				program->geometry_input = MGL_TRIANGLES_ADJACENCY;
				break;

			default:
				program->geometry_input = MGL_NO_PRIMITIVE;
				break;
		}

		switch (geometry_out) {
			case GL_TRIANGLES:
				program->geometry_output = MGL_TRIANGLES;
				break;

			case GL_TRIANGLE_STRIP:
				program->geometry_output = MGL_TRIANGLE_STRIP;
				break;

			case GL_TRIANGLE_FAN:
				program->geometry_output = MGL_TRIANGLE_FAN;
				break;

			case GL_LINES:
				program->geometry_output = MGL_LINES;
				break;

			case GL_LINE_STRIP:
				program->geometry_output = MGL_LINE_STRIP;
				break;

			case GL_LINE_LOOP:
				program->geometry_output = MGL_LINE_LOOP;
				break;

			case GL_POINTS:
				program->geometry_output = MGL_POINTS;
				break;

			case GL_LINE_STRIP_ADJACENCY:
				program->geometry_output = MGL_LINE_STRIP_ADJACENCY;
				break;

			case GL_LINES_ADJACENCY:
				program->geometry_output = MGL_LINES_ADJACENCY;
				break;

			case GL_TRIANGLE_STRIP_ADJACENCY:
				program->geometry_output = MGL_TRIANGLE_STRIP_ADJACENCY;
				break;

			case GL_TRIANGLES_ADJACENCY:
				program->geometry_output = MGL_TRIANGLES_ADJACENCY;
				break;

			default:
				program->geometry_output = MGL_NO_PRIMITIVE;
				break;
		}

		Py_INCREF(program->geometry_input);
		Py_INCREF(program->geometry_output);

	} else {

		program->geometry_input = MGL_NO_PRIMITIVE;
		program->geometry_output = MGL_NO_PRIMITIVE;
		program->geometry_vertices = 0;
	}
}
