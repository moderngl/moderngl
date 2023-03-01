from array import array
import struct
import pytest


def test_1(ctx):
    if ctx.version_code < 430:
        pytest.skip('compute shaders not supported')

    compute_shader = ctx.compute_shader('''
        #version 430

        layout (local_size_x = 1, local_size_y = 1) in;

        layout (std430, binding = 1) buffer Input {
            float v1[4];
        };

        layout (std430, binding = 2) buffer Output {
            float v2[4];
        };

        void main() {
            v2[0] = v1[3];
            v2[1] = v1[2];
            v2[2] = v1[1];
            v2[3] = v1[0];
        }
    ''')

    buf1 = ctx.buffer(struct.pack('4f', 1.0, 2.0, 3.0, 4.0))
    buf2 = ctx.buffer(struct.pack('4f', 0.0, 0.0, 0.0, 0.0))

    buf1.bind_to_storage_buffer(1)
    buf2.bind_to_storage_buffer(2)

    compute_shader.run()

    a, b, c, d = struct.unpack('4f', buf2.read())

    assert pytest.approx(a) == 4.0
    assert pytest.approx(b) == 3.0
    assert pytest.approx(c) == 2.0
    assert pytest.approx(d) == 1.0


def test_image(ctx):
    if ctx.version_code < 430:
        pytest.skip('compute shaders not supported')

    texture = ctx.texture((100, 100), 4)
    texture.bind_to_image(0, read=True, write=True)
    assert ctx.error == 'GL_NO_ERROR'
    texture.release()


def test_image_float(ctx):
    if ctx.version_code < 430:
        pytest.skip('compute shaders not supported')

    texture = ctx.texture((100, 100), 4, dtype='f4')
    texture.bind_to_image(0, read=True, write=True)
    assert ctx.error == 'GL_NO_ERROR'
    texture.release()


def test_image_wrong_format(ctx):
    if ctx.version_code < 430:
        pytest.skip('compute shaders not supported')

    texture = ctx.texture((100, 100), 4)
    texture.bind_to_image(0, read=True, write=True, format=13371337)
    assert ctx.error == 'GL_INVALID_VALUE'
    texture.release()


def test_3d_image(ctx):
    if ctx.version_code < 430:
        pytest.skip('compute shaders not supported')

    program = ctx.compute_shader(
        """
        #version 430

        layout(local_size_x=4, local_size_y=4, local_size_z=4) in;

        layout(rgba32f, binding=0) uniform image3D img_in;
        layout(rgba32f, binding=1) uniform image3D img_out;

        void main() {
            vec4 fragment = imageLoad(img_in, ivec3(gl_LocalInvocationID.xyz));
            imageStore(img_out, ivec3(gl_LocalInvocationID.xyz), fragment);
        }
        """
    )
    tex_in = ctx.texture3d((4, 4, 4), 4, data=array('f', [v for v in range(4 * 4 * 4 * 4)]), dtype="f4")
    tex_out = ctx.texture3d((4, 4, 4), 4, dtype="f4")

    tex_in.bind_to_image(0, read=True, write=False)
    tex_out.bind_to_image(1, read=False, write=True)
    program.run(group_x=1)
    assert ctx.error == 'GL_NO_ERROR'

    data_in = struct.unpack("256f", tex_in.read())
    data_out = struct.unpack("256f", tex_out.read())

    assert data_in == data_out


def test_texture_array_image(ctx):
    if ctx.version_code < 430:
        pytest.skip('compute shaders not supported')

    program = ctx.compute_shader(
        """
        #version 430

        layout(local_size_x=4, local_size_y=4, local_size_z=4) in;

        layout(rgba32f, binding=0) uniform image2DArray img_in;
        layout(rgba32f, binding=1) uniform image2DArray img_out;

        void main() {
            vec4 fragment = imageLoad(img_in, ivec3(gl_LocalInvocationID.xyz));
            imageStore(img_out, ivec3(gl_LocalInvocationID.xyz), fragment);
        }
        """
    )
    tex_in = ctx.texture_array((4, 4, 4), 4, data=array('f', [v for v in range(4 * 4 * 4 * 4)]), dtype="f4")
    tex_out = ctx.texture_array((4, 4, 4), 4, dtype="f4")

    tex_in.bind_to_image(0, read=True, write=False)
    tex_out.bind_to_image(1, read=False, write=True)
    program.run(group_x=1)

    data_in = struct.unpack("256f", tex_in.read())
    data_out = struct.unpack("256f", tex_out.read())

    assert data_in == data_out


def test_texture_cube_image(ctx):
    if ctx.version_code < 430:
        pytest.skip('compute shaders not supported')

    program = ctx.compute_shader(
        """
        #version 450

        layout(local_size_x=4, local_size_y=4) in;

        layout(rgba8, binding=0) uniform imageCube img_in;
        layout(rgba8, binding=1) uniform imageCube img_out;

        void main() {
            for (int i = 0; i < 6; i++) {
                vec4 fragment = imageLoad(img_in, ivec3(gl_LocalInvocationID.xy, i));
                imageStore(img_out, ivec3(gl_LocalInvocationID.xy, i), fragment);
            }
        }
        """
    )
    data = ([1] * 64) + ([2] * 64) + ([3] * 64) + ([4] * 64) + ([5] * 64) + ([6] * 64)
    tex_in = ctx.texture_cube((4, 4), 4, data=array('B', data), dtype="f1")
    tex_out = ctx.texture_cube((4, 4), 4, data=array('B', [0] * 64 * 6), dtype="f1")
    tex_in.bind_to_image(0, read=True, write=False)
    tex_out.bind_to_image(1, read=False, write=True)
    program.run(group_x=1)

    for face in range(0, 6):
        assert struct.unpack("64B", tex_in.read(face)) == struct.unpack("64B", tex_out.read(face))


def test_ssbo_binding(ctx):
    if ctx.version_code < 430:
        pytest.skip('compute shaders not supported')

    program = ctx.compute_shader(
        """
        #version 430

        layout(local_size_x=1, local_size_y=1, local_size_z=1) in;

        layout(std430, binding=0) buffer ssbo_in {
            float data[4];
        } in_data;

        layout(std430, binding=1) buffer ssbo_out {
            float data[4];
        } out_data;

        void main() {
            out_data.data[gl_LocalInvocationIndex] = in_data.data[gl_LocalInvocationIndex];
        }
        """
    )
    ssbo_in = program['ssbo_in']
    ssbo_out = program['ssbo_out']

    assert ssbo_in.name == 'ssbo_in'
    assert ssbo_out.name == 'ssbo_out'

    assert ssbo_in.binding == 0
    assert ssbo_in.value == 0
    assert ssbo_out.binding == 1
    assert ssbo_out.value == 1

    ssbo_in.binding = 2
    ssbo_out.binding = 3
    assert ssbo_in.binding == 2
    assert ssbo_in.value == 2
    assert ssbo_out.binding == 3
    assert ssbo_out.value == 3
