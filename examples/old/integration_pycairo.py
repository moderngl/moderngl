"""
Using pycairo with moderngl.

We simply create a screen aligned quad with texture coordinates
to render the uploaded texture from cairo.

Textures in OpenGL are stored "upside-down" so we build
a vertex array with inverted y coordinates

"""
import math
from array import array

import cairo
import moderngl
from _example import Example


class CairoExample(Example):
    title = "Cairo Integration"
    aspect_ratio = 1.0

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.texture = self.render_cairo_to_texture(512, 512)
        self.prog = self.ctx.program(
            vertex_shader="""
            #version 330
            in vec3 in_position;
            in vec2 in_texcoord_0;
            out vec2 uv;
            void main() {
                gl_Position = vec4(in_position, 1.0);
                uv = in_texcoord_0;
            }
            """,
            fragment_shader="""
            #version 330
            uniform sampler2D texture0;
            in vec2 uv;
            out vec4 outColor;
            void main() {
                outColor = texture(texture0, uv);
            }
            """,
        )
        # Create a simple screen rectangle. The texture coordinates
        # are reverted on the y axis here to make the cairo texture appear correctly.
        vertices = [
            # x, y | u, v
            -1,  1,  0, 0,
            -1, -1,  0, 1,
             1,  1,  1, 0,
             1, -1,  1, 1,
        ]
        self.screen_rectangle = self.ctx.vertex_array(
            self.prog,
            [
                (
                    self.ctx.buffer(array('f', vertices)),
                    '2f 2f',
                    'in_position', 'in_texcoord_0',
                )
            ],
        )

    def on_render(self, time, frame_time):
        self.texture.use(location=0)
        self.screen_rectangle.render(mode=moderngl.TRIANGLE_STRIP)

    def render_cairo_to_texture(self, width, height):
        # Draw with cairo to surface
        x, y, radius = (250, 250, 200)
        surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, width, height)
        ctx = cairo.Context(surface)
        ctx.set_line_width(15)
        ctx.arc(x, y, radius, 0, 2.0 * math.pi)
        ctx.set_source_rgb(0.8, 0.8, 0.8)
        ctx.fill_preserve()
        ctx.set_source_rgb(1, 1, 0)
        ctx.stroke()

        ctx.move_to(20, 30)
        ctx.select_font_face("Purisa", cairo.FONT_SLANT_NORMAL,
                            cairo.FONT_WEIGHT_NORMAL)
        ctx.set_font_size(13)
        ctx.set_source_rgb(0.8, 0.8, 0.8)
        ctx.show_text("Example Text")
        # Copy surface to texture
        texture = self.ctx.texture((width, height), 4, data=surface.get_data())
        texture.swizzle = 'BGRA' # use Cairo channel order (alternatively, the shader could do the swizzle)
        return texture

if __name__ == "__main__":
    CairoExample.run()
