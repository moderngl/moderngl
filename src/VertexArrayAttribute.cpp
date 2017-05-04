#include "VertexArrayAttribute.hpp"

#include "Error.hpp"

PyObject * MGLVertexArrayAttribute_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLVertexArrayAttribute * self = (MGLVertexArrayAttribute *)type->tp_alloc(type, 0);

	#ifdef MGL_VERBOSE
	printf("MGLVertexArrayAttribute_tp_new %p\n", self);
	#endif

	if (self) {
	}

	return (PyObject *)self;
}

void MGLVertexArrayAttribute_tp_dealloc(MGLVertexArrayAttribute * self) {

	#ifdef MGL_VERBOSE
	printf("MGLVertexArrayAttribute_tp_dealloc %p\n", self);
	#endif

	Py_TYPE(self)->tp_free((PyObject *)self);
}

int MGLVertexArrayAttribute_tp_init(MGLVertexArrayAttribute * self, PyObject * args, PyObject * kwargs) {
	MGLError * error = MGLError_New(TRACE, "Cannot create ModernGL.VertexArrayAttribute manually");
	PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
	return -1;
}

PyObject * MGLVertexArrayAttribute_tp_str(MGLVertexArrayAttribute * self) {
	return PyUnicode_FromFormat("<ModernGL.VertexArrayAttribute: location = %d>", self->location);
}

PyObject * MGLVertexArrayAttribute_bind(MGLVertexArrayAttribute * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"buffer", "offset", "stride", "divisor", 0};

	MGLBuffer * buffer;
	int offset;
	int stride;
	int divisor;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"O!|III",
		(char **)kwlist,
		&MGLBuffer_Type,
		&buffer,
		&offset,
		&stride,
		&divisor
	);

	if (!args_ok) {
		return 0;
	}

	if (self->vertex_array->context != buffer->context) {
		MGLError * error = MGLError_New(TRACE, "buffer belongs to a different context");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	const GLMethods & gl = self->vertex_array->context->gl;
	gl.BindVertexArray(self->vertex_array->vertex_array_obj);
	gl.BindBuffer(GL_ARRAY_BUFFER, buffer->buffer_obj);

	if (self->normalizable) {
		((gl_attribute_normal_ptr_proc)self->gl_attrib_ptr_proc)(self->location, self->row_length, self->scalar_type, false, stride, (void *)(long long)offset);
	} else {
		((gl_attribute_ptr_proc)self->gl_attrib_ptr_proc)(self->location, self->row_length, self->scalar_type, stride, (void *)(long long)offset);
	}
	
	gl.VertexAttribDivisor(self->location, divisor);
	gl.EnableVertexAttribArray(self->location);

	Py_RETURN_NONE;
}

const char * MGLVertexArrayAttribute_bind_doc = R"(
	bind(buffer, offset, stride, divisor)
)";

PyObject * MGLVertexArrayAttribute_enable(MGLVertexArrayAttribute * self) {
	const GLMethods & gl = self->vertex_array->context->gl;
	gl.BindVertexArray(self->vertex_array->vertex_array_obj);
	gl.EnableVertexAttribArray(self->location);
	Py_RETURN_NONE;
}

const char * MGLVertexArrayAttribute_enable_doc = R"(
	enable()
)";

PyObject * MGLVertexArrayAttribute_disable(MGLVertexArrayAttribute * self) {
	const GLMethods & gl = self->vertex_array->context->gl;
	gl.BindVertexArray(self->vertex_array->vertex_array_obj);
	gl.DisableVertexAttribArray(self->location);
	Py_RETURN_NONE;
}

const char * MGLVertexArrayAttribute_disable_doc = R"(
	disable()
)";

