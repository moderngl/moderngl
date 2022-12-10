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
