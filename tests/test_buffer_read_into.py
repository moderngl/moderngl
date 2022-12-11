"""
Test buffer read_into method
"""

def test_1(ctx):
    buf = ctx.buffer(b'abc')
    data = bytearray(3)
    buf.read_into(data)
    assert bytes(data) == b'abc'

def test_2(ctx):
    buf = ctx.buffer(b'abcxyz123')
    data = bytearray(3)
    buf.read_into(data, offset=6)
    assert bytes(data) == b'123'

def test_3(ctx):
    buf = ctx.buffer(b'abcxyz123')
    data = bytearray(12)
    buf.read_into(data, 3, offset=3, write_offset=0)
    buf.read_into(data, 3, offset=0, write_offset=3)
    buf.read_into(data, 3, offset=6, write_offset=6)
    buf.read_into(data, 3, offset=3, write_offset=9)
    assert bytes(data) == b'xyzabc123xyz'
