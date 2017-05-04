#include "Context.hpp"

#include "Error.hpp"
#include "Buffer.hpp"
#include "ComputeShader.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "Program.hpp"
#include "Shader.hpp"
#include "Framebuffer.hpp"
#include "FramebufferAttachment.hpp"
#include "Renderbuffer.hpp"
#include "EnableFlag.hpp"
#include "InvalidObject.hpp"
#include "Attribute.hpp"
#include "Uniform.hpp"

#include "BufferFormat.hpp"

PyObject * MGLContext_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLContext * self = (MGLContext *)type->tp_alloc(type, 0);

	#ifdef MGL_VERBOSE
	printf("MGLContext_tp_new %p\n", self);
	#endif

	if (self) {
	}

	return (PyObject *)self;
}

void MGLContext_tp_dealloc(MGLContext * self) {

	#ifdef MGL_VERBOSE
	printf("MGLContext_tp_dealloc %p\n", self);
	#endif

	MGLContext_Type.tp_free((PyObject *)self);
}

int MGLContext_tp_init(MGLContext * self, PyObject * args, PyObject * kwargs) {
	MGLError * error = MGLError_New(TRACE, "Cannot create Context manually");
	PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
	return -1;
}

PyObject * MGLContext_tp_str(MGLContext * self) {
//	return PyUnicode_FromFormat("<ModernGL.Context: rc = %p, dc = %p>", self->rc_handle, self->dc_handle);
	return PyUnicode_FromFormat("<ModernGL.Context");
}

// PyObject * MGLContext_make_current(MGLContext * self) {
// 	if (!wglMakeCurrent(self->dc_handle, self->rc_handle)) {
// 		MGLError * error = MGLError_New(TRACE, "Cannot select context");
// 		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
// 		return 0;
// 	}

// 	Py_RETURN_NONE;
// }

const char * MGLContext_make_current_doc = R"(
)";

PyObject * MGLContext_clear(MGLContext * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"r", "g", "b", "a", 0};

	int r = 0;
	int g = 0;
	int b = 0;
	int a = 0;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"|BBBB",
		(char **)kwlist,
		&r,
		&g,
		&b,
		&a
	);

	if (!args_ok) {
		return 0;
	}

	const float c = 1.0f / 255.0f;

	const GLMethods & gl = self->gl;
	gl.ClearColor(r * c, g * c, b * c, a * c);
	gl.Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	Py_RETURN_NONE;
}

const char * MGLContext_clear_doc = R"(
	clear(r = 0, g = 0, b = 0, a = 0)

	Clear the framebuffer.

	Args:
		optional r, g, b: color components.
		optional a: alpha component.

	Returns:
		``None``
)";

PyObject * MGLContext_enable(MGLContext * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"flags", 0};

	MGLEnableFlag * flags;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"O!",
		(char **)kwlist,
		&MGLEnableFlag_Type,
		&flags
	);

	if (!args_ok) {
		return 0;
	}

	self->gl.Enable(flags->flag);

	Py_RETURN_NONE;
}

const char * MGLContext_enable_doc = R"(
	enable(flags)

	Enable flags.

	Args:
		flags: flags to enable.

	Returns:
		``None``
)";

PyObject * MGLContext_disable(MGLContext * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"flags", 0};

	MGLEnableFlag * flags;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"O!",
		(char **)kwlist,
		&MGLEnableFlag_Type,
		&flags
	);

	if (!args_ok) {
		return 0;
	}

	self->gl.Disable(flags->flag);

	Py_RETURN_NONE;
}

const char * MGLContext_disable_doc = R"(
	disable(flags)

	Disable flags.

	Args:
		flags: flags to disable.

	Returns:
		``None``
)";

PyObject * MGLContext_finish(MGLContext * self) {
	self->gl.Finish();
	Py_RETURN_NONE;
}

const char * MGLContext_finish_doc = R"(
	finish()

	Not used.
)";

PyObject * MGLContext_copy_buffer(MGLContext * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"dst", "src", "size", "read_offset", "write_offset", 0};

	MGLBuffer * dst;
	MGLBuffer * src;

	int size = -1;
	int read_offset = 0;
	int write_offset = 0;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"O!O!|I$II",
		(char **)kwlist,
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
		MGLError * error = MGLError_New(TRACE, "buffer underflow");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	if (read_offset + size > src->size || write_offset + size > dst->size) {
		MGLError * error = MGLError_New(TRACE, "buffer overflow");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	const GLMethods & gl = self->gl;

	gl.BindBuffer(GL_COPY_READ_BUFFER, src->buffer_obj);
	gl.BindBuffer(GL_COPY_WRITE_BUFFER, dst->buffer_obj);
	gl.CopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, read_offset, write_offset, size);

	Py_RETURN_NONE;
}

const char * MGLContext_copy_buffer_doc = R"(
	copy_buffer(dst, src, size, read_offset = 0, write_offset = 0)

	Copy buffer content.

	Args:
		dst: Destination buffer.
		src: Source buffer.
		optional size: Size to copy.

	Keyword Args:
		read_offset: Read offset.
		write_offset: Write offset.

	Returns:
		``None``
)";

PyObject * MGLContext_copy_texture(MGLContext * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"value", 0};

	// TODO:

	int value = 0;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"|i",
		(char **)kwlist,
		&value
	);

	if (!args_ok) {
		return 0;
	}

	PyErr_Format(PyExc_Exception, "Unknown error in %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__);
	return 0;

	// Py_RETURN_NONE;
}

const char * MGLContext_copy_texture_doc = R"(
	copy_texture()
)";

