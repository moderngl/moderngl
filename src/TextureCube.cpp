#include "Types.hpp"

#include "InlineMethods.hpp"

PyObject * MGLTextureCube_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLTextureCube * self = (MGLTextureCube *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLTextureCube_tp_dealloc(MGLTextureCube * self) {
	MGLTextureCube_Type.tp_free((PyObject *)self);
}

int MGLTextureCube_tp_init(MGLTextureCube * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.TextureCube manually");
	return -1;
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

	int expected_size = self->width * self->components * (self->floats ? 4 : 1);
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * self->height;

	PyObject * result = PyBytes_FromStringAndSize(0, expected_size);
	char * data = PyBytes_AS_STRING(result);

	const int formats[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};
	const int faces[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	};

	int pixel_type = self->floats ? GL_FLOAT : GL_UNSIGNED_BYTE;
	int format = formats[self->components];

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);

	gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
	gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
	gl.GetTexImage(faces[face], 0, format, pixel_type, data);

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

	int expected_size = self->width * self->components * (self->floats ? 4 : 1);
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * self->height;

	const int formats[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};
	const int faces[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	};

	int pixel_type = self->floats ? GL_FLOAT : GL_UNSIGNED_BYTE;
	int format = formats[self->components];

	if (Py_TYPE(data) == &MGLBuffer_Type) {

		MGLBuffer * buffer = (MGLBuffer *)data;

		const GLMethods & gl = self->context->gl;

		gl.BindBuffer(GL_PIXEL_PACK_BUFFER, buffer->buffer_obj);
		gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
		gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.GetTexImage(faces[face], 0, format, pixel_type, (char *)write_offset);
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
		gl.GetTexImage(faces[face], 0, format, pixel_type, ptr);

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

	int expected_size = width * self->components * (self->floats ? 4 : 1);
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * height;

	const int formats[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};
	const int faces[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	};

	int pixel_type = self->floats ? GL_FLOAT : GL_UNSIGNED_BYTE;
	int format = formats[self->components];

	if (Py_TYPE(data) == &MGLBuffer_Type) {

		MGLBuffer * buffer = (MGLBuffer *)data;

		const GLMethods & gl = self->context->gl;

		gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer->buffer_obj);
		gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
		gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.TexSubImage2D(faces[face], 0, x, y, width, height, format, pixel_type, 0);
		gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	} else {

		int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
		if (get_buffer < 0) {
			MGLError_Set("data (%s) does not support buffer interface", Py_TYPE(data)->tp_name);
			return 0;
		}

		if (buffer_view.len != expected_size) {
			MGLError_Set("data size mismatch %d != %d", buffer_view.len, expected_size);
			if (data != Py_None) {
				PyBuffer_Release(&buffer_view);
			}
			return 0;
		}

		const GLMethods & gl = self->context->gl;

		gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
		gl.BindTexture(GL_TEXTURE_CUBE_MAP, self->texture_obj);

		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.TexSubImage2D(faces[face], 0, x, y, width, height, format, pixel_type, buffer_view.buf);

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

PyObject * MGLTextureCube_release(MGLTextureCube * self) {
	MGLTextureCube_Invalidate(self);
	Py_RETURN_NONE;
}

PyMethodDef MGLTextureCube_tp_methods[] = {
	{"read", (PyCFunction)MGLTextureCube_read, METH_VARARGS, 0},
	{"read_into", (PyCFunction)MGLTextureCube_read_into, METH_VARARGS, 0},
	{"write", (PyCFunction)MGLTextureCube_write, METH_VARARGS, 0},
	{"use", (PyCFunction)MGLTextureCube_use, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLTextureCube_release, METH_NOARGS, 0},
	{0},
};

MGLContext * MGLTextureCube_get_context(MGLTextureCube * self, void * closure) {
	Py_INCREF(self->context);
	return self->context;
}

PyObject * MGLTextureCube_get_glo(MGLTextureCube * self, void * closure) {
	return PyLong_FromLong(self->texture_obj);
}

PyGetSetDef MGLTextureCube_tp_getseters[] = {
	{(char *)"context", (getter)MGLTextureCube_get_context, 0, 0, 0},
	{(char *)"glo", (getter)MGLTextureCube_get_glo, 0, 0, 0},
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
	(initproc)MGLTextureCube_tp_init,                       // tp_init
	0,                                                      // tp_alloc
	MGLTextureCube_tp_new,                                  // tp_new
};

void MGLTextureCube_Invalidate(MGLTextureCube * texture) {
	if (Py_TYPE(texture) == &MGLInvalidObject_Type) {
		return;
	}

	texture->context->gl.DeleteTextures(1, (GLuint *)&texture->texture_obj);

	Py_DECREF(texture->context);
	Py_TYPE(texture) = &MGLInvalidObject_Type;
	Py_DECREF(texture);
}
