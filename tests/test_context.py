import pytest
import moderngl
import numpy
import platform


def test_create_destroy(ctx_new):
    """Create and destroy a context"""
    # Context will destroy itself on exit
    pass


def test_extensions(ctx_new):
    # self.assertTrue("GL_ARB_vertex_array_object" in ctx.extensions)
    # self.assertTrue("GL_ARB_transform_feedback2" in ctx.extensions)
    # self.assertTrue("GL_ARB_shader_subroutine" in ctx.extensions)
    ctx = ctx_new
    assert isinstance(ctx.extensions, set)
    assert len(ctx.extensions) > 0

def test_attributes(ctx_new):
    """Ensure enums are present in the context instance"""
    ctx = ctx_new
    # Flags
    assert isinstance(ctx.NOTHING, int)
    assert isinstance(ctx.BLEND, int)
    assert isinstance(ctx.DEPTH_TEST, int)
    assert isinstance(ctx.CULL_FACE, int)
    assert isinstance(ctx.RASTERIZER_DISCARD, int)
    assert isinstance(ctx.PROGRAM_POINT_SIZE, int)

    # Primitive modes
    assert isinstance(ctx.POINTS, int)
    assert isinstance(ctx.LINES, int)
    assert isinstance(ctx.LINE_LOOP, int)
    assert isinstance(ctx.LINE_STRIP, int)
    assert isinstance(ctx.TRIANGLES, int)
    assert isinstance(ctx.TRIANGLE_STRIP, int)
    assert isinstance(ctx.TRIANGLE_FAN, int)
    assert isinstance(ctx.LINES_ADJACENCY, int)
    assert isinstance(ctx.LINE_STRIP_ADJACENCY, int)
    assert isinstance(ctx.TRIANGLES_ADJACENCY, int)
    assert isinstance(ctx.TRIANGLE_STRIP_ADJACENCY, int)
    assert isinstance(ctx.PATCHES, int)

    # Texture filters
    assert isinstance(ctx.LINEAR, int)
    assert isinstance(ctx.NEAREST, int)
    assert isinstance(ctx.NEAREST_MIPMAP_NEAREST, int)
    assert isinstance(ctx.LINEAR_MIPMAP_LINEAR, int)
    assert isinstance(ctx.LINEAR_MIPMAP_NEAREST, int)
    assert isinstance(ctx.NEAREST_MIPMAP_LINEAR, int)

    # Blend functions
    assert isinstance(ctx.ZERO, int)
    assert isinstance(ctx.ONE, int)
    assert isinstance(ctx.SRC_COLOR, int)
    assert isinstance(ctx.ONE_MINUS_SRC_COLOR, int)
    assert isinstance(ctx.SRC_ALPHA, int)
    assert isinstance(ctx.ONE_MINUS_SRC_ALPHA, int)
    assert isinstance(ctx.DST_ALPHA, int)
    assert isinstance(ctx.ONE_MINUS_DST_ALPHA, int)
    assert isinstance(ctx.DST_COLOR, int)
    assert isinstance(ctx.ONE_MINUS_DST_COLOR, int)

    # Blend shortcuts
    assert isinstance(ctx.DEFAULT_BLENDING, tuple)
    assert isinstance(ctx.ADDITIVE_BLENDING, tuple)
    assert isinstance(ctx.PREMULTIPLIED_ALPHA, tuple)

    # Blend equations
    assert isinstance(ctx.FUNC_ADD, int)
    assert isinstance(ctx.FUNC_SUBTRACT, int)
    assert isinstance(ctx.FUNC_REVERSE_SUBTRACT, int)
    assert isinstance(ctx.MIN, int)
    assert isinstance(ctx.MAX, int)

    # Provoking vertex
    assert isinstance(ctx.FIRST_VERTEX_CONVENTION, int)
    assert isinstance(ctx.LAST_VERTEX_CONVENTION, int)

def test_enable_direct(ctx_new):
    # We already support this, but it's a safe value
    ctx = ctx_new
    GL_PROGRAM_POINT_SIZE = 0x8642

    ctx.enable_direct(GL_PROGRAM_POINT_SIZE)
    assert ctx.error == "GL_NO_ERROR"

    ctx.disable_direct(GL_PROGRAM_POINT_SIZE)
    assert ctx.error == "GL_NO_ERROR"

def test_info(ctx):
    assert isinstance(ctx.info, dict)
    assert len(ctx.info) > 50
    assert ctx.info["GL_MAX_GEOMETRY_OUTPUT_VERTICES"] >= 256

def test_polygon_offset(ctx):
    ctx.polygon_offset = 0.0, 1.0
    assert ctx.polygon_offset == (0.0, 1.0)
    ctx.polygon_offset = 1.0, 0.0
    assert ctx.polygon_offset == (1.0, 0.0)
    ctx.polygon_offset = -1.0, -1.0
    assert ctx.polygon_offset == (-1.0, -1.0)

