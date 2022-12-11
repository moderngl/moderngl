import struct
import pytest
import moderngl


@pytest.fixture(scope='module')
def prog(ctx_static):
    return ctx_static.program(
        vertex_shader='''
            #version 330

            in int a_in;
            in int b_in;
            in int c_in;
            in int d_in;

            out int a_out;
            out int b_out;
            out int c_out;
            out int d_out;

            void main() {
                a_out = a_in * 2;
                b_out = b_in * 2;
                c_out = c_in * 2;
                d_out = d_in * 2;
            }
        ''',
        varyings=['a_out', 'b_out', 'c_out', 'd_out']
    )

@pytest.fixture(scope='module', autouse=True)
def fbo(ctx_static):
    """Bind an fbo to avoid GL_INVALID_FRAMEBUFFER_OPERATION with transforms"""
    fbo = ctx_static.simple_framebuffer((2, 2))
    fbo.use()

def test_padding_1(ctx, prog):
    buf = ctx.buffer(struct.pack('=ixi12xii', 1, 2, 3, 4))
    res = ctx.buffer(reserve=16)

    vao = ctx.vertex_array(prog, [
        (buf, 'i x i 12x i i', 'a_in', 'b_in', 'c_in', 'd_in'),
    ])

    vao.transform(res, moderngl.POINTS)
    a, b, c, d = struct.unpack('=iiii', res.read())

    assert ctx.error == 'GL_NO_ERROR'

    assert a == 2
    assert b == 4
    assert c == 6
    assert d == 8

def test_padding_2(ctx, prog):
    buf = ctx.buffer(struct.pack('=i8xi8xi8xi8x', 1, 2, 3, 4))
    res = ctx.buffer(reserve=64)

    vao = ctx.vertex_array(prog, [
        (buf, 'i 8x', 'a_in'),
    ])

    vao.transform(res, moderngl.POINTS)
    assert vao.vertices == 4

    a1, a2, a3, a4 = struct.unpack('=i12xi12xi12xi12x', res.read())

    assert a1 == 2
    assert a2 == 4
    assert a3 == 6
    assert a4 == 8

def test_padding_3(ctx, prog):
    buf = ctx.buffer(struct.pack('=128xiiii', 1, 2, 3, 4))
    res = ctx.buffer(reserve=16)

    vao = ctx.vertex_array(prog, [
        (buf, '128x i i i i', 'a_in', 'b_in', 'c_in', 'd_in'),
    ])

    vao.transform(res, moderngl.POINTS)
    a, b, c, d = struct.unpack('=iiii', res.read())

    assert a == 2
    assert b == 4
    assert c == 6
    assert d == 8
