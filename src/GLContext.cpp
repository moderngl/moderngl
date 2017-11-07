#include "GLContext.hpp"

#include "Types.hpp"

struct GLVersion {
	int major;
	int minor;
};

GLVersion version[] = {
	{4, 5},
	{4, 4},
	{4, 3},
	{4, 2},
	{4, 1},
	{4, 0},
	{3, 3},
	{3, 2},
	{3, 1},
	{0, 0},
};

int versions = sizeof(version) / sizeof(GLVersion);

#if defined(_WIN32) || defined(_WIN64)

#include <Windows.h>

#define WGL_ACCELERATION                0x2003
#define WGL_FULL_ACCELERATION           0x2027
#define WGL_CONTEXT_MAJOR_VERSION       0x2091
#define WGL_CONTEXT_MINOR_VERSION       0x2092
#define WGL_CONTEXT_PROFILE_MASK        0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT    0x0001

typedef int (WINAPI * mglChoosePixelFormatProc)(HDC hdc, const int * piAttribIList, const float * pfAttribFList, unsigned nMaxFormats, int * piFormats, unsigned * nNumFormats);
typedef HGLRC (WINAPI * mglCreateContextAttribsProc)(HDC hdc, HGLRC hglrc, const int * attribList);

mglChoosePixelFormatProc mglChoosePixelFormat;
mglCreateContextAttribsProc mglCreateContextAttribs;

PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR),  // nSize
	1,                              // nVersion
	PFD_DRAW_TO_WINDOW |
	PFD_SUPPORT_OPENGL |
	PFD_GENERIC_ACCELERATED |
	PFD_DOUBLEBUFFER,               // dwFlags
	0,                              // iPixelType
	32,                             // cColorBits
	0,                              // cRedBits
	0,                              // cRedShift
	0,                              // cGreenBits
	0,                              // cGreenShift
	0,                              // cBlueBits
	0,                              // cBlueShift
	0,                              // cAlphaBits
	0,                              // cAlphaShift
	0,                              // cAccumBits
	0,                              // cAccumRedBits
	0,                              // cAccumGreenBits
	0,                              // cAccumBlueBits
	0,                              // cAccumAlphaBits
	24,                             // cDepthBits
	0,                              // cStencilBits
	0,                              // cAuxBuffers
	0,                              // iLayerType
	0,                              // bReserved
	0,                              // dwLayerMask
	0,                              // dwVisibleMask
	0,                              // dwDamageMask
};

GLContext LoadCurrentGLContext() {
	GLContext context = {};

	HGLRC hrc = wglGetCurrentContext();

	if (!hrc) {
		MGLError_Set("cannot detect context");
		return context;
	}

	HDC hdc = wglGetCurrentDC();

	if (!hdc) {
		MGLError_Set("cannot detect device content");
		return context;
	}

	HWND hwnd = WindowFromDC(hdc);

	if (!hwnd) {
		MGLError_Set("cannot detect window");
		return context;
	}

	context.hwnd = (void *)hwnd;
	context.hdc = (void *)hdc;
	context.hrc = (void *)hrc;

	context.standalone = false;

	return context;
}

