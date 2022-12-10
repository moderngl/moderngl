import unittest

import moderngl

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_buffer_create_bytes(self):
        self.ctx.buffer(b'Hello World!')

    def test_buffer_reserve(self):
        self.ctx.buffer(reserve=1024)

    def test_buffer_reserve_human_readable(self):
        buf = self.ctx.buffer(reserve='2KB')
        self.assertEqual(buf.size, 2 * 1024)

    def test_buffer_data_and_reserve(self):
        with self.assertRaises(moderngl.Error):
            self.ctx.buffer(b'Hello World!', reserve=1024)

    def test_data_size(self):
        buf = self.ctx.buffer(b'\x00\x00\x00\x00')
        self.assertEqual(buf.size, 4)

    def test_reserve_size(self):
        buf = self.ctx.buffer(reserve=8)
        self.assertEqual(buf.size, 8)

    def test_dynamic(self):
        buf = self.ctx.buffer(b'\x00\x00\x00\x00', dynamic=False)
        self.assertFalse(buf.dynamic)

    def test_non_dynamic(self):
        buf = self.ctx.buffer(b'\x00\x00\x00\x00', dynamic=True)
        self.assertTrue(buf.dynamic)


if __name__ == '__main__':
    unittest.main()
