#include "Context.hpp"

#include "Error.hpp"
#include "Buffer.hpp"
#include "ComputeShader.hpp"
#include "Texture.hpp"
#include "Texture3D.hpp"
#include "TextureCube.hpp"
#include "VertexArray.hpp"
#include "Program.hpp"
#include "Shader.hpp"
#include "Framebuffer.hpp"
#include "Renderbuffer.hpp"
#include "EnableFlag.hpp"
#include "InvalidObject.hpp"
#include "Attribute.hpp"
#include "Uniform.hpp"

#include "BufferFormat.hpp"

PyObject * MGLContext_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLContext * self = (MGLContext *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLContext_tp_dealloc(MGLContext * self) {
	MGLContext_Type.tp_free((PyObject *)self);
}

int MGLContext_tp_init(MGLContext * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.Context manually");
	return -1;
}

PyObject * MGLContext_clear(MGLContext * self, PyObject * args) {
	float r, g, b, a;
	PyObject * viewport;

	int args_ok = PyArg_ParseTuple(
		args,
		"ffffO",
		&r,
		&g,
		&b,
		&a,
		&viewport
	);

	if (!args_ok) {
		return 0;
	}

	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;

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
			MGLError_Set("invalid values in the viewport");
			return 0;
		}

	}

	const GLMethods & gl = self->gl;

	gl.ClearColor(r, g, b, a);

	if (viewport != Py_None) {
		gl.Enable(GL_SCISSOR_TEST);
		gl.Scissor(x, y, width, height);
		gl.Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		gl.Disable(GL_SCISSOR_TEST);
	} else {
		gl.Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	}

	Py_RETURN_NONE;
}

PyObject * MGLContext_enable_only(MGLContext * self, PyObject * args) {
	int flags;

	int args_ok = PyArg_ParseTuple(
		args,
		"i",
		&flags
	);

	if (!args_ok) {
		return 0;
	}

	if (flags & MGL_BLEND) {
		self->gl.Enable(GL_BLEND);
	} else {
		self->gl.Disable(GL_BLEND);
	}

	if (flags & MGL_DEPTH_TEST) {
		self->gl.Enable(GL_DEPTH_TEST);
	} else {
		self->gl.Disable(GL_DEPTH_TEST);
	}

	if (flags & MGL_CULL_FACE) {
		self->gl.Enable(GL_CULL_FACE);
	} else {
		self->gl.Disable(GL_CULL_FACE);
	}

	Py_RETURN_NONE;
}

PyObject * MGLContext_enable(MGLContext * self, PyObject * args) {
	int flags;

	int args_ok = PyArg_ParseTuple(
		args,
		"i",
		&flags
	);

	if (!args_ok) {
		return 0;
	}

	if (flags & MGL_BLEND) {
		self->gl.Enable(GL_BLEND);
	}

	if (flags & MGL_DEPTH_TEST) {
		self->gl.Enable(GL_DEPTH_TEST);
	}

	if (flags & MGL_CULL_FACE) {
		self->gl.Enable(GL_CULL_FACE);
	}

	Py_RETURN_NONE;
}

PyObject * MGLContext_disable(MGLContext * self, PyObject * args) {
	int flags;

	int args_ok = PyArg_ParseTuple(
		args,
		"i",
		&flags
	);

	if (!args_ok) {
		return 0;
	}

	if (flags & MGL_BLEND) {
		self->gl.Disable(GL_BLEND);
	}

	if (flags & MGL_DEPTH_TEST) {
		self->gl.Disable(GL_DEPTH_TEST);
	}

	if (flags & MGL_CULL_FACE) {
		self->gl.Disable(GL_CULL_FACE);
	}

	Py_RETURN_NONE;
}

PyObject * MGLContext_finish(MGLContext * self) {
	self->gl.Finish();
	Py_RETURN_NONE;
}

PyObject * MGLContext_copy_buffer(MGLContext * self, PyObject * args) {
	MGLBuffer * dst;
	MGLBuffer * src;

	Py_ssize_t size;
	Py_ssize_t read_offset;
	Py_ssize_t write_offset;

	int args_ok = PyArg_ParseTuple(
		args,
		"O!O!nnn",
		&MGLBuffer_Type,
		&dst,
		&MGLBuffer_Type,
		&src,
		&size,
		&read_offset,
		&write_offset
	);

	if (!args_ok) {
		return 0;
	}

	if (size < 0) {
		size = src->size - read_offset;
	}

	if (read_offset < 0 || write_offset < 0) {
		MGLError_Set("buffer underflow");
		return 0;
	}

	if (read_offset + size > src->size || write_offset + size > dst->size) {
		MGLError_Set("buffer overflow");
		return 0;
	}

	const GLMethods & gl = self->gl;

	gl.BindBuffer(GL_COPY_READ_BUFFER, src->buffer_obj);
	gl.BindBuffer(GL_COPY_WRITE_BUFFER, dst->buffer_obj);
	gl.CopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, read_offset, write_offset, size);

	Py_RETURN_NONE;
}

PyObject * MGLContext_copy_framebuffer(MGLContext * self, PyObject * args) {
	PyObject * dst;
	MGLFramebuffer * src;

	int args_ok = PyArg_ParseTuple(
		args,
		"OO!",
		&dst,
		&MGLFramebuffer_Type,
		&src
	);

	if (!args_ok) {
		return 0;
	}

	const GLMethods & gl = self->gl;

	// If the sizes of the source and destination rectangles are not equal,
	// filter specifies the interpolation method that will be applied to resize the source image,
	// and must be GL_NEAREST or GL_LINEAR. GL_LINEAR is only a valid interpolation
	// method for the color buffer. If filter is not GL_NEAREST and mask includes
	// GL_DEPTH_BUFFER_BIT or GL_STENCIL_BUFFER_BIT, no data is transferred and a
	// GL_INVALID_OPERATION error is generated.

	if (Py_TYPE(dst) == &MGLFramebuffer_Type) {

		MGLFramebuffer * dst_framebuffer = (MGLFramebuffer *)dst;

		int width = 0;
		int height = 0;

		if (!dst_framebuffer->framebuffer_obj) {
			width = src->width;
			height = src->height;
		} else if (!src->framebuffer_obj) {
			width = dst_framebuffer->width;
			height = dst_framebuffer->height;
		} else {
			width = src->width < dst_framebuffer->width ? src->width : dst_framebuffer->width;
			height = src->height < dst_framebuffer->height ? src->height : dst_framebuffer->height;
		}

		gl.BindFramebuffer(GL_READ_FRAMEBUFFER, src->framebuffer_obj);
		gl.BindFramebuffer(GL_DRAW_FRAMEBUFFER, dst_framebuffer->framebuffer_obj);
		gl.BlitFramebuffer(
			0, 0, width, height,
			0, 0, width, height,
			GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
			GL_NEAREST
		);
		gl.BindFramebuffer(GL_FRAMEBUFFER, self->bound_framebuffer->framebuffer_obj);

	} else if (Py_TYPE(dst) == &MGLTexture_Type) {

		MGLTexture * dst_texture = (MGLTexture *)dst;

		if (dst_texture->samples) {
			MGLError_Set("multisample texture targets are not accepted");
			return 0;
		}

		if (src->samples) {
			MGLError_Set("multisample framebuffer source with texture targets are not accepted");
			return 0;
		}

		int width = src->width < dst_texture->width ? src->width : dst_texture->width;
		int height = src->height < dst_texture->height ? src->height : dst_texture->height;

		if (!src->framebuffer_obj) {
			width = dst_texture->width;
			height = dst_texture->height;
		} else {
			width = src->width < dst_texture->width ? src->width : dst_texture->width;
			height = src->height < dst_texture->height ? src->height : dst_texture->height;
		}

		const int formats[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};
		int texture_target = dst_texture->samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		int format = formats[dst_texture->components];

		gl.BindFramebuffer(GL_READ_FRAMEBUFFER, src->framebuffer_obj);
		gl.CopyTexImage2D(texture_target, 0, format, 0, 0, width, height, 0);
		gl.BindFramebuffer(GL_FRAMEBUFFER, self->bound_framebuffer->framebuffer_obj);

	} else {

		MGLError_Set("the dst must be a Framebuffer or Texture");
		return 0;

	}

	Py_RETURN_NONE;
}

MGLFramebuffer * MGLContext_detect_framebuffer(MGLContext * self, PyObject * args) {
	PyObject * glo;

	int args_ok = PyArg_ParseTuple(
		args,
		"O",
		&glo
	);

	if (!args_ok) {
		return 0;
	}

	const GLMethods & gl = self->gl;

	int framebuffer_obj = 0;
	if (glo == Py_None) {
		gl.GetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &framebuffer_obj);
	} else {
		framebuffer_obj = PyLong_AsLong(glo);
		if (PyErr_Occurred()) {
			MGLError_Set("the glo must be an integer");
			return 0;
		}
	}

	if (!framebuffer_obj) {
		Py_INCREF(self->screen);
		return self->screen;
	}

	gl.BindFramebuffer(GL_FRAMEBUFFER, framebuffer_obj);

	int num_color_attachments = self->max_color_attachments;

	for (int i = 0; i < self->max_color_attachments; ++i) {
		int color_attachment_type = 0;
		gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &color_attachment_type);

		if (!color_attachment_type) {
			num_color_attachments = i;
			break;
		}
	}

	int color_attachment_type = 0;
	gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &color_attachment_type);

	int color_attachment_name = 0;
	gl.GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &color_attachment_name);

	int width = 0;
	int height = 0;

	switch (color_attachment_type) {
		case GL_RENDERBUFFER: {
			gl.BindRenderbuffer(GL_RENDERBUFFER, color_attachment_name);
			gl.GetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
			gl.GetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
			break;
		}
		case GL_TEXTURE: {
			gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);
			gl.BindTexture(GL_TEXTURE_2D, color_attachment_name);
			gl.GetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
			gl.GetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
			break;
		}
		default: {
			// TODO:
			break;
		}
	}

	MGLFramebuffer * framebuffer = (MGLFramebuffer *)MGLFramebuffer_Type.tp_alloc(&MGLFramebuffer_Type, 0);

	framebuffer->framebuffer_obj = framebuffer_obj;
	framebuffer->color_attachments = 0;
	framebuffer->depth_attachment = 0;

	framebuffer->draw_buffers_len = num_color_attachments;
	framebuffer->draw_buffers = new unsigned[num_color_attachments];
	framebuffer->color_mask = new bool[4 * num_color_attachments];

	for (int i = 0; i < num_color_attachments; ++i) {
		framebuffer->draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
		framebuffer->color_mask[i * 4 + 0] = true;
		framebuffer->color_mask[i * 4 + 1] = true;
		framebuffer->color_mask[i * 4 + 2] = true;
		framebuffer->color_mask[i * 4 + 3] = true;
	}

	framebuffer->depth_mask = true;

	framebuffer->context = self;

	framebuffer->viewport_x = 0;
	framebuffer->viewport_y = 0;
	framebuffer->viewport_width = width;
	framebuffer->viewport_height = height;

	framebuffer->width = width;
	framebuffer->height = height;

	gl.BindFramebuffer(GL_FRAMEBUFFER, self->bound_framebuffer->framebuffer_obj);

	return framebuffer;
}

