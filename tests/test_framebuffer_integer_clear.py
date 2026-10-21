import numpy as np
import pytest


@pytest.mark.parametrize('renderbuffer', [False, True])
@pytest.mark.parametrize('dtype, color', [
    ('u1', (17, 1, 254, 0)),
    ('i1', (-17, 1, 126, -1)),
    ('u2', (17, 1, 65534, 0)),
    ('i2', (-17, 1, 32766, -1)),
    ('u4', (2**24 + 1, 2**32 - 1, 2**31 + 1, 0)),
    ('i4', (-2**31, 2**31 - 1, 2**24 + 1, -2**24 - 1)),
])
def test_integer_clear(ctx, renderbuffer, dtype, color):
    previous = ctx.fbo
    image = (ctx.renderbuffer((4, 3), 4, dtype=dtype) if renderbuffer else
             ctx.texture((4, 3), 4, dtype=dtype))
    target = ctx.framebuffer([image])
    previous.use()

    target.clear(*color)

    assert ctx.fbo is previous
    actual = np.frombuffer(target.read(components=4, dtype=dtype), dtype=dtype).reshape(12, 4)
    np.testing.assert_array_equal(actual, np.tile(np.array(color, dtype=dtype), (12, 1)))


@pytest.mark.parametrize('viewport', [None, (0, 0, 2, 3)])
def test_mixed_clear_masks_and_scissor(ctx, viewport):
    previous = ctx.fbo
    integer_values = np.full((4, 5, 4), 13, dtype='i4')
    float_values = np.full((4, 5, 4), 13, dtype='f4')
    integer = ctx.texture((5, 4), 4, integer_values.tobytes(), dtype='i4')
    floating = ctx.texture((5, 4), 4, float_values.tobytes(), dtype='f4')
    target = ctx.framebuffer([integer, floating])
    target.color_mask = ((True, False, True, False), (False, True, False, True))
    target.scissor = (1, 1, 3, 2)
    previous.use()

    target.clear(17.5, 2.25, 255.75, -1.5, viewport=viewport)

    x, y, w, h = viewport if viewport is not None else target.scissor
    integer_values[y:y+h, x:x+w, 0] = 17
    integer_values[y:y+h, x:x+w, 2] = 255
    float_values[y:y+h, x:x+w, 1] = 2.25
    float_values[y:y+h, x:x+w, 3] = -1.5
    assert ctx.fbo is previous
    np.testing.assert_array_equal(np.frombuffer(integer.read(), dtype='i4').reshape(4, 5, 4), integer_values)
    np.testing.assert_array_equal(np.frombuffer(floating.read(), dtype='f4').reshape(4, 5, 4), float_values)
