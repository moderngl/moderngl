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
import OpenGL

OpenGL.ERROR_CHECKING = False  # Don't want PyOpenGL to raise any exceptions
from OpenGL import GL

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


# Constructors that fail after the OpenGL object was created: the object must be
# deleted again and the framebuffer that was bound before must be bound again

def framebuffer_names():
    return {name for name in range(1, 512) if GL.glIsFramebuffer(name)}


def bound_framebuffer():
    return GL.glGetIntegerv(GL.GL_FRAMEBUFFER_BINDING)


FRAMEBUFFER_CASES = {}


def fb_case(fn):
    FRAMEBUFFER_CASES[fn.__name__.replace("fb_", "")] = fn
    return fn


@fb_case
def fb_invalid_color_attachment(ctx):
    attachments = (object(),)
    return lambda: ctx.mglo.framebuffer(attachments, None), [attachments]


@fb_case
def fb_color_attachment_sizes(ctx):
    rbo1, rbo2 = ctx.renderbuffer((4, 4)), ctx.renderbuffer((8, 8))
    attachments = (rbo1.mglo, rbo2.mglo)
    return lambda: ctx.mglo.framebuffer(attachments, None), [attachments, rbo1.mglo, rbo2.mglo]


@fb_case
def fb_invalid_depth_attachment(ctx):
    rbo1, rbo2 = ctx.renderbuffer((4, 4)), ctx.renderbuffer((4, 4))
    attachments = (rbo1.mglo,)
    return lambda: ctx.mglo.framebuffer(attachments, rbo2.mglo), [attachments, rbo2.mglo]


@fb_case
def fb_missing_attachments(ctx):
    return lambda: ctx.mglo.framebuffer((), None), []


@fb_case
def fb_incomplete(ctx):
    # A layered attachment next to one that is not
    rbo, array = ctx.renderbuffer((4, 4)), ctx.texture_array((4, 4, 2), 4)
    attachments = (rbo.mglo, array.mglo)
    return lambda: ctx.mglo.framebuffer(attachments, None), [attachments, rbo.mglo, array.mglo]


@fb_case
def fb_empty_incomplete(ctx):
    return lambda: ctx.mglo.empty_framebuffer((0, 0), 0, 0), []


@pytest.mark.parametrize("name", sorted(FRAMEBUFFER_CASES))
def test_failing_framebuffers(ctx_new, name):
    ctx = ctx_new
    call, box = FRAMEBUFFER_CASES[name](ctx)
    other = ctx.simple_framebuffer((4, 4))
    other.use()
    names = framebuffer_names()
    assert bound_framebuffer() == other.glo

    growth = fails(call, box)

    assert framebuffer_names() == names
    assert bound_framebuffer() == other.glo
    assert growth == [0] * len(box)


# Program creation fails at many places after the OpenGL program was created (and
# some of the shaders): everything has to be deleted and given back

VS = """
    #version 330
    in vec2 in_vert;
    out float v;
    void main() {
        v = in_vert.x;
        gl_Position = vec4(in_vert, 0.0, 1.0);
    }
"""

FS = """
    #version 330
    in float v;
    out vec4 color;
    void main() {
        color = vec4(v);
    }
"""


class BadSource:
    """Not a string, not bytes, nothing"""


class RaisingSource:
    def to_shader_source(self):
        raise ValueError("no source")


def program_names():
    programs = {name for name in range(1, 512) if GL.glIsProgram(name)}
    shaders = {name for name in range(1, 512) if GL.glIsShader(name)}
    return programs, shaders


PROGRAM_CASES = {}


def program_case(exc=moderngl.Error):
    def decorator(fn):
        PROGRAM_CASES[fn.__name__.replace("prog_", "")] = (fn, exc)
        return fn

    return decorator


@program_case()
def prog_vertex_compile_error(ctx):
    varyings = ("v",)
    bad = "#version 330\nvoid main() { oops }"
    return lambda: ctx.program(vertex_shader=bad, fragment_shader=FS, varyings=varyings), [varyings]


@program_case()
def prog_fragment_compile_error(ctx):
    # The vertex shader is compiled and attached when the fragment shader fails
    varyings = ("v",)
    bad = "#version 330\nvoid main() { oops }"
    return lambda: ctx.program(vertex_shader=VS, fragment_shader=bad, varyings=varyings), [varyings]


@program_case()
def prog_link_error(ctx):
    varyings = ("v",)
    # The vertex shader outputs a float
    fs = "#version 330\nin vec4 v;\nout vec4 color;\nvoid main() { color = v; }"
    return lambda: ctx.program(vertex_shader=VS, fragment_shader=fs, varyings=varyings), [varyings]


@program_case()
def prog_wrong_source_type(ctx):
    varyings = ("v",)
    source = BadSource()
    return lambda: ctx.program(vertex_shader=VS, fragment_shader=source, varyings=varyings), [varyings, source]


