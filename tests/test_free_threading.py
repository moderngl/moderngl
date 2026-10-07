"""Free-threading tests.

Every scenario runs in its own interpreter: a crash or a double free in the native
code fails one test instead of killing the pytest process, and the scenarios do not
share contexts with the rest of the test suite. The scenarios are plain functions in
this file, run by ``python test_free_threading.py <scenario>``.

What is tested (and what the tests do not cover):

* Contexts on different threads run in parallel and do not see each other.
* One context shared by several threads is serialized, nothing crashes and the
  reference counts of the objects that are replaced (the bound framebuffer, the index
  buffer of a vertex array) are not corrupted.
* The containers the caller owns (``fragment_outputs``, the outputs of a transform)
  can change while they are used.
* Objects are released from several threads at once.

Only the main thread of a scenario makes the OpenGL context current (creating it does), and the
other threads do not call ``ctx.__enter__``: making one EGL context current on several threads
at once is an error. OpenGL calls of a thread without a current context are ignored by
the implementation, but all the state of moderngl (the objects, their references, the state kept in the
context) is still used by all the threads. That is what is tested here, not the rendering of
shared contexts. Scenarios that check pixels use one context per thread.

The scenarios also run with the GIL (they have to pass there), but are only a regression test
for races without it. ``PYTHON_GIL=0`` forces the GIL off for a module that did not declare support.
"""
import gc
import os
import subprocess
import sys
import sysconfig
import threading
import time

import pytest

SKIP = 77  # exit code of a scenario that cannot run on this machine

FREE_THREADED = bool(sysconfig.get_config_var("Py_GIL_DISABLED"))

VERTEX_SHADER = """
    #version 330
    in vec2 in_vert;
    void main() {
        gl_Position = vec4(in_vert, 0.0, 1.0);
    }
"""

FRAGMENT_SHADER = """
    #version 330
    uniform vec4 color;
    out vec4 fragColor;
    void main() {
        fragColor = color;
    }
"""

TRANSFORM_SHADER = """
    #version 330
    in float in_value;
    out float out_value;
    void main() {
        out_value = in_value * 2.0;
    }
"""

# Scenarios, these run in the subprocess


def skip(reason):
    print("SKIP: %s" % reason)
    sys.exit(SKIP)


def run_threads(count, target, timeout=60):
    """Run target(index) in count threads released together, raise if any of them failed."""
    barrier = threading.Barrier(count)
    errors = []

    def wrapper(index):
        try:
            barrier.wait(30)
            target(index)
        except BaseException as e:
            errors.append(e)

    threads = [threading.Thread(target=wrapper, args=(i,)) for i in range(count)]
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join(timeout)
        if thread.is_alive():
            # A deadlock, the daemon-less thread would keep the interpreter from exiting
            print("FAIL: thread is stuck", flush=True)
            os._exit(2)
    if errors:
        raise errors[0]


def run_rounds(count, rounds, prepare, work, timeout=60):
    """
    Run `rounds` rounds. A round is prepare(round) in this thread, which returns the argument for
    work(index, argument) in each of the count threads, started together. The threads stay the same
    for all rounds, that is a lot faster than starting them every time.

    Without the GIL the threads wait for the start of a round in a loop to make them
    start in the same moment: the races that are tested here are over in a few nanoseconds.
    With the GIL a thread that spins only delays the others, so there they wait on a barrier.
    """
    spin = FREE_THREADED
    start = threading.Barrier(count + 1)
    end = threading.Barrier(count + 1)
    generation = [0]  # the round the threads can start
    finished = [0]  # the number of threads that finished the round
    lock = threading.Lock()
    argument = [None]
    errors = []
    failed = [False]

    def wrapper(index):
        try:
            for i in range(rounds):
                if spin:
                    while generation[0] <= i:
                        if failed[0]:
                            return
                else:
                    start.wait(timeout)
                work(index, argument[0])
                if spin:
                    with lock:
                        finished[0] += 1
                else:
                    end.wait(timeout)
        except BaseException as e:
            errors.append(e)
            failed[0] = True
            start.abort()
            end.abort()

    threads = [threading.Thread(target=wrapper, args=(i,)) for i in range(count)]
    for thread in threads:
        thread.start()
    try:
        for i in range(rounds):
            argument[0] = prepare(i)
            if spin:
                finished[0] = 0
                generation[0] = i + 1
                deadline = time.monotonic() + timeout
                while finished[0] < count:
                    if failed[0] or time.monotonic() > deadline:
                        raise threading.BrokenBarrierError()
            else:
                start.wait(timeout)
                end.wait(timeout)
            argument[0] = None
    except threading.BrokenBarrierError:
        failed[0] = True
    for thread in threads:
        thread.join(timeout)
        if thread.is_alive():
            print("FAIL: thread is stuck", flush=True)
            os._exit(2)
    if errors:
        raise errors[0]
    assert not failed[0], "the threads did not finish"


