import struct
import pytest

# NOTE: This is only needed for older version of mesa/llvmpipe
# if 'llvmpipe' in cls.ctx.info['GL_RENDERER']:
#     raise unittest.SkipTest('Temp disable for llvmpipe')

@pytest.fixture(scope='module', autouse=True)
def prog(ctx_static):
    return ctx_static.program(
        vertex_shader='''
            #version 330

            in vec2 in_v;
            out vec2 out_v;

            uniform Block1 {
                float x;
            };

            uniform Block2 {
                float y;
            };

            uniform Block3 {
                float z;
            };

            void main() {
                out_v = in_v * z + vec2(x, y);
            }
        ''',
        varyings=['out_v']
    )

def test_1(ctx, prog):
    buf_v = ctx.buffer(struct.pack('2f', 100.0, 1000.0))
    buf_u1 = ctx.buffer(struct.pack('f', 9.5))
    buf_u2 = ctx.buffer(struct.pack('f', 4.0))
    buf_u3 = ctx.buffer(struct.pack('f', 3.0))
    buf_u4 = ctx.buffer(struct.pack('f', 0.0))
    buf_r = ctx.buffer(reserve=buf_v.size)

    vao = ctx.vertex_array(prog, [
        (buf_v, '2f', 'in_v'),
    ])

    assert 'Block1' in prog
    assert 'Block2' in prog
    assert 'Block3' in prog

    assert 'x' not in prog
    assert 'y' not in prog
    assert 'z' not in prog

    prog['Block1'].binding = 2
    prog['Block2'].binding = 4
    prog['Block3'].binding = 1

    buf_u1.bind_to_uniform_block(2)
    buf_u2.bind_to_uniform_block(4)
    buf_u4.bind_to_uniform_block(1)

    vao.transform(buf_r)
    a, b = struct.unpack('2f', buf_r.read())
    assert pytest.approx(a) == 9.5
    assert pytest.approx(b) == 4.0

    buf_u3.bind_to_uniform_block(1)

    vao.transform(buf_r)
    a, b = struct.unpack('2f', buf_r.read())
    assert pytest.approx(a) == 309.5
    assert pytest.approx(b) == 3004.0

def test_2(ctx, prog):
    min_offset = ctx.info['GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT']
    u3_len = (min_offset // 4) + 1
    u3_data = list(range(u3_len))

    buf_v = ctx.buffer(struct.pack('2f', 100.0, 1000.0))
    buf_u1 = ctx.buffer(struct.pack('f', 9.5))
    buf_u2 = ctx.buffer(struct.pack('f', 4.0))
    buf_u3 = ctx.buffer(struct.pack('{:d}f'.format(u3_len), *u3_data))
    buf_r = ctx.buffer(reserve=buf_v.size)

    vao = ctx.vertex_array(prog, [
        (buf_v, '2f', 'in_v'),
    ])

    prog['Block1'].binding = 2
    prog['Block2'].binding = 4
    prog['Block3'].binding = 1

    buf_u1.bind_to_uniform_block(2)
    buf_u2.bind_to_uniform_block(4)
    buf_u3.bind_to_uniform_block(1, offset=min_offset)

    vao.transform(buf_r)
    a, b = struct.unpack('2f', buf_r.read())
    assert pytest.approx(a) == u3_data[-1] * 100 + 9.5
    assert pytest.approx(b) == u3_data[-1] * 1000 + 4.0

    buf_u3.bind_to_uniform_block(1, offset=0)

    vao.transform(buf_r)
    a, b = struct.unpack('2f', buf_r.read())
    assert pytest.approx(a) == 9.5
    assert pytest.approx(b) == 4.0
