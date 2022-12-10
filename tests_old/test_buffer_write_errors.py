import unittest

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_1(self):
        buf = self.ctx.buffer(b'abc')

        with self.assertRaises(Exception):
            buf.write(b'1234')

        with self.assertRaises(Exception):
            buf.write(b'u', offset=-1)

        with self.assertRaises(Exception):
            buf.write(b'abc', offset=1)

    def test_2(self):
        buf = self.ctx.buffer(b'123456789')

        with self.assertRaises(Exception):
            buf.write_chunks(b'12345', 0, 1, 4)

        with self.assertRaises(Exception):
            buf.write_chunks(b'12345', 0, 1, 6)

        with self.assertRaises(Exception):
            buf.write_chunks(b'yyyyyn', 4, 1, 6)

        with self.assertRaises(Exception):
            buf.write_chunks(b'yyyyyn', 4, -1, 6)

        with self.assertRaises(Exception):
            buf.write_chunks(b'yn', -1, 2, 1)

    def test_3(self):
        buf = self.ctx.buffer(b'123456789')

        with self.assertRaises(Exception):
            buf.write_chunks(b'yyyyyn', 2, 3, 3)

        with self.assertRaises(Exception):
            buf.write_chunks(b'ynyyyy', -1, -3, 3)

        with self.assertRaises(Exception):
            buf.write_chunks(b'yyyyny', -4, -3, 3)

    def test_4(self):
        buf = self.ctx.buffer(b'123456789')

        with self.assertRaises(Exception):
            buf.write_chunks(b'yyynyy', 0, 2, 2)


if __name__ == '__main__':
    unittest.main()
