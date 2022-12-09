import struct
import unittest
from array import array
import struct
import platform

import moderngl

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

        prog = cls.ctx.program(
            vertex_shader='''
                #version 330
                in vec2 vert;
                out vec2 text;
                void main() {
                    gl_Position = vec4(vert * 2.0 - 1.0, 0.0, 1.0);
                    text = vert;
                }
            ''',
            fragment_shader='''
                #version 330
                uniform sampler2D Texture;
                in vec2 text;
                out vec4 color;
                void main() {
                    color = texture(Texture, text);
                }
            ''',
        )

        vbo = cls.ctx.buffer(struct.pack('8f', 0, 0, 0, 1, 1, 0, 1, 1))
        cls.vao = cls.ctx.simple_vertex_array(prog, vbo, 'vert')

    def test_1(self):
        self.assertEqual(self.vao.mode, moderngl.TRIANGLES)
        fbo = self.ctx.framebuffer(self.ctx.renderbuffer((16, 16)))
        pixels = struct.pack('16B', 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 0, 0, 0, 255)
        texture = self.ctx.texture((2, 2), 4, pixels)

        fbo.use()
        texture.use()

        texture.filter = (moderngl.LINEAR, moderngl.LINEAR)
        self.vao.render(moderngl.TRIANGLE_STRIP)

        r, g, b = struct.unpack('3B', fbo.read((8, 8, 1, 1)))
        self.assertTrue(abs(r - 49) < 8 and abs(g - 63) < 8 and abs(b - 63) < 8)

        texture.filter = (moderngl.NEAREST, moderngl.NEAREST)
        self.vao.render(moderngl.TRIANGLE_STRIP)

        r, g, b = struct.unpack('3B', fbo.read((6, 6, 1, 1)))
        self.assertTrue(abs(r - 255) < 8 and abs(g - 0) < 8 and abs(b - 0) < 8)

        r, g, b = struct.unpack('3B', fbo.read((9, 6, 1, 1)))
        self.assertTrue(abs(r - 0) < 8 and abs(g - 255) < 8 and abs(b - 0) < 8)

        r, g, b = struct.unpack('3B', fbo.read((6, 9, 1, 1)))
        self.assertTrue(abs(r - 0) < 8 and abs(g - 0) < 8 and abs(b - 255) < 8)

        r, g, b = struct.unpack('3B', fbo.read((9, 9, 1, 1)))
        self.assertTrue(abs(r - 0) < 8 and abs(g - 0) < 8 and abs(b - 0) < 8)

    def test_2(self):
        fbo = self.ctx.framebuffer(self.ctx.renderbuffer((16, 16)))
        pixels1 = struct.pack('16B', 255, 0, 0, 255, 0, 255, 0, 255, 255, 0, 0, 255, 0, 255, 0, 255)
        pixels2 = struct.pack('16B', 20, 30, 40, 255, 80, 70, 60, 255, 20, 30, 40, 255, 80, 70, 60, 255)
        texture = self.ctx.texture((2, 2), 4, pixels1)

        fbo.use()
        texture.use()

        texture.filter = (moderngl.NEAREST, moderngl.NEAREST)
        self.vao.render(moderngl.TRIANGLE_STRIP)

        r, g, b = struct.unpack('3B', fbo.read((5, 7, 1, 1)))
        self.assertTrue(abs(r - 255) < 8 and abs(g - 0) < 8 and abs(b - 0) < 8)

        r, g, b = struct.unpack('3B', fbo.read((10, 7, 1, 1)))
        self.assertTrue(abs(r - 0) < 8 and abs(g - 255) < 8 and abs(b - 0) < 8)

        texture.write(pixels2)

        texture.filter = (moderngl.NEAREST, moderngl.NEAREST)
        self.vao.render(moderngl.TRIANGLE_STRIP)

        r, g, b = struct.unpack('3B', fbo.read((5, 7, 1, 1)))
        self.assertTrue(abs(r - 20) < 8 and abs(g - 30) < 8 and abs(b - 40) < 8)

        r, g, b = struct.unpack('3B', fbo.read((10, 7, 1, 1)))
        self.assertTrue(abs(r - 80) < 8 and abs(g - 70) < 8 and abs(b - 60) < 8)

    def test_3(self):
        fbo = self.ctx.framebuffer(self.ctx.renderbuffer((16, 16)))
        pixels1 = struct.pack('16B', 255, 0, 0, 255, 0, 255, 0, 255, 255, 0, 0, 255, 0, 255, 0, 255)
        pixels2 = struct.pack('16B', 20, 30, 40, 255, 80, 70, 60, 255, 20, 30, 40, 255, 80, 70, 60, 255)
        texture = self.ctx.texture((2, 2), 4)

        pbo1 = self.ctx.buffer(pixels1)
        pbo2 = self.ctx.buffer(pixels2)

        fbo.use()
        texture.use()

        texture.write(pbo1)
        texture.filter = (moderngl.NEAREST, moderngl.NEAREST)
        self.vao.render(moderngl.TRIANGLE_STRIP)

        r, g, b = struct.unpack('3B', fbo.read((5, 7, 1, 1)))
        self.assertTrue(abs(r - 255) < 8 and abs(g - 0) < 8 and abs(b - 0) < 8)

        r, g, b = struct.unpack('3B', fbo.read((10, 7, 1, 1)))
        self.assertTrue(abs(r - 0) < 8 and abs(g - 255) < 8 and abs(b - 0) < 8)

        texture.write(pbo2)
        texture.filter = (moderngl.NEAREST, moderngl.NEAREST)
        self.vao.render(moderngl.TRIANGLE_STRIP)

        r, g, b = struct.unpack('3B', fbo.read((5, 7, 1, 1)))
        self.assertTrue(abs(r - 20) < 8 and abs(g - 30) < 8 and abs(b - 40) < 8)

        r, g, b = struct.unpack('3B', fbo.read((10, 7, 1, 1)))
        self.assertTrue(abs(r - 80) < 8 and abs(g - 70) < 8 and abs(b - 60) < 8)

    def test_4(self):
        pixels = struct.pack('16B', 255, 0, 0, 255, 0, 255, 0, 255, 255, 0, 0, 255, 0, 255, 0, 255)
        texture = self.ctx.texture((2, 2), 4, pixels)
        self.assertEqual(texture.read(), pixels)

    def test_5(self):
        pixels = struct.pack('16B', 255, 0, 0, 255, 0, 255, 0, 255, 255, 0, 0, 255, 0, 255, 0, 255)
        texture = self.ctx.texture((2, 2), 4, pixels)
        buf = bytearray(len(pixels))
        texture.read_into(buf)

        self.assertEqual(bytes(buf), pixels)

    def test_6(self):
        pixels = struct.pack('16B', 255, 0, 0, 255, 0, 255, 0, 255, 255, 0, 0, 255, 0, 255, 0, 255)
        texture = self.ctx.texture((2, 2), 4, pixels)
        pbo = self.ctx.buffer(reserve=len(pixels))
        texture.read_into(pbo)

        self.assertEqual(pbo.read(), pixels)

    def test_override_internalformat(self):
        """Ensure no errors occur when overriding internalformat"""
        self.ctx.error
        if not "GL_EXT_texture_sRGB" in self.ctx.extensions:
            self.skipTest('GL_EXT_texture_sRGB extension not supported')

        GL_SRGB8 = 0x8C41
        pixels = struct.pack('16B', 255, 0, 0, 255, 0, 255, 0, 255, 255, 0, 0, 255, 0, 255, 0, 255)
        texture = self.ctx.texture((2, 2), 4, pixels, internal_format=GL_SRGB8)
        data = texture.read()
        self.assertEqual(self.ctx.error, "GL_NO_ERROR")

    def test_normalized_textures(self):
        """8 and 16 bit normalized integer textures"""
        ni1 = self.ctx.texture((4, 4), 4, dtype="ni1")
        ni2 = self.ctx.texture((4, 4), 4, dtype="ni2")
        nu1 = self.ctx.texture((4, 4), 4, dtype="nu1")
        nu2 = self.ctx.texture((4, 4), 4, dtype="nu2")

        nu1_data = array('B', [255] * 16 * 4).tobytes()
        nu2_data = array('H', [65535] * 16 * 4).tobytes()
        ni1_data = array('b', [127] * 16 * 4).tobytes()
        ni2_data = array('h', [32767] * 16 * 4).tobytes()

        nu1.write(nu1_data)
        nu2.write(nu2_data)
        ni1.write(ni1_data)
        ni2.write(ni2_data)

        self.assertEqual(nu1.read(), nu1_data)
        self.assertEqual(nu2.read(), nu2_data)
        self.assertEqual(ni1.read(), ni1_data)
        self.assertEqual(ni2.read(), ni2_data)

        fbo = self.ctx.simple_framebuffer((4, 4))
        fbo.use()

        # Render these textures to an RGBA8 framebuffer and ensure the result is a pure white color (FF)
        fbo.clear()
        nu1.use()
        self.vao.render()
        self.assertEqual(fbo.read(viewport=(0, 0, 1, 1), components=4, dtype="f1"), b'\xff\xff\xff\xff')

        fbo.clear()
        nu2.use()
        self.vao.render()
        self.assertEqual(fbo.read(viewport=(0, 0, 1, 1), components=4, dtype="f1"), b'\xff\xff\xff\xff')

        fbo.clear()
        ni1.use()
        self.vao.render()
        self.assertEqual(fbo.read(viewport=(0, 0, 1, 1), components=4, dtype="f1"), b'\xff\xff\xff\xff')

        fbo.clear()
        ni2.use()
        self.vao.render()
        self.assertEqual(fbo.read(viewport=(0, 0, 1, 1), components=4, dtype="f1"), b'\xff\xff\xff\xff')

    def test_depth_texture_write(self):
        """Write data into depth texture"""
        size = 4, 4
        fbo = self.ctx.framebuffer(
            color_attachments=[self.ctx.texture(size, 4)],
            depth_attachment=self.ctx.depth_texture(size),
        )
        fbo.clear(depth=1.0)

        first = struct.unpack("16f", fbo.depth_attachment.read())
        second = (1.0,) * 16
        for a, b in zip(first, second):
            self.assertAlmostEqual(a, b, places=6)

        fbo.depth_attachment.write(struct.pack("16f", *([0.5] * 16)))
        first = struct.unpack("16f", fbo.depth_attachment.read())
        second = (0.5,) * 16
        for a, b in zip(first, second):
            self.assertAlmostEqual(a, b, places=5)


if __name__ == '__main__':
    unittest.main()