MGLBuffer * MGLContext_buffer(MGLContext * self, PyObject * args) {
	PyObject * data;
	int reserve;
	int dynamic;

	int args_ok = PyArg_ParseTuple(
		args,
		"OIp",
		&data,
		&reserve,
		&dynamic
	);

	if (!args_ok) {
		return 0;
	}

	if ((data != Py_None && reserve) || (data == Py_None && !reserve)) {
		MGLError_Set("data and reserve are mutually exclusive");
		return 0;
	}

	Py_buffer buffer_view;

	if (data != Py_None) {
		int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
		if (get_buffer < 0) {
			MGLError_Set("data (%s) does not support buffer interface", Py_TYPE(data)->tp_name);
			return 0;
		}
	} else {
		buffer_view.len = reserve;
		buffer_view.buf = 0;
	}

	if (!buffer_view.len) {
		MGLError_Set("the buffer cannot be empty");
		if (data != Py_None) {
			PyBuffer_Release(&buffer_view);
		}
		return 0;
	}

	MGLBuffer * buffer = (MGLBuffer *)MGLBuffer_Type.tp_alloc(&MGLBuffer_Type, 0);

	buffer->size = (int)buffer_view.len;
	buffer->dynamic = dynamic ? true : false;

	const GLMethods & gl = self->gl;

	buffer->buffer_obj = 0;
	gl.GenBuffers(1, (GLuint *)&buffer->buffer_obj);

	if (!buffer->buffer_obj) {
		MGLError_Set("cannot create buffer");
		Py_DECREF(buffer);
		return 0;
	}

	gl.BindBuffer(GL_ARRAY_BUFFER, buffer->buffer_obj);
	gl.BufferData(GL_ARRAY_BUFFER, buffer->size, buffer_view.buf, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

	Py_INCREF(self);
	buffer->context = self;

	if (data != Py_None) {
		PyBuffer_Release(&buffer_view);
	}

	Py_INCREF(buffer);
	return buffer;
}

MGLTexture * MGLContext_texture(MGLContext * self, PyObject * args) {
	int width;
	int height;

	int components;

	PyObject * data;

	int samples;
	int alignment;
	int floats;

	int args_ok = PyArg_ParseTuple(
		args,
		"(II)IOIIp",
		&width,
		&height,
		&components,
		&data,
		&samples,
		&alignment,
		&floats
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

	int expected_size = width * components * (floats ? 4 : 1);
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * height;

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

	const int base_formats[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};
	const int int_formats[] = {0, GL_R8, GL_RG8, GL_RGB8, GL_RGBA8};
	const int float_formats[] = {0, GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F};

	int texture_target = samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	int pixel_type = floats ? GL_FLOAT : GL_UNSIGNED_BYTE;
	int base_format = base_formats[components];
	int internal_format = floats ? float_formats[components] : int_formats[components];

	const GLMethods & gl = self->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);

	MGLTexture * texture = (MGLTexture *)MGLTexture_Type.tp_alloc(&MGLTexture_Type, 0);

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
		gl.TexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		gl.TexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	if (data != Py_None) {
		PyBuffer_Release(&buffer_view);
	}

	texture->width = width;
	texture->height = height;
	texture->components = components;
	texture->samples = samples;
	texture->floats = floats ? true : false;
	texture->depth = false;

	Py_INCREF(MGL_LINEAR);
	texture->filter = MGL_LINEAR;

	texture->repeat_x = true;
	texture->repeat_y = true;

	Py_INCREF(self);
	texture->context = self;

	Py_INCREF(texture);
	return texture;
}

MGLTexture3D * MGLContext_texture3d(MGLContext * self, PyObject * args) {
	int width;
	int height;
	int depth;

	int components;

	PyObject * data;

	int alignment;
	int floats;

	int args_ok = PyArg_ParseTuple(
		args,
		"(III)IOIp",
		&width,
		&height,
		&depth,
		&components,
		&data,
		&alignment,
		&floats
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

	int expected_size = width * components * (floats ? 4 : 1);
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * height * depth;

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

	const int formats[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};

	int pixel_type = floats ? GL_FLOAT : GL_UNSIGNED_BYTE;
	int format = formats[components];

	const GLMethods & gl = self->gl;

	MGLTexture3D * texture = (MGLTexture3D *)MGLTexture3D_Type.tp_alloc(&MGLTexture3D_Type, 0);

	texture->texture_obj = 0;
	gl.GenTextures(1, (GLuint *)&texture->texture_obj);

	if (!texture->texture_obj) {
		MGLError_Set("cannot create texture");
		Py_DECREF(texture);
		return 0;
	}

	gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_3D, texture->texture_obj);

	gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
	gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
	gl.TexImage3D(GL_TEXTURE_3D, 0, format, width, height, depth, 0, format, pixel_type, buffer_view.buf);
	gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data != Py_None) {
		PyBuffer_Release(&buffer_view);
	}

	texture->width = width;
	texture->height = height;
	texture->depth = depth;
	texture->components = components;
	texture->floats = floats ? true : false;

	Py_INCREF(MGL_LINEAR);
	texture->filter = MGL_LINEAR;

	texture->repeat_x = true;
	texture->repeat_y = true;
	texture->repeat_z = true;

	Py_INCREF(self);
	texture->context = self;

	Py_INCREF(texture);
	return texture;
}

MGLTextureCube * MGLContext_texture_cube(MGLContext * self, PyObject * args) {
	int width;
	int height;

	int components;

	PyObject * data;

	int alignment;
	int floats;

	int args_ok = PyArg_ParseTuple(
		args,
		"(II)IOIp",
		&width,
		&height,
		&components,
		&data,
		&alignment,
		&floats
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

	int expected_size = width * components * (floats ? 4 : 1);
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

	const int formats[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};

	int pixel_type = floats ? GL_FLOAT : GL_UNSIGNED_BYTE;
	int format = formats[components];

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
	gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, width, height, 0, format, pixel_type, ptr[0]);
	gl.TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, width, height, 0, format, pixel_type, ptr[1]);
	gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, width, height, 0, format, pixel_type, ptr[2]);
	gl.TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, width, height, 0, format, pixel_type, ptr[3]);
	gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, width, height, 0, format, pixel_type, ptr[4]);
	gl.TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, width, height, 0, format, pixel_type, ptr[5]);
	gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data != Py_None) {
		PyBuffer_Release(&buffer_view);
	}

	texture->width = width;
	texture->height = height;
	texture->components = components;
	texture->floats = floats ? true : false;

	Py_INCREF(self);
	texture->context = self;

	Py_INCREF(texture);
	return texture;
}

MGLTexture * MGLContext_depth_texture(MGLContext * self, PyObject * args) {
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

	int texture_target = samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	int pixel_type = GL_FLOAT;

	const GLMethods & gl = self->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);

	MGLTexture * texture = (MGLTexture *)MGLTexture_Type.tp_alloc(&MGLTexture_Type, 0);

	texture->texture_obj = 0;
	gl.GenTextures(1, (GLuint *)&texture->texture_obj);

	if (!texture->texture_obj) {
		MGLError_Set("cannot create texture");
		Py_DECREF(texture);
		return 0;
	}

	gl.BindTexture(texture_target, texture->texture_obj);

	// TODO: check depth texture parametering they cause GL_INVALID_ENUM

	// gl.TexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// gl.TexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// gl.TexParameteri(texture_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// gl.TexParameteri(texture_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (samples) {
		gl.TexImage2DMultisample(texture_target, samples, GL_DEPTH_COMPONENT24, width, height, true);
	} else {
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.TexImage2D(texture_target, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, pixel_type, buffer_view.buf);
	}

	if (data != Py_None) {
		PyBuffer_Release(&buffer_view);
	}

	texture->width = width;
	texture->height = height;
	texture->components = 1;
	texture->samples = samples;
	texture->floats = true;
	texture->depth = true;

	Py_INCREF(MGL_LINEAR);
	texture->filter = MGL_LINEAR;

	texture->repeat_x = false;
	texture->repeat_y = false;

	Py_INCREF(self);
	texture->context = self;

	Py_INCREF(texture);
	return texture;
}