void InitModernContext() {
	static bool initialized = false;

	if (initialized) {
		return;
	}

	initialized = true;

	HMODULE hinst = GetModuleHandle(0);

	if (!hinst) {
		return;
	}

	WNDCLASS extClass = {
		CS_OWNDC,                       // style
		DefWindowProc,                  // lpfnWndProc
		0,                              // cbClsExtra
		0,                              // cbWndExtra
		hinst,                          // hInstance
		0,                              // hIcon
		0,                              // hCursor
		0,                              // hbrBackground
		0,                              // lpszMenuName
		"ContextLoader",                // lpszClassName
	};

	if (!RegisterClass(&extClass)) {
		return;
	}

	HWND loader_hwnd = CreateWindow(
		"ContextLoader",                // lpClassName
		0,                              // lpWindowName
		0,                              // dwStyle
		0,                              // x
		0,                              // y
		0,                              // nWidth
		0,                              // nHeight
		0,                              // hWndParent
		0,                              // hMenu
		hinst,                          // hInstance
		0                               // lpParam
	);

	if (!loader_hwnd) {
		return;
	}

	HDC loader_hdc = GetDC(loader_hwnd);

	if (!loader_hdc) {
		return;
	}

	int loader_pixelformat = ChoosePixelFormat(loader_hdc, &pfd);

	if (!loader_pixelformat) {
		return;
	}

	if (!SetPixelFormat(loader_hdc, loader_pixelformat, &pfd)) {
		return;
	}

	HGLRC loader_hglrc = wglCreateContext(loader_hdc);

	if (!loader_hglrc) {
		return;
	}

	if (!wglMakeCurrent(loader_hdc, loader_hglrc)) {
		return;
	}

	mglCreateContextAttribs = (mglCreateContextAttribsProc)wglGetProcAddress("wglCreateContextAttribsARB");

	if (!mglCreateContextAttribs) {
		return;
	}

	mglChoosePixelFormat = (mglChoosePixelFormatProc)wglGetProcAddress("wglChoosePixelFormatARB");

	if (!mglChoosePixelFormat) {
		return;
	}

	if (!wglMakeCurrent(0, 0)) {
		return;
	}

	if (!wglDeleteContext(loader_hglrc)) {
		return;
	}

	if (!ReleaseDC(loader_hwnd, loader_hdc)) {
		return;
	}

	if (!DestroyWindow(loader_hwnd)) {
		return;
	}

	if (!UnregisterClass("ContextLoader", hinst)) {
		return;
	}
}

