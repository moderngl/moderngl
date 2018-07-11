#include "Types.hpp"

PyObject * MGLContext_framebuffer(MGLContext * self, PyObject * args) {
	PyObject * color_attachments;
	PyObject * depth_attachment;

	int args_ok = PyArg_ParseTuple(
		args,
		"OO",
		&color_attachments,
		&depth_attachment
	);

	if (!args_ok) {
		return 0;
	}

	// If the attachment sizes are not all identical, rendering will be limited to the
	// largest area that can fit in all of the attachments (an intersection of rectangles
	// having a lower left of (0; 0) and an upper right of (width; height) for each
	// attachment).

	int width = 0;
	int height = 0;
	int samples = 0;

	int color_attachments_len = (int)PyTuple_GET_SIZE(color_attachments);

	if (!color_attachments_len && depth_attachment == Py_None) {
		MGLError_Set("the framebuffer is empty");
		return 0;
	}

	// if (!color_attachments_len) {
	// 	MGLError_Set("the color_attachments must not be empty");
	// 	return 0;
	// }

	for (int i = 0; i < color_attachments_len; ++i) {
		PyObject * item = PyTuple_GET_ITEM(color_attachments, i);

		if (Py_TYPE(item) == &MGLTexture_Type) {
			MGLTexture * texture = (MGLTexture *)item;

			if (texture->depth) {
				MGLError_Set("color_attachments[%d] is a depth attachment", i);
				return 0;
			}

			if (i == 0) {
				width = texture->width;
				height = texture->height;
				samples = texture->samples;
			} else {
				if (texture->width != width || texture->height != height || texture->samples != samples) {
					MGLError_Set("the color_attachments have different sizes or samples");
					return 0;
				}
			}

			if (texture->context != self) {
				MGLError_Set("color_attachments[%d] belongs to a different context", i);
				return 0;
			}
		} else if (Py_TYPE(item) == &MGLRenderbuffer_Type) {
			MGLRenderbuffer * renderbuffer = (MGLRenderbuffer *)item;

			if (renderbuffer->depth) {
				MGLError_Set("color_attachments[%d] is a depth attachment", i);
				return 0;
			}

			if (i == 0) {
				width = renderbuffer->width;
				height = renderbuffer->height;
				samples = renderbuffer->samples;
			} else {
				if (renderbuffer->width != width || renderbuffer->height != height || renderbuffer->samples != samples) {
					MGLError_Set("the color_attachments have different sizes or samples");
					return 0;
				}
			}

			if (renderbuffer->context != self) {
				MGLError_Set("color_attachments[%d] belongs to a different context", i);
				return 0;
			}
		} else {
			MGLError_Set("color_attachments[%d] must be a Renderbuffer or Texture not %s", i, Py_TYPE(item)->tp_name);
			return 0;
		}
	}

	const GLMethods & gl = self->gl;

	if (depth_attachment != Py_None) {

		if (Py_TYPE(depth_attachment) == &MGLTexture_Type) {
			MGLTexture * texture = (MGLTexture *)depth_attachment;

			if (!texture->depth) {
				MGLError_Set("the depth_attachment is a color attachment");
				return 0;
			}

			if (texture->context != self) {
				MGLError_Set("the depth_attachment belongs to a different context");
				return 0;
			}

			if (texture->width != width || texture->height != height || texture->samples != samples) {
				MGLError_Set("the depth_attachment have different sizes or samples");
				return 0;
			}
		} else if (Py_TYPE(depth_attachment) == &MGLRenderbuffer_Type) {
			MGLRenderbuffer * renderbuffer = (MGLRenderbuffer *)depth_attachment;

			if (!renderbuffer->depth) {
				MGLError_Set("the depth_attachment is a color attachment");
				return 0;
			}

			if (renderbuffer->context != self) {
				MGLError_Set("the depth_attachment belongs to a different context");
				return 0;
			}

			if (renderbuffer->width != width || renderbuffer->height != height || renderbuffer->samples != samples) {
				MGLError_Set("the depth_attachment have different sizes or samples");
				return 0;
			}
		} else {
			MGLError_Set("the depth_attachment must be a Renderbuffer or Texture not %s", Py_TYPE(depth_attachment)->tp_name);
			return 0;
		}
	}

	MGLFramebuffer * framebuffer = (MGLFramebuffer *)MGLFramebuffer_Type.tp_alloc(&MGLFramebuffer_Type, 0);

	framebuffer->framebuffer_obj = 0;
	gl.GenFramebuffers(1, (GLuint *)&framebuffer->framebuffer_obj);

	if (!framebuffer->framebuffer_obj) {
		MGLError_Set("cannot create framebuffer");
		Py_DECREF(framebuffer);
		return 0;
	}

	gl.BindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebuffer_obj);

	for (int i = 0; i < color_attachments_len; ++i) {
		PyObject * item = PyTuple_GET_ITEM(color_attachments, i);

		if (Py_TYPE(item) == &MGLTexture_Type) {

			MGLTexture * texture = (MGLTexture *)item;

			gl.FramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0 + i,
				texture->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D,
				texture->texture_obj,
				0
			);

		} else if (Py_TYPE(item) == &MGLRenderbuffer_Type) {

			MGLRenderbuffer * renderbuffer = (MGLRenderbuffer *)item;

			gl.FramebufferRenderbuffer(
				GL_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0 + i,
				GL_RENDERBUFFER,
				renderbuffer->renderbuffer_obj
			);
		}
	}

	if (Py_TYPE(depth_attachment) == &MGLTexture_Type) {
		MGLTexture * texture = (MGLTexture *)depth_attachment;

		gl.FramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			texture->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D,
			texture->texture_obj,
			0
		);

	} else if (Py_TYPE(depth_attachment) == &MGLRenderbuffer_Type) {
		MGLRenderbuffer * renderbuffer = (MGLRenderbuffer *)depth_attachment;

		gl.FramebufferRenderbuffer(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER,
			renderbuffer->renderbuffer_obj
		);
	}

	int status = gl.CheckFramebufferStatus(GL_FRAMEBUFFER);

	gl.BindFramebuffer(GL_FRAMEBUFFER, self->bound_framebuffer->framebuffer_obj);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		const char * message = "the framebuffer is not complete";

		switch (status) {
			case GL_FRAMEBUFFER_UNDEFINED:
				message = "the framebuffer is not complete (UNDEFINED)";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				message = "the framebuffer is not complete (INCOMPLETE_ATTACHMENT)";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				message = "the framebuffer is not complete (INCOMPLETE_MISSING_ATTACHMENT)";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				message = "the framebuffer is not complete (INCOMPLETE_DRAW_BUFFER)";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				message = "the framebuffer is not complete (INCOMPLETE_READ_BUFFER)";
				break;

			case GL_FRAMEBUFFER_UNSUPPORTED:
				message = "the framebuffer is not complete (UNSUPPORTED)";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				message = "the framebuffer is not complete (INCOMPLETE_MULTISAMPLE)";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				message = "the framebuffer is not complete (INCOMPLETE_LAYER_TARGETS)";
				break;
		}

		MGLError_Set(message);
		return 0;
	}

	framebuffer->draw_buffers = new unsigned[color_attachments_len];
	framebuffer->draw_buffers_len = color_attachments_len;

	for (int i = 0; i < color_attachments_len; ++i) {
		framebuffer->draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	framebuffer->color_mask = new bool[color_attachments_len * 4 + 1];

	for (int i = 0; i < color_attachments_len; ++i) {
		PyObject * item = PyTuple_GET_ITEM(color_attachments, i);
		if (Py_TYPE(item) == &MGLTexture_Type) {
			MGLTexture * texture = (MGLTexture *)item;
			framebuffer->color_mask[i * 4 + 0] = texture->components >= 1;
			framebuffer->color_mask[i * 4 + 1] = texture->components >= 2;
			framebuffer->color_mask[i * 4 + 2] = texture->components >= 3;
			framebuffer->color_mask[i * 4 + 3] = texture->components >= 4;
		} else if (Py_TYPE(item) == &MGLRenderbuffer_Type) {
			MGLTexture * renderbuffer = (MGLTexture *)item;
			framebuffer->color_mask[i * 4 + 0] = renderbuffer->components >= 1;
			framebuffer->color_mask[i * 4 + 1] = renderbuffer->components >= 2;
			framebuffer->color_mask[i * 4 + 2] = renderbuffer->components >= 3;
			framebuffer->color_mask[i * 4 + 3] = renderbuffer->components >= 4;
		}
	}

	framebuffer->depth_mask = (depth_attachment != Py_None);

	framebuffer->viewport_x = 0;
	framebuffer->viewport_y = 0;
	framebuffer->viewport_width = width;
	framebuffer->viewport_height = height;

	framebuffer->width = width;
	framebuffer->height = height;
	framebuffer->samples = samples;

	Py_INCREF(self);
	framebuffer->context = self;

	Py_INCREF(framebuffer);

	PyObject * size = PyTuple_New(2);
	PyTuple_SET_ITEM(size, 0, PyLong_FromLong(framebuffer->width));
	PyTuple_SET_ITEM(size, 1, PyLong_FromLong(framebuffer->height));

	Py_INCREF(framebuffer);
	PyObject * result = PyTuple_New(4);
	PyTuple_SET_ITEM(result, 0, (PyObject *)framebuffer);
	PyTuple_SET_ITEM(result, 1, size);
	PyTuple_SET_ITEM(result, 2, PyLong_FromLong(framebuffer->samples));
	PyTuple_SET_ITEM(result, 3, PyLong_FromLong(framebuffer->framebuffer_obj));
	return result;
}

