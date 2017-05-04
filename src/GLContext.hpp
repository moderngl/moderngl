#pragma once

#if defined(_WIN32) || defined(_WIN64)

struct GLContext {
	void * hwnd;
	void * hrc;
	void * hdc;
	bool standalone;
};

#elif defined(__APPLE__)

struct GLContext {
	bool standalone;
};

#else

struct GLContext {
	void * display;
	void * window;
	void * context;
	bool standalone;
};

#endif

GLContext LoadCurrentGLContext();
GLContext CreateGLContext();
void DestroyGLContext(const GLContext & context);
