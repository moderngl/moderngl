"""
Demonstrates redering a terrain/height map on the fly without any
pre-generated geometry.
"""

import numpy as np
from pyrr import Matrix44, Matrix33

import moderngl
from _example import Example


class HeightmapOnTheFly(Example):
    title = "Heightmap - On the fly"
    gl_version = (3, 3)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader="""
                #version 330

                uniform int dim;
                out vec2 uv;

                void main() {
                    // grid position from gl_VertexID normalized
                    vec2 pos = vec2(gl_VertexID % dim, gl_VertexID / dim) / dim;
                    gl_Position = vec4(pos, 0.0, 1.0);
                }
            """,
            geometry_shader="""
            #version 330

            uniform sampler2D heightmap;

            uniform mat4 projection;
            uniform mat4 modelview;
            uniform mat3 normal_matrix;
            uniform int dim;
            uniform float terrain_size;

            out vec2 g_uv;
            // out vec3 g_pos;
            out vec3 normal;

            layout(points) in;
            layout(triangle_strip, max_vertices = 4) out;

            const float scale = 0.5;
            const float height = -0.15;

            float calculateHeight(float h) {
                return h * scale + height;
            }

            vec3 calculateNormal(vec2 uv, float step, float size) {
                float hl = calculateHeight(texture(heightmap, uv + vec2(-step, 0.0)).r);
                float hr = calculateHeight(texture(heightmap, uv + vec2(step, 0.0)).r);
                float hu = calculateHeight(texture(heightmap, uv + vec2(0.0, step)).r);
                float hd = calculateHeight(texture(heightmap, uv + vec2(0.0, -step)).r);
                return normalize(vec3(hl - hr, hd - hu, size));
            }

            void main() {
                // width and height of a quad
                float size = terrain_size / dim;

                // lower left corner of the quad
                vec2 pos = gl_in[0].gl_Position.xy * terrain_size - terrain_size / 2.0;
                vec2 uv = gl_in[0].gl_Position.xy;
                float uv_step = 1.0 / dim;

                // Calculate mvp
                mat4 mvp = projection * modelview;

                // Read heights for each corner
                vec2 uv1 = uv + vec2(0.0, uv_step);
                float h1 = calculateHeight(texture(heightmap, uv1).r);

                vec2 uv2 = uv;
                float h2 = calculateHeight(texture(heightmap, uv2).r);

                vec2 uv3 = uv + vec2(uv_step, uv_step);
                float h3 = calculateHeight(texture(heightmap, uv3).r);

                vec2 uv4 = uv + vec2(uv_step, 0.0);
                float h4 = calculateHeight(texture(heightmap, uv4).r);

                // Upper left
                vec4 pos1 = vec4(pos + vec2(0.0, size), h1, 1.0);
                gl_Position = mvp * pos1;
                g_uv = uv1;
                normal = normal_matrix * calculateNormal(uv1, uv_step, size);
                // g_pos = (modelview * pos1).xyz;
                EmitVertex();

                // Lower left
                vec4 pos2 = vec4(pos, h2, 1.0);
                gl_Position = mvp * pos2;
                g_uv = uv2;
                normal = normal_matrix * calculateNormal(uv2, uv_step, size);
                // g_pos = (modelview * pos2).xyz;
                EmitVertex();

                // Upper right
                vec4 pos3 = vec4(pos + vec2(size, size), h3, 1.0);
                gl_Position = mvp * pos3;
                g_uv = uv3;
                normal = normal_matrix * calculateNormal(uv3, uv_step, size);
                // g_pos = (modelview * pos3).xyz;
                EmitVertex();

                // Lower right
                vec4 pos4 = vec4(pos + vec2(size, 0.0), h4, 1.0);
                gl_Position = mvp * pos4;
                g_uv = uv4;
                normal = normal_matrix * calculateNormal(uv4, uv_step, size);
                // g_pos = (modelview * pos4).xyz;
                EmitVertex();

                EndPrimitive();
            }
            """,
            fragment_shader="""
                #version 330

                uniform sampler2D heightmap;
                out vec4 fragColor;
                in vec2 g_uv;
                // in vec3 g_pos;
                in vec3 normal;

                void main() {
                    // vec3 normal = normalize(cross(dFdx(g_pos), dFdy(g_pos)));
                    float l = abs(dot(vec3(0, 0, 1), normal));

                    // fragColor = vec4(vec3(texture(heightmap, g_uv).r) * l, 1.0);
                    // fragColor = vec4(normal * l, 1.0);
                    fragColor = vec4(vec3(1.0) * l, 1.0);
                }
            """,
        )
        self.heightmap = self.load_texture_2d('heightmap_detailed.png')
        self.heightmap.repeat_x = False
        self.heightmap.repeat_y = False
        self.dim = self.heightmap.width
        self.vao = self.ctx.vertex_array(self.prog, [])

        projection = Matrix44.perspective_projection(45.0, self.aspect_ratio, 0.1, 1000.0, dtype='f4')
        self.prog['projection'].write(projection)
        self.prog['dim'] = self.dim - 1
        self.prog['terrain_size'] = 1.0

    def on_render(self, time, frame_time):
        self.ctx.clear()
        self.ctx.enable(moderngl.DEPTH_TEST | moderngl.CULL_FACE)
        angle = time * 0.2

        lookat = Matrix44.look_at(
            (np.cos(angle), np.sin(angle), 0.4),
            (0.0, 0.0, 0.0),
            (0.0, 0.0, 1.0),
            dtype='f4',
        )
        normal_matrix = Matrix33.from_matrix44(lookat).inverse.transpose()

        self.prog['modelview'].write(lookat)
        self.prog['normal_matrix'].write(normal_matrix.astype('f4').tobytes())
        self.heightmap.use(0)
        self.vao.render(moderngl.POINTS, vertices=(self.dim - 1) ** 2)


if __name__ == '__main__':
    HeightmapOnTheFly.run()