MGLVertexArray * MGLContext_vertex_array(MGLContext * self, PyObject * args) {
	MGLProgram * program;
	PyObject * content;
	MGLBuffer * index_buffer;

	int args_ok = PyArg_ParseTuple(
		args,
		"O!OO",
		&MGLProgram_Type,
		&program,
		&content,
		&index_buffer
	);

	if (!args_ok) {
		return 0;
	}

	if (program->context != self) {
		MGLError_Set("the program belongs to a different context");
		return 0;
	}

	if (index_buffer != (MGLBuffer *)Py_None && index_buffer->context != self) {
		MGLError_Set("the index_buffer belongs to a different context");
		return 0;
	}

	int content_len = (int)PyTuple_GET_SIZE(content);

	if (!content_len) {
		MGLError_Set("the content must not be emtpy");
		return 0;
	}

	for (int i = 0; i < content_len; ++i) {
		PyObject * tuple = PyTuple_GET_ITEM(content, i);
		PyObject * buffer = PyTuple_GET_ITEM(tuple, 0);
		PyObject * format = PyTuple_GET_ITEM(tuple, 1);
		PyObject * attributes = PyTuple_GET_ITEM(tuple, 2);

		if (Py_TYPE(buffer) != &MGLBuffer_Type) {
			MGLError_Set("content[%d][0] must be a Buffer not %s", i, Py_TYPE(buffer)->tp_name);
			return 0;
		}

		if (Py_TYPE(format) != &PyUnicode_Type) {
			MGLError_Set("content[%d][1] must be a string not %s", i, Py_TYPE(format)->tp_name);
			return 0;
		}

		if (((MGLBuffer *)buffer)->context != self) {
			MGLError_Set("content[%d][0] belongs to a different context", i);
			return 0;
		}

		FormatIterator it = FormatIterator(PyUnicode_AsUTF8(format));
		FormatInfo format_info = it.info();

		if (!format_info.valid) {
			MGLError_Set("content[%d][1] is an invalid format", i);
			return 0;
		}

		if (i == 0 && format_info.divisor) {
			MGLError_Set("the first vertex attribute must not be a per instance attribute");
			return 0;
		}

		int attributes_len = (int)PyTuple_GET_SIZE(attributes);

		if (!attributes_len) {
			MGLError_Set("content[%d][2] must not be empty", i);
			return 0;
		}

		if (attributes_len != format_info.nodes) {
			MGLError_Set("content[%d][1] and content[%d][2] size mismatch %d != %d", i, i, format_info.nodes, attributes_len);
			return 0;
		}

		for (int j = 0; j < attributes_len; ++j) {
			PyObject * attribute = PyTuple_GET_ITEM(attributes, j);

			if (Py_TYPE(attribute) != &PyUnicode_Type) {
				MGLError_Set("content[%d][2][%d] must be a string not %s", i, j, Py_TYPE(attribute)->tp_name);
				return 0;
			}

			{
				const char * name = PyUnicode_AsUTF8(attribute);

				FormatNode * node = it.next();

				while (node->shape == 'x') {
					node = it.next();
				}

				MGLAttribute * attribute = (MGLAttribute *)PyDict_GetItemString(program->attributes, name);

				if (!attribute) {
					MGLError_Set("%s is not a valid attribute", name);
					return 0;
				}

				int scalars = attribute->dimension * attribute->array_length;

				if (scalars != node->count) {
					MGLError_Set("%s size is %d not %d", name, scalars, node->count);
					return 0;
				}

				if (attribute->shape != node->shape) {
					MGLError_Set("%s shape is '%c' not '%c'", name, attribute->shape, node->shape);
					return 0;
				}
			}
		}
	}

	if (index_buffer != (MGLBuffer *)Py_None && Py_TYPE(index_buffer) != &MGLBuffer_Type) {
		MGLError_Set("the index_buffer must be a Buffer not %s", Py_TYPE(index_buffer)->tp_name);
		return 0;
	}

	const GLMethods & gl = self->gl;

	MGLVertexArray * array = (MGLVertexArray *)MGLVertexArray_Type.tp_alloc(&MGLVertexArray_Type, 0);

	Py_INCREF(program);
	array->program = program;

	array->vertex_array_obj = 0;
	gl.GenVertexArrays(1, (GLuint *)&array->vertex_array_obj);

	if (!array->vertex_array_obj) {
		MGLError_Set("cannot create vertex array");
		Py_DECREF(array);
		return 0;
	}

	gl.BindVertexArray(array->vertex_array_obj);
	gl.UseProgram(program->program_obj);

	Py_INCREF(index_buffer);
	array->index_buffer = index_buffer;

	if (index_buffer != (MGLBuffer *)Py_None) {
		array->num_vertices = (int)(index_buffer->size / 4);
		gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->buffer_obj);
	} else {
		array->num_vertices = -1;
	}

	for (int i = 0; i < content_len; ++i) {
		PyObject * tuple = PyTuple_GET_ITEM(content, i);

		MGLBuffer * buffer = (MGLBuffer *)PyTuple_GET_ITEM(tuple, 0);
		const char * format = PyUnicode_AsUTF8(PyTuple_GET_ITEM(tuple, 1));
		PyObject * attributes = PyTuple_GET_ITEM(tuple, 2);

		FormatIterator it = FormatIterator(format);
		FormatInfo format_info = it.info();

		int buf_vertices = (int)(buffer->size / format_info.size);

		if (!format_info.divisor && array->index_buffer == (MGLBuffer *)Py_None && (!i || array->num_vertices > buf_vertices)) {
			array->num_vertices = buf_vertices;
		}

		gl.BindBuffer(GL_ARRAY_BUFFER, buffer->buffer_obj);

		char * ptr = 0;

		int attributes_len = (int)PyTuple_GET_SIZE(attributes);

		for (int j = 0; j < attributes_len; ++j) {
			FormatNode * node = it.next();

			while (node->shape == 'x') {
				ptr += node->count * node->size;
				node = it.next();
			}

			MGLAttribute * attribute = (MGLAttribute *)PyDict_GetItem(program->attributes, PyTuple_GET_ITEM(attributes, j));

			for (int r = 0; r < attribute->rows_length; ++r) {
				int location = attribute->location + r;

				if (attribute->normalizable) {
					((gl_attribute_normal_ptr_proc)attribute->gl_attrib_ptr_proc)(location, attribute->row_length, attribute->scalar_type, false, format_info.size, ptr);
				} else {
					((gl_attribute_ptr_proc)attribute->gl_attrib_ptr_proc)(location, attribute->row_length, attribute->scalar_type, format_info.size, ptr);
				}

				gl.VertexAttribDivisor(location, format_info.divisor);

				gl.EnableVertexAttribArray(location);

				ptr += attribute->row_size;
			}
		}
	}

	Py_INCREF(self);
	array->context = self;

	MGLVertexArray_Complete(array);

	Py_INCREF(array);
	return array;
}

MGLProgram * MGLContext_program(MGLContext * self, PyObject * args) {
	PyObject * shaders;
	PyObject * varyings;

	int args_ok = PyArg_ParseTuple(
		args,
		"OO",
		&shaders,
		&varyings
	);

	if (!args_ok) {
		return 0;
	}

	int num_varyings = (int)PyTuple_GET_SIZE(varyings);

	for (int i = 0; i < num_varyings; ++i) {
		PyObject * item = PyTuple_GET_ITEM(varyings, i);
		if (Py_TYPE(item) != &PyUnicode_Type) {
			MGLError_Set("varyings[%d] must be a string not %s", i, Py_TYPE(item)->tp_name);
			return 0;
		}
	}

	int counter[NUM_SHADER_SLOTS] = {};

	int num_shaders = (int)PyTuple_GET_SIZE(shaders);

	for (int i = 0; i < num_shaders; ++i) {
		PyObject * item = PyTuple_GET_ITEM(shaders, i);
		if (Py_TYPE(item) != &MGLShader_Type) {
			MGLError_Set("shaders[%d] must be a Shader not %s", i, Py_TYPE(item)->tp_name);
			return 0;
		}

		MGLShader * shader = (MGLShader *)item;

		if (shader->context != self) {
			MGLError_Set("shaders[%d] belongs to a different context", i);
			return 0;
		}

		counter[shader->shader_slot] += 1;

		if (counter[shader->shader_slot] > 1) {
			MGLError_Set("duplicate %s", SHADER_NAME[shader->shader_slot]);
			return 0;
		}
	}

	MGLProgram * program = (MGLProgram *)MGLProgram_Type.tp_alloc(&MGLProgram_Type, 0);

	Py_INCREF(shaders);
	program->shaders = shaders;

	Py_INCREF(self);
	program->context = self;

	MGLProgram_Compile(program, varyings);

	if (PyErr_Occurred()) {
		Py_DECREF(program);
		return 0;
	}

	Py_INCREF(program);
	return program;
}

template <int ShaderSlot>
MGLShader * MGLContext_shader(MGLContext * self, PyObject * args) {
	PyObject * source;

	int args_ok = PyArg_ParseTuple(
		args,
		"O",
		&source
	);

	if (!args_ok) {
		return 0;
	}

	if (!PyUnicode_Check(source)) {
		MGLError_Set("the source must be a string not %s", Py_TYPE(source)->tp_name);
		return 0;
	}

	MGLShader * shader = (MGLShader *)MGLShader_Type.tp_alloc(&MGLShader_Type, 0);

	shader->shader_slot = ShaderSlot;
	shader->shader_type = SHADER_TYPE[ShaderSlot];

	Py_INCREF(source);
	shader->source = source;

	Py_INCREF(self);
	shader->context = self;

	MGLShader_Compile(shader);

	if (PyErr_Occurred()) {
		Py_DECREF(shader);
		return 0;
	}

	Py_INCREF(shader);
	return shader;
}

MGLShader * MGLContext_shader_vertex_shader(MGLContext * self, PyObject * args) {
	return MGLContext_shader<VERTEX_SHADER_SLOT>(self, args);
}

MGLShader * MGLContext_shader_fragment_shader(MGLContext * self, PyObject * args) {
	return MGLContext_shader<FRAGMENT_SHADER_SLOT>(self, args);
}

MGLShader * MGLContext_shader_geometry_shader(MGLContext * self, PyObject * args) {
	return MGLContext_shader<GEOMETRY_SHADER_SLOT>(self, args);
}

MGLShader * MGLContext_shader_tess_evaluation_shader(MGLContext * self, PyObject * args) {
	return MGLContext_shader<TESS_EVALUATION_SHADER_SLOT>(self, args);
}

MGLShader * MGLContext_shader_tess_control_shader(MGLContext * self, PyObject * args) {
	return MGLContext_shader<TESS_CONTROL_SHADER_SLOT>(self, args);
}

