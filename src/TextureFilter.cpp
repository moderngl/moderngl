#include "TextureFilter.hpp"

#include "Error.hpp"

PyObject * MGLTextureFilter_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLTextureFilter * self = (MGLTextureFilter *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLTextureFilter_tp_dealloc(MGLTextureFilter * self) {
	Py_TYPE(self)->tp_free((PyObject *)self);
}

int MGLTextureFilter_tp_init(MGLTextureFilter * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.TextureFilter manually");
	return -1;
}

PyMethodDef MGLTextureFilter_tp_methods[] = {
	{0},
};

int MGLTextureFilter_set_wrapper(MGLTextureFilter * self, PyObject * value, void * closure) {
	// if (self->wrapper) {
	// 	MGLError_Set("wrapper already set");
	// 	return -1;
	// }

	// Missing Py_INCREF is on purpose.
	self->wrapper = value;
	return 0;
}

PyGetSetDef MGLTextureFilter_tp_getseters[] = {
	{(char *)"wrapper", 0, (setter)MGLTextureFilter_set_wrapper, 0, 0},
	{0},
};

PyTypeObject MGLTextureFilter_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.TextureFilter",                                    // tp_name
	sizeof(MGLTextureFilter),                               // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLTextureFilter_tp_dealloc,                // tp_dealloc
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
	MGLTextureFilter_tp_methods,                            // tp_methods
	0,                                                      // tp_members
	MGLTextureFilter_tp_getseters,                          // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLTextureFilter_tp_init,                     // tp_init
	0,                                                      // tp_alloc
	MGLTextureFilter_tp_new,                                // tp_new
};

MGLTextureFilter * MGLTextureFilter_New() {
	MGLTextureFilter * self = (MGLTextureFilter *)MGLTextureFilter_tp_new(&MGLTextureFilter_Type, 0, 0);
	return self;
}

MGLTextureFilter * MGL_LINEAR;
MGLTextureFilter * MGL_NEAREST;
MGLTextureFilter * MGL_MIPMAP;
