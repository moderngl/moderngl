"""
Tests for bindless texture handle functionality.

Tests the new ability to assign lists of texture handles to uniform arrays,
which was added to support bindless texture arrays in OpenGL.
"""
import ctypes
import pytest
import moderngl


@pytest.fixture
def make_sampler_array_program(ctx):
    """Returns a callable that creates a minimal `#version 330` program with
    `sampler2D Textures[count]` and no bindless extension.

    Used by tests that exercise Uniform.handle's Python-side validation or
    the C-side pre-GL guards in `_set_uniform_handle` / `_get_uniform_handle`
    -- those checks all fire before any bindless GL call, so the test only
    needs a uniform with a known array_length. This lets the validation
    tests run on any GL >= 3.3 context (e.g., Mesa CI), separating wrapper-
    logic regressions from end-to-end bindless behavior.
    """
    def make(count):
        return ctx.program(
            vertex_shader="""
                #version 330
                void main() { gl_Position = vec4(0.0); }
            """,
            fragment_shader=f"""
                #version 330
                uniform sampler2D Textures[{count}];
                out vec4 fragColor;
                void main() {{ fragColor = texture(Textures[0], vec2(0.5)); }}
            """,
        )
    return make


def _load_is_resident(ctx):
    """Returns a callable wrapping glIsTextureHandleResidentARB.

    PyOpenGL's static binding for this ARB-extension entry point can't be
    resolved against an EGL context, so we load the function pointer
    directly via the glcontext attached to the moderngl Context and wrap
    it with ctypes.
    """
    fn_ptr = ctx.mglo._context.load_opengl_function("glIsTextureHandleResidentARB")
    if not fn_ptr:
        return None
    prototype = ctypes.CFUNCTYPE(ctypes.c_ubyte, ctypes.c_uint64)
    return prototype(fn_ptr)


def test_single_handle_backward_compatibility(ctx):
    """Tests that single handle assignment still works (backward compatibility)."""
    if not ctx.supports_bindless:
        pytest.skip("Bindless textures not supported")

    prog = ctx.program(
        vertex_shader="""
            #version 330
            void main() {
                gl_Position = vec4(0.0);
            }
        """,
        fragment_shader="""
            #version 330
            #extension GL_ARB_bindless_texture : require
            layout (bindless_sampler) uniform sampler2D Texture;
            out vec4 fragColor;
            void main() {
                fragColor = texture(Texture, vec2(0.5));
            }
        """,
    )

    texture = ctx.texture((4, 4), 4)
    handle = texture.get_handle()

    # Should work without raising an exception
    prog["Texture"].handle = handle

    # Safe to release after assigning the handle: texture.release() makes
    # the bindless handle non-resident before deleting the texture, which
    # the GL spec requires. The uniform's stored handle becomes dangling,
    # so don't sample from it after this point. Always release textures
    # this way (or rely on a context with gc_mode="auto") to avoid leaking
    # resident handles.
    texture.release()


def test_handle_list_correct_length(ctx):
    """Tests assigning a list of handles with correct array length."""
    if not ctx.supports_bindless:
        pytest.skip("Bindless textures not supported")

    prog = ctx.program(
        vertex_shader="""
            #version 330
            void main() {
                gl_Position = vec4(0.0);
            }
        """,
        fragment_shader="""
            #version 330
            #extension GL_ARB_bindless_texture : require
            layout (bindless_sampler) uniform sampler2D Textures[3];
            out vec4 fragColor;
            void main() {
                fragColor = texture(Textures[0], vec2(0.5)) +
                            texture(Textures[1], vec2(0.5)) +
                            texture(Textures[2], vec2(0.5));
            }
        """,
    )

    # Create 3 textures and get their handles
    textures = [ctx.texture((4, 4), 4) for _ in range(3)]
    handles = [tex.get_handle() for tex in textures]

    # Should work - list length matches array_length
    prog["Textures"].handle = handles

    # Cleanup
    for tex in textures:
        tex.release()


def test_handle_list_wrong_length_too_many(make_sampler_array_program):
    """Tests that assigning too many handles raises ValueError. The length
    check fires in Python before any GL call, so this works on any context."""
    prog = make_sampler_array_program(3)
    with pytest.raises(ValueError, match="has 5 elements but uniform array requires exactly 3"):
        prog["Textures"].handle = [1, 2, 3, 4, 5]


