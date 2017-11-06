#pragma once

#include "Python.hpp"

#include "Context.hpp"
#include "TextureFilter.hpp"

struct MGLTexture {
	PyObject_HEAD

	MGLContext * context;

	union {
		int renderbuffer_obj;
		int texture_obj;
	};

	int width;
	int height;
	int components;

	int samples;

	bool floats;
	bool depth;

	MGLTextureFilter * filter;
	bool repeat_x;
	bool repeat_y;
};

extern PyTypeObject MGLTexture_Type;

void MGLTexture_Invalidate(MGLTexture * texture);
