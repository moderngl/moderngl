import pytest
import numpy as np
import moderngl

VERSION_CODE = 330
_ctx = None


@pytest.fixture(scope="session")
def ctx_static():
    """Session context"""
    return _create_context()


@pytest.fixture(scope="function")
def ctx():
    """
    Per function context.

    The same context is reused, but the context is cleaned before and after each test.
    """
    ctx = _create_context()
    _clean_ctx(ctx)
    yield ctx
    _clean_ctx(ctx)


def _create_context():
    global _ctx
    if _ctx is None:
        try:
            _ctx = moderngl.create_context(
                require=VERSION_CODE,
                standalone=True,
            )
        except Exception:
            _ctx = moderngl.create_context(
                require=VERSION_CODE,
                standalone=True,
                backend="egl",
            )
        _ctx.gc_mode = "auto"
        return _ctx
    return _ctx


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
