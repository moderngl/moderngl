#include "Types.hpp"

#include "InlineMethods.hpp"

PyObject * MGLContext_texture_cube(MGLContext * self, PyObject * args) {
	int width;
	int height;

	int components;

	PyObject * data;

	int alignment;

	const char * dtype;
	Py_ssize_t dtype_size;

	int args_ok = PyArg_ParseTuple(
		args,
		"(II)IOIs#",
		&width,
		&height,
		&components,
		&data,
		&alignment,
		&dtype,
		&dtype_size
	);

	if (!args_ok) {
		return 0;
	}

	if (components < 1 || components > 4) {
		MGLError_Set("the components must be 1, 2, 3 or 4");
		return 0;
	}

	if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
		MGLError_Set("the alignment must be 1, 2, 4 or 8");
		return 0;
	}

	if (dtype_size != 2) {
		MGLError_Set("invalid dtype");
		return 0;
	}

	MGLDataType * data_type = from_dtype(dtype);

	if (!data_type) {
		MGLError_Set("invalid dtype");
		return 0;
	}

	int expected_size = width * components * data_type->size;
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * height * 6;

	Py_buffer buffer_view;

	if (data != Py_None) {
		int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
		if (get_buffer < 0) {
			MGLError_Set("data (%s) does not support buffer interface", Py_TYPE(data)->tp_name);
			return 0;
		}
	} else {
		buffer_view.len = expected_size;
		buffer_view.buf = 0;
	}

	if (buffer_view.len != expected_size) {
		MGLError_Set("data size mismatch %d != %d", buffer_view.len, expected_size);
		if (data != Py_None) {
			PyBuffer_Release(&buffer_view);
		}
		return 0;
	}

	int pixel_type = data_type->gl_type;
	int base_format = data_type->base_format[components];
	int internal_format = data_type->internal_format[components];

	const GLMethods & gl = self->gl;

	MGLTextureCube * texture = (MGLTextureCube *)MGLTextureCube_Type.tp_alloc(&MGLTextureCube_Type, 0);

	texture->texture_obj = 0;
	gl.GenTextures(1, (GLuint *)&texture->texture_obj);

	if (!texture->texture_obj) {
		MGLError_Set("cannot create texture");
		Py_DECREF(texture);
		return 0;
	}

	gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_CUBE_MAP, texture->texture_obj);

	if (data == Py_None) {
		expected_size = 0;
	}

	const char * ptr[6] = {
		(const char *)buffer_view.buf + expected_size * 0 / 6,
		(const char *)buffer_view.buf + expected_size * 1 / 6,
		(const char *)buffer_view.buf + expected_size * 2 / 6,
		(const char *)buffer_view.buf + expected_size * 3 / 6,
		(const char *)buffer_view.buf + expected_size * 4 / 6,
		(const char *)buffer_view.buf + expected_size * 5 / 6,
	};

	gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
	gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
	gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internal_format, width, height, 0, base_format, pixel_type, ptr[0]);
	gl.TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internal_format, width, height, 0, base_format, pixel_type, ptr[1]);
	gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internal_format, width, height, 0, base_format, pixel_type, ptr[2]);
	gl.TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internal_format, width, height, 0, base_format, pixel_type, ptr[3]);
	gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internal_format, width, height, 0, base_format, pixel_type, ptr[4]);
	gl.TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internal_format, width, height, 0, base_format, pixel_type, ptr[5]);
	gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data != Py_None) {
		PyBuffer_Release(&buffer_view);
	}

	texture->width = width;
	texture->height = height;
	texture->components = components;
	texture->data_type = data_type;

	texture->min_filter = GL_LINEAR;
	texture->mag_filter = GL_LINEAR;
	texture->max_level = 0;
	texture->anisotropy = 1.0;

	texture->reference = nullptr;

	Py_INCREF(self);
	texture->context = self;

	Py_INCREF(texture);

	PyObject * result = PyTuple_New(2);
	PyTuple_SET_ITEM(result, 0, (PyObject *)texture);
	PyTuple_SET_ITEM(result, 1, PyLong_FromLong(texture->texture_obj));
	return result;
}

