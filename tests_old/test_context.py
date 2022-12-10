from unittest import TestCase

import pytest
import moderngl
import numpy
import platform


class ContextTests(TestCase):

    def test_create_destroy(self):
        """Create and destroy a context"""
        for _ in range(25):
            ctx = moderngl.create_context(standalone=True)
            ctx.release()

    def test_context_switch(self):
        """Ensure context switching is working"""
        ctx1 = moderngl.create_context(standalone=True)
        ctx2 = moderngl.create_context(standalone=True)

        with ctx1 as ctx:
            buffer1 = ctx.buffer(reserve=1024)
        
        with ctx2 as ctx:
            buffer2 = ctx.buffer(reserve=1024)

        self.assertEqual(buffer1.glo, buffer2.glo)
        ctx1.release()
        ctx2.release()

    def test_exit(self):
        """Ensure the previous context was activated on exit"""
        ctx1 = moderngl.create_context(standalone=True)
        ctx2 = moderngl.create_context(standalone=True)

        with ctx1 as ctx:
            ctx.buffer(reserve=1024)

        # Will error out if no context is active "moderngl.error.Error: cannot create buffer"
        ctx1.buffer(reserve=1024)

        ctx1.release()
        ctx2.release()

    def test_share(self):
        """Create resources with shared context"""
        if platform.system().lower() in ["darwin", "linux"]:
            self.skipTest('Context sharing not supported on darwin')

        data1 = numpy.array([1, 2, 3, 4], dtype='u1')
        data2 = numpy.array([4, 3, 2, 1], dtype='u1')

        ctx1 = moderngl.create_context(standalone=True)
        ctx2 = moderngl.create_context(standalone=True, share=True)

        with ctx1 as ctx:
            b1 = ctx.buffer(data=data1)

        with ctx2 as ctx:
            b2 = ctx.buffer(data=data2)

        # Because the resources are shared the name should increment
        self.assertEqual(b1.glo, 1)
        self.assertEqual(b2.glo, 2)

        # Ensure we can read the same buffer data in both contexts
        with ctx1:
            self.assertEqual(b1.read(), b'\x01\x02\x03\x04')
            self.assertEqual(b2.read(), b'\x04\x03\x02\x01')

        with ctx2:
            self.assertEqual(b1.read(), b'\x01\x02\x03\x04')
            self.assertEqual(b2.read(), b'\x04\x03\x02\x01')

        ctx1.release()
        ctx2.release()

    def test_extensions(self):
        ctx = moderngl.create_context(standalone=True)
        # self.assertTrue("GL_ARB_vertex_array_object" in ctx.extensions)
        # self.assertTrue("GL_ARB_transform_feedback2" in ctx.extensions)
        # self.assertTrue("GL_ARB_shader_subroutine" in ctx.extensions)
        self.assertIsInstance(ctx.extensions, set)
        self.assertTrue(len(ctx.extensions) > 0)
        ctx.release()

    def test_attributes(self):
        """Ensure enums are present in the context instance"""
        ctx = moderngl.create_context(standalone=True)
        # Flags
        self.assertIsInstance(ctx.NOTHING, int)
        self.assertIsInstance(ctx.BLEND, int)
        self.assertIsInstance(ctx.DEPTH_TEST, int)
        self.assertIsInstance(ctx.CULL_FACE, int)
        self.assertIsInstance(ctx.RASTERIZER_DISCARD, int)
        self.assertIsInstance(ctx.PROGRAM_POINT_SIZE, int)

        # Primitive modes
        self.assertIsInstance(ctx.POINTS, int)
        self.assertIsInstance(ctx.LINES, int)
        self.assertIsInstance(ctx.LINE_LOOP, int)
        self.assertIsInstance(ctx.LINE_STRIP, int)
        self.assertIsInstance(ctx.TRIANGLES, int)
        self.assertIsInstance(ctx.TRIANGLE_STRIP, int)
        self.assertIsInstance(ctx.TRIANGLE_FAN, int)
        self.assertIsInstance(ctx.LINES_ADJACENCY, int)
        self.assertIsInstance(ctx.LINE_STRIP_ADJACENCY, int)
        self.assertIsInstance(ctx.TRIANGLES_ADJACENCY, int)
        self.assertIsInstance(ctx.TRIANGLE_STRIP_ADJACENCY, int)
        self.assertIsInstance(ctx.PATCHES, int)

        # Texture filters
        self.assertIsInstance(ctx.LINEAR, int)
        self.assertIsInstance(ctx.NEAREST, int)
        self.assertIsInstance(ctx.NEAREST_MIPMAP_NEAREST, int)
        self.assertIsInstance(ctx.LINEAR_MIPMAP_LINEAR, int)
        self.assertIsInstance(ctx.LINEAR_MIPMAP_NEAREST, int)
        self.assertIsInstance(ctx.NEAREST_MIPMAP_LINEAR, int)

        # Blend functions
        self.assertIsInstance(ctx.ZERO, int)
        self.assertIsInstance(ctx.ONE, int)
        self.assertIsInstance(ctx.SRC_COLOR, int)
        self.assertIsInstance(ctx.ONE_MINUS_SRC_COLOR, int)
        self.assertIsInstance(ctx.SRC_ALPHA, int)
        self.assertIsInstance(ctx.ONE_MINUS_SRC_ALPHA, int)
        self.assertIsInstance(ctx.DST_ALPHA, int)
        self.assertIsInstance(ctx.ONE_MINUS_DST_ALPHA, int)
        self.assertIsInstance(ctx.DST_COLOR, int)
        self.assertIsInstance(ctx.ONE_MINUS_DST_COLOR, int)

        # Blend shortcuts
        self.assertIsInstance(ctx.DEFAULT_BLENDING, tuple)
        self.assertIsInstance(ctx.ADDITIVE_BLENDING, tuple)
        self.assertIsInstance(ctx.PREMULTIPLIED_ALPHA, tuple)

        # Blend equations
        self.assertIsInstance(ctx.FUNC_ADD, int)
        self.assertIsInstance(ctx.FUNC_SUBTRACT, int)
        self.assertIsInstance(ctx.FUNC_REVERSE_SUBTRACT, int)
        self.assertIsInstance(ctx.MIN, int)
        self.assertIsInstance(ctx.MAX, int)

        # Provoking vertex
        self.assertIsInstance(ctx.FIRST_VERTEX_CONVENTION, int)
        self.assertIsInstance(ctx.LAST_VERTEX_CONVENTION, int)

    def test_enable_direct(self):
        ctx = moderngl.create_context(standalone=True)
        ctx.error  # consume error during initialization
        # We already support this, but it's a safe value
        GL_PROGRAM_POINT_SIZE = 0x8642

        ctx.enable_direct(GL_PROGRAM_POINT_SIZE)
        self.assertEqual(ctx.error, "GL_NO_ERROR")

        ctx.disable_direct(GL_PROGRAM_POINT_SIZE)
        self.assertEqual(ctx.error, "GL_NO_ERROR")

    def test_info(self):
        ctx = moderngl.create_context(standalone=True)
        self.assertIsInstance(ctx.info, dict)
        self.assertTrue(len(ctx.info) > 50)
        self.assertTrue(ctx.info["GL_MAX_GEOMETRY_OUTPUT_VERTICES"] >= 256)

    def test_polyon_offset(self):
        ctx = moderngl.create_context(standalone=True)
        ctx.polygon_offset = 0.0, 1.0
        self.assertEqual(ctx.polygon_offset, (0.0, 1.0))
        ctx.polygon_offset = 1.0, 0.0
        self.assertEqual(ctx.polygon_offset, (1.0, 0.0))
        ctx.polygon_offset = -1.0, -1.0
        self.assertEqual(ctx.polygon_offset, (-1.0, -1.0))

    def test_context_gc_modes(self):
        """Check gc mode default and set different modes"""
        ctx = moderngl.create_context(standalone=True)
        # gc mode should be None by default
        self.assertIsNone(ctx.gc_mode)
        ctx.gc_mode = None
        ctx.gc_mode = "auto"
        ctx.gc_mode = "context_gc"
        with pytest.raises(ValueError):
            ctx.gc_mode = "something"
        ctx.release()

    def test_context_gc(self):
        """Simple usage of context_gc"""
        ctx = moderngl.create_context(standalone=True)
        ctx.gc_mode = "context_gc"

        # Buffer
        buff = ctx.buffer(reserve=1024)
        buff = None
        self.assertEqual(ctx.gc(), 1)

        # Texture
        tex = ctx.texture((10, 10), 4)
        tex = None
        self.assertEqual(ctx.gc(), 1)

        # Texture Arrray
        tex_array = ctx.texture_array((10, 10, 10), 4)
        tex_array = None
        self.assertEqual(ctx.gc(), 1)

        # Texture cube
        tex_cube = ctx.texture_cube((10, 10), 4)
        tex_cube = None
        self.assertEqual(ctx.gc(), 1)

        # Texture3D
        tex_3d = ctx.texture3d((10, 10, 10), 4)
        tex_3d = None
        self.assertEqual(ctx.gc(), 1)

        # Sampler
        samp = ctx.sampler()
        samp = None
        self.assertEqual(ctx.gc(), 1)

        # Renderbuffer
        rb = ctx.renderbuffer((10, 19))
        rb = None
        self.assertEqual(ctx.gc(), 1)

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
        self.assertEqual(ctx.gc(), 1)

        # Framebuffer
        fbo = ctx.framebuffer(
            color_attachments=[ctx.texture((10, 10), 4)],
            depth_attachment=ctx.depth_texture((10, 10))
        )
        fbo = None
        self.assertEqual(ctx.gc(), 3)

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

        ctx.release()
