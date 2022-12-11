"""
Simple test script for gc

* Leak then mgl object fails on creation?
* If object doesn't have ctx attribute it's not valid?
"""
import logging
import time
from ctypes import c_long

import numpy as np
import moderngl

logging.basicConfig(level=logging.DEBUG)

ctx = moderngl.create_context(standalone=True, require=330)

print(ctx)
ctx.gc_mode = "auto"


def ref_count():
    """Simple refcount demo"""
    var1 = [1, 2]
    print(c_long.from_address(id(var1)).value)


# --- Texture ---

def test_texture():
    return ctx.texture((100, 100), 4, data=np.random.randint(0, 1, 100 * 100 * 4, dtype="u1"), dtype="u1")

def test_texture_mass_create(n=1_000):
    for _ in range(n):
        texture = ctx.texture((1000, 1000), 4, data=np.random.randint(0, 255, 1000 * 1000 * 4, dtype="u1"))


def test_texture_failure():
    try:
        texture = ctx.texture((1000, 1000), 4, data=np.random.randint(0, 255, 1000 * 1001 * 4, dtype="u1"))
    except Exception as ex:
        print(ex)


def test_texture_failure_mass_create(n=1000):
    for i in range(n):
        test_texture_failure()


# --- TextureArray ---

def test_texture_array():
    texture = ctx.texture_array((100, 100, 100), 4, data=np.random.randint(0, 255, 100 * 100 * 100 * 4, dtype="u1"))


def test_texture_array_mass_create(n=1000):
    for _ in range(n):
        texture = ctx.texture_array((100, 100, 100), 4, data=np.random.randint(0, 255, 100 * 100 * 100 * 4, dtype="u1"))


def test_texture_array_failure():
    try:
        texture = ctx.texture_array((100, 100, 10), 4, data=np.random.randint(0, 255, 100 * 100 * 100 * 4, dtype="u1"))
    except Exception as ex:
        print(ex)


def test_texture_array_failure_mass_create(n=1000):
    for i in range(n):
        test_texture_array_failure()


# --- Texture3D ---

def test_texture_3d():
    texture = ctx.texture3d((100, 100, 100), 4, data=np.random.randint(0, 255, 100 * 100 * 100 * 4, dtype="u1"))


def test_texture_3d_mass_create(n=1000):
    for _ in range(n):
        texture = ctx.texture3d((100, 100, 100), 4, data=np.random.randint(0, 255, 100 * 100 * 100 * 4, dtype="u1"))


def test_texture_3d_failure():
    try:
        texture = ctx.texture3d((100, 100, 100), 4, data=np.random.randint(0, 255, 100 * 100 * 101 * 4, dtype="u1"))
    except Exception as ex:
        print(ex)


def test_texture_3d_failure_mass_create(n=1000):
    for i in range(n):
        test_texture_3d_failure()


# --- Framebuffer ---

def test_framebuffer():
    texture = ctx.texture((100, 100), 4)
    fbo = ctx.framebuffer(color_attachments=[texture])


def test_framebuffer_mass_create(n=1000):
    for i in range(n):
        texture = ctx.texture((100, 100), 4)
        ctx.framebuffer(color_attachments=[texture])
        # Depending on drivers FBOs might not release until flush/finish
        ctx.finish()


def test_framebuffer_failure():
    try:
        fbo = ctx.framebuffer(color_attachments=[])
    except Exception as ex:
        print(ex)


def test_framebuffer_failure_mass_create(n=1000):
    for i in range(n):
        test_framebuffer_failure()


# --- RenderBuffer ---

def test_renderbuffer_mass_create(n=1000):
    for i in range(n):
        rb = ctx.renderbuffer((1000, 1000))
        ctx.finish()


def test_renderbuffer_failure_mass_create(n=1000):
    for i in range(n):
        try:
            rb = ctx.renderbuffer((1000, 1000), components=4, dtype="moo")
        except Exception as ex:
            print(ex)
        ctx.finish()

# --- Buffer ---

def test_buffer():
    buffer = ctx.buffer(data=np.random.randint(0, 255, 1024 * 1024 * 1024, dtype="u1"))
    buffer.release()


def test_buffer_mass_create(n=1000):
    for i in range(n):
        b = ctx.buffer(data=np.random.randint(0, 255, 1024 * 1024, dtype="u1"))


def test_buffer_creation_failed():
    try:
        buffer = ctx.buffer(data=None)
    except Exception as ex:
        print(ex)


def test_buffer_creation_failed_mass_create(n=1000):
    for i in range(n):
        test_buffer_creation_failed()


# --- Query ---

def test_query_mass_create(n=1000):
    for _ in range(n):
        q = ctx.query(samples=True, any_samples=True, time=True, primitives=True)
        # print(c_long.from_address(id(q.mglo)))


