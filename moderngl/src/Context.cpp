#include "Types.hpp"

#include "BufferFormat.hpp"
#include "InlineMethods.hpp"

PyObject * MGLContext_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLContext * self = (MGLContext *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLContext_tp_dealloc(MGLContext * self) {
	MGLContext_Type.tp_free((PyObject *)self);
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

	self->enable_flags = flags;

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

	if (flags & MGL_RASTERIZER_DISCARD) {
		self->gl.Enable(GL_RASTERIZER_DISCARD);
	} else {
		self->gl.Disable(GL_RASTERIZER_DISCARD);
	}

	if (flags & MGL_PROGRAM_POINT_SIZE) {
		self->gl.Enable(GL_PROGRAM_POINT_SIZE);
	} else {
		self->gl.Disable(GL_PROGRAM_POINT_SIZE);
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

	self->enable_flags |= flags;

	if (flags & MGL_BLEND) {
		self->gl.Enable(GL_BLEND);
	}

	if (flags & MGL_DEPTH_TEST) {
		self->gl.Enable(GL_DEPTH_TEST);
	}

	if (flags & MGL_CULL_FACE) {
		self->gl.Enable(GL_CULL_FACE);
	}

	if (flags & MGL_RASTERIZER_DISCARD) {
		self->gl.Enable(GL_RASTERIZER_DISCARD);
	}

	if (flags & MGL_PROGRAM_POINT_SIZE) {
		self->gl.Enable(GL_PROGRAM_POINT_SIZE);
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

	self->enable_flags &= ~flags;

	if (flags & MGL_BLEND) {
		self->gl.Disable(GL_BLEND);
	}

	if (flags & MGL_DEPTH_TEST) {
		self->gl.Disable(GL_DEPTH_TEST);
	}

	if (flags & MGL_CULL_FACE) {
		self->gl.Disable(GL_CULL_FACE);
	}

	if (flags & MGL_RASTERIZER_DISCARD) {
		self->gl.Disable(GL_RASTERIZER_DISCARD);
	}

	if (flags & MGL_PROGRAM_POINT_SIZE) {
		self->gl.Disable(GL_PROGRAM_POINT_SIZE);
	}

	Py_RETURN_NONE;
}

PyObject * MGLContext_enable_direct(MGLContext * self, PyObject * args) {
	int value;

	int args_ok = PyArg_ParseTuple(
		args,
		"i",
		&value
	);

	if (!args_ok) {
		return 0;
	}

	self->gl.Enable(value);
	Py_RETURN_NONE;
}

PyObject * MGLContext_disable_direct(MGLContext * self, PyObject * args) {
	int value;

	int args_ok = PyArg_ParseTuple(
		args,
		"i",
		&value
	);

	if (!args_ok) {
		return 0;
	}

	self->gl.Disable(value);
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
		gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);
		gl.BindTexture(GL_TEXTURE_2D, dst_texture->texture_obj);
		gl.CopyTexImage2D(texture_target, 0, format, 0, 0, width, height, 0);
		gl.BindFramebuffer(GL_FRAMEBUFFER, self->bound_framebuffer->framebuffer_obj);

	} else {

		MGLError_Set("the dst must be a Framebuffer or Texture");
		return 0;

	}

	Py_RETURN_NONE;
}

PyObject * MGLContext_detect_framebuffer(MGLContext * self, PyObject * args) {
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

	int bound_framebuffer = 0;
	gl.GetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &bound_framebuffer);

	int framebuffer_obj = bound_framebuffer;
	if (glo != Py_None) {
		framebuffer_obj = PyLong_AsLong(glo);
		if (PyErr_Occurred()) {
			MGLError_Set("the glo must be an integer");
			return 0;
		}
	}

	if (!framebuffer_obj) {
		PyObject * size = PyTuple_New(2);
		PyTuple_SET_ITEM(size, 0, PyLong_FromLong(self->default_framebuffer->width));
		PyTuple_SET_ITEM(size, 1, PyLong_FromLong(self->default_framebuffer->height));

		Py_INCREF(self->default_framebuffer);
		PyObject * result = PyTuple_New(4);
		PyTuple_SET_ITEM(result, 0, (PyObject *)self->default_framebuffer);
		PyTuple_SET_ITEM(result, 1, size);
		PyTuple_SET_ITEM(result, 2, PyLong_FromLong(self->default_framebuffer->samples));
		PyTuple_SET_ITEM(result, 3, PyLong_FromLong(self->default_framebuffer->framebuffer_obj));
		return result;
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
	}

	MGLFramebuffer * framebuffer = (MGLFramebuffer *)MGLFramebuffer_Type.tp_alloc(&MGLFramebuffer_Type, 0);

	framebuffer->framebuffer_obj = framebuffer_obj;

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

	framebuffer->scissor_enabled = false;
	framebuffer->scissor_x = 0;
	framebuffer->scissor_y = 0;
	framebuffer->scissor_width = width;
	framebuffer->scissor_height = height;

	framebuffer->width = width;
	framebuffer->height = height;
	framebuffer->dynamic = true;

	gl.BindFramebuffer(GL_FRAMEBUFFER, bound_framebuffer);

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

PyObject * MGLContext_clear_samplers(MGLContext * self, PyObject * args) {
	int start;
	int end;

	int args_ok = PyArg_ParseTuple(
		args,
		"ii",
		&start,
		&end
	);

	if (!args_ok) {
		return 0;
	}

	start = max(start, 0);
	if (end == -1) {
		end = self->max_texture_units;
	} else {
		end = min(end, self->max_texture_units);
	}

	const GLMethods & gl = self->gl;

	for(int i = start; i < end; i++) {
		gl.BindSampler(i, 0);
	}

	Py_RETURN_NONE;
}

PyObject * MGLContext_buffer(MGLContext * self, PyObject * args);
PyObject * MGLContext_texture(MGLContext * self, PyObject * args);
PyObject * MGLContext_texture3d(MGLContext * self, PyObject * args);
PyObject * MGLContext_texture_array(MGLContext * self, PyObject * args);
PyObject * MGLContext_texture_cube(MGLContext * self, PyObject * args);
PyObject * MGLContext_depth_texture(MGLContext * self, PyObject * args);
PyObject * MGLContext_external_texture(MGLContext * self, PyObject * args);
PyObject * MGLContext_vertex_array(MGLContext * self, PyObject * args);
PyObject * MGLContext_program(MGLContext * self, PyObject * args);
PyObject * MGLContext_framebuffer(MGLContext * self, PyObject * args);
PyObject * MGLContext_renderbuffer(MGLContext * self, PyObject * args);
PyObject * MGLContext_depth_renderbuffer(MGLContext * self, PyObject * args);
PyObject * MGLContext_compute_shader(MGLContext * self, PyObject * args);
PyObject * MGLContext_query(MGLContext * self, PyObject * args);
PyObject * MGLContext_scope(MGLContext * self, PyObject * args);
PyObject * MGLContext_sampler(MGLContext * self, PyObject * args);

PyObject * MGLContext_enter(MGLContext * self) {
	PyObject_CallMethod(self->ctx, "__enter__", NULL);
	Py_RETURN_NONE;
}

PyObject * MGLContext_exit(MGLContext * self) {
	PyObject_CallMethod(self->ctx, "__exit__", NULL);
	Py_RETURN_NONE;
}

PyObject * MGLContext_release(MGLContext * self) {
	MGLContext_Invalidate(self);
	Py_RETURN_NONE;
}

PyMethodDef MGLContext_tp_methods[] = {
	{"enable_only", (PyCFunction)MGLContext_enable_only, METH_VARARGS, 0},
	{"enable", (PyCFunction)MGLContext_enable, METH_VARARGS, 0},
	{"disable", (PyCFunction)MGLContext_disable, METH_VARARGS, 0},
	{"enable_direct", (PyCFunction)MGLContext_enable_direct, METH_VARARGS, 0},
	{"disable_direct", (PyCFunction)MGLContext_disable_direct, METH_VARARGS, 0},
	{"finish", (PyCFunction)MGLContext_finish, METH_NOARGS, 0},
	{"copy_buffer", (PyCFunction)MGLContext_copy_buffer, METH_VARARGS, 0},
	{"copy_framebuffer", (PyCFunction)MGLContext_copy_framebuffer, METH_VARARGS, 0},
	{"detect_framebuffer", (PyCFunction)MGLContext_detect_framebuffer, METH_VARARGS, 0},
	{"clear_samplers", (PyCFunction)MGLContext_clear_samplers, METH_VARARGS, 0},

	{"buffer", (PyCFunction)MGLContext_buffer, METH_VARARGS, 0},
	{"texture", (PyCFunction)MGLContext_texture, METH_VARARGS, 0},
	{"texture3d", (PyCFunction)MGLContext_texture3d, METH_VARARGS, 0},
	{"texture_array", (PyCFunction)MGLContext_texture_array, METH_VARARGS, 0},
	{"texture_cube", (PyCFunction)MGLContext_texture_cube, METH_VARARGS, 0},
	{"depth_texture", (PyCFunction)MGLContext_depth_texture, METH_VARARGS, 0},
	{"external_texture", (PyCFunction)MGLContext_external_texture, METH_VARARGS, 0},
	{"vertex_array", (PyCFunction)MGLContext_vertex_array, METH_VARARGS, 0},
	{"program", (PyCFunction)MGLContext_program, METH_VARARGS, 0},
	// {"shader", (PyCFunction)MGLContext_shader, METH_VARARGS, 0},
	{"framebuffer", (PyCFunction)MGLContext_framebuffer, METH_VARARGS, 0},
	{"renderbuffer", (PyCFunction)MGLContext_renderbuffer, METH_VARARGS, 0},
	{"depth_renderbuffer", (PyCFunction)MGLContext_depth_renderbuffer, METH_VARARGS, 0},
	{"compute_shader", (PyCFunction)MGLContext_compute_shader, METH_VARARGS, 0},
	{"query", (PyCFunction)MGLContext_query, METH_VARARGS, 0},
	{"scope", (PyCFunction)MGLContext_scope, METH_VARARGS, 0},
	{"sampler", (PyCFunction)MGLContext_sampler, METH_VARARGS, 0},

	{"__enter__", (PyCFunction)MGLContext_enter, METH_NOARGS, 0},
	{"__exit__", (PyCFunction)MGLContext_exit, METH_VARARGS, 0},
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

// NOTE: currently never called from python
PyObject * MGLContext_get_blend_func(MGLContext * self) {
	PyObject * res = PyTuple_New(2);
	PyTuple_SET_ITEM(res, 0, PyLong_FromLong(self->blend_func_src));
	PyTuple_SET_ITEM(res, 1, PyLong_FromLong(self->blend_func_dst));
	return res;
}

int MGLContext_set_blend_func(MGLContext * self, PyObject * value) {
	Py_ssize_t num_values = PyTuple_GET_SIZE(value);

	if (!(num_values == 2 || num_values == 4)) {
		MGLError_Set("Invalid number of values. Must be 2 or 4.");
		return -1;
	}

	int src_rgb = (int)PyLong_AsLong(PyTuple_GET_ITEM(value, 0));
	int dst_rgb = (int)PyLong_AsLong(PyTuple_GET_ITEM(value, 1));
	int src_alpha = src_rgb;
	int dst_alpha = dst_rgb;

	if (num_values == 4) {
		src_alpha = (int)PyLong_AsLong(PyTuple_GET_ITEM(value, 2));
		dst_alpha = (int)PyLong_AsLong(PyTuple_GET_ITEM(value, 3));
	}

	if (PyErr_Occurred()) {
		return -1;
	}

	self->gl.BlendFuncSeparate(src_rgb, dst_rgb, src_alpha, dst_alpha);

	return 0;
}

// NOTE: currently never called from python
PyObject * MGLContext_get_blend_equation(MGLContext * self) {
	PyObject * res = PyTuple_New(2);
	PyTuple_SET_ITEM(res, 0, PyLong_FromLong(GL_FUNC_ADD));
	PyTuple_SET_ITEM(res, 1, PyLong_FromLong(GL_FUNC_ADD));
	return res;
}

int MGLContext_set_blend_equation(MGLContext * self, PyObject * value) {
	Py_ssize_t num_values = PyTuple_GET_SIZE(value);

	if (!(num_values == 1 || num_values == 2)) {
		MGLError_Set("Invalid number of values. Must be 1 or 2.");
		return -1;
	}

	int mode_rgb = (int)PyLong_AsLong(PyTuple_GET_ITEM(value, 0));
	int mode_alpha = mode_rgb;
	if (num_values == 2) {
		mode_alpha = (int)PyLong_AsLong(PyTuple_GET_ITEM(value, 1));
	}

	if (PyErr_Occurred()) {
		return -1;
	}

	self->gl.BlendEquationSeparate(mode_rgb, mode_alpha);

	return 0;
}

PyObject * MGLContext_get_depth_func(MGLContext * self) {
	return compare_func_to_string(self->depth_func);
}

int MGLContext_set_depth_func(MGLContext * self, PyObject * value) {
	const char * func = PyUnicode_AsUTF8(value);

	if (PyErr_Occurred()) {
		return -1;
	}

	int depth_func = compare_func_from_string(func);

	if (!depth_func) {
		return -1;
	}

	self->depth_func = depth_func;
	self->gl.DepthFunc(self->depth_func);

	return 0;
}

PyObject * MGLContext_get_multisample(MGLContext * self) {
	return PyBool_FromLong(self->multisample);
}

int MGLContext_set_multisample(MGLContext * self, PyObject * value) {
	if (value == Py_True) {
		self->gl.Enable(GL_MULTISAMPLE);
		self->multisample = true;
		return 0;
	} else if (value == Py_False) {
		self->gl.Disable(GL_MULTISAMPLE);
		self->multisample = false;
		return 0;
	}
	return -1;
}

PyObject * MGLContext_get_provoking_vertex(MGLContext * self) {
	return PyLong_FromLong(self->provoking_vertex);
}

int MGLContext_set_provoking_vertex(MGLContext * self, PyObject * value) {
	int provoking_vertex_value = PyLong_AsLong(value);
	const GLMethods & gl = self->gl;

	if (provoking_vertex_value == GL_FIRST_VERTEX_CONVENTION || provoking_vertex_value == GL_LAST_VERTEX_CONVENTION) {
		gl.ProvokingVertex(provoking_vertex_value);
		self->provoking_vertex = provoking_vertex_value;
		return 0;
	}
	return -1;
}

PyObject * MGLContext_get_polygon_offset(MGLContext * self) {
	return Py_BuildValue("ff", self->polygon_offset_factor, self->polygon_offset_units);
}

int MGLContext_set_polygon_offset(MGLContext * self, PyObject * value) {
    if (!PyTuple_CheckExact(value) || PyTuple_Size(value) != 2) {
        return -1;
    }
    float polygon_offset_factor = (float)PyFloat_AsDouble(PyTuple_GetItem(value, 0));
    float polygon_offset_units = (float)PyFloat_AsDouble(PyTuple_GetItem(value, 1));

	const GLMethods & gl = self->gl;
    if (polygon_offset_factor || polygon_offset_units) {
        gl.Enable(GL_POLYGON_OFFSET_POINT);
        gl.Enable(GL_POLYGON_OFFSET_LINE);
        gl.Enable(GL_POLYGON_OFFSET_FILL);
    } else {
        gl.Disable(GL_POLYGON_OFFSET_POINT);
        gl.Disable(GL_POLYGON_OFFSET_LINE);
        gl.Disable(GL_POLYGON_OFFSET_FILL);
    }
    gl.PolygonOffset(polygon_offset_factor, polygon_offset_units);
    self->polygon_offset_factor = polygon_offset_factor;
    self->polygon_offset_units = polygon_offset_units;
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

PyObject * MGLContext_get_max_anisotropy(MGLContext * self) {
	return PyFloat_FromDouble(self->max_anisotropy);
}

MGLFramebuffer * MGLContext_get_fbo(MGLContext * self) {
	Py_INCREF(self->bound_framebuffer);
	return self->bound_framebuffer;
}

int MGLContext_set_fbo(MGLContext * self, PyObject * value) {
	if (Py_TYPE(value) != &MGLFramebuffer_Type) {
		return -1;
	}
	Py_INCREF(value);
	Py_DECREF(self->bound_framebuffer);
	self->bound_framebuffer = (MGLFramebuffer *)value;
	return 0;
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
		static PyObject * res_cw = PyUnicode_FromString("cw");
		Py_INCREF(res_cw);
		return res_cw;
	}
	static PyObject * res_ccw = PyUnicode_FromString("ccw");
	Py_INCREF(res_ccw);
	return res_ccw;
}

int MGLContext_set_front_face(MGLContext * self, PyObject * value) {
	const char * str = PyUnicode_AsUTF8(value);

	if (!strcmp(str, "cw")) {
		self->front_face = GL_CW;
	} else if (!strcmp(str, "ccw")) {
		self->front_face = GL_CCW;
	} else {
		MGLError_Set("invalid front_face");
		return -1;
	}

	self->gl.FrontFace(self->front_face);
	return 0;
}

PyObject * MGLContext_get_cull_face(MGLContext * self) {
	if (self->front_face == GL_FRONT) {
		static PyObject * res_cw = PyUnicode_FromString("front");
		Py_INCREF(res_cw);
		return res_cw;
	}
	else if (self->front_face == GL_BACK) {
		static PyObject * res_cw = PyUnicode_FromString("back");
		Py_INCREF(res_cw);
		return res_cw;
	}
	static PyObject * res_ccw = PyUnicode_FromString("front_and_back");
	Py_INCREF(res_ccw);
	return res_ccw;
}

int MGLContext_set_cull_face(MGLContext * self, PyObject * value) {
	const char * str = PyUnicode_AsUTF8(value);

	if (!strcmp(str, "front")) {
		self->cull_face = GL_FRONT;
	} else if (!strcmp(str, "back")) {
		self->cull_face = GL_BACK;
    } else if (!strcmp(str, "front_and_back")) {
		self->cull_face = GL_FRONT_AND_BACK;
	} else {
		MGLError_Set("invalid cull_face");
		return -1;
	}

	self->gl.CullFace(self->cull_face);
	return 0;
}


PyObject * MGLContext_get_patch_vertices(MGLContext * self) {
	int patch_vertices = 0;

	self->gl.GetIntegerv(GL_PATCH_VERTICES, &patch_vertices);

	return PyLong_FromLong(patch_vertices);
}

int MGLContext_set_patch_vertices(MGLContext * self, PyObject * value) {
	int patch_vertices = PyLong_AsLong(value);

	if (PyErr_Occurred()) {
		return -1;
	}

	if (!patch_vertices) {
		return -1;
	}

	self->gl.PatchParameteri(GL_PATCH_VERTICES, patch_vertices);

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

PyObject * MGLContext_get_version_code(MGLContext * self, void * closure) {
	return PyLong_FromLong(self->version_code);
}

PyObject * MGLContext_get_extensions(MGLContext * self, void * closure) {
	return self->extensions;
}

PyObject * MGLContext_get_info(MGLContext * self, void * closure) {
	const GLMethods & gl = self->gl;

	PyObject * info = PyDict_New();

	const char * vendor = (const char *)gl.GetString(GL_VENDOR);
	PyDict_SetItemString(
		info,
		"GL_VENDOR",
		PyUnicode_FromString(vendor ? vendor : "")
	);

	const char * renderer = (const char *)gl.GetString(GL_RENDERER);
	PyDict_SetItemString(
		info,
		"GL_RENDERER",
		PyUnicode_FromString(renderer ? renderer : "")
	);

	const char * version = (const char *)gl.GetString(GL_VERSION);
	PyDict_SetItemString(
		info,
		"GL_VERSION",
		PyUnicode_FromString(version ? version : "")
	);

	{
		float gl_point_size_range[2] = {};
		gl.GetFloatv(GL_POINT_SIZE_RANGE, gl_point_size_range);

		PyDict_SetItemString(
			info,
			"GL_POINT_SIZE_RANGE",
			tuple2(
				PyFloat_FromDouble(gl_point_size_range[0]),
				PyFloat_FromDouble(gl_point_size_range[1])
			)
		);

		float gl_smooth_line_width_range[2] = {};
		gl.GetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, gl_smooth_line_width_range);

		PyDict_SetItemString(
			info,
			"GL_SMOOTH_LINE_WIDTH_RANGE",
			tuple2(
				PyFloat_FromDouble(gl_smooth_line_width_range[0]),
				PyFloat_FromDouble(gl_smooth_line_width_range[1])
			)
		);

		float gl_aliased_line_width_range[2] = {};
		gl.GetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, gl_aliased_line_width_range);

		PyDict_SetItemString(
			info,
			"GL_ALIASED_LINE_WIDTH_RANGE",
			tuple2(
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

		int gl_context_profile_mask = 0;
		gl.GetIntegerv(GL_CONTEXT_PROFILE_MASK, &gl_context_profile_mask);

		int gl_uniform_buffer_offset_alignment = 0;
		gl.GetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &gl_uniform_buffer_offset_alignment);

		PyDict_SetItemString(info, "GL_MINOR_VERSION", PyLong_FromLong(gl_minor_version));
		PyDict_SetItemString(info, "GL_MAJOR_VERSION", PyLong_FromLong(gl_major_version));
		PyDict_SetItemString(info, "GL_SAMPLE_BUFFERS", PyLong_FromLong(gl_sample_buffers));
		PyDict_SetItemString(info, "GL_SUBPIXEL_BITS", PyLong_FromLong(gl_subpixel_bits));
		PyDict_SetItemString(info, "GL_CONTEXT_PROFILE_MASK", PyLong_FromLong(gl_context_profile_mask));
		PyDict_SetItemString(info, "GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT", PyLong_FromLong(gl_uniform_buffer_offset_alignment));
	}

	{
		unsigned char gl_doublebuffer = 0;
		gl.GetBooleanv(GL_DOUBLEBUFFER, &gl_doublebuffer);

		unsigned char gl_stereo = 0;
		gl.GetBooleanv(GL_STEREO, &gl_stereo);

		PyDict_SetItemString(info, "GL_DOUBLEBUFFER", PyBool_FromLong(gl_doublebuffer));
		PyDict_SetItemString(info, "GL_STEREO", PyBool_FromLong(gl_stereo));
	}

	{
		int gl_max_viewport_dims[2] = {};
		gl.GetIntegerv(GL_MAX_VIEWPORT_DIMS, gl_max_viewport_dims);

		PyDict_SetItemString(
			info,
			"GL_MAX_VIEWPORT_DIMS",
			tuple2(
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

		int gl_max_geometry_output_vertices = 0;
		gl.GetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &gl_max_geometry_output_vertices);

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

		long long gl_max_server_wait_timeout = 0;

		if (gl.GetInteger64v) {
			gl.GetInteger64v(GL_MAX_SERVER_WAIT_TIMEOUT, &gl_max_server_wait_timeout);
		}

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

		int gl_max_varying_vectors = 0;
		gl.GetIntegerv(GL_MAX_VARYING_VECTORS, &gl_max_varying_vectors);

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
		PyDict_SetItemString(info, "GL_MAX_GEOMETRY_OUTPUT_VERTICES", PyLong_FromLong(gl_max_geometry_output_vertices));
		PyDict_SetItemString(info, "GL_MAX_INTEGER_SAMPLES", PyLong_FromLong(gl_max_integer_samples));
		PyDict_SetItemString(info, "GL_MAX_SAMPLES", PyLong_FromLong(gl_max_samples));
		PyDict_SetItemString(info, "GL_MAX_RECTANGLE_TEXTURE_SIZE", PyLong_FromLong(gl_max_rectangle_texture_size));
		PyDict_SetItemString(info, "GL_MAX_RENDERBUFFER_SIZE", PyLong_FromLong(gl_max_renderbuffer_size));
		PyDict_SetItemString(info, "GL_MAX_SAMPLE_MASK_WORDS", PyLong_FromLong(gl_max_sample_mask_words));
		PyDict_SetItemString(info, "GL_MAX_SERVER_WAIT_TIMEOUT", PyLong_FromLongLong(gl_max_server_wait_timeout));
		PyDict_SetItemString(info, "GL_MAX_TEXTURE_BUFFER_SIZE", PyLong_FromLong(gl_max_texture_buffer_size));
		PyDict_SetItemString(info, "GL_MAX_TEXTURE_IMAGE_UNITS", PyLong_FromLong(gl_max_texture_image_units));
		PyDict_SetItemString(info, "GL_MAX_TEXTURE_LOD_BIAS", PyLong_FromLong(gl_max_texture_lod_bias));
		PyDict_SetItemString(info, "GL_MAX_TEXTURE_SIZE", PyLong_FromLong(gl_max_texture_size));
		PyDict_SetItemString(info, "GL_MAX_UNIFORM_BUFFER_BINDINGS", PyLong_FromLong(gl_max_uniform_buffer_bindings));
		PyDict_SetItemString(info, "GL_MAX_UNIFORM_BLOCK_SIZE", PyLong_FromLong(gl_max_uniform_block_size));
		PyDict_SetItemString(info, "GL_MAX_VARYING_VECTORS", PyLong_FromLong(gl_max_varying_vectors));
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
			tuple2(
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
			tuple3(
				PyLong_FromLong(gl_max_compute_work_group_count[0]),
				PyLong_FromLong(gl_max_compute_work_group_count[1]),
				PyLong_FromLong(gl_max_compute_work_group_count[2])
			)
		);

		PyDict_SetItemString(
			info,
			"GL_MAX_COMPUTE_WORK_GROUP_SIZE",
			tuple3(
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

		long long gl_max_element_index = 0;

		if (gl.GetInteger64v) {
			gl.GetInteger64v(GL_MAX_ELEMENT_INDEX, &gl_max_element_index);
		}

		long long gl_max_shader_storage_block_size = 0;

		if (gl.GetInteger64v) {
			gl.GetInteger64v(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &gl_max_shader_storage_block_size);
		}

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
		PyDict_SetItemString(info, "GL_MAX_ELEMENT_INDEX", PyLong_FromLongLong(gl_max_element_index));
		PyDict_SetItemString(info, "GL_MAX_SHADER_STORAGE_BLOCK_SIZE", PyLong_FromLongLong(gl_max_shader_storage_block_size));
	}

	return info;
}

PyGetSetDef MGLContext_tp_getseters[] = {
	{(char *)"line_width", (getter)MGLContext_get_line_width, (setter)MGLContext_set_line_width, 0, 0},
	{(char *)"point_size", (getter)MGLContext_get_point_size, (setter)MGLContext_set_point_size, 0, 0},

	{(char *)"depth_func", (getter)MGLContext_get_depth_func, (setter)MGLContext_set_depth_func, 0, 0},
	{(char *)"blend_func", (getter)MGLContext_get_blend_func, (setter)MGLContext_set_blend_func, 0, 0},
	{(char *)"blend_equation", (getter)MGLContext_get_blend_equation, (setter)MGLContext_set_blend_equation, 0, 0},
	{(char *)"multisample", (getter)MGLContext_get_multisample, (setter)MGLContext_set_multisample, 0, 0},

	{(char *)"provoking_vertex", (getter)MGLContext_get_provoking_vertex, (setter)MGLContext_set_provoking_vertex, 0, 0},
	{(char *)"polygon_offset", (getter)MGLContext_get_polygon_offset, (setter)MGLContext_set_polygon_offset, 0, 0},

	{(char *)"default_texture_unit", (getter)MGLContext_get_default_texture_unit, (setter)MGLContext_set_default_texture_unit, 0, 0},
	{(char *)"max_samples", (getter)MGLContext_get_max_samples, 0, 0, 0},
	{(char *)"max_integer_samples", (getter)MGLContext_get_max_integer_samples, 0, 0, 0},
	{(char *)"max_texture_units", (getter)MGLContext_get_max_texture_units, 0, 0, 0},
	{(char *)"max_anisotropy", (getter)MGLContext_get_max_anisotropy, 0, 0, 0},

	{(char *)"fbo", (getter)MGLContext_get_fbo, (setter)MGLContext_set_fbo, 0, 0},

	{(char *)"wireframe", (getter)MGLContext_get_wireframe, (setter)MGLContext_set_wireframe, 0, 0},
	{(char *)"front_face", (getter)MGLContext_get_front_face, (setter)MGLContext_set_front_face, 0, 0},
	{(char *)"cull_face", (getter)MGLContext_get_cull_face, (setter)MGLContext_set_cull_face, 0, 0},

	{(char *)"patch_vertices", (getter)MGLContext_get_patch_vertices, (setter)MGLContext_set_patch_vertices, 0, 0},

	{(char *)"extensions", (getter)MGLContext_get_extensions, 0, 0, 0},
	{(char *)"info", (getter)MGLContext_get_info, 0, 0, 0},
	{(char *)"error", (getter)MGLContext_get_error, 0, 0, 0},
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
	0,                                                      // tp_init
	0,                                                      // tp_alloc
	MGLContext_tp_new,                                      // tp_new
};

void MGLContext_Invalidate(MGLContext * context) {
	if (Py_TYPE(context) == &MGLInvalidObject_Type) {
		return;
	}

	PyObject_CallMethod(context->ctx, "release", NULL);

	// TODO: decref

	Py_SET_TYPE(context, &MGLInvalidObject_Type);
	Py_DECREF(context);
}
