#!/usr/bin/env python3
'''Simple example of using tessellation to render a cubic Bézier curve'''

import numpy as np

import moderngl
from ported._example import Example


class Tessellation(Example):
    title = "Tessellation"
    gl_version = (4, 0)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
            #version 400 core

            in vec2 in_pos;

            void main() { gl_Position = vec4(in_pos, 0.0, 1.0); }
            ''',
            tess_control_shader='''
            #version 400 core

            layout(vertices = 4) out;

            void main() {
            // set tessellation levels, TODO compute dynamically
            gl_TessLevelOuter[0] = 1;
            gl_TessLevelOuter[1] = 32;

            // pass through vertex positions
            gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
            }
            ''',
            tess_evaluation_shader='''
            #version 400 core

            layout(isolines, fractional_even_spacing, ccw) in;

            // compute a point on a bezier curve with the points p0, p1, p2, p3
            // the parameter u is in [0, 1] and determines the position on the curve
            vec3 bezier(float u, vec3 p0, vec3 p1, vec3 p2, vec3 p3) {
            float B0 = (1.0 - u) * (1.0 - u) * (1.0 - u);
            float B1 = 3.0 * (1.0 - u) * (1.0 - u) * u;
            float B2 = 3.0 * (1.0 - u) * u * u;
            float B3 = u * u * u;

            return B0 * p0 + B1 * p1 + B2 * p2 + B3 * p3;
            }

            void main() {
            float u = gl_TessCoord.x;

            vec3 p0 = vec3(gl_in[0].gl_Position);
            vec3 p1 = vec3(gl_in[1].gl_Position);
            vec3 p2 = vec3(gl_in[2].gl_Position);
            vec3 p3 = vec3(gl_in[3].gl_Position);

            gl_Position = vec4(bezier(u, p0, p1, p2, p3), 1.0);
            }
            ''',
            fragment_shader='''
            #version 400 core

            out vec4 frag_color;

            void main() { frag_color = vec4(1.0); }
            '''
        )

        # four vertices define a cubic Bézier curve; has to match the shaders
        self.ctx.patch_vertices = 4

        self.ctx.line_width = 5.0
        vertices = np.array([
            [-1.0, 0.0],
            [-0.5, 1.0],
            [0.5, -1.0],
            [1.0, 0.0],
        ])

        vbo = self.ctx.buffer(vertices.astype('f4'))
        self.vao = self.ctx.simple_vertex_array(self.prog, vbo, 'in_pos')

    def render(self, time, frame_time):
        self.ctx.clear(0.2, 0.4, 0.7)
        self.vao.render(mode=moderngl.PATCHES)


if __name__ == '__main__':
    Tessellation.run()
