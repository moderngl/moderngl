'''
    Renders a color triangle with imageStore
'''

from _example import Example


class HelloWorld(Example):
    gl_version = (4, 5)
    title = "Empty Framebuffer"

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 450 core
                out vec3 v_color;
                vec2 positions[3] = vec2[](
                    vec2(0.0, 0.8),
                    vec2(-0.6, -0.8),
                    vec2(0.6, -0.8)
                );
                vec3 colors[3] = vec3[](
                    vec3(1.0, 0.0, 0.0),
                    vec3(0.0, 1.0, 0.0),
                    vec3(0.0, 0.0, 1.0)
                );
                void main() {
                    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
                    v_color = colors[gl_VertexID];
                }
            ''',
            fragment_shader='''
                #version 450 core
                in vec3 v_color;
                layout(rgba8, binding = 0) writeonly uniform image2D output_image;
                void main() {
                    imageStore(output_image, ivec2(gl_FragCoord.xy), vec4(v_color, 1.0));
                }
            ''',
        )

        self.prog['output_image'] = 0

        self.texture = self.ctx.texture(self.window_size, components=4)
        self.texture.bind_to_image(0, read=False, write=True)

        self.empty_framebuffer = self.ctx.empty_framebuffer(self.window_size)
        self.vao = self.ctx.vertex_array(self.prog, [])
        self.vao.scope = self.ctx.scope(self.empty_framebuffer)
        self.vao.vertices = 3

        self.fbo = self.ctx.framebuffer([self.texture])

    def on_render(self, time, frame_time):
        self.fbo.clear(1.0, 1.0, 1.0, 1.0)
        self.vao.render()
        self.ctx.memory_barrier()
        self.ctx.copy_framebuffer(self.ctx.screen, self.fbo)


if __name__ == '__main__':
    HelloWorld.run()
