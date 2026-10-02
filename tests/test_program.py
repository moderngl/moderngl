import moderngl
import pytest


def test_program(ctx):
    program = ctx.program(
        vertex_shader='''
            #version 330

            uniform vec2 pos;
            uniform float scale;

            in vec2 vert;
            out vec2 v_vert;

            void main() {
                gl_Position = vec4(pos + vert * scale, 0.0, 1.0);
                v_vert = vert;
            }
        ''',
        fragment_shader='''
            #version 330

            in vec2 v_vert;
            out vec4 color;

            void main() {
                color = vec4(v_vert, 0.0, 1.0);
            }
        ''',
    )

    program.label = "my favorite program"
    assert program.glo > 0

    assert 'vert' in program
    assert 'pos' in program
    assert 'scale' in program
    assert program.is_transform is False
    assert program.label == "my favorite program"

    assert isinstance(program['vert'], moderngl.Attribute)
    assert isinstance(program['pos'], moderngl.Uniform)
    assert isinstance(program['scale'], moderngl.Uniform)


def test_geo_input_output_primitive(ctx):
    """Test detection of all geo shader input/output primitives types"""
    input_types = [
        ("points", moderngl.POINTS),
        ("lines", moderngl.LINES),
        ("lines_adjacency", moderngl.LINES_ADJACENCY),
        ("triangles", moderngl.TRIANGLES),
        ("triangles_adjacency", moderngl.TRIANGLES_ADJACENCY),
    ]
    output_types = [
        ("points", moderngl.POINTS),
        ("line_strip", moderngl.LINES),
        ("triangle_strip", moderngl.TRIANGLES),
    ]

    for out_name, out_type in output_types:
        for in_name, in_type in input_types:
            p = ctx.program(
                vertex_shader="""
                #version 330
                void main() {
                    gl_Position = vec4(1.0);
                }
                """,
                geometry_shader=(
                    "#version 330\n"
                    f"layout({in_name}) in;\n"
                    f"layout({out_name}, max_vertices=6) out;\n"
                    "void main() {\n"
                    "    gl_Position = vec4(1.0);\n"
                    "    EmitVertex();\n"
                    "    EndPrimitive();\n"
                    "}\n"
                ),
            )
            assert p.geometry_input == in_type
            assert p.geometry_output == out_type, f"input: {in_name}, output: {out_name}"


FRAGMENT_OUTPUT_VERTEX_SHADER = '''
    #version 330

    in vec2 vert;

    void main() {
        gl_Position = vec4(vert, 0.0, 1.0);
    }
'''

FRAGMENT_OUTPUT_FRAGMENT_SHADER = '''
    #version 330

    out vec4 color;

    void main() {
        color = vec4(1.0);
    }
'''


def test_fragment_outputs(ctx):
    program = ctx.program(
        vertex_shader=FRAGMENT_OUTPUT_VERTEX_SHADER,
        fragment_shader=FRAGMENT_OUTPUT_FRAGMENT_SHADER,
        fragment_outputs={"color": 0},
    )
    assert "vert" in program


@pytest.mark.parametrize(
    "fragment_outputs, message",
    [
        ({1: 0}, "keys must be str"),
        ({b"color": 0}, "keys must be str"),
        ({"color": "0"}, "values must be int"),
        ({"color": 0.5}, "values must be int"),
    ],
)
def test_fragment_outputs_invalid(ctx, fragment_outputs, message):
    # Used to pass a NULL name (or location -1) to glBindFragDataLocation
    # and leave a stale Python exception behind
    with pytest.raises(moderngl.Error, match=message):
        ctx.program(
            vertex_shader=FRAGMENT_OUTPUT_VERTEX_SHADER,
            fragment_shader=FRAGMENT_OUTPUT_FRAGMENT_SHADER,
            fragment_outputs=fragment_outputs,
        )
