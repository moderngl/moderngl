import unittest

import ModernGL

from common import get_context


class TestBuffer(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def tearDown(self):
        self.assertEqual(self.ctx.error, 'GL_NO_ERROR')

    def test_write_chunks_1(self):
        buf = self.ctx.buffer(reserve=12)
        buf.write_chunks(b'AAAAAA', 0, 2, 6)
        buf.write_chunks(b'BBBBBB', 1, 2, 6)
        print(buf.read())

    def test_write_chunks_2(self):
        buf = self.ctx.buffer(reserve=18)
        buf.write_chunks(b'AAAAAAAAAAAA', 0, 3, 6)
        buf.write_chunks(b'BBBBBB', 2, 3, 6)
        print(buf.read())


if __name__ == '__main__':
    unittest.main()
