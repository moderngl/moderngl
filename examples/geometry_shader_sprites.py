"""
Quick and dirty example showing how sprites can be rendered using a geometry shader.
We also show simple scrolling with projection matrix.

The goal is to redice the sprite data on the client as much as possible.
We can define a sprite by its position, size and rotation. This can be
expressed in 5 32 bit floats. This also opens up for individually rotating
each sprite in the shader itself. This technique can be extended with more
sprite parameters.

Other optimizations that can be done:
* Cull sprites outside the viewport in geo shader
"""
import math

import moderngl
from ported._example import Example

from pyrr import Matrix44
from array import array


class GeoSprites(Example):

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.ball_texture = self.load_texture_2d("ball.png")

        # Sprite shader using geometry shader
        self.program = self.ctx.program(
            vertex_shader="""
            #version 330

            // The per sprite input data
            in vec2 in_position;
            in vec2 in_size;
            in float in_rotation;

            out vec2 size;
            out float rotation;

            void main() {
                // We just pass the values unmodified to the geometry shader
                gl_Position = vec4(in_position, 0, 1);
                size = in_size;
                rotation = in_rotation;
            }
            """,
            geometry_shader="""
            #version 330

            // We are taking single points form the vertex shader
            // and emitting 4 new vertices creating a quad/sprites
            layout (points) in;
            layout (triangle_strip, max_vertices = 4) out;

            uniform mat4 projection;

            // Since geometry shader can take multiple values from a vertex
            // shader we need to define the inputs from it as arrays.
            // In our instance we just take single values (points)
            in vec2 size[];
            in float rotation[];
            out vec2 uv;

            void main() {
                // We grab the position value from the vertex shader
                vec2 center = gl_in[0].gl_Position.xy;
                // Calculate the half size of the sprites for easier calculations
                vec2 hsize = size[0] / 2.0;
                // Convert the rotation to radians
                float angle = radians(rotation[0]);

                // Create a 2d rotation matrix
                mat2 rot = mat2(
                    cos(angle), sin(angle),
                    -sin(angle), cos(angle)
                );

                // Emit a triangle strip creating a quad (4 vertices).
                // Here we need to make sure the rotation is applied before we position the sprite.
                // We just use hardcoded texture coordinates here. If an atlas is used we
                // can pass an additional vec4 for specific texture coordinates.

                // Each EmitVertex() emits values down the shader pipeline just like a single
                // run of a vertex shader, but in geomtry shaders we can do it multiple times!

                // Upper left
                gl_Position = projection * vec4(rot * vec2(-hsize.x, hsize.y) + center, 0.0, 1.0);
                uv = vec2(0, 1);
                EmitVertex();

                // lower left
                gl_Position = projection * vec4(rot * vec2(-hsize.x, -hsize.y) + center, 0.0, 1.0);
                uv = vec2(0, 0);
                EmitVertex();

                // upper right
                gl_Position = projection * vec4(rot * vec2(hsize.x, hsize.y) + center, 0.0, 1.0);
                uv = vec2(1, 1);
                EmitVertex();

                // lower right
                gl_Position = projection * vec4(rot * vec2(hsize.x, -hsize.y) + center, 0.0, 1.0);
                uv = vec2(1, 0);
                EmitVertex();

                // We are done with this triangle strip now
                EndPrimitive();
            }

            """,
            fragment_shader="""
            #version 330

            uniform sampler2D sprite_texture;

            in vec2 uv;
            out vec4 fragColor;
            
            void main() {
                fragColor = texture(sprite_texture, uv);
            }
            """,
        )

        self.sprite_data_size = 5 * 4  # 5 32 bit floats
        self.sprite_data = self.ctx.buffer(reserve=1000 * self.sprite_data_size)  # Capacity for 1000 sprites
        self.vao = self.ctx.vertex_array(
            self.program,
            [
                (self.sprite_data, "2f 2f 1f", "in_position", "in_size", "in_rotation"),
            ]
        )

    def render(self, time, frame_time):
        self.ctx.clear()
        self.ctx.enable(moderngl.BLEND)

        num_sprites = 16
        # We'll just generate some sprite data on the fly here.
        # This should only be necessary every time the sprite data changes (in a prefect wold)

        # Grab the size of the screen or current render target
        width, height = self.ctx.fbo.size

        # We just create a generator function instead of 
        def gen_sprites(time):
            rot_step = math.pi * 2 / num_sprites
            for i in range(num_sprites):
                # Position
                yield width / 2 + math.sin(time + rot_step * i) * 300
                yield height / 2 + math.cos(time + rot_step * i) * 300
                # size
                yield 100
                yield 100
                # rotation
                yield math.sin(time + i) * 200

        self.sprite_data.write(array("f", gen_sprites(time)))

        # calculate scroll offsets. We truncate to integers here.
        # This depends what "look" you want for your game.
        scroll_x, scroll_y = int(math.sin(time) * 100), int(math.cos(time) * 100)

        # Create a orthogonal projection matrix
        # Let's also modify the projection to scroll the entire screen.
        projection = Matrix44.orthogonal_projection(
            scroll_x,   # left
            width + scroll_x,   # right
            height + scroll_y,   # top
            scroll_y,   # bottom
            1,   # near
            -1,  # far
            dtype="f4",  # ensure we create 32 bit value (64 bit is default)
        )

        # Update the projection uniform
        self.program["projection"].write(projection)
        # Configure the sprite_texture uniform to read from texture channel 0
        self.program["sprite_texture"] = 0
        # Bind the texture to channel 0
        self.ball_texture.use(0)
        # Since we have overallocated the buffer (room for 1000 sprites) we 
        # need to specify how many we actually want to render passing number of vertices.
        # Also the mode needs to be the same as the geometry shader input type (points!)
        self.vao.render(mode=moderngl.POINTS, vertices=num_sprites)


if __name__ == "__main__":
    GeoSprites.run()
