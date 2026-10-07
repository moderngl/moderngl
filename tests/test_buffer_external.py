"""
Test buffers wrapping an existing OpenGL buffer object (Context.external_buffer)
"""


def test_external_buffer_properties(ctx):
    buf = ctx.buffer(b'abcdefgh')
    ext = ctx.external_buffer(buf.glo, buf.size)
    assert ext.glo == buf.glo
    assert ext.size == buf.size
    assert ext.mglo != buf.mglo
    assert ext.ctx == ctx
    assert ext.read() == b'abcdefgh'


def test_external_buffer_release_keeps_gl_object(ctx):
    """Releasing an external buffer must not delete the buffer object it wraps"""
    buf = ctx.buffer(b'abcdefgh')
    ext = ctx.external_buffer(buf.glo, buf.size)
    ext.release()

    # The wrapped buffer still owns a live GL buffer object
    assert buf.read() == b'abcdefgh'
    buf.write(b'12345678')
    assert buf.read() == b'12345678'