def import_moderngl():
    """Import moderngl, a free-threaded build has to keep the GIL disabled."""
    import moderngl

    if FREE_THREADED:
        assert not sys._is_gil_enabled(), "importing moderngl enabled the GIL (set PYTHON_GIL=0 to test without)"
    return moderngl


def create_context(moderngl):
    try:
        return moderngl.create_context(standalone=True, backend="egl")
    except Exception as e:
        skip("egl is not available: %s" % e)


def refs(obj):
    """The reference count of obj, the same amount is added by the call every time."""
    return sys.getrefcount(obj)


def scenario_parallel_contexts():
    """Every thread has its own context and renders a color of its own, nothing is mixed up."""
    moderngl = import_moderngl()
    create_context(moderngl).release()
    import struct

    count = 6
    rounds = 40

    def work(index):
        ctx = moderngl.create_context(standalone=True, backend="egl")
        fbo = ctx.simple_framebuffer((8, 8), components=4)
        fbo.use()
        prog = ctx.program(vertex_shader=VERTEX_SHADER, fragment_shader=FRAGMENT_SHADER)
        vbo = ctx.buffer(struct.pack("6f", -1.0, -1.0, 3.0, -1.0, -1.0, 3.0))  # covers the viewport
        vao = ctx.vertex_array(prog, [(vbo, "2f", "in_vert")])

        for i in range(rounds):
            red = (index * 40 + i) % 256
            green = (index * 7 + i * 3) % 256
            blue = (255 - index * 30) % 256
            prog["color"].value = (red / 255.0, green / 255.0, blue / 255.0, 1.0)
            fbo.clear(0.0, 0.0, 0.0, 1.0)
            vao.render(moderngl.TRIANGLES, vertices=3)
            data = fbo.read(components=4)
            want = bytes((red, green, blue, 255))
            assert data[:4] == want and data[-4:] == want, (index, i, data[:4], want)

        assert ctx.error == "GL_NO_ERROR"
        for obj in (vao, vbo, prog, fbo):
            obj.release()
        ctx.release()

    run_threads(count, work)


def scenario_concurrent_release(kinds=None):
    """Release the same object from 8 threads at once, it has to be released once."""
    moderngl = import_moderngl()
    ctx = create_context(moderngl)

    prog = ctx.program(vertex_shader=VERTEX_SHADER, fragment_shader=FRAGMENT_SHADER)
    helper_texture = ctx.texture((4, 4), 4)
    helper_fbo = ctx.framebuffer(color_attachments=[helper_texture])

    factories = {
        "buffer": lambda: ctx.buffer(reserve=64),
        "texture": lambda: ctx.texture((4, 4), 4),
        "texture3d": lambda: ctx.texture3d((2, 2, 2), 4),
        "texture_array": lambda: ctx.texture_array((2, 2, 2), 4),
        "texture_cube": lambda: ctx.texture_cube((2, 2), 4),
        "framebuffer": lambda: ctx.framebuffer(color_attachments=[helper_texture]),
        "program": lambda: ctx.program(vertex_shader=VERTEX_SHADER),
        "vertex_array": lambda: ctx.vertex_array(prog, []),
        "sampler": lambda: ctx.sampler(),
        "renderbuffer": lambda: ctx.renderbuffer((4, 4)),
        "scope": lambda: ctx.scope(helper_fbo),
    }

    for kind in kinds or factories:
        make = factories[kind]
        gc.collect()
        baseline = refs(ctx.mglo)

        objects = []

        def prepare(i):
            # The objects are kept in a list, the threads get the mgl object (that is what is released)
            objects.append(make())
            return objects[-1].mglo

        def release(index, raw):
            raw.release()

        run_rounds(8, 200, prepare, release)

        for obj in objects:
            obj.release()  # already released, a no-op
        del objects, obj
        gc.collect()
        assert refs(ctx.mglo) == baseline, "%s: the references to the context are %d, not %d" % (
            kind,
            refs(ctx.mglo),
            baseline,
        )


