#include "Buffer.hpp"

#include "Error.hpp"
#include "InvalidObject.hpp"
#include "BufferAccess.hpp"

#include "UniformBlock.hpp"

PyObject * MGLBuffer_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLBuffer * self = (MGLBuffer *)type->tp_alloc(type, 0);

	#ifdef MGL_VERBOSE
	printf("MGLBuffer_tp_new %p\n", self);
	#endif

	if (self) {
	}

	return (PyObject *)self;
}

void MGLBuffer_tp_dealloc(MGLBuffer * self) {

	#ifdef MGL_VERBOSE
	printf("MGLBuffer_tp_dealloc %p\n", self);
	#endif

	MGLBuffer_Type.tp_free((PyObject *)self);
}

int MGLBuffer_tp_init(MGLBuffer * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.Buffer manually");
	return -1;
}

MGLBufferAccess * MGLBuffer_access(MGLBuffer * self, PyObject * args) {
	Py_ssize_t size;
	Py_ssize_t offset;
	int readonly;

	int args_ok = PyArg_ParseTuple(
		args,
		"nnp",
		&size,
		&offset,
		&readonly
	);

	if (!args_ok) {
		return 0;
	}

	if (size < 0) {
		size = self->size - offset;
	}

	if (offset < 0 || offset + size > self->size) {
		MGLError_Set("out of range offset = %d or size = %d", offset, size);
		return 0;
	}

	MGLBufferAccess * access = MGLBufferAccess_New();

	access->gl = &self->context->gl;
	access->ptr = 0;

	access->buffer_obj = self->buffer_obj;
	access->offset = offset;
	access->size = size;
	access->access = readonly ? GL_MAP_READ_BIT : (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);

	return access;
}

PyObject * MGLBuffer_read(MGLBuffer * self, PyObject * args) {
	Py_ssize_t size;
	Py_ssize_t offset;

	int args_ok = PyArg_ParseTuple(
		args,
		"nn",
		&size,
		&offset
	);

	if (!args_ok) {
		return 0;
	}

	if (size < 0) {
		size = self->size - offset;
	}

	if (offset < 0 || offset + size > self->size) {
		MGLError_Set("out of rangeoffset = %d or size = %d", offset, size);
		return 0;
	}

	const GLMethods & gl = self->context->gl;

	gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);
	void * map = gl.MapBufferRange(GL_ARRAY_BUFFER, offset, size, GL_MAP_READ_BIT);

	if (!map) {
		MGLError_Set("cannot map the buffer");
		return 0;
	}

	PyObject * data = PyBytes_FromStringAndSize((const char *)map, size);

	gl.UnmapBuffer(GL_ARRAY_BUFFER);

	return data;
}

PyObject * MGLBuffer_read_into(MGLBuffer * self, PyObject * args) {
	PyObject * data;
	Py_ssize_t size;
	Py_ssize_t offset;
	Py_ssize_t write_offset;

	int args_ok = PyArg_ParseTuple(
		args,
		"Onnn",
		&data,
		&size,
		&offset,
		&write_offset
	);

	if (!args_ok) {
		return 0;
	}

	if (size < 0) {
		size = self->size - offset;
	}

	if (offset < 0 || write_offset < 0 || offset + size > self->size) {
		MGLError_Set("out of range");
		return 0;
	}

	Py_buffer buffer_view;

	int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_WRITABLE);
	if (get_buffer < 0) {
		MGLError_Set("the buffer (%s) does not support buffer interface", Py_TYPE(data)->tp_name);
		return 0;
	}

	if (buffer_view.len < write_offset + size) {
		MGLError_Set("the buffer is too small");
		PyBuffer_Release(&buffer_view);
		return 0;
	}

	const GLMethods & gl = self->context->gl;

	gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);
	void * map = gl.MapBufferRange(GL_ARRAY_BUFFER, offset, size, GL_MAP_READ_BIT);

	char * ptr = (char *)buffer_view.buf + write_offset;
	memcpy(ptr, map, size);

	gl.UnmapBuffer(GL_ARRAY_BUFFER);

	PyBuffer_Release(&buffer_view);
	Py_RETURN_NONE;
}

