import moderngl
import pytest


def test_1(ctx):
    buf = ctx.buffer(b'abc')

    with pytest.raises(Exception):
        buf.read(4)

    with pytest.raises(Exception):
        buf.read(offset=-1)

    with pytest.raises(Exception):
        buf.read(offset=1, size=3)


def test_2(ctx):
    buf = ctx.buffer(b'123456789')

    with pytest.raises(Exception):
        buf.read_chunks(1, 4, 1, 6)

    with pytest.raises(Exception):
        buf.read_chunks(1, 4, -1, 6)

    with pytest.raises(Exception):
        buf.read_chunks(2, -1, 2, 1)


def test_3(ctx):
    buf = ctx.buffer(b'123456789')

    with pytest.raises(Exception):
        buf.read_chunks(2, 2, 3, 3)

    with pytest.raises(Exception):
        buf.read_chunks(2, -1, -3, 3)

    with pytest.raises(Exception):
        buf.read_chunks(2, -4, -3, 3)


def test_4(ctx):
    buf = ctx.buffer(b'123456789')

    with pytest.raises(Exception):
        buf.read_chunks(3, 0, 2, 2)


def test_read_into_out_of_range(ctx):
    """read_into must validate offset and size (and never copy from a failed map)"""
    buf = ctx.buffer(b'abc')
    data = bytearray(16)

    # size=-1 means "to the end of the buffer", which is negative for an offset past the end.
    # This used to make glMapBufferRange fail and read_into memcpy from NULL.
    with pytest.raises(moderngl.Error):
        buf.read_into(data, offset=8)

    with pytest.raises(moderngl.Error):
        buf.read_into(data, offset=4)

    with pytest.raises(moderngl.Error):
        buf.read_into(data, offset=-1)

    with pytest.raises(moderngl.Error):
        buf.read_into(data, size=4)

    with pytest.raises(moderngl.Error):
        buf.read_into(data, size=2, offset=2)

    assert data == bytearray(16)
    assert ctx.error == "GL_NO_ERROR"

    # An offset at the end with an explicit empty size is still valid
    buf.read_into(data, size=0, offset=3)
    buf.read_into(data, offset=1)
    assert bytes(data[:2]) == b'bc'