def scenario_concurrent_release_context():
    """Release the same context from 8 threads at once."""
    moderngl = import_moderngl()
    create_context(moderngl).release()

    for _ in range(20):
        ctx = moderngl.create_context(standalone=True, backend="egl")
        raw = ctx.mglo
        # Not current on the threads releasing it: unbind it here first
        ctx.mglo.__exit__(None, None, None)
        run_threads(8, lambda index: raw.release())
        ctx.release()
        del ctx, raw
        gc.collect()


def scenario_shared_context():
    """Several threads use the same context and the same two framebuffers."""
    moderngl = import_moderngl()
    import struct

    ctx = create_context(moderngl)
    fbo_a = ctx.simple_framebuffer((8, 8))
    fbo_b = ctx.simple_framebuffer((8, 8))
    prog = ctx.program(vertex_shader=VERTEX_SHADER, fragment_shader=FRAGMENT_SHADER)
    vbo = ctx.buffer(struct.pack("6f", -1.0, -1.0, 3.0, -1.0, -1.0, 3.0))
    vao = ctx.vertex_array(prog, [(vbo, "2f", "in_vert")])
    count = 8
    scopes = [ctx.scope(fbo_a if i % 2 else fbo_b, enable_only=moderngl.BLEND) for i in range(count)]

    # The framebuffer that is bound holds one reference. fbo_b is bound now and at the end.
    fbo_a.use()
    fbo_b.use()
    gc.collect()
    baseline_a = refs(fbo_a.mglo)
    baseline_b = refs(fbo_b.mglo)
    baseline_ctx = refs(ctx.mglo)
    baseline_vao = refs(vao.mglo)

    def work(index):
        mine = fbo_a if index % 2 else fbo_b
        other = fbo_b if index % 2 else fbo_a
        scope = scopes[index]
        for i in range(300):
            mine.use()
            other.use()
            ctx.clear(0.0, 0.0, 0.0, 0.0)
            assert ctx.mglo.fbo is not None  # the one that is bound, it changes all the time
            ctx.enable(moderngl.BLEND | moderngl.DEPTH_TEST)
            ctx.disable(moderngl.DEPTH_TEST)
            vao.render(moderngl.TRIANGLES, vertices=3)
            if i % 20 == 0:
                with scope:
                    vao.render(moderngl.TRIANGLES, vertices=3)
            ctx.viewport
            mine.viewport = (0, 0, 8, 8)

    run_threads(count, work)

    fbo_b.use()
    ctx.disable(moderngl.BLEND)
    gc.collect()
    assert refs(fbo_a.mglo) == baseline_a, (refs(fbo_a.mglo), baseline_a)
    assert refs(fbo_b.mglo) == baseline_b, (refs(fbo_b.mglo), baseline_b)
    assert refs(ctx.mglo) == baseline_ctx
    assert refs(vao.mglo) == baseline_vao

    # Everything still works
    fbo_b.use()
    fbo_b.clear(0.0, 0.0, 0.0, 1.0)
    prog["color"].value = (1.0, 0.0, 0.0, 1.0)
    vao.render(moderngl.TRIANGLES, vertices=3)
    assert fbo_b.read(components=4)[:4] == bytes((255, 0, 0, 255))