PyMethodDef MGLVertexArrayAttribute_tp_methods[] = {
	{"bind", (PyCFunction)MGLVertexArrayAttribute_bind, METH_VARARGS | METH_KEYWORDS, MGLVertexArrayAttribute_bind_doc},
	{"enable", (PyCFunction)MGLVertexArrayAttribute_enable, METH_NOARGS, MGLVertexArrayAttribute_enable_doc},
	{"disable", (PyCFunction)MGLVertexArrayAttribute_disable, METH_NOARGS, MGLVertexArrayAttribute_disable_doc},
	{0},
};

PyObject * MGLVertexArrayAttribute_get_location(MGLVertexArrayAttribute * self, void * closure) {
	return PyLong_FromLong(self->location);
}

char MGLVertexArrayAttribute_location_doc[] = R"(
	location
)";

PyObject * MGLVertexArrayAttribute_get_default(MGLVertexArrayAttribute * self, void * closure) {
	if (!self->gl_attrib_getter_proc) {
		MGLError * error = MGLError_New(TRACE, "gl_attrib_getter_proc is null in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	PyErr_Format(PyExc_NotImplementedError, "Not implemented: %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
	return 0;
}

int MGLVertexArrayAttribute_set_default(MGLVertexArrayAttribute * self, PyObject * value, void * closure) {
	if (!self->gl_attrib_setter_proc) {
		MGLError * error = MGLError_New(TRACE, "gl_attrib_getter_proc is null in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	PyErr_Format(PyExc_NotImplementedError, "Not implemented: %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
	return -1;
}

char MGLVertexArrayAttribute_default_doc[] = R"(
	default
)";

PyObject * MGLVertexArrayAttribute_get_enabled(MGLVertexArrayAttribute * self, void * closure) {
	int enabled = 0;
	const GLMethods & gl = self->vertex_array->context->gl;
	gl.BindVertexArray(self->vertex_array->vertex_array_obj);
	gl.GetVertexAttribiv(self->location, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
	return PyBool_FromLong(enabled);
}

char MGLVertexArrayAttribute_enabled_doc[] = R"(
	enabled
)";

PyObject * MGLVertexArrayAttribute_get_divisor(MGLVertexArrayAttribute * self, void * closure) {
	int divisor = 0;
	const GLMethods & gl = self->vertex_array->context->gl;
	gl.BindVertexArray(self->vertex_array->vertex_array_obj);
	gl.GetVertexAttribiv(self->location, GL_VERTEX_ATTRIB_ARRAY_DIVISOR, &divisor);
	return PyLong_FromLong(divisor);
}

char MGLVertexArrayAttribute_divisor_doc[] = R"(
	divisor
)";

PyObject * MGLVertexArrayAttribute_get_stride(MGLVertexArrayAttribute * self, void * closure) {
	int stride = 0;
	const GLMethods & gl = self->vertex_array->context->gl;
	gl.BindVertexArray(self->vertex_array->vertex_array_obj);
	gl.GetVertexAttribiv(self->location, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);
	return PyBool_FromLong(stride);
}

char MGLVertexArrayAttribute_stride_doc[] = R"(
	stride
)";

PyGetSetDef MGLVertexArrayAttribute_tp_getseters[] = {
	{(char *)"location", (getter)MGLVertexArrayAttribute_get_location, 0, MGLVertexArrayAttribute_location_doc, 0},
	{(char *)"divisor", (getter)MGLVertexArrayAttribute_get_divisor, 0, MGLVertexArrayAttribute_divisor_doc, 0},
	{(char *)"stride", (getter)MGLVertexArrayAttribute_get_stride, 0, MGLVertexArrayAttribute_stride_doc, 0},
	{(char *)"enabled", (getter)MGLVertexArrayAttribute_get_enabled, 0, MGLVertexArrayAttribute_enabled_doc, 0},
	{(char *)"default", (getter)MGLVertexArrayAttribute_get_default, (setter)MGLVertexArrayAttribute_set_default, MGLVertexArrayAttribute_default_doc, 0},
	{0},
};

const char * MGLVertexArrayAttribute_tp_doc = R"(
	VertexArrayAttribute
)";

