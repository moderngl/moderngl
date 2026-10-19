
import numpy as np
import pytest

import moderngl


def test_clear_inactive_framebuffer_does_not_inherit_scissor(ctx):
    active = ctx.simple_framebuffer((4, 4), components=4)
    target = ctx.simple_framebuffer((4, 4), components=4)
    target.use()
    target.clear(0.0, 0.0, 0.0, 1.0)
    active.use()
    active.scissor = (1, 1, 2, 2)

    target.clear(1.0, 0.0, 0.0, 1.0)

    assert target.read(components=4) == b'\xff\x00\x00\xff' * 16
    assert ctx.fbo is active
    assert active.scissor == (1, 1, 2, 2)


@pytest.mark.parametrize('viewport', [None, (0, 0, 3, 3)])
def test_clear_inactive_framebuffer_restores_render_state(ctx, color_prog, ndc_quad, viewport):
    active = ctx.simple_framebuffer((4, 4), components=4)
    target = ctx.simple_framebuffer((4, 4), components=4)
    active.use()
    active.clear(0.0, 0.0, 0.0, 1.0, depth=1.0)
    active.scissor = (0, 0, 2, 2)
    active.color_mask = True, False, False, False
    active.depth_mask = False
    target.scissor = (2, 2, 1, 1)
    ctx.enable_only(moderngl.DEPTH_TEST)
    color_prog['color'].value = (1.0, 1.0, 1.0, 1.0)
    vao = ctx.simple_vertex_array(color_prog, ndc_quad, 'in_vert')

    target.clear(viewport=viewport)
    vao.render(moderngl.TRIANGLE_STRIP)

    expected = np.zeros((4, 4, 4), dtype=np.uint8)
    expected[:, :, 3] = 255
    expected[:2, :2, 0] = 255
    actual = np.frombuffer(active.read(components=4), dtype=np.uint8).reshape(4, 4, 4)
    np.testing.assert_array_equal(actual, expected)
    depth = np.frombuffer(active.read(components=1, attachment=-1, dtype='f4'), dtype=np.float32)
    np.testing.assert_array_equal(depth, np.ones(16, dtype=np.float32))
    assert ctx.fbo is active


def test_1(ctx):
    fbo1 = ctx.simple_framebuffer((4, 4))
    fbo2 = ctx.simple_framebuffer((4, 4))

    fbo1.clear(0.0, 0.0, 0.0, 0.0)

    assert fbo1.read() == b'\x00\x00\x00' * 4 * 4

    fbo1.clear(1.0, 0.0, 0.0, 0.0, viewport=(2, 2))
    fbo1.clear(0.0, 1.0, 0.0, 0.0, viewport=(2, 0, 2, 2))
    fbo1.clear(0.0, 0.0, 1.0, 0.0, viewport=(0, 2, 2, 2))

    assert fbo1.read((2, 2)), b'\xff\x00\x00' * 2 * 2
    assert fbo1.read((2, 0, 2, 2)), b'\x00\xff\x00' * 2 * 2
    assert fbo1.read((0, 2, 2, 2)), b'\x00\x00\xff' * 2 * 2
    assert fbo1.read((2, 2, 2, 2)), b'\x00\x00\x00' * 2 * 2

    ctx.copy_framebuffer(fbo2, fbo1)

    assert fbo2.read((2, 2)), b'\xff\x00\x00' * 2 * 2
    assert fbo2.read((2, 0, 2, 2)), b'\x00\xff\x00' * 2 * 2
    assert fbo2.read((0, 2, 2, 2)), b'\x00\x00\xff' * 2 * 2
    assert fbo2.read((2, 2, 2, 2)), b'\x00\x00\x00' * 2 * 2
