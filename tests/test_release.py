
def test_texture(ctx):
    tex = ctx.texture((4, 4), 4)
    tex.release()

def test_texture_array(ctx):
    tex = ctx.texture_array((4, 4, 4), 4)
    tex.release()

def test_texture_cube(ctx):
    tex = ctx.texture_cube((4, 4), 4)
    tex.release()

def test_texture_3d(ctx):
    tex = ctx.texture3d((4, 4, 4), 4)
    tex.release()

def test_renderbuffer(ctx):
    rbo = ctx.renderbuffer((4, 4))
    rbo.release()

def test_buffer(ctx):
    buf = ctx.buffer(reserve=4)
    buf.release()

def test_framebuffer(ctx):
    fbo = ctx.framebuffer(ctx.renderbuffer((4, 4)))
    fbo.release()

def test_vertex_array(ctx):
    vao = ctx.vertex_array(ctx.program(
        vertex_shader="""
            #version 330
            void main() {
                gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
            }
        """,
    ), [])
    vao.release()

def test_sampler(ctx):
    smp = ctx.sampler()
    smp.release()

def test_scope(ctx):
    fbo = ctx.simple_framebuffer((4, 4))
    scope = ctx.scope(framebuffer=fbo)
    fbo.release()
    scope.release()

# def test_query(ctx):
#     qry = ctx.query()
# NOTE: Query objects are not released
