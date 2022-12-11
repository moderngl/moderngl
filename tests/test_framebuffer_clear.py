
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
