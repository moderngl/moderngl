"""

"""
import numpy as np

import moderngl as mgl


def test_cull_face(standalone_context,
                   prog_render_depth_pass,
                   vbo_triangle,
                   fbo_with_rasterised_triangle,
                   np_triangle_rasterised):
    ctx = standalone_context

    size = (16,) * 2

    ctx.enable(mgl.DEPTH_TEST)
    ctx.enable(mgl.CULL_FACE)

    def _do_test_cull_face(cull_face: str, expected_results: np.array):
        ctx.cull_face = cull_face
        fbo_depth, tex_depth = fbo_with_rasterised_triangle(prog_render_depth_pass, size)
        depth_from_dbo = np.frombuffer(tex_depth.read(), dtype=np.dtype('f4')).reshape(size[::-1])
        np.testing.assert_array_almost_equal(depth_from_dbo, expected_results)

    ############################################################################
    # EXPECTED DATAS                                                                                                   #
    # It should have 0.5's where the triangle lies.                                                                    #
    ############################################################################
    np_triangle_raster = np_triangle_rasterised(size)
    ############################################################################
    # cull face is set to back, only one 'front' triangle in the scene => no culling
    _do_test_cull_face('back', np_triangle_raster)

    ############################################################################
    # EXPECTED DATAS                                                                                                   #
    # It should have 1.0's everywhere.                                                                    #
    ############################################################################
    np_clear_depth = np.full(size, 1.0)
    ############################################################################
    # cull face is set to front, only one 'front' triangle in the scene => the triangle is culled
    _do_test_cull_face('front', np_clear_depth)

    # cull face is set to front_and_back, only one 'front' triangle in the scene => the triangle is culled
    _do_test_cull_face('front_and_back', np_clear_depth)

    ctx.enable_only(mgl.NOTHING)
