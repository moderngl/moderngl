#include "Types.hpp"

PyObject * MGLRenderbuffer_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLRenderbuffer * self = (MGLRenderbuffer *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLRenderbuffer_tp_dealloc(MGLRenderbuffer * self) {
	MGLRenderbuffer_Type.tp_free((PyObject *)self);
}

int MGLRenderbuffer_tp_init(MGLRenderbuffer * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.Renderbuffer manually");
	return -1;
}

PyObject * MGLRenderbuffer_release(MGLRenderbuffer * self) {
	MGLRenderbuffer_Invalidate(self);
	Py_RETURN_NONE;
}

PyMethodDef MGLRenderbuffer_tp_methods[] = {
	{"release", (PyCFunction)MGLRenderbuffer_release, METH_NOARGS, 0},
	{0},
};

PyObject * MGLRenderbuffer_get_width(MGLRenderbuffer * self, void * closure) {
	return PyLong_FromLong(self->width);
}

PyObject * MGLRenderbuffer_get_height(MGLRenderbuffer * self, void * closure) {
	return PyLong_FromLong(self->height);
}

PyObject * MGLRenderbuffer_get_components(MGLRenderbuffer * self, void * closure) {
	return PyLong_FromLong(self->components);
}

PyObject * MGLRenderbuffer_get_samples(MGLRenderbuffer * self, void * closure) {
	return PyLong_FromLong(self->samples);
}

PyObject * MGLRenderbuffer_get_depth(MGLRenderbuffer * self, void * closure) {
	return PyBool_FromLong(self->depth);
}

MGLContext * MGLRenderbuffer_get_context(MGLRenderbuffer * self, void * closure) {
	Py_INCREF(self->context);
	return self->context;
}

PyObject * MGLRenderbuffer_get_glo(MGLRenderbuffer * self, void * closure) {
	return PyLong_FromLong(self->renderbuffer_obj);
}

PyGetSetDef MGLRenderbuffer_tp_getseters[] = {
	{(char *)"width", (getter)MGLRenderbuffer_get_width, 0, 0, 0},
	{(char *)"height", (getter)MGLRenderbuffer_get_height, 0, 0, 0},
	{(char *)"components", (getter)MGLRenderbuffer_get_components, 0, 0, 0},
	{(char *)"samples", (getter)MGLRenderbuffer_get_samples, 0, 0, 0},
	{(char *)"depth", (getter)MGLRenderbuffer_get_depth, 0, 0, 0},
	{(char *)"context", (getter)MGLRenderbuffer_get_context, 0, 0, 0},
	{(char *)"glo", (getter)MGLRenderbuffer_get_glo, 0, 0, 0},
	{0},
};

PyTypeObject MGLRenderbuffer_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.Renderbuffer",                                     // tp_name
	sizeof(MGLRenderbuffer),                                // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLRenderbuffer_tp_dealloc,                 // tp_dealloc
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
	MGLRenderbuffer_tp_methods,                             // tp_methods
	0,                                                      // tp_members
	MGLRenderbuffer_tp_getseters,                           // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLRenderbuffer_tp_init,                      // tp_init
	0,                                                      // tp_alloc
	MGLRenderbuffer_tp_new,                                 // tp_new
};

void MGLRenderbuffer_Invalidate(MGLRenderbuffer * renderbuffer) {
	if (Py_TYPE(renderbuffer) == &MGLInvalidObject_Type) {
		return;
	}

	renderbuffer->context->gl.DeleteRenderbuffers(1, (GLuint *)&renderbuffer->renderbuffer_obj);

	Py_DECREF(renderbuffer->context);
	Py_TYPE(renderbuffer) = &MGLInvalidObject_Type;
	Py_DECREF(renderbuffer);
}
