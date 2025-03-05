import moderngl_window
from moderngl_window import geometry
from pyrr import Matrix44


class PolygonOffset(moderngl_window.WindowConfig):
    title = "Polygon Offset"
    samples = 4

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.program_sphere = self.ctx.program(
            vertex_shader="""
            #version 330

            uniform mat4 m_model;
            uniform mat4 m_proj;

            in vec3 in_position;
            in vec3 in_normal;

            out vec3 pos;
            out vec3 normal;

            void main() {
                vec4 p = m_model * vec4(in_position, 1.0);
                gl_Position =  m_proj * p;
                mat3 m_normal = inverse(transpose(mat3(m_model)));
                normal = m_normal * normalize(in_normal);
                pos = p.xyz;
            }
            """,
            fragment_shader="""
            #version 330

            out vec4 fragColor;
            uniform vec4 color = vec4(1.0, 0.0, 0.0, 1.0);

            in vec3 pos;
            in vec3 normal;

            void main() {
                float l = dot(normalize(-pos), normalize(normal));
                fragColor = color * (0.25 + abs(l) * 0.75);
            }
            """,
        )
        self.program_lines = self.ctx.program(
            vertex_shader="""
            #version 330

            uniform mat4 m_model;
            uniform mat4 m_proj;

            in vec3 in_position;

            void main() {
                gl_Position = m_proj * m_model * vec4(in_position, 1.0);
            }

            """,
            fragment_shader="""
            #version 330

            out vec4 fragColor;

            void main() {
                fragColor = vec4(0.7, 0.7, 0.7, 1.0);
            }
            """,
        )

        self.sphere = geometry.sphere(radius=2.0, sectors=32, rings=16)
        self.projection = Matrix44.perspective_projection(60, self.wnd.aspect_ratio, 1, 100, dtype="f4")
        self.poly_offset_enabled = True

    def on_render(self, time: float, frame_time: float):
        self.ctx.clear()
        self.ctx.enable(self.ctx.DEPTH_TEST | self.ctx.CULL_FACE)

        self.program_lines["m_proj"].write(self.projection)
        self.program_sphere["m_proj"].write(self.projection)

        trans = Matrix44.from_translation((0, 0, -4.5), dtype="f4")
        rot = Matrix44.from_eulers((time/2, time/12.33, time/11.94), dtype="f4")
        matrix = rot @ trans
        self.program_lines["m_model"].write(matrix)
        self.program_sphere["m_model"].write(matrix)

        self.sphere.render(self.program_sphere)
        if self.poly_offset_enabled:
            self.ctx.polygon_offset = -1, -1
        self.ctx.wireframe = True
        self.sphere.render(self.program_lines)
        if self.poly_offset_enabled:
            self.ctx.polygon_offset = 0, 0
        self.ctx.wireframe = False

    def key_event(self, key, action, modifiers):
        keys = self.wnd.keys
        if action == keys.ACTION_PRESS and key == keys.SPACE:
            self.poly_offset_enabled = not self.poly_offset_enabled


PolygonOffset.run()
