#pragma once

#include "Python.hpp"

#include "Context.hpp"

enum SHADER_SLOT_ENUM {
	VERTEX_SHADER_SLOT,
	FRAGMENT_SHADER_SLOT,
	GEOMETRY_SHADER_SLOT,
	TESS_EVALUATION_SHADER_SLOT,
	TESS_CONTROL_SHADER_SLOT,
	NUM_SHADER_SLOTS,
};

extern const int SHADER_TYPE[];
extern const char * SHADER_NAME[];

struct MGLShader {
	PyObject_HEAD

	MGLContext * context;

	PyObject * source;

	int shader_obj;
	int shader_type;
	int shader_slot;
};

extern PyTypeObject MGLShader_Type;

void MGLShader_Invalidate(MGLShader * shader);
void MGLShader_Compile(MGLShader * shader);
