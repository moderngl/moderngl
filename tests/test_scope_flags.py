import struct

import pytest
from OpenGL import GL

import moderngl


FLAGS = (
    (moderngl.BLEND, GL.GL_BLEND),
    (moderngl.DEPTH_TEST, GL.GL_DEPTH_TEST),
    (moderngl.CULL_FACE, GL.GL_CULL_FACE),
    (moderngl.RASTERIZER_DISCARD, GL.GL_RASTERIZER_DISCARD),
    (moderngl.PROGRAM_POINT_SIZE, GL.GL_PROGRAM_POINT_SIZE),
)


def assert_enabled(flags):
    for flag, gl_flag in FLAGS:
        assert bool(GL.glIsEnabled(gl_flag)) == bool(flags & flag)


@pytest.mark.parametrize("flags", [
    moderngl.NOTHING,
    moderngl.BLEND,
    moderngl.DEPTH_TEST,
    moderngl.CULL_FACE,
    moderngl.RASTERIZER_DISCARD,
    moderngl.PROGRAM_POINT_SIZE,
    moderngl.BLEND | moderngl.DEPTH_TEST | moderngl.CULL_FACE,
    sum(flag for flag, _ in FLAGS),
])
def test_default_scope_inherits_flags_at_entry(ctx_new, flags):
    previous = ctx_new.simple_framebuffer((2, 2))
    target = ctx_new.simple_framebuffer((2, 2))
    nested = ctx_new.simple_framebuffer((2, 2))
    previous.use()
    scope = ctx_new.scope(target)
    ctx_new.enable_only(flags)

    with scope:
        assert_enabled(flags)
        with ctx_new.scope(nested, enable_only=0):
            assert_enabled(0)
        assert_enabled(flags)
        with ctx_new.scope(nested):
            assert_enabled(flags)
        assert_enabled(flags)
    assert_enabled(flags)


@pytest.mark.parametrize("settings", [
    {"enable_only": 0},
    {"enable_only": moderngl.CULL_FACE},
    {"enable": moderngl.CULL_FACE},
])
def test_explicit_scope_flags(ctx_new, settings):
    target = ctx_new.simple_framebuffer((2, 2))
    target.use()
    ctx_new.enable_only(moderngl.BLEND)
    expected = settings.get("enable_only", settings.get("enable"))
    with ctx_new.scope(target, **settings):
        assert_enabled(expected)
    assert_enabled(moderngl.BLEND)


def test_default_scope_reuse_and_exception(ctx_new):
    target = ctx_new.simple_framebuffer((2, 2))
    target.use()
    scope = ctx_new.scope(target, enable_only=None)
    for flags in (moderngl.BLEND, moderngl.DEPTH_TEST):
        ctx_new.enable_only(flags)
        with pytest.raises(ValueError, match="render failed"):
            with scope:
                assert_enabled(flags)
                ctx_new.enable_only(moderngl.CULL_FACE)
                raise ValueError("render failed")
        assert_enabled(flags)


def test_default_scope_preserves_alpha_blending(ctx_new):
    target = ctx_new.simple_framebuffer((1, 1), dtype="f4")
    target.use()
    target.clear(0, 1, 0, 1)
    program = ctx_new.program(
        vertex_shader="""
            #version 330
            const vec2 positions[3] = vec2[3](vec2(-1,-1), vec2(3,-1), vec2(-1,3));
            void main() { gl_Position = vec4(positions[gl_VertexID], 0, 1); }
        """,
        fragment_shader="""
            #version 330
            out vec4 color;
            void main() { color = vec4(1, 0, 0, 0.5); }
        """,
    )
    vao = ctx_new.vertex_array(program, [])
    ctx_new.enable_only(moderngl.BLEND)
    ctx_new.blend_func = (moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA)
    with ctx_new.scope(target):
        vao.render(vertices=3)
    actual = struct.unpack("4f", target.read(components=4, dtype="f4"))
    assert actual == pytest.approx((0.5, 0.5, 0, 0.75))
