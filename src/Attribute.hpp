#pragma once

#include "Python.hpp"

#include "Context.hpp"
#include "GLMethods.hpp"

struct MGLAttribute {
	PyObject_HEAD

	MGLContext * context;

	PyObject * name;

	void * gl_attrib_ptr_proc;

	int program_obj;

	int number;
	int location;
	int type;

	int scalar_type;

	int dimension;
	int array_length;

	int rows_length;
	int row_length;
	int row_size;

	char shape;
	bool normalizable;
};

extern PyTypeObject MGLAttribute_Type;

MGLAttribute * MGLAttribute_New();
void MGLAttribute_Invalidate(MGLAttribute * attribute);
void MGLAttribute_Complete(MGLAttribute * attribute, const GLMethods & gl);
