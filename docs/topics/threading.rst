.. _threading:

Threads and Free-Threaded Python
================================

.. py:currentmodule:: moderngl

This page describes how ModernGL can be used from more than one thread.
It applies to free-threaded builds of Python (3.13t, 3.14t) and to regular
builds with the GIL alike. The ``moderngl.mgl`` extension module declares that it
does not need the GIL, importing ModernGL does not turn the GIL back on in a
free-threaded Python (the backend module of glcontext_ has to support it as well).

OpenGL Needs a Current Context
------------------------------

OpenGL calls always go to the context that is *current* on the calling thread. A context
can only be current on one thread at a time and ModernGL does not make it current for you.
Creating a context makes it current on the thread that created it. Use the context as a
context manager to make it current on another thread, and to give it up again::

    with ctx:
        fbo.use()
        vao.render()

Calling ModernGL from a thread without the context being current there
does nothing useful, the OpenGL calls are ignored or fail depending on the driver.

One Context per Thread
----------------------

The simplest and fastest way is one context for each thread that renders, with
everything created from it used only by that thread. Contexts do not share any state
and calls on different contexts run in parallel, also without the GIL::

    def render(index):
        ctx = moderngl.create_context(standalone=True)
        ...
        ctx.release()

    threads = [threading.Thread(target=render, args=(i,)) for i in range(4)]

Sharing One Context between Threads
-----------------------------------

A context and all the objects created from it (buffers, textures, framebuffers,
programs, vertex arrays ...) can be used from several threads. Every call is
serialized by a lock that belongs to the context, like it was by the GIL before:
a thread that calls a method while another thread is in a call on the same context
waits for it. Only one lock is used for everything that belongs to a context, so
calls on different contexts never wait for each other.
A call that waits for something itself, like the ones calling back into your code
(the ``to_shader_source()`` of a shader object in :py:meth:`Context.program`), lets the
other threads in meanwhile, as the GIL was released then.

This protects ModernGL, the objects and the state it keeps for the context (the bound
framebuffer, the enabled flags, the default texture unit) from being corrupted. It does not
make a sequence of calls atomic, and it does not make the context current. If threads
share a context, they need to hand the context over (``with ctx:`` while no other thread
has it) and to take turns using a lock of their own for every sequence that belongs together
(binding a framebuffer and rendering to it). Otherwise one thread can bind another
framebuffer between the two calls of another thread.

Garbage Collection
------------------

With ``gc_mode="auto"`` (see :ref:`gc`) the OpenGL object of a ModernGL object is released
by the thread that drops the last reference to it. Which one that is depends on the
program, and it is rarely the thread the context is current on. When more than one thread is used
set ``ctx.gc_mode = "context_gc"`` and call :py:meth:`Context.gc` on the thread
that has the context current. The objects that were dropped are collected until then.
``Context.gc()`` can be called from several threads.

The Default Context
-------------------

:py:func:`moderngl.create_context` stores the context it creates in a global variable
that :py:func:`moderngl.get_context` returns. This is shared by all threads
of the process: the last context created by any thread is the one that is returned.
This is meant for applications with one context. When contexts are created on more
than one thread keep the context objects and pass them around, do not use
``get_context()``.

.. _glcontext: https://github.com/moderngl/glcontext
