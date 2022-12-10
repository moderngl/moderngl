import unittest

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_texture(self):
        tex = self.ctx.texture((4, 4), 4)
        tex.release()

    def test_texture_array(self):
        tex = self.ctx.texture_array((4, 4, 4), 4)
        tex.release()

    def test_texture_cube(self):
        tex = self.ctx.texture_cube((4, 4), 4)
        tex.release()

    def test_texture_3d(self):
        tex = self.ctx.texture3d((4, 4, 4), 4)
        tex.release()

    def test_renderbuffer(self):
        rbo = self.ctx.renderbuffer((4, 4))
        rbo.release()

    def test_buffer(self):
        buf = self.ctx.buffer(reserve=4)
        buf.release()

    def test_framebuffer(self):
        fbo = self.ctx.framebuffer(self.ctx.renderbuffer((4, 4)))
        fbo.release()

    def test_vertex_array(self):
        vao = self.ctx.vertex_array(self.ctx.program(
            vertex_shader="""
                #version 330
                void main() {
                    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
                }
            """,
        ), [])
        vao.release()

    def test_sampler(self):
        smp = self.ctx.sampler()
        smp.release()

    def test_scope(self):
        fbo = self.ctx.simple_framebuffer((4, 4))
        scope = self.ctx.scope(framebuffer=fbo)
        fbo.release()
        scope.release()

    # def test_query(self):
    #     qry = self.ctx.query()
    # NOTE: Query objects are not released
