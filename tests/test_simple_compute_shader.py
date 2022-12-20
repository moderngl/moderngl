import moderngl
import pytest


def test_compute_shader(ctx):
    if ctx.version_code < 430:
        pytest.skip('OpenGL 4.3 is not supported')

    src = '''
        #version 430
        layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

        struct Something
        {
            vec4 pos;
        };

        layout(std430, binding=0) buffer something_in
        {
            Something entries[];
        } In;

        // output into the buffer bound to 1
        layout(std430, binding=1) buffer something_out
        {
            Something entries[];
        } Out;

        uniform float multiplier;

        void main() {
            int x = int(gl_GlobalInvocationID);
            Something entry = In.entries[x];

            Out.entries[x] = Something(entry.pos * multiplier);
        }
    '''

    buffer_in = ctx.buffer(reserve=16)
    buffer_out = ctx.buffer(reserve=16)

    buffer_in.bind_to_storage_buffer(binding=0)
    buffer_out.bind_to_storage_buffer(binding=1)

    compute_shader = ctx.compute_shader(src)

    assert isinstance(compute_shader.glo, int)
    assert compute_shader.mglo is not None
    compute_shader.extra = {'name': 'test'}   
    assert compute_shader.extra == {'name': 'test'}
    assert isinstance(compute_shader.ctx, moderngl.Context)

    compute_shader['multiplier'] = 1.0
    compute_shader['multiplier'].value = 1.0
    compute_shader.run()
    assert compute_shader.get('multiplier', None) is not None
    assert compute_shader == compute_shader
    assert [i for i in compute_shader] == ['multiplier', 'something_in', 'something_out']
    compute_shader.release()
