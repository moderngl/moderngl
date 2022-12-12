
VERTEX_SHADER = '''
    #version 330
    uniform mat4 Mvp;
    in vec4 vertex;
    uniform Block {
        mat4 Transform;
    };
    void main() {
        gl_Position = Mvp * Transform * vertex;
    }
'''

GEOMETRY_SHADER = '''
    #version 330
    layout(points) in;
    uniform vec4 Position;
    layout(line_strip, max_vertices = 2) out;
    out vec4 color;
    void main() {
        gl_Position = gl_in[0].gl_Position + Position;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position - Position;
        EmitVertex();
        EndPrimitive();
    }
'''

FRAGMENT_SHADER = '''
    #version 330
    uniform vec4 Color;
    uniform Lights {
        uniform vec4 Light[1];
    };
    out vec4 color;
    void main() {
        color = Color * Light[0];
    }
'''

def test_program_uniforms_1(ctx):
    program = ctx.program(vertex_shader=VERTEX_SHADER)

    assert 'Mvp' in program
    assert 'Block' in program
    assert 'Transform' not in program

def test_program_uniforms_2(ctx):
    program = ctx.program(
        vertex_shader=VERTEX_SHADER,
        geometry_shader=GEOMETRY_SHADER,
        fragment_shader=FRAGMENT_SHADER,
    )

    assert 'Mvp' in program
    assert 'Position' in program
    assert 'Color' in program

def test_program_uniform_blocks(ctx):
    program = ctx.program(
        vertex_shader=VERTEX_SHADER,
        fragment_shader=FRAGMENT_SHADER,
    )

    assert 'Block' in program
    assert 'Lights' in program
    assert 'Transform' not in program
    assert 'Light' not in program

def test_program_geometry_primitives_1(ctx):
    program = ctx.program(vertex_shader=VERTEX_SHADER)

    assert program.geometry_input is None
    assert program.geometry_output is None
    assert program.geometry_vertices == 0

def test_program_geometry_primitives_2(ctx):
    program = ctx.program(
        vertex_shader=VERTEX_SHADER,
        geometry_shader=GEOMETRY_SHADER,
    )
    assert program.geometry_vertices == 2
