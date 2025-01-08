'''
    This examples renders 8 clones of a rectangle with different sizes and positions
    and having the same color
'''

import numpy as np

from _example import Example


class InstancedRendering(Example):
    gl_version = (3, 3)
    title = "Instanced Rendering"
    aspect_ratio = 1.0
    window_size = (640, 640)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                in vec2 in_vert;
                in vec2 in_pos;
                in float in_scale;
                in vec3 in_color;

                out vec3 v_color;

                void main() {
                    gl_Position = vec4(in_pos + (in_vert * in_scale), 0.0, 1.0);
                    v_color = in_color;
                }
            ''',
            fragment_shader='''
                #version 330

                in vec3 v_color;
                out vec4 f_color;

                void main() {
                    f_color = vec4(v_color, 1.0);
                }
            ''',
        )

        # Vertex coordinates stored in position_vertex_buffer
        #
        #     B------D
        #     |      |
        #     A------C
        vertices = np.array([
            -0.5, -0.5,
            -0.5, 0.5,
            0.5, -0.5,
            0.5, 0.5,
        ])

        self.position_vertex_buffer = self.ctx.buffer(vertices.astype('f4'))

        # Vertex colors stored in color_buffer
        #
        #     A, B are green
        #     C, D are blue
        colors = np.array([
            0.0, 1.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, 1.0,
            0.0, 0.0, 1.0,
        ])
        self.color_buffer = self.ctx.buffer(colors.astype('f4'))

        # (Per instance) positions and scales stored in pos_scale_buffer
        # There are 8 (x_position, y_position, scale) pairs
        position_scale = np.array([
            0.5, 0.0, 0.3,
            0.35, 0.35, 0.2,
            0.0, 0.5, 0.3,
            -0.35, 0.35, 0.2,
            -0.5, 0.0, 0.3,
            -0.35, -0.35, 0.2,
            0.0, -0.5, 0.3,
            0.35, -0.35, 0.2,
        ])
        self.pos_scale_buffer = self.ctx.buffer(position_scale.astype('f4'))

        # Index buffer (also called element buffer)
        # There are 2 trianges to render
        #
        #     A, B, C
        #     B, C, D
        render_indicies = np.array([
            0, 1, 2,
            1, 2, 3
        ])
        self.index_buffer = self.ctx.buffer(render_indicies.astype('i4'))

        # The vao_content is a list of 3-tuples (buffer, format, attribs)
        # the format can have an empty or '/v', '/i', '/r' ending.
        # '/v' attributes are the default
        # '/i` attributes are per instance attributes
        # '/r' attributes are default values for the attributes (per render attributes)
        vao_content = [
            (self.position_vertex_buffer, '2f', 'in_vert'),
            (self.color_buffer, '3f', 'in_color'),
            (self.pos_scale_buffer, '2f 1f/i', 'in_pos', 'in_scale'),
        ]

        self.vao = self.ctx.vertex_array(self.prog, vao_content, self.index_buffer)

    def on_render(self, time: float, frame_time: float):
        self.ctx.clear(1.0, 1.0, 1.0)
        self.vao.render(instances=8)


if __name__ == '__main__':
    InstancedRendering.run()
