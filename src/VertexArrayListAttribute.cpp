#include "VertexArrayListAttribute.hpp"

#include "Error.hpp"

PyObject * MGLVertexArrayListAttribute_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLVertexArrayListAttribute * self = (MGLVertexArrayListAttribute *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLVertexArrayListAttribute_tp_dealloc(MGLVertexArrayListAttribute * self) {
	Py_TYPE(self)->tp_free((PyObject *)self);
}

int MGLVertexArrayListAttribute_tp_init(MGLVertexArrayListAttribute * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.VertexArrayListAttribute manually");
	return -1;
}

PyMethodDef MGLVertexArrayListAttribute_tp_methods[] = {
	{0},
};

PyObject * MGLVertexArrayListAttribute_get_location(MGLVertexArrayListAttribute * self, void * closure) {
	return PyLong_FromLong(self->location);
}

PyGetSetDef MGLVertexArrayListAttribute_tp_getseters[] = {
	{(char *)"location", (getter)MGLVertexArrayListAttribute_get_location, 0, 0, 0},
	{0},
};

Py_ssize_t MGLVertexArrayListAttribute_tp_sequence_length(MGLVertexArrayListAttribute * self) {
	return PyTuple_GET_SIZE(self->content);
}

PyObject * MGLVertexArrayListAttribute_tp_sequence_item(MGLVertexArrayListAttribute * self, Py_ssize_t key) {
	PyObject * item = PyTuple_GetItem(self->content, key);

	if (item) {
		Py_INCREF(item);
	}

	return item;
}

PySequenceMethods MGLVertexArrayListAttribute_tp_sequence = {
	(lenfunc)MGLVertexArrayListAttribute_tp_sequence_length,         // sq_length
	0,                                                               // sq_concat
	0,                                                               // sq_repeat
	(ssizeargfunc)MGLVertexArrayListAttribute_tp_sequence_item,      // sq_item
	0,                                                               // sq_ass_item
	0,                                                               // sq_contains
	0,                                                               // sq_inplace_concat
	0,                                                               // sq_inplace_repeat
};

PyTypeObject MGLVertexArrayListAttribute_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.VertexArrayListAttribute",                         // tp_name
	sizeof(MGLVertexArrayListAttribute),                    // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLVertexArrayListAttribute_tp_dealloc,     // tp_dealloc
	0,                                                      // tp_print
	0,                                                      // tp_getattr
	0,                                                      // tp_setattr
	0,                                                      // tp_reserved
	0,                                                      // tp_repr
	0,                                                      // tp_as_number
	&MGLVertexArrayListAttribute_tp_sequence,               // tp_as_sequence
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
	MGLVertexArrayListAttribute_tp_methods,                 // tp_methods
	0,                                                      // tp_members
	MGLVertexArrayListAttribute_tp_getseters,               // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLVertexArrayListAttribute_tp_init,          // tp_init
	0,                                                      // tp_alloc
	MGLVertexArrayListAttribute_tp_new,                     // tp_new
};