@program_case(ValueError)
def prog_to_shader_source_raises(ctx):
    varyings = ("v",)
    source = RaisingSource()
    return lambda: ctx.program(vertex_shader=VS, fragment_shader=source, varyings=varyings), [varyings, source]


@program_case(KeyError)
def prog_include_missing(ctx):
    varyings = ("v",)
    fs = FS.replace("void main", '#include "missing"\nvoid main')
    return lambda: ctx.program(vertex_shader=VS, fragment_shader=fs, varyings=varyings), [varyings]


@program_case()
def prog_invalid_varyings(ctx):
    varyings = ("v", 5)
    return lambda: ctx.program(vertex_shader=VS, fragment_shader=FS, varyings=varyings), [varyings]


@program_case(TypeError)
def prog_invalid_fragment_outputs(ctx):
    varyings = ("v",)
    return lambda: ctx.program(
        vertex_shader=VS, fragment_shader=FS, varyings=varyings, fragment_outputs={"color": "zero"}
    ), [varyings]


@pytest.mark.parametrize("name", sorted(PROGRAM_CASES))
def test_failing_programs(ctx_new, name):
    ctx = ctx_new
    fn, exc = PROGRAM_CASES[name]
    call, box = fn(ctx)
    box = box + [ctx.mglo]
    names = program_names()

    growth = fails(call, box, exc)

    assert program_names() == names
    assert growth == [0] * len(box)


def test_programs_give_back_the_varyings(ctx_new):
    ctx = ctx_new
    varyings = ("v",)
    box = [varyings, ctx.mglo]
    before = counts(box)

    for _ in range(LOOPS):
        prog = ctx.program(vertex_shader=VS, fragment_shader=FS, varyings=varyings)
        prog.release()
        del prog

    assert counts(box) == before


# Vertex arrays read three attributes of every attribute object they are given

class FakeAttribute:
    """Has what a vertex array reads from an attribute, or not"""

    def __init__(self, **attributes):
        self.__dict__.update(attributes)


def vertex_array_names():
    return {name for name in range(1, 512) if GL.glIsVertexArray(name)}


VERTEX_ARRAY_CASES = {}


def vao_case(exc):
    def decorator(fn):
        VERTEX_ARRAY_CASES[fn.__name__.replace("vao_", "")] = (fn, exc)
        return fn

    return decorator


def _vao_call(ctx, attribute):
    prog = ctx.program(vertex_shader=VS)
    vbo, ibo = ctx.buffer(reserve=64), ctx.buffer(reserve=64)
    content = ((vbo.mglo, "2f", attribute),)
    call = lambda: ctx.mglo.vertex_array(prog.mglo, content, ibo.mglo, 4)
    return call, [content, prog.mglo, vbo.mglo, ibo.mglo, ctx.mglo]


@vao_case(AttributeError)
def vao_no_scalar_type(ctx):
    # Two of the three attributes are there when the third one is missing
    location, rows_length = 1000 + len("a"), 2000 + len("b")
    attribute = FakeAttribute(location=location, rows_length=rows_length)
    call, box = _vao_call(ctx, attribute)
    return call, box + [location, rows_length, attribute]


@vao_case(AttributeError)
def vao_no_attributes(ctx):
    attribute = FakeAttribute()
    call, box = _vao_call(ctx, attribute)
    return call, box + [attribute]


@vao_case(TypeError)
def vao_not_numbers(ctx):
    location, rows_length, scalar_type = 1000 + len("a"), 2000 + len("b"), "x"
    attribute = FakeAttribute(location=location, rows_length=rows_length, scalar_type=scalar_type)
    call, box = _vao_call(ctx, attribute)
    return call, box + [location, rows_length, scalar_type, attribute]


@pytest.mark.parametrize("name", sorted(VERTEX_ARRAY_CASES))
def test_failing_vertex_arrays(ctx_new, name):
    ctx = ctx_new
    fn, exc = VERTEX_ARRAY_CASES[name]
    call, box = fn(ctx)
    names = vertex_array_names()

    growth = fails(call, box, exc)

    assert vertex_array_names() == names
    assert growth == [0] * len(box)


def test_vertex_arrays_give_back_the_attributes(ctx_new):
    ctx = ctx_new
    prog = ctx.program(vertex_shader=VS)
    vbo = ctx.buffer(reserve=64)
    attribute = prog["in_vert"]
    # The attribute objects hold ints that are not cached
    box = [attribute.scalar_type, attribute.location, attribute.rows_length, attribute]
    box += [prog.mglo, vbo.mglo, ctx.mglo]
    before = counts(box)

    for _ in range(LOOPS):
        vao = ctx.vertex_array(prog, [(vbo, "2f", "in_vert")])
        vao.release()
        del vao

    assert counts(box) == before