PyObject * MGLTextureCube_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLTextureCube * self = (MGLTextureCube *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLTextureCube_tp_dealloc(MGLTextureCube * self) {
	MGLTextureCube_Type.tp_free((PyObject *)self);
}

PyObject * MGLTextureCube_read(MGLTextureCube * self, PyObject * args) {
	int face;
	int alignment;

	int args_ok = PyArg_ParseTuple(
		args,
		"iI",
		&face,
		&alignment
	);

	if (!args_ok) {
		return 0;
	}

	if (face < 0 || face > 5) {
		MGLError_Set("the face must be 0, 1, 2, 3, 4 or 5");
		return 0;
	}

	if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
		MGLError_Set("the alignment must be 1, 2, 4 or 8");
		return 0;
	}

	int expected_size = self->width * self->components * self->data_type->size;
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * self->height;

	PyObject * result = PyBytes_FromStringAndSize(0, expected_size);
	char * data = PyBytes_AS_STRING(result);

	int pixel_type = self->data_type->gl_type;
	int format = self->data_type->base_format[self->components];

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);

	gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
	gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
	gl.GetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, format, pixel_type, data);

	return result;
}

PyObject * MGLTextureCube_read_into(MGLTextureCube * self, PyObject * args) {
	PyObject * data;
	int face;
	int alignment;
	Py_ssize_t write_offset;

	int args_ok = PyArg_ParseTuple(
		args,
		"OiIn",
		&data,
		&face,
		&alignment,
		&write_offset
	);

	if (!args_ok) {
		return 0;
	}

	if (face < 0 || face > 5) {
		MGLError_Set("the face must be 0, 1, 2, 3, 4 or 5");
		return 0;
	}

	if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
		MGLError_Set("the alignment must be 1, 2, 4 or 8");
		return 0;
	}

	int expected_size = self->width * self->components * self->data_type->size;
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * self->height;

	int pixel_type = self->data_type->gl_type;
	int format = self->data_type->base_format[self->components];

	if (Py_TYPE(data) == &MGLBuffer_Type) {

		MGLBuffer * buffer = (MGLBuffer *)data;

		const GLMethods & gl = self->context->gl;

		gl.BindBuffer(GL_PIXEL_PACK_BUFFER, buffer->buffer_obj);
		gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
		gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.GetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, format, pixel_type, (char *)write_offset);
		gl.BindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	} else {

		Py_buffer buffer_view;

		int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_WRITABLE);
		if (get_buffer < 0) {
			MGLError_Set("the buffer (%s) does not support buffer interface", Py_TYPE(data)->tp_name);
			return 0;
		}

		if (buffer_view.len < write_offset + expected_size) {
			MGLError_Set("the buffer is too small");
			PyBuffer_Release(&buffer_view);
			return 0;
		}

		char * ptr = (char *)buffer_view.buf + write_offset;

		const GLMethods & gl = self->context->gl;
		gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
		gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.GetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, format, pixel_type, ptr);

		PyBuffer_Release(&buffer_view);

	}

	Py_RETURN_NONE;
}

