
def test_buffer_clear_1(ctx):
    buf = ctx.buffer(data=b'\xAA\x55' * 10)
    buf.clear(chunk=b'AB')
    assert buf.read() == b'AB' * 10

def test_buffer_clear_2(ctx):
    buf = ctx.buffer(data=b'\xAA\x55' * 10)
    buf.clear(offset=1, size=18, chunk=b'AB')
    assert buf.read() == b'\xAAABABABABABABABABAB\x55'

def test_buffer_create(ctx):
    buf = ctx.buffer(data=b'\xAA\x55' * 10)
    assert buf.read() == b'\xAA\x55' * 10

def test_buffer_read_write(ctx):
    buf = ctx.buffer(reserve=10)
    buf.write(b'abcd')
    assert buf.read(4) == b'abcd'
    buf.write(b'abcd', offset=3)
    assert buf.read(4, offset=1) == b'bcab'
    buf.write(b'abcd', offset=6)
    assert buf.read() == b'abcabcabcd'
    assert buf.read(offset=3) == b'abcabcd'

def test_buffer_read_into_1(ctx):
    data = b'Hello World!'
    buf = ctx.buffer(data)
    res = bytearray(buf.size)
    buf.read_into(res)
    assert bytes(res) == data

def test_buffer_read_into_2(ctx):
    data = b'Hello World!'
    buf = ctx.buffer(data)
    res = bytearray(10)
    buf.read_into(res, offset=6, size=5, write_offset=0)
    buf.read_into(res, offset=0, size=5, write_offset=5)
    assert bytes(res) == b'WorldHello'

def test_buffer_orphan(ctx):
    buf = ctx.buffer(reserve=1024)
    buf.orphan()

def test_buffer_orphan_resize(ctx):
    buf = ctx.buffer(reserve=10)
    assert buf.size == 10
    assert len(buf.read()) == 10

    buf.orphan(20)
    assert buf.size == 20
    assert len(buf.read()) == 20

    buf.orphan(100)
    assert buf.size == 100
    assert len(buf.read()) == 100

    buf.orphan(-100)
    assert buf.size == 100
    assert len(buf.read()) == 100