PyObject * MGLFramebuffer_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLFramebuffer * self = (MGLFramebuffer *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLFramebuffer_tp_dealloc(MGLFramebuffer * self) {
	MGLFramebuffer_Type.tp_free((PyObject *)self);
}

PyObject * MGLFramebuffer_release(MGLFramebuffer * self) {
	MGLFramebuffer_Invalidate(self);
	Py_RETURN_NONE;
}

PyObject * MGLFramebuffer_clear(MGLFramebuffer * self, PyObject * args) {
	float r, g, b, a, depth;
	PyObject * viewport;

	int args_ok = PyArg_ParseTuple(
		args,
		"fffffO",
		&r,
		&g,
		&b,
		&a,
		&depth,
		&viewport
	);

	if (!args_ok) {
		return 0;
	}

	int x = 0;
	int y = 0;
	int width = self->width;
	int height = self->height;

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

	const GLMethods & gl = self->context->gl;

	gl.BindFramebuffer(GL_FRAMEBUFFER, self->framebuffer_obj);
	gl.DrawBuffers(self->draw_buffers_len, self->draw_buffers);
	gl.ClearColor(r, g, b, a);
	gl.ClearDepth(depth);

	for (int i = 0; i < self->draw_buffers_len; ++i) {
		gl.ColorMaski(
			i,
			self->color_mask[i * 4 + 0],
			self->color_mask[i * 4 + 1],
			self->color_mask[i * 4 + 2],
			self->color_mask[i * 4 + 3]
		);
	}

	gl.DepthMask(self->depth_mask);

	if (viewport != Py_None) {
		gl.Enable(GL_SCISSOR_TEST);
		gl.Scissor(x, y, width, height);
		gl.Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		gl.Disable(GL_SCISSOR_TEST);
	} else {
		gl.Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	}

	gl.BindFramebuffer(GL_FRAMEBUFFER, self->context->bound_framebuffer->framebuffer_obj);

	Py_RETURN_NONE;
}

PyObject * MGLFramebuffer_use(MGLFramebuffer * self) {
	const GLMethods & gl = self->context->gl;

	gl.BindFramebuffer(GL_FRAMEBUFFER, self->framebuffer_obj);

	if (self->framebuffer_obj) {
		gl.DrawBuffers(self->draw_buffers_len, self->draw_buffers);
	}

	if (self->viewport_width && self->viewport_height) {
		gl.Viewport(
			self->viewport_x,
			self->viewport_y,
			self->viewport_width,
			self->viewport_height
		);
	}

	for (int i = 0; i < self->draw_buffers_len; ++i) {
		gl.ColorMaski(
			i,
			self->color_mask[i * 4 + 0],
			self->color_mask[i * 4 + 1],
			self->color_mask[i * 4 + 2],
			self->color_mask[i * 4 + 3]
		);
	}

	gl.DepthMask(self->depth_mask);

	Py_INCREF(self);
	Py_DECREF(self->context->bound_framebuffer);
	self->context->bound_framebuffer = self;

	Py_RETURN_NONE;
}

PyObject * MGLFramebuffer_read(MGLFramebuffer * self, PyObject * args) {
	PyObject * viewport;
	int components;
	int alignment;
	int attachment;

	const char * dtype;
	Py_ssize_t dtype_size;

	int args_ok = PyArg_ParseTuple(
		args,
		"OIIIs#",
		&viewport,
		&components,
		&attachment,
		&alignment,
		&dtype,
		&dtype_size
	);

	if (!args_ok) {
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

	int x = 0;
	int y = 0;
	int width = self->width;
	int height = self->height;

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

	bool read_depth = false;

	if (components == -1) {
		components = 1;
		read_depth = true;
		data_type = from_dtype("f4");
	}

	int expected_size = width * components * data_type->size;
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * height;

	int pixel_type = data_type->gl_type;

	const int base_formats[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};
	int base_format = base_formats[components];

	PyObject * result = PyBytes_FromStringAndSize(0, expected_size);
	char * data = PyBytes_AS_STRING(result);

	const GLMethods & gl = self->context->gl;

	gl.BindFramebuffer(GL_FRAMEBUFFER, self->framebuffer_obj);
	// if (self->framebuffer_obj) {
	if (!read_depth) {
		gl.ReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
	} else {
		gl.ReadBuffer(GL_NONE);
	}
	// } else {
	// gl.ReadBuffer(GL_BACK_LEFT);
	// gl.ReadBuffer(self->draw_buffers[0]);
	// }
	gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
	gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
	if (!read_depth) {
		gl.ReadPixels(x, y, width, height, base_format, pixel_type, data);
	} else {
		gl.ReadPixels(x, y, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, data);
	}
	gl.BindFramebuffer(GL_FRAMEBUFFER, self->context->bound_framebuffer->framebuffer_obj);

	return result;
}

PyObject * MGLFramebuffer_read_into(MGLFramebuffer * self, PyObject * args) {
	PyObject * data;
	PyObject * viewport;
	int components;
	int attachment;
	int alignment;
	const char * dtype;
	Py_ssize_t dtype_size;
	Py_ssize_t write_offset;

	int args_ok = PyArg_ParseTuple(
		args,
		"OOIIIs#n",
		&data,
		&viewport,
		&components,
		&attachment,
		&alignment,
		&dtype,
		&dtype_size,
		&write_offset
	);

	if (!args_ok) {
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

	int x = 0;
	int y = 0;
	int width = self->width;
	int height = self->height;

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

	int expected_size = width * components * data_type->size;
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * height;

	int pixel_type = data_type->gl_type;

	const int base_formats[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};
	int base_format = base_formats[components];

	if (Py_TYPE(data) == &MGLBuffer_Type) {

		MGLBuffer * buffer = (MGLBuffer *)data;

		const GLMethods & gl = self->context->gl;

		gl.BindBuffer(GL_PIXEL_PACK_BUFFER, buffer->buffer_obj);
		gl.BindFramebuffer(GL_FRAMEBUFFER, self->framebuffer_obj);
		gl.ReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.ReadPixels(x, y, width, height, base_format, pixel_type, (void *)write_offset);
		gl.BindFramebuffer(GL_FRAMEBUFFER, self->context->bound_framebuffer->framebuffer_obj);
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

		gl.BindFramebuffer(GL_FRAMEBUFFER, self->framebuffer_obj);
		gl.ReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.ReadPixels(x, y, width, height, base_format, pixel_type, ptr);
		gl.BindFramebuffer(GL_FRAMEBUFFER, self->context->bound_framebuffer->framebuffer_obj);

		PyBuffer_Release(&buffer_view);
	}

	return PyLong_FromLong(expected_size);
}

PyMethodDef MGLFramebuffer_tp_methods[] = {
	{"clear", (PyCFunction)MGLFramebuffer_clear, METH_VARARGS, 0},
	{"use", (PyCFunction)MGLFramebuffer_use, METH_NOARGS, 0},
	{"read", (PyCFunction)MGLFramebuffer_read, METH_VARARGS, 0},
	{"read_into", (PyCFunction)MGLFramebuffer_read_into, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLFramebuffer_release, METH_NOARGS, 0},
	{0},
};

PyObject * MGLFramebuffer_get_viewport(MGLFramebuffer * self, void * closure) {
	PyObject * x = PyLong_FromLong(self->viewport_x);
	PyObject * y = PyLong_FromLong(self->viewport_y);
	PyObject * width = PyLong_FromLong(self->viewport_width);
	PyObject * height = PyLong_FromLong(self->viewport_height);
	return PyTuple_Pack(4, x, y, width, height);
}

int MGLFramebuffer_set_viewport(MGLFramebuffer * self, PyObject * value, void * closure) {
	if (PyTuple_GET_SIZE(value) != 4) {
		MGLError_Set("the viewport must be a 4-tuple not %d-tuple", PyTuple_GET_SIZE(value));
		return -1;
	}

	int viewport_x = PyLong_AsLong(PyTuple_GET_ITEM(value, 0));
	int viewport_y = PyLong_AsLong(PyTuple_GET_ITEM(value, 1));
	int viewport_width = PyLong_AsLong(PyTuple_GET_ITEM(value, 2));
	int viewport_height = PyLong_AsLong(PyTuple_GET_ITEM(value, 3));

	if (PyErr_Occurred()) {
		MGLError_Set("the viewport is invalid");
		return -1;
	}

	self->viewport_x = viewport_x;
	self->viewport_y = viewport_y;
	self->viewport_width = viewport_width;
	self->viewport_height = viewport_height;

	if (self->framebuffer_obj == self->context->bound_framebuffer->framebuffer_obj) {
		const GLMethods & gl = self->context->gl;

		gl.Viewport(
			self->viewport_x,
			self->viewport_y,
			self->viewport_width,
			self->viewport_height
		);
	}

	return 0;
}

PyObject * MGLFramebuffer_get_color_mask(MGLFramebuffer * self, void * closure) {
	if (self->draw_buffers_len == 1) {
		PyObject * color_mask = PyTuple_New(4);
		PyTuple_SET_ITEM(color_mask, 0, PyBool_FromLong(self->color_mask[0]));
		PyTuple_SET_ITEM(color_mask, 1, PyBool_FromLong(self->color_mask[1]));
		PyTuple_SET_ITEM(color_mask, 2, PyBool_FromLong(self->color_mask[2]));
		PyTuple_SET_ITEM(color_mask, 3, PyBool_FromLong(self->color_mask[3]));
		return color_mask;
	}

	PyObject * res = PyTuple_New(self->draw_buffers_len);

	for (int i = 0; i < self->draw_buffers_len; ++i) {
		PyObject * color_mask = PyTuple_New(4);
		PyTuple_SET_ITEM(color_mask, 0, PyBool_FromLong(self->color_mask[i * 4 + 0]));
		PyTuple_SET_ITEM(color_mask, 1, PyBool_FromLong(self->color_mask[i * 4 + 1]));
		PyTuple_SET_ITEM(color_mask, 2, PyBool_FromLong(self->color_mask[i * 4 + 2]));
		PyTuple_SET_ITEM(color_mask, 3, PyBool_FromLong(self->color_mask[i * 4 + 3]));
		PyTuple_SET_ITEM(res, i, color_mask);
	}

	return res;
}

int MGLFramebuffer_set_color_mask(MGLFramebuffer * self, PyObject * value, void * closure) {
	if (self->draw_buffers_len == 1) {
		if (Py_TYPE(value) != &PyTuple_Type || PyTuple_GET_SIZE(value) != 4) {
			MGLError_Set("the color_mask must be a 4-tuple not %s", Py_TYPE(value)->tp_name);
			return -1;
		}

		PyObject * r = PyTuple_GET_ITEM(value, 0);
		PyObject * g = PyTuple_GET_ITEM(value, 1);
		PyObject * b = PyTuple_GET_ITEM(value, 2);
		PyObject * a = PyTuple_GET_ITEM(value, 3);

		if (r == Py_True) {
			self->color_mask[0] = true;
		} else if (r == Py_False) {
			self->color_mask[0] = false;
		} else {
			MGLError_Set("the color_mask[0] must be a bool not %s", Py_TYPE(r)->tp_name);
			return -1;
		}

		if (g == Py_True) {
			self->color_mask[1] = true;
		} else if (g == Py_False) {
			self->color_mask[1] = false;
		} else {
			MGLError_Set("the color_mask[1] must be a bool not %s", Py_TYPE(g)->tp_name);
			return -1;
		}

		if (b == Py_True) {
			self->color_mask[2] = true;
		} else if (b == Py_False) {
			self->color_mask[2] = false;
		} else {
			MGLError_Set("the color_mask[2] must be a bool not %s", Py_TYPE(b)->tp_name);
			return -1;
		}

		if (a == Py_True) {
			self->color_mask[3] = true;
		} else if (a == Py_False) {
			self->color_mask[3] = false;
		} else {
			MGLError_Set("the color_mask[3] must be a bool not %s", Py_TYPE(a)->tp_name);
			return -1;
		}
	} else {
		for (int i = 0; i < self->draw_buffers_len; ++i) {
			PyObject * color_mask = PyTuple_GET_ITEM(value, i);

			if (Py_TYPE(color_mask) != &PyTuple_Type || PyTuple_GET_SIZE(color_mask) != 4) {
				MGLError_Set("the color_mask must be a 4-tuple not %s", Py_TYPE(color_mask)->tp_name);
				return -1;
			}

			PyObject * r = PyTuple_GET_ITEM(color_mask, 0);
			PyObject * g = PyTuple_GET_ITEM(color_mask, 1);
			PyObject * b = PyTuple_GET_ITEM(color_mask, 2);
			PyObject * a = PyTuple_GET_ITEM(color_mask, 3);

			if (r == Py_True) {
				self->color_mask[i * 4 + 0] = true;
			} else if (r == Py_False) {
				self->color_mask[i * 4 + 0] = false;
			} else {
				MGLError_Set("the color_mask[%d][0] must be a bool not %s", i, Py_TYPE(r)->tp_name);
				return -1;
			}

			if (g == Py_True) {
				self->color_mask[i * 4 + 1] = true;
			} else if (g == Py_False) {
				self->color_mask[i * 4 + 1] = false;
			} else {
				MGLError_Set("the color_mask[%d][1] must be a bool not %s", i, Py_TYPE(g)->tp_name);
				return -1;
			}

			if (b == Py_True) {
				self->color_mask[i * 4 + 2] = true;
			} else if (b == Py_False) {
				self->color_mask[i * 4 + 2] = false;
			} else {
				MGLError_Set("the color_mask[%d][2] must be a bool not %s", i, Py_TYPE(b)->tp_name);
				return -1;
			}

			if (a == Py_True) {
				self->color_mask[i * 4 + 3] = true;
			} else if (a == Py_False) {
				self->color_mask[i * 4 + 3] = false;
			} else {
				MGLError_Set("the color_mask[%d][3] must be a bool not %s", i, Py_TYPE(a)->tp_name);
				return -1;
			}
		}
	}

	if (self->framebuffer_obj == self->context->bound_framebuffer->framebuffer_obj) {
		const GLMethods & gl = self->context->gl;

		for (int i = 0; i < self->draw_buffers_len; ++i) {
			gl.ColorMaski(
				i,
				self->color_mask[i * 4 + 0],
				self->color_mask[i * 4 + 1],
				self->color_mask[i * 4 + 2],
				self->color_mask[i * 4 + 3]
			);
		}
	}

	return 0;
}

PyObject * MGLFramebuffer_get_depth_mask(MGLFramebuffer * self, void * closure) {
	return PyBool_FromLong(self->depth_mask);
}

int MGLFramebuffer_set_depth_mask(MGLFramebuffer * self, PyObject * value, void * closure) {
	if (value == Py_True) {
		self->depth_mask = true;
	} else if (value == Py_False) {
		self->depth_mask = false;
	} else {
		MGLError_Set("the depth_mask must be a bool not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	if (self->framebuffer_obj == self->context->bound_framebuffer->framebuffer_obj) {
		const GLMethods & gl = self->context->gl;
		gl.DepthMask(self->depth_mask);
	}

	return 0;
}

PyObject * MGLFramebuffer_get_bits(MGLFramebuffer * self, void * closure) {
	if (self->framebuffer_obj) {
		MGLError_Set("only the default_framebuffer have bits");
		return 0;
	}

	int red_bits = 0;
	int green_bits = 0;
	int blue_bits = 0;
	int alpha_bits = 0;
	int depth_bits = 0;
	int stencil_bits = 0;

	const GLMethods & gl = self->context->gl;

	gl.BindFramebuffer(GL_FRAMEBUFFER, self->framebuffer_obj);
	gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE, &red_bits);
	gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE, &green_bits);
	gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE, &blue_bits);
	gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE, &alpha_bits);
	gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH, GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depth_bits);
	gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencil_bits);
	gl.BindFramebuffer(GL_FRAMEBUFFER, self->context->bound_framebuffer->framebuffer_obj);

	PyObject * red_obj = PyLong_FromLong(red_bits);
	PyObject * green_obj = PyLong_FromLong(green_bits);
	PyObject * blue_obj = PyLong_FromLong(blue_bits);
	PyObject * alpha_obj = PyLong_FromLong(alpha_bits);
	PyObject * depth_obj = PyLong_FromLong(depth_bits);
	PyObject * stencil_obj = PyLong_FromLong(stencil_bits);

	PyObject * result = PyDict_New();

	PyDict_SetItemString(result, "red", red_obj);
	PyDict_SetItemString(result, "green", green_obj);
	PyDict_SetItemString(result, "blue", blue_obj);
	PyDict_SetItemString(result, "alpha", alpha_obj);
	PyDict_SetItemString(result, "depth", depth_obj);
	PyDict_SetItemString(result, "stencil", stencil_obj);

	Py_DECREF(red_obj);
	Py_DECREF(green_obj);
	Py_DECREF(blue_obj);
	Py_DECREF(alpha_obj);
	Py_DECREF(depth_obj);
	Py_DECREF(stencil_obj);

	return result;
}

