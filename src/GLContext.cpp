#include "GLContext.hpp"

#include "Error.hpp"

#if defined(_WIN32) || defined(_WIN64)

#include <Windows.h>

GLContext LoadCurrentGLContext() {
	GLContext context = {};

	HGLRC hrc = wglGetCurrentContext();
	HDC hdc = wglGetCurrentDC();

	if (!hrc || !hdc) {
		MGLError * error = MGLError_New(TRACE, "Cannot detect current context.");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return context;
	}
	
	HWND hwnd = WindowFromDC(hdc);

	context.hwnd = (void *)hwnd;
	context.hdc = (void *)hdc;
	context.hrc = (void *)hrc;
	context.standalone = false;

	return context;
}

GLContext CreateGLContext() {
	GLContext context = {};

	HINSTANCE inst = GetModuleHandle(0);

	if (!inst) {
		MGLError * error = MGLError_New(TRACE, "Module handle is null");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return context;
	}

	static bool registered = false;

	if (!registered) {
		WNDCLASS wndClass = {
			CS_OWNDC,                            // style
			DefWindowProc,                       // lpfnWndProc
			0,                                   // cbClsExtra
			0,                                   // cbWndExtra
			inst,                                // hInstance
			0,                                   // hIcon
			0,                                   // hCursor
			0,                                   // hbrBackground
			0,                                   // lpszMenuName
			"standalone_context_class",          // lpszClassName
		};

		if (!RegisterClass(&wndClass)) {
			MGLError * error = MGLError_New(TRACE, "Cannot register window class");
			PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
			return context;
		}

		registered = true;
	}

	HWND hwnd = CreateWindowEx(
		0,                                   // exStyle
		"standalone_context_class",          // lpClassName
		0,                                   // lpWindowName
		0,                                   // dwStyle
		0,                                   // x
		0,                                   // y
		0,                                   // nWidth
		0,                                   // nHeight
		0,                                   // hWndParent
		0,                                   // hMenu
		inst,                                // hInstance
		0                                    // lpParam
	);

	if (!hwnd) {
		MGLError * error = MGLError_New(TRACE, "Cannot create window");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return context;
	}

	HDC hdc = GetDC(hwnd);

	if (!hdc) {
		MGLError * error = MGLError_New(TRACE, "Cannot create device content");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return context;
	}

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),   // nSize
		1,                               // nVersion
		PFD_DRAW_TO_WINDOW |
		PFD_GENERIC_ACCELERATED |
		PFD_SUPPORT_OPENGL,              // dwFlags
		PFD_TYPE_RGBA,                   // iPixelType
		32,                              // cColorBits
		0,                               // cRedBits
		0,                               // cRedShift
		0,                               // cGreenBits
		0,                               // cGreenShift
		0,                               // cBlueBits
		0,                               // cBlueShift
		0,                               // cAlphaBits
		0,                               // cAlphaShift
		0,                               // cAccumBits
		0,                               // cAccumRedBits
		0,                               // cAccumGreenBits
		0,                               // cAccumBlueBits
		0,                               // cAccumAlphaBits
		24,                              // cDepthBits
		0,                               // cStencilBits
		0,                               // cAuxBuffers
		0,                               // iLayerType
		0,                               // bReserved
		0,                               // dwLayerMask
		0,                               // dwVisibleMask
		0,                               // dwDamageMask
	};

	int pf = ChoosePixelFormat(hdc, &pfd);

	if (!pf) {
		MGLError * error = MGLError_New(TRACE, "Cannot choose pixel format");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return context;
	}

	int set_pixel_format = SetPixelFormat(hdc, pf, &pfd);

	if (!set_pixel_format) {
		MGLError * error = MGLError_New(TRACE, "Cannot set pixel format");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return context;
	}

	HGLRC hrc = wglCreateContext(hdc);

	if (!hrc) {
		MGLError * error = MGLError_New(TRACE, "Cannot create OpenGL context");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return context;
	}

	int make_current = wglMakeCurrent(hdc, hrc);

	if (!make_current) {
		MGLError * error = MGLError_New(TRACE, "Cannot select OpenGL context");
		PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
		return context;
	}

	context.hwnd = (void *)hwnd;
	context.hrc = (void *)hrc;
	context.hdc = (void *)hdc;
	context.standalone = true;

	return context;
}

void DestroyGLContext(const GLContext & context) {
	if (!context.standalone) {
		return;
	}

	if (wglGetCurrentContext() == context.hrc) {
		wglMakeCurrent(0, 0);
	}

	wglDeleteContext((HGLRC)context.hrc);
	ReleaseDC((HWND)context.hwnd, (HDC)context.hdc);
	DestroyWindow((HWND)context.hwnd);
}

#elif defined(__APPLE__)

#include <OpenGL/gl.h>

GLContext LoadCurrentGLContext() {
	GLContext context = {};
	return context;
}

GLContext CreateGLContext() {
	GLContext context = {};
	return context;
}

void DestroyGLContext(const GLContext & context) {
	// TODO:
}

#else

#include <GL/glx.h>
#include <GL/gl.h>

#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092

typedef GLXContext (* GLXCREATECONTEXTATTRIBSARBPROC)(Display * display, GLXFBConfig config, GLXContext context, Bool direct, const int * attribs);

GLContext LoadCurrentGLContext() {
	GLContext context = {};
	return context;
}

GLContext CreateGLContext() {
	GLContext context = {};

	Display * dpy = XOpenDisplay(0);

	int nelements = 0;
	GLXFBConfig * fbc = glXChooseFBConfig(dpy, DefaultScreen(dpy), 0, &nelements);

	static int attributeList[] = {GLX_RGBA, GLX_DOUBLEBUFFER, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1, None};
	XVisualInfo * vi = glXChooseVisual(dpy, DefaultScreen(dpy), attributeList);

	XSetWindowAttributes swa;
	swa.colormap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask;
	Window win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, 100, 100, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
	// XMapWindow(dpy, win);

	// TODO: keep this as a plan B
	// oldstyle context:
	// GLXContext ctx = glXCreateContext(dpy, vi, 0, GL_TRUE);

	GLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (GLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((const GLubyte *)"glXCreateContextAttribsARB");

	// TODO: 4.5 -> 3.0 or old context

	int attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 3,
		0,
	};

	GLXContext ctx = glXCreateContextAttribsARB(dpy, *fbc, 0, true, attribs);
	glXMakeCurrent(dpy, win, ctx);

	context.display = (void *)dpy;
	context.window = (void *)win;
	context.context = (void *)ctx;

	return context;
}

void DestroyGLContext(const GLContext & context) {
	// TODO:
}

#endif