PyObject * MGLBuffer_write(MGLBuffer * self, PyObject * args) {
	PyObject * data;
	Py_ssize_t offset;

	int args_ok = PyArg_ParseTuple(
		args,
		"On",
		&data,
		&offset
	);

	if (!args_ok) {
		return 0;
	}

	Py_buffer buffer_view;

	int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
	if (get_buffer < 0) {
		MGLError_Set("data (%s) does not support buffer interface", Py_TYPE(data)->tp_name);
		return 0;
	}

	if (offset < 0 || buffer_view.len + offset > self->size) {
		MGLError_Set("out of range offset = %d or size = %d", offset, buffer_view.len);
		PyBuffer_Release(&buffer_view);
		return 0;
	}

	const GLMethods & gl = self->context->gl;
	gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);
	gl.BufferSubData(GL_ARRAY_BUFFER, (GLintptr)offset, buffer_view.len, buffer_view.buf);
	PyBuffer_Release(&buffer_view);
	Py_RETURN_NONE;
}

PyObject * MGLBuffer_write_slice(MGLBuffer * self, PyObject * args) {
	PyObject * data;
	PyObject * slice;

	int args_ok = PyArg_ParseTuple(
		args,
		"OO",
		&data,
		&slice
	);

	if (!args_ok) {
		return 0;
	}

	Py_buffer buffer_view;

	int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
	if (get_buffer < 0) {
		MGLError_Set("data (%s) does not support buffer interface", Py_TYPE(data)->tp_name);
		return 0;
	}

	Py_ssize_t start;
	Py_ssize_t end;
	Py_ssize_t step;

	int get_indices = PySlice_GetIndices(slice, self->size, &start, &end, &step);
	if (get_indices < 0) {
		MGLError_Set("slice is not valid");
		PyBuffer_Release(&buffer_view);
		return 0;
	}

	const GLMethods & gl = self->context->gl;
	gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);

	Py_ssize_t one = step < 0 ? -1 : 1;
	Py_ssize_t count = (end - start + step - one) / step;
	Py_ssize_t chunk_size = buffer_view.len / count;

	if (buffer_view.len != chunk_size * count || chunk_size > step * one) {
		MGLError_Set("data (%d bytes) cannot be divided to %d equal chunks", buffer_view.len, count);
		PyBuffer_Release(&buffer_view);
		return 0;
	}

	if (start + chunk_size > self->size || start + count * step - step + chunk_size > self->size) {
		MGLError_Set("buffer overflow");
		PyBuffer_Release(&buffer_view);
		return 0;
	}

	char * write_ptr = (char *)gl.MapBufferRange(GL_ARRAY_BUFFER, 0, self->size, GL_MAP_WRITE_BIT);
	char * read_ptr = (char *)buffer_view.buf;

	if (!write_ptr) {
		MGLError_Set("cannot map the buffer");
		PyBuffer_Release(&buffer_view);
		return 0;
	}

	write_ptr += start;
	for (Py_ssize_t i = 0; i < count; ++i) {
		memcpy(write_ptr, read_ptr, chunk_size);
		read_ptr += chunk_size;
		write_ptr += step;
	}

	gl.UnmapBuffer(GL_ARRAY_BUFFER);
	PyBuffer_Release(&buffer_view);
	Py_RETURN_NONE;
}

