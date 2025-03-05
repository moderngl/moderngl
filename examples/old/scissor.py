'''
    Framebuffer scissoring.
    Renders a fullscreen quad four times with different scissor
    values filling each quadrant of the screen with a different color.

    We swap between rendering geometry and using clear.
'''

import numpy as np

import moderngl
from _example import Example


class Scissor(Example):
    gl_version = (3, 3)
    title = "Scissor"
    resizable = True
    aspect_ratio = None

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                in vec2 in_vert;

                void main() {
                    gl_Position = vec4(in_vert, 0.0, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330

                out vec4 fragColor;
                uniform vec4 color;

                void main() {
                    fragColor = color;
                }
            ''',
        )
        quad = [
            -1.0,  1.0,
            -1.0, -1.0,
            1.0, 1.0,
            1.0, -1.0,
        ]
        vbo = self.ctx.buffer(np.array(quad, dtype='f4'))
        self.vao = self.ctx.simple_vertex_array(self.prog, vbo, 'in_vert')

    def on_render(self, time: float, frame_time: float):
        """Swap between rendering geometry and using clear"""
        fb_width_half = self.wnd.buffer_width // 2
        fb_height_half = self.wnd.buffer_height // 2

        if self.wnd.frames % 2 == 0:
            # upper left (red)
            self.ctx.scissor = 0, fb_height_half, fb_width_half, fb_height_half
            self.prog['color'].value = 1.0, 0.0, 0.0, 0.0
            self.vao.render(mode=moderngl.TRIANGLE_STRIP)

            # upper right (green)
            self.ctx.scissor = fb_width_half, fb_height_half, fb_width_half, fb_height_half
            self.prog['color'].value = 0.0, 1.0, 0.0, 0.0
            self.vao.render(mode=moderngl.TRIANGLE_STRIP)

            # Lower left
            self.ctx.scissor = 0, 0, fb_width_half, fb_height_half
            self.prog['color'].value = 0.0, 0.0, 1.0, 0.0
            self.vao.render(mode=moderngl.TRIANGLE_STRIP)

            # Lower right
            self.ctx.scissor = fb_width_half, 0, fb_width_half, fb_height_half
            self.prog['color'].value = 1.0, 1.0, 1.0, 1.0
            self.vao.render(mode=moderngl.TRIANGLE_STRIP)
        else:
            # upper left (red)
            self.ctx.scissor = 0, fb_height_half, fb_width_half, fb_height_half
            self.ctx.clear(1.0, 0.0, 0.0, 0.0)

            # upper right (green)
            self.ctx.scissor = fb_width_half, fb_height_half, fb_width_half, fb_height_half
            self.ctx.clear(0.0, 1.0, 0.0, 0.0)

            # Lower left
            self.ctx.scissor = 0, 0, fb_width_half, fb_height_half
            self.ctx.clear(0.0, 0.0, 1.0, 0.0)

            # Lower right
            self.ctx.scissor = fb_width_half, 0, fb_width_half, fb_height_half
            self.ctx.clear(1.0, 1.0, 1.0, 1.0)

        # Reset scissor
        self.ctx.scissor = None


if __name__ == '__main__':
    Scissor.run()
