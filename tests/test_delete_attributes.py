import subprocess
import sys
import textwrap

# Deleting an attribute of a low level object calls its C setter with
# value == NULL. This used to dereference the NULL pointer for some of the
# attributes (e.g. `del vao.mglo.index_buffer`) and raise a wrong exception
# for the others. A crash would take the test runner down, so the check is
# done in a subprocess.

SCRIPT = textwrap.dedent('''
    import types
    import moderngl
    from glcontext import egl

    ctx = moderngl.create_context(
        standalone=True,
        context=egl.create_context(glversion=330, mode="standalone"),
    )

    prog = ctx.program(
        vertex_shader="""
            #version 330
            in vec2 in_vert;
            void main() {
                gl_Position = vec4(in_vert, 0.0, 1.0);
            }
        """,
        fragment_shader="""
            #version 330
            out vec4 color;
            void main() {
                color = vec4(1.0);
            }
        """,
    )
    buf = ctx.buffer(reserve=64)
    objects = {
        "context": ctx,
        "buffer": buf,
        "program": prog,
        "vertex_array": ctx.vertex_array(prog, [(buf, "2f", "in_vert")]),
        "framebuffer": ctx.simple_framebuffer((4, 4)),
        "renderbuffer": ctx.renderbuffer((4, 4)),
        "sampler": ctx.sampler(),
        "texture": ctx.texture((4, 4), 4),
        "texture_3d": ctx.texture3d((4, 4, 4), 4),
        "texture_array": ctx.texture_array((4, 4, 4), 4),
        "texture_cube": ctx.texture_cube((4, 4), 4),
        "query": ctx.query(time=True),
    }

    checked = 0
    for label, obj in objects.items():
        mglo = obj.mglo
        for name, descr in vars(type(mglo)).items():
            if not isinstance(descr, types.GetSetDescriptorType):
                continue
            print("%s.%s" % (label, name), flush=True)
            try:
                delattr(mglo, name)
            except AttributeError:
                checked += 1
            except BaseException as exc:
                print("WRONG %s.%s: %r" % (label, name, exc), flush=True)
            else:
                print("WRONG %s.%s: deleted" % (label, name), flush=True)

    print("checked %d" % checked, flush=True)
''')


def test_delete_attributes():
    result = subprocess.run(
        [sys.executable, "-c", SCRIPT],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        universal_newlines=True,
    )
    assert result.returncode == 0, "crashed (code %d), last output:\n%s" % (
        result.returncode,
        result.stdout[-500:],
    )
    assert "WRONG" not in result.stdout, result.stdout
    # Sanity check that the setters were actually found
    assert "vertex_array.index_buffer" in result.stdout
    assert "context.fbo" in result.stdout
