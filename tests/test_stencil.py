"""
"""
import struct
from functools import reduce
from operator import mul

import moderngl
import numpy as np
import pytest


class TestStencil:

    @pytest.fixture(autouse=True)
    def ctx_inject(self, ctx):
        self.__class__.ctx = ctx

    @pytest.fixture
    @staticmethod
    def np_triangle_rasterized():
        def _build_np_triangle_rasterized(size):
            # It should have 0.5's where the triangle lies.
            depth_value_from_triangle_vertices = 0.0
            # Map [-1, +1] -> [0, 1]
            depth_value_in_depth_buffer = (depth_value_from_triangle_vertices + 1) * 0.5
            return np.array(
                [
                    [depth_value_in_depth_buffer] * (size[0] - (j + 1)) + [1.0] * (j + 1)
                    for j in range(size[1])
                ]
            )

        return _build_np_triangle_rasterized

    def test_stencil(self, np_triangle_rasterized):
        self.ctx.enable(moderngl.DEPTH_TEST | moderngl.STENCIL_TEST)

        prog = self.ctx.program(
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
        vertices = np.array([[-1, 1, 0.], [-1, -1, 0.], [1, -1, 0.], ])
        vbo_triangle = self.ctx.buffer(vertices.astype('f4').tobytes())
        size = (16,) * 2
        depth_clear = 1.0
        tex_depth = self.ctx.depth_texture(size, use_stencilbuffer=True)  # implicit -> dtype='f4', components=1
        fbo_depth = self.ctx.framebuffer(depth_attachment=tex_depth)
        # vertex array object of triangle with depth pass prog
        vao_triangle = self.ctx.simple_vertex_array(prog, vbo_triangle, 'in_vert')

        self.ctx.stencil_func = ("1", 1, 0xFF)
        self.ctx.stencil_op = (moderngl.KEEP, moderngl.KEEP, moderngl.KEEP)
        fbo_depth.use()
        fbo_depth.clear(depth=depth_clear, stencil=0)
        # Now we render a triangle in there
        vao_triangle.render()

        depth_from_dbo = np.frombuffer(tex_depth.read(), dtype=np.dtype('f4')).reshape(size[::-1])
        ############################################################################
        # EXPECTED DATAS
        # It should have 0.5's where the triangle lies.
        ############################################################################
        np_triangle_raster = np_triangle_rasterized(size)
        # some stuff should be 1s and 0.5 where the triangle lies.
        np.testing.assert_array_almost_equal(depth_from_dbo, np_triangle_raster)

        self.ctx.stencil_func = ("==", 1, 0xFF)
        self.ctx.stencil_op = (moderngl.KEEP, moderngl.KEEP, moderngl.KEEP)
        fbo_depth.use()
        fbo_depth.clear(depth=depth_clear, stencil=0)
        # Now we render a triangle in there
        vao_triangle.render()

        depth_from_dbo = np.frombuffer(tex_depth.read(), dtype=np.dtype('f4')).reshape(size[::-1])
        # no pixel are rendered
        assert depth_from_dbo.sum() == reduce(mul, size)

    def test_invalid_stencil_func(self):
        # TypeError: Not iterable
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_func = "1"

        # Incorrect tuple size
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_func = "1",

        # TypeError: "Test" is not an integer
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_func = "1", "Test"

        # TypeError: "Test" is not an integer
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_func = "1", 0x0, "Test"

        # Incorrect tuple size=2
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_func = "1", 0x0,

        # Incorrect tuple size=4
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_func = ("1", 0x0, 0x0, 0x0)

        # "==" is GL_EQUAL => "=" doesn't exist !
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_func = ("=", 0, ~0)

        # wrong type for the 1st argument: expected valid string, got integer)
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_func = (42, 0, ~0)

        # wrong type for the 1st argument: expected valid string, got integer)
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_func = (42, "1", 0, ~0)

        # invert 1st and 2nd arguments
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_func = ("1", "front_and_back", 0, ~0)

        # Working versions
        self.ctx.stencil_func = "1", 0, ~0
        self.ctx.stencil_func = "front_and_back", "1", 0, ~0
        self.ctx.stencil_func = "front", "1", 0, ~0
        self.ctx.stencil_func = "back", "1", 0, ~0

    def test_invalid_stencil_op(self):
        # TypeError: Not iterable
        with pytest.raises(TypeError):
            self.ctx.stencil_op = moderngl.KEEP

        # TypeError: "Test" is not an integer
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_op = moderngl.KEEP, "Test"

        # Incorrect tuple size=1
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_op = moderngl.KEEP,

        # Incorrect tuple size=2
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_op = moderngl.KEEP, moderngl.ZERO,

        # Incorrect tuple size=5
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_op = "front_and_back", moderngl.KEEP, moderngl.ZERO, moderngl.KEEP, moderngl.ZERO

        # Good tuple size=4 but wrong type for the first argument (expected string got int)
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_op = 42, moderngl.ZERO, moderngl.ZERO, moderngl.ZERO
        # Good tuple size=4 but wrong string value for the first argument ("42" means nothing for selected face name)
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_op = "42", moderngl.ZERO, moderngl.ZERO, moderngl.ZERO
        # Good tuple size=4 but wrong type for the second argument (expected int got string)
        with pytest.raises(moderngl.Error):
            self.ctx.stencil_op = "front", "moderngl.ZERO", moderngl.ZERO, moderngl.ZERO

        # Working versions
        # use implicitely 'front_and_back' for selected face
        self.ctx.stencil_op = moderngl.KEEP, moderngl.ZERO, moderngl.REPLACE
        self.ctx.stencil_op = "front_and_back", moderngl.INCR, moderngl.INCR_WRAP, moderngl.DECR
        self.ctx.stencil_op = "front", moderngl.DECR_WRAP, moderngl.KEEP, moderngl.KEEP
        self.ctx.stencil_op = "back", moderngl.KEEP, moderngl.KEEP, moderngl.KEEP

    def test_get_values(self):
        with pytest.raises(NotImplementedError):
            self.ctx.stencil_func
        with pytest.raises(NotImplementedError):
            self.ctx.stencil_op
