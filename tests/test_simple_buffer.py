import moderngl
import pytest

def test_buffer_create_bytes(ctx):
    ctx.buffer(b'Hello World!')

def test_buffer_reserve(ctx):
    ctx.buffer(reserve=1024)

def test_buffer_reserve_human_readable(ctx):
    buf = ctx.buffer(reserve='2KB')
    assert buf.size == 2 * 1024

def test_buffer_data_and_reserve(ctx):
    with pytest.raises(moderngl.Error):
        ctx.buffer(b'Hello World!', reserve=1024)

def test_data_size(ctx):
    buf = ctx.buffer(b'\x00\x00\x00\x00')
    assert buf.size == 4

def test_reserve_size(ctx):
    buf = ctx.buffer(reserve=8)
    assert buf.size == 8

def test_dynamic(ctx):
    buf = ctx.buffer(b'\x00\x00\x00\x00', dynamic=False)
    assert buf.dynamic is False

def test_non_dynamic(ctx):
    buf = ctx.buffer(b'\x00\x00\x00\x00', dynamic=True)
    assert buf.dynamic is True