PyObject * MGLBuffer_clear(MGLBuffer * self, PyObject * args) {
	Py_ssize_t size;
	Py_ssize_t offset;
	PyObject * chunk;

	int args_ok = PyArg_ParseTuple(
		args,
		"nnO",
		&size,
		&offset,
		&chunk
	);

	if (!args_ok) {
		return 0;
	}

	if (size < 0) {
		size = self->size - offset;
	}

	Py_buffer buffer_view;

	if (chunk != Py_None) {
		int get_buffer = PyObject_GetBuffer(chunk, &buffer_view, PyBUF_SIMPLE);
		if (get_buffer < 0) {
			MGLError_Set("the chunk (%s) does not support buffer interface", Py_TYPE(chunk)->tp_name);
			return 0;
		}

		if (size % buffer_view.len != 0) {
			MGLError_Set("the chunk does not fit the size");
			PyBuffer_Release(&buffer_view);
			return 0;
		}
	} else {
		buffer_view.len = 0;
		buffer_view.buf = 0;
	}

	const GLMethods & gl = self->context->gl;
	gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);

	char * map = (char *)gl.MapBufferRange(GL_ARRAY_BUFFER, offset, size, GL_MAP_WRITE_BIT);

	if (!map) {
		MGLError_Set("cannot map the buffer");
		return 0;
	}

	if (buffer_view.len) {
		char * src = (char *)buffer_view.buf;
		Py_ssize_t divisor = buffer_view.len;

		for (Py_ssize_t i = 0; i < size; ++i) {
			map[i] = src[i % divisor];
		}
	} else {
		memset(map + offset, 0, size);
	}

	gl.UnmapBuffer(GL_ARRAY_BUFFER);

	if (chunk != Py_None) {
		PyBuffer_Release(&buffer_view);
	}

	Py_RETURN_NONE;
}

PyObject * MGLBuffer_orphan(MGLBuffer * self) {
	const GLMethods & gl = self->context->gl;
	gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);
	gl.BufferData(GL_ARRAY_BUFFER, self->size, 0, self->dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	Py_RETURN_NONE;
}

PyObject * MGLBuffer_bind_to_uniform_block(MGLBuffer * self, PyObject * args) {
	int binding;

	int args_ok = PyArg_ParseTuple(
		args,
		"I",
		&binding
	);

	if (!args_ok) {
		return 0;
	}

	const GLMethods & gl = self->context->gl;
	gl.BindBufferBase(GL_UNIFORM_BUFFER, binding, self->buffer_obj);
	Py_RETURN_NONE;
}

PyObject * MGLBuffer_bind_to_storage_buffer(MGLBuffer * self, PyObject * args) {
	int binding;

	int args_ok = PyArg_ParseTuple(
		args,
		"I",
		&binding
	);

	if (!args_ok) {
		return 0;
	}

	const GLMethods & gl = self->context->gl;
	gl.BindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, self->buffer_obj);
	Py_RETURN_NONE;
}

PyObject * MGLBuffer_release(MGLBuffer * self) {
	MGLBuffer_Invalidate(self);
	Py_RETURN_NONE;
}

PyMethodDef MGLBuffer_tp_methods[] = {
	{"access", (PyCFunction)MGLBuffer_access, METH_VARARGS, 0},
	{"read", (PyCFunction)MGLBuffer_read, METH_VARARGS, 0},
	{"read_into", (PyCFunction)MGLBuffer_read_into, METH_VARARGS, 0},
	{"write", (PyCFunction)MGLBuffer_write, METH_VARARGS, 0},
	{"write_slice", (PyCFunction)MGLBuffer_write_slice, METH_VARARGS, 0},
	{"clear", (PyCFunction)MGLBuffer_clear, METH_VARARGS, 0},
	{"orphan", (PyCFunction)MGLBuffer_orphan, METH_NOARGS, 0},
	{"bind_to_uniform_block", (PyCFunction)MGLBuffer_bind_to_uniform_block, METH_VARARGS, 0},
	{"bind_to_storage_buffer", (PyCFunction)MGLBuffer_bind_to_storage_buffer, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLBuffer_release, METH_NOARGS, 0},
	{0},
};

PyObject * MGLBuffer_get_size(MGLBuffer * self, void * closure) {
	return PyLong_FromSsize_t(self->size);
}

PyObject * MGLBuffer_get_dynamic(MGLBuffer * self, void * closure) {
	return PyBool_FromLong(self->dynamic);
}

