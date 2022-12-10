import unittest

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_1(self):
        buf1 = self.ctx.buffer(b'abc')
        buf2 = self.ctx.buffer(reserve=3)
        self.ctx.copy_buffer(buf2, buf1)
        self.assertEqual(buf2.read(), b'abc')

    def test_2(self):
        buf1 = self.ctx.buffer(b'abcxyz123')
        buf2 = self.ctx.buffer(reserve=12)
        self.ctx.copy_buffer(buf2, buf1, 3, read_offset=3, write_offset=0)
        self.ctx.copy_buffer(buf2, buf1, 3, read_offset=0, write_offset=3)
        self.ctx.copy_buffer(buf2, buf1, 3, read_offset=6, write_offset=6)
        self.ctx.copy_buffer(buf2, buf1, 3, read_offset=3, write_offset=9)
        self.assertEqual(buf2.read(), b'xyzabc123xyz')


if __name__ == '__main__':
    unittest.main()
