import moderngl

def test_1(ctx):
    prog = ctx.program(
        vertex_shader='''
            #version 330

            in vec2 in_vert;
            out vec2 v_vert;

            uniform mat2 Mvp;
            uniform vec2 Position;
            uniform float Scale;

            void main() {
                v_vert = Position + in_vert * Scale;
                gl_Position = vec4(Mvp * v_vert, 0.0, 1.0);
            }
        ''',
        fragment_shader='''
            #version 330

            uniform bool UseTexture;
            uniform sampler2D Texture;

            uniform vec3 Color;
            uniform float Alpha;

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
    assert 'Position' in prog
    assert 'Scale' in prog
    assert 'Mvp' in prog
    assert 'UseTexture' in prog
    assert 'Texture' in prog
    assert 'Color' in prog
    assert 'Alpha' in prog

    assert isinstance(prog['in_vert'], moderngl.Attribute)
    assert isinstance(prog['Position'], moderngl.Uniform)
    assert isinstance(prog['Scale'], moderngl.Uniform)
    assert isinstance(prog['Mvp'], moderngl.Uniform)
    assert isinstance(prog['UseTexture'], moderngl.Uniform)
    assert isinstance(prog['Texture'], moderngl.Uniform)
    assert isinstance(prog['Color'], moderngl.Uniform)
    assert isinstance(prog['Alpha'], moderngl.Uniform)
