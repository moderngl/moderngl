from array import array
import pytest
import struct

@pytest.fixture(scope='module', autouse=True)
def res(ctx_static):
    """Global buffer we transform results into"""
    return ctx_static.buffer(reserve=1024)

def test_float_uniform(ctx, res):
    prog = ctx.program(
        vertex_shader='''
        #version 330
        uniform float Uniform;
        in float v_in;
        out float v_out;
        void main() {
            v_out = Uniform * v_in;
        }
    ''',
        varyings=['v_out']
    )
    vbo = ctx.buffer(struct.pack('f', 1.0))
    vao = ctx.simple_vertex_array(prog, vbo, 'v_in')

    prog['Uniform'].value = 7.0
    vao.transform(res)

    val = struct.unpack('f', res.read(4))[0]
    assert pytest.approx(val) == 7.0

def test_int_uniform(ctx, res):
    prog = ctx.program(
        vertex_shader='''
            #version 330
            uniform int Uniform;
            in int v_in;
            out int v_out;
            void main() {
                v_out = Uniform * v_in;
            }
        ''',
        varyings=['v_out']
    )
    vbo = ctx.buffer(struct.pack('i', 1))
    vao = ctx.simple_vertex_array(prog, vbo, 'v_in')

    prog['Uniform'].value = -2
    vao.transform(res)

    val = struct.unpack('i', res.read(4))[0]
    assert pytest.approx(val) == -2

def test_vec_uniform(ctx, res):
    prog = ctx.program(
        vertex_shader='''
            #version 330
            uniform vec3 Uniform;
            in vec3 v_in;
            out vec3 v_out;
            void main() {
                v_out = Uniform * v_in;
            }
        ''',
        varyings=['v_out']
    )

    vbo = ctx.buffer(struct.pack('3f', 1.0, 1.0, 1.0))
    vao = ctx.simple_vertex_array(prog, vbo, 'v_in')

    prog['Uniform'].value = (0.5, 1.0, 1.5)
    vao.transform(res)

    x, y, z = struct.unpack('3f', res.read(12))
    assert pytest.approx(x) == 0.5
    assert pytest.approx(y) == 1.0
    assert pytest.approx(z) == 1.5

def test_mat_uniform(ctx, res):
    prog = ctx.program(
        vertex_shader='''
            #version 330
            uniform mat2x3 Uniform;
            in float v_in;
            out mat2x3 v_out;
            void main() {
                v_out = Uniform * v_in;
            }
        ''',
        varyings=['v_out']
    )

    vbo = ctx.buffer(struct.pack('f', 1.0))
    vao = ctx.simple_vertex_array(prog, vbo, 'v_in')

    prog['Uniform'].value = (0.0, 1.0, 2.0, 3.0, 4.0, 5.0)
    vao.transform(res)

    m = struct.unpack('6f', res.read(24))
    assert pytest.approx(m[0]) == 0.0
    assert pytest.approx(m[1]) == 1.0
    assert pytest.approx(m[2]) == 2.0
    assert pytest.approx(m[3]) == 3.0
    assert pytest.approx(m[4]) == 4.0
    assert pytest.approx(m[5]) == 5.0

def test_sampler_2d(ctx):
    """RGBA8 2d sampler"""
    prog = ctx.program(vertex_shader="""
        #version 330
        uniform sampler2D tex;
        out float color;
        void main() {
            color = texelFetch(tex, ivec2(gl_VertexID, 0), 0).r;
        }
        """,
        varyings=['color',],
    )
    tex = ctx.texture((4, 1), 1, data=array('B', [127, 0, 255, 64]), dtype='f1')
    buff = ctx.buffer(reserve=4 * 4)
    vao = ctx.vertex_array(prog, [])
    tex.use(0)
    vao.transform(buff, vertices=4)
    data = struct.unpack('4f', buff.read())
    assert pytest.approx(data[0], rel=0.001) == 0.498
    assert pytest.approx(data[1]) == 0.0
    assert pytest.approx(data[2]) == 1.0
    assert pytest.approx(data[3], rel=0.01) == 0.25

def test_sampler_2d_int(ctx):
    prog = ctx.program(vertex_shader="""
        #version 330
        uniform isampler2D tex;
        out int color;
        void main() {
            color = texelFetch(tex, ivec2(gl_VertexID, 0), 0).r;
        }
        """,
        varyings=['color',],
    )
    tex = ctx.texture((4, 1), 1, data=array('i', [-1, 0, 1000, 4353454]), dtype='i4')
    buff = ctx.buffer(reserve=4 * 4)
    vao = ctx.vertex_array(prog, [])
    tex.use(0)
    vao.transform(buff, vertices=4)
    data = struct.unpack('4i', buff.read())
    assert data[0] == -1
    assert data[1] == 0
    assert data[2] == 1000
    assert data[3] == 4353454

def test_sampler_2d_uint(ctx):
    prog = ctx.program(vertex_shader="""
        #version 330
        uniform usampler2D tex;
        out uint color;
        void main() {
            color = texelFetch(tex, ivec2(gl_VertexID, 0), 0).r;
        }
        """,
        varyings=['color',],
    )
    tex = ctx.texture((4, 1), 1, data=array('I', [0, 500, 1000, 4353454]), dtype='u4')
    buff = ctx.buffer(reserve=4 * 4)
    vao = ctx.vertex_array(prog, [])
    tex.use(0)
    vao.transform(buff, vertices=4)
    data = struct.unpack('4I', buff.read())
    assert data[0] == 0
    assert data[1] == 500
    assert data[2] == 1000
    assert data[3] == 4353454

def test_sampler_2d_array(ctx):
    """RGBA8 2d array sampler. Read two pixels from two different layers"""
    prog = ctx.program(vertex_shader="""
        #version 330
        uniform sampler2DArray tex;
        out float color;
        void main() {
            color = texelFetch(tex, ivec3(gl_VertexID % 2, 0, gl_VertexID / 2), 0).r;
        }
        """,
        varyings=['color',],
    )
    tex = ctx.texture_array((2, 1, 2), 1, data=array('B', [127, 0, 255, 64]), dtype='f1')
    buff = ctx.buffer(reserve=4 * 4)
    vao = ctx.vertex_array(prog, [])
    tex.use(0)
    vao.transform(buff, vertices=4)
    data = struct.unpack('4f', buff.read())
    assert pytest.approx(data[0], rel=0.001) == 0.498
    assert pytest.approx(data[1]) == 0.0
    assert pytest.approx(data[2]) == 1.0
    assert pytest.approx(data[3], rel=0.01) == 0.25

def test_sampler_1d(ctx):
    prog = ctx.program(
        vertex_shader="""
        #version 330
        uniform sampler1D tex;
        out vec4 color;
        void main() {
            color = texelFetch(tex, 0, 0);
        }
        """,
        varyings=["color"],
    )
    assert "tex" in prog
