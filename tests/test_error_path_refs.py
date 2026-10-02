"""
Calls that fail must not leak references.

Every case triggers an error (a moderngl.Error) in a loop and checks that the
reference counts of the objects passed in did not change. The arguments are
exact tuples on purpose: converting a tuple to a tuple returns the very same
object, so a reference that was taken and never given back shows up in the
count of the tuple passed in. The objects handed to the call as the data are
kept in a list, the counts are read through that list so they do not depend on
how the interpreter counts local variables.
"""
import sys

import pytest

import moderngl

LOOPS = 100


class BadBool:
    def __bool__(self):
        raise ValueError("no truth value")


def counts(box):
    return [sys.getrefcount(box[i]) for i in range(len(box))]


def fails(call, box, exc=moderngl.Error, loops=LOOPS):
    """Calls call() expecting `exc`, and returns the change of the reference counts of the box"""
    # The first call can create things the others reuse (interned strings, caches)
    with pytest.raises(exc):
        call()
    before = counts(box)
    for _ in range(loops):
        with pytest.raises(exc):
            call()
    after = counts(box)
    return [a - b for a, b in zip(after, before)]


# Cases are functions of the context that return (call, box): the call that fails
# and the objects whose reference counts are watched
CASES = {}


def case(fn):
    CASES[fn.__name__.replace("case_", "")] = fn
    return fn


def _fbo(ctx, attachments=1):
    rbos = [ctx.renderbuffer((4, 4)) for _ in range(attachments)]
    return ctx.framebuffer(rbos), rbos


@case
def case_viewport_size(ctx):
    fbo, _ = _fbo(ctx)
    bad = (1, 2, 3)
    return lambda: setattr(fbo.mglo, "viewport", bad), [bad, fbo.mglo]


@case
def case_viewport_values(ctx):
    fbo, _ = _fbo(ctx)
    bad = ("a", "b")
    return lambda: setattr(fbo.mglo, "viewport", bad), [bad, fbo.mglo]


@case
def case_viewport_values4(ctx):
    fbo, _ = _fbo(ctx)
    bad = (1, 2, 3, "d")
    return lambda: setattr(fbo.mglo, "viewport", bad), [bad, fbo.mglo]


@case
def case_scissor(ctx):
    fbo, _ = _fbo(ctx)
    bad = (1, 2, 3)
    return lambda: setattr(fbo.mglo, "scissor", bad), [bad]


@case
def case_scissor_values(ctx):
    fbo, _ = _fbo(ctx)
    bad = ("a", "b", "c", "d")
    return lambda: setattr(fbo.mglo, "scissor", bad), [bad]


@case
def case_clear_viewport(ctx):
    fbo, _ = _fbo(ctx)
    bad = (1, 2, 3)
    return lambda: fbo.clear(viewport=bad), [bad]


@case
def case_read_viewport(ctx):
    fbo, _ = _fbo(ctx)
    bad = (1, 2, 3)
    data = bytearray(64)
    return lambda: fbo.read_into(data, viewport=bad), [bad, data]


@case
def case_texture_write_viewport(ctx):
    tex = ctx.texture((4, 4), 4)
    bad = (1, 2, 3)
    return lambda: tex.write(bytes(16), viewport=bad), [bad]


@case
def case_texture_write_viewport_values(ctx):
    tex = ctx.texture((4, 4), 4)
    bad = ("x", "y")
    return lambda: tex.write(bytes(16), viewport=bad), [bad]


@case
def case_texture3d_write_viewport(ctx):
    tex = ctx.texture3d((4, 4, 4), 4)
    bad = (1, 2)
    return lambda: tex.write(bytes(16), viewport=bad), [bad]


@case
def case_texture3d_write_viewport_values(ctx):
    tex = ctx.texture3d((4, 4, 4), 4)
    bad = ("x", "y", "z")
    return lambda: tex.write(bytes(16), viewport=bad), [bad]


@case
def case_texture_array_write_viewport(ctx):
    tex = ctx.texture_array((4, 4, 4), 4)
    bad = (1, 2)
    return lambda: tex.write(bytes(16), viewport=bad), [bad]


@case
def case_texture_cube_write_viewport(ctx):
    tex = ctx.texture_cube((4, 4), 4)
    bad = (1, 2, 3)
    return lambda: tex.write(0, bytes(16), viewport=bad), [bad]


@case
def case_color_mask_size(ctx):
    fbo, _ = _fbo(ctx)
    bad = (True, False, True)
    return lambda: setattr(fbo, "color_mask", bad), [bad]