MGLBuffer * MGLContext_Buffer(MGLContext * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"data", "reserve", "dynamic", 0};

	PyObject * data = Py_None;
	int reserve = 0;
	int dynamic = false;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"|O$Ip",
		(char **)kwlist,
		&data,
		&reserve,
		&dynamic
	);

	if (!args_ok) {
		return 0;
	}

	if ((data != Py_None && reserve) || (data == Py_None && !reserve)) {
		MGLError * error = MGLError_New(TRACE, "data and reserve are mutually exclusive");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	Py_buffer buffer_view;

	if (data != Py_None) {
		int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
		if (get_buffer < 0) {
			MGLError * error = MGLError_New(TRACE, "data (%s) does not support buffer interface", Py_TYPE(data)->tp_name);
			PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
			return 0;
		}
	} else {
		buffer_view.len = reserve;
		buffer_view.buf = 0;
	}

	if (!buffer_view.len) {
		MGLError * error = MGLError_New(TRACE, "buffer cannot be empty");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		if (data != Py_None) {
			PyBuffer_Release(&buffer_view);
		}
		return 0;
	}

	MGLBuffer * buffer = MGLBuffer_New();

	buffer->size = (int)buffer_view.len;
	buffer->dynamic = dynamic ? true : false;

	const GLMethods & gl = self->gl;

	buffer->buffer_obj = 0;
	gl.GenBuffers(1, (GLuint *)&buffer->buffer_obj);

	if (!buffer->buffer_obj) {
		MGLError * error = MGLError_New(TRACE, "Cannot create buffer object.");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
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

const char * MGLContext_Buffer_doc = R"(
	Buffer(data, reserve = 0, dynamic = False)

	Create a Buffer.

	Args:
		data: Content of the new buffer.

	Keyword Args:
		reserve: The number of bytes to reserve.
		dynamic: Treat buffer as dynamic.

	Returns:
		:py:class:`Buffer`
)";

MGLTexture * MGLContext_Texture(MGLContext * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"size", "components", "data", "floats", 0};

	int width;
	int height;

	int components = 3;

	PyObject * data = Py_None;

	int floats = false;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"(II)|IO$p",
		(char **)kwlist,
		&width,
		&height,
		&components,
		&data,
		&floats
	);

	if (!args_ok) {
		return 0;
	}

	if (components < 1 || components > 4) {
		MGLError * error = MGLError_New(TRACE, "components must be 1, 2, 3 or 4");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	int expected_size = floats ? (width * height * components * 4) : (height * ((width * components + 3) & ~3));

	Py_buffer buffer_view;

	if (data != Py_None) {
		PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
	} else {
		buffer_view.len = expected_size;
		buffer_view.buf = 0;
	}

	if (buffer_view.len != expected_size) {
		MGLError * error = MGLError_New(TRACE, "data size mismatch %d != %d", buffer_view.len, expected_size);
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		if (data != Py_None) {
			PyBuffer_Release(&buffer_view);
		}
		return 0;
	}

	const int formats[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};

	int pixel_type = floats ? GL_FLOAT : GL_UNSIGNED_BYTE;
	int format = formats[components];

	const GLMethods & gl = self->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);

	MGLTexture * texture = MGLTexture_New();

	texture->texture_obj = 0;
	gl.GenTextures(1, (GLuint *)&texture->texture_obj);
	gl.BindTexture(GL_TEXTURE_2D, texture->texture_obj);
	gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gl.TexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, pixel_type, buffer_view.buf);

	if (data != Py_None) {
		PyBuffer_Release(&buffer_view);
	}

	texture->width = width;
	texture->height = height;
	texture->components = components;
	texture->floats = floats ? true : false;
	texture->depth = false;

	Py_INCREF(self);
	texture->context = self;

	Py_INCREF(texture);
	return texture;
}

const char * MGLContext_Texture_doc = R"(
	Texture(size, components, data = None, floats = False)

	Create a Texture.

	Args:
		size: Width, height.
		components: The number of components 1, 2, 3 or 4.
		optional data: Content of the image.

	Keyword Args:
		floats: Use floating point precision.

	Returns:
		:py:class:`Texture`
)";

MGLTexture * MGLContext_DepthTexture(MGLContext * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"size", "data", 0};

	int width;
	int height;

	PyObject * data = Py_None;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"(II)|O",
		(char **)kwlist,
		&width,
		&height,
		&data
	);

	if (!args_ok) {
		return 0;
	}

	int expected_size = width * height * 4;

	Py_buffer buffer_view;

	if (data != Py_None) {
		PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
	} else {
		buffer_view.len = expected_size;
		buffer_view.buf = 0;
	}

	if (buffer_view.len != expected_size) {
		MGLError * error = MGLError_New(TRACE, "data size mismatch %d != %d", buffer_view.len, expected_size);
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		if (data != Py_None) {
			PyBuffer_Release(&buffer_view);
		}
		return 0;
	}

	int pixel_type = GL_FLOAT;

	const GLMethods & gl = self->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->default_texture_unit);

	MGLTexture * texture = MGLTexture_New();

	texture->texture_obj = 0;
	gl.GenTextures(1, (GLuint *)&texture->texture_obj);
	gl.BindTexture(GL_TEXTURE_2D, texture->texture_obj);
	gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gl.TexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, pixel_type, buffer_view.buf);

	if (data != Py_None) {
		PyBuffer_Release(&buffer_view);
	}

	texture->width = width;
	texture->height = height;
	texture->components = 1;
	texture->floats = true;
	texture->depth = true;

	Py_INCREF(self);
	texture->context = self;

	Py_INCREF(texture);
	return texture;
}

const char * MGLContext_DepthTexture_doc = R"(
	DepthTexture(size, data = None)

	Create a DepthTexture.

	Args:
		size: The width and height.
		optional data: The pixels.

	Returns:
		:py:class:`Texture`
)";