def test_handle_list_wrong_length_too_few(make_sampler_array_program):
    """Tests that assigning too few handles raises ValueError."""
    prog = make_sampler_array_program(3)
    with pytest.raises(ValueError, match="has 2 elements but uniform array requires exactly 3"):
        prog["Textures"].handle = [1, 2]


def test_handle_list_empty(make_sampler_array_program):
    """Tests that assigning an empty list is rejected."""
    prog = make_sampler_array_program(3)
    with pytest.raises(ValueError, match="has 0 elements but uniform array requires exactly 3"):
        prog["Textures"].handle = []


def test_handle_scalar_to_array_uniform(make_sampler_array_program):
    """Tests that assigning a scalar handle to an array uniform is rejected.

    Without this check, the scalar would silently be written to slot 0 of
    the array via ProgramUniformHandleui64ARB, leaving slots 1..N-1
    untouched. That's almost always a bug (forgot to wrap in a list).
    """
    prog = make_sampler_array_program(3)
    with pytest.raises(ValueError, match="uniform array of length 3 .* list"):
        prog["Textures"].handle = 1


def test_handle_list_non_integer_elements(make_sampler_array_program):
    """Tests that non-integer elements in handle list raise TypeError. The
    type check fires in C before the bindless GL call."""
    prog = make_sampler_array_program(3)
    with pytest.raises(TypeError, match="All items in handle list must be integers"):
        prog["Textures"].handle = ["not", "an", "integer"]


def test_handle_list_mixed_types(make_sampler_array_program):
    """Tests that mixing valid and invalid types raises TypeError."""
    prog = make_sampler_array_program(3)
    with pytest.raises(TypeError, match="All items in handle list must be integers"):
        prog["Textures"].handle = [1, "bad", 2]


def test_handle_large_array(ctx):
    """Tests assigning handles to a larger array (performance/stress test)."""
    if not ctx.supports_bindless:
        pytest.skip("Bindless textures not supported")

    prog = ctx.program(
        vertex_shader="""
            #version 330
            void main() {
                gl_Position = vec4(0.0);
            }
        """,
        fragment_shader="""
            #version 330
            #extension GL_ARB_bindless_texture : require
            layout (bindless_sampler) uniform sampler2D Textures[16];
            out vec4 fragColor;
            void main() {
                vec4 color = vec4(0.0);
                for (int i = 0; i < 16; i++) {
                    color += texture(Textures[i], vec2(0.5));
                }
                fragColor = color;
            }
        """,
    )

    # Create 16 textures
    textures = [ctx.texture((4, 4), 4) for _ in range(16)]
    handles = [tex.get_handle() for tex in textures]

    # Should work with larger arrays
    prog["Textures"].handle = handles

    # Cleanup
    for tex in textures:
        tex.release()


def test_handle_uniform_not_array(ctx):
    """Tests that single uniform (not array) still works with list of size 1."""
    if not ctx.supports_bindless:
        pytest.skip("Bindless textures not supported")

    prog = ctx.program(
        vertex_shader="""
            #version 330
            void main() {
                gl_Position = vec4(0.0);
            }
        """,
        fragment_shader="""
            #version 330
            #extension GL_ARB_bindless_texture : require
            layout (bindless_sampler) uniform sampler2D Texture;
            out vec4 fragColor;
            void main() {
                fragColor = texture(Texture, vec2(0.5));
            }
        """,
    )

    texture = ctx.texture((4, 4), 4)
    handle = texture.get_handle()

    # For a non-array uniform, array_length must be 1, and a list with 1
    # element should work.
    assert prog["Texture"].array_length == 1
    prog["Texture"].handle = [handle]

    texture.release()


def test_nonexistent_uniform_not_in_program(ctx):
    """Tests that a uniform absent from the shader doesn't appear in prog's
    member dict. Doesn't exercise bindless; lives here for proximity to the
    other handle-related tests."""
    prog = ctx.program(
        vertex_shader="""
            #version 330
            void main() {
                gl_Position = vec4(0.0);
            }
        """,
        fragment_shader="""
            #version 330
            out vec4 fragColor;
            void main() {
                fragColor = vec4(1.0);
            }
        """,
    )

    assert "NonExistentUniform" not in prog


def test_handle_integer_overflow(make_sampler_array_program):
    """Tests that integer overflow in handle values is caught. The overflow
    error fires in C (PyLong_AsUnsignedLongLong) before the bindless GL call."""
    prog = make_sampler_array_program(2)
    huge_number = 2**128  # Way beyond uint64 max
    with pytest.raises((OverflowError, ValueError)):
        prog["Textures"].handle = [huge_number, huge_number]


