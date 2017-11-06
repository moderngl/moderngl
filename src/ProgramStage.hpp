#pragma once

#include "Python.hpp"

#include "GLMethods.hpp"

struct MGLProgramStage {
	PyObject_HEAD

	PyObject * subroutines;
	PyObject * subroutine_uniforms;
};

extern PyTypeObject MGLProgramStage_Type;

void MGLProgramStage_Complete(MGLProgramStage * program_stage, int shader_type, int program_obj, int & location_base, const GLMethods & gl);
