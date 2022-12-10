# coding=utf-8
import os
import moderngl as mgl
import numpy as np
import pytest

# https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html
# https://en.wikipedia.org/wiki/Machine_epsilon
# binary32	single precision	float	2	24 (one bit is implicit)	2−24 ≈ 5.96e-08	2−23 ≈ 1.19e-07
EPSILON_IEEE_754 = 5.96e-08


@pytest.fixture(scope="session", autouse=True)
def standalone_context():
    # # HACK for CI
    # if os.environ.get("CI"):
    #     return None
    return mgl.create_standalone_context()


@pytest.fixture
def prog_render_depth_pass(standalone_context):
    return standalone_context.program(
        vertex_shader='''
                #version 330

                in vec3 in_vert;

                void main() {
                    gl_Position = vec4(in_vert, 1.0);
                }
            ''',
        fragment_shader='''
                #version 330

                void main() {
                }
            ''',
    )


@pytest.fixture
def prog_draw_depth(standalone_context):
    prog = standalone_context.program(
        vertex_shader='''
                    #version 330

                    in vec2 in_vert;
                    out vec2 uv;

                    void main() {
                        uv = in_vert;
                        gl_Position = vec4(in_vert * 2.0 - 1.0, 0.0, 1.0);
                    }
                ''',
        # from `texture.py`
        fragment_shader='''
                    #version 330

                    uniform sampler2D depth;

                    in vec2 uv;
                    out float fragColor;

                    void main() {
                        float raw_depth_nonlinear = texture(depth, uv).r;
                        fragColor = raw_depth_nonlinear;
                    }
                ''',
    )
    prog['depth'].value = 0
    return prog


@pytest.fixture
def vbo_triangle(standalone_context):
    vertices = np.array([[-1, 1, 0.], [-1, -1, 0.], [1, -1, 0.], ])
    return standalone_context.buffer(vertices.astype('f4').tobytes())


@pytest.fixture
def vbo_quad(standalone_context):
    canvas_fs_quad = np.array(
        [0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0]).astype('f4')
    return standalone_context.buffer(canvas_fs_quad.tobytes())


@pytest.fixture
def prog_shadow_test(standalone_context):
    prog = standalone_context.program(
        vertex_shader='''
                #version 330

                in vec2 in_vert;
                out vec2 uv;

                void main() {
                    uv = in_vert;
                    gl_Position = vec4(in_vert * 2.0 - 1.0, 0.0, 1.0);
                }
        ''',
        fragment_shader='''
                #version 330

                uniform sampler2DShadow depth;

                uniform vec2 u_shadow_coord_offset;

                uniform float u_depth_bias;
                uniform float u_depth_triangle;

                in vec2 uv;
                out float fragColor;

                void main() {
                    // Compute depth caster
                    float depth_shadow_caster = u_depth_triangle + u_depth_bias;
                    // Compute shadow coordinates + depth of caster
                    vec3 shadow_coord = vec3(uv, depth_shadow_caster);
                    shadow_coord += vec3(u_shadow_coord_offset, 0.0);
                    // Compute visibility:
                    // -> shadow test: receiver (potential) vs caster
                    float visibility = texture(depth, shadow_coord);
                    // Output the visibility
                    fragColor = visibility;
                }
            ''',
    )
    #
    prog['u_depth_bias'].value = EPSILON_IEEE_754
    depth_triangle_in_obj_space = 0
    depth_triangle_in_view_space = (depth_triangle_in_obj_space + 1) * 0.5
    prog['u_depth_triangle'].value = depth_triangle_in_view_space
    #
    return prog


@pytest.fixture
def fbo_with_rasterised_triangle(standalone_context, vbo_triangle):
    def _build_fbo_with_rasterised_triangle(prog, size=(4, 4), depth_clear=1.0):
        tex_depth = standalone_context.depth_texture(size)  # implicit -> dtype='f4', components=1
        fbo_depth = standalone_context.framebuffer(depth_attachment=tex_depth)
        fbo_depth.use()
        fbo_depth.clear(depth=depth_clear)

        # vertex array object of triangle with depth pass prog
        vao_triangle = standalone_context.simple_vertex_array(prog, vbo_triangle, 'in_vert')
        # Now we render a triangle in there
        vao_triangle.render()

        return fbo_depth, tex_depth

    return _build_fbo_with_rasterised_triangle


@pytest.fixture
def np_triangle_rasterised():
    def _build_np_triangle_rastised(size):
        # It should have 0.5's where the triangle lies.
        depth_value_from_triangle_vertices = 0.0
        # Map [-1, +1] -> [0, 1]
        depth_value_in_depthbuffer = (depth_value_from_triangle_vertices + 1) * 0.5
        return np.array(
            [
                [depth_value_in_depthbuffer] * (size[0] - (j + 1)) + [1.0] * (j + 1)
                for j in range(size[1])
            ]
        )

    return _build_np_triangle_rastised