PyObject * MGLTextureCube_write(MGLTextureCube * self, PyObject * args) {
	int face;
	PyObject * data;
	PyObject * viewport;
	int alignment;

	int args_ok = PyArg_ParseTuple(
		args,
		"iOOI",
		&face,
		&data,
		&viewport,
		&alignment
	);

	if (!args_ok) {
		return 0;
	}

	if (face < 0 || face > 5) {
		MGLError_Set("the face must be 0, 1, 2, 3, 4 or 5");
		return 0;
	}

	if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
		MGLError_Set("the alignment must be 1, 2, 4 or 8");
		return 0;
	}

	int x = 0;
	int y = 0;
	int width = self->width;
	int height = self->height;

	Py_buffer buffer_view;

	if (viewport != Py_None) {
		if (Py_TYPE(viewport) != &PyTuple_Type) {
			MGLError_Set("the viewport must be a tuple not %s", Py_TYPE(viewport)->tp_name);
			return 0;
		}

		if (PyTuple_GET_SIZE(viewport) == 4) {

			x = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 0));
			y = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 1));
			width = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 2));
			height = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 3));

		} else if (PyTuple_GET_SIZE(viewport) == 2) {

			width = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 0));
			height = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 1));

		} else {

			MGLError_Set("the viewport size %d is invalid", PyTuple_GET_SIZE(viewport));
			return 0;

		}

		if (PyErr_Occurred()) {
			MGLError_Set("wrong values in the viewport");
			return 0;
		}

	}

	int expected_size = width * self->components * self->data_type->size;
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * height;

	// GL_TEXTURE_CUBE_MAP_POSITIVE_X = GL_TEXTURE_CUBE_MAP_POSITIVE_X + 0
	// GL_TEXTURE_CUBE_MAP_NEGATIVE_X = GL_TEXTURE_CUBE_MAP_POSITIVE_X + 1
	// GL_TEXTURE_CUBE_MAP_POSITIVE_Y = GL_TEXTURE_CUBE_MAP_POSITIVE_X + 2
	// GL_TEXTURE_CUBE_MAP_NEGATIVE_Y = GL_TEXTURE_CUBE_MAP_POSITIVE_X + 3
	// GL_TEXTURE_CUBE_MAP_POSITIVE_Z = GL_TEXTURE_CUBE_MAP_POSITIVE_X + 4
	// GL_TEXTURE_CUBE_MAP_NEGATIVE_Z = GL_TEXTURE_CUBE_MAP_POSITIVE_X + 5

	int pixel_type = self->data_type->gl_type;
	int format = self->data_type->base_format[self->components];

	if (Py_TYPE(data) == &MGLBuffer_Type) {

		MGLBuffer * buffer = (MGLBuffer *)data;

		const GLMethods & gl = self->context->gl;

		gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer->buffer_obj);
		gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
		gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.TexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, x, y, width, height, format, pixel_type, 0);
		gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	} else {

		int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
		if (get_buffer < 0) {
			MGLError_Set("data (%s) does not support buffer interface", Py_TYPE(data)->tp_name);
			return 0;
		}

		if (buffer_view.len != expected_size) {
			MGLError_Set("data size mismatch %d != %d", buffer_view.len, expected_size);
			PyBuffer_Release(&buffer_view);
			return 0;
		}

		const GLMethods & gl = self->context->gl;

		gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
		gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);

		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.TexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, x, y, width, height, format, pixel_type, buffer_view.buf);

		PyBuffer_Release(&buffer_view);
	}

	Py_RETURN_NONE;
}

PyObject * MGLTextureCube_use(MGLTextureCube * self, PyObject * args) {
	int index;

	int args_ok = PyArg_ParseTuple(
		args,
		"I",
		&index
	);

	if (!args_ok) {
		return 0;
	}

	const GLMethods & gl = self->context->gl;
	gl.ActiveTexture(GL_TEXTURE0 + index);
	gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);

	Py_RETURN_NONE;
}

PyObject * MGLTextureCube_transfer(MGLTextureCube * self, PyObject * args) {
	MGLContext* context;
	bool args_ok = PyArg_ParseTuple(
			args,
			"O!",
			&MGLContext_Type,
			&context);
	if (!args_ok) return 0;
	MGLTextureCube* new_texture = new MGLTextureCube(*self);
	Py_INCREF(context);
	Py_INCREF(new_texture);
	Py_INCREF(self);
	new_texture->context = context;
	new_texture->reference = self;
	PyObject* result = PyTuple_New(2);
	PyTuple_SET_ITEM(result, 0, (PyObject*)new_texture);
	PyTuple_SET_ITEM(result, 1, PyLong_FromLong(new_texture->texture_obj));
	return result;
}

PyObject * MGLTextureCube_release(MGLTextureCube * self) {
	MGLTextureCube_Invalidate(self);
	Py_RETURN_NONE;
}

PyMethodDef MGLTextureCube_tp_methods[] = {
	{"write", (PyCFunction)MGLTextureCube_write, METH_VARARGS, 0},
	{"use", (PyCFunction)MGLTextureCube_use, METH_VARARGS, 0},
//	{"build_mipmaps", (PyCFunction)MGLTextureCube_build_mipmaps, METH_VARARGS, 0},
	{"read", (PyCFunction)MGLTextureCube_read, METH_VARARGS, 0},
	{"read_into", (PyCFunction)MGLTextureCube_read_into, METH_VARARGS, 0},
	{"transfer", (PyCFunction)MGLTextureCube_transfer, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLTextureCube_release, METH_NOARGS, 0},
	{0},
};

PyObject * MGLTextureCube_get_filter(MGLTextureCube * self) {
	PyObject * res = PyTuple_New(2);
	PyTuple_SET_ITEM(res, 0, PyLong_FromLong(self->min_filter));
	PyTuple_SET_ITEM(res, 1, PyLong_FromLong(self->mag_filter));
	return res;
}

