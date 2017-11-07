#pragma once

#include "Python.hpp"

#include "Context.hpp"

struct MGLBuffer {
	PyObject_HEAD

	MGLContext * context;

	int buffer_obj;

	Py_ssize_t size;
	bool dynamic;
};

extern PyTypeObject MGLBuffer_Type;

void MGLBuffer_Invalidate(MGLBuffer * buffer);