def test_context_gc_modes(ctx_new):
    """Check gc mode default and set different modes"""
    ctx = ctx_new
    # gc mode should be None by default
    assert ctx.gc_mode is None
    ctx.gc_mode = None
    ctx.gc_mode = "context_gc"
    ctx.gc_mode = "auto"
    with pytest.raises(ValueError):
        ctx.gc_mode = "something"

def test_context_gc(ctx_new):
    """Simple usage of context_gc"""
    ctx = ctx_new
    ctx.gc_mode = "context_gc"

    # Buffer
    buff = ctx.buffer(reserve=1024)
    buff = None
    assert ctx.gc() == 1

    # Texture
    tex = ctx.texture((10, 10), 4)
    tex = None
    assert ctx.gc() == 1

    # Texture Array
    tex_array = ctx.texture_array((10, 10, 10), 4)
    tex_array = None
    assert ctx.gc() == 1

    # Texture cube
    tex_cube = ctx.texture_cube((10, 10), 4)
    tex_cube = None
    assert ctx.gc() == 1

    # Texture3D
    tex_3d = ctx.texture3d((10, 10, 10), 4)
    tex_3d = None
    assert ctx.gc() == 1

    # Sampler
    sampler = ctx.sampler()
    sampler = None
    assert ctx.gc() == 1

    # Renderbuffer
    rb = ctx.renderbuffer((10, 19))
    rb = None
    assert ctx.gc() == 1

    # Program
    prog = ctx.program(
        vertex_shader="""
        #version 330
        out float value;
        void main() {
            value = float(gl_VertexID);
        }
        """
    )
    prog = None
    assert ctx.gc() == 1

    # Framebuffer
    fbo = ctx.framebuffer(
        color_attachments=[ctx.texture((10, 10), 4)],
        depth_attachment=ctx.depth_texture((10, 10))
    )
    fbo = None
    assert ctx.gc() == 3

    # # Compute Shader
    # cs = ctx.compute_shader(
    #     """
    #     #version 450
        
    #     layout(local_size_x=4, local_size_y=4) in;

    #     layout(rgba8, binding=0) uniform imageCube img_in;
    #     layout(rgba8, binding=1) uniform imageCube img_out;

    #     void main() {
    #         for (int i = 0; i < 6; i++) {
    #             vec4 fragment = imageLoad(img_in, ivec3(gl_LocalInvocationID.xy, i));
    #             imageStore(img_out, ivec3(gl_LocalInvocationID.xy, i), fragment);
    #         }
    #     }
    #     """
    # )
    # cs = None
    # self.assertEqual(ctx.gc(), 1)

# # --- Too complex to test right now

# # def test_context_switch(self):
# #     """Ensure context switching is working"""
# #     ctx1 = moderngl.create_context(standalone=True)
# #     ctx2 = moderngl.create_context(standalone=True)

# #     with ctx1 as ctx:
# #         buffer1 = ctx.buffer(reserve=1024)
    
# #     with ctx2 as ctx:
# #         buffer2 = ctx.buffer(reserve=1024)

# #     self.assertEqual(buffer1.glo, buffer2.glo)
# #     ctx1.release()
# #     ctx2.release()

# # def test_exit(self):
# #     """Ensure the previous context was activated on exit"""
# #     ctx1 = moderngl.create_context(standalone=True)
# #     ctx2 = moderngl.create_context(standalone=True)

# #     with ctx1 as ctx:
# #         ctx.buffer(reserve=1024)

# #     # Will error out if no context is active "moderngl.Error: cannot create buffer"
# #     ctx1.buffer(reserve=1024)

# #     ctx1.release()
# #     ctx2.release()

# # def test_share(self):
# #     """Create resources with shared context"""
# #     if platform.system().lower() in ["darwin", "linux"]:
# #         self.skipTest('Context sharing not supported on darwin')

# #     data1 = numpy.array([1, 2, 3, 4], dtype='u1')
# #     data2 = numpy.array([4, 3, 2, 1], dtype='u1')

# #     ctx1 = moderngl.create_context(standalone=True)
# #     ctx2 = moderngl.create_context(standalone=True, share=True)

# #     with ctx1 as ctx:
# #         b1 = ctx.buffer(data=data1)

# #     with ctx2 as ctx:
# #         b2 = ctx.buffer(data=data2)

# #     # Because the resources are shared the name should increment
# #     self.assertEqual(b1.glo, 1)
# #     self.assertEqual(b2.glo, 2)

# #     # Ensure we can read the same buffer data in both contexts
# #     with ctx1:
# #         self.assertEqual(b1.read(), b'\x01\x02\x03\x04')
# #         self.assertEqual(b2.read(), b'\x04\x03\x02\x01')

# #     with ctx2:
# #         self.assertEqual(b1.read(), b'\x01\x02\x03\x04')
# #         self.assertEqual(b2.read(), b'\x04\x03\x02\x01')

# #     ctx1.release()
# #     ctx2.release()
