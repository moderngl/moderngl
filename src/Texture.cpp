#include "Texture.hpp"

#include "Error.hpp"
#include "InvalidObject.hpp"
#include "Buffer.hpp"

#include "InlineMethods.hpp"

PyObject * MGLTexture_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLTexture * self = (MGLTexture *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLTexture_tp_dealloc(MGLTexture * self) {
	MGLTexture_Type.tp_free((PyObject *)self);
}

int MGLTexture_tp_init(MGLTexture * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.Texture manually");
	return -1;
}

PyObject * MGLTexture_read(MGLTexture * self, PyObject * args) {
	int alignment;

	int args_ok = PyArg_ParseTuple(
		args,
		"I",
		&alignment
	);

	if (!args_ok) {
		return 0;
	}

	if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
		MGLError_Set("the alignment must be 1, 2, 4 or 8");
		return 0;
	}

	if (self->samples) {
		MGLError_Set("multisample textures cannot be read directly");
		return 0;
	}

	int expected_size = self->width * self->components * (self->floats ? 4 : 1);
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * self->height;

	PyObject * result = PyBytes_FromStringAndSize(0, expected_size);
	char * data = PyBytes_AS_STRING(result);

	const int formats[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};

	int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	int pixel_type = self->floats ? GL_FLOAT : GL_UNSIGNED_BYTE;
	int format = formats[self->components];

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(texture_target, self->texture_obj);

	gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
	gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);

	// To determine the required size of pixels, use glGetTexLevelParameter to determine
	// the dimensions of the internal texture image, then scale the required number of pixels
	// by the storage required for each pixel, based on format and type. Be sure to take the
	// pixel storage parameters into account, especially GL_PACK_ALIGNMENT.

	// int pack = 0;
	// gl.GetIntegerv(GL_PACK_ALIGNMENT, &pack);
	// printf("GL_PACK_ALIGNMENT: %d\n", pack);

	// glGetTexLevelParameter with argument GL_TEXTURE_WIDTH
	// glGetTexLevelParameter with argument GL_TEXTURE_HEIGHT
	// glGetTexLevelParameter with argument GL_TEXTURE_INTERNAL_FORMAT

	// int level_width = 0;
	// int level_height = 0;
	// gl.GetTexLevelParameteriv(texture_target, 0, GL_TEXTURE_WIDTH, &level_width);
	// gl.GetTexLevelParameteriv(texture_target, 0, GL_TEXTURE_HEIGHT, &level_height);
	// printf("level_width: %d\n", level_width);
	// printf("level_height: %d\n", level_height);

	gl.GetTexImage(texture_target, 0, format, pixel_type, data);

	return result;
}

PyObject * MGLTexture_read_into(MGLTexture * self, PyObject * args) {
	PyObject * data;
	int alignment;
	Py_ssize_t write_offset;

	int args_ok = PyArg_ParseTuple(
		args,
		"OIn",
		&data,
		&alignment,
		&write_offset
	);

	if (!args_ok) {
		return 0;
	}

	if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
		MGLError_Set("the alignment must be 1, 2, 4 or 8");
		return 0;
	}

	if (self->samples) {
		MGLError_Set("multisample textures cannot be read directly");
		return 0;
	}

	int expected_size = self->width * self->components * (self->floats ? 4 : 1);
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * self->height;

	const int formats[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};

	int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	int pixel_type = self->floats ? GL_FLOAT : GL_UNSIGNED_BYTE;
	int format = formats[self->components];

	if (Py_TYPE(data) == &MGLBuffer_Type) {

		MGLBuffer * buffer = (MGLBuffer *)data;

		const GLMethods & gl = self->context->gl;

		gl.BindBuffer(GL_PIXEL_PACK_BUFFER, buffer->buffer_obj);
		gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
		gl.BindTexture(texture_target, self->texture_obj);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.GetTexImage(texture_target, 0, format, pixel_type, (void *)write_offset);
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
		gl.BindTexture(texture_target, self->texture_obj);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.GetTexImage(texture_target, 0, format, pixel_type, ptr);

		PyBuffer_Release(&buffer_view);

	}

	Py_RETURN_NONE;
}

PyObject * MGLTexture_write(MGLTexture * self, PyObject * args) {
	PyObject * data;
	PyObject * viewport;
	int alignment;

	int args_ok = PyArg_ParseTuple(
		args,
		"OOI",
		&data,
		&viewport,
		&alignment
	);

	if (!args_ok) {
		return 0;
	}

	if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
		MGLError_Set("the alignment must be 1, 2, 4 or 8");
		return 0;
	}

	if (self->samples) {
		MGLError_Set("multisample textures cannot be written directly");
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

	int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	int pixel_type = self->floats ? GL_FLOAT : GL_UNSIGNED_BYTE;
	int format = formats[self->components];

	if (Py_TYPE(data) == &MGLBuffer_Type) {

		MGLBuffer * buffer = (MGLBuffer *)data;

		const GLMethods & gl = self->context->gl;

		gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer->buffer_obj);
		gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
		gl.BindTexture(texture_target, self->texture_obj);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.TexSubImage2D(texture_target, 0, x, y, width, height, format, pixel_type, 0);
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
		gl.BindTexture(texture_target, self->texture_obj);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.TexSubImage2D(texture_target, 0, x, y, width, height, format, pixel_type, buffer_view.buf);

		PyBuffer_Release(&buffer_view);

	}

	Py_RETURN_NONE;
}

