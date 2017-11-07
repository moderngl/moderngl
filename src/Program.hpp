#pragma once

#include "Python.hpp"

#include "Context.hpp"
#include "Primitive.hpp"

struct MGLProgramStage;

struct MGLProgram {
	PyObject_HEAD

	MGLContext * context;

	PyObject * shaders;

	MGLProgramStage * vertex_shader;
	MGLProgramStage * fragment_shader;
	MGLProgramStage * geometry_shader;
	MGLProgramStage * tess_evaluation_shader;
	MGLProgramStage * tess_control_shader;

	PyObject * uniforms;
	PyObject * uniform_blocks;
	PyObject * attributes;
	PyObject * varyings;

	MGLPrimitive * geometry_input;
	MGLPrimitive * geometry_output;

	int program_obj;

	int num_vertex_shader_subroutines;
	int num_fragment_shader_subroutines;
	int num_geometry_shader_subroutines;
	int num_tess_evaluation_shader_subroutines;
	int num_tess_control_shader_subroutines;

	int geometry_vertices;
	int num_varyings;
};

extern PyTypeObject MGLProgram_Type;

void MGLProgram_Invalidate(MGLProgram * program);
void MGLProgram_Compile(MGLProgram * program, PyObject * outputs);
