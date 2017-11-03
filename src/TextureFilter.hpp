#pragma once

#include "Python.hpp"

#include "Object.hpp"

struct MGLTextureFilter : public MGLObject {
	PyObject * wrapper;

	int min_filter;
	int mag_filter;
};

extern PyTypeObject MGLTextureFilter_Type;

MGLTextureFilter * MGLTextureFilter_New();

extern MGLTextureFilter * MGL_LINEAR;
extern MGLTextureFilter * MGL_NEAREST;
extern MGLTextureFilter * MGL_LINEAR_MIPMAP;
extern MGLTextureFilter * MGL_NEAREST_MIPMAP;