def scenario_fragment_outputs():
    """The fragment_outputs dict of ctx.program is changed by another thread meanwhile."""
    moderngl = import_moderngl()
    ctx = create_context(moderngl)

    outputs = {"fragColor": 0}
    for i in range(300):
        outputs["unused%d" % i] = i % 4
    stop = threading.Event()
    compiled = [0]

    def mutate():
        i = 0
        while not stop.is_set():
            i += 1
            name = "unused%d" % (i % 300)
            outputs.pop(name, None)
            outputs["".join(["unused", str(i % 300)])] = i % 4  # a new string every time
            if i % 50 == 0:
                for j in range(300):
                    outputs.pop("unused%d" % j, None)
                for j in range(300):
                    outputs["unused%d" % j] = j % 4

    thread = threading.Thread(target=mutate)
    thread.start()
    try:
        for _ in range(150):
            prog = ctx.program(
                vertex_shader=VERTEX_SHADER,
                fragment_shader=FRAGMENT_SHADER,
                fragment_outputs=outputs,
            )
            compiled[0] += 1
            prog.release()
            ctx.clear_errors()
    finally:
        stop.set()
        thread.join()
    assert compiled[0] == 150


def scenario_index_buffer_swap():
    """Threads replace the index buffer of a vertex array while it is rendered."""
    moderngl = import_moderngl()
    import struct

    ctx = create_context(moderngl)
    fbo = ctx.simple_framebuffer((8, 8))
    fbo.use()
    prog = ctx.program(vertex_shader=VERTEX_SHADER, fragment_shader=FRAGMENT_SHADER)
    vbo = ctx.buffer(struct.pack("6f", -1.0, -1.0, 3.0, -1.0, -1.0, 3.0))
    index_a = ctx.buffer(struct.pack("3I", 0, 1, 2))
    index_b = ctx.buffer(struct.pack("3I", 0, 1, 2))
    vao = ctx.vertex_array(prog, [(vbo, "2f", "in_vert")], index_buffer=index_a)
    raw = vao.mglo

    gc.collect()
    # The vertex array holds a reference to its index buffer (a now, a at the end)
    baseline_a = refs(index_a.mglo)
    baseline_b = refs(index_b.mglo)

    stop = threading.Event()

    def swap(index):
        buffers = (index_a.mglo, index_b.mglo)
        for i in range(2000):
            raw.index_buffer = buffers[(i + index) % 2]

    def render():
        while not stop.is_set():
            vao.render(moderngl.TRIANGLES, vertices=3)

    renderers = [threading.Thread(target=render) for _ in range(2)]
    for thread in renderers:
        thread.start()
    try:
        run_threads(6, swap)
    finally:
        stop.set()
        for thread in renderers:
            thread.join()

    raw.index_buffer = index_a.mglo
    gc.collect()
    assert refs(index_a.mglo) == baseline_a, (refs(index_a.mglo), baseline_a)
    assert refs(index_b.mglo) == baseline_b, (refs(index_b.mglo), baseline_b)


def scenario_transform_outputs():
    """The list of outputs of a transform is changed by another thread meanwhile."""
    moderngl = import_moderngl()
    import struct

    ctx = create_context(moderngl)
    prog = ctx.program(vertex_shader=TRANSFORM_SHADER, varyings=["out_value"])
    source = ctx.buffer(struct.pack("4f", 1.0, 2.0, 3.0, 4.0))
    out_a = ctx.buffer(reserve=16)
    out_b = ctx.buffer(reserve=16)
    vao = ctx.vertex_array(prog, [(source, "f", "in_value")])
    raw = vao.mglo
    outputs = [out_a.mglo, out_b.mglo]
    stop = threading.Event()

    def mutate():
        while not stop.is_set():
            # Between the length and the items of the list for the transform
            del outputs[:]
            outputs.append(out_a.mglo)
            outputs.append(out_b.mglo)
            outputs[1:] = []
            outputs.append(out_b.mglo)

    thread = threading.Thread(target=mutate)
    thread.start()
    try:
        for _ in range(3000):
            try:
                raw.transform(outputs, moderngl.POINTS, 4, 0, 1, 0)
            except moderngl.Error:
                pass
            ctx.clear_errors()
    finally:
        stop.set()
        thread.join()


