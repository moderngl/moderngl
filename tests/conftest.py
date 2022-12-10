import pytest
import moderngl

VERSION_CODE = 330


@pytest.fixture(scope="session")
def ctx():
    try:
        return moderngl.create_context(
            require=VERSION_CODE,
            standalone=True,
        )
    except Exception:
        return moderngl.create_context(
            require=VERSION_CODE,
            standalone=True,
            backend="egl",
        )