MGLVertexArray * MGLContext_VertexArray(MGLContext * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"program", "content", "index_buffer", "skip_errors", 0};

	MGLProgram * program;
	PyObject * content;
	MGLBuffer * index_buffer = (MGLBuffer *)Py_None;
	int skip_errors = false;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"O!O!|O$p",
		(char **)kwlist,
		&MGLProgram_Type,
		&program,
		&PyList_Type,
		&content,
		&index_buffer,
		&skip_errors
	);

	if (!args_ok) {
		return 0;
	}

	if (program->context != self) {
		MGLError * error = MGLError_New(TRACE, "program belongs to a different context");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	if (index_buffer != (MGLBuffer *)Py_None && index_buffer->context != self) {
		MGLError * error = MGLError_New(TRACE, "index_buffer belongs to a different context");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	int content_len = (int)PyList_GET_SIZE(content);

	if (!content_len) {
		MGLError * error = MGLError_New(TRACE, "content must not be emtpy");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	for (int i = 0; i < content_len; ++i) {
		PyObject * tuple = PyList_GET_ITEM(content, i);

		if (Py_TYPE(tuple) != &PyTuple_Type) {
			MGLError * error = MGLError_New(TRACE, "content[%d] must be a tuple not %s", i, Py_TYPE(tuple)->tp_name);
			PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
			return 0;
		}

		int size = (int)PyTuple_GET_SIZE(tuple);

		if (size != 3) {
			MGLError * error = MGLError_New(TRACE, "content[%d] must be a tuple of size 3 not %d", i, size);
			PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
			return 0;
		}

		PyObject * buffer = PyTuple_GET_ITEM(tuple, 0);
		PyObject * format = PyTuple_GET_ITEM(tuple, 1);
		PyObject * attributes = PyTuple_GET_ITEM(tuple, 2);

		if (Py_TYPE(buffer) != &MGLBuffer_Type) {
			MGLError * error = MGLError_New(TRACE, "content[%d][0] must be a ModernGL.Buffer not %s", i, Py_TYPE(buffer)->tp_name);
			PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
			return 0;
		}

		if (Py_TYPE(format) != &PyUnicode_Type) {
			MGLError * error = MGLError_New(TRACE, "content[%d][1] must be a str not %s", i, Py_TYPE(format)->tp_name);
			PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
			return 0;
		}

		if (Py_TYPE(attributes) != &PyList_Type) {
			MGLError * error = MGLError_New(TRACE, "content[%d][2] must be a list not %s", i, Py_TYPE(attributes)->tp_name);
			PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
			return 0;
		}

		if (((MGLBuffer *)buffer)->context != self) {
			MGLError * error = MGLError_New(TRACE, "content[%d][0] belongs to a different context", i);
			PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
			return 0;
		}

		FormatIterator it = FormatIterator(PyUnicode_AsUTF8(format));
		FormatInfo format_info = it.info();

		if (!format_info.valid) {
			MGLError * error = MGLError_New(TRACE, "content[%d][1] is an invalid format", i);
			PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
			return 0;
		}

		if (i == 0 && format_info.per_instance) {
			MGLError * error = MGLError_New(TRACE, "the first vertex attribute must not be a per instance attribute");
			PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
			return 0;
		}

		int attributes_len = (int)PyList_GET_SIZE(attributes);

		if (!attributes_len) {
			MGLError * error = MGLError_New(TRACE, "content[%d][2] must not be empty", i);
			PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
			return 0;
		}

		if (attributes_len != format_info.nodes) {
			MGLError * error = MGLError_New(TRACE, "content[%d][1] and content[%d][2] size mismatch %d != %d", i, i, format_info.nodes, attributes_len);
			PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
			return 0;
		}

		for (int j = 0; j < attributes_len; ++j) {
			PyObject * attribute = PyList_GET_ITEM(attributes, j);

			if (Py_TYPE(attribute) != &PyUnicode_Type) {
				MGLError * error = MGLError_New(TRACE, "content[%d][2][%d] must be a str not %s", i, j, Py_TYPE(attribute)->tp_name);
				PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
				return 0;
			}

			if (!skip_errors) {
				const char * name = PyUnicode_AsUTF8(attribute);

				FormatNode * node = it.next();

				while (node->shape == 'x') {
					node = it.next();
				}
	
				MGLAttribute * attribute = (MGLAttribute *)PyDict_GetItemString(program->attributes, name);

				if (!attribute) {
					MGLError * error = MGLError_New(TRACE, "%s is not a valid attribute", name);
					PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
					return 0;
				}

				int scalars = attribute->dimension * attribute->array_length;

				if (scalars != node->count) {
					MGLError * error = MGLError_New(TRACE, "%s size is %d not %d", name, scalars, node->count);
					PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
					return 0;
				}

				if (attribute->shape != node->shape) {
					MGLError * error = MGLError_New(TRACE, "%s shape is '%c' not '%c'", name, attribute->shape, node->shape);
					PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
					return 0;
				}
			}
		}
	}

	if (index_buffer != (MGLBuffer *)Py_None && Py_TYPE(index_buffer) != &MGLBuffer_Type) {
		MGLError * error = MGLError_New(TRACE, "index_buffer must be a ModernGL.Buffer not %s", Py_TYPE(index_buffer)->tp_name);
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	PyObject * vertex_array_content = PyTuple_New(content_len);

	for (int i = 0; i < content_len; ++i) {
		PyObject * tuple = PyList_GET_ITEM(content, i);

		PyObject * buffer = PyTuple_GET_ITEM(tuple, 0);
		PyObject * format = PyTuple_GET_ITEM(tuple, 1);
		PyObject * attributes = PyTuple_GET_ITEM(tuple, 2);

		PyObject * new_attributes = PyList_AsTuple(attributes);
		PyObject * new_tuple = PyTuple_Pack(3, buffer, format, new_attributes);
		Py_DECREF(new_attributes);

		PyTuple_SET_ITEM(vertex_array_content, i, new_tuple);
	}

	const GLMethods & gl = self->gl;

	MGLVertexArray * array = MGLVertexArray_New();

	Py_INCREF(program);
	array->program = program;

	array->vertex_array_obj = 0;
	gl.GenVertexArrays(1, (GLuint *)&array->vertex_array_obj);
	gl.BindVertexArray(array->vertex_array_obj);

	array->content = vertex_array_content;

	Py_INCREF(index_buffer);
	array->index_buffer = index_buffer;

	if (index_buffer != (MGLBuffer *)Py_None) {
		array->num_vertices = index_buffer->size / 4;
		gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->buffer_obj);
	} else {
		array->num_vertices = -1;
	}

	for (int i = 0; i < content_len; ++i) {
		PyObject * tuple = PyTuple_GET_ITEM(vertex_array_content, i);

		MGLBuffer * buffer = (MGLBuffer *)PyTuple_GET_ITEM(tuple, 0);
		const char * format = PyUnicode_AsUTF8(PyTuple_GET_ITEM(tuple, 1));
		PyObject * attributes = PyTuple_GET_ITEM(tuple, 2);

		FormatIterator it = FormatIterator(format);
		FormatInfo format_info = it.info();

		int buf_vertices = buffer->size / format_info.size;

		if (!format_info.per_instance && array->index_buffer == (MGLBuffer *)Py_None && (!i || array->num_vertices > buf_vertices)) {
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

				if (format_info.per_instance) {
					gl.VertexAttribDivisor(location, 1);
				}

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

const char * MGLContext_VertexArray_doc = R"(
	VertexArray(program, content, index_buffer = None, skip_errors = False)

	Create a VertexArray.

	Args:
		program: The program used by :py:meth:`~VertexArray.render` and :py:meth:`~VertexArray.transform`.
		content: A list of (buffer, format, attributes).
		optional index_buffer: An index buffer.

	Keyword Args:
		skip_errors: Ignore missing attributes.

	Returns:
		:py:class:`VertexArray`
)";

MGLVertexArray * MGLContext_SimpleVertexArray(MGLContext * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"program", "buffer", "format", "attributes", "index_buffer", "skip_errors", 0};

	PyObject * program;
	PyObject * buffer;
	PyObject * format;
	PyObject * attributes;
	PyObject * index_buffer = Py_None;
	PyObject * skip_errors = Py_False;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"O!O!O!O!|O$O!",
		(char **)kwlist,
		&MGLProgram_Type,
		&program,
		&MGLBuffer_Type,
		&buffer,
		&PyUnicode_Type,
		&format,
		&PyList_Type,
		&attributes,
		&index_buffer,
		&PyBool_Type,
		&skip_errors
	);

	if (!args_ok) {
		return 0;
	}

	Py_INCREF(program);
	Py_INCREF(buffer);
	Py_INCREF(format);
	Py_INCREF(attributes);
	Py_INCREF(index_buffer);

	PyObject * new_tuple = PyTuple_New(3);
	PyTuple_SET_ITEM(new_tuple, 0, buffer);
	PyTuple_SET_ITEM(new_tuple, 1, format);
	PyTuple_SET_ITEM(new_tuple, 2, attributes);

	PyObject * content = PyList_New(1);
	PyList_SET_ITEM(content, 0, new_tuple);

	PyObject * new_args = PyTuple_New(3);
	PyTuple_SET_ITEM(new_args, 0, program);
	PyTuple_SET_ITEM(new_args, 1, content);
	PyTuple_SET_ITEM(new_args, 2, index_buffer);

	PyObject * new_kwargs = PyDict_New();
	PyDict_SetItemString(new_kwargs, "skip_errors", skip_errors);

	MGLVertexArray * result = MGLContext_VertexArray(self, new_args, new_kwargs);

	Py_DECREF(new_args);
	Py_DECREF(new_kwargs);

	return result;
}

const char * MGLContext_SimpleVertexArray_doc = R"(
	SimpleVertexArray(program, buffer, format, attributes, index_buffer = None, skip_errors = False)

	Create a SimpleVertexArray.

	Args:
		program: The program used by :py:meth:`~VertexArray.render` and :py:meth:`~VertexArray.transform`.
		buffer: The buffer.
		format: The buffer format string.
		attributes: A list of attribute names.
		optional index_buffer: An index buffer.

	Keyword Args:
		skip_errors: Ignore missing attributes.

	Returns:
		:py:class:`VertexArray`
)";

MGLProgram * MGLContext_Program(MGLContext * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"shaders", "varyings", 0};

	PyObject * shaders;
	PyObject * varyings = Py_None;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"O|O",
		(char **)kwlist,
		&shaders,
		&varyings
	);

	if (!args_ok) {
		return 0;
	}

	if (varyings != Py_None) {
		if (Py_TYPE(varyings) != &PyList_Type) {
			MGLError * error = MGLError_New(TRACE, "varyings must be a list not %s", Py_TYPE(varyings)->tp_name);
			PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
			return 0;
		}

		int num_varyings = (int)PyList_GET_SIZE(varyings);

		for (int i = 0; i < num_varyings; ++i) {
			PyObject * item = PyList_GET_ITEM(varyings, i);
			if (Py_TYPE(item) != &PyUnicode_Type) {
				MGLError * error = MGLError_New(TRACE, "varyings[%d] must be a str not %s", i, Py_TYPE(item)->tp_name);
				PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
				return 0;
			}
		}
	}

	PyObject * program_shaders = 0;

	if (Py_TYPE(shaders) == &PyList_Type) {

		int counter[NUM_SHADER_SLOTS] = {};

		int num_shaders = (int)PyList_GET_SIZE(shaders);

		for (int i = 0; i < num_shaders; ++i) {
			PyObject * item = PyList_GET_ITEM(shaders, i);
			if (Py_TYPE(item) != &MGLShader_Type) {
				MGLError * error = MGLError_New(TRACE, "shaders[%d] must be a ModernGL.Shader not %s", i, Py_TYPE(item)->tp_name);
				PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
				return 0;
			}

			MGLShader * shader = (MGLShader *)item;

			if (shader->context != self) {
				MGLError * error = MGLError_New(TRACE, "shaders[%d] belongs to a different context", i);
				PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
				return 0;
			}

			counter[shader->shader_slot] += 1;

			if (counter[shader->shader_slot] > 1) {
				MGLError * error = MGLError_New(TRACE, "shaders has duplicate %s", SHADER_NAME[shader->shader_slot]);
				PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
				return 0;
			}
		}

		program_shaders = PyList_AsTuple(shaders);

	} else if (Py_TYPE(shaders) == &MGLShader_Type) {

		Py_INCREF(shaders);
		program_shaders = PyTuple_Pack(1, shaders);

	} else {
		MGLError * error = MGLError_New(TRACE, "shaders must be a list not %s", Py_TYPE(shaders)->tp_name);
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	MGLProgram * program = MGLProgram_New();

	program->shaders = program_shaders;

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

const char * MGLContext_Program_doc = R"(
	Program(shaders, varyings = None)

	Create a Program.

	Args:
		shaders: A list of :py:class:`Shader` objects.
		optional varyings: A list of varying names.

	Returns:
		:py:class:`Program`
)";

