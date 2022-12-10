import unittest

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_1(self):
        buf = self.ctx.buffer(b'abc')

        with self.assertRaises(Exception):
            buf.read(4)

        with self.assertRaises(Exception):
            buf.read(offset=-1)

        with self.assertRaises(Exception):
            buf.read(offset=1, size=3)

    def test_2(self):
        buf = self.ctx.buffer(b'123456789')

        with self.assertRaises(Exception):
            buf.read_chunks(1, 4, 1, 6)

        with self.assertRaises(Exception):
            buf.read_chunks(1, 4, -1, 6)

        with self.assertRaises(Exception):
            buf.read_chunks(2, -1, 2, 1)

    def test_3(self):
        buf = self.ctx.buffer(b'123456789')

        with self.assertRaises(Exception):
            buf.read_chunks(2, 2, 3, 3)

        with self.assertRaises(Exception):
            buf.read_chunks(2, -1, -3, 3)

        with self.assertRaises(Exception):
            buf.read_chunks(2, -4, -3, 3)

    def test_4(self):
        buf = self.ctx.buffer(b'123456789')

        with self.assertRaises(Exception):
            buf.read_chunks(3, 0, 2, 2)


if __name__ == '__main__':
    unittest.main()
