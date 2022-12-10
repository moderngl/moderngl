import random
import math
import struct
import unittest

import moderngl
import numpy as np

from common import get_context


class TestBlend(unittest.TestCase):
    """
    Test blending and blend equations with float32 framebuffer.
    Emulate blend and equation values in python and compare with moderngl.
    """
    @classmethod
    def setUpClass(cls):
        """Set up program, vbo and fbo"""
        cls.ctx = get_context()
        cls.prog = cls.ctx.program(
            vertex_shader='''
                #version 330

                in vec2 in_vert;

                void main() {
                    gl_Position = vec4(in_vert, 0.0, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330

                out vec4 fragColor;
                uniform vec4 color;

                void main() {
                    fragColor = color;
                }
            ''',
        )
        quad = [
            -1.0,  1.0,
            -1.0, -1.0,
            1.0, 1.0,
            1.0, -1.0,
        ]
        vbo = cls.ctx.buffer(np.array(quad, dtype='f4').tobytes())
        cls.vao = cls.ctx.simple_vertex_array(cls.prog, vbo, 'in_vert')
        cls.fbo = cls.ctx.framebuffer(color_attachments=[
            cls.ctx.texture((1, 1), 4, dtype='f4'),
        ])

    @classmethod
    def tearDownClass(cls):
        """Set everything back to default"""
        cls.ctx.disable(moderngl.BLEND)
        cls.ctx.blend_equation = moderngl.FUNC_ADD, moderngl.FUNC_ADD
        cls.ctx.blend_func = moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA

    def unpack_fbo(self):
        return struct.unpack('4f', self.fbo.read(components=4, dtype='f4')[:16])

    def err(self):
        e = self.ctx.error
        if e != 'GL_NO_ERROR':
            raise ValueError(e)

    def cf(self, blend_func, val, src, dst, src_a, dst_a):
        """Calculate factor

        Args:
            blend_func: The blend func to use
            src: source value (single r, g, b, or a)
            dest: dest value (single r, g, b, or a)
            src_a: alpha value
            dst_a: alpha value
        """
        result = None
        if blend_func == moderngl.ZERO:
            result = 0
        if blend_func == moderngl.ONE:
            result = val
        if blend_func == moderngl.SRC_COLOR:
            result = val * src
        if blend_func == moderngl.ONE_MINUS_SRC_COLOR:
            result = val * (1.0 - src)
        if blend_func == moderngl.DST_COLOR:
            result = val * dst
        if blend_func == moderngl.ONE_MINUS_DST_COLOR:
            result = val * (1.0 - dst)
        if blend_func == moderngl.SRC_ALPHA:
            result = val * src_a
        if blend_func == moderngl.ONE_MINUS_SRC_ALPHA:
            result = val * (1.0 - src_a)
        if blend_func == moderngl.DST_ALPHA:
            result = val * dst_a
        if blend_func == moderngl.ONE_MINUS_DST_ALPHA:
            result = val * (1.0 - dst_a)

        if result is None:
            raise ValueError("Unsupported blend_func")

        # Don't clamp here. It needs to happen in the combine
        return result

    def bleq(self, bsx, bdx, sx, dx, func):
        """Blend equation. Note that MIN and MAX have special behavior.

        Args:
            bsx: blended source value
            bdx: blended dest value
            sx: original value from source
            dx: original value from dest
        """
        if func == moderngl.FUNC_ADD:
            return bsx + bdx
        elif func == moderngl.FUNC_SUBTRACT:
            return bsx - bdx
        elif func == moderngl.FUNC_REVERSE_SUBTRACT:
            return bdx - bsx
        elif func == moderngl.MIN:
            return min(sx, dx)
        elif func == moderngl.MAX:
            return max(sx, dx)
        else:
            raise ValueError('Unsupported blend equation: {}'.format(func))

    def blend_emulate(self, src, dst, blend_func, blend_equation):
        """Emulates blending in python"""
        sr = self.cf(blend_func[0], src[0], src[0], dst[0], src[3], dst[3])
        sg = self.cf(blend_func[0], src[1], src[1], dst[1], src[3], dst[3])
        sb = self.cf(blend_func[0], src[2], src[2], dst[2], src[3], dst[3])
        sa = self.cf(blend_func[2], src[3], src[3], dst[3], src[3], dst[3])

        dr = self.cf(blend_func[1], dst[0], src[0], dst[0], src[3], dst[3])
        dg = self.cf(blend_func[1], dst[1], src[1], dst[1], src[3], dst[3])
        db = self.cf(blend_func[1], dst[2], src[2], dst[2], src[3], dst[3])
        da = self.cf(blend_func[3], dst[3], src[3], dst[3], src[3], dst[3])

        return (
            self.bleq(sr, dr, src[0], dst[0], blend_equation[0]),
            self.bleq(sg, dg, src[1], dst[1], blend_equation[0]),
            self.bleq(sb, db, src[2], dst[2], blend_equation[0]),
            self.bleq(sa, da, src[3], dst[3], blend_equation[1]),
        )

    def blend_moderngl(self, src, dst, blend_func, blend_equation):
        """Performs blending in moderngl.

        Args:
            src (tuple): The source fragment (r, g, b, a)
            dst (tuple): The destination fragment (r, g, b, a)
            blend_func (tuple): The blend func
            blend_func (tuple): The blend equation
        Returns:
            The result fragment
        """
        self.fbo.use()
        self.fbo.clear(red=dst[0], green=dst[1], blue=dst[2], alpha=dst[3])
        self.ctx.enable(moderngl.BLEND)

        self.prog['color'].value = src
        self.ctx.blend_func = blend_func
        self.ctx.blend_equation = blend_equation
        self.vao.render(mode=moderngl.TRIANGLE_STRIP)
        return self.unpack_fbo()

    def assertEqualColors(self, a, b):
        self.assertAlmostEqual(a[0], b[0], places=2)
        self.assertAlmostEqual(a[1], b[1], places=2)
        self.assertAlmostEqual(a[2], b[2], places=2)
        self.assertAlmostEqual(a[3], b[3], places=2)

    def test_blend_default(self):
        src = (0.0, 1.0, 0.0, 0.2)
        dst = (1.0, 0.0, 1.0, 0.3)
        blend_func = (
            moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA,
            moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA,
        )
        blend_equation = moderngl.FUNC_ADD, moderngl.FUNC_ADD

        a = self.blend_emulate(src, dst, blend_func, blend_equation)
        b = self.blend_moderngl(src, dst, blend_func, blend_equation)
        self.assertEqualColors(a, b)

    def test_blend_separate(self):
        src = (0.2, 0.6, 0.1, 0.3)
        dst = (0.3, 0.1, 0.4, 0.2)
        blend_func = (
            moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA,
            moderngl.ONE, moderngl.ONE,
        )
        blend_equation = moderngl.FUNC_ADD, moderngl.FUNC_ADD

        a = self.blend_emulate(src, dst, blend_func, blend_equation)
        b = self.blend_moderngl(src, dst, blend_func, blend_equation)
        self.assertEqualColors(a, b)

    def test_blend_separate_equation(self):
        src = (0.2, 0.6, 0.1, 0.3)
        dst = (0.3, 0.1, 0.4, 0.2)
        blend_func = (
            moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA,
            moderngl.ONE, moderngl.ONE,
        )
        blend_equation = moderngl.FUNC_REVERSE_SUBTRACT, moderngl.MAX

        a = self.blend_emulate(src, dst, blend_func, blend_equation)
        b = self.blend_moderngl(src, dst, blend_func, blend_equation)
        self.assertEqualColors(a, b)

    def test_blend_chaos(self):
        """Pick N random colors with random blend modes and equations"""
        bf = [
            moderngl.ZERO, moderngl.ONE, moderngl.SRC_COLOR, moderngl.ONE_MINUS_SRC_COLOR,
            moderngl.DST_COLOR, moderngl.ONE_MINUS_DST_COLOR, moderngl.SRC_ALPHA,
            moderngl.ONE_MINUS_SRC_ALPHA, moderngl.DST_ALPHA, moderngl.ONE_MINUS_DST_ALPHA,
        ]
        eq = [
            moderngl.FUNC_ADD, moderngl.FUNC_SUBTRACT, moderngl.FUNC_REVERSE_SUBTRACT,
            moderngl.MIN, moderngl.MAX,
        ]

        for _ in range(1000):
            src = tuple([random.uniform(0, 1) for i in range(4)])
            dst = tuple([random.uniform(0, 1) for i in range(4)])
            blend_func = tuple([random.choice(bf) for i in range(4)])
            blend_equation = random.choice(eq), random.choice(eq)
            a = self.blend_emulate(src, dst, blend_func, blend_equation)
            b = self.blend_moderngl(src, dst, blend_func, blend_equation)
            self.assertEqualColors(a, b)

    def test_invalid_blend_func(self):
        # TypeError: Not iterable
        with self.assertRaises(TypeError):
            self.ctx.blend_func = moderngl.ONE

        # Incorrect tuple size
        with self.assertRaises(moderngl.error.Error):
            self.ctx.blend_func = moderngl.ONE,

        # TypeError: "Test" is not an integer
        with self.assertRaises(TypeError):
            self.ctx.blend_func = moderngl.ONE, "Test"

        # Incorrect tuple size=3
        with self.assertRaises(moderngl.error.Error):
            self.ctx.blend_func = moderngl.ONE, moderngl.ONE, moderngl.ONE

        # Incorrect tuple size=5
        with self.assertRaises(moderngl.error.Error):
            self.ctx.blend_func = (
                moderngl.ONE,
                moderngl.ONE,
                moderngl.ONE,
                moderngl.ONE,
                moderngl.ONE,
            )

        # Working versions
        self.ctx.blend_func = moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA
        self.ctx.blend_func = (
            moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA,
            moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA,
        )

    def test_invalid_blend_equation(self):
        with self.assertRaises(TypeError):
            self.ctx.blend_equation = None

        with self.assertRaises(moderngl.error.Error):
            self.ctx.blend_equation = moderngl.MAX, moderngl.MAX, moderngl.MAX

        self.ctx.blend_equation = moderngl.MAX
        self.ctx.blend_equation = moderngl.MAX, moderngl.MAX

    def test_get_values(self):
        with self.assertRaises(NotImplementedError):
            self.ctx.blend_func

        with self.assertRaises(NotImplementedError):
            self.ctx.blend_equation
