#pragma once

#include "Python.hpp"

#include "Context.hpp"
#include "Program.hpp"
#include "Buffer.hpp"

struct MGLVertexArray {
	PyObject_HEAD

	MGLContext * context;

	MGLProgram * program;
	MGLBuffer * index_buffer;
	PyObject * attributes;

	unsigned * subroutines;
	int num_subroutines;

	int vertex_array_obj;
	int num_vertices;
};

extern PyTypeObject MGLVertexArray_Type;

void MGLVertexArray_Invalidate(MGLVertexArray * vertex_array);
void MGLVertexArray_Complete(MGLVertexArray * vertex_array);
