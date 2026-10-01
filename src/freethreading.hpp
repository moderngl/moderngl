#pragma once

#include <Python.h>

// Serializes the use of one OpenGL context for the lifetime of the variable.
// This is what the GIL did implicitly for the calls of the objects of a context.
// Critical sections exist only in free-threaded builds (3.13+), with the GIL (or on older
// Pythons) this is a no-op and compiles to nothing.
//
// A critical section is not a plain mutex: the same thread can enter it again for the same
// object (Scope.begin calls Framebuffer.use), and it is suspended while the thread blocks in
// a callback or waits for another critical section, like the GIL is released at those points.
#ifdef Py_GIL_DISABLED
struct ContextLock {
    PyCriticalSection section;

    explicit ContextLock(PyObject * context) {
        PyCriticalSection_Begin(&section, context);
    }

    ~ContextLock() {
        PyCriticalSection_End(&section);
    }

    ContextLock(const ContextLock &) = delete;
    ContextLock & operator=(const ContextLock &) = delete;
};
#else
struct ContextLock {
    explicit ContextLock(PyObject *) {}
};
#endif
