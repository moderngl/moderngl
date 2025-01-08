# This example opens an image, and offsets the red, green, and blue channels to create a glitchy RGB split effect.
from pathlib import Path
from array import array

from PIL import Image

import moderngl
import moderngl_window


class ImageProcessing(moderngl_window.WindowConfig):
    window_size = 3840 // 2, 2160 // 2
    resource_dir = Path(__file__).parent.resolve()
    aspect_ratio = None

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.image_processing = ImageTransformer(self.ctx, self.window_size)
        self.texture = self.load_texture_2d("data/ball.png")

    def on_render(self, time, frame_time):
        self.image_processing.render(self.texture, target=self.ctx.screen)

        # Headless
        self.image_processing.render(self.texture)
        self.image_processing.write("output.png")


class ImageTransformer:

    def __init__(self, ctx, size, program=None):
        self.ctx = ctx
        self.size = size
        self.program = None
        self.fbo = self.ctx.framebuffer(
            color_attachments=[self.ctx.texture(self.size, 4)]
        )

        # Create some default program if needed
        if not program:
            self.program = self.ctx.program(
                vertex_shader="""
                    #version 330

                    in vec2 in_position;
                    in vec2 in_uv;
                    out vec2 uv;

                    void main() {
                        gl_Position = vec4(in_position, 0.0, 1.0);
                        uv = in_uv;
                    }
                """,
                fragment_shader = """
                    #version 330

                    uniform sampler2D image;
                    in vec2 uv;
                    out vec4 out_color;

                    void main() {
                        // Get the Red, green, blue values from the image
                        float red = texture(image, uv).r;
                        // Offset green and blue
                        float green = texture(image, uv+(1.0/20.0)).g;
                        float blue = texture(image, uv+(2.0/20.0)).b;
                        float alpha = texture(image, uv).a;
                        
                        out_color = vec4(red, green, blue, alpha);
                    }
                """,          
            )

        # Fullscreen quad in NDC
        self.vertices = self.ctx.buffer(
            array(
                'f',
                [
                    # Triangle strip creating a fullscreen quad
                    # x, y, u, v
                    -1,  1, 0, 1,  # upper left
                    -1, -1, 0, 0, # lower left
                     1,  1, 1, 1, # upper right
                     1, -1, 1, 0, # lower right
                ]
            )
        )
        self.quad = self.ctx.vertex_array(
            self.program,
            [
                (self.vertices, '2f 2f', 'in_position', 'in_uv'),
            ]
        )

    def on_render(self, texture, target=None):
        if target:
            target.use()
        else:
            self.fbo.use()

        texture.use(0)
        self.quad.render(mode=moderngl.TRIANGLE_STRIP)

    def write(self, name):
        image = Image.frombytes("RGBA", self.fbo.size, self.fbo.read(components=4))
        image = image.transpose(Image.FLIP_TOP_BOTTOM)
        image.save(name, format="png")


if __name__ == "__main__":
    ImageProcessing.run()
