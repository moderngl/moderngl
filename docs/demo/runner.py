import ast
import os
import sys
import textwrap
import traceback

import js
from pyodide.ffi import create_proxy


def run(path):
    sys.path.insert(0, os.path.dirname(path))
    source = open(path).read()
    head, marker, rest = source.partition('\nwhile running:\n')
    lines = rest.split('\n')
    end = next((i for i, line in enumerate(lines) if line and not line[0].isspace()), len(lines))
    body = '\n'.join(lines[:end])
    tail = '\n'.join(lines[end:])

    def compiled(text, offset):
        tree = ast.parse(textwrap.dedent(text))
        ast.increment_lineno(tree, offset)
        return compile(tree, path, 'exec')

    head_lines = head.count('\n') + 2
    scope = {'__name__': '__main__', '__file__': path}
    exec(compiled(head, 0), scope)
    loop = compiled(body, head_lines)
    after = compiled(tail, head_lines + end)

    def frame(timestamp):
        try:
            if not scope.get('running'):
                exec(after, scope)
                return
            exec(loop, scope)
        except Exception:
            js.document.getElementById('status').textContent = traceback.format_exc()
            raise
        js.requestAnimationFrame(proxy)

    proxy = create_proxy(frame)
    js.requestAnimationFrame(proxy)
