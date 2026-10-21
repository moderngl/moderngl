"""Check the bundled stubs through a real type checker."""
import json
from pathlib import Path
import subprocess
import sys
import textwrap

import pytest


@pytest.mark.parametrize("source", [
    """
    import moderngl

    def check(ctx: moderngl.Context) -> None:
        ctx.blend_func = (moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA)
        ctx.blend_func = (moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA,
                          moderngl.ONE, moderngl.ONE)
        ctx.blend_func = (1, 1, 1)  # error
        ctx.blend_func = (1.0, 1.0)  # error
    """,
    """
    from contextlib import ExitStack
    import moderngl

    def check(query: moderngl.Query) -> None:
        with query as entered:
            samples: int = entered.samples
            entered.missing_attribute  # error
        with ExitStack() as stack:
            entered = stack.enter_context(query)
            samples = entered.samples
            entered.missing_attribute  # error
        query.__exit__(None, None, None)
        query.__exit__(ValueError, ValueError(), None)
    """,
    """
    from contextlib import ExitStack
    import moderngl

    def check(scope: moderngl.Scope) -> None:
        with scope as entered:
            entered.release()
            entered.missing_attribute  # error
        with ExitStack() as stack:
            entered = stack.enter_context(scope)
            entered.release()
            entered.missing_attribute  # error
        scope.__exit__(None, None, None)
        scope.__exit__(ValueError, ValueError(), None)
    """,
])
def test_type_stubs(tmp_path, source):
    source = textwrap.dedent(source)
    (tmp_path / "check.py").write_text(source)
    config = {
        "include": ["check.py"],
        "extraPaths": [str(Path(__file__).resolve().parents[1])],
        "typeCheckingMode": "basic",
    }
    (tmp_path / "pyrightconfig.json").write_text(json.dumps(config))
    result = subprocess.run(
        [sys.executable, "-m", "pyright", "--outputjson"],
        cwd=tmp_path, capture_output=True, text=True,
    )
    report = json.loads(result.stdout)
    expected = [i for i, line in enumerate(source.splitlines()) if "# error" in line]
    errors = [d for d in report["generalDiagnostics"] if d["severity"] == "error"]
    assert result.returncode == 1, result.stderr
    assert report["summary"]["filesAnalyzed"] == 1
    assert sorted(d["range"]["start"]["line"] for d in errors) == expected, errors
