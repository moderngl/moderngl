
def test_sampler_2D_get_setter(ctx):
    prog = ctx.program(
        vertex_shader='''
            #version 330
            uniform sampler2D Texture2D;
            void main() {
                gl_Position = texture(Texture2D, vec2(0.0, 0.0));
            }
        '''
    )

    sampler_uniform = prog['Texture2D']

    sampler_uniform.value = 3
    assert sampler_uniform.value == 3

    sampler_uniform.value = 1
    assert sampler_uniform.value == 1

def test_sampler_3D_get_setter(ctx):
    prog = ctx.program(
        vertex_shader='''
            #version 330
            uniform sampler3D Texture3D;
            void main() {
                gl_Position = texture(Texture3D, vec3(0.0, 0.0, 0.0));
            }
        '''
    )

    sampler_uniform = prog['Texture3D']

    sampler_uniform.value = 3
    assert sampler_uniform.value == 3

    sampler_uniform.value = 1
    assert sampler_uniform.value == 1
