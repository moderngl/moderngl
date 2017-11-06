#pragma once

#include "Python.hpp"

#include "Context.hpp"

struct MGLComputeShader {
	PyObject_HEAD

	MGLContext * context;

	PyObject * source;

	PyObject * uniforms;
	PyObject * uniform_blocks;
	PyObject * subroutines;
	PyObject * subroutine_uniforms;

	int program_obj;
	int shader_obj;
};

extern PyTypeObject MGLComputeShader_Type;

MGLComputeShader * MGLComputeShader_New();
void MGLComputeShader_Invalidate(MGLComputeShader * program);
void MGLComputeShader_Compile(MGLComputeShader * compute_shader);
