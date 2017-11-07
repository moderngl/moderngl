#include "Types.hpp"

PyObject * MGLVarying_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLVarying * self = (MGLVarying *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLVarying_tp_dealloc(MGLVarying * self) {

	MGLVarying_Type.tp_free((PyObject *)self);
}

int MGLVarying_tp_init(MGLVarying * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.Varying manually");
	return -1;
}

PyMethodDef MGLVarying_tp_methods[] = {
	{0},
};

PyObject * MGLVarying_get_name(MGLVarying * self, void * closure) {
	Py_INCREF(self->name);
	return self->name;
}

PyObject * MGLVarying_get_number(MGLVarying * self, void * closure) {
	return PyLong_FromLong(self->number);
}

PyGetSetDef MGLVarying_tp_getseters[] = {
	{(char *)"name", (getter)MGLVarying_get_name, 0, 0, 0},
	{(char *)"number", (getter)MGLVarying_get_number, 0, 0, 0},
	{0},
};

PyTypeObject MGLVarying_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.Varying",                                          // tp_name
	sizeof(MGLVarying),                                     // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLVarying_tp_dealloc,                      // tp_dealloc
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
	MGLVarying_tp_methods,                                  // tp_methods
	0,                                                      // tp_members
	MGLVarying_tp_getseters,                                // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLVarying_tp_init,                           // tp_init
	0,                                                      // tp_alloc
	MGLVarying_tp_new,                                      // tp_new
};

void MGLVarying_Invalidate(MGLVarying * varying) {
	Py_DECREF(varying->name);
	Py_TYPE(varying) = &MGLInvalidObject_Type;
	Py_DECREF(varying);
}

void MGLVarying_Complete(MGLVarying * self, const GLMethods & gl) {
}
