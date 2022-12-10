import unittest

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_1(self):
        buf = self.ctx.buffer(b'abc')
        data = bytearray(3)
        buf.read_into(data)
        self.assertEqual(bytes(data), b'abc')

    def test_2(self):
        buf = self.ctx.buffer(b'abcxyz123')
        data = bytearray(3)
        buf.read_into(data, offset=6)
        self.assertEqual(bytes(data), b'123')

    def test_3(self):
        buf = self.ctx.buffer(b'abcxyz123')
        data = bytearray(12)
        buf.read_into(data, 3, offset=3, write_offset=0)
        buf.read_into(data, 3, offset=0, write_offset=3)
        buf.read_into(data, 3, offset=6, write_offset=6)
        buf.read_into(data, 3, offset=3, write_offset=9)
        self.assertEqual(bytes(data), b'xyzabc123xyz')


if __name__ == '__main__':
    unittest.main()
