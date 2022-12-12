
def test_1(ctx):
    buf1 = ctx.buffer(data=b'\xAA\x55' * 10)
    buf2 = ctx.buffer(reserve=buf1.size)
    buf2.write(b'Hello World!')

    assert buf1.read(5, offset=1) == b'\x55\xaa\x55\xaa\x55'
    assert buf2.read(5, offset=6) == b'World'

    ctx.copy_buffer(buf2, buf1, read_offset=1, write_offset=6, size=5)
    assert buf2.read(12) == b'Hello \x55\xaa\x55\xaa\x55!'
