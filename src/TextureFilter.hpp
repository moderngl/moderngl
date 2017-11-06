#pragma once

#include "Python.hpp"

struct MGLTextureFilter {
	PyObject_HEAD

	PyObject * wrapper;

	int min_filter;
	int mag_filter;
};

extern PyTypeObject MGLTextureFilter_Type;

MGLTextureFilter * MGLTextureFilter_New();

extern MGLTextureFilter * MGL_LINEAR;
extern MGLTextureFilter * MGL_NEAREST;
extern MGLTextureFilter * MGL_MIPMAP;