PyGetSetDef MGLFramebuffer_tp_getseters[] = {
	{(char *)"viewport", (getter)MGLFramebuffer_get_viewport, (setter)MGLFramebuffer_set_viewport, 0, 0},
	{(char *)"color_mask", (getter)MGLFramebuffer_get_color_mask, (setter)MGLFramebuffer_set_color_mask, 0, 0},
	{(char *)"depth_mask", (getter)MGLFramebuffer_get_depth_mask, (setter)MGLFramebuffer_set_depth_mask, 0, 0},

	{(char *)"bits", (getter)MGLFramebuffer_get_bits, 0, 0, 0},
	{0},
};

PyTypeObject MGLFramebuffer_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.Framebuffer",                                      // tp_name
	sizeof(MGLFramebuffer),                                 // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLFramebuffer_tp_dealloc,                  // tp_dealloc
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
	MGLFramebuffer_tp_methods,                              // tp_methods
	0,                                                      // tp_members
	MGLFramebuffer_tp_getseters,                            // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	0,                                                      // tp_init
	0,                                                      // tp_alloc
	MGLFramebuffer_tp_new,                                  // tp_new
};

void MGLFramebuffer_Invalidate(MGLFramebuffer * framebuffer) {
	if (Py_TYPE(framebuffer) == &MGLInvalidObject_Type) {
		return;
	}

	// TODO: decref

	if (framebuffer->framebuffer_obj) {
		framebuffer->context->gl.DeleteFramebuffers(1, (GLuint *)&framebuffer->framebuffer_obj);
		Py_DECREF(framebuffer->context);
	}

	Py_TYPE(framebuffer) = &MGLInvalidObject_Type;
	Py_DECREF(framebuffer);
}
