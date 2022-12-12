import moderngl
import pytest
import numpy as np

@pytest.fixture(scope='module')
def prog(ctx_static):
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

@pytest.fixture(scope='module')
def vao(ctx_static, prog):
    quad = [
        -1.0,  1.0,
        -1.0, -1.0,
        1.0, 1.0,
        1.0, -1.0,
    ]
    vbo = ctx_static.buffer(np.array(quad, dtype='f4'))
    return ctx_static.simple_vertex_array(prog, vbo, 'in_vert')


def create_fbo(ctx, size, components=3):
    return ctx.framebuffer(
        color_attachments=[ctx.texture(size, components)],
    )

def test_default_value(ctx):
    """Ensure default value of the scissor matches the framebuffer size"""
    size = (16, 32)
    fbo = create_fbo(ctx, size)
    assert fbo.scissor == (0, 0, *size)

def test_reset_scissor(ctx):
    """We should be able to reset scissor with `None`"""
    size = (16, 32)
    fbo = create_fbo(ctx, size)
    new_val = 2, 2, 14, 30

    fbo.scissor = new_val
    assert fbo.scissor == new_val

    fbo.scissor = None
    assert fbo.scissor == (0, 0, *size)

def test_render(ctx, prog, vao):
    """Render different color to the four corners of the fbo using scissor"""
    size = (4, 4)
    fbo = create_fbo(ctx, size)
    fbo.use()

    # upper left (red)
    fbo.scissor = 0, 2, 2, 2
    prog['color'].value = 1.0, 0.0, 0.0, 0.0
    vao.render(mode=moderngl.TRIANGLE_STRIP)

    # upper right (green)
    fbo.scissor = 2, 2, 2, 2
    prog['color'].value = 0.0, 1.0, 0.0, 0.0
    vao.render(mode=moderngl.TRIANGLE_STRIP)

    # lower left (green)
    fbo.scissor = 0, 0, 2, 2
    prog['color'].value = 0.0, 0.0, 1.0, 0.0
    vao.render(mode=moderngl.TRIANGLE_STRIP)

    # lower right (green)
    fbo.scissor = 2, 0, 2, 2
    prog['color'].value = 1.0, 1.0, 1.0, 1.0
    vao.render(mode=moderngl.TRIANGLE_STRIP)

    data = fbo.read()
    # Note that the data is flipped horizontally
    expected = (
        b'\x00\x00\xff'b'\x00\x00\xff'b'\xff\xff\xff'b'\xff\xff\xff'
        b'\x00\x00\xff'b'\x00\x00\xff'b'\xff\xff\xff'b'\xff\xff\xff'
        b'\xff\x00\x00'b'\xff\x00\x00'b'\x00\xff\x00'b'\x00\xff\x00'
        b'\xff\x00\x00'b'\xff\x00\x00'b'\x00\xff\x00'b'\x00\xff\x00'
    )
    assert data == expected

def test_clear(ctx):
    """Ensure fbo clearing works with scissoring"""
    size = (4, 4)
    fbo = create_fbo(ctx, size)
    fbo.use()

    # upper left (red)
    fbo.scissor = 0, 2, 2, 2
    fbo.clear(1.0, 0.0, 0.0, 0.0)

    # upper right (green)
    fbo.scissor = 2, 2, 2, 2
    fbo.clear(0.0, 1.0, 0.0, 0.0)

    # lower left (green)
    fbo.scissor = 0, 0, 2, 2
    fbo.clear(0.0, 0.0, 1.0, 0.0)

    # lower right (green)
    fbo.scissor = 2, 0, 2, 2
    fbo.clear(1.0, 1.0, 1.0, 1.0)

    data = fbo.read()
    # Note that the data is flipped horizontally
    expected = (
        b'\x00\x00\xff'b'\x00\x00\xff'b'\xff\xff\xff'b'\xff\xff\xff'
        b'\x00\x00\xff'b'\x00\x00\xff'b'\xff\xff\xff'b'\xff\xff\xff'
        b'\xff\x00\x00'b'\xff\x00\x00'b'\x00\xff\x00'b'\x00\xff\x00'
        b'\xff\x00\x00'b'\xff\x00\x00'b'\x00\xff\x00'b'\x00\xff\x00'
    )
    assert data == expected

def test_scissor_leak(ctx):
    """Make sure we don't leak scissor values to other framebuffers"""
    size = 2, 2
    fbo1 = create_fbo(ctx, size, components=4)
    fbo2 = create_fbo(ctx, size, components=4)

    fbo1.scissor = 0, 0, 1, 1
    fbo2.scissor = 1, 1, 1, 1

    fbo1.clear(color=(1.0, 0.0, 0.0, 0.0))
    fbo2.clear(color=(0.0, 1.0, 0.0, 0.0))

    assert fbo1.read() == b'\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
    assert fbo2.read() == b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\x00'

def test_clear_with_viewport(ctx):
    """Clearing with viewport take presence over scissor"""
    size = 2, 1
    fbo = create_fbo(ctx, size)
    fbo.scissor = 1, 0, 1, 1
    fbo.clear(color=(1.0, 1.0, 1.0, 1.0), viewport=(0, 0, 1, 1))
    assert fbo.read() == b'\xff\xff\xff\x00\x00\x00'
