"""
Render using empty Vertexbuffer.
Renders 100 triangels emitted by a geometry shaders.
In addition we test if instancing is working passing gl_InstanceID from the vertex shader.
"""

import moderngl

from _example import Example


class HelloWorld(Example):
    gl_version = (3, 3)
    title = "Empty Vertexbuffer"

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330
                out int inst;
                void main() {
                    inst = gl_InstanceID;
                }
            ''',
            geometry_shader='''
                #version 330
                layout (points) in;
                layout (triangle_strip, max_vertices = 3) out;
                in int inst[1];
                void main() {
                    float x = float(gl_PrimitiveIDIn / 10) / 9 - 0.5 + inst[0] / 20.0;
                    float y = float(gl_PrimitiveIDIn % 10) / 9 - 0.5 + inst[0] / 20.0;
                    gl_Position = vec4(x - 0.03, y - 0.03, 0.0, 1.0);
                    EmitVertex();
                    gl_Position = vec4(x + 0.03, y - 0.03, 0.0, 1.0);
                    EmitVertex();
                    gl_Position = vec4(x, y + 0.03, 0.0, 1.0);
                    EmitVertex();
                    EndPrimitive();
                }
            ''',
            fragment_shader='''
                #version 330
                out vec4 f_color;
                void main() {
                    f_color = vec4(0.3, 0.5, 1.0, 1.0);
                }
            ''',
        )
        self.vao = self.ctx.vertex_array(self.prog, [])

    def on_render(self, time, frame_time):
        self.ctx.clear(1.0, 1.0, 1.0)
        self.vao.render(mode=moderngl.POINTS, vertices=100, instances=2)


if __name__ == '__main__':
    HelloWorld.run()