MGLFramebuffer * MGLContext_framebuffer(MGLContext * self, PyObject * args) {
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

	if (!color_attachments_len) {
		MGLError_Set("the color_attachments must not be empty");
		return 0;
	}

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

	Py_INCREF(color_attachments);
	Py_INCREF(depth_attachment);

	framebuffer->depth_mask = (depth_attachment != Py_None);

	framebuffer->color_attachments = color_attachments;
	framebuffer->depth_attachment = depth_attachment;

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
	return framebuffer;
}

MGLRenderbuffer * MGLContext_renderbuffer(MGLContext * self, PyObject * args) {
	int width;
	int height;

	int components;

	int samples;
	int floats;

	int args_ok = PyArg_ParseTuple(
		args,
		"(II)IIp",
		&width,
		&height,
		&components,
		&samples,
		&floats
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

	const int int_formats[] = {0, GL_R8, GL_RG8, GL_RGB8, GL_RGBA8};
	const int float_formats[] = {0, GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F};

	int format = floats ? float_formats[components] : int_formats[components];

	const GLMethods & gl = self->gl;

	MGLRenderbuffer * renderbuffer = (MGLRenderbuffer *)MGLRenderbuffer_Type.tp_alloc(&MGLRenderbuffer_Type, 0);

	renderbuffer->renderbuffer_obj = 0;
	gl.GenRenderbuffers(1, (GLuint *)&renderbuffer->renderbuffer_obj);

	if (!renderbuffer->renderbuffer_obj) {
		MGLError_Set("cannot create renderbuffer");
		Py_DECREF(renderbuffer);
		return 0;
	}

	gl.BindRenderbuffer(GL_RENDERBUFFER, renderbuffer->renderbuffer_obj);

	if (samples == 0) {
		gl.RenderbufferStorage(GL_RENDERBUFFER, format, width, height);
	} else {
		gl.RenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
	}

	renderbuffer->width = width;
	renderbuffer->height = height;
	renderbuffer->components = components;
	renderbuffer->samples = samples;
	renderbuffer->floats = floats ? true : false;
	renderbuffer->depth = false;

	Py_INCREF(self);
	renderbuffer->context = self;

	Py_INCREF(renderbuffer);
	return renderbuffer;
}

MGLRenderbuffer * MGLContext_depth_renderbuffer(MGLContext * self, PyObject * args) {
	int width;
	int height;

	int samples;

	int args_ok = PyArg_ParseTuple(
		args,
		"(II)I",
		&width,
		&height,
		&samples
	);

	if (!args_ok) {
		return 0;
	}

	if ((samples & (samples - 1)) || samples > self->max_samples) {
		MGLError_Set("the number of samples is invalid");
		return 0;
	}

	const GLMethods & gl = self->gl;

	MGLRenderbuffer * renderbuffer = (MGLRenderbuffer *)MGLRenderbuffer_Type.tp_alloc(&MGLRenderbuffer_Type, 0);

	renderbuffer->renderbuffer_obj = 0;
	gl.GenRenderbuffers(1, (GLuint *)&renderbuffer->renderbuffer_obj);

	if (!renderbuffer->renderbuffer_obj) {
		MGLError_Set("cannot create renderbuffer");
		Py_DECREF(renderbuffer);
		return 0;
	}

	gl.BindRenderbuffer(GL_RENDERBUFFER, renderbuffer->renderbuffer_obj);

	if (samples == 0) {
		gl.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	} else {
		gl.RenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT24, width, height);
	}

	renderbuffer->width = width;
	renderbuffer->height = height;
	renderbuffer->components = 1;
	renderbuffer->samples = samples;
	renderbuffer->floats = true;
	renderbuffer->depth = true;

	Py_INCREF(self);
	renderbuffer->context = self;

	Py_INCREF(renderbuffer);
	return renderbuffer;
}

MGLComputeShader * MGLContext_compute_shader(MGLContext * self, PyObject * args) {
	PyObject * source;

	int args_ok = PyArg_ParseTuple(
		args,
		"O",
		&source
	);

	if (!args_ok) {
		return 0;
	}

	if (!PyUnicode_Check(source)) {
		MGLError_Set("the source must be a string not %s", Py_TYPE(source)->tp_name);
		return 0;
	}

	MGLComputeShader * compute_shader = (MGLComputeShader *)MGLComputeShader_Type.tp_alloc(&MGLComputeShader_Type, 0);

	Py_INCREF(source);
	compute_shader->source = source;

	Py_INCREF(self);
	compute_shader->context = self;

	MGLComputeShader_Compile(compute_shader);

	if (PyErr_Occurred()) {
		Py_DECREF(compute_shader);
		return 0;
	}

	return compute_shader;
}

PyObject * MGLContext_release(MGLContext * self) {
	// TODO:
	// MGLContext_Invalidate(self);
	Py_RETURN_NONE;
}

// typedef MGLShader * (* MGLContext_shader_proc)(MGLContext *, PyObject *);

// MGLContext_shader_proc MGLContext_shader_vertex_shader = MGLContext_shader<VERTEX_SHADER_SLOT>;
// MGLContext_shader_proc MGLContext_shader_fragment_shader = MGLContext_shader<FRAGMENT_SHADER_SLOT>;
// MGLContext_shader_proc MGLContext_shader_geometry_shader = MGLContext_shader<GEOMETRY_SHADER_SLOT>;
// MGLContext_shader_proc MGLContext_shader_tess_evaluation_shader = MGLContext_shader<TESS_EVALUATION_SHADER_SLOT>;
// MGLContext_shader_proc MGLContext_shader_tess_control_shader = MGLContext_shader<TESS_CONTROL_SHADER_SLOT>;

PyMethodDef MGLContext_tp_methods[] = {
	{"clear", (PyCFunction)MGLContext_clear, METH_VARARGS, 0},
	{"enable_only", (PyCFunction)MGLContext_enable_only, METH_VARARGS, 0},
	{"enable", (PyCFunction)MGLContext_enable, METH_VARARGS, 0},
	{"disable", (PyCFunction)MGLContext_disable, METH_VARARGS, 0},
	{"finish", (PyCFunction)MGLContext_finish, METH_NOARGS, 0},
	{"copy_buffer", (PyCFunction)MGLContext_copy_buffer, METH_VARARGS, 0},
	{"copy_framebuffer", (PyCFunction)MGLContext_copy_framebuffer, METH_VARARGS, 0},
	{"detect_framebuffer", (PyCFunction)MGLContext_detect_framebuffer, METH_VARARGS, 0},

	{"buffer", (PyCFunction)MGLContext_buffer, METH_VARARGS, 0},
	{"texture", (PyCFunction)MGLContext_texture, METH_VARARGS, 0},
	{"texture3d", (PyCFunction)MGLContext_texture3d, METH_VARARGS, 0},
	{"texture_cube", (PyCFunction)MGLContext_texture_cube, METH_VARARGS, 0},
	{"depth_texture", (PyCFunction)MGLContext_depth_texture, METH_VARARGS, 0},
	{"vertex_array", (PyCFunction)MGLContext_vertex_array, METH_VARARGS, 0},
	{"program", (PyCFunction)MGLContext_program, METH_VARARGS, 0},
	{"vertex_shader", (PyCFunction)MGLContext_shader_vertex_shader, METH_VARARGS, 0},
	{"fragment_shader", (PyCFunction)MGLContext_shader_fragment_shader, METH_VARARGS, 0},
	{"geometry_shader", (PyCFunction)MGLContext_shader_geometry_shader, METH_VARARGS, 0},
	{"tess_evaluation_shader", (PyCFunction)MGLContext_shader_tess_evaluation_shader, METH_VARARGS, 0},
	{"tess_control_shader", (PyCFunction)MGLContext_shader_tess_control_shader, METH_VARARGS, 0},
	{"framebuffer", (PyCFunction)MGLContext_framebuffer, METH_VARARGS, 0},
	{"renderbuffer", (PyCFunction)MGLContext_renderbuffer, METH_VARARGS, 0},
	{"depth_renderbuffer", (PyCFunction)MGLContext_depth_renderbuffer, METH_VARARGS, 0},
	{"compute_shader", (PyCFunction)MGLContext_compute_shader, METH_VARARGS, 0},

	{"release", (PyCFunction)MGLContext_release, METH_NOARGS, 0},

	{0},
};

PyObject * MGLContext_get_line_width(MGLContext * self) {
	float line_width = 0.0f;

	self->gl.GetFloatv(GL_LINE_WIDTH, &line_width);

	return PyFloat_FromDouble(line_width);
}

int MGLContext_set_line_width(MGLContext * self, PyObject * value) {
	float line_width = (float)PyFloat_AsDouble(value);

	if (PyErr_Occurred()) {
		return -1;
	}

	self->gl.LineWidth(line_width);

	return 0;
}

PyObject * MGLContext_get_point_size(MGLContext * self) {
	float point_size = 0.0f;

	self->gl.GetFloatv(GL_POINT_SIZE, &point_size);

	return PyFloat_FromDouble(point_size);
}

int MGLContext_set_point_size(MGLContext * self, PyObject * value) {
	float point_size = (float)PyFloat_AsDouble(value);

	if (PyErr_Occurred()) {
		return -1;
	}

	self->gl.PointSize(point_size);

	return 0;
}

PyObject * MGLContext_get_viewport(MGLContext * self) {
	int viewport[4] = {};

	self->gl.GetIntegerv(GL_VIEWPORT, viewport);

	PyObject * x = PyLong_FromLong(viewport[0]);
	PyObject * y = PyLong_FromLong(viewport[1]);
	PyObject * width = PyLong_FromLong(viewport[2]);
	PyObject * height = PyLong_FromLong(viewport[3]);

	return PyTuple_Pack(4, x, y, width, height);
}