def test_get_uniform_handle_rejects_array_length_mismatch(make_sampler_array_program):
    """Regression: _get_uniform_handle must reject a caller-supplied
    array_length that disagrees with the program's actual array size.

    Without the C-side check, an undersized array_length causes
    glGetUniformui64v to overflow our PyMem_Malloc'd buffer (heap
    corruption). The check must catch the mismatch even when the
    Python wrapper is bypassed -- e.g. by mutating Uniform.array_length
    or calling ctx._get_uniform_handle directly.

    Uses a non-bindless sampler array because the array-length validation
    in lookup_uniform_array_size relies only on core GL introspection.
    """
    prog = make_sampler_array_program(16)
    uniform = prog["Textures"]
    assert uniform.array_length == 16

    # Direct call to the underlying C method bypasses Uniform.handle entirely.
    with pytest.raises(ValueError, match="array_length mismatch"):
        uniform.ctx._get_uniform_handle(uniform.program_obj, uniform.location, 1)
    with pytest.raises(ValueError, match="array_length mismatch"):
        uniform.ctx._get_uniform_handle(uniform.program_obj, uniform.location, 100)

    # Mutating Uniform.array_length goes through the public accessor.
    uniform.array_length = 1
    with pytest.raises(ValueError, match="array_length mismatch"):
        _ = uniform.handle
    uniform.array_length = 16  # restore


def test_get_single_handle(ctx):
    """Tests that getting a single handle works."""
    if not ctx.supports_bindless:
        pytest.skip("Bindless textures not supported")

    prog = ctx.program(
        vertex_shader="""
            #version 330
            void main() {
                gl_Position = vec4(0.0);
            }
        """,
        fragment_shader="""
            #version 330
            #extension GL_ARB_bindless_texture : require
            layout (bindless_sampler) uniform sampler2D Texture;
            out vec4 fragColor;
            void main() {
                // Actually use the texture to prevent optimization
                fragColor = texture(Texture, vec2(0.5, 0.5));
            }
        """,
    )

    texture = ctx.texture((4, 4), 4)
    handle = texture.get_handle()

    # Set the handle
    prog["Texture"].handle = handle

    # Get the handle back and verify it matches
    retrieved_handle = prog["Texture"].handle
    assert retrieved_handle == handle, f"Expected {handle}, got {retrieved_handle}"

    texture.release()


def test_get_handle_array(ctx):
    """Tests that getting a list of handles works."""
    if not ctx.supports_bindless:
        pytest.skip("Bindless textures not supported")

    prog = ctx.program(
        vertex_shader="""
            #version 330
            void main() {
                gl_Position = vec4(0.0);
            }
        """,
        fragment_shader="""
            #version 330
            #extension GL_ARB_bindless_texture : require
            layout (bindless_sampler) uniform sampler2D Textures[3];
            out vec4 fragColor;
            void main() {
                // Actually use the textures to prevent optimization
                fragColor = texture(Textures[0], vec2(0.5, 0.5)) +
                            texture(Textures[1], vec2(0.5, 0.5)) +
                            texture(Textures[2], vec2(0.5, 0.5));
            }
        """,
    )

    # Create 3 textures and get their handles
    textures = [ctx.texture((4, 4), 4) for _ in range(3)]
    handles = [tex.get_handle() for tex in textures]

    # Set the handles
    prog["Textures"].handle = handles

    # Get the handles back and verify they match
    retrieved_handles = prog["Textures"].handle
    assert isinstance(retrieved_handles, list), f"Expected list, got {type(retrieved_handles)}"
    assert len(retrieved_handles) == 3, f"Expected 3 handles, got {len(retrieved_handles)}"
    assert retrieved_handles == handles, f"Expected {handles}, got {retrieved_handles}"

    # Cleanup
    for tex in textures:
        tex.release()