def scenario_gc_two_threads():
    """Two threads call ctx.gc() at the same time, the objects are released once."""
    moderngl = import_moderngl()
    ctx = create_context(moderngl)
    ctx.gc_mode = "context_gc"

    released = []
    lock = threading.Lock()

    class Dead:
        def release(self):
            with lock:
                released.append(self)

    rounds = 2000
    results = []

    def prepare(i):
        ctx.objects.append(Dead())  # one object, both threads try to take it

    def collect(index, argument):
        count = ctx.gc()
        with lock:
            results.append(count)

    run_rounds(2, rounds, prepare, collect)

    assert len(released) == rounds, len(released)
    assert len(set(map(id, released))) == rounds  # every one once
    assert sum(results) == rounds


def scenario_gc_real_objects():
    """Wrappers dropped on other threads in context_gc mode are released by ctx.gc() on two threads."""
    moderngl = import_moderngl()
    ctx = create_context(moderngl)
    ctx.gc_mode = "context_gc"
    gc.collect()
    baseline = refs(ctx.mglo)

    count = 400
    buffers = [ctx.buffer(reserve=16) for _ in range(count)]
    textures = [ctx.texture((2, 2), 4) for _ in range(count)]
    raws = [b.mglo for b in buffers] + [t.mglo for t in textures]

    # The wrappers are dropped here, not on the thread that created them
    def drop_items(index):
        for i in range(index, count, 4):
            buffers[i] = None
            textures[i] = None

    run_threads(4, drop_items)
    assert len(ctx.objects) == 2 * count

    def collect(index):
        while len(ctx.objects):
            ctx.gc()

    run_threads(2, collect)
    assert len(ctx.objects) == 0
    del raws, buffers, textures
    gc.collect()
    assert refs(ctx.mglo) == baseline


def scenario_getters_first_call(name):
    """The first call of the getters that used to have static strings, from many threads at once."""
    moderngl = import_moderngl()
    ctx = create_context(moderngl)

    values = {
        "front_face": ("cw", "ccw"),
        "cull_face": ("front", "back", "front_and_back"),
    }
    attribute = name.split("=")[0]
    value = name.split("=")[1]
    setattr(ctx.mglo, attribute, value)  # a setter does not create the strings

    results = []

    def work(index):
        for _ in range(100):
            results.append(getattr(ctx.mglo, attribute))

    run_threads(16, work)
    assert set(results) == {value}, set(results)
    assert value in values[attribute]
    # The getters give back the same string objects every time
    assert getattr(ctx.mglo, attribute) is getattr(ctx.mglo, attribute)


GETTER_CASES = [
    "front_face=cw",
    "front_face=ccw",
    "cull_face=front",
    "cull_face=back",
    "cull_face=front_and_back",
]


def scenario_buffer_view_released(variant):
    """The memory view of a buffer is released after the buffer or the context was released."""
    moderngl = import_moderngl()
    ctx = create_context(moderngl)
    buffer = ctx.buffer(b"\x01\x02\x03\x04")
    view = memoryview(buffer.mglo)
    assert bytes(view) == b"\x01\x02\x03\x04"

    if variant == "buffer":
        buffer.release()
        view.release()
        assert ctx.error == "GL_NO_ERROR"
        ctx.release()
    elif variant == "context":
        ctx.release()
        view.release()  # no OpenGL context to call into any more
    elif variant == "thread":
        # The view is dropped on another thread, one that has no current context
        def drop(index):
            view.release()

        run_threads(1, drop)
        assert ctx.error == "GL_NO_ERROR"
        buffer.release()
        ctx.release()
    else:
        raise AssertionError(variant)


def scenario_import():
    moderngl = import_moderngl()
    import moderngl.mgl

    ctx = create_context(moderngl)
    buffer = ctx.buffer(reserve=16)
    buffer.release()
    ctx.release()
    if FREE_THREADED:
        assert not sys._is_gil_enabled(), "using moderngl enabled the GIL"


