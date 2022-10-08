"""
Shows transform feedback with geometry shaders were
the input and output primitive is different.

We have a list of points and emit triangle data to a buffer
It creates a quads per point using two triangles.
"""
import struct
from ported._example import Example
import moderngl


class GenerateData(Example):
    title = "Transform Feedback Geometry shader"
    gl_version = (3, 3)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330
                in vec3 in_vert;
                void main() {
                    gl_Position = vec4(in_vert, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330
                out vec4 f_color;
                void main() {
                    f_color = vec4(1.0);
                }
            ''',
        )

        # Transform program with geo shader
        self.transform_prog = self.ctx.program(
            vertex_shader='''
                #version 330

                void main() {
                    gl_Position = vec4(gl_VertexID/5.0 - 0.9, 0.0, 0.0, 1.0);
                }
            ''',
            geometry_shader="""
            #version 330
            layout(points) in;
            layout(triangle_strip, max_vertices = 6) out;
            out vec3 pos;
            const float SIZE = 0.05;
            void main() {
                vec3 center = gl_in[0].gl_Position.xyz;

                // First triangle
                pos = center + vec3(-SIZE, SIZE, 0.0);
                EmitVertex();
                pos = center + vec3(-SIZE, -SIZE, 0.0);
                EmitVertex();
                pos = center + vec3(SIZE, SIZE, 0.0);
                EmitVertex();
                EndPrimitive();

                // Second triangle
                pos = center + vec3(SIZE, SIZE, 0.0);
                EmitVertex();
                pos = center + vec3(-SIZE, -SIZE, 0.0);
                EmitVertex();
                pos = center + vec3(SIZE, -SIZE, 0.0);
                EmitVertex();

                EndPrimitive();
            }
            """,
            varyings=['pos'],
        )

        N = 10
        self.buffer = self.ctx.buffer(reserve=N * 12 * 6)  # N * 6 x vec3
        self.transform_vao = self.ctx.vertex_array(self.transform_prog, [])
        self.transform_vao.transform(self.buffer, mode=moderngl.POINTS, vertices=N)

        self.vao = self.ctx.vertex_array(
            self.prog,
            [(self.buffer, '3f', 'in_vert')]
        )

    def render(self, time, frame_time):
        self.wnd.clear()
        self.vao.render(mode=moderngl.TRIANGLES)


if __name__ == '__main__':
    GenerateData.run()
