#include "Types.hpp"

PyObject * MGLSubroutine_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLSubroutine * self = (MGLSubroutine *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLSubroutine_tp_dealloc(MGLSubroutine * self) {
	MGLSubroutine_Type.tp_free((PyObject *)self);
}

int MGLSubroutine_tp_init(MGLSubroutine * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.Subroutine manually");
	return -1;
}

PyMethodDef MGLSubroutine_tp_methods[] = {
	{0},
};

PyObject * MGLSubroutine_get_name(MGLSubroutine * self, void * closure) {
	Py_INCREF(self->name);
	return self->name;
}

PyObject * MGLSubroutine_get_index(MGLSubroutine * self, void * closure) {
	return PyLong_FromUnsignedLong(self->index);
}

PyGetSetDef MGLSubroutine_tp_getseters[] = {
	{(char *)"name", (getter)MGLSubroutine_get_name, 0, 0, 0},
	{(char *)"index", (getter)MGLSubroutine_get_index, 0, 0, 0},
	{0},
};

PyTypeObject MGLSubroutine_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.Subroutine",                                       // tp_name
	sizeof(MGLSubroutine),                                  // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLSubroutine_tp_dealloc,                   // tp_dealloc
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
	MGLSubroutine_tp_methods,                               // tp_methods
	0,                                                      // tp_members
	MGLSubroutine_tp_getseters,                             // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLSubroutine_tp_init,                        // tp_init
	0,                                                      // tp_alloc
	MGLSubroutine_tp_new,                                   // tp_new
};

void MGLSubroutine_Invalidate(MGLSubroutine * subroutine) {
	Py_DECREF(subroutine->name);
	Py_TYPE(subroutine) = &MGLInvalidObject_Type;
	Py_DECREF(subroutine);
}

void MGLSubroutine_Complete(MGLSubroutine * subroutine) {
}
