#pragma once

#include "Python.hpp"
#include "Context.hpp"

struct MGLRenderbuffer {
	PyObject_HEAD

	MGLContext * context;

	union {
		int renderbuffer_obj;
		int texture_obj;
	};

	int width;
	int height;
	int components;

	int samples;

	bool floats;
	bool depth;
};

extern PyTypeObject MGLRenderbuffer_Type;

MGLRenderbuffer * MGLRenderbuffer_New();
void MGLRenderbuffer_Invalidate(MGLRenderbuffer * renderbuffer);
