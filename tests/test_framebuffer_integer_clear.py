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

    target.clear_attachment(0, color=color)

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

    color = (17.5, 2.25, 255.75, -1.5)
    target.clear_attachment(0, color=color, viewport=viewport)
    np.testing.assert_array_equal(np.frombuffer(floating.read(), dtype='f4').reshape(4, 5, 4), float_values)

    target.clear_attachment(1, color=color, viewport=viewport)

    x, y, w, h = viewport if viewport is not None else target.scissor
    integer_values[y:y+h, x:x+w, 0] = 17
    integer_values[y:y+h, x:x+w, 2] = 255
    float_values[y:y+h, x:x+w, 1] = 2.25
    float_values[y:y+h, x:x+w, 3] = -1.5
    assert ctx.fbo is previous
    np.testing.assert_array_equal(np.frombuffer(integer.read(), dtype='i4').reshape(4, 5, 4), integer_values)
    np.testing.assert_array_equal(np.frombuffer(floating.read(), dtype='f4').reshape(4, 5, 4), float_values)

@pytest.mark.parametrize('dtype, color', [('f1', (0, 1, 0, 1)), ('f2', (.25, .5, .75, 1)),
                                        ('f4', (-1.5, 2.25, 0, 1))])
@pytest.mark.parametrize('renderbuffer', [False, True])
def test_clear_attachment_floating(ctx, dtype, color, renderbuffer):
    image = (ctx.renderbuffer((4, 3), 4, dtype=dtype) if renderbuffer else
             ctx.texture((4, 3), 4, dtype=dtype))
    target = ctx.framebuffer([image])
    target.clear_attachment(color=color)
    actual = np.frombuffer(target.read(components=4, dtype='f4'), dtype='f4').reshape(12, 4)
    np.testing.assert_array_equal(actual, np.tile(color, (12, 1)))


def test_clear_attachment_detected_and_depth_untouched(ctx):
    first = ctx.texture((4, 3), 4, np.full((3, 4, 4), 13, dtype='i4').tobytes(), dtype='i4')
    second = ctx.texture((4, 3), 4, np.full((3, 4, 4), 23, dtype='u4').tobytes(), dtype='u4')
    depth = ctx.depth_texture((4, 3))
    target = ctx.framebuffer([first, second], depth)
    target.color_mask = ((False, False, False, False), (False, False, False, False))
    target.clear(depth=.25)
    depth_before = depth.read()
    detected = ctx.detect_framebuffer(target.glo)
    detected.color_mask = ((True, True, True, True), (True, True, True, True))
    detected.clear_attachment(1, color=(2**32 - 1,))
    np.testing.assert_array_equal(np.frombuffer(first.read(), dtype='i4'), 13)
    expected = np.tile([2**32 - 1, 0, 0, 0], (12, 1))
    np.testing.assert_array_equal(np.frombuffer(second.read(), dtype='u4').reshape(12, 4), expected)
    assert depth.read() == depth_before
