import pytest
import numpy as np
import moderngl as mgl

@pytest.fixture
def prog_render_depth_pass(ctx):
    return ctx.program(
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
def vbo_triangle(ctx):
    vertices = np.array([[-1, 1, 0.], [-1, -1, 0.], [1, -1, 0.], ])
    return ctx.buffer(vertices.astype('f4').tobytes())

@pytest.fixture
def fbo_with_rasterised_triangle(ctx, vbo_triangle):
    def _build_fbo_with_rasterised_triangle(prog, size=(4, 4), depth_clear=1.0):
        tex_depth = ctx.depth_texture(size)  # implicit -> dtype='f4', components=1
        fbo_depth = ctx.framebuffer(depth_attachment=tex_depth)
        fbo_depth.use()
        fbo_depth.clear(depth=depth_clear)

        # vertex array object of triangle with depth pass prog
        vao_triangle = ctx.simple_vertex_array(prog, vbo_triangle, 'in_vert')
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


def test_cull_face(ctx,
                   prog_render_depth_pass,
                   vbo_triangle,
                   fbo_with_rasterised_triangle,
                   np_triangle_rasterised):

    size = (16,) * 2

    ctx.enable(mgl.DEPTH_TEST)
    ctx.enable(mgl.CULL_FACE)

    def _do_test_cull_face(cull_face: str, expected_results: np.array):
        ctx.cull_face = cull_face
        fbo_depth, tex_depth = fbo_with_rasterised_triangle(prog_render_depth_pass, size)
        depth_from_dbo = np.frombuffer(tex_depth.read(), dtype=np.dtype('f4')).reshape(size[::-1])
        np.testing.assert_array_almost_equal(depth_from_dbo, expected_results)

    ############################################################################
    # EXPECTED DATAS#
    # It should have 0.5's where the triangle lies.
    ############################################################################
    np_triangle_raster = np_triangle_rasterised(size)
    ############################################################################
    # cull face is set to back, only one 'front' triangle in the scene => no culling
    _do_test_cull_face('back', np_triangle_raster)

    ############################################################################
    # EXPECTED DATAS
    # It should have 1.0's everywhere.
    ############################################################################
    np_clear_depth = np.full(size, 1.0)
    ############################################################################
    # cull face is set to front, only one 'front' triangle in the scene => the triangle is culled
    _do_test_cull_face('front', np_clear_depth)

    # cull face is set to front_and_back, only one 'front' triangle in the scene => the triangle is culled
    _do_test_cull_face('front_and_back', np_clear_depth)

    ctx.enable_only(mgl.NOTHING)
