#include "Types.hpp"

#include "InlineMethods.hpp"

PyObject * MGLContext_texture(MGLContext * self, PyObject * args) {
	int width;
	int height;

	int components;

	PyObject * data;

	int samples;
	int alignment;

	const char * dtype;
	Py_ssize_t dtype_size;
	int internal_format_override;

	int args_ok = PyArg_ParseTuple(
		args,
		"(II)IOIIs#I",
		&width,
		&height,
		&components,
		&data,
		&samples,
		&alignment,
		&dtype,
		&dtype_size,
		&internal_format_override
	);

	if (!args_ok) {
		return 0;
	}

	if (components < 1 || components > 4) {
		MGLError_Set("the components must be 1, 2, 3 or 4");
		return 0;
	}

	if ((samples & (samples - 1)) || samples > self->max_samples) {
		MGLError_Set("the number of samples is invalid");
		return 0;
	}

	if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
		MGLError_Set("the alignment must be 1, 2, 4 or 8");
		return 0;
	}

	if (data != Py_None && samples) {
		MGLError_Set("multisample textures are not writable directly");
		return 0;
	}

	MGLDataType * data_type = from_dtype(dtype, dtype_size);

	if (!data_type) {
		MGLError_Set("invalid dtype");
		return 0;
	}

	int expected_size = width * components * data_type->size;
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * height;

	Py_buffer buffer_view;

	if (data != Py_None) {
		int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
		if (get_buffer < 0) {
			// Propagate the default error
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

	int texture_target = samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	int pixel_type = data_type->gl_type;
	int base_format = data_type->base_format[components];
	int internal_format = internal_format_override ? internal_format_override : data_type->internal_format[components];

	const GLMethods & gl = self->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);

	MGLTexture * texture = (MGLTexture *)MGLTexture_Type.tp_alloc(&MGLTexture_Type, 0);
    texture->external = false;

	texture->texture_obj = 0;
	gl.GenTextures(1, (GLuint *)&texture->texture_obj);

	if (!texture->texture_obj) {
		MGLError_Set("cannot create texture");
		Py_DECREF(texture);
		return 0;
	}

	gl.BindTexture(texture_target, texture->texture_obj);

	if (samples) {
		gl.TexImage2DMultisample(texture_target, samples, internal_format, width, height, true);
	} else {
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.TexImage2D(texture_target, 0, internal_format, width, height, 0, base_format, pixel_type, buffer_view.buf);
		if (data_type->float_type) {
			gl.TexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			gl.TexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else {
			gl.TexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			gl.TexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}

	if (data != Py_None) {
		PyBuffer_Release(&buffer_view);
	}

	texture->width = width;
	texture->height = height;
	texture->components = components;
	texture->samples = samples;
	texture->data_type = data_type;

	texture->max_level = 0;
	texture->compare_func = 0;
	texture->anisotropy = 1.0f;
	texture->depth = false;

	texture->min_filter = data_type->float_type ? GL_LINEAR : GL_NEAREST;
	texture->mag_filter = data_type->float_type ? GL_LINEAR : GL_NEAREST;

	texture->repeat_x = true;
	texture->repeat_y = true;

	Py_INCREF(self);
	texture->context = self;

	Py_INCREF(texture);

	PyObject * result = PyTuple_New(2);
	PyTuple_SET_ITEM(result, 0, (PyObject *)texture);
	PyTuple_SET_ITEM(result, 1, PyLong_FromLong(texture->texture_obj));
	return result;
}

PyObject * MGLContext_depth_texture(MGLContext * self, PyObject * args) {
	int width;
	int height;

	PyObject * data;

	int samples;
	int alignment;

	int args_ok = PyArg_ParseTuple(
		args,
		"(II)OII",
		&width,
		&height,
		&data,
		&samples,
		&alignment
	);

	if (!args_ok) {
		return 0;
	}

	if ((samples & (samples - 1)) || samples > self->max_samples) {
		MGLError_Set("the number of samples is invalid");
		return 0;
	}

	if (data != Py_None && samples) {
		MGLError_Set("multisample textures are not writable directly");
		return 0;
	}

	int expected_size = width * 4;
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * height;

	Py_buffer buffer_view;

	if (data != Py_None) {
		int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
		if (get_buffer < 0) {
			// Propagate the default error
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

	int texture_target = samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	int pixel_type = GL_FLOAT;

	const GLMethods & gl = self->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);

	MGLTexture * texture = (MGLTexture *)MGLTexture_Type.tp_alloc(&MGLTexture_Type, 0);
    texture->external = false;

	texture->texture_obj = 0;
	gl.GenTextures(1, (GLuint *)&texture->texture_obj);

	if (!texture->texture_obj) {
		MGLError_Set("cannot create texture");
		Py_DECREF(texture);
		return 0;
	}

	gl.BindTexture(texture_target, texture->texture_obj);

	if (samples) {
		gl.TexImage2DMultisample(texture_target, samples, GL_DEPTH_COMPONENT24, width, height, true);
	} else {
		gl.TexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		gl.TexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.TexImage2D(texture_target, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, pixel_type, buffer_view.buf);
		gl.TexParameteri(texture_target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		gl.TexParameteri(texture_target, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	}

	if (data != Py_None) {
		PyBuffer_Release(&buffer_view);
	}

	texture->width = width;
	texture->height = height;
	texture->components = 1;
	texture->samples = samples;
	texture->data_type = from_dtype("f4", 2);

	texture->compare_func = GL_LEQUAL;
	texture->depth = true;

	texture->min_filter = GL_LINEAR;
	texture->mag_filter = GL_LINEAR;
	texture->max_level = 0;

	texture->repeat_x = false;
	texture->repeat_y = false;

	Py_INCREF(self);
	texture->context = self;

	Py_INCREF(texture);

	PyObject * result = PyTuple_New(2);
	PyTuple_SET_ITEM(result, 0, (PyObject *)texture);
	PyTuple_SET_ITEM(result, 1, PyLong_FromLong(texture->texture_obj));
	return result;
}

PyObject * MGLContext_external_texture(MGLContext * self, PyObject * args) {
	int glo;
	int width;
	int height;
	int components;
	int samples;
	const char * dtype;
	Py_ssize_t dtype_size;

	int args_ok = PyArg_ParseTuple(
		args,
		"I(II)IIs#",
		&glo,
		&width,
		&height,
		&components,
		&samples,
		&dtype,
		&dtype_size
	);

	MGLDataType * data_type = from_dtype(dtype, dtype_size);

	if (!data_type) {
		MGLError_Set("invalid dtype");
		return 0;
	}

	MGLTexture * texture = (MGLTexture *)MGLTexture_Type.tp_alloc(&MGLTexture_Type, 0);
    texture->external = true;

	texture->texture_obj = glo;
	texture->width = width;
	texture->height = height;
	texture->components = components;
	texture->samples = samples;
	texture->data_type = data_type;

	texture->max_level = 0;
	texture->compare_func = 0;
	texture->anisotropy = 1.0f;
	texture->depth = false;

	texture->min_filter = data_type->float_type ? GL_LINEAR : GL_NEAREST;
	texture->mag_filter = data_type->float_type ? GL_LINEAR : GL_NEAREST;

	texture->repeat_x = true;
	texture->repeat_y = true;

	Py_INCREF(self);
	texture->context = self;

	Py_INCREF(texture);

	PyObject * result = PyTuple_New(2);
	PyTuple_SET_ITEM(result, 0, (PyObject *)texture);
	PyTuple_SET_ITEM(result, 1, PyLong_FromLong(texture->texture_obj));
	return result;
}

PyObject * MGLTexture_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLTexture * self = (MGLTexture *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLTexture_tp_dealloc(MGLTexture * self) {
	MGLTexture_Type.tp_free((PyObject *)self);
}

PyObject * MGLTexture_read(MGLTexture * self, PyObject * args) {
	int level;
	int alignment;

	int args_ok = PyArg_ParseTuple(
		args,
		"II",
		&level,
		&alignment
	);

	if (!args_ok) {
		return 0;
	}

	if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
		MGLError_Set("the alignment must be 1, 2, 4 or 8");
		return 0;
	}

	if (level > self->max_level) {
		MGLError_Set("invalid level");
		return 0;
	}

	if (self->samples) {
		MGLError_Set("multisample textures cannot be read directly");
		return 0;
	}

	int width = self->width / (1 << level);
	int height = self->height / (1 << level);

	width = width > 1 ? width : 1;
	height = height > 1 ? height : 1;

	int expected_size = width * self->components * self->data_type->size;
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * height;

	PyObject * result = PyBytes_FromStringAndSize(0, expected_size);
	char * data = PyBytes_AS_STRING(result);

	int pixel_type = self->data_type->gl_type;
	int base_format = self->depth ? GL_DEPTH_COMPONENT : self->data_type->base_format[self->components];

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_2D, self->texture_obj);

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

	gl.GetTexImage(GL_TEXTURE_2D, level, base_format, pixel_type, data);

	return result;
}

PyObject * MGLTexture_read_into(MGLTexture * self, PyObject * args) {
	PyObject * data;
	int level;
	int alignment;
	Py_ssize_t write_offset;

	int args_ok = PyArg_ParseTuple(
		args,
		"OIIn",
		&data,
		&level,
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

	if (level > self->max_level) {
		MGLError_Set("invalid level");
		return 0;
	}

	if (self->samples) {
		MGLError_Set("multisample textures cannot be read directly");
		return 0;
	}

	int width = self->width / (1 << level);
	int height = self->height / (1 << level);

	width = width > 1 ? width : 1;
	height = height > 1 ? height : 1;

	int expected_size = width * self->components * self->data_type->size;
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * height;

	int pixel_type = self->data_type->gl_type;
	int base_format = self->depth ? GL_DEPTH_COMPONENT : self->data_type->base_format[self->components];

	if (Py_TYPE(data) == &MGLBuffer_Type) {

		MGLBuffer * buffer = (MGLBuffer *)data;

		const GLMethods & gl = self->context->gl;

		gl.BindBuffer(GL_PIXEL_PACK_BUFFER, buffer->buffer_obj);
		gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
		gl.BindTexture(GL_TEXTURE_2D, self->texture_obj);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.GetTexImage(GL_TEXTURE_2D, level, base_format, pixel_type, (void *)write_offset);
		gl.BindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	} else {

		Py_buffer buffer_view;

		int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_WRITABLE);
		if (get_buffer < 0) {
			// Propagate the default error
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
		gl.BindTexture(GL_TEXTURE_2D, self->texture_obj);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.GetTexImage(GL_TEXTURE_2D, level, base_format, pixel_type, ptr);

		PyBuffer_Release(&buffer_view);

	}

	Py_RETURN_NONE;
}

PyObject * MGLTexture_write(MGLTexture * self, PyObject * args) {
	PyObject * data;
	PyObject * viewport;
	int level;
	int alignment;

	int args_ok = PyArg_ParseTuple(
		args,
		"OOII",
		&data,
		&viewport,
		&level,
		&alignment
	);

	if (!args_ok) {
		return 0;
	}

	if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
		MGLError_Set("the alignment must be 1, 2, 4 or 8");
		return 0;
	}

	if (level > self->max_level) {
		MGLError_Set("invalid level");
		return 0;
	}

	if (self->samples) {
		MGLError_Set("multisample textures cannot be written directly");
		return 0;
	}

	int x = 0;
	int y = 0;
	int width = self->width / (1 << level);
	int height = self->height / (1 << level);

	width = width > 1 ? width : 1;
	height = height > 1 ? height : 1;

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

	int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	int pixel_type = self->data_type->gl_type;
	int format = self->data_type->base_format[self->components];

	if (Py_TYPE(data) == &MGLBuffer_Type) {

		MGLBuffer * buffer = (MGLBuffer *)data;

		const GLMethods & gl = self->context->gl;

		gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer->buffer_obj);
		gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
		gl.BindTexture(texture_target, self->texture_obj);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.TexSubImage2D(texture_target, level, x, y, width, height, format, pixel_type, 0);
		gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	} else {

		int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
		if (get_buffer < 0) {
			// Propagate the default error
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
		gl.TexSubImage2D(texture_target, level, x, y, width, height, format, pixel_type, buffer_view.buf);

		PyBuffer_Release(&buffer_view);

	}

	Py_RETURN_NONE;
}

PyObject * MGLTexture_meth_bind(MGLTexture * self, PyObject * args) {
	int unit;
	int read;
	int write;
	int level;
	int format;

	int args_ok = PyArg_ParseTuple(
		args,
		"IppII",
		&unit,
		&read,
		&write,
		&level,
		&format
	);

	if (!args_ok) {
		return NULL;
	}

	int access = GL_READ_WRITE;
	if (read && !write) access = GL_READ_ONLY;
	else if (!read && write) access = GL_WRITE_ONLY;
	else if (!read && !write) {
		MGLError_Set("Illegal access mode. Read or write needs to be enabled.");
		return NULL;
	}

	int frmt = format ? format : self->data_type->internal_format[self->components];

    const GLMethods & gl = self->context->gl;
	gl.BindImageTexture(unit, self->texture_obj, level, 0, 0, access, frmt);
    Py_RETURN_NONE;
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

	if (base > self->max_level) {
		MGLError_Set("invalid base");
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

	self->min_filter = GL_LINEAR_MIPMAP_LINEAR;
	self->mag_filter = GL_LINEAR;
	self->max_level = max;

	Py_RETURN_NONE;
}

PyObject * MGLTexture_release(MGLTexture * self) {
	MGLTexture_Invalidate(self);
	Py_RETURN_NONE;
}

PyMethodDef MGLTexture_tp_methods[] = {
	{"write", (PyCFunction)MGLTexture_write, METH_VARARGS, 0},
	{"bind", (PyCFunction)MGLTexture_meth_bind, METH_VARARGS, 0},
	{"use", (PyCFunction)MGLTexture_use, METH_VARARGS, 0},
	{"build_mipmaps", (PyCFunction)MGLTexture_build_mipmaps, METH_VARARGS, 0},
	{"read", (PyCFunction)MGLTexture_read, METH_VARARGS, 0},
	{"read_into", (PyCFunction)MGLTexture_read_into, METH_VARARGS, 0},
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
	PyObject * res = PyTuple_New(2);
	PyTuple_SET_ITEM(res, 0, PyLong_FromLong(self->min_filter));
	PyTuple_SET_ITEM(res, 1, PyLong_FromLong(self->mag_filter));
	return res;
}

int MGLTexture_set_filter(MGLTexture * self, PyObject * value) {
	if (PyTuple_GET_SIZE(value) != 2) {
		MGLError_Set("invalid filter");
		return -1;
	}

	self->min_filter = PyLong_AsLong(PyTuple_GET_ITEM(value, 0));
	self->mag_filter = PyLong_AsLong(PyTuple_GET_ITEM(value, 1));

	int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(texture_target, self->texture_obj);
	gl.TexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, self->min_filter);
	gl.TexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, self->mag_filter);

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

PyObject * MGLTexture_get_compare_func(MGLTexture * self) {
	if (!self->depth) {
		MGLError_Set("only depth textures have compare_func");
		return 0;
	}

	return compare_func_to_string(self->compare_func);
}

int MGLTexture_set_compare_func(MGLTexture * self, PyObject * value) {
	if (!self->depth) {
		MGLError_Set("only depth textures have compare_func");
		return -1;
	}

	int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	const char * func = PyUnicode_AsUTF8(value);

	if (PyErr_Occurred()) {
		return -1;
	}

	self->compare_func = compare_func_from_string(func);

	const GLMethods & gl = self->context->gl;
	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(texture_target, self->texture_obj);
	if (self->compare_func == 0) {
		gl.TexParameteri(texture_target, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	} else {
		gl.TexParameteri(texture_target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		gl.TexParameteri(texture_target, GL_TEXTURE_COMPARE_FUNC, self->compare_func);
	}

	return 0;
}

PyObject * MGLTexture_get_anisotropy(MGLTexture * self) {
	return PyFloat_FromDouble(self->anisotropy);
}

int MGLTexture_set_anisotropy(MGLTexture * self, PyObject * value) {
	self->anisotropy = (float)min(max(PyFloat_AsDouble(value), 1.0), self->context->max_anisotropy);
	int texture_target = self->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(texture_target, self->texture_obj);
	gl.TexParameterf(texture_target, GL_TEXTURE_MAX_ANISOTROPY, self->anisotropy);

	return 0;
}

PyGetSetDef MGLTexture_tp_getseters[] = {
	{(char *)"repeat_x", (getter)MGLTexture_get_repeat_x, (setter)MGLTexture_set_repeat_x, 0, 0},
	{(char *)"repeat_y", (getter)MGLTexture_get_repeat_y, (setter)MGLTexture_set_repeat_y, 0, 0},
	{(char *)"filter", (getter)MGLTexture_get_filter, (setter)MGLTexture_set_filter, 0, 0},
	{(char *)"swizzle", (getter)MGLTexture_get_swizzle, (setter)MGLTexture_set_swizzle, 0, 0},
	{(char *)"compare_func", (getter)MGLTexture_get_compare_func, (setter)MGLTexture_set_compare_func, 0, 0},
	{(char *)"anisotropy", (getter)MGLTexture_get_anisotropy, (setter)MGLTexture_set_anisotropy, 0, 0},
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
	0,                                                      // tp_init
	0,                                                      // tp_alloc
	MGLTexture_tp_new,                                      // tp_new
};

void MGLTexture_Invalidate(MGLTexture * texture) {
	if (Py_TYPE(texture) == &MGLInvalidObject_Type) {
		return;
	}

	const GLMethods & gl = texture->context->gl;
	gl.DeleteTextures(1, (GLuint *)&texture->texture_obj);

	Py_DECREF(texture->context);
	Py_SET_TYPE(texture, &MGLInvalidObject_Type);
	Py_DECREF(texture);
}
