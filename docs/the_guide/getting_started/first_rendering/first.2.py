import moderngl

ctx = moderngl.create_standalone_context()

prog = ctx.program(
    vertex_shader="""
        #version 330

        in vec2 in_vert;
        in vec3 in_color;

        out vec3 v_color;

        void main() {
            v_color = in_color;
            gl_Position = vec4(in_vert, 0.0, 1.0);
        }
    """,
    fragment_shader="""
        #version 330

        in vec3 v_color;

        out vec3 f_color;

        void main() {
            f_color = v_color;
        }
    """,
)
