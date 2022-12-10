
def test_1(ctx):
    buf = ctx.buffer(b'abc')
    assert buf.read() == b'abc'
    buf.write(b'xyz')
    assert buf.read() == b'xyz'
    assert buf.read(offset=1) == b'yz'

def test_2(ctx):
    buf = ctx.buffer(reserve=10)
    assert buf.size == 10
    buf.write(b'12345')
    buf.write(b'abcde', offset=5)
    assert buf.read(5) == b'12345'
    assert buf.read(5, offset=5) == b'abcde'

def test_3(ctx):
    buf = ctx.buffer(reserve='1KB')
    assert buf.size == 1024
    buf.clear(chunk=b'u')
    assert buf.read() == b'u' * 1024
    buf.clear(size=10, offset=10, chunk=b'ab')
    assert buf.read(10, offset=0) == b'uuuuuuuuuu'
    assert buf.read(10, offset=10) == b'ababababab'
    assert buf.read(10, offset=20) == b'uuuuuuuuuu'

def test_4(ctx):
    buf = ctx.buffer(reserve=10)
    buf.write_chunks(b'00000', 0, 2, 5)
    buf.write_chunks(b'11111', 1, 2, 5)
    assert buf.read(10, offset=0) == b'0101010101'

def test_5(ctx):
    buf = ctx.buffer(b'123456789')
    buf.write_chunks(b'AABBCC', 0, 3, 3)
    assert buf.read() == b'AA3BB6CC9'
    assert buf.read_chunks(2, 0, 3, 3) == b'AABBCC'

def test_6(ctx):
    buf = ctx.buffer(b'123456789')
    buf.write_chunks(b'XYZ', -1, -3, 3)
    assert buf.read() == b'12Z45Y78X'
    assert buf.read_chunks(1, -1, -3, 3) == b'XYZ'

def test_7(ctx):
    buf = ctx.buffer(b'123456789')
    assert buf.read_chunks(3, 0, 3, 3) == b'123456789'

def test_8(ctx):
    buf1 = ctx.buffer(b'abc', dynamic=True)
    buf2 = ctx.buffer(b'abc', dynamic=False)
    assert buf1.dynamic is True
    assert buf2.dynamic is False
