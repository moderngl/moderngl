import pytest

def test_1(ctx):
    buf = ctx.buffer(b'abc')

    with pytest.raises(Exception):
        buf.write(b'1234')

    with pytest.raises(Exception):
        buf.write(b'u', offset=-1)

    with pytest.raises(Exception):
        buf.write(b'abc', offset=1)

def test_2(ctx):
    buf = ctx.buffer(b'123456789')

    with pytest.raises(Exception):
        buf.write_chunks(b'12345', 0, 1, 4)

    with pytest.raises(Exception):
        buf.write_chunks(b'12345', 0, 1, 6)

    with pytest.raises(Exception):
        buf.write_chunks(b'yyyyyn', 4, 1, 6)

    with pytest.raises(Exception):
        buf.write_chunks(b'yyyyyn', 4, -1, 6)

    with pytest.raises(Exception):
        buf.write_chunks(b'yn', -1, 2, 1)

def test_3(ctx):
    buf = ctx.buffer(b'123456789')

    with pytest.raises(Exception):
        buf.write_chunks(b'yyyyyn', 2, 3, 3)

    with pytest.raises(Exception):
        buf.write_chunks(b'ynyyyy', -1, -3, 3)

    with pytest.raises(Exception):
        buf.write_chunks(b'yyyyny', -4, -3, 3)

def test_4(ctx):
    buf = ctx.buffer(b'123456789')

    with pytest.raises(Exception):
        buf.write_chunks(b'yyynyy', 0, 2, 2)