PyObject * MGLTexture_clear(MGLTexture * self, PyObject * args) {

	// TODO:

	PyErr_Format(PyExc_NotImplementedError, "NYI");
	return 0;
}

PyObject * MGLTexture_use(MGLTexture * self, PyObject * args) {
	int index;

	int args_ok = PyArg_ParseTuple(
		args,
		"I",
		&index
	);

	if (!args_ok) {
		return 0;
	}

	int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	const GLMethods & gl = self->context->gl;
	gl.ActiveTexture(GL_TEXTURE0 + index);
	gl.BindTexture(texture_target, self->texture_obj);

	Py_RETURN_NONE;
}

PyObject * MGLTexture_build_mipmaps(MGLTexture * self, PyObject * args) {
	int base = 0;
	int max = 1000;

	int args_ok = PyArg_ParseTuple(
		args,
		"II",
		&base,
		&max
	);

	if (!args_ok) {
		return 0;
	}

	int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(texture_target, self->texture_obj);

	gl.TexParameteri(texture_target, GL_TEXTURE_BASE_LEVEL, base);
	gl.TexParameteri(texture_target, GL_TEXTURE_MAX_LEVEL, max);

	gl.GenerateMipmap(texture_target);

	gl.TexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gl.TexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// TODO: filter attrib

	Py_RETURN_NONE;
}

PyObject * MGLTexture_release(MGLTexture * self) {
	MGLTexture_Invalidate(self);
	Py_RETURN_NONE;
}

PyMethodDef MGLTexture_tp_methods[] = {
	{"read", (PyCFunction)MGLTexture_read, METH_VARARGS, 0},
	{"read_into", (PyCFunction)MGLTexture_read_into, METH_VARARGS, 0},
	{"write", (PyCFunction)MGLTexture_write, METH_VARARGS, 0},
	{"clear", (PyCFunction)MGLTexture_clear, METH_VARARGS, 0},
	{"use", (PyCFunction)MGLTexture_use, METH_VARARGS, 0},
	{"build_mipmaps", (PyCFunction)MGLTexture_build_mipmaps, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLTexture_release, METH_NOARGS, 0},
	{0},
};

PyObject * MGLTexture_get_repeat_x(MGLTexture * self) {
	return PyBool_FromLong(self->repeat_x);
}