int MGLContext_set_viewport(MGLContext * self, PyObject * value) {
	int size = (int)PyTuple_Size(value);

	if (PyErr_Occurred()) {
		return -1;
	}

	if (size != 4) {
		MGLError_Set("the viewport must be a tuple of size 4 not %d", size);
		return -1;
	}

	int x = PyLong_AsLong(PyTuple_GET_ITEM(value, 0));
	int y = PyLong_AsLong(PyTuple_GET_ITEM(value, 1));
	int width = PyLong_AsLong(PyTuple_GET_ITEM(value, 2));
	int height = PyLong_AsLong(PyTuple_GET_ITEM(value, 3));

	if (PyErr_Occurred()) {
		MGLError_Set("invalid values in the viewport");
		return -1;
	}

	self->gl.Viewport(x, y, width, height);

	self->bound_framebuffer->viewport_x = x;
	self->bound_framebuffer->viewport_y = y;
	self->bound_framebuffer->viewport_width = width;
	self->bound_framebuffer->viewport_height = height;

	return 0;
}

PyObject * MGLContext_get_default_texture_unit(MGLContext * self) {
	return PyLong_FromLong(self->default_texture_unit);
}

int MGLContext_set_default_texture_unit(MGLContext * self, PyObject * value) {
	int default_texture_unit = PyLong_AsLong(value);

	if (PyErr_Occurred()) {
		return -1;
	}

	self->default_texture_unit = default_texture_unit;

	return 0;
}

PyObject * MGLContext_get_max_samples(MGLContext * self) {
	return PyLong_FromLong(self->max_samples);
}

PyObject * MGLContext_get_max_integer_samples(MGLContext * self) {
	return PyLong_FromLong(self->max_integer_samples);
}

PyObject * MGLContext_get_max_texture_units(MGLContext * self) {
	return PyLong_FromLong(self->max_texture_units);
}

MGLFramebuffer * MGLContext_get_screen(MGLContext * self) {
	Py_INCREF(self->screen);
	return self->screen;
}

PyObject * MGLContext_get_wireframe(MGLContext * self) {
	return PyBool_FromLong(self->wireframe);
}

int MGLContext_set_wireframe(MGLContext * self, PyObject * value) {
	if (value == Py_True) {
		self->gl.PolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		self->wireframe = true;
	} else if (value == Py_False) {
		self->gl.PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		self->wireframe = false;
	} else {
		MGLError_Set("invalid value for wireframe");
		return -1;
	}
	return 0;
}

PyObject * MGLContext_get_front_face(MGLContext * self) {
	if (self->front_face == GL_CW) {
		return PyUnicode_FromString("CW");
	}
	return PyUnicode_FromString("CCW");
}

int MGLContext_set_front_face(MGLContext * self, PyObject * value) {
	const char * str = PyUnicode_AsUTF8(value);

	if (!strcmp(str, "CW")) {
		self->front_face = GL_CW;
	} else if (!strcmp(str, "CCW")) {
		self->front_face = GL_CCW;
	} else {
		MGLError_Set("invalid value for front_face");
		return -1;
	}

	self->gl.FrontFace(self->front_face);
	return 0;
}

PyObject * MGLContext_get_error(MGLContext * self, void * closure) {
	switch (self->gl.GetError()) {
		case GL_NO_ERROR:
			return PyUnicode_FromFormat("GL_NO_ERROR");
		case GL_INVALID_ENUM:
			return PyUnicode_FromFormat("GL_INVALID_ENUM");
		case GL_INVALID_VALUE:
			return PyUnicode_FromFormat("GL_INVALID_VALUE");
		case GL_INVALID_OPERATION:
			return PyUnicode_FromFormat("GL_INVALID_OPERATION");
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return PyUnicode_FromFormat("GL_INVALID_FRAMEBUFFER_OPERATION");
		case GL_OUT_OF_MEMORY:
			return PyUnicode_FromFormat("GL_OUT_OF_MEMORY");
		case GL_STACK_UNDERFLOW:
			return PyUnicode_FromFormat("GL_STACK_UNDERFLOW");
		case GL_STACK_OVERFLOW:
			return PyUnicode_FromFormat("GL_STACK_OVERFLOW");
	}
	return PyUnicode_FromFormat("GL_UNKNOWN_ERROR");
}

PyObject * MGLContext_get_vendor(MGLContext * self, void * closure) {
	const char * vendor = (const char *)self->gl.GetString(GL_VENDOR);

	if (!vendor) {
		MGLError_Set("missing vendor information");
		return 0;
	}

	return PyUnicode_FromFormat("%s", vendor);
}

PyObject * MGLContext_get_renderer(MGLContext * self, void * closure) {
	const char * renderer = (const char *)self->gl.GetString(GL_RENDERER);

	if (!renderer) {
		MGLError_Set("missing renderer information");
		return 0;
	}

	return PyUnicode_FromFormat("%s", renderer);
}

PyObject * MGLContext_get_version(MGLContext * self, void * closure) {
	const char * version = (const char *)self->gl.GetString(GL_VERSION);

	if (!version) {
		MGLError_Set("missing version information");
		return 0;
	}

	return PyUnicode_FromFormat("%s", version);
}

PyObject * MGLContext_get_version_code(MGLContext * self, void * closure) {
	return PyLong_FromLong(self->version_code);
}

