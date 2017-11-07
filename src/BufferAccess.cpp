#include "Types.hpp"

PyObject * MGLBufferAccess_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLBufferAccess * self = (MGLBufferAccess *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLBufferAccess_tp_dealloc(MGLBufferAccess * self) {
	MGLBufferAccess_Type.tp_free((PyObject *)self);
}

int MGLBufferAccess_tp_init(MGLBufferAccess * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.BufferAccess manually");
	return -1;
}

PyObject * MGLBufferAccess_open(MGLBufferAccess * self) {
	if (self->ptr) {
		MGLError_Set("already open");
		return 0;
	}

	self->gl->BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);
	self->ptr = (char *)self->gl->MapBufferRange(GL_ARRAY_BUFFER, self->offset, self->size, self->access);

	if (!self->ptr) {
		MGLError_Set("cannot map the buffer");
		return 0;
	}

	Py_RETURN_NONE;
}

PyObject * MGLBufferAccess_close(MGLBufferAccess * self) {
	if (self->ptr) {
		self->gl->UnmapBuffer(GL_ARRAY_BUFFER);
		self->ptr = 0;
	}
	Py_RETURN_NONE;
}

PyObject * MGLBufferAccess_read(MGLBufferAccess * self, PyObject * args) {
	Py_ssize_t size;
	Py_ssize_t offset;

	int args_ok = PyArg_ParseTuple(
		args,
		"nn",
		&size,
		&offset
	);

	if (!args_ok) {
		return 0;
	}

	if (size < 0) {
		size = self->size - offset;
	}

	if (offset < 0 || offset + size > self->size) {
		MGLError_Set("out of range offset = %d or size = %d", offset, size);
		return 0;
	}

	if (!self->ptr) {
		MGLError_Set("the access object is not open");
		return 0;
	}

	return PyBytes_FromStringAndSize(self->ptr + offset, size);
}

PyObject * MGLBufferAccess_read_into(MGLBufferAccess * self, PyObject * args) {
	PyObject * data;
	Py_ssize_t size;
	Py_ssize_t offset;
	Py_ssize_t write_offset;

	int args_ok = PyArg_ParseTuple(
		args,
		"Onnn",
		&data,
		&size,
		&offset,
		&write_offset
	);

	if (!args_ok) {
		return 0;
	}

	if (size < 0) {
		size = self->size - offset;
	}

	if (offset < 0 || offset + size > self->size) {
		MGLError_Set("out of range offset = %d or size = %d", offset, size);
		return 0;
	}

	if (!self->ptr) {
		MGLError_Set("the access object is not open");
		return 0;
	}

	Py_buffer buffer_view;

	int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_WRITABLE);
	if (get_buffer < 0) {
		MGLError_Set("the buffer (%s) does not support buffer interface", Py_TYPE(data)->tp_name);
		return 0;
	}

	if (buffer_view.len < write_offset + size) {
		MGLError_Set("the buffer is too small");
		PyBuffer_Release(&buffer_view);
		return 0;
	}

	char * ptr = (char *)buffer_view.buf + write_offset;
	memcpy(ptr, self->ptr + offset, size);

	PyBuffer_Release(&buffer_view);
	Py_RETURN_NONE;
}

PyObject * MGLBufferAccess_write(MGLBufferAccess * self, PyObject * args) {
	PyObject * data;
	Py_ssize_t offset;

	int args_ok = PyArg_ParseTuple(
		args,
		"On",
		&data,
		&offset
	);

	if (!args_ok) {
		return 0;
	}

	Py_buffer buffer_view;

	int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
	if (get_buffer < 0) {
		MGLError_Set("data (%s) does not support buffer interface", Py_TYPE(data)->tp_name);
		return 0;
	}

	if (offset < 0 || buffer_view.len + offset > self->size) {
		MGLError_Set("out of range offset = %d or size = %d", offset, buffer_view.len);
		PyBuffer_Release(&buffer_view);
		return 0;
	}

	if (!self->ptr) {
		MGLError_Set("access objet is not open");
		PyBuffer_Release(&buffer_view);
		return 0;
	}

	memcpy(self->ptr + offset, buffer_view.buf, buffer_view.len);
	PyBuffer_Release(&buffer_view);
	Py_RETURN_NONE;
}

PyMethodDef MGLBufferAccess_tp_methods[] = {
	{"open", (PyCFunction)MGLBufferAccess_open, METH_NOARGS, 0},
	{"close", (PyCFunction)MGLBufferAccess_close, METH_VARARGS, 0},
	{"read", (PyCFunction)MGLBufferAccess_read, METH_VARARGS, 0},
	{"read_into", (PyCFunction)MGLBufferAccess_read_into, METH_VARARGS, 0},
	{"write", (PyCFunction)MGLBufferAccess_write, METH_VARARGS, 0},
	{0},
};

PyObject * MGLBufferAccess_get_offset(MGLBufferAccess * self) {
	return PyLong_FromSsize_t(self->offset);
}

PyObject * MGLBufferAccess_get_size(MGLBufferAccess * self) {
	return PyLong_FromSsize_t(self->size);
}

PyObject * MGLBufferAccess_get_readonly(MGLBufferAccess * self) {
	return PyBool_FromLong(!(self->access & GL_MAP_WRITE_BIT));
}

PyGetSetDef MGLBufferAccess_tp_getseters[] = {
	{(char *)"offset", (getter)MGLBufferAccess_get_offset, 0, 0, 0},
	{(char *)"size", (getter)MGLBufferAccess_get_size, 0, 0, 0},
	{(char *)"readonly", (getter)MGLBufferAccess_get_readonly, 0, 0, 0},
	{0},
};

PyTypeObject MGLBufferAccess_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.BufferAccess",                                     // tp_name
	sizeof(MGLBufferAccess),                                // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLBufferAccess_tp_dealloc,                 // tp_dealloc
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
	MGLBufferAccess_tp_methods,                             // tp_methods
	0,                                                      // tp_members
	MGLBufferAccess_tp_getseters,                           // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLBufferAccess_tp_init,                      // tp_init
	0,                                                      // tp_alloc
	MGLBufferAccess_tp_new,                                 // tp_new
};
