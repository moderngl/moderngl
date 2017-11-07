#pragma once

#include "Python.hpp"

#include "GLMethods.hpp"

struct MGLVarying {
	PyObject_HEAD

	PyObject * name;

	int number;
	int type;

	int array_length;

	bool matrix;
};

extern PyTypeObject MGLVarying_Type;

void MGLVarying_Invalidate(MGLVarying * varying);
void MGLVarying_Complete(MGLVarying * varying, const GLMethods & gl);
