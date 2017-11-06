#include "UniformBlock.hpp"

#include "Error.hpp"

PyObject * MGLUniformBlock_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLUniformBlock * self = (MGLUniformBlock *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLUniformBlock_tp_dealloc(MGLUniformBlock * self) {

	Py_TYPE(self)->tp_free((PyObject *)self);
}

int MGLUniformBlock_tp_init(MGLUniformBlock * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.UniformBlock manually");
	return -1;
}

PyMethodDef MGLUniformBlock_tp_methods[] = {
	{0},
};

PyObject * MGLUniformBlock_get_name(MGLUniformBlock * self, void * closure) {
	Py_INCREF(self->name);
	return self->name;
}

PyObject * MGLUniformBlock_get_index(MGLUniformBlock * self, void * closure) {
	return PyLong_FromLong(self->index);
}

PyObject * MGLUniformBlock_get_size(MGLUniformBlock * self, void * closure) {
	return PyLong_FromLong(self->size);
}

PyObject * MGLUniformBlock_get_binding(MGLUniformBlock * self, void * closure) {
	int binding = 0;
	self->gl->GetActiveUniformBlockiv(self->program_obj, self->index, GL_UNIFORM_BLOCK_BINDING, &binding);
	return PyLong_FromLong(binding);
}

int MGLUniformBlock_set_binding(MGLUniformBlock * self, PyObject * value, void * closure) {
	int binding = PyLong_AsUnsignedLong(value);

	if (PyErr_Occurred()) {
		MGLError_Set("invalid value for binding");
		return -1;
	}

	self->gl->UniformBlockBinding(self->program_obj, self->index, binding);
	return 0;
}

PyGetSetDef MGLUniformBlock_tp_getseters[] = {
	{(char *)"name", (getter)MGLUniformBlock_get_name, 0, 0, 0},
	{(char *)"index", (getter)MGLUniformBlock_get_index, 0, 0, 0},
	{(char *)"size", (getter)MGLUniformBlock_get_size, 0, 0, 0},
	{(char *)"binding", (getter)MGLUniformBlock_get_binding, (setter)MGLUniformBlock_set_binding, 0, 0},
	{0},
};

PyTypeObject MGLUniformBlock_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.UniformBlock",                                     // tp_name
	sizeof(MGLUniformBlock),                                // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLUniformBlock_tp_dealloc,                 // tp_dealloc
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
	MGLUniformBlock_tp_methods,                             // tp_methods
	0,                                                      // tp_members
	MGLUniformBlock_tp_getseters,                           // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLUniformBlock_tp_init,                      // tp_init
	0,                                                      // tp_alloc
	MGLUniformBlock_tp_new,                                 // tp_new
};

void MGLUniformBlock_Complete(MGLUniformBlock * uniform_block, const GLMethods & gl) {
}
