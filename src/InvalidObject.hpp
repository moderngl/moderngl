#pragma once

#include "Python.hpp"

struct MGLInvalidObject {
	PyObject_HEAD
};

extern PyTypeObject MGLInvalidObject_Type;