template <int ShaderSlot>
MGLShader * MGLContext_Shader(MGLContext * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"source", 0};

	PyObject * source;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"O",
		(char **)kwlist,
		&source
	);

	if (!args_ok) {
		return 0;
	}

	if (!PyUnicode_Check(source)) {
		MGLError * error = MGLError_New(TRACE, "source must be a str not %s", Py_TYPE(source)->tp_name);
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	MGLShader * shader = MGLShader_New();

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

const char * MGLContext_VertexShader_doc = R"(
	VertexShader(source)

	Create a Shader.

	Args:
		source: The source code in GLSL.

	Returns:
		:py:class:`Shader`
)";

const char * MGLContext_FragmentShader_doc = R"(
	FragmentShader(source)

	Create a Shader.

	Args:
		source: The source code in GLSL.

	Returns:
		:py:class:`Shader`
)";

const char * MGLContext_GeometryShader_doc = R"(
	GeometryShader(source)

	Create a Shader.

	Args:
		source: The source code in GLSL.

	Returns:
		:py:class:`Shader`
)";

const char * MGLContext_TessEvaluationShader_doc = R"(
	TessEvaluationShader(source)

	Create a Shader.

	Args:
		source: The source code in GLSL.

	Returns:
		:py:class:`Shader`
)";

