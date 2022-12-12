"""
Global fixtures for moderngl tests.

We rely on Context.gc_mode = "auto" to clean up resources.
Mainly a global context is used it test, but some test needs a new context

* ctx_static: A global context that is reused for all tests
* ctx: Also a global context, but it is cleaned before and after each test
* ctx_new: A new context for each test

Context creation can be refined in _create_context if issues arise
"""
import pytest
import numpy as np
import moderngl

VERSION_CODES = 430, 410, 330
_ctx = None
_fbo = None  # Fake framebuffer to avoid errors during transforms

@pytest.fixture(scope="session")
def ctx_static():
    """Session context"""
    return _get_context()


@pytest.fixture(scope="function")
def ctx():
    """
    Per function context.

    The same context is reused, but the context is cleaned before and after each test.
    """
    ctx = _get_context()
    error = ctx.error
    if error != "GL_NO_ERROR":
        raise RuntimeError("Context has error before use: {}".format(error))
    _clean_ctx(ctx)
    yield ctx
    error = ctx.error
    if error != "GL_NO_ERROR":
        raise RuntimeError("Context has error after use: {}".format(error))
    _clean_ctx(ctx)


@pytest.fixture(scope="function")
def ctx_new():
    """Returns a new context for each test"""
    ctx = _create_context()
    yield ctx
    ctx.release()


def _get_context():
    """Get the global test context"""
    global _ctx, _fbo
    if _ctx is None:
        _ctx = _create_context()
        _ctx.gc_mode = "auto"
        _fbo = _ctx.simple_framebuffer((2, 2))

    _ctx.__enter__()
    _fbo.use()
    return _ctx

def _create_context():
    """
    Create a new context.

    This is the only place context creation should happen.
    For now we just brute force context creation.
    """
    # Attempt standard standalone context
    try:
        for VERSION_CODE in VERSION_CODES:
            try:
                return moderngl.create_context(
                    require=VERSION_CODE,
                    standalone=True,
                )
            except Exception:
                pass
    except Exception:
        pass

    # Attempt EGL standalone context
    try:
        for VERSION_CODE in VERSION_CODES:
            return moderngl.create_context(
                require=VERSION_CODE,
                standalone=True,
                backend="egl",
            )
    except Exception:
        pass

    raise RuntimeError("Unable to create a context")

def _clean_ctx(ctx):
    """Clean the context"""
    # Reset the context
    ctx.blend_func = moderngl.DEFAULT_BLENDING
    ctx.blend_equation = moderngl.FUNC_ADD
    ctx.enable_only(moderngl.NOTHING)
    ctx.point_size = 1.0
    ctx.line_width = 1.0
    ctx.front_face = 'ccw'
    ctx.cull_face = 'back'
    ctx.wireframe = False
    ctx.provoking_vertex = moderngl.FIRST_VERTEX_CONVENTION
    ctx.polygon_offset = 0.0, 0.0    
    ctx.gc()


@pytest.fixture(scope="session")
def color_prog(ctx_static):
    """A simple program that renders a solid color."""
    return ctx_static.program(
        vertex_shader='''
            #version 330

            in vec2 in_vert;

            void main() {
                gl_Position = vec4(in_vert, 0.0, 1.0);
            }
        ''',
        fragment_shader='''
            #version 330

            out vec4 fragColor;
            uniform vec4 color;

            void main() {
                fragColor = color;
            }
        ''',
    )


@pytest.fixture(scope="session")
def ndc_quad(ctx_static):
    """Creates a buffer with an NDC quad."""
    quad = [
        -1.0,  1.0,
        -1.0, -1.0,
        1.0, 1.0,
        1.0, -1.0,
    ]
    return ctx_static.buffer(np.array(quad, dtype='f4'))