PyObject * MGLContext_get_info(MGLContext * self, void * closure) {
	PyObject * error_before = MGLContext_get_error(self, closure);

	const GLMethods & gl = self->gl;

	PyObject * info = PyDict_New();

	{
		float gl_point_size_range[2] = {};
		gl.GetFloatv(GL_POINT_SIZE_RANGE, gl_point_size_range);

		PyDict_SetItemString(
			info,
			"GL_POINT_SIZE_RANGE",
			PyTuple_Pack(
				2,
				PyFloat_FromDouble(gl_point_size_range[0]),
				PyFloat_FromDouble(gl_point_size_range[1])
			)
		);

		float gl_smooth_line_width_range[2] = {};
		gl.GetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, gl_smooth_line_width_range);

		PyDict_SetItemString(
			info,
			"GL_SMOOTH_LINE_WIDTH_RANGE",
			PyTuple_Pack(
				2,
				PyFloat_FromDouble(gl_smooth_line_width_range[0]),
				PyFloat_FromDouble(gl_smooth_line_width_range[1])
			)
		);

		float gl_aliased_line_width_range[2] = {};
		gl.GetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, gl_aliased_line_width_range);

		PyDict_SetItemString(
			info,
			"GL_ALIASED_LINE_WIDTH_RANGE",
			PyTuple_Pack(
				2,
				PyFloat_FromDouble(gl_aliased_line_width_range[0]),
				PyFloat_FromDouble(gl_aliased_line_width_range[1])
			)
		);

		float gl_point_fade_threshold_size = 0.0f;
		gl.GetFloatv(GL_POINT_FADE_THRESHOLD_SIZE, &gl_point_fade_threshold_size);

		float gl_point_size_granularity = 0.0f;
		gl.GetFloatv(GL_POINT_SIZE_GRANULARITY, &gl_point_size_granularity);

		float gl_smooth_line_width_granularity = 0.0f;
		gl.GetFloatv(GL_SMOOTH_LINE_WIDTH_GRANULARITY, &gl_smooth_line_width_granularity);

		float gl_min_program_texel_offset = 0.0f;
		gl.GetFloatv(GL_MIN_PROGRAM_TEXEL_OFFSET, &gl_min_program_texel_offset);

		float gl_max_program_texel_offset = 0.0f;
		gl.GetFloatv(GL_MAX_PROGRAM_TEXEL_OFFSET, &gl_max_program_texel_offset);

		PyDict_SetItemString(info, "GL_POINT_FADE_THRESHOLD_SIZE", PyFloat_FromDouble(gl_point_fade_threshold_size));
		PyDict_SetItemString(info, "GL_POINT_SIZE_GRANULARITY", PyFloat_FromDouble(gl_point_size_granularity));
		PyDict_SetItemString(info, "GL_SMOOTH_LINE_WIDTH_GRANULARITY", PyFloat_FromDouble(gl_smooth_line_width_granularity));
		PyDict_SetItemString(info, "GL_MIN_PROGRAM_TEXEL_OFFSET", PyFloat_FromDouble(gl_min_program_texel_offset));
		PyDict_SetItemString(info, "GL_MAX_PROGRAM_TEXEL_OFFSET", PyFloat_FromDouble(gl_max_program_texel_offset));
	}

	{
		int gl_minor_version = 0;
		gl.GetIntegerv(GL_MINOR_VERSION, &gl_minor_version);

		int gl_major_version = 0;
		gl.GetIntegerv(GL_MAJOR_VERSION, &gl_major_version);

		int gl_sample_buffers = 0;
		gl.GetIntegerv(GL_SAMPLE_BUFFERS, &gl_sample_buffers);

		int gl_subpixel_bits = 0;
		gl.GetIntegerv(GL_SUBPIXEL_BITS, &gl_subpixel_bits);

		PyDict_SetItemString(info, "GL_MINOR_VERSION", PyLong_FromLong(gl_minor_version));
		PyDict_SetItemString(info, "GL_MAJOR_VERSION", PyLong_FromLong(gl_major_version));
		PyDict_SetItemString(info, "GL_SAMPLE_BUFFERS", PyLong_FromLong(gl_sample_buffers));
		PyDict_SetItemString(info, "GL_SUBPIXEL_BITS", PyLong_FromLong(gl_subpixel_bits));
	}

	{
		unsigned char gl_doublebuffer = 0;
		gl.GetBooleanv(GL_DOUBLEBUFFER, &gl_doublebuffer);

		unsigned char gl_stereo = 0;
		gl.GetBooleanv(GL_STEREO, &gl_stereo);

		PyDict_SetItemString(info, "GL_DOUBLEBUFFER", PyBool_FromLong(gl_doublebuffer));
		PyDict_SetItemString(info, "GL_STEREO", PyBool_FromLong(gl_stereo));
	}

	// if (gl.GetInteger64v) {
	// 	long long gl_timestamp = 0;
	// 	gl.GetInteger64v(GL_TIMESTAMP, &gl_timestamp);

	// 	PyDict_SetItemString(info, "GL_TIMESTAMP", PyLong_FromLongLong(gl_timestamp));
	// }

	{
		int gl_max_viewport_dims[2] = {};
		gl.GetIntegerv(GL_MAX_VIEWPORT_DIMS, gl_max_viewport_dims);

		PyDict_SetItemString(
			info,
			"GL_MAX_VIEWPORT_DIMS",
			PyTuple_Pack(
				2,
				PyLong_FromLong(gl_max_viewport_dims[0]),
				PyLong_FromLong(gl_max_viewport_dims[1])
			)
		);

		int gl_max_3d_texture_size = 0;
		gl.GetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &gl_max_3d_texture_size);

		int gl_max_array_texture_layers = 0;
		gl.GetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &gl_max_array_texture_layers);

		int gl_max_clip_distances = 0;
		gl.GetIntegerv(GL_MAX_CLIP_DISTANCES, &gl_max_clip_distances);

		int gl_max_color_attachments = 0;
		gl.GetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &gl_max_color_attachments);

		int gl_max_color_texture_samples = 0;
		gl.GetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &gl_max_color_texture_samples);

		int gl_max_combined_fragment_uniform_components = 0;
		gl.GetIntegerv(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, &gl_max_combined_fragment_uniform_components);

		int gl_max_combined_geometry_uniform_components = 0;
		gl.GetIntegerv(GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS, &gl_max_combined_geometry_uniform_components);

		int gl_max_combined_texture_image_units = 0;
		gl.GetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &gl_max_combined_texture_image_units);

		int gl_max_combined_uniform_blocks = 0;
		gl.GetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &gl_max_combined_uniform_blocks);

		int gl_max_combined_vertex_uniform_components = 0;
		gl.GetIntegerv(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, &gl_max_combined_vertex_uniform_components);

		int gl_max_cube_map_texture_size = 0;
		gl.GetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &gl_max_cube_map_texture_size);

		int gl_max_depth_texture_samples = 0;
		gl.GetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &gl_max_depth_texture_samples);

		int gl_max_draw_buffers = 0;
		gl.GetIntegerv(GL_MAX_DRAW_BUFFERS, &gl_max_draw_buffers);

		int gl_max_dual_source_draw_buffers = 0;
		gl.GetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, &gl_max_dual_source_draw_buffers);

		int gl_max_elements_indices = 0;
		gl.GetIntegerv(GL_MAX_ELEMENTS_INDICES, &gl_max_elements_indices);

		int gl_max_elements_vertices = 0;
		gl.GetIntegerv(GL_MAX_ELEMENTS_VERTICES, &gl_max_elements_vertices);

		int gl_max_fragment_input_components = 0;
		gl.GetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &gl_max_fragment_input_components);

		int gl_max_fragment_uniform_components = 0;
		gl.GetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &gl_max_fragment_uniform_components);

		int gl_max_fragment_uniform_vectors = 0;
		gl.GetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &gl_max_fragment_uniform_vectors);

		int gl_max_fragment_uniform_blocks = 0;
		gl.GetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &gl_max_fragment_uniform_blocks);

		int gl_max_geometry_input_components = 0;
		gl.GetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS, &gl_max_geometry_input_components);

		int gl_max_geometry_output_components = 0;
		gl.GetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, &gl_max_geometry_output_components);

		int gl_max_geometry_texture_image_units = 0;
		gl.GetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &gl_max_geometry_texture_image_units);

		int gl_max_geometry_uniform_blocks = 0;
		gl.GetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &gl_max_geometry_uniform_blocks);

		int gl_max_geometry_uniform_components = 0;
		gl.GetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, &gl_max_geometry_uniform_components);

		int gl_max_integer_samples = 0;
		gl.GetIntegerv(GL_MAX_INTEGER_SAMPLES, &gl_max_integer_samples);

		int gl_max_samples = 0;
		gl.GetIntegerv(GL_MAX_SAMPLES, &gl_max_samples);

		int gl_max_rectangle_texture_size = 0;
		gl.GetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE, &gl_max_rectangle_texture_size);

		int gl_max_renderbuffer_size = 0;
		gl.GetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &gl_max_renderbuffer_size);

		int gl_max_sample_mask_words = 0;
		gl.GetIntegerv(GL_MAX_SAMPLE_MASK_WORDS, &gl_max_sample_mask_words);

		int gl_max_server_wait_timeout = 0;
		gl.GetIntegerv(GL_MAX_SERVER_WAIT_TIMEOUT, &gl_max_server_wait_timeout);

		int gl_max_texture_buffer_size = 0;
		gl.GetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &gl_max_texture_buffer_size);

		int gl_max_texture_image_units = 0;
		gl.GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &gl_max_texture_image_units);

		int gl_max_texture_lod_bias = 0;
		gl.GetIntegerv(GL_MAX_TEXTURE_LOD_BIAS, &gl_max_texture_lod_bias);

		int gl_max_texture_size = 0;
		gl.GetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_max_texture_size);

		int gl_max_uniform_buffer_bindings = 0;
		gl.GetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &gl_max_uniform_buffer_bindings);

		int gl_max_uniform_block_size = 0;
		gl.GetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &gl_max_uniform_block_size);

		int gl_max_varying_components = 0;
		gl.GetIntegerv(GL_MAX_VARYING_COMPONENTS, &gl_max_varying_components);

		int gl_max_varying_vectors = 0;
		gl.GetIntegerv(GL_MAX_VARYING_VECTORS, &gl_max_varying_vectors);

		int gl_max_varying_floats = 0;
		gl.GetIntegerv(GL_MAX_VARYING_FLOATS, &gl_max_varying_floats);

		int gl_max_vertex_attribs = 0;
		gl.GetIntegerv(GL_MAX_VERTEX_ATTRIBS, &gl_max_vertex_attribs);

		int gl_max_vertex_texture_image_units = 0;
		gl.GetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &gl_max_vertex_texture_image_units);

		int gl_max_vertex_uniform_components = 0;
		gl.GetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &gl_max_vertex_uniform_components);

		int gl_max_vertex_uniform_vectors = 0;
		gl.GetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &gl_max_vertex_uniform_vectors);

		int gl_max_vertex_output_components = 0;
		gl.GetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &gl_max_vertex_output_components);

		int gl_max_vertex_uniform_blocks = 0;
		gl.GetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &gl_max_vertex_uniform_blocks);

		int gl_max_vertex_attrib_relative_offset = 0;
		gl.GetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, &gl_max_vertex_attrib_relative_offset);

		int gl_max_vertex_attrib_bindings = 0;
		gl.GetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &gl_max_vertex_attrib_bindings);

		PyDict_SetItemString(info, "GL_MAX_3D_TEXTURE_SIZE", PyLong_FromLong(gl_max_3d_texture_size));
		PyDict_SetItemString(info, "GL_MAX_ARRAY_TEXTURE_LAYERS", PyLong_FromLong(gl_max_array_texture_layers));
		PyDict_SetItemString(info, "GL_MAX_CLIP_DISTANCES", PyLong_FromLong(gl_max_clip_distances));
		PyDict_SetItemString(info, "GL_MAX_COLOR_ATTACHMENTS", PyLong_FromLong(gl_max_color_attachments));
		PyDict_SetItemString(info, "GL_MAX_COLOR_TEXTURE_SAMPLES", PyLong_FromLong(gl_max_color_texture_samples));
		PyDict_SetItemString(info, "GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS", PyLong_FromLong(gl_max_combined_fragment_uniform_components));
		PyDict_SetItemString(info, "GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS", PyLong_FromLong(gl_max_combined_geometry_uniform_components));
		PyDict_SetItemString(info, "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", PyLong_FromLong(gl_max_combined_texture_image_units));
		PyDict_SetItemString(info, "GL_MAX_COMBINED_UNIFORM_BLOCKS", PyLong_FromLong(gl_max_combined_uniform_blocks));
		PyDict_SetItemString(info, "GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS", PyLong_FromLong(gl_max_combined_vertex_uniform_components));
		PyDict_SetItemString(info, "GL_MAX_CUBE_MAP_TEXTURE_SIZE", PyLong_FromLong(gl_max_cube_map_texture_size));
		PyDict_SetItemString(info, "GL_MAX_DEPTH_TEXTURE_SAMPLES", PyLong_FromLong(gl_max_depth_texture_samples));
		PyDict_SetItemString(info, "GL_MAX_DRAW_BUFFERS", PyLong_FromLong(gl_max_draw_buffers));
		PyDict_SetItemString(info, "GL_MAX_DUAL_SOURCE_DRAW_BUFFERS", PyLong_FromLong(gl_max_dual_source_draw_buffers));
		PyDict_SetItemString(info, "GL_MAX_ELEMENTS_INDICES", PyLong_FromLong(gl_max_elements_indices));
		PyDict_SetItemString(info, "GL_MAX_ELEMENTS_VERTICES", PyLong_FromLong(gl_max_elements_vertices));
		PyDict_SetItemString(info, "GL_MAX_FRAGMENT_INPUT_COMPONENTS", PyLong_FromLong(gl_max_fragment_input_components));
		PyDict_SetItemString(info, "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS", PyLong_FromLong(gl_max_fragment_uniform_components));
		PyDict_SetItemString(info, "GL_MAX_FRAGMENT_UNIFORM_VECTORS", PyLong_FromLong(gl_max_fragment_uniform_vectors));
		PyDict_SetItemString(info, "GL_MAX_FRAGMENT_UNIFORM_BLOCKS", PyLong_FromLong(gl_max_fragment_uniform_blocks));
		PyDict_SetItemString(info, "GL_MAX_GEOMETRY_INPUT_COMPONENTS", PyLong_FromLong(gl_max_geometry_input_components));
		PyDict_SetItemString(info, "GL_MAX_GEOMETRY_OUTPUT_COMPONENTS", PyLong_FromLong(gl_max_geometry_output_components));
		PyDict_SetItemString(info, "GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS", PyLong_FromLong(gl_max_geometry_texture_image_units));
		PyDict_SetItemString(info, "GL_MAX_GEOMETRY_UNIFORM_BLOCKS", PyLong_FromLong(gl_max_geometry_uniform_blocks));
		PyDict_SetItemString(info, "GL_MAX_GEOMETRY_UNIFORM_COMPONENTS", PyLong_FromLong(gl_max_geometry_uniform_components));
		PyDict_SetItemString(info, "GL_MAX_INTEGER_SAMPLES", PyLong_FromLong(gl_max_integer_samples));
		PyDict_SetItemString(info, "GL_MAX_SAMPLES", PyLong_FromLong(gl_max_samples));
		PyDict_SetItemString(info, "GL_MAX_RECTANGLE_TEXTURE_SIZE", PyLong_FromLong(gl_max_rectangle_texture_size));
		PyDict_SetItemString(info, "GL_MAX_RENDERBUFFER_SIZE", PyLong_FromLong(gl_max_renderbuffer_size));
		PyDict_SetItemString(info, "GL_MAX_SAMPLE_MASK_WORDS", PyLong_FromLong(gl_max_sample_mask_words));
		PyDict_SetItemString(info, "GL_MAX_SERVER_WAIT_TIMEOUT", PyLong_FromLong(gl_max_server_wait_timeout));
		PyDict_SetItemString(info, "GL_MAX_TEXTURE_BUFFER_SIZE", PyLong_FromLong(gl_max_texture_buffer_size));
		PyDict_SetItemString(info, "GL_MAX_TEXTURE_IMAGE_UNITS", PyLong_FromLong(gl_max_texture_image_units));
		PyDict_SetItemString(info, "GL_MAX_TEXTURE_LOD_BIAS", PyLong_FromLong(gl_max_texture_lod_bias));
		PyDict_SetItemString(info, "GL_MAX_TEXTURE_SIZE", PyLong_FromLong(gl_max_texture_size));
		PyDict_SetItemString(info, "GL_MAX_UNIFORM_BUFFER_BINDINGS", PyLong_FromLong(gl_max_uniform_buffer_bindings));
		PyDict_SetItemString(info, "GL_MAX_UNIFORM_BLOCK_SIZE", PyLong_FromLong(gl_max_uniform_block_size));
		PyDict_SetItemString(info, "GL_MAX_VARYING_COMPONENTS", PyLong_FromLong(gl_max_varying_components));
		PyDict_SetItemString(info, "GL_MAX_VARYING_VECTORS", PyLong_FromLong(gl_max_varying_vectors));
		PyDict_SetItemString(info, "GL_MAX_VARYING_FLOATS", PyLong_FromLong(gl_max_varying_floats));
		PyDict_SetItemString(info, "GL_MAX_VERTEX_ATTRIBS", PyLong_FromLong(gl_max_vertex_attribs));
		PyDict_SetItemString(info, "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS", PyLong_FromLong(gl_max_vertex_texture_image_units));
		PyDict_SetItemString(info, "GL_MAX_VERTEX_UNIFORM_COMPONENTS", PyLong_FromLong(gl_max_vertex_uniform_components));
		PyDict_SetItemString(info, "GL_MAX_VERTEX_UNIFORM_VECTORS", PyLong_FromLong(gl_max_vertex_uniform_vectors));
		PyDict_SetItemString(info, "GL_MAX_VERTEX_OUTPUT_COMPONENTS", PyLong_FromLong(gl_max_vertex_output_components));
		PyDict_SetItemString(info, "GL_MAX_VERTEX_UNIFORM_BLOCKS", PyLong_FromLong(gl_max_vertex_uniform_blocks));
		PyDict_SetItemString(info, "GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET", PyLong_FromLong(gl_max_vertex_attrib_relative_offset));
		PyDict_SetItemString(info, "GL_MAX_VERTEX_ATTRIB_BINDINGS", PyLong_FromLong(gl_max_vertex_attrib_bindings));
	}

	if (self->version_code >= 410) {
		int gl_viewport_bounds_range[2] = {};
		gl.GetIntegerv(GL_VIEWPORT_BOUNDS_RANGE, gl_viewport_bounds_range);

		PyDict_SetItemString(
			info,
			"GL_VIEWPORT_BOUNDS_RANGE",
			PyTuple_Pack(
				2,
				PyLong_FromLong(gl_viewport_bounds_range[0]),
				PyLong_FromLong(gl_viewport_bounds_range[1])
			)
		);

		int gl_viewport_subpixel_bits = 0;
		gl.GetIntegerv(GL_VIEWPORT_SUBPIXEL_BITS, &gl_viewport_subpixel_bits);

		int gl_max_viewports = 0;
		gl.GetIntegerv(GL_MAX_VIEWPORTS, &gl_max_viewports);

		PyDict_SetItemString(info, "GL_VIEWPORT_SUBPIXEL_BITS", PyLong_FromLong(gl_viewport_subpixel_bits));
		PyDict_SetItemString(info, "GL_MAX_VIEWPORTS", PyLong_FromLong(gl_max_viewports));
	}

	if (self->version_code >= 420) {
		int gl_min_map_buffer_alignment = 0;
		gl.GetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT, &gl_min_map_buffer_alignment);

		int gl_max_combined_atomic_counters = 0;
		gl.GetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTERS, &gl_max_combined_atomic_counters);

		int gl_max_fragment_atomic_counters = 0;
		gl.GetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTERS, &gl_max_fragment_atomic_counters);

		int gl_max_geometry_atomic_counters = 0;
		gl.GetIntegerv(GL_MAX_GEOMETRY_ATOMIC_COUNTERS, &gl_max_geometry_atomic_counters);

		int gl_max_tess_control_atomic_counters = 0;
		gl.GetIntegerv(GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS, &gl_max_tess_control_atomic_counters);

		int gl_max_tess_evaluation_atomic_counters = 0;
		gl.GetIntegerv(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS, &gl_max_tess_evaluation_atomic_counters);

		int gl_max_vertex_atomic_counters = 0;
		gl.GetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTERS, &gl_max_vertex_atomic_counters);

		PyDict_SetItemString(info, "GL_MIN_MAP_BUFFER_ALIGNMENT", PyLong_FromLong(gl_min_map_buffer_alignment));
		PyDict_SetItemString(info, "GL_MAX_COMBINED_ATOMIC_COUNTERS", PyLong_FromLong(gl_max_combined_atomic_counters));
		PyDict_SetItemString(info, "GL_MAX_FRAGMENT_ATOMIC_COUNTERS", PyLong_FromLong(gl_max_fragment_atomic_counters));
		PyDict_SetItemString(info, "GL_MAX_GEOMETRY_ATOMIC_COUNTERS", PyLong_FromLong(gl_max_geometry_atomic_counters));
		PyDict_SetItemString(info, "GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS", PyLong_FromLong(gl_max_tess_control_atomic_counters));
		PyDict_SetItemString(info, "GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS", PyLong_FromLong(gl_max_tess_evaluation_atomic_counters));
		PyDict_SetItemString(info, "GL_MAX_VERTEX_ATOMIC_COUNTERS", PyLong_FromLong(gl_max_vertex_atomic_counters));
	}

	if (self->version_code >= 430) {
		int gl_max_compute_work_group_count[3] = {};
		gl.GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &gl_max_compute_work_group_count[0]);
		gl.GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &gl_max_compute_work_group_count[1]);
		gl.GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &gl_max_compute_work_group_count[2]);

		int gl_max_compute_work_group_size[3] = {};
		gl.GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &gl_max_compute_work_group_size[0]);
		gl.GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &gl_max_compute_work_group_size[1]);
		gl.GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &gl_max_compute_work_group_size[2]);

		PyDict_SetItemString(
			info,
			"GL_MAX_COMPUTE_WORK_GROUP_COUNT",
			PyTuple_Pack(
				3,
				PyLong_FromLong(gl_max_compute_work_group_count[0]),
				PyLong_FromLong(gl_max_compute_work_group_count[0]),
				PyLong_FromLong(gl_max_compute_work_group_count[0])
			)
		);

		PyDict_SetItemString(
			info,
			"GL_MAX_COMPUTE_WORK_GROUP_SIZE",
			PyTuple_Pack(
				3,
				PyLong_FromLong(gl_max_compute_work_group_size[0]),
				PyLong_FromLong(gl_max_compute_work_group_size[1]),
				PyLong_FromLong(gl_max_compute_work_group_size[2])
			)
		);

		int gl_max_shader_storage_buffer_bindings = 0;
		gl.GetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &gl_max_shader_storage_buffer_bindings);

		int gl_max_combined_shader_storage_blocks = 0;
		gl.GetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &gl_max_combined_shader_storage_blocks);

		int gl_max_vertex_shader_storage_blocks = 0;
		gl.GetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &gl_max_vertex_shader_storage_blocks);

		int gl_max_fragment_shader_storage_blocks = 0;
		gl.GetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &gl_max_fragment_shader_storage_blocks);

		int gl_max_geometry_shader_storage_blocks = 0;
		gl.GetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, &gl_max_geometry_shader_storage_blocks);

		int gl_max_tess_evaluation_shader_storage_blocks = 0;
		gl.GetIntegerv(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, &gl_max_tess_evaluation_shader_storage_blocks);

		int gl_max_tess_control_shader_storage_blocks = 0;
		gl.GetIntegerv(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS, &gl_max_tess_control_shader_storage_blocks);

		int gl_max_compute_shader_storage_blocks = 0;
		gl.GetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &gl_max_compute_shader_storage_blocks);

		int gl_max_compute_uniform_components = 0;
		gl.GetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, &gl_max_compute_uniform_components);

		int gl_max_compute_atomic_counters = 0;
		gl.GetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTERS, &gl_max_compute_atomic_counters);

		int gl_max_compute_atomic_counter_buffers = 0;
		gl.GetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, &gl_max_compute_atomic_counter_buffers);

		int gl_max_compute_work_group_invocations = 0;
		gl.GetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &gl_max_compute_work_group_invocations);

		int gl_max_compute_uniform_blocks = 0;
		gl.GetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, &gl_max_compute_uniform_blocks);

		int gl_max_compute_texture_image_units = 0;
		gl.GetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &gl_max_compute_texture_image_units);

		int gl_max_combined_compute_uniform_components = 0;
		gl.GetIntegerv(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, &gl_max_combined_compute_uniform_components);

		int gl_max_framebuffer_width = 0;
		gl.GetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &gl_max_framebuffer_width);

		int gl_max_framebuffer_height = 0;
		gl.GetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &gl_max_framebuffer_height);

		int gl_max_framebuffer_layers = 0;
		gl.GetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &gl_max_framebuffer_layers);

		int gl_max_framebuffer_samples = 0;
		gl.GetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &gl_max_framebuffer_samples);

		int gl_max_uniform_locations = 0;
		gl.GetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &gl_max_uniform_locations);

		int gl_max_element_index = 0;
		gl.GetIntegerv(GL_MAX_ELEMENT_INDEX, &gl_max_element_index);

		PyDict_SetItemString(info, "GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS", PyLong_FromLong(gl_max_shader_storage_buffer_bindings));
		PyDict_SetItemString(info, "GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS", PyLong_FromLong(gl_max_combined_shader_storage_blocks));
		PyDict_SetItemString(info, "GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS", PyLong_FromLong(gl_max_vertex_shader_storage_blocks));
		PyDict_SetItemString(info, "GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS", PyLong_FromLong(gl_max_fragment_shader_storage_blocks));
		PyDict_SetItemString(info, "GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS", PyLong_FromLong(gl_max_geometry_shader_storage_blocks));
		PyDict_SetItemString(info, "GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS", PyLong_FromLong(gl_max_tess_evaluation_shader_storage_blocks));
		PyDict_SetItemString(info, "GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS", PyLong_FromLong(gl_max_tess_control_shader_storage_blocks));
		PyDict_SetItemString(info, "GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS", PyLong_FromLong(gl_max_compute_shader_storage_blocks));
		PyDict_SetItemString(info, "GL_MAX_COMPUTE_UNIFORM_COMPONENTS", PyLong_FromLong(gl_max_compute_uniform_components));
		PyDict_SetItemString(info, "GL_MAX_COMPUTE_ATOMIC_COUNTERS", PyLong_FromLong(gl_max_compute_atomic_counters));
		PyDict_SetItemString(info, "GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS", PyLong_FromLong(gl_max_compute_atomic_counter_buffers));
		PyDict_SetItemString(info, "GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS", PyLong_FromLong(gl_max_compute_work_group_invocations));
		PyDict_SetItemString(info, "GL_MAX_COMPUTE_UNIFORM_BLOCKS", PyLong_FromLong(gl_max_compute_uniform_blocks));
		PyDict_SetItemString(info, "GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS", PyLong_FromLong(gl_max_compute_texture_image_units));
		PyDict_SetItemString(info, "GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS", PyLong_FromLong(gl_max_combined_compute_uniform_components));
		PyDict_SetItemString(info, "GL_MAX_FRAMEBUFFER_WIDTH", PyLong_FromLong(gl_max_framebuffer_width));
		PyDict_SetItemString(info, "GL_MAX_FRAMEBUFFER_HEIGHT", PyLong_FromLong(gl_max_framebuffer_height));
		PyDict_SetItemString(info, "GL_MAX_FRAMEBUFFER_LAYERS", PyLong_FromLong(gl_max_framebuffer_layers));
		PyDict_SetItemString(info, "GL_MAX_FRAMEBUFFER_SAMPLES", PyLong_FromLong(gl_max_framebuffer_samples));
		PyDict_SetItemString(info, "GL_MAX_UNIFORM_LOCATIONS", PyLong_FromLong(gl_max_uniform_locations));
		PyDict_SetItemString(info, "GL_MAX_ELEMENT_INDEX", PyLong_FromLong(gl_max_element_index));
	}

	PyObject * error_after = MGLContext_get_error(self, closure);

	PyDict_SetItemString(info, "GL_ERROR_BEFORE_INFO", error_before);
	PyDict_SetItemString(info, "GL_ERROR_AFTER_INFO", error_after);

	Py_DECREF(error_before);
	Py_DECREF(error_after);

	return info;
}

