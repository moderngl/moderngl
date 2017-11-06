#pragma once

#include "Python.hpp"

#include "GLMethods.hpp"

struct MGLUniformBlock {
	PyObject_HEAD

	const GLMethods * gl;

	PyObject * name;

	int program_obj;

	int index;
	int size;
};

extern PyTypeObject MGLUniformBlock_Type;

MGLUniformBlock * MGLUniformBlock_New();
void MGLUniformBlock_Complete(MGLUniformBlock * uniform_block, const GLMethods & gl);
