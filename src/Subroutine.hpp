#pragma once

#include "Python.hpp"

struct MGLSubroutine {
	PyObject_HEAD

	PyObject * name;
	unsigned index;
};

extern PyTypeObject MGLSubroutine_Type;

MGLSubroutine * MGLSubroutine_New();
void MGLSubroutine_Invalidate(MGLSubroutine * subroutine);
void MGLSubroutine_Complete(MGLSubroutine * subroutine);