PyGetSetDef MGLContext_tp_getseters[] = {
	{(char *)"line_width", (getter)MGLContext_get_line_width, (setter)MGLContext_set_line_width, 0, 0},
	{(char *)"point_size", (getter)MGLContext_get_point_size, (setter)MGLContext_set_point_size, 0, 0},
	{(char *)"viewport", (getter)MGLContext_get_viewport, (setter)MGLContext_set_viewport, 0, 0},

	{(char *)"max_samples", (getter)MGLContext_get_max_samples, 0, 0, 0},
	{(char *)"max_integer_samples", (getter)MGLContext_get_max_integer_samples, 0, 0, 0},
	{(char *)"max_texture_units", (getter)MGLContext_get_max_texture_units, 0, 0, 0},
	{(char *)"default_texture_unit", (getter)MGLContext_get_default_texture_unit, (setter)MGLContext_set_default_texture_unit, 0, 0},
	{(char *)"screen", (getter)MGLContext_get_screen, 0, 0, 0},

	{(char *)"wireframe", (getter)MGLContext_get_wireframe, (setter)MGLContext_set_wireframe, 0, 0},
	{(char *)"front_face", (getter)MGLContext_get_front_face, (setter)MGLContext_set_front_face, 0, 0},

	{(char *)"error", (getter)MGLContext_get_error, 0, 0, 0},
	{(char *)"vendor", (getter)MGLContext_get_vendor, 0, 0, 0},
	{(char *)"renderer", (getter)MGLContext_get_renderer, 0, 0, 0},
	{(char *)"version", (getter)MGLContext_get_version, 0, 0, 0},
	{(char *)"version_code", (getter)MGLContext_get_version_code, 0, 0, 0},
	{(char *)"info", (getter)MGLContext_get_info, 0, 0, 0},
	{0},
};