GLContext CreateGLContext(int width, int height) {
	GLContext context = {};

	InitModernContext();

	HINSTANCE inst = GetModuleHandle(0);

	if (!inst) {
		MGLError_Set("module handle is null");
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
			"StandaloneContext",                 // lpszClassName
		};

		if (!RegisterClass(&wndClass)) {
			MGLError_Set("cannot register window class");
			return context;
		}

		registered = true;
	}

	HWND hwnd = CreateWindowEx(
		0,                                   // exStyle
		"StandaloneContext",                 // lpClassName
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
		MGLError_Set("cannot create window");
		return context;
	}

	HDC hdc = GetDC(hwnd);

	if (!hdc) {
		MGLError_Set("cannot create device content");
		return context;
	}

	HGLRC hrc = 0;

	if (mglCreateContextAttribs && mglChoosePixelFormat) {

		int pixelformat = 0;
		unsigned num_formats = 0;

		// WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB

		if (!mglChoosePixelFormat(hdc, 0, 0, 1, &pixelformat, &num_formats)) {
			MGLError_Set("cannot choose pixel format");
			return context;
		}

		if (!num_formats) {
			MGLError_Set("no pixel formats available");
			return context;
		}

		if (!SetPixelFormat(hdc, pixelformat, &pfd)) {
			MGLError_Set("cannot set pixel format");
			return context;
		}

		for (int i = 0; i < versions; ++i) {
			int attribs[] = {
				WGL_CONTEXT_PROFILE_MASK, WGL_CONTEXT_CORE_PROFILE_BIT,
				WGL_CONTEXT_MAJOR_VERSION, version[i].major,
				WGL_CONTEXT_MINOR_VERSION, version[i].minor,
				0, 0,
			};

			hrc = mglCreateContextAttribs(hdc, 0, attribs);

			if (hrc) {
				break;
			}
		}

	} else {

		int pf = ChoosePixelFormat(hdc, &pfd);

		if (!pf) {
			MGLError_Set("cannot choose pixel format");
			return context;
		}

		int set_pixel_format = SetPixelFormat(hdc, pf, &pfd);

		if (!set_pixel_format) {
			MGLError_Set("cannot set pixel format");
			return context;
		}

		hrc = wglCreateContext(hdc);

	}

	if (!hrc) {
		MGLError_Set("cannot create OpenGL context");
		return context;
	}

	int make_current = wglMakeCurrent(hdc, hrc);

	if (!make_current) {
		MGLError_Set("cannot select OpenGL context");
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

#include <OpenGL/OpenGL.h>
#include <ApplicationServices/ApplicationServices.h>

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

GLContext LoadCurrentGLContext() {
	GLContext context = {};

	CGLContextObj cgl_context = CGLGetCurrentContext();

	if (!cgl_context) {
		MGLError_Set("cannot detect OpenGL context");
		return context;
	}

	context.context = (void *)cgl_context;

	context.standalone = false;

	return context;
}

GLContext CreateGLContext(int width, int height) {
	GLContext context = {};

	// CGDirectDisplayID display = CGMainDisplayID();
	// CGOpenGLDisplayMask cglDisplayMask = CGDisplayIDToOpenGLDisplayMask(display);
	// CGLContextObj curr_ctx = CGLGetCurrentContext();

	GLint num_pixelformats = 0;
	CGLPixelFormatObj pixelformat = 0;

	// kCGLPFAAccelerated
	CGLPixelFormatAttribute attribs[] = {
		kCGLPFAOpenGLProfile,
		(CGLPixelFormatAttribute)kCGLOGLPVersion_GL4_Core,
		(CGLPixelFormatAttribute)0,
	};

	CGLChoosePixelFormat(attribs, &pixelformat, &num_pixelformats);

	if (!pixelformat) {

		CGLPixelFormatAttribute attribs[] = {
			kCGLPFAOpenGLProfile,
			(CGLPixelFormatAttribute)kCGLOGLPVersion_GL3_Core,
			(CGLPixelFormatAttribute)0,
		};

		CGLChoosePixelFormat(attribs, &pixelformat, &num_pixelformats);

		if (!pixelformat) {
			CGLPixelFormatAttribute attribs[] = {
				(CGLPixelFormatAttribute)0,
			};

			CGLChoosePixelFormat(attribs, &pixelformat, &num_pixelformats);
		}
	}

	if (!pixelformat) {
		MGLError_Set("cannot choose pixel format");
		return context;
	}

	CGLContextObj cgl_context = 0;

	CGLCreateContext(pixelformat, 0, &cgl_context);
	CGLDestroyPixelFormat(pixelformat);

	if (!cgl_context) {
		MGLError_Set("cannot create OpenGL context");
		return context;
	}

	CGLSetCurrentContext(cgl_context);

	int fbo = 0;
	int color_rbo = 0;
	int depth_rbo = 0;

	glGenFramebuffers(1, (GLuint *)&fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenRenderbuffers(1, (GLuint *)&color_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, color_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);

	glGenRenderbuffers(1, (GLuint *)&depth_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_RENDERBUFFER,
		color_rbo
	);

	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER,
		depth_rbo
	);

	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		const char * message = "the framebuffer is not complete";

		switch (status) {
			case GL_FRAMEBUFFER_UNDEFINED:
				message = "the framebuffer is not complete (UNDEFINED)";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				message = "the framebuffer is not complete (INCOMPLETE_ATTACHMENT)";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				message = "the framebuffer is not complete (INCOMPLETE_MISSING_ATTACHMENT)";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				message = "the framebuffer is not complete (INCOMPLETE_DRAW_BUFFER)";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				message = "the framebuffer is not complete (INCOMPLETE_READ_BUFFER)";
				break;

			case GL_FRAMEBUFFER_UNSUPPORTED:
				message = "the framebuffer is not complete (UNSUPPORTED)";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				message = "the framebuffer is not complete (INCOMPLETE_MULTISAMPLE)";
				break;
		}

		MGLError_Set(message);
		return context;
	}

	context.context = (void *)cgl_context;

	context.standalone = true;

	return context;
}

void DestroyGLContext(const GLContext & context) {
	if (!context.standalone) {
		return;
	}

	if (context.context) {
		CGLDestroyContext((CGLContextObj)context.context);
		// context.context = 0;
	}
}

#else

#include <GL/glx.h>
#include <GL/gl.h>

#define GLX_CONTEXT_MAJOR_VERSION 0x2091
#define GLX_CONTEXT_MINOR_VERSION 0x2092
#define GLX_CONTEXT_PROFILE_MASK 0x9126
#define GLX_CONTEXT_CORE_PROFILE_BIT 0x0001

