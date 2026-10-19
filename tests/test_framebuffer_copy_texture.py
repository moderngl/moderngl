import numpy as np
from OpenGL import GL
import pytest


@pytest.mark.parametrize('dtype', ['f1', 'f2', 'f4', 'i1', 'i2', 'i4', 'u1', 'u2', 'u4'])
@pytest.mark.parametrize('components', [1, 4])
@pytest.mark.parametrize('target_size', [(4, 3), (7, 5), (3, 2)])
def test_copy_preserves_texture_storage(ctx, dtype, components, target_size):
    previous = ctx.fbo
    npdtype = 'u1' if dtype == 'f1' else dtype
    values = np.arange(12*components).reshape(3, 4, components)
    if dtype in ('f2', 'f4'):
        values = values / 8. - 1.25
    values = np.asarray(values, dtype=npdtype)
    source_texture = ctx.texture((4, 3), components, values.tobytes(), dtype=dtype, alignment=1)
    source = ctx.framebuffer([source_texture])
    initial = np.full((*target_size[::-1], components), 17, dtype=npdtype)
    target = ctx.texture(target_size, components, initial.tobytes(), dtype=dtype, alignment=1)
    GL.glBindTexture(GL.GL_TEXTURE_2D, target.glo)
    internal_format = GL.glGetTexLevelParameteriv(GL.GL_TEXTURE_2D, 0, GL.GL_TEXTURE_INTERNAL_FORMAT)

    ctx.copy_framebuffer(target, source)

    assert ctx.fbo is previous
    assert target.size == target_size
    GL.glBindTexture(GL.GL_TEXTURE_2D, target.glo)
    assert GL.glGetTexLevelParameteriv(GL.GL_TEXTURE_2D, 0, GL.GL_TEXTURE_WIDTH) == target_size[0]
    assert GL.glGetTexLevelParameteriv(GL.GL_TEXTURE_2D, 0, GL.GL_TEXTURE_HEIGHT) == target_size[1]
    assert GL.glGetTexLevelParameteriv(GL.GL_TEXTURE_2D, 0, GL.GL_TEXTURE_INTERNAL_FORMAT) == internal_format
    actual = np.frombuffer(target.read(alignment=1), dtype=npdtype).reshape(initial.shape)
    width, height = min(4, target_size[0]), min(3, target_size[1])
    initial[:height, :width] = values[:height, :width]
    np.testing.assert_array_equal(actual, initial)


@pytest.mark.parametrize('attachment', [1, -1])
@pytest.mark.parametrize('read_method', ['read', 'read_into', 'buffer'])
def test_copy_texture_after_reading_other_attachment(ctx, attachment, read_method):
    red = bytes([255, 0, 0, 255]) * 16
    green = bytes([0, 255, 0, 255]) * 16
    source = ctx.framebuffer(
        [ctx.texture((4, 4), 4, red), ctx.texture((4, 4), 4, green)],
        depth_attachment=ctx.depth_renderbuffer((4, 4)),
    )
    target = ctx.texture((4, 4), 4, bytes(64))
    previous = ctx.fbo

    if read_method == 'read':
        source.read(attachment=attachment, components=4)
    elif read_method == 'read_into':
        source.read_into(bytearray(64), attachment=attachment, components=4)
    else:
        buffer = ctx.buffer(reserve=64)
        source.read_into(buffer, attachment=attachment, components=4)

    ctx.copy_framebuffer(target, source)

    assert ctx.fbo is previous
    assert target.read() == red
