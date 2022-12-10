import unittest

from common import get_context


class TestBuffer(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_buffer_clear_1(self):
        buf = self.ctx.buffer(data=b'\xAA\x55' * 10)
        buf.clear(chunk=b'AB')
        self.assertEqual(buf.read(), b'AB' * 10)

    def test_buffer_clear_2(self):
        buf = self.ctx.buffer(data=b'\xAA\x55' * 10)
        buf.clear(offset=1, size=18, chunk=b'AB')
        self.assertEqual(buf.read(), b'\xAAABABABABABABABABAB\x55')

    def test_buffer_create(self):
        buf = self.ctx.buffer(data=b'\xAA\x55' * 10)
        self.assertEqual(buf.read(), b'\xAA\x55' * 10)

    def test_buffer_read_write(self):
        buf = self.ctx.buffer(reserve=10)
        buf.write(b'abcd')
        self.assertEqual(buf.read(4), b'abcd')
        buf.write(b'abcd', offset=3)
        self.assertEqual(buf.read(4, offset=1), b'bcab')
        buf.write(b'abcd', offset=6)
        self.assertEqual(buf.read(), b'abcabcabcd')
        self.assertEqual(buf.read(offset=3), b'abcabcd')

    def test_buffer_read_into_1(self):
        data = b'Hello World!'
        buf = self.ctx.buffer(data)
        res = bytearray(buf.size)
        buf.read_into(res)
        self.assertEqual(bytes(res), data)

    def test_buffer_read_into_2(self):
        data = b'Hello World!'
        buf = self.ctx.buffer(data)
        res = bytearray(10)
        buf.read_into(res, offset=6, size=5, write_offset=0)
        buf.read_into(res, offset=0, size=5, write_offset=5)
        self.assertEqual(bytes(res), b'WorldHello')

    def test_buffer_orphan(self):
        buf = self.ctx.buffer(reserve=1024)
        buf.orphan()

    def test_buffer_orphan_resize(self):
        buf = self.ctx.buffer(reserve=10)
        self.assertEqual(buf.size, 10)
        self.assertEqual(len(buf.read()), 10)

        buf.orphan(20)
        self.assertEqual(buf.size, 20)
        self.assertEqual(len(buf.read()), 20)

        buf.orphan(100)
        self.assertEqual(buf.size, 100)
        self.assertEqual(len(buf.read()), 100)

        buf.orphan(-100)
        self.assertEqual(buf.size, 100)
        self.assertEqual(len(buf.read()), 100)


if __name__ == '__main__':
    unittest.main()
