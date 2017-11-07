#include "Types.hpp"

PyObject * MGLInvalidObject_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLInvalidObject * self = (MGLInvalidObject *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLInvalidObject_tp_dealloc(MGLInvalidObject * self) {
	MGLInvalidObject_Type.tp_free((PyObject *)self);
}

int MGLInvalidObject_tp_init(MGLInvalidObject * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.InvalidObject manually");
	return -1;
}

PyMethodDef MGLInvalidObject_tp_methods[] = {
	{0},
};

PyGetSetDef MGLInvalidObject_tp_getseters[] = {
	{0},
};

PyTypeObject MGLInvalidObject_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.InvalidObject",                                    // tp_name
	sizeof(MGLInvalidObject),                               // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLInvalidObject_tp_dealloc,                // tp_dealloc
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
	MGLInvalidObject_tp_methods,                            // tp_methods
	0,                                                      // tp_members
	MGLInvalidObject_tp_getseters,                          // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLInvalidObject_tp_init,                     // tp_init
	0,                                                      // tp_alloc
	MGLInvalidObject_tp_new,                                // tp_new
};