PyTypeObject MGLVertexArrayAttribute_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"ModernGL.VertexArrayAttribute",                        // tp_name
	sizeof(MGLVertexArrayAttribute),                        // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLVertexArrayAttribute_tp_dealloc,         // tp_dealloc
	0,                                                      // tp_print
	0,                                                      // tp_getattr
	0,                                                      // tp_setattr
	0,                                                      // tp_reserved
	(reprfunc)MGLVertexArrayAttribute_tp_str,               // tp_repr
	0,                                                      // tp_as_number
	0,                                                      // tp_as_sequence
	0,                                                      // tp_as_mapping
	0,                                                      // tp_hash
	0,                                                      // tp_call
	(reprfunc)MGLVertexArrayAttribute_tp_str,               // tp_str
	0,                                                      // tp_getattro
	0,                                                      // tp_setattro
	0,                                                      // tp_as_buffer
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,               // tp_flags
	MGLVertexArrayAttribute_tp_doc,                         // tp_doc
	0,                                                      // tp_traverse
	0,                                                      // tp_clear
	0,                                                      // tp_richcompare
	0,                                                      // tp_weaklistoffset
	0,                                                      // tp_iter
	0,                                                      // tp_iternext
	MGLVertexArrayAttribute_tp_methods,                     // tp_methods
	0,                                                      // tp_members
	MGLVertexArrayAttribute_tp_getseters,                   // tp_getset
	&MGLVertexArrayMember_Type,                             // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLVertexArrayAttribute_tp_init,              // tp_init
	0,                                                      // tp_alloc
	MGLVertexArrayAttribute_tp_new,                         // tp_new
};

MGLVertexArrayAttribute * MGLVertexArrayAttribute_New() {
	MGLVertexArrayAttribute * self = (MGLVertexArrayAttribute *)MGLVertexArrayAttribute_tp_new(&MGLVertexArrayAttribute_Type, 0, 0);
	return self;
}