@case
def case_color_mask_bool(ctx):
    fbo, _ = _fbo(ctx)
    bad = (True, True, True, BadBool())
    return lambda: setattr(fbo, "color_mask", bad), [bad]


@case
def case_color_masks_size(ctx):
    fbo, _ = _fbo(ctx, 2)
    good = (True, True, True, True)
    bad = (True, True, True)
    masks = (good, bad)
    return lambda: setattr(fbo, "color_mask", masks), [masks, good, bad]


@case
def case_color_masks_bool(ctx):
    fbo, _ = _fbo(ctx, 2)
    good = (True, True, True, True)
    bad = (True, True, True, BadBool())
    masks = (good, bad)
    return lambda: setattr(fbo, "color_mask", masks), [masks, good, bad]


@case
def case_texture_filter(ctx):
    tex = ctx.texture((4, 4), 4)
    bad = (1, 2, 3)
    return lambda: setattr(tex, "filter", bad), [bad]


@case
def case_texture_filter_values(ctx):
    tex = ctx.texture((4, 4), 4)
    bad = ("a", "b")
    return lambda: setattr(tex, "filter", bad), [bad]


@case
def case_texture3d_filter(ctx):
    tex = ctx.texture3d((4, 4, 4), 4)
    bad = (1, 2, 3)
    return lambda: setattr(tex, "filter", bad), [bad]


@case
def case_texture_array_filter(ctx):
    tex = ctx.texture_array((4, 4, 4), 4)
    bad = ("a", "b")
    return lambda: setattr(tex, "filter", bad), [bad]


@case
def case_texture_cube_filter(ctx):
    tex = ctx.texture_cube((4, 4), 4)
    bad = (1,)
    return lambda: setattr(tex, "filter", bad), [bad]


@case
def case_sampler_filter(ctx):
    sampler = ctx.sampler()
    bad = (1, 2, 3)
    return lambda: setattr(sampler, "filter", bad), [bad]


@case
def case_sampler_filter_values(ctx):
    sampler = ctx.sampler()
    bad = ("a", "b")
    return lambda: setattr(sampler, "filter", bad), [bad]


@case
def case_sampler_border_color(ctx):
    sampler = ctx.sampler()
    bad = (1.0, 2.0, 3.0)
    return lambda: setattr(sampler, "border_color", bad), [bad]


@case
def case_sampler_border_color_values(ctx):
    sampler = ctx.sampler()
    bad = (1.0, 2.0, 3.0, "a")
    return lambda: setattr(sampler, "border_color", bad), [bad]


@case
def case_blend_func(ctx):
    bad = (1, 2, 3)
    return lambda: setattr(ctx, "blend_func", bad), [bad]


@case
def case_blend_func_values(ctx):
    bad = ("a", "b")
    return lambda: setattr(ctx, "blend_func", bad), [bad]


@case
def case_blend_func_values4(ctx):
    bad = (1, 2, 3, "d")
    return lambda: setattr(ctx, "blend_func", bad), [bad]


@case
def case_blend_equation(ctx):
    bad = (1, 2, 3)
    return lambda: setattr(ctx, "blend_equation", bad), [bad]


@case
def case_blend_equation_values(ctx):
    bad = ("a", "b")
    return lambda: setattr(ctx, "blend_equation", bad), [bad]


@case
def case_blend_equation_values1(ctx):
    bad = ("a",)
    return lambda: setattr(ctx, "blend_equation", bad), [bad]


def _scope_call(ctx, textures=(), uniform_buffers=(), storage_buffers=(), samplers=()):
    fbo, _ = _fbo(ctx)
    return lambda: ctx.mglo.scope(
        fbo.mglo, None, textures, uniform_buffers, storage_buffers, samplers
    )


@case
def case_scope_texture_size(ctx):
    tex = ctx.texture((4, 4), 4)
    entry = (tex.mglo, 0, 0)
    textures = (entry,)
    return _scope_call(ctx, textures=textures), [entry, textures, tex.mglo]


@case
def case_scope_texture_type(ctx):
    entry = (object(), 0)
    textures = (entry,)
    return _scope_call(ctx, textures=textures), [entry, textures]


@case
def case_scope_texture_location(ctx):
    tex = ctx.texture((4, 4), 4)
    entry = (tex.mglo, "a")
    textures = (entry,)
    return _scope_call(ctx, textures=textures), [entry, textures, tex.mglo]


@case
def case_scope_uniform_buffer_size(ctx):
    buf = ctx.buffer(reserve=16)
    entry = (buf.mglo,)
    buffers = (entry,)
    return _scope_call(ctx, uniform_buffers=buffers), [entry, buffers, buf.mglo]


