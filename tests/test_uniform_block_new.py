import moderngl

def test_1(ctx):
    prog = ctx.program(
        vertex_shader='''
            #version 330

            in vec2 in_vert;
            out vec2 v_vert;

            uniform mat2 Mvp;

            uniform VertexShaderUniforms {
                vec2 Position;
                float Scale;
            };

            void main() {
                v_vert = Position + in_vert * Scale;
                gl_Position = vec4(Mvp * v_vert, 0.0, 1.0);
            }
        ''',
        fragment_shader='''
            #version 330

            uniform bool UseTexture;
            uniform sampler2D Texture;

            uniform FragmentShaderUniforms {
                vec3 Color;
                float Alpha;
            };

            in vec2 v_vert;
            out vec4 f_color;

            void main() {
                if (UseTexture) {
                    f_color = texture(Texture, v_vert);
                    f_color.rgb *= Color;
                    f_color.a *= Alpha;
                } else {
                    f_color = vec4(Color, Alpha);
                }
            }
        ''',
    )

    assert 'in_vert' in prog
    assert 'Position' not in prog
    assert 'Scale' not in prog
    assert 'Mvp' in prog
    assert 'UseTexture' in prog
    assert 'Texture' in prog
    assert 'Color' not in prog
    assert 'Alpha' not in prog
    assert 'VertexShaderUniforms' in prog
    assert 'FragmentShaderUniforms' in prog

    assert isinstance(prog['in_vert'], moderngl.Attribute)
    assert isinstance(prog['Mvp'], moderngl.Uniform)
    assert isinstance(prog['UseTexture'], moderngl.Uniform)
    assert isinstance(prog['Texture'], moderngl.Uniform)
    assert isinstance(prog['VertexShaderUniforms'], moderngl.UniformBlock)
    assert isinstance(prog['FragmentShaderUniforms'], moderngl.UniformBlock)
