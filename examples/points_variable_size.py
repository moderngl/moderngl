"""
Example showing the use of PROGRAM_POINT_SIZE
were we modify point sizes int the vertex shader
making us able to specify a unique size per point.

Without PROGRAM_POINT_SIZE enabled the point size
will be static and obtained from ctx.point_size.

We also show the use of gl_PointCoord making us able
to fill the point with any data we want just like a quad.
"""
import numpy as np
from pyrr import Matrix44

import moderngl
from ported._example import Example


class Particles(Example):
    title = "Particle System"
    gl_version = (3, 3)
    aspect_ratio = None

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader="""
            #version 330

            in vec3 in_position;

            uniform mat4 projection;
            uniform mat4 modelview;
            uniform float time;

            out vec3 color;

            void main() {
                gl_Position = projection * modelview * vec4(in_position, 1.0);

                // Set the point size
                gl_PointSize = 25 - gl_Position.z + sin((time + gl_VertexID) * 7.0) * 10.0;

                // Calculate a random color based on the vertex index
                color = vec3(mod(gl_VertexID * 432.43, 1.0), mod(gl_VertexID * 6654.32, 1.0), mod(gl_VertexID  * 6544.11, 1.0));
            }
            """,
            fragment_shader="""
            #version 330

            in vec3 color;
            out vec4 outColor;

            void main() {
                // Calculate the distance from the center of the point
                // gl_PointCoord is available when redering points. It's basically an uv coordinate.
                float dist = step(length(gl_PointCoord.xy - vec2(0.5)), 0.5);

                // .. an use to render a circle!
                outColor = vec4(dist * color, dist);
            }
            """,
        )

        positions = np.random.random_sample((1000,)) * 10 - 5
        self.pos_buffer = self.ctx.buffer(positions.astype('f4'))
        self.vao = self.ctx.vertex_array(
            self.prog,
            [(self.pos_buffer, '3f', 'in_position')],
        )
        self.resize(*self.wnd.buffer_size)

    def render(self, time, frame_time):
        self.ctx.enable_only(moderngl.PROGRAM_POINT_SIZE | moderngl.BLEND)
        self.ctx.blend_func = moderngl.ADDITIVE_BLENDING

        rotation = Matrix44.from_eulers((time, time / 2 , time / 3), dtype='f4')
        translation = Matrix44.from_translation((0, 0, -10), dtype='f4')
        modelview = translation * rotation

        self.prog['modelview'].write(modelview)
        self.prog['time'].value = time
        self.vao.render(mode=moderngl.POINTS)

    def resize(self, width, height):
        self.projection = Matrix44.perspective_projection(60, self.wnd.aspect_ratio, 1, 100, dtype='f4')
        self.prog['projection'].write(self.projection)


if __name__ == '__main__':
    Particles.run()
