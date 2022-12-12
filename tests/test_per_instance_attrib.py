import struct
import pytest
import moderngl

@pytest.fixture(scope='module')
def prog(ctx_static):
    return ctx_static.program(
        vertex_shader='''
            #version 330

            in float v_in_1;
            in float v_in_2;
            in float v_in_3;
            out float v_out;

            void main() {
                v_out = v_in_1 + v_in_2 + v_in_3;
            }
        ''',
        varyings=['v_out']
    )

@pytest.fixture(scope='module')
def vbo1(ctx_static):
    return ctx_static.buffer(struct.pack('4f', 1.0, 2.0, 3.0, 4.0))

@pytest.fixture(scope='module')
def vbo2(ctx_static):
    return ctx_static.buffer(struct.pack('4f', 10.0, 20.0, 30.0, 40.0))

@pytest.fixture(scope='module')
def vbo3(ctx_static):
    return ctx_static.buffer(struct.pack('4f', 100.0, 200.0, 300.0, 400.0))

@pytest.fixture(scope='module')
def vao1(ctx_static, prog, vbo1, vbo2):
    content = [
        (vbo1, 'f', 'v_in_1'),
        (vbo2, 'f/i', 'v_in_2'),
    ]
    return ctx_static.vertex_array(prog, content)


@pytest.fixture(scope='module')
def vao2(ctx_static, prog, vbo1, vbo2, vbo3):
    content = [
        (vbo1, 'f', 'v_in_1'),
        (vbo2, 'f/i', 'v_in_2'),
        (vbo3, 'f/r', 'v_in_3'),
    ]
    return ctx_static.vertex_array(prog, content)

@pytest.fixture(scope='module')
def res(ctx_static):
    return ctx_static.buffer(reserve=1024)


def test_vertex_attrib_per_instance_1(vao1, res):
    vao1.transform(res, moderngl.POINTS, vertices=4, instances=1)

    a, b, c, d = struct.unpack('4f', res.read(16))

    assert pytest.approx(a) == 11.0
    assert pytest.approx(b) == 12.0
    assert pytest.approx(c) == 13.0
    assert pytest.approx(d) == 14.0

def test_vertex_attrib_per_instance_2(vao1, res):
    vao1.transform(res, moderngl.POINTS, vertices=2, instances=2)

    a, b, c, d = struct.unpack('4f', res.read(16))

    assert pytest.approx(a) == 11.0
    assert pytest.approx(b) == 12.0
    assert pytest.approx(c) == 21.0
    assert pytest.approx(d) == 22.0

def test_vertex_attrib_per_instance_3(vao1, res):
    vao1.transform(res, moderngl.POINTS, vertices=1, instances=4)

    a, b, c, d = struct.unpack('4f', res.read(16))

    assert pytest.approx(a) == 11.0
    assert pytest.approx(b) == 21.0
    assert pytest.approx(c) == 31.0
    assert pytest.approx(d) == 41.0

def test_vertex_attrib_per_render_1(vao2, res):
    vao2.transform(res, moderngl.POINTS, vertices=4, instances=1)

    a, b, c, d = struct.unpack('4f', res.read(16))

    assert pytest.approx(a) == 111.0
    assert pytest.approx(b) == 112.0
    assert pytest.approx(c) == 113.0
    assert pytest.approx(d) == 114.0

def test_vertex_attrib_per_render_2(vao2, res):
    vao2.transform(res, moderngl.POINTS, vertices=2, instances=2)

    a, b, c, d = struct.unpack('4f', res.read(16))

    assert pytest.approx(a) == 111.0
    assert pytest.approx(b) == 112.0
    assert pytest.approx(c) == 121.0
    assert pytest.approx(d) == 122.0

def test_vertex_attrib_per_render_3(vao2, res):
    vao2.transform(res, moderngl.POINTS, vertices=1, instances=4)

    a, b, c, d = struct.unpack('4f', res.read(16))

    assert pytest.approx(a) == 111.0
    assert pytest.approx(b) == 121.0
    assert pytest.approx(c) == 131.0
    assert pytest.approx(d) == 141.0