def test_bindless_state_machine_no_gl_errors(ctx):
    """Exercises the BindlessHandleState transitions and asserts no GL
    error is generated.

    Per the GL_ARB_bindless_texture spec, each of the following is an
    INVALID_OPERATION:
      - MakeTextureHandleResidentARB on an already-resident handle
      - MakeTextureHandleNonResidentARB on a non-resident handle
      - Deleting a texture whose handle is still resident

    The state machine in BindlessHandleState exists to short-circuit
    the redundant transitions and to non-resident-then-delete on
    release. If a future refactor breaks any of those guards, this
    test catches the resulting GL error.
    """
    if not ctx.supports_bindless:
        pytest.skip("Bindless textures not supported")

    is_resident = _load_is_resident(ctx)
    if is_resident is None:
        pytest.skip("Could not load glIsTextureHandleResidentARB")

    def assert_clean(label):
        err = ctx.error
        assert err == "GL_NO_ERROR", f"GL error after {label}: {err}"

    def assert_resident(handle, expected, label):
        actual = bool(is_resident(handle))
        assert actual == expected, (
            f"After {label}: expected resident={expected}, GL says {actual}"
        )

    assert_clean("baseline")

    # 1. get_handle is memoized: two resident=True calls in a row must
    #    not double-call MakeTextureHandleResidentARB.
    tex = ctx.texture((4, 4), 4)
    h1 = tex.get_handle(resident=True)
    assert_clean("first get_handle(resident=True)")
    assert_resident(h1, True, "first get_handle(resident=True)")
    h2 = tex.get_handle(resident=True)
    assert_clean("second get_handle(resident=True)")
    assert_resident(h2, True, "second get_handle(resident=True)")
    assert h1 == h2, "get_handle should return the same handle each call"

    # 2. Toggle resident -> non-resident -> resident.
    tex.get_handle(resident=False)
    assert_clean("toggle to non-resident")
    assert_resident(h1, False, "toggle to non-resident")
    tex.get_handle(resident=False)
    assert_clean("redundant non-resident (should be no-op)")
    assert_resident(h1, False, "redundant non-resident")
    tex.get_handle(resident=True)
    assert_clean("toggle back to resident")
    assert_resident(h1, True, "toggle back to resident")

    # 3. Release while resident: state machine must non-res first. Can't
    #    query residency after release -- the handle is invalid.
    tex.release()
    assert_clean("release while resident")

    # 4. Release without ever obtaining a handle: must be a clean no-op
    #    for the bindless side, just delete the texture.
    untouched = ctx.texture((4, 4), 4)
    untouched.release()
    assert_clean("release without get_handle")

    # 5. Get a handle but never make it resident, then release.
    no_res = ctx.texture((4, 4), 4)
    h_no_res = no_res.get_handle(resident=False)
    assert_clean("get_handle(resident=False) on fresh texture")
    assert_resident(h_no_res, False, "get_handle(resident=False) on fresh texture")
    no_res.release()
    assert_clean("release without ever being resident")


def test_bindless_texture_array_integration(bindless_ctx, bindless_textures, ndc_quad):
    """Integration test: actually samples from bindless texture arrays and verifies output."""
    import numpy as np

    # Use first 3 textures from the fixture
    test_textures = bindless_textures[:3]

    # Get handles for the textures
    handles = [tex.get_handle() for tex in test_textures]

    # Create a shader that samples from a texture array
    prog = bindless_ctx.program(
        vertex_shader="""
            #version 330
            in vec2 in_vert;
            void main() {
                gl_Position = vec4(in_vert, 0.0, 1.0);
            }
        """,
        fragment_shader="""
            #version 330
            #extension GL_ARB_bindless_texture : require
            layout (bindless_sampler) uniform sampler2D Textures[3];
            uniform int texIndex;
            out vec4 fragColor;
            void main() {
                fragColor = texture(Textures[texIndex], vec2(0.5, 0.5));
            }
        """,
    )

    # Set the texture handles
    prog["Textures"].handle = handles

    # Create framebuffer and VAO
    fbo = bindless_ctx.simple_framebuffer((4, 4))
    fbo.use()
    vao = bindless_ctx.simple_vertex_array(prog, ndc_quad, "in_vert")

    # Test sampling from each texture in the array
    for i in range(3):
        prog["texIndex"].value = i
        fbo.clear(0.0, 0.0, 0.0, 1.0)
        vao.render(mode=moderngl.TRIANGLE_STRIP)

        result = np.frombuffer(fbo.read(components=4), dtype=np.uint8)
        expected_value = i * 40

        # Check that we got approximately the right color
        actual_r = result[0]
        assert abs(actual_r - expected_value) < 5, \
            f"Texture {i}: expected ~{expected_value}, got {actual_r}"

    # Cleanup
    vao.release()
    fbo.release()