int MGLTextureCube_set_filter(MGLTextureCube * self, PyObject * value) {
	if (PyTuple_GET_SIZE(value) != 2) {
		MGLError_Set("invalid filter");
		return -1;
	}

	self->min_filter = PyLong_AsLong(PyTuple_GET_ITEM(value, 0));
	self->mag_filter = PyLong_AsLong(PyTuple_GET_ITEM(value, 1));

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);
	gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, self->min_filter);
	gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, self->mag_filter);

	return 0;
}

PyObject * MGLTextureCube_get_swizzle(MGLTextureCube * self, void * closure) {

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);

	int swizzle_r = 0;
	int swizzle_g = 0;
	int swizzle_b = 0;
	int swizzle_a = 0;

	gl.GetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_R, &swizzle_r);
	gl.GetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_G, &swizzle_g);
	gl.GetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_B, &swizzle_b);
	gl.GetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_A, &swizzle_a);

	char swizzle[5] = {
		char_from_swizzle(swizzle_r),
		char_from_swizzle(swizzle_g),
		char_from_swizzle(swizzle_b),
		char_from_swizzle(swizzle_a),
		0,
	};

	return PyUnicode_FromStringAndSize(swizzle, 4);
}

int MGLTextureCube_set_swizzle(MGLTextureCube * self, PyObject * value, void * closure) {
	const char * swizzle = PyUnicode_AsUTF8(value);

	if (!swizzle[0]) {
		MGLError_Set("the swizzle is empty");
		return -1;
	}

	int tex_swizzle[4] = {-1, -1, -1, -1};

	for (int i = 0; swizzle[i]; ++i) {
		if (i > 3) {
			MGLError_Set("the swizzle is too long");
			return -1;
		}

		tex_swizzle[i] = swizzle_from_char(swizzle[i]);

		if (tex_swizzle[i] == -1) {
			MGLError_Set("'%c' is not a valid swizzle parameter", swizzle[i]);
			return -1;
		}
	}


	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);

	gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_R, tex_swizzle[0]);
	if (tex_swizzle[1] != -1) {
		gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_G, tex_swizzle[1]);
		if (tex_swizzle[2] != -1) {
			gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_B, tex_swizzle[2]);
			if (tex_swizzle[3] != -1) {
				gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_A, tex_swizzle[3]);
			}
		}
	}

	return 0;
}

PyObject * MGLTextureCube_get_anisotropy(MGLTextureCube * self) {
	return PyFloat_FromDouble(self->anisotropy);
}

int MGLTextureCube_set_anisotropy(MGLTextureCube * self, PyObject * value) {
	self->anisotropy = (float)min(max(PyFloat_AsDouble(value), 1.0), self->context->max_anisotropy);

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);
	gl.TexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, self->anisotropy);

	return 0;
}

PyGetSetDef MGLTextureCube_tp_getseters[] = {
	{(char *)"filter", (getter)MGLTextureCube_get_filter, (setter)MGLTextureCube_set_filter, 0, 0},
	{(char *)"swizzle", (getter)MGLTextureCube_get_swizzle, (setter)MGLTextureCube_set_swizzle, 0, 0},
	{(char *)"anisotropy", (getter)MGLTextureCube_get_anisotropy, (setter)MGLTextureCube_set_anisotropy, 0, 0},
	{0},
};

PyTypeObject MGLTextureCube_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.TextureCube",                                      // tp_name
	sizeof(MGLTextureCube),                                 // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLTextureCube_tp_dealloc,                  // tp_dealloc
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
	MGLTextureCube_tp_methods,                              // tp_methods
	0,                                                      // tp_members
	MGLTextureCube_tp_getseters,                            // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	0,                                                      // tp_init
	0,                                                      // tp_alloc
	MGLTextureCube_tp_new,                                  // tp_new
};

void MGLTextureCube_Invalidate(MGLTextureCube * texture) {
	if (Py_TYPE(texture) == &MGLInvalidObject_Type) {
		return;
	}

	// TODO: decref

	if (texture->reference) {
		Py_DECREF(texture->reference);
	} else {
		const GLMethods & gl = texture->context->gl;
		gl.DeleteTextures(1, (GLuint *)&texture->texture_obj);
	}

	Py_TYPE(texture) = &MGLInvalidObject_Type;
	Py_DECREF(texture);
}