const char * MGLContext_TessControlShader_doc = R"(
	TessControlShader(source)

	Create a Shader.

	Args:
		source: The source code in GLSL.

	Returns:
		:py:class:`Shader`
)";

MGLFramebuffer * MGLContext_Framebuffer(MGLContext * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"attachments", 0};

	PyObject * attachments;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"O",
		(char **)kwlist,
		&attachments
	);

	if (!args_ok) {
		return 0;
	}

	PyObject * color_attachments = 0;
	PyObject * depth_attachment = 0;

	if (Py_TYPE(attachments) == &PyList_Type) {

		int attachments_len = (int)PyList_GET_SIZE(attachments);

		if (!attachments_len) {
			MGLError * error = MGLError_New(TRACE, "attachments must not be empty");
			PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
			return 0;
		}

		int depth_attachment_index = -1;

		for (int i = 0; i < attachments_len; ++i) {
			PyObject * item = PyList_GET_ITEM(attachments, i);

			if (!PyObject_IsSubclass((PyObject *)Py_TYPE(item), (PyObject *)&MGLFramebufferAttachment_Type)) {
				MGLError * error = MGLError_New(TRACE, "attachments[%d] must be a ModernGL.FramebufferAttachment not %s", i, Py_TYPE(item)->tp_name);
				PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
				return 0;
			}

			MGLFramebufferAttachment * attachment = (MGLFramebufferAttachment *)item;

			if (attachment->context != self) {
				MGLError * error = MGLError_New(TRACE, "attachments[%d] belongs to a different context", i);
				PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
				return 0;
			}

			if (attachment->depth) {
				if (depth_attachment_index >= 0) {
					MGLError * error = MGLError_New(TRACE, "attachments[%d] and attachments[%d] are both depth attachments", i, depth_attachment_index);
					PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
					return 0;
				}
				depth_attachment_index = i;
			}
		}

		int color_attachments_cnt = 0;
		int color_attachments_len = attachments_len - ((depth_attachment_index >= 0) ? 1 : 0);
		color_attachments = PyTuple_New(color_attachments_len);

		for (int i = 0; i < attachments_len; ++i) {
			if (i == depth_attachment_index) {
				continue;
			}

			PyObject * item = PyList_GET_ITEM(attachments, i);

			Py_INCREF(item);
			PyTuple_SET_ITEM(color_attachments, color_attachments_cnt, item);

			color_attachments_cnt += 1;
		}

		if (depth_attachment_index >= 0) {
			depth_attachment = PyList_GET_ITEM(attachments, depth_attachment_index);
			Py_INCREF(depth_attachment);
		}

	} else if (Py_TYPE(attachments) == &MGLTexture_Type) {

		MGLTexture * texture = (MGLTexture *)attachments;

		if (texture->depth) {
			depth_attachment = attachments;
			Py_INCREF(depth_attachment);
		} else {
			color_attachments = PyTuple_Pack(1, attachments);
		}

	} else if (Py_TYPE(attachments) == &MGLRenderbuffer_Type) {

		MGLRenderbuffer * renderbuffer = (MGLRenderbuffer *)attachments;

		if (renderbuffer->depth) {
			depth_attachment = attachments;
			Py_INCREF(depth_attachment);
		} else {
			color_attachments = PyTuple_Pack(1, attachments);
		}

	} else {
		MGLError * error = MGLError_New(TRACE, "attachments must be a list not %s", Py_TYPE(attachments)->tp_name);
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	const GLMethods & gl = self->gl;

	int draw_framebuffer = 0;
	gl.GetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint *)&draw_framebuffer);

	MGLFramebuffer * framebuffer = MGLFramebuffer_New();

	framebuffer->framebuffer_obj = 0;
	gl.GenFramebuffers(1, (GLuint *)&framebuffer->framebuffer_obj);
	gl.BindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebuffer_obj);

	if (color_attachments) {
		int color_attachments_len = (int)PyTuple_GET_SIZE(color_attachments);

		for (int i = 0; i < color_attachments_len; ++i) {
			PyObject * item = PyTuple_GET_ITEM(color_attachments, i);

			if (Py_TYPE(item) == &MGLTexture_Type) {

				MGLTexture * texture = (MGLTexture *)item;

				gl.FramebufferTexture2D(
					GL_FRAMEBUFFER,
					GL_COLOR_ATTACHMENT0 + i,
					GL_TEXTURE_2D,
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
	}

	if (depth_attachment) {
		if (Py_TYPE(depth_attachment) == &MGLTexture_Type) {
			MGLTexture * texture = (MGLTexture *)depth_attachment;

			gl.FramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_DEPTH_ATTACHMENT,
				GL_TEXTURE_2D,
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
	}

	int status = gl.CheckFramebufferStatus(GL_FRAMEBUFFER);

	gl.BindFramebuffer(GL_FRAMEBUFFER, draw_framebuffer);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		const char * message = "framebuffer is not complete";

		switch (status) {
			case GL_FRAMEBUFFER_UNDEFINED:
				message = "framebuffer is not complete 0";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				message = "framebuffer is not complete 1";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				message = "framebuffer is not complete 2";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				message = "framebuffer is not complete 3";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				message = "framebuffer is not complete 4";
				break;

			case GL_FRAMEBUFFER_UNSUPPORTED:
				message = "framebuffer is not complete 5";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				message = "framebuffer is not complete 6";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				message = "framebuffer is not complete 7";
				break;
		}

		MGLError * error = MGLError_New(TRACE, message);
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	framebuffer->color_attachments = color_attachments;
	framebuffer->depth_attachment = depth_attachment;

	Py_INCREF(self);
	framebuffer->context = self;

	Py_INCREF(framebuffer);
	return framebuffer;
}

const char * MGLContext_Framebuffer_doc = R"(
	Framebuffer(attachments)

	Create a Framebuffer.

	Args:
		attachments: A list of :py:class:`Texture` or :py:class:`Renderbuffer` objects.

	Returns:
		:py:class:`Framebuffer`
)";

MGLRenderbuffer * MGLContext_Renderbuffer(MGLContext * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"size", "components", "floats", 0};

	int width;
	int height;

	int components = 4;

	int floats = true;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"(II)|I$p",
		(char **)kwlist,
		&width,
		&height,
		&components,
		&floats
	);

	if (!args_ok) {
		return 0;
	}

	if (components < 1 || components > 4) {
		MGLError * error = MGLError_New(TRACE, "components must be 1, 2, 3 or 4");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	const int int_formats[] = {0, GL_R8UI, GL_RG8UI, GL_RGB8UI, GL_RGBA8UI};
	const int float_formats[] = {0, GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F};

	int format = floats ? float_formats[components] : int_formats[components];

	const GLMethods & gl = self->gl;

	MGLRenderbuffer * renderbuffer = MGLRenderbuffer_New();

	renderbuffer->renderbuffer_obj = 0;
	gl.GenRenderbuffers(1, (GLuint *)&renderbuffer->renderbuffer_obj);
	gl.BindRenderbuffer(GL_RENDERBUFFER, renderbuffer->renderbuffer_obj);
	gl.RenderbufferStorage(GL_RENDERBUFFER, format, width, height);

	renderbuffer->width = width;
	renderbuffer->height = height;
	renderbuffer->components = components;
	renderbuffer->floats = floats ? true : false;
	renderbuffer->depth = false;

	Py_INCREF(self);
	renderbuffer->context = self;

	Py_INCREF(renderbuffer);
	return renderbuffer;
}

const char * MGLContext_Renderbuffer_doc = R"(
	Renderbuffer(size, components, floats = True)

	Create a Renderbuffer.

	Args:
		size: Width, height.
		components: The number of components 1, 2, 3 or 4.

	Keyword Args:
		floats: Use floating point precision.

	Returns:
		:py:class:`Renderbuffer`
)";

