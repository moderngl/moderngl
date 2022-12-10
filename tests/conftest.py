import pytest
import numpy as np
import moderngl

VERSION_CODE = 330
_ctx = None

@pytest.fixture(scope="session")
def ctx():
    """Create a context with the latest version of OpenGL available."""
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

    # Reset the context
    _ctx.blend_func = moderngl.DEFAULT_BLENDING
    _ctx.blend_equation = moderngl.FUNC_ADD
    _ctx.enable_only(moderngl.NOTHING)
    _ctx.point_size = 1.0
    _ctx.line_width = 1.0
    _ctx.front_face = 'ccw'
    _ctx.cull_face = 'back'
    _ctx.wireframe = False
    _ctx.provoking_vertex = moderngl.FIRST_VERTEX_CONVENTION
    _ctx.polygon_offset = 0.0, 0.0    
    return _ctx


@pytest.fixture(scope="session")
def color_prog(ctx):
    """A simple program that renders a solid color."""
    return ctx.program(
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
def ndc_quad(ctx):
    """Creates a buffer with an NDC quad."""
    quad = [
        -1.0,  1.0,
        -1.0, -1.0,
        1.0, 1.0,
        1.0, -1.0,
    ]
    return ctx.buffer(np.array(quad, dtype='f4').tobytes())
