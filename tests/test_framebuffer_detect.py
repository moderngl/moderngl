import pytest
from OpenGL import GL


@pytest.mark.parametrize("depth_only", [False, True])
@pytest.mark.parametrize("texture", [False, True])
@pytest.mark.parametrize("current", [False, True])
def test_detect_framebuffer_dimensions(ctx, depth_only, texture, current):
    previous = ctx.fbo
    size = (17, 11)
    depth = ctx.depth_texture(size) if texture else ctx.depth_renderbuffer(size)
    color = None if depth_only else ctx.texture(size, 4)
    raw = int(GL.glGenFramebuffers(1))
    GL.glBindFramebuffer(GL.GL_FRAMEBUFFER, raw)
    if texture:
        GL.glFramebufferTexture2D(
            GL.GL_FRAMEBUFFER, GL.GL_DEPTH_ATTACHMENT, GL.GL_TEXTURE_2D, depth.glo, 0
        )
    else:
        GL.glFramebufferRenderbuffer(
            GL.GL_FRAMEBUFFER, GL.GL_DEPTH_ATTACHMENT, GL.GL_RENDERBUFFER, depth.glo
        )
    if color is not None:
        GL.glFramebufferTexture2D(
            GL.GL_FRAMEBUFFER, GL.GL_COLOR_ATTACHMENT0, GL.GL_TEXTURE_2D, color.glo, 0
        )
    GL.glDrawBuffer(GL.GL_NONE if depth_only else GL.GL_COLOR_ATTACHMENT0)
    GL.glReadBuffer(GL.GL_NONE if depth_only else GL.GL_COLOR_ATTACHMENT0)
    assert GL.glCheckFramebufferStatus(GL.GL_FRAMEBUFFER) == GL.GL_FRAMEBUFFER_COMPLETE
    if not current:
        previous.use()
    bound = int(GL.glGetIntegerv(GL.GL_DRAW_FRAMEBUFFER_BINDING))
    detected = ctx.detect_framebuffer(None if current else raw)
    try:
        assert int(GL.glGetIntegerv(GL.GL_DRAW_FRAMEBUFFER_BINDING)) == bound
        assert detected.size == size
        assert detected.viewport == (0, 0, *size)
        detected.use()
        assert tuple(GL.glGetIntegerv(GL.GL_VIEWPORT)) == (0, 0, *size)
    finally:
        previous.use()
        detected.release()
        depth.release()
        if color is not None:
            color.release()
