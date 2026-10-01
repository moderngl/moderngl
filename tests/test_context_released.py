"""
Using objects after the context they belong to was released.

Context.release() destroys the OpenGL context and the mgl.Context drops the
default and the bound framebuffer it holds. Objects created from the context
keep the mgl.Context itself alive, so they can still be called. Everything
that needs the framebuffers must raise moderngl.Error instead of
dereferencing the missing pointers.

The calls go through the Python wrappers and through the mgl objects
(.mglo) of the context and its children, which is what is left once the
wrapper of the context is gone.
"""
import gc

import pytest

import moderngl

VERTEX_SHADER = """
    #version 330
    void main() {
        gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
    }
"""


class Setup:
    """Objects of a released context"""

    def __init__(self, ctx):
        self.cm = ctx.mglo  # What is left of the context
        self.rbo = ctx.renderbuffer((4, 4))
        self.fbo = ctx.framebuffer(self.rbo)
        self.fbo2 = ctx.framebuffer(ctx.renderbuffer((4, 4)))
        self.default = ctx.detect_framebuffer()  # The default framebuffer
        self.scope = ctx.scope(framebuffer=self.fbo)
        self.prog = ctx.program(vertex_shader=VERTEX_SHADER)
        self.vao = ctx.vertex_array(self.prog, [])
        # Leave a framebuffer bound that is not the default one
        self.fbo.use()
        ctx.release()


@pytest.fixture
def released(ctx_new):
    return Setup(ctx_new)


def _set(obj, name, value):
    setattr(obj, name, value)


# Framebuffer wrappers and the mgl framebuffers behind them
FRAMEBUFFER_CASES = {
    "use": lambda f: f.use(),
    "clear": lambda f: f.clear(),
    "read": lambda f: f.read(),
    "read_into": lambda f: f.read_into(bytearray(4 * 4 * 4)),
    "viewport": lambda f: _set(f, "viewport", (0, 0, 2, 2)),
    "scissor": lambda f: _set(f, "scissor", (0, 0, 2, 2)),
    "scissor_none": lambda f: _set(f, "scissor", None),
    "color_mask": lambda f: _set(f, "color_mask", (True, False, True, False)),
    "depth_mask": lambda f: _set(f, "depth_mask", False),
    "bits": lambda f: f.bits,
    "mglo_use": lambda f: f.mglo.use(),
    "mglo_clear": lambda f: f.mglo.clear(0.0, 0.0, 0.0, 0.0, 1.0, None),
    "mglo_viewport": lambda f: _set(f.mglo, "viewport", (0, 0, 2, 2)),
    "mglo_scissor": lambda f: _set(f.mglo, "scissor", (0, 0, 2, 2)),
    "mglo_color_mask": lambda f: _set(f.mglo, "color_mask", (True, True, True, True)),
    "mglo_depth_mask": lambda f: _set(f.mglo, "depth_mask", True),
    "mglo_bits": lambda f: f.mglo.bits,
}


@pytest.mark.parametrize("name", sorted(FRAMEBUFFER_CASES))
@pytest.mark.parametrize("which", ["fbo", "default"])
def test_framebuffer(released, which, name):
    framebuffer = getattr(released, which)
    with pytest.raises(moderngl.Error, match="the context was released"):
        FRAMEBUFFER_CASES[name](framebuffer)


# The mgl.Context, as left behind by the wrapper
CONTEXT_CASES = {
    "fbo": lambda o: o.cm.fbo,
    "set_fbo": lambda o: _set(o.cm, "fbo", o.fbo.mglo),
    "framebuffer": lambda o: o.cm.framebuffer((o.rbo.mglo,), None),
    "empty_framebuffer": lambda o: o.cm.empty_framebuffer((4, 4), 0, 0),
    "copy_framebuffer": lambda o: o.cm.copy_framebuffer(o.fbo2.mglo, o.fbo.mglo),
    "detect_framebuffer": lambda o: o.cm.detect_framebuffer(None),
    "detect_framebuffer_glo": lambda o: o.cm.detect_framebuffer(o.fbo.glo),
    "scope": lambda o: o.cm.scope(o.fbo.mglo, None, (), (), (), ()),
}


@pytest.mark.parametrize("name", sorted(CONTEXT_CASES))
def test_context(released, name):
    with pytest.raises(moderngl.Error, match="the context was released"):
        CONTEXT_CASES[name](released)


SCOPE_CASES = {
    "enter": lambda s: s.__enter__(),
    "mglo_begin": lambda s: s.mglo.begin(),
    "mglo_end": lambda s: s.mglo.end(),
}


@pytest.mark.parametrize("name", sorted(SCOPE_CASES))
def test_scope(released, name):
    with pytest.raises(moderngl.Error, match="the context was released"):
        SCOPE_CASES[name](released.scope)


def test_scope_with(released):
    with pytest.raises(moderngl.Error, match="the context was released"):
        with released.scope:
            pytest.fail("the scope must not be entered")


def test_failed_calls_change_nothing(released):
    """A rejected setter does not touch the object"""
    viewport = released.fbo.viewport
    with pytest.raises(moderngl.Error):
        released.fbo.viewport = (0, 0, 1, 1)
    assert released.fbo.viewport == viewport


def test_release_and_dealloc_still_work(released):
    """Releasing the children of a released context is fine, and so is dropping them"""
    for obj in [
        released.scope,
        released.vao,
        released.prog,
        released.fbo,
        released.fbo2,
        released.default,
        released.rbo,
    ]:
        obj.release()
        obj.release()

    released.__dict__.clear()
    gc.collect()


def test_draw_does_not_crash(released):
    """
    Drawing does not use the framebuffers. It is a call into the destroyed
    OpenGL context (which does nothing with EGL), so all that is checked is that
    it does not crash. A moderngl.Error is fine as well.
    """
    try:
        released.vao.render(vertices=3)
    except moderngl.Error:
        pass
    try:
        released.vao.mglo.render(0, 3, 0, 1)
    except moderngl.Error:
        pass
