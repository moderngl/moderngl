import pytest
import numpy as np
import moderngl
import moderngl as mgl
from scipy import ndimage

# https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html
# https://en.wikipedia.org/wiki/Machine_epsilon
# binary32	single precision	float	2	24 (one bit is implicit)	2−24 ≈ 5.96e-08	2−23 ≈ 1.19e-07
EPSILON_IEEE_754 = 5.96e-08

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
def prog_draw_depth(ctx):
    prog = ctx.program(
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
def vbo_triangle(ctx):
    vertices = np.array([[-1, 1, 0.], [-1, -1, 0.], [1, -1, 0.], ])
    return ctx.buffer(vertices.astype('f4').tobytes())

@pytest.fixture
def vbo_quad(ctx):
    canvas_fs_quad = np.array(
        [0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0]).astype('f4')
    return ctx.buffer(canvas_fs_quad.tobytes())

@pytest.fixture
def prog_shadow_test(ctx):
    prog = ctx.program(
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


def test_depth_sampler(
    ctx,
    prog_render_depth_pass,
    prog_draw_depth,
    vbo_triangle,
    vbo_quad,
    fbo_with_rasterised_triangle,
    np_triangle_rasterised,
):
    ctx.enable(mgl.DEPTH_TEST)

    size = (16,) * 2
    fbo_depth, tex_depth = fbo_with_rasterised_triangle(prog_render_depth_pass, size)
    depth_from_dbo = np.frombuffer(tex_depth.read(), dtype=np.dtype('f4')).reshape(size[::-1])

    ############################################################################
    # EXPECTED DATAS
    # It should have 0.5's where the triangle lies.
    ############################################################################
    np_triangle_raster = np_triangle_rasterised(size)

    depth_tex_filter = (moderngl.NEAREST, moderngl.NEAREST)
    compare_func = ''

    def _do_test():
        # some stuff should be 1s and 0.5 where the triangle lies.
        np.testing.assert_array_almost_equal(depth_from_dbo, np_triangle_raster)

        fbo_draw_depth = ctx.framebuffer(color_attachments=[ctx.renderbuffer(size, components=1, dtype='f4')])
        fbo_draw_depth.use()

        tex_depth.use(location=0)

        # vertex array object of (fullscreen) quad with render depth prog
        vao_quad = ctx.simple_vertex_array(prog_draw_depth, vbo_quad, 'in_vert')
        vao_quad.render(moderngl.TRIANGLE_STRIP)

        data = np.frombuffer(fbo_draw_depth.read(components=1, dtype='f4'), dtype=np.dtype('f4')).reshape(size[::-1])
        np.testing.assert_array_almost_equal(data, np_triangle_raster)

    def _with_sampler():
        # from: https://github.com/Contraz/demosys-py/blob/master/demosys/opengl/texture.py
        depth_sampler = ctx.sampler(filter=depth_tex_filter, compare_func=compare_func)  # disable depth func
        depth_sampler.use(location=0)
        _do_test()

    def _with_texture_parameters():
        tex_depth.filter = depth_tex_filter
        tex_depth.compare_func = compare_func
        _do_test()

    _with_sampler()
    _with_texture_parameters()


def test_sampler_shadow(
    ctx,
    prog_render_depth_pass, prog_shadow_test,
    vbo_triangle, vbo_quad,
    fbo_with_rasterised_triangle,
):
    ctx.enable(mgl.DEPTH_TEST)

    size = (3,) * 2
    fbo_depth, tex_depth = fbo_with_rasterised_triangle(prog_render_depth_pass, size)

    ############################################################################
    # EXPECTED DATAS                                                                                                   #
    # It should have 1's where the triangle lies.                                                                      #
    ############################################################################
    texel_shadowed = 1
    np_triangle_raster_visibility = np.array(
        [
            [texel_shadowed] * (size[0] - (j + 1)) + [1 - texel_shadowed] * (j + 1)
            for j in range(size[1])
        ]
    )
    ############################################################################

    # No offset - fetch the center pixel (NEAREST)
    prog_shadow_test['u_shadow_coord_offset'].value = (0,) * 2

    depth_tex_filter = (moderngl.NEAREST, moderngl.NEAREST)
    compare_func = '>='

    def _do_test():
        fbo_draw_depth = ctx.framebuffer(color_attachments=[ctx.renderbuffer(size, components=1, dtype='f4')])
        fbo_draw_depth.use()

        tex_depth.use(location=0)

        # https://www.khronos.org/opengl/wiki/Sampler_(GLSL)#Shadow_samplers
        vao_fs_shadow = ctx.simple_vertex_array(prog_shadow_test, vbo_quad, 'in_vert')
        vao_fs_shadow.render(moderngl.TRIANGLE_STRIP)

        data = np.frombuffer(fbo_draw_depth.read(components=1, dtype='f4'), dtype=np.dtype('f4')).reshape(size[::-1])
        np.testing.assert_array_almost_equal(data, np_triangle_raster_visibility)

    def _with_sampler():
        # https://github.com/Contraz/demosys-py/blob/01f285cd3a132012e14a51bf0ae9d7aa5a489b55/demosys/opengl/texture.py#L151
        shadow_sampler = ctx.sampler(filter=depth_tex_filter, compare_func=compare_func)  # enable depth func
        shadow_sampler.use(location=0)
        _do_test()
        shadow_sampler.clear(location=0)

    def _with_texture_paramters():
        tex_depth.filter = depth_tex_filter
        tex_depth.compare_func = compare_func
        _do_test()

    _with_sampler()
    _with_texture_paramters()


def test_sampler_shadow_with_bilinear_interpolation(
        ctx,
        prog_render_depth_pass, prog_shadow_test,
        vbo_triangle, vbo_quad,
        fbo_with_rasterised_triangle
):
    ctx.enable(mgl.DEPTH_TEST)

    size = (7,) * 2

    fbo_depth, tex_depth = fbo_with_rasterised_triangle(prog_render_depth_pass, size)

    ############################################################################
    # EXPECTED DATAS                                                           #
    ############################################################################
    # Box filter (2x2) on triangle visibility rasterisation
    # is similar to bilinear interpolation with samplerShadow
    # https://docs.scipy.org/doc/scipy/reference/generated/scipy.ndimage.uniform_filter.html#scipy.ndimage.uniform_filter
    np_triangle_raster_visbility = np.array(
        [
            [1.0] * (size[0] - (j + 1)) + [0.0] * (j + 1)
            for j in range(size[1])
        ]
    )
    box_filter_on_triangle_raster_visibility = ndimage.uniform_filter(
        np_triangle_raster_visbility,
        size=2, mode='constant', cval=0, origin=-1)
    ############################################################################

    half_pixel_size = (1.0/size[0]*0.5, 1.0/size[1]*0.5)
    prog_shadow_test['u_shadow_coord_offset'].value = tuple(half_pixel_size)

    def _do_test():
        fbo_draw_depth = ctx.framebuffer(
            color_attachments=[ctx.renderbuffer(size,
                                                components=1, dtype='f4')])
        fbo_draw_depth.use()

        tex_depth.use(location=0)

        # https://www.khronos.org/opengl/wiki/Sampler_(GLSL)#Shadow_samplers
        vao_fs_shadow = ctx.simple_vertex_array(prog_shadow_test, vbo_quad,
                                                'in_vert')
        vao_fs_shadow.render(moderngl.TRIANGLE_STRIP)

        #
        data = np.frombuffer(fbo_draw_depth.read(components=1, dtype='f4'),
                             dtype=np.dtype('f4')).reshape(size[::-1])
        np.testing.assert_array_almost_equal(
            data, box_filter_on_triangle_raster_visibility)

    depth_tex_filter = (moderngl.LINEAR, moderngl.LINEAR)
    compare_func = '>='

    def _with_sampler():
        # https://github.com/Contraz/demosys-py/blob/01f285cd3a132012e14a51bf0ae9d7aa5a489b55/demosys/opengl/texture.py#L151
        shadow_sampler = ctx.sampler(
            filter=depth_tex_filter,
            compare_func=compare_func,  # enable depth func
            # from context.py:
            #   border_color (tuple): The (r, g, b, a) color for the texture border.
            #   When this value is set the ``repeat_`` values are overriden
            #   setting the texture wrap to return the border color when outside ``[0, 1]`` range.
            # border_color=(0, 0, 0, 0),    # seems not to work well ...
            repeat_x=False, repeat_y=False,
        )
        shadow_sampler.use(location=0)
        _do_test()
        shadow_sampler.clear(location=0)

    def _with_texture_paremeters():
        tex_depth.filter = depth_tex_filter
        tex_depth.repeat_x = False
        tex_depth.repeat_y = False
        tex_depth.compare_func = compare_func
        _do_test()

    _with_sampler()
    _with_texture_paremeters()