int MGLTexture_set_repeat_x(MGLTexture * self, PyObject * value) {
	int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(texture_target, self->texture_obj);

	if (value == Py_True) {
		gl.TexParameteri(texture_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		self->repeat_x = true;
		return 0;
	} else if (value == Py_False) {
		gl.TexParameteri(texture_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		self->repeat_x = false;
		return 0;
	} else {
		MGLError_Set("invalid value for texture_x");
		return -1;
	}
}

PyObject * MGLTexture_get_repeat_y(MGLTexture * self) {
	return PyBool_FromLong(self->repeat_y);
}

int MGLTexture_set_repeat_y(MGLTexture * self, PyObject * value) {
	int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(texture_target, self->texture_obj);

	if (value == Py_True) {
		gl.TexParameteri(texture_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		self->repeat_y = true;
		return 0;
	} else if (value == Py_False) {
		gl.TexParameteri(texture_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		self->repeat_y = false;
		return 0;
	} else {
		MGLError_Set("invalid value for texture_y");
		return -1;
	}
}

PyObject * MGLTexture_get_filter(MGLTexture * self) {
	Py_INCREF(self->filter->wrapper);
	return self->filter->wrapper;
}

int MGLTexture_set_filter(MGLTexture * self, PyObject * value) {
	if (Py_TYPE(value) != &MGLTextureFilter_Type) {
		MGLError_Set("the value must be a TextureFilter not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	MGLTextureFilter * filter = (MGLTextureFilter *)value;

	Py_INCREF(filter);
	Py_DECREF(self->filter);
	self->filter = filter;

	int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(texture_target, self->texture_obj);
	gl.TexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, filter->min_filter);
	gl.TexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, filter->mag_filter);

	return 0;
}

PyObject * MGLTexture_get_swizzle(MGLTexture * self, void * closure) {

	if (self->depth) {
		MGLError_Set("cannot get swizzle of depth textures");
		return 0;
	}

	int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(texture_target, self->texture_obj);

	int swizzle_r = 0;
	int swizzle_g = 0;
	int swizzle_b = 0;
	int swizzle_a = 0;

	gl.GetTexParameteriv(texture_target, GL_TEXTURE_SWIZZLE_R, &swizzle_r);
	gl.GetTexParameteriv(texture_target, GL_TEXTURE_SWIZZLE_G, &swizzle_g);
	gl.GetTexParameteriv(texture_target, GL_TEXTURE_SWIZZLE_B, &swizzle_b);
	gl.GetTexParameteriv(texture_target, GL_TEXTURE_SWIZZLE_A, &swizzle_a);

	char swizzle[5] = {
		char_from_swizzle(swizzle_r),
		char_from_swizzle(swizzle_g),
		char_from_swizzle(swizzle_b),
		char_from_swizzle(swizzle_a),
		0,
	};

	return PyUnicode_FromStringAndSize(swizzle, 4);
}

int MGLTexture_set_swizzle(MGLTexture * self, PyObject * value, void * closure) {
	const char * swizzle = PyUnicode_AsUTF8(value);

	if (self->depth) {
		MGLError_Set("cannot set swizzle for depth textures");
		return -1;
	}

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

	int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(texture_target, self->texture_obj);

	gl.TexParameteri(texture_target, GL_TEXTURE_SWIZZLE_R, tex_swizzle[0]);
	if (tex_swizzle[1] != -1) {
		gl.TexParameteri(texture_target, GL_TEXTURE_SWIZZLE_G, tex_swizzle[1]);
		if (tex_swizzle[2] != -1) {
			gl.TexParameteri(texture_target, GL_TEXTURE_SWIZZLE_B, tex_swizzle[2]);
			if (tex_swizzle[3] != -1) {
				gl.TexParameteri(texture_target, GL_TEXTURE_SWIZZLE_A, tex_swizzle[3]);
			}
		}
	}

	return 0;
}

PyObject * MGLTexture_get_width(MGLTexture * self, void * closure) {
	return PyLong_FromLong(self->width);
}

PyObject * MGLTexture_get_height(MGLTexture * self, void * closure) {
	return PyLong_FromLong(self->height);
}

PyObject * MGLTexture_get_components(MGLTexture * self, void * closure) {
	return PyLong_FromLong(self->components);
}

PyObject * MGLTexture_get_samples(MGLTexture * self, void * closure) {
	return PyLong_FromLong(self->samples);
}

PyObject * MGLTexture_get_floats(MGLTexture * self, void * closure) {
	return PyBool_FromLong(self->floats);
}

PyObject * MGLTexture_get_depth(MGLTexture * self, void * closure) {
	return PyBool_FromLong(self->depth);
}

MGLContext * MGLTexture_get_context(MGLTexture * self, void * closure) {
	Py_INCREF(self->context);
	return self->context;
}

PyObject * MGLTexture_get_glo(MGLTexture * self, void * closure) {
	return PyLong_FromLong(self->texture_obj);
}

PyGetSetDef MGLTexture_tp_getseters[] = {
	{(char *)"repeat_x", (getter)MGLTexture_get_repeat_x, (setter)MGLTexture_set_repeat_x, 0, 0},
	{(char *)"repeat_y", (getter)MGLTexture_get_repeat_y, (setter)MGLTexture_set_repeat_y, 0, 0},
	{(char *)"filter", (getter)MGLTexture_get_filter, (setter)MGLTexture_set_filter, 0, 0},
	{(char *)"swizzle", (getter)MGLTexture_get_swizzle, (setter)MGLTexture_set_swizzle, 0, 0},

	{(char *)"width", (getter)MGLTexture_get_width, 0, 0, 0},
	{(char *)"height", (getter)MGLTexture_get_height, 0, 0, 0},
	{(char *)"components", (getter)MGLTexture_get_components, 0, 0, 0},
	{(char *)"samples", (getter)MGLTexture_get_samples, 0, 0, 0},
	{(char *)"floats", (getter)MGLTexture_get_floats, 0, 0, 0},
	{(char *)"depth", (getter)MGLTexture_get_depth, 0, 0, 0},
	{(char *)"context", (getter)MGLTexture_get_context, 0, 0, 0},
	{(char *)"glo", (getter)MGLTexture_get_glo, 0, 0, 0},
	{0},
};

PyTypeObject MGLTexture_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.Texture",                                          // tp_name
	sizeof(MGLTexture),                                     // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLTexture_tp_dealloc,                      // tp_dealloc
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
	MGLTexture_tp_methods,                                  // tp_methods
	0,                                                      // tp_members
	MGLTexture_tp_getseters,                                // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLTexture_tp_init,                           // tp_init
	0,                                                      // tp_alloc
	MGLTexture_tp_new,                                      // tp_new
};

MGLTexture * MGLTexture_New() {
	MGLTexture * self = (MGLTexture *)MGLTexture_tp_new(&MGLTexture_Type, 0, 0);
	return self;
}

void MGLTexture_Invalidate(MGLTexture * texture) {
	if (Py_TYPE(texture) == &MGLInvalidObject_Type) {
		return;
	}

	texture->context->gl.DeleteTextures(1, (GLuint *)&texture->texture_obj);

	Py_DECREF(texture->context);
	Py_TYPE(texture) = &MGLInvalidObject_Type;
	Py_DECREF(texture);
}
