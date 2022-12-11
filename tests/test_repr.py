import re
import pytest

def test_buffer_repr(ctx):
    buf = ctx.buffer(reserve=1024)
    assert re.search(r'<Buffer: \d+>', repr(buf))

def test_compute_shader_repr(ctx):
    if ctx.version_code < 430:
        pytest.skip('OpenGL 4.3 is not supported')

    compute_shader = ctx.compute_shader('''
        #version 430
        layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
        void main() {
        }
    ''')

    assert re.search(r'<ComputeShader: \d+>', repr(compute_shader))

def test_renderbuffer_and_framebuffer_repr(ctx):
    rbo = ctx.renderbuffer((16, 16))
    assert re.search(r'<Renderbuffer: \d+>', repr(rbo))

    fbo = ctx.simple_framebuffer((16, 19))
    assert re.search(r'<Framebuffer: \d+>', repr(fbo))

def test_texture_repr(ctx):
    texture = ctx.texture((4, 4), 3)
    assert re.search(r'<Texture: \d+>', repr(texture))

def test_program_repr(ctx):
    prog = ctx.program(
        vertex_shader="""
        #version 330
        in float value_1;
        out float value_2;
        void main() {
            value_2 = value_1;
        }
        """,
        varyings=['value_2'],
    )
    assert re.search(r'<Program: \d+>', repr(prog))
    repr(prog._members)