PyTypeObject MGLContext_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.Context",                                          // tp_name
	sizeof(MGLContext),                                     // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLContext_tp_dealloc,                      // tp_dealloc
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
	MGLContext_tp_methods,                                  // tp_methods
	0,                                                      // tp_members
	MGLContext_tp_getseters,                                // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLContext_tp_init,                           // tp_init
	0,                                                      // tp_alloc
	MGLContext_tp_new,                                      // tp_new
};

void MGLContext_Invalidate(MGLContext * context) {
	if (Py_TYPE(context) == &MGLInvalidObject_Type) {
		return;
	}

	DestroyGLContext(context->gl_context);
	Py_TYPE(context) = &MGLInvalidObject_Type;
	Py_DECREF(context);
}

void MGLContext_Initialize(MGLContext * self) {

	GLMethods & gl = self->gl;

	if (!gl.load()) {
		return;
	}

	int major = 0;
	int minor = 0;

	gl.GetIntegerv(GL_MAJOR_VERSION, &major);
	gl.GetIntegerv(GL_MINOR_VERSION, &minor);

	self->version_code = major * 100 + minor * 10;

	gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gl.Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	gl.Enable(GL_PRIMITIVE_RESTART);
	gl.PrimitiveRestartIndex(-1);

	self->max_samples = 0;
	gl.GetIntegerv(GL_MAX_SAMPLES, (GLint *)&self->max_samples);

	self->max_integer_samples = 0;
	gl.GetIntegerv(GL_MAX_INTEGER_SAMPLES, (GLint *)&self->max_integer_samples);

	self->max_color_attachments = 0;
	gl.GetIntegerv(GL_MAX_COLOR_ATTACHMENTS, (GLint *)&self->max_color_attachments);

	self->max_texture_units = 0;
	gl.GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint *)&self->max_texture_units);
	self->default_texture_unit = self->max_texture_units - 1;

	int bound_framebuffer = 0;
	gl.GetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &bound_framebuffer);

	{
		MGLFramebuffer * framebuffer = (MGLFramebuffer *)MGLFramebuffer_Type.tp_alloc(&MGLFramebuffer_Type, 0);

		framebuffer->framebuffer_obj = 0;

		framebuffer->color_attachments = 0;
		framebuffer->depth_attachment = 0;

		framebuffer->draw_buffers_len = 1;
		framebuffer->draw_buffers = new unsigned[1];

		// According to glGet docs:
		// The initial value is GL_BACK if there are back buffers, otherwise it is GL_FRONT.

		// According to glDrawBuffer docs:
		// The symbolic constants GL_FRONT, GL_BACK, GL_LEFT, GL_RIGHT, and GL_FRONT_AND_BACK
		// are not allowed in the bufs array since they may refer to multiple buffers.

		// GL_COLOR_ATTACHMENT0 is causes error: 1282
		// This value is temporarily ignored

		// framebuffer->draw_buffers[0] = GL_COLOR_ATTACHMENT0;
		// framebuffer->draw_buffers[0] = GL_BACK_LEFT;

		gl.GetIntegerv(GL_DRAW_BUFFER, (int *)&framebuffer->draw_buffers[0]);

		framebuffer->color_mask = new bool[4];
		framebuffer->color_mask[0] = true;
		framebuffer->color_mask[1] = true;
		framebuffer->color_mask[2] = true;
		framebuffer->color_mask[3] = true;

		framebuffer->depth_mask = true;

		framebuffer->context = self;

		int scrissor_box[4] = {};
		gl.GetIntegerv(GL_SCISSOR_BOX, scrissor_box);

		framebuffer->viewport_x = scrissor_box[0];
		framebuffer->viewport_y = scrissor_box[1];
		framebuffer->viewport_width = scrissor_box[2];
		framebuffer->viewport_height = scrissor_box[3];

		framebuffer->width = scrissor_box[2];
		framebuffer->height = scrissor_box[3];

		self->screen = framebuffer;
	}

	if (bound_framebuffer) {
		PyObject * args = PyTuple_New(1);
		PyTuple_SET_ITEM(args, 0, PyLong_FromLong(bound_framebuffer));
		MGLFramebuffer * framebuffer = MGLContext_detect_framebuffer(self, args);
		self->bound_framebuffer = framebuffer;
	} else {
		Py_INCREF(self->screen);
		self->bound_framebuffer = self->screen;
	}

	self->wireframe = false;
	self->front_face = GL_CCW;

	// TODO: multisample getter setter (bool)
}
