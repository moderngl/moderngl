"""
A Game of Life implementation using transform feedback.

We calculate the next state of the map with transform()
meaning a vertex shader will generate the new state into a buffer.
This buffer is then written into the texture we display.
This is a fast vram to vram copy.

Comments:

Another way to do this is simply rendering to Framebuffers.
"""
import numpy as np

import moderngl
from ported._example import Example


class Conway(Example):
    title = "Conway's Game of Life"
    window_size = 800, 800

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        # How often the map should be updated
        self.update_delay = 1 / 60  # updates per second
        self.last_updated = 0
        # size of the map
        self.width, self.height = 400, 400
        # Force the window to calculate black borders if needed to retain the aspect ratio
        self.wnd.fixed_aspect_ratio = self.width / self.height
        # Initial state of the map (random)
        pixels = np.round(np.random.rand(self.width, self.height)).astype('f4')

        # Program drawing the result to the screen.
        # This is rendered simply using a textured screen aligned triangle strip
        self.display_prog = self.ctx.program(
            vertex_shader='''
                #version 330

                in vec2 in_vert;
                in vec2 in_texcoord;

                out vec2 v_text;

                void main() {
                    v_text = in_texcoord;
                    gl_Position = vec4(in_vert, 0.0, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330

                // Will read from texture bound to channel / locaton 0 by default
                uniform sampler2D Texture;

                // Interpolated texture coordinate from vertex shader
                in vec2 v_text;
                // The fragment ending up on the screen
                out vec4 f_color;

                void main() {
                    f_color = texture(Texture, v_text);
                }
            ''',
        )

        # Program calculating the next state of the map
        self.transform_prog = self.ctx.program(
            vertex_shader='''
                #version 330

                uniform sampler2D Texture;

                out float out_vert;

                #define LIVING 0.0
                #define DEAD 1.0

                bool cell(int x, int y) {
                    // get the texture size
                    ivec2 tSize = textureSize(Texture, 0).xy;
                    // Ensure lookups are not going outside the texture area because
                    // texelFetch do not support texture repeat / wrap modes
                    return texelFetch(Texture, ivec2((x + tSize.x) % tSize.x, (y + tSize.y) % tSize.y), 0).r < 0.5;
                }

                void main() {
                    int width = textureSize(Texture, 0).x;
                    ivec2 in_text = ivec2(gl_VertexID % width, gl_VertexID / width);
                    bool living = cell(in_text.x, in_text.y);

                    int neighbours = 0;
                    if (cell(in_text.x - 1, in_text.y - 1)) neighbours++;
                    if (cell(in_text.x - 1, in_text.y + 0)) neighbours++;
                    if (cell(in_text.x - 1, in_text.y + 1)) neighbours++;
                    if (cell(in_text.x + 1, in_text.y - 1)) neighbours++;
                    if (cell(in_text.x + 1, in_text.y + 0)) neighbours++;
                    if (cell(in_text.x + 1, in_text.y + 1)) neighbours++;
                    if (cell(in_text.x + 0, in_text.y + 1)) neighbours++;
                    if (cell(in_text.x + 0, in_text.y - 1)) neighbours++;

                    if (living) {
                        out_vert = (neighbours == 2 || neighbours == 3) ? LIVING : DEAD;
                    } else {
                        out_vert = (neighbours == 3) ? LIVING : DEAD;
                    }
                }
            ''',
            varyings=['out_vert']
        )

        # Create the map texture
        self.texture = self.ctx.texture((self.width, self.height), 1, pixels.tobytes(), dtype='f4')
        self.texture.filter = moderngl.NEAREST, moderngl.NEAREST
        self.texture.swizzle = 'RRR1'  # What components texelFetch will get from the texture (in shader)

        # A quad covering the screen with texture coordinates
        self.vbo = self.ctx.buffer(np.array([
            # x    y     u  v
            -1.0, -1.0,  0, 0,  # lower left
            -1.0,  1.0,  0, 1,  # upper left
            1.0,  -1.0,  1, 0,  # lower right
            1.0,   1.0,  1, 1,  # upper right
        ], dtype="f4"))
        self.vao = self.ctx.simple_vertex_array(self.display_prog, self.vbo, 'in_vert', 'in_texcoord')

        # Transform vertex array to generate new map state
        self.tao = self.ctx.vertex_array(self.transform_prog, [])
        self.pbo = self.ctx.buffer(reserve=pixels.nbytes)  # buffer to store the result

    def render(self, time, frame_time):
        self.ctx.clear(1.0, 1.0, 1.0)

        # Bind texture to channel 0
        self.texture.use(location=0)

        if time - self.last_updated > self.update_delay:
            # Generate the new map and write that to the pbo buffer
            self.tao.transform(self.pbo, vertices=self.width * self.height)
            # Copy the pbo into the texture
            self.texture.write(self.pbo)
            self.last_updated = time

        # Render the texture
        self.vao.render(moderngl.TRIANGLE_STRIP)


if __name__ == '__main__':
    Conway.run()