typedef GLXContext (* GLXCREATECONTEXTATTRIBSARBPROC)(Display * display, GLXFBConfig config, GLXContext context, Bool direct, const int * attribs);

GLContext LoadCurrentGLContext() {
	GLContext context = {};

	Display * dpy = glXGetCurrentDisplay();

	if (!dpy) {
		MGLError_Set("cannot detect display");
		return context;
	}

	Window win = glXGetCurrentDrawable();

	if (!win) {
		MGLError_Set("cannot detect window");
		return context;
	}

	GLXContext ctx = glXGetCurrentContext();

	if (!ctx) {
		MGLError_Set("cannot detect OpenGL context");
		return context;
	}

	context.display = (void *)dpy;
	context.window = (void *)win;
	context.context = (void *)ctx;

	context.standalone = false;

	return context;
}

int SilentXErrorHandler(Display * d, XErrorEvent * e) {
    return 0;
}

GLContext CreateGLContext(int width, int height) {
	GLContext context = {};

	Display * dpy = XOpenDisplay(0);

	if (!dpy) {
		dpy = XOpenDisplay(":0.0");
	}

	if (!dpy) {
		MGLError_Set("cannot detect the display");
		return context;
	}

	int nelements = 0;

	GLXFBConfig * fbc = glXChooseFBConfig(dpy, DefaultScreen(dpy), 0, &nelements);

	if (!fbc) {
		MGLError_Set("cannot read the display configuration");
		XCloseDisplay(dpy);
		return context;
	}

	static int attributeList[] = {
		GLX_RGBA,
		GLX_DOUBLEBUFFER,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		None,
	};

	XVisualInfo * vi = glXChooseVisual(dpy, DefaultScreen(dpy), attributeList);

	if (!vi) {
		XCloseDisplay(dpy);

		MGLError_Set("cannot choose a visual info");
		return context;
	}

	XSetWindowAttributes swa;
	swa.colormap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask;

	Window win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, width, height, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);

	if (!win) {
		XCloseDisplay(dpy);

		MGLError_Set("cannot create window");
		return context;
	}

	// XMapWindow(dpy, win);

	GLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (GLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((const GLubyte *)"glXCreateContextAttribsARB");

	GLXContext ctx = 0;

	XSetErrorHandler(SilentXErrorHandler);

	if (glXCreateContextAttribsARB) {
		for (int i = 0; i < versions; ++i) {
			int attribs[] = {
				GLX_CONTEXT_PROFILE_MASK, GLX_CONTEXT_CORE_PROFILE_BIT,
				GLX_CONTEXT_MAJOR_VERSION, version[i].major,
				GLX_CONTEXT_MINOR_VERSION, version[i].minor,
				0, 0,
			};

			ctx = glXCreateContextAttribsARB(dpy, *fbc, 0, true, attribs);

			if (ctx) {
				break;
			}
		}
	}

	if (!ctx) {
		ctx = glXCreateContext(dpy, vi, 0, GL_TRUE);
	}

	if (!ctx) {
		XDestroyWindow(dpy, win);
		XCloseDisplay(dpy);

		MGLError_Set("cannot create OpenGL context");
		return context;
	}

	XSetErrorHandler(0);

	int make_current = glXMakeCurrent(dpy, win, ctx);

	if (!make_current) {
		glXDestroyContext(dpy, ctx);
		XDestroyWindow(dpy, win);
		XCloseDisplay(dpy);

		MGLError_Set("cannot select OpenGL context");
		return context;
	}

	context.display = (void *)dpy;
	context.window = (void *)win;
	context.context = (void *)ctx;

	context.standalone = true;

	return context;
}

void DestroyGLContext(const GLContext & context) {
	if (!context.standalone) {
		return;
	}

	if (context.display) {
		glXMakeCurrent((Display *)context.display, 0, 0);

		if (context.context) {
			glXDestroyContext((Display *)context.display, (GLXContext)context.context);
			// context.context = 0;
		}

		if (context.window) {
			XDestroyWindow((Display *)context.display, (Window)context.window);
			// context.window = 0;
		}

		XCloseDisplay((Display *)context.display);
		// context.display = 0;
	}
}

#endif
