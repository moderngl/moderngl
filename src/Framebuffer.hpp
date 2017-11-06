#pragma once

#include "Python.hpp"

#include "Context.hpp"

struct MGLFramebuffer {
	PyObject_HEAD

	MGLContext * context;

	PyObject * color_attachments;
	PyObject * depth_attachment;

	bool * color_mask;

	unsigned * draw_buffers;
	int draw_buffers_len;

	int framebuffer_obj;

	int viewport_x;
	int viewport_y;
	int viewport_width;
	int viewport_height;

	int width;
	int height;
	int samples;

	bool depth_mask;
};

extern PyTypeObject MGLFramebuffer_Type;

MGLFramebuffer * MGLFramebuffer_New();
void MGLFramebuffer_Invalidate(MGLFramebuffer * framebuffer);
