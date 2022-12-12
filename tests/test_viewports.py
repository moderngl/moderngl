
def test_viewport(ctx):
    fbo = ctx.simple_framebuffer((10, 10))
    fbo.use()
    assert ctx.viewport == (0, 0, 10, 10)
    assert fbo.viewport == (0, 0, 10, 10)
    ctx.viewport = 2, 3, 7, 8
    assert fbo.viewport == (2, 3, 7, 8)
    assert ctx.viewport == (2, 3, 7, 8)
    assert ctx.fbo.viewport == (2, 3, 7, 8)
