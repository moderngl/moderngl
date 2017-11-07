#include "Types.hpp"

PyObject * MGLVertexArrayAttribute_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLVertexArrayAttribute * self = (MGLVertexArrayAttribute *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLVertexArrayAttribute_tp_dealloc(MGLVertexArrayAttribute * self) {
	Py_TYPE(self)->tp_free((PyObject *)self);
}

int MGLVertexArrayAttribute_tp_init(MGLVertexArrayAttribute * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.VertexArrayAttribute manually");
	return -1;
}

PyObject * MGLVertexArrayAttribute_bind(MGLVertexArrayAttribute * self, PyObject * args) {
	MGLBuffer * buffer;
	Py_ssize_t offset;
	int stride;
	int divisor;

	int args_ok = PyArg_ParseTuple(
		args,
		"O!nII",
		&MGLBuffer_Type,
		&buffer,
		&offset,
		&stride,
		&divisor
	);

	if (!args_ok) {
		return 0;
	}

	const GLMethods & gl = self->attribute->context->gl;

	gl.BindVertexArray(self->vertex_array_obj);
	gl.BindBuffer(GL_ARRAY_BUFFER, buffer->buffer_obj);

	if (self->attribute->normalizable) {
		((gl_attribute_normal_ptr_proc)self->attribute->gl_attrib_ptr_proc)(
			self->location,
			self->attribute->row_length,
			self->attribute->scalar_type,
			false,
			stride,
			(void *)offset
		);
	} else {
		((gl_attribute_ptr_proc)self->attribute->gl_attrib_ptr_proc)(
			self->location,
			self->attribute->row_length,
			self->attribute->scalar_type,
			stride,
			(void *)offset
		);
	}

	gl.VertexAttribDivisor(self->location, divisor);
	gl.EnableVertexAttribArray(self->location);

	Py_RETURN_NONE;
}

PyObject * MGLVertexArrayAttribute_enable(MGLVertexArrayAttribute * self) {
	const GLMethods & gl = self->attribute->context->gl;

	gl.BindVertexArray(self->vertex_array_obj);
	gl.EnableVertexAttribArray(self->location);

	Py_RETURN_NONE;
}

PyObject * MGLVertexArrayAttribute_disable(MGLVertexArrayAttribute * self) {
	const GLMethods & gl = self->attribute->context->gl;

	gl.BindVertexArray(self->vertex_array_obj);
	gl.DisableVertexAttribArray(self->location);

	Py_RETURN_NONE;
}

PyMethodDef MGLVertexArrayAttribute_tp_methods[] = {
	{"bind", (PyCFunction)MGLVertexArrayAttribute_bind, METH_VARARGS, 0},
	{"enable", (PyCFunction)MGLVertexArrayAttribute_enable, METH_NOARGS, 0},
	{"disable", (PyCFunction)MGLVertexArrayAttribute_disable, METH_NOARGS, 0},
	{0},
};

PyObject * MGLVertexArrayAttribute_get_location(MGLVertexArrayAttribute * self, void * closure) {
	return PyLong_FromLong(self->location);
}

PyObject * MGLVertexArrayAttribute_get_divisor(MGLVertexArrayAttribute * self, void * closure) {
	int divisor = 0;

	const GLMethods & gl = self->attribute->context->gl;

	gl.BindVertexArray(self->vertex_array_obj);
	gl.GetVertexAttribiv(self->location, GL_VERTEX_ATTRIB_ARRAY_DIVISOR, &divisor);

	return PyLong_FromLong(divisor);
}

PyObject * MGLVertexArrayAttribute_get_stride(MGLVertexArrayAttribute * self, void * closure) {
	int stride = 0;

	const GLMethods & gl = self->attribute->context->gl;

	gl.BindVertexArray(self->vertex_array_obj);
	gl.GetVertexAttribiv(self->location, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);

	return PyLong_FromLong(stride);
}

PyObject * MGLVertexArrayAttribute_get_enabled(MGLVertexArrayAttribute * self, void * closure) {
	int enabled = 0;

	const GLMethods & gl = self->attribute->context->gl;

	gl.BindVertexArray(self->vertex_array_obj);
	gl.GetVertexAttribiv(self->location, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);

	return PyBool_FromLong(enabled);
}

int MGLVertexArrayAttribute_set_enabled(MGLVertexArrayAttribute * self, PyObject * value, void * closure) {
	const GLMethods & gl = self->attribute->context->gl;

	if (value == Py_True) {
		gl.BindVertexArray(self->vertex_array_obj);
		gl.EnableVertexAttribArray(self->location);
	} else if (value == Py_False) {
		gl.BindVertexArray(self->vertex_array_obj);
		gl.DisableVertexAttribArray(self->location);
	} else {
		MGLError_Set("invalid value for enabled");
		return -1;
	}

	return 0;
}

PyGetSetDef MGLVertexArrayAttribute_tp_getseters[] = {
	{(char *)"location", (getter)MGLVertexArrayAttribute_get_location, 0, 0, 0},
	{(char *)"divisor", (getter)MGLVertexArrayAttribute_get_divisor, 0, 0, 0},
	{(char *)"stride", (getter)MGLVertexArrayAttribute_get_stride, 0, 0, 0},
	{(char *)"enabled", (getter)MGLVertexArrayAttribute_get_enabled, 0, 0, 0},
	{0},
};

PyTypeObject MGLVertexArrayAttribute_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.VertexArrayAttribute",                             // tp_name
	sizeof(MGLVertexArrayAttribute),                        // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLVertexArrayAttribute_tp_dealloc,         // tp_dealloc
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
	MGLVertexArrayAttribute_tp_methods,                     // tp_methods
	0,                                                      // tp_members
	MGLVertexArrayAttribute_tp_getseters,                   // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLVertexArrayAttribute_tp_init,              // tp_init
	0,                                                      // tp_alloc
	MGLVertexArrayAttribute_tp_new,                         // tp_new
};

void MGLVertexArrayAttribute_Complete(MGLVertexArrayAttribute * attribute, const GLMethods & gl) {
}
