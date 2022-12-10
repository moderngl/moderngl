import os
import unittest

import moderngl
import pycodestyle


class TestCase(unittest.TestCase):
    def test_style(self):
        config_file = os.path.join(os.path.dirname(__file__), '..', 'tox.ini')
        style = pycodestyle.StyleGuide(config_file=config_file, ignore='E402')
        check = style.check_files([
            os.path.join(os.path.dirname(__file__), '../moderngl/__init__.py'),
            os.path.join(os.path.dirname(__file__), '../moderngl/__main__.py'),
        ])
        self.assertEqual(check.total_errors, 0)


if __name__ == '__main__':
    unittest.main()
