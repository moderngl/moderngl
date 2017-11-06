#include "Error.hpp"

#include <cstdarg>

PyObject * MGLError_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLError * self = (MGLError *)type->tp_alloc(type, 0);

	if (self) {
		self->dict = 0;
		self->args = 0;
		self->traceback = 0;
		self->context = 0;
		self->cause = 0;
		self->suppress_context = 0;

		self->filename = 0;
		self->function = 0;
	}

	return (PyObject *)self;
}

void MGLError_tp_dealloc(MGLError * self) {
	PyTypeObject * super = Py_TYPE(self)->tp_base;
	return super->tp_dealloc((PyObject *)self);
}

int MGLError_tp_init(MGLError * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.Error manually");
	return -1;
}

PyMethodDef MGLError_tp_methods[] = {
	{0},
};

PyObject * MGLError_get_filename(MGLError * self) {
	if (self->filename) {
		return PyUnicode_FromFormat("%s", self->filename);
	} else {
		Py_RETURN_NONE;
	}
}

PyObject * MGLError_get_function(MGLError * self) {
	if (self->function) {
		return PyUnicode_FromFormat("%s", self->function);
	} else {
		Py_RETURN_NONE;
	}
}

PyObject * MGLError_get_line(MGLError * self) {
	return PyLong_FromLong(self->line);
}

PyGetSetDef MGLError_tp_getseters[] = {
	{(char *)"filename", (getter)MGLError_get_filename, 0, 0, 0},
	{(char *)"function", (getter)MGLError_get_function, 0, 0, 0},
	{(char *)"line", (getter)MGLError_get_line, 0, 0, 0},
	{0},
};

PyTypeObject MGLError_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.Error",                                            // tp_name
	sizeof(MGLError),                                       // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLError_tp_dealloc,                        // tp_dealloc
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
	MGLError_tp_methods,                                    // tp_methods
	0,                                                      // tp_members
	MGLError_tp_getseters,                                  // tp_getset
	(PyTypeObject *)PyExc_Exception,                        // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLError_tp_init,                             // tp_init
	0,                                                      // tp_alloc
	MGLError_tp_new,                                        // tp_new
};

void MGLError_SetTrace(const char * filename, const char * function, int line, const char * format, ...) {
	MGLError * error = (MGLError *)MGLError_tp_new(&MGLError_Type, 0, 0);

	va_list va_args;
	va_start(va_args, format);
	error->args = PyTuple_New(1);
	PyObject * message = PyUnicode_FromFormatV(format, va_args);
	PyTuple_SET_ITEM(error->args, 0, message);
	va_end(va_args);

	PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
}

void MGLError_SetTrace(const char * filename, const char * function, int line, PyObject * message) {
	MGLError * error = (MGLError *)MGLError_tp_new(&MGLError_Type, 0, 0);

	error->filename = filename;
	error->function = function;
	error->line = line;

	error->args = PyTuple_New(1);
	PyTuple_SET_ITEM(error->args, 0, message);

	PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
}
