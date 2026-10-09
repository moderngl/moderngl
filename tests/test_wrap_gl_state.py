"""
Verifies that setting `wrap` actually changes the OpenGL state on the intended
texture, not whatever happens to be bound at the time of the call.

These tests catch the failure mode where the wrap setter calls
`glTexParameteri(target, ...)` without first binding `self->texture_obj` to
that target — the parameter would silently land on whichever texture was
last bound (e.g. a more recently created one).
"""
import OpenGL
OpenGL.ERROR_CHECKING = False  # Don't want PyOpenGL to raise on its own
from OpenGL import GL


def _query_wrap(target, pname):
    """Reads a single GL_TEXTURE_WRAP_* parameter from the texture currently
    bound to `target` on the active texture unit."""
    out = (GL.GLint * 1)()
    GL.glGetTexParameteriv(target, pname, out)
    return out[0]


def test_texture_wrap_applies_to_self_not_bound_texture(ctx):
    tex_a = ctx.texture((4, 4), 4)
    # Creating tex_b leaves it bound to the default texture unit on
    # GL_TEXTURE_2D, displacing tex_a's binding.
    tex_b = ctx.texture((4, 4), 4)

    tex_a.wrap = {"x": "clamp_to_edge", "y": "mirrored_repeat"}

    tex_a.use(0)
    assert GL.GL_CLAMP_TO_EDGE == _query_wrap(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_S)
    assert GL.GL_MIRRORED_REPEAT == _query_wrap(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_T)

    tex_b.use(0)
    assert GL.GL_REPEAT == _query_wrap(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_S)
    assert GL.GL_REPEAT == _query_wrap(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_T)


def test_texture3d_wrap_applies_to_self_not_bound_texture(ctx):
    tex_a = ctx.texture3d((4, 4, 4), 4)
    tex_b = ctx.texture3d((4, 4, 4), 4)

    tex_a.wrap = {
        "x": "clamp_to_edge",
        "y": "mirrored_repeat",
        "z": "clamp_to_border",
    }

    tex_a.use(0)
    assert GL.GL_CLAMP_TO_EDGE == _query_wrap(GL.GL_TEXTURE_3D, GL.GL_TEXTURE_WRAP_S)
    assert GL.GL_MIRRORED_REPEAT == _query_wrap(GL.GL_TEXTURE_3D, GL.GL_TEXTURE_WRAP_T)
    assert GL.GL_CLAMP_TO_BORDER == _query_wrap(GL.GL_TEXTURE_3D, GL.GL_TEXTURE_WRAP_R)

    tex_b.use(0)
    assert GL.GL_REPEAT == _query_wrap(GL.GL_TEXTURE_3D, GL.GL_TEXTURE_WRAP_S)
    assert GL.GL_REPEAT == _query_wrap(GL.GL_TEXTURE_3D, GL.GL_TEXTURE_WRAP_T)
    assert GL.GL_REPEAT == _query_wrap(GL.GL_TEXTURE_3D, GL.GL_TEXTURE_WRAP_R)


def test_texture_array_wrap_applies_to_self_not_bound_texture(ctx):
    tex_a = ctx.texture_array((4, 4, 2), 4)
    tex_b = ctx.texture_array((4, 4, 2), 4)

    tex_a.wrap = {"x": "clamp_to_edge", "y": "mirrored_repeat"}

    tex_a.use(0)
    assert GL.GL_CLAMP_TO_EDGE == _query_wrap(GL.GL_TEXTURE_2D_ARRAY, GL.GL_TEXTURE_WRAP_S)
    assert GL.GL_MIRRORED_REPEAT == _query_wrap(GL.GL_TEXTURE_2D_ARRAY, GL.GL_TEXTURE_WRAP_T)

    tex_b.use(0)
    assert GL.GL_REPEAT == _query_wrap(GL.GL_TEXTURE_2D_ARRAY, GL.GL_TEXTURE_WRAP_S)
    assert GL.GL_REPEAT == _query_wrap(GL.GL_TEXTURE_2D_ARRAY, GL.GL_TEXTURE_WRAP_T)