#include "Types.hpp"

PyObject * MGLSubroutineUniform_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLSubroutineUniform * self = (MGLSubroutineUniform *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLSubroutineUniform_tp_dealloc(MGLSubroutineUniform * self) {
	Py_TYPE(self)->tp_free((PyObject *)self);
}

int MGLSubroutineUniform_tp_init(MGLSubroutineUniform * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.SubroutineUniform manually");
	return -1;
}

PyMethodDef MGLSubroutineUniform_tp_methods[] = {
	{0},
};

PyObject * MGLSubroutineUniform_get_name(MGLSubroutineUniform * self, void * closure) {
	Py_INCREF(self->name);
	return self->name;
}

PyObject * MGLSubroutineUniform_get_location(MGLSubroutineUniform * self, void * closure) {
	return PyLong_FromUnsignedLong(self->location);
}

PyGetSetDef MGLSubroutineUniform_tp_getseters[] = {
	{(char *)"name", (getter)MGLSubroutineUniform_get_name, 0, 0, 0},
	{(char *)"location", (getter)MGLSubroutineUniform_get_location, 0, 0, 0},
	{0},
};

PyTypeObject MGLSubroutineUniform_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.SubroutineUniform",                                // tp_name
	sizeof(MGLSubroutineUniform),                           // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLSubroutineUniform_tp_dealloc,            // tp_dealloc
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
	MGLSubroutineUniform_tp_methods,                        // tp_methods
	0,                                                      // tp_members
	MGLSubroutineUniform_tp_getseters,                      // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLSubroutineUniform_tp_init,                 // tp_init
	0,                                                      // tp_alloc
	MGLSubroutineUniform_tp_new,                            // tp_new
};

void MGLSubroutineUniform_Complete(MGLSubroutineUniform * subroutine_uniform) {
}