MGLRenderbuffer * MGLContext_DepthRenderbuffer(MGLContext * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"size", 0};

	int width;
	int height;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"(II)",
		(char **)kwlist,
		&width,
		&height
	);

	if (!args_ok) {
		return 0;
	}

	const GLMethods & gl = self->gl;

	MGLRenderbuffer * renderbuffer = MGLRenderbuffer_New();

	renderbuffer->renderbuffer_obj = 0;
	gl.GenRenderbuffers(1, (GLuint *)&renderbuffer->renderbuffer_obj);
	gl.BindRenderbuffer(GL_RENDERBUFFER, renderbuffer->renderbuffer_obj);
	gl.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

	renderbuffer->width = width;
	renderbuffer->height = height;
	renderbuffer->components = 1;
	renderbuffer->floats = true;
	renderbuffer->depth = true;

	Py_INCREF(self);
	renderbuffer->context = self;

	Py_INCREF(renderbuffer);
	return renderbuffer;
}

const char * MGLContext_DepthRenderbuffer_doc = R"(
	DepthRenderbuffer(size, floats = True)

	Create a Renderbuffer.

	Args:
		size: Width, height.

	Keyword Args:
		floats: Use floating point precision.

	Returns:
		:py:class:`Renderbuffer`
)";

MGLComputeShader * MGLContext_ComputeShader(MGLContext * self, PyObject * args, PyObject * kwargs) {
	static const char * kwlist[] = {"source", 0};

	PyObject * source;

	int args_ok = PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"O",
		(char **)kwlist,
		&source
	);

	if (!args_ok) {
		return 0;
	}

	if (!PyUnicode_Check(source)) {
		MGLError * error = MGLError_New(TRACE, "source must be a str not %s", Py_TYPE(source)->tp_name);
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	MGLComputeShader * compute_shader = MGLComputeShader_New();

	Py_INCREF(source);
	compute_shader->source = source;

	Py_INCREF(self);
	compute_shader->context = self;

	const char * source_str = PyUnicode_AsUTF8(source);

	const GLMethods & gl = self->gl;

	int shader_obj = gl.CreateShader(GL_COMPUTE_SHADER);

	if (!shader_obj) {
		MGLError * error = MGLError_New(TRACE, "cannot create shader object");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	gl.ShaderSource(shader_obj, 1, &source_str, 0);
	gl.CompileShader(shader_obj);

	int compiled = GL_FALSE;
	gl.GetShaderiv(shader_obj, GL_COMPILE_STATUS, &compiled);

	if (!compiled) {
		const char * message = "GLSL Compiler failed";
		const char * title = "ComputeShader";
		const char * underline = "=============";

		int log_len = 0;
		gl.GetShaderiv(shader_obj, GL_INFO_LOG_LENGTH, &log_len);

		char * log = new char[log_len];
		gl.GetShaderInfoLog(shader_obj, log_len, &log_len, log);

		gl.DeleteShader(shader_obj);

		MGLError * error = MGLError_New(TRACE, "%s\n\n%s\n%s\n%s\n", message, title, underline, log);
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);

		delete[] log;
		return 0;
	}

	compute_shader->shader_obj = shader_obj;

	int program_obj = gl.CreateProgram();

	if (!program_obj) {
		MGLError * error = MGLError_New(TRACE, "cannot create program object");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	gl.AttachShader(program_obj, shader_obj);
	gl.LinkProgram(program_obj);

	int linked = GL_FALSE;
	gl.GetProgramiv(program_obj, GL_LINK_STATUS, &linked);

	if (!linked) {
		const char * message = "GLSL Linker failed";
		const char * title = "ComputeShader";
		const char * underline = "=============";

		int log_len = 0;
		gl.GetProgramiv(program_obj, GL_INFO_LOG_LENGTH, &log_len);

		char * log = new char[log_len];
		gl.GetProgramInfoLog(program_obj, log_len, &log_len, log);

		gl.DeleteProgram(program_obj);

		MGLError * error = MGLError_New(TRACE, "%s\n\n%s\n%s\n%s\n", message, title, underline, log);
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);

		delete[] log;
		return 0;
	}

	compute_shader->program_obj = program_obj;

	return compute_shader;
}

