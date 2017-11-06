#include "Primitive.hpp"

#include "Error.hpp"

PyObject * MGLPrimitive_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLPrimitive * self = (MGLPrimitive *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLPrimitive_tp_dealloc(MGLPrimitive * self) {
	Py_TYPE(self)->tp_free((PyObject *)self);
}

int MGLPrimitive_tp_init(MGLPrimitive * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.Primitive manually");
	return -1;
}

PyMethodDef MGLPrimitive_tp_methods[] = {
	{0},
};

int MGLPrimitive_set_wrapper(MGLPrimitive * self, PyObject * value, void * closure) {
	// if (self->wrapper) {
	// 	MGLError_Set("wrapper already set");
	// 	return -1;
	// }

	// Missing Py_INCREF is on purpose.
	self->wrapper = value;
	return 0;
}

PyGetSetDef MGLPrimitive_tp_getseters[] = {
	{(char *)"wrapper", 0, (setter)MGLPrimitive_set_wrapper, 0, 0},
	{0},
};

PyTypeObject MGLPrimitive_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.Primitive",                                        // tp_name
	sizeof(MGLPrimitive),                                   // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLPrimitive_tp_dealloc,                    // tp_dealloc
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
	MGLPrimitive_tp_methods,                                // tp_methods
	0,                                                      // tp_members
	MGLPrimitive_tp_getseters,                              // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLPrimitive_tp_init,                         // tp_init
	0,                                                      // tp_alloc
	MGLPrimitive_tp_new,                                    // tp_new
};

MGLPrimitive * MGLPrimitive_New() {
	MGLPrimitive * self = (MGLPrimitive *)MGLPrimitive_tp_new(&MGLPrimitive_Type, 0, 0);
	return self;
}

MGLPrimitive * MGL_TRIANGLES;
MGLPrimitive * MGL_TRIANGLE_STRIP;
MGLPrimitive * MGL_TRIANGLE_FAN;
MGLPrimitive * MGL_LINES;
MGLPrimitive * MGL_LINE_STRIP;
MGLPrimitive * MGL_LINE_LOOP;
MGLPrimitive * MGL_POINTS;
MGLPrimitive * MGL_LINE_STRIP_ADJACENCY;
MGLPrimitive * MGL_LINES_ADJACENCY;
MGLPrimitive * MGL_TRIANGLE_STRIP_ADJACENCY;
MGLPrimitive * MGL_TRIANGLES_ADJACENCY;
MGLPrimitive * MGL_NO_PRIMITIVE;