SCENARIOS = {
    "import": scenario_import,
    "parallel_contexts": scenario_parallel_contexts,
    "concurrent_release": scenario_concurrent_release,
    "concurrent_release_context": scenario_concurrent_release_context,
    "shared_context": scenario_shared_context,
    "fragment_outputs": scenario_fragment_outputs,
    "index_buffer_swap": scenario_index_buffer_swap,
    "transform_outputs": scenario_transform_outputs,
    "gc_two_threads": scenario_gc_two_threads,
    "gc_real_objects": scenario_gc_real_objects,
    "buffer_view_released_buffer": lambda: scenario_buffer_view_released("buffer"),
    "buffer_view_released_context": lambda: scenario_buffer_view_released("context"),
    "buffer_view_released_thread": lambda: scenario_buffer_view_released("thread"),
}
for case in GETTER_CASES:
    SCENARIOS["getters_first_call:" + case] = (lambda case: lambda: scenario_getters_first_call(case))(case)


# Tests, these run in pytest


def run_python(args, env=None, timeout=120):
    return subprocess.run(
        [sys.executable] + args,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        universal_newlines=True,
        env=env,
        timeout=timeout,
    )


def package_env():
    """The environment of the subprocess, it has to import the moderngl that is tested here."""
    import moderngl

    root = os.path.dirname(os.path.dirname(os.path.abspath(moderngl.__file__)))
    env = dict(os.environ)
    env["PYTHONPATH"] = os.pathsep.join(filter(None, [root, env.get("PYTHONPATH")]))
    return env


def run_scenario(name):
    result = run_python([os.path.abspath(__file__), name], env=package_env())
    if result.returncode == SKIP:
        pytest.skip(result.stdout.strip())
    assert result.returncode == 0, "scenario %s exited with %s\n%s" % (name, result.returncode, result.stdout)


@pytest.mark.skipif(not FREE_THREADED, reason="requires a free-threaded build")
def test_import_keeps_gil_disabled():
    """Importing and using the extension module must not re-enable the GIL."""
    env = package_env()
    env.pop("PYTHON_GIL", None)
    code = (
        "import sys, warnings\n"
        "warnings.simplefilter('error', RuntimeWarning)\n"
        "assert not sys._is_gil_enabled()\n"
        "import moderngl\n"
        "import moderngl.mgl\n"
        "assert not sys._is_gil_enabled(), 'importing moderngl enabled the GIL'\n"
    )
    result = run_python(["-c", code], env=env)
    assert result.returncode == 0, result.stdout

    # Creating a context imports glcontext too, that has to keep the GIL disabled as well
    result = run_python([os.path.abspath(__file__), "import"], env=env)
    if result.returncode == SKIP:
        pytest.skip(result.stdout.strip())
    assert result.returncode == 0, result.stdout


def test_parallel_contexts():
    run_scenario("parallel_contexts")


def test_concurrent_release():
    run_scenario("concurrent_release")


def test_concurrent_release_context():
    run_scenario("concurrent_release_context")


def test_shared_context():
    run_scenario("shared_context")


def test_fragment_outputs_changed_meanwhile():
    run_scenario("fragment_outputs")


def test_index_buffer_swap():
    run_scenario("index_buffer_swap")


def test_transform_outputs_changed_meanwhile():
    run_scenario("transform_outputs")


def test_gc_two_threads():
    run_scenario("gc_two_threads")


def test_gc_real_objects():
    run_scenario("gc_real_objects")


@pytest.mark.skipif(sys.version_info < (3, 9), reason="buffers can be exported from Python 3.9")
@pytest.mark.parametrize("variant", ["buffer", "context", "thread"])
def test_buffer_view_released(variant):
    run_scenario("buffer_view_released_" + variant)


@pytest.mark.parametrize("case", GETTER_CASES)
def test_getters_first_call(case):
    run_scenario("getters_first_call:" + case)


if __name__ == "__main__":
    SCENARIOS[sys.argv[1]]()