const char * MGLContext_ComputeShader_doc = R"(
	ComputeShader(source)

	Create a ComputeShader.

	Args:
		source: The source of the compute shader.

	Returns:
		:py:class:`ComputeShader`
)";

PyObject * MGLContext_release(MGLContext * self) {
	// MGLContext_Invalidate(self);
	Py_RETURN_NONE;
}

PyMethodDef MGLContext_tp_methods[] = {
	// {"make_current", (PyCFunction)MGLContext_make_current, METH_NOARGS, MGLContext_make_current_doc},

	{"release", (PyCFunction)MGLContext_release, METH_NOARGS, 0},

	{"clear", (PyCFunction)MGLContext_clear, METH_VARARGS | METH_KEYWORDS, MGLContext_clear_doc},
	{"enable", (PyCFunction)MGLContext_enable, METH_VARARGS | METH_KEYWORDS, MGLContext_enable_doc},
	{"disable", (PyCFunction)MGLContext_disable, METH_VARARGS | METH_KEYWORDS, MGLContext_disable_doc},
	{"finish", (PyCFunction)MGLContext_finish, METH_NOARGS, MGLContext_finish_doc},
	{"copy_buffer", (PyCFunction)MGLContext_copy_buffer, METH_VARARGS | METH_KEYWORDS, MGLContext_copy_buffer_doc},
	{"copy_texture", (PyCFunction)MGLContext_copy_texture_doc, METH_VARARGS | METH_KEYWORDS, MGLContext_copy_texture_doc},


	{"Buffer", (PyCFunction)MGLContext_Buffer, METH_VARARGS | METH_KEYWORDS, MGLContext_Buffer_doc},
	{"Texture", (PyCFunction)MGLContext_Texture, METH_VARARGS | METH_KEYWORDS, MGLContext_Texture_doc},
	{"DepthTexture", (PyCFunction)MGLContext_DepthTexture, METH_VARARGS | METH_KEYWORDS, MGLContext_DepthTexture_doc},
	{"VertexArray", (PyCFunction)MGLContext_VertexArray, METH_VARARGS | METH_KEYWORDS, MGLContext_VertexArray_doc},
	{"SimpleVertexArray", (PyCFunction)MGLContext_SimpleVertexArray, METH_VARARGS | METH_KEYWORDS, MGLContext_SimpleVertexArray_doc},
	{"Program", (PyCFunction)MGLContext_Program, METH_VARARGS | METH_KEYWORDS, MGLContext_Program_doc},
	{"VertexShader", (PyCFunction)MGLContext_Shader<VERTEX_SHADER_SLOT>, METH_VARARGS | METH_KEYWORDS, MGLContext_VertexShader_doc},
	{"FragmentShader", (PyCFunction)MGLContext_Shader<FRAGMENT_SHADER_SLOT>, METH_VARARGS | METH_KEYWORDS, MGLContext_FragmentShader_doc},
	{"GeometryShader", (PyCFunction)MGLContext_Shader<GEOMETRY_SHADER_SLOT>, METH_VARARGS | METH_KEYWORDS, MGLContext_GeometryShader_doc},
	{"TessEvaluationShader", (PyCFunction)MGLContext_Shader<TESSELATION_EVALUATION_SHADER_SLOT>, METH_VARARGS | METH_KEYWORDS, MGLContext_TessEvaluationShader_doc},
	{"TessControlShader", (PyCFunction)MGLContext_Shader<TESSELATION_CONTROL_SHADER_SLOT>, METH_VARARGS | METH_KEYWORDS, MGLContext_TessControlShader_doc},
	{"Framebuffer", (PyCFunction)MGLContext_Framebuffer, METH_VARARGS | METH_KEYWORDS, MGLContext_Framebuffer_doc},
	{"Renderbuffer", (PyCFunction)MGLContext_Renderbuffer, METH_VARARGS | METH_KEYWORDS, MGLContext_Renderbuffer_doc},
	{"DepthRenderbuffer", (PyCFunction)MGLContext_DepthRenderbuffer, METH_VARARGS | METH_KEYWORDS, MGLContext_DepthRenderbuffer_doc},
	{"ComputeShader", (PyCFunction)MGLContext_ComputeShader, METH_VARARGS | METH_KEYWORDS, MGLContext_ComputeShader_doc},

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

char MGLContext_line_width_doc[] = R"(
	line_width

	Set the default line width.
)";

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

char MGLContext_point_size_doc[] = R"(
	point_size

	Set the default point size.
)";

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
	int size = (int)PyTuple_GET_SIZE(value);

	if (PyErr_Occurred()) {
		return -1;
	}

	if (size != 4) {
		MGLError * error = MGLError_New(TRACE, "viewport must be a tuple of size 4 not %d", size);
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return -1;
	}

	int x = PyLong_AsLong(PyTuple_GET_ITEM(value, 0));
	int y = PyLong_AsLong(PyTuple_GET_ITEM(value, 1));
	int width = PyLong_AsLong(PyTuple_GET_ITEM(value, 2));
	int height = PyLong_AsLong(PyTuple_GET_ITEM(value, 3));

	if (PyErr_Occurred()) {
		return -1;
	}

	self->gl.Viewport(x, y, width, height);

	return 0;
}

char MGLContext_viewport_doc[] = R"(
	viewport

	The viewport.
)";

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

char MGLContext_default_texture_unit_doc[] = R"(
	default_texture_unit

	The default texture unit.
)";

PyObject * MGLContext_get_max_texture_units(MGLContext * self) {
	return PyLong_FromLong(self->max_texture_units);
}

char MGLContext_max_texture_units_doc[] = R"(
	max_texture_units

	The max texture units.
)";