# --- VertexArray / Program ---

prog_src = {
        "vertex_shader": """
        #version 330
        in vec2 in_pos;
        void main() {
            gl_Position = vec4(in_pos, 0.0, 1.0);
        }
        """,
        "fragment_shader": """
        #version 330
        out vec4 color;
        void main() {
            color = vec4(1.0);
        }
        """
}


def test_vertex_array():
    buffer = ctx.buffer(reserve=1024)
    prog = ctx.program(**prog_src)
    vao = ctx.vertex_array(prog, [(buffer, "2f", "in_pos")])


def test_vertex_array_mass_create(n=1000):
    for i in range(n):
        prog = ctx.program(**prog_src)
        vbo = ctx.buffer(reserve=1_000_000)
        ibo = ctx.buffer(reserve=1_000_000)
        vao = ctx.vertex_array(prog, [(vbo, "2f", "in_pos")], index_buffer=ibo)


def test_vertex_array_creation_failure():
    try:
        buffer = ctx.buffer(reserve=1024)
        prog = ctx.program(**prog_src)
        vao = ctx.vertex_array(prog, [(buffer, "2f", "in_pos_invalid")])
    except moderngl.Error:
        pass


def test_vertex_array_creation_failure_mass_create(n=1000):
    for i in range(n):
        test_vertex_array_creation_failure()


# --- ComputeShader ---

def test_compute_shader():
    compute_shader = ctx.compute_shader('''
        #version 430
        layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
        void main() {
        }
    ''')


def test_compute_shader_mass_create(n=1000):
    for i in range(n):
        test_compute_shader()
        print(i)


def test_compute_shader_fail():
    try:
        compute_shader = ctx.compute_shader('''
            #version 430
            layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
            void main() {
            }
        ''')
    except Exception as ex:
        print(ex)


def test_compute_shader_fail_mass_create(n=1000):
    for i in range(n):
        test_compute_shader_fail()


# --- Sampler ---

def test_sampler_mass_create(n=1000):
    for i in range(n):
        sampler = ctx.sampler()


# --- Scope ---

def test_scope():
    fbo = ctx.simple_framebuffer((100, 100))
    tex1 = ctx.texture((10, 100), 4)
    tex2 = ctx.texture((10, 100), 4)
    ubo1 = ctx.buffer(reserve=16000)
    ubo2 = ctx.buffer(reserve=16000)
    ssob1 = ctx.buffer(reserve=1024)
    ssob2 = ctx.buffer(reserve=1024)

    for i in range(10_000):
        print(i)
        scope = ctx.scope(
            framebuffer=fbo,
            enable_only=moderngl.NOTHING,
            textures=[(tex1, 0), (tex2, 1)],
            uniform_buffers=[(ubo1, 0), (ubo2, 1)],
            storage_buffers=[(ssob1, 0), (ssob2, 1)],
        )
        # scope = ctx.scope(
        #     framebuffer=ctx.simple_framebuffer((100, 100)),
        #     enable_only=moderngl.NOTHING,
        #     textures=[(ctx.texture((10, 100), 4), 0), (ctx.texture((10, 100), 4), 1)],
        #     uniform_buffers=[(ctx.buffer(reserve=16000), 0), (ctx.buffer(reserve=16000), 1)],
        #     storage_buffers=[(ctx.buffer(reserve=1024), 0), (ctx.buffer(reserve=1024), 1)],
        # )
        # with scope:
        #     pass


# ------------------------------------------------------------------------------------------

# ref_count()

N = 100

test_texture()
test_texture_mass_create(N)
test_texture_failure()
test_texture_failure_mass_create(N)

test_texture_array()
test_texture_array_mass_create(N)
test_texture_array_failure()
test_texture_array_failure_mass_create(N)

test_texture_3d()
test_texture_3d_mass_create(N)
test_texture_3d_failure()
test_texture_3d_failure_mass_create(N)

test_framebuffer()
test_framebuffer_mass_create(N)
test_framebuffer_failure()
test_framebuffer_failure_mass_create(N)

test_renderbuffer_mass_create(N)
test_renderbuffer_failure_mass_create(N)

test_buffer()
test_buffer_mass_create(N)
test_buffer_creation_failed()
test_buffer_creation_failed_mass_create(N)

test_query_mass_create()

test_vertex_array()
test_vertex_array_mass_create(N)
test_vertex_array_creation_failure()
test_vertex_array_creation_failure_mass_create(N)

if ctx.version_code >= 430:
    test_compute_shader()
    test_compute_shader_mass_create(N)
    test_compute_shader_fail()
    test_compute_shader_fail_mass_create(N)

test_sampler_mass_create(N)

# test_scope()
print("Done. Waiting")
time.sleep(300)
