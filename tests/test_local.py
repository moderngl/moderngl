import os
import moderngl

def test_local():
    path1 = os.path.normpath(os.path.dirname(os.path.dirname(moderngl.__file__)))
    path2 = os.path.normpath(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    assert path1.lower() == path2.lower(), 'Using installed moderngl'