PyObject * MGLContext_get_default_framebuffer(MGLContext * self) {
	if (!self->default_framebuffer) {
		MGLFramebuffer * framebuffer = MGLFramebuffer_New();

		framebuffer->framebuffer_obj = 0;

		framebuffer->color_attachments = 0;
		framebuffer->depth_attachment = 0;

		// NO INCREF
		framebuffer->context = self;

		self->default_framebuffer = (PyObject *)framebuffer;
	}

	Py_INCREF(self->default_framebuffer);
	return self->default_framebuffer;
}

char MGLContext_default_framebuffer_doc[] = R"(
)";

PyObject * MGLContext_get_vendor(MGLContext * self, void * closure) {
	const char * vendor = (const char *)self->gl.GetString(GL_VENDOR);

	if (!vendor) {
		MGLError * error = MGLError_New(TRACE, "Missing vendor information");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	return PyUnicode_FromFormat("%s", vendor);

}

char MGLContext_vendor_doc[] = R"(
	vendor
)";

PyObject * MGLContext_get_renderer(MGLContext * self, void * closure) {
	const char * renderer = (const char *)self->gl.GetString(GL_RENDERER);

	if (!renderer) {
		MGLError * error = MGLError_New(TRACE, "Missing renderer information");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	return PyUnicode_FromFormat("%s", renderer);
}

char MGLContext_renderer_doc[] = R"(
	renderer
)";

PyObject * MGLContext_get_version(MGLContext * self, void * closure) {
	const char * version = (const char *)self->gl.GetString(GL_VERSION);

	if (!version) {
		MGLError * error = MGLError_New(TRACE, "Missing version information");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return 0;
	}

	return PyUnicode_FromFormat("%s", version);
}

char MGLContext_version_doc[] = R"(
	version
)";

PyGetSetDef MGLContext_tp_getseters[] = {
	{(char *)"line_width", (getter)MGLContext_get_line_width, (setter)MGLContext_set_line_width, MGLContext_line_width_doc, 0},
	{(char *)"point_size", (getter)MGLContext_get_point_size, (setter)MGLContext_set_point_size, MGLContext_point_size_doc, 0},
	{(char *)"viewport", (getter)MGLContext_get_viewport, (setter)MGLContext_set_viewport, MGLContext_viewport_doc, 0},
	{(char *)"default_texture_unit", (getter)MGLContext_get_default_texture_unit, (setter)MGLContext_set_default_texture_unit, MGLContext_default_texture_unit_doc, 0},
	{(char *)"max_texture_units", (getter)MGLContext_get_max_texture_units, 0, MGLContext_max_texture_units_doc, 0},
	{(char *)"default_framebuffer", (getter)MGLContext_get_default_framebuffer, 0, MGLContext_default_framebuffer_doc, 0},

	{(char *)"vendor", (getter)MGLContext_get_vendor, 0, MGLContext_vendor_doc, 0},
	{(char *)"renderer", (getter)MGLContext_get_renderer, 0, MGLContext_renderer_doc, 0},
	{(char *)"version", (getter)MGLContext_get_version, 0, MGLContext_version_doc, 0},
	{0},
};

const char * MGLContext_tp_doc = R"(
	Context

	Create a :py:class:`Context` using:

		- :py:func:`~ModernGL.create_context`
		- :py:func:`~ModernGL.create_standalone_context`

	Members:

		- :py:meth:`Context.Buffer`
		- :py:meth:`Context.DepthRenderbuffer`
		- :py:meth:`Context.DepthTexture`
		- :py:meth:`Context.Framebuffer`
		- :py:meth:`Context.Program`
		- :py:meth:`Context.Renderbuffer`
		- :py:meth:`Context.Shader`
		- :py:meth:`Context.SimpleVertexArray`
		- :py:meth:`Context.Texture`
		- :py:meth:`Context.VertexArray`
)";

PyTypeObject MGLContext_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"ModernGL.Context",                                     // tp_name
	sizeof(MGLContext),                                     // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLContext_tp_dealloc,                      // tp_dealloc
	0,                                                      // tp_print
	0,                                                      // tp_getattr
	0,                                                      // tp_setattr
	0,                                                      // tp_reserved
	(reprfunc)MGLContext_tp_str,                            // tp_repr
	0,                                                      // tp_as_number
	0,                                                      // tp_as_sequence
	0,                                                      // tp_as_mapping
	0,                                                      // tp_hash
	0,                                                      // tp_call
	(reprfunc)MGLContext_tp_str,                            // tp_str
	0,                                                      // tp_getattro
	0,                                                      // tp_setattro
	0,                                                      // tp_as_buffer
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,               // tp_flags
	MGLContext_tp_doc,                                      // tp_doc
	0,                                                      // tp_traverse
	0,                                                      // tp_clear
	0,                                                      // tp_richcompare
	0,                                                      // tp_weaklistoffset
	0,                                                      // tp_iter
	0,                                                      // tp_iternext
	MGLContext_tp_methods,                                  // tp_methods
	0,                                                      // tp_members
	MGLContext_tp_getseters,                                // tp_getset
	&MGLObject_Type,                                        // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLContext_tp_init,                           // tp_init
	0,                                                      // tp_alloc
	MGLContext_tp_new,                                      // tp_new
};

MGLContext * MGLContext_New() {
	MGLContext * self = (MGLContext *)MGLContext_tp_new(&MGLContext_Type, 0, 0);
	return self;
}

void MGLContext_Invalidate(MGLContext * context) {
	if (Py_TYPE(context) == &MGLInvalidObject_Type) {

		#ifdef MGL_VERBOSE
		printf("MGLContext_Invalidate %p already released\n", context);
		#endif

		return;
	}

	#ifdef MGL_VERBOSE
	printf("MGLContext_Invalidate %p\n", context);
	#endif

	DestroyGLContext(context->gl_context);

	Py_TYPE(context) = &MGLInvalidObject_Type;

	Py_DECREF(context);
}

void MGLContext_Initialize(MGLContext * self) {

	GLMethods & gl = self->gl;

	gl.load();

	gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// gl.Enable(GL_PRIMITIVE_RESTART_FIXED_INDEX);

	gl.Enable(GL_PRIMITIVE_RESTART);
	gl.PrimitiveRestartIndex(-1);

	gl.GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint *)&self->max_texture_units);
	self->default_texture_unit = self->max_texture_units - 1;
}
