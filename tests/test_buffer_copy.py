
def test_1(ctx):
    buf1 = ctx.buffer(b'abc')
    buf2 = ctx.buffer(reserve=3)
    ctx.copy_buffer(buf2, buf1)
    assert buf2.read() == b'abc'

def test_2(ctx):
    buf1 = ctx.buffer(b'abcxyz123')
    buf2 = ctx.buffer(reserve=12)
    ctx.copy_buffer(buf2, buf1, 3, read_offset=3, write_offset=0)
    ctx.copy_buffer(buf2, buf1, 3, read_offset=0, write_offset=3)
    ctx.copy_buffer(buf2, buf1, 3, read_offset=6, write_offset=6)
    ctx.copy_buffer(buf2, buf1, 3, read_offset=3, write_offset=9)
    assert buf2.read() == b'xyzabc123xyz'