MGLContext * MGLBuffer_get_context(MGLBuffer * self, void * closure) {
	Py_INCREF(self->context);
	return self->context;
}

PyObject * MGLBuffer_get_glo(MGLBuffer * self, void * closure) {
	return PyLong_FromLong(self->buffer_obj);
}

PyGetSetDef MGLBuffer_tp_getseters[] = {
	{(char *)"size", (getter)MGLBuffer_get_size, 0, 0, 0},
	{(char *)"dynamic", (getter)MGLBuffer_get_dynamic, 0, 0, 0},
	{(char *)"context", (getter)MGLBuffer_get_context, 0, 0, 0},
	{(char *)"glo", (getter)MGLBuffer_get_glo, 0, 0, 0},
	{0},
};

int MGLBuffer_tp_as_buffer_get_view(MGLBuffer * self, Py_buffer * view, int flags) {
	int access = (flags == PyBUF_SIMPLE) ? GL_MAP_READ_BIT : (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);

	const GLMethods & gl = self->context->gl;
	gl.BindBuffer(GL_ARRAY_BUFFER, self->buffer_obj);
	void * map = gl.MapBufferRange(GL_ARRAY_BUFFER, 0, self->size, access);

	if (!map) {
		PyErr_Format(PyExc_BufferError, "Cannot map buffer");
		view->obj = 0;
		return -1;
	}

	view->buf = map;
	view->len = self->size;
	view->itemsize = 1;

	view->format = 0;
	view->ndim = 0;
	view->shape = 0;
	view->strides = 0;
	view->suboffsets = 0;

	Py_INCREF(self);
	view->obj = (PyObject *)self;
	return 0;
}

void MGLBuffer_tp_as_buffer_release_view(MGLBuffer * self, Py_buffer * view) {
	const GLMethods & gl = self->context->gl;
	gl.UnmapBuffer(GL_ARRAY_BUFFER);
}

PyBufferProcs MGLBuffer_tp_as_buffer = {
	(getbufferproc)MGLBuffer_tp_as_buffer_get_view,                  // getbufferproc bf_getbuffer
	(releasebufferproc)MGLBuffer_tp_as_buffer_release_view,          // releasebufferproc bf_releasebuffer
};

PyTypeObject MGLBuffer_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.Buffer",                                           // tp_name
	sizeof(MGLBuffer),                                      // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLBuffer_tp_dealloc,                       // tp_dealloc
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
	&MGLBuffer_tp_as_buffer,                                // tp_as_buffer
	Py_TPFLAGS_DEFAULT,                                     // tp_flags
	0,                                                      // tp_doc
	0,                                                      // tp_traverse
	0,                                                      // tp_clear
	0,                                                      // tp_richcompare
	0,                                                      // tp_weaklistoffset
	0,                                                      // tp_iter
	0,                                                      // tp_iternext
	MGLBuffer_tp_methods,                                   // tp_methods
	0,                                                      // tp_members
	MGLBuffer_tp_getseters,                                 // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLBuffer_tp_init,                            // tp_init
	0,                                                      // tp_alloc
	MGLBuffer_tp_new,                                       // tp_new
};

MGLBuffer * MGLBuffer_New() {
	MGLBuffer * self = (MGLBuffer *)MGLBuffer_tp_new(&MGLBuffer_Type, 0, 0);
	return self;
}

void MGLBuffer_Invalidate(MGLBuffer * buffer) {
	if (Py_TYPE(buffer) == &MGLInvalidObject_Type) {

		#ifdef MGL_VERBOSE
		printf("MGLBuffer_Invalidate %p already released\n", buffer);
		#endif

		return;
	}

	#ifdef MGL_VERBOSE
	printf("MGLBuffer_Invalidate %p\n", buffer);
	#endif

	const GLMethods & gl = buffer->context->gl;
	gl.DeleteBuffers(1, (GLuint *)&buffer->buffer_obj);

	Py_DECREF(buffer->context);

	Py_TYPE(buffer) = &MGLInvalidObject_Type;

	Py_DECREF(buffer);
}
