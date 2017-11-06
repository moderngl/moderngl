#pragma once

#include "Python.hpp"

struct MGLPrimitive {
	PyObject_HEAD
	PyObject * wrapper;

	int primitive;
	int geometry_primitive;
	int transform_primitive;
};

extern PyTypeObject MGLPrimitive_Type;

MGLPrimitive * MGLPrimitive_New();

extern MGLPrimitive * MGL_TRIANGLES;
extern MGLPrimitive * MGL_TRIANGLE_STRIP;
extern MGLPrimitive * MGL_TRIANGLE_FAN;
extern MGLPrimitive * MGL_LINES;
extern MGLPrimitive * MGL_LINE_STRIP;
extern MGLPrimitive * MGL_LINE_LOOP;
extern MGLPrimitive * MGL_POINTS;
extern MGLPrimitive * MGL_LINE_STRIP_ADJACENCY;
extern MGLPrimitive * MGL_LINES_ADJACENCY;
extern MGLPrimitive * MGL_TRIANGLE_STRIP_ADJACENCY;
extern MGLPrimitive * MGL_TRIANGLES_ADJACENCY;
extern MGLPrimitive * MGL_NO_PRIMITIVE;
