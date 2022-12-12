import struct
import pytest

def test_1(ctx):
    if ctx.version_code < 430:
        raise pytest.skip('Compute shaders not supported')

    compute_shader = ctx.compute_shader('''
        #version 430

        layout (local_size_x = 1, local_size_y = 1) in;

        layout (std430, binding = 1) buffer Input {
            float v1[4];
        };

        layout (std430, binding = 2) buffer Output {
            float v2[4];
        };

        uniform float mul;
        uniform vec4 add;

        void main() {
            v2[0] = v1[3] * mul + add.x;
            v2[1] = v1[2] * mul + add.y;
            v2[2] = v1[1] * mul + add.z;
            v2[3] = v1[0] * mul + add.w;
        }
    ''')

    buf1 = ctx.buffer(struct.pack('4f', 1.0, 2.0, 3.0, 4.0))
    buf2 = ctx.buffer(struct.pack('4f', 0.0, 0.0, 0.0, 0.0))

    buf1.bind_to_storage_buffer(1)
    buf2.bind_to_storage_buffer(2)

    compute_shader['add'].value = (10.5, 20.0, 30.5, 40.0)
    compute_shader['mul'].value = 100.0
    compute_shader.run()

    a, b, c, d = struct.unpack('4f', buf2.read())

    assert pytest.approx(a) == 410.5
    assert pytest.approx(b) == 320.0
    assert pytest.approx(c) == 230.5
    assert pytest.approx(d) == 140.0
