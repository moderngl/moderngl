#pragma once

#include "Python.hpp"

struct MGLVertexArrayListAttribute {
	PyObject_HEAD

	PyObject * content;
	int location;
};

extern PyTypeObject MGLVertexArrayListAttribute_Type;

MGLVertexArrayListAttribute * MGLVertexArrayListAttribute_New();