void MGLVertexArrayAttribute_Complete(MGLVertexArrayAttribute * attribute, const GLMethods & gl) {
	switch (attribute->type) {
		case GL_INT:
			attribute->normalizable = false;
			attribute->row_length = 1;
			attribute->scalar_type = GL_INT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribIPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_INT_VEC2:
			attribute->normalizable = false;
			attribute->row_length = 2;
			attribute->scalar_type = GL_INT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribIPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_INT_VEC3:
			attribute->normalizable = false;
			attribute->row_length = 3;
			attribute->scalar_type = GL_INT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribIPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_INT_VEC4:
			attribute->normalizable = false;
			attribute->row_length = 4;
			attribute->scalar_type = GL_INT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribIPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_UNSIGNED_INT:
			attribute->normalizable = false;
			attribute->row_length = 1;
			attribute->scalar_type = GL_UNSIGNED_INT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribIPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_UNSIGNED_INT_VEC2:
			attribute->normalizable = false;
			attribute->row_length = 2;
			attribute->scalar_type = GL_UNSIGNED_INT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribIPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_UNSIGNED_INT_VEC3:
			attribute->normalizable = false;
			attribute->row_length = 3;
			attribute->scalar_type = GL_UNSIGNED_INT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribIPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_UNSIGNED_INT_VEC4:
			attribute->normalizable = false;
			attribute->row_length = 4;
			attribute->scalar_type = GL_UNSIGNED_INT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribIPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_FLOAT:
			attribute->normalizable = true;
			attribute->row_length = 1;
			attribute->scalar_type = GL_FLOAT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_FLOAT_VEC2:
			attribute->normalizable = true;
			attribute->row_length = 2;
			attribute->scalar_type = GL_FLOAT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_FLOAT_VEC3:
			attribute->normalizable = true;
			attribute->row_length = 3;
			attribute->scalar_type = GL_FLOAT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_FLOAT_VEC4:
			attribute->normalizable = true;
			attribute->row_length = 4;
			attribute->scalar_type = GL_FLOAT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_DOUBLE:
			attribute->normalizable = false;
			attribute->row_length = 1;
			attribute->scalar_type = GL_DOUBLE;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_DOUBLE_VEC2:
			attribute->normalizable = false;
			attribute->row_length = 2;
			attribute->scalar_type = GL_DOUBLE;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_DOUBLE_VEC3:
			attribute->normalizable = false;
			attribute->row_length = 3;
			attribute->scalar_type = GL_DOUBLE;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_DOUBLE_VEC4:
			attribute->normalizable = false;
			attribute->row_length = 4;
			attribute->scalar_type = GL_DOUBLE;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_FLOAT_MAT2:
			attribute->normalizable = true;
			attribute->row_length = 2;
			attribute->scalar_type = GL_FLOAT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_FLOAT_MAT2x3:
			attribute->normalizable = true;
			attribute->row_length = 3;
			attribute->scalar_type = GL_FLOAT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_FLOAT_MAT2x4:
			attribute->normalizable = true;
			attribute->row_length = 4;
			attribute->scalar_type = GL_FLOAT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_FLOAT_MAT3x2:
			attribute->normalizable = true;
			attribute->row_length = 2;
			attribute->scalar_type = GL_FLOAT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_FLOAT_MAT3:
			attribute->normalizable = true;
			attribute->row_length = 3;
			attribute->scalar_type = GL_FLOAT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_FLOAT_MAT3x4:
			attribute->normalizable = true;
			attribute->row_length = 4;
			attribute->scalar_type = GL_FLOAT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_FLOAT_MAT4x2:
			attribute->normalizable = true;
			attribute->row_length = 2;
			attribute->scalar_type = GL_FLOAT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_FLOAT_MAT4x3:
			attribute->normalizable = true;
			attribute->row_length = 3;
			attribute->scalar_type = GL_FLOAT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_FLOAT_MAT4:
			attribute->normalizable = true;
			attribute->row_length = 4;
			attribute->scalar_type = GL_FLOAT;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_DOUBLE_MAT2:
			attribute->normalizable = false;
			attribute->row_length = 2;
			attribute->scalar_type = GL_DOUBLE;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_DOUBLE_MAT2x3:
			attribute->normalizable = false;
			attribute->row_length = 3;
			attribute->scalar_type = GL_DOUBLE;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_DOUBLE_MAT2x4:
			attribute->normalizable = false;
			attribute->row_length = 4;
			attribute->scalar_type = GL_DOUBLE;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_DOUBLE_MAT3x2:
			attribute->normalizable = false;
			attribute->row_length = 2;
			attribute->scalar_type = GL_DOUBLE;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_DOUBLE_MAT3:
			attribute->normalizable = false;
			attribute->row_length = 3;
			attribute->scalar_type = GL_DOUBLE;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_DOUBLE_MAT3x4:
			attribute->normalizable = false;
			attribute->row_length = 4;
			attribute->scalar_type = GL_DOUBLE;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_DOUBLE_MAT4x2:
			attribute->normalizable = false;
			attribute->row_length = 2;
			attribute->scalar_type = GL_DOUBLE;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_DOUBLE_MAT4x3:
			attribute->normalizable = false;
			attribute->row_length = 3;
			attribute->scalar_type = GL_DOUBLE;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		case GL_DOUBLE_MAT4:
			attribute->normalizable = false;
			attribute->row_length = 4;
			attribute->scalar_type = GL_DOUBLE;

			attribute->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;

		default:
			attribute->normalizable = false;
			attribute->row_length = 1;
			attribute->scalar_type = 0;

			attribute->gl_attrib_ptr_proc = 0;
			attribute->gl_attrib_getter_proc = 0;
			attribute->gl_attrib_setter_proc = 0;
			break;
	}
}