@case
def case_scope_uniform_buffer_type(ctx):
    entry = (object(), 0)
    buffers = (entry,)
    return _scope_call(ctx, uniform_buffers=buffers), [entry, buffers]


@case
def case_scope_storage_buffer_location(ctx):
    buf = ctx.buffer(reserve=16)
    entry = (buf.mglo, "a")
    buffers = (entry,)
    return _scope_call(ctx, storage_buffers=buffers), [entry, buffers, buf.mglo]


@case
def case_scope_sampler_size(ctx):
    sampler = ctx.sampler()
    entry = (sampler, 0, 0)
    samplers = (entry,)
    return _scope_call(ctx, samplers=samplers), [entry, samplers, sampler]


@case
def case_scope_sampler_location(ctx):
    sampler = ctx.sampler()
    entry = (sampler, "a")
    samplers = (entry,)
    return _scope_call(ctx, samplers=samplers), [entry, samplers, sampler]


def _valid_scope_args(ctx):
    """Valid arguments for the four binding tuples of scope(), each one an exact tuple"""
    tex = ctx.texture((4, 4), 4)
    buf = ctx.buffer(reserve=16)
    buf2 = ctx.buffer(reserve=16)
    sampler = ctx.sampler()
    keep = [tex, buf, buf2, sampler]
    return keep, ((tex.mglo, 0),), ((buf.mglo, 0),), ((buf2.mglo, 1),), ((sampler, 0),)


@case
def case_scope_uniform_buffers_not_a_sequence(ctx):
    fbo, _ = _fbo(ctx)
    keep, textures, _, _, _ = _valid_scope_args(ctx)
    return lambda: ctx.mglo.scope(fbo.mglo, None, textures, 5, (), ()), [textures]


@case
def case_scope_storage_buffers_not_a_sequence(ctx):
    fbo, _ = _fbo(ctx)
    keep, textures, uniform_buffers, _, _ = _valid_scope_args(ctx)
    call = lambda: ctx.mglo.scope(fbo.mglo, None, textures, uniform_buffers, 5, ())
    return call, [textures, uniform_buffers]


@case
def case_scope_samplers_not_a_sequence(ctx):
    fbo, _ = _fbo(ctx)
    keep, textures, uniform_buffers, storage_buffers, _ = _valid_scope_args(ctx)
    call = lambda: ctx.mglo.scope(fbo.mglo, None, textures, uniform_buffers, storage_buffers, 5)
    return call, [textures, uniform_buffers, storage_buffers]


@case
def case_scope_enable_flags(ctx):
    fbo, _ = _fbo(ctx)
    keep, textures, uniform_buffers, storage_buffers, samplers = _valid_scope_args(ctx)
    call = lambda: ctx.mglo.scope(fbo.mglo, "x", textures, uniform_buffers, storage_buffers, samplers)
    return call, [textures, uniform_buffers, storage_buffers, samplers]


@pytest.mark.parametrize("name", sorted(CASES))
def test_failing_calls(ctx_new, name):
    call, box = CASES[name](ctx_new)
    assert fails(call, box) == [0] * len(box)


# The buffers a failing call was given must be released: an object with an
# export cannot be resized (bytearray) and keeps the exporter locked

def _mapped_buffer(ctx):
    """A buffer that is mapped, which makes the next attempt to map it fail"""
    buf = ctx.buffer(b"\x00" * 16)
    view = memoryview(buf.mglo)
    return buf, view


def test_buffer_read_chunks_into_cannot_map(ctx_new):
    ctx = ctx_new
    buf, view = _mapped_buffer(ctx)
    data = bytearray(16)

    for _ in range(LOOPS):
        with pytest.raises(moderngl.Error, match="cannot map the buffer"):
            buf.mglo.read_chunks_into(data, 4, 0, 4, 4, 0)
        data.append(0)  # BufferError if the export was not released
        del data[-1]

    view.release()
    ctx.clear_errors()
    buf.mglo.read_chunks_into(data, 4, 0, 4, 4, 0)


def test_buffer_clear_cannot_map(ctx_new):
    ctx = ctx_new
    buf, view = _mapped_buffer(ctx)
    chunk = bytearray(4)

    for _ in range(LOOPS):
        with pytest.raises(moderngl.Error, match="cannot map the buffer"):
            buf.clear(chunk=chunk)
        chunk.append(0)
        del chunk[-1]

    view.release()
    ctx.clear_errors()
    buf.clear(chunk=chunk)
    assert buf.read() == b"\x00" * 16
