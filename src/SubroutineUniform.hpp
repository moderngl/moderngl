#pragma once

#include "Python.hpp"

struct MGLSubroutineUniform {
	PyObject_HEAD

	PyObject * name;
	int location;
};

extern PyTypeObject MGLSubroutineUniform_Type;

void MGLSubroutineUniform_Complete(MGLSubroutineUniform * subroutine_uniform);
