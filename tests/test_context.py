import ctypes
import struct
import sys

import moderngl
import pytest


FULLSCREEN_VERT = '''
    #version 330 core
    vec2 positions[3] = vec2[](vec2(-1.0, -1.0), vec2(3.0, -1.0), vec2(-1.0, 3.0));
    void main() {
        gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
    }
'''

COLOR_FRAG = '''
    #version 330 core
    uniform vec4 color;
    layout (location = 0) out vec4 out_color;
    void main() {
        out_color = color;
    }
'''


CACHE = {}


def empty(ctx):
    if 'empty' not in CACHE:
        CACHE['empty'] = ctx.vertex_array([])
    return CACHE['empty']


@pytest.fixture(scope='session')
def ctx():
    headless = moderngl.headless_context()
    ctx = moderngl.get_context()
    ctx.shaders['fullscreen.vert'] = FULLSCREEN_VERT
    ctx.shaders['color.frag'] = COLOR_FRAG
    yield ctx
    headless.release()


def test_program(ctx):
    program = ctx.program('fullscreen.vert', 'color.frag', uniforms={'color': 'vec4', 'unused': 'float'})
    assert isinstance(program, moderngl.Program)
    assert program.uniforms['color'] >= 0
    assert program.uniforms['unused'] == -1
    with pytest.raises(RuntimeError, match='program.use'):
        program['color'] = (1.0, 0.0, 0.0, 1.0)
    program.use()
    program['color'] = (1.0, 0.0, 0.0, 1.0)
    program['unused'] = 1.0
    with pytest.raises(KeyError):
        program['missing'] = 1.0
    with pytest.raises(TypeError):
        program['color']


def test_render(ctx):
    program = ctx.program('fullscreen.vert', 'color.frag', uniforms={'color': 'vec4'})
    color = ctx.texture((4, 4), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    fbo.clear()
    fbo.use()
    program.use()
    program['color'] = (1.0, 0.0, 0.0, 1.0)
    empty(ctx).render(vertices=3)
    assert color.read() == b'\xff\x00\x00\xff' * 16
    program['color'] = (0.0, 0.0, 1.0, 1.0)
    fbo.use()
    program.use()
    empty(ctx).render(vertices=3)
    assert color.read() == b'\x00\x00\xff\xff' * 16
    fbo.clear()
    fbo.viewport = (0, 0, 2, 2)
    fbo.use()
    program.use()
    empty(ctx).render(vertices=3)
    assert color.read(size=(2, 2)) == b'\x00\x00\xff\xff' * 4
    assert color.read(size=(2, 2), offset=(2, 2)) == bytes(16)


def test_render_vertex_array(ctx):
    ctx.shaders['vertex_array.vert'] = '''
            #version 330 core
            layout (location = 0) in vec2 in_vertex;
            layout (location = 1) in vec4 in_color;
            layout (location = 2) in vec2 in_offset;
            out vec4 v_color;
            void main() {
                gl_Position = vec4(in_vertex + in_offset, 0.0, 1.0);
                v_color = in_color;
            }
    '''
    ctx.shaders['vertex_array.frag'] = '''
            #version 330 core
            in vec4 v_color;
            layout (location = 0) out vec4 out_color;
            void main() {
                out_color = v_color;
            }
    '''
    program = ctx.program('vertex_array.vert', 'vertex_array.frag')
    vertices = ctx.buffer(struct.pack('8f16B', 0, 0, 1, 0, 0, 1, 1, 1, *[255, 0, 255, 255] * 4))
    offsets = ctx.buffer(struct.pack('4f', -1.0, -1.0, -1.0, 0.0))
    index = ctx.buffer(struct.pack('6H', 0, 1, 2, 2, 1, 3), kind='index_buffer')
    attributes = [*vertices.bind('2f', 0), *vertices.bind('4nu1', 1, offset=32), *offsets.bind('2f /i', 2)]
    vao = ctx.vertex_array(attributes, index_buffer=index, index_size=2)
    color = ctx.texture((4, 4), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    fbo.clear()
    assert vao.vertices == 6
    fbo.use()
    program.use()
    vao.render(instances=2)
    pixels = color.read()
    assert pixels[:8] == b'\xff\x00\xff\xff' * 2
    assert pixels[8:16] == bytes(8)
    assert pixels[32:40] == b'\xff\x00\xff\xff' * 2
    fbo.clear()
    fbo.use()
    program.use()
    vao.render(vertices=3, first=3)
    assert color.read(size=(1, 1), offset=(1, 1)) == b'\xff\x00\xff\xff'


def test_program_uniform_types(ctx):
    ctx.shaders['types.frag'] = '''
            #version 330 core
            uniform float scale;
            uniform int count;
            uniform uint mask;
            uniform bool flag;
            uniform mat2 rotation;
            uniform float seeds[4];
            layout (location = 0) out vec4 out_color;
            void main() {
                float value = scale + float(count) + float(mask) + rotation[0][0] + seeds[0] + seeds[1] + seeds[2] + seeds[3];
                out_color = flag ? vec4(value / 255.0) : vec4(0.0);
            }
    '''
    uniforms = {'scale': 'float', 'count': 'int', 'mask': 'uint', 'flag': 'bool', 'rotation': 'mat2', 'seeds': 'float[4]'}
    program = ctx.program('fullscreen.vert', 'types.frag', uniforms=uniforms)
    color = ctx.texture((1, 1), 'r8unorm')
    fbo = ctx.framebuffer([color])
    fbo.use()
    program.use()
    program['scale'] = 2
    program['count'] = struct.pack('i', 3)
    program['mask'] = 4
    program['flag'] = True
    program['rotation'] = ((5.0, 0.0), (0.0, 1.0))
    program['seeds'] = [10.0, 20.0, 30.0, 40.0]
    empty(ctx).render(vertices=3)
    assert color.read()[0] == 114
    with pytest.raises(TypeError):
        program['count'] = 1.5
    with pytest.raises(ValueError):
        program['seeds'] = [1.0, 2.0]
    with pytest.raises(ValueError):
        program['scale'] = b'abc'
    with pytest.raises(TypeError):
        program['scale'] = 'x'


def test_program_uniform_declarations(ctx):
    ctx.shaders['seeds.frag'] = '''
            #version 330 core
            uniform float seeds[8];
            layout (location = 0) out vec4 out_color;
            void main() {
                out_color = vec4(seeds[0] + seeds[1]);
            }
    '''
    program = ctx.program('fullscreen.vert', 'seeds.frag', uniforms={'seeds': 'float[8]'})
    program.use()
    program['seeds'] = [1.0] * 8
    ctx.program('fullscreen.vert', 'seeds.frag', uniforms={'seeds': 'float[2]'})
    with pytest.raises(ValueError, match='different type'):
        ctx.program('fullscreen.vert', 'color.frag', uniforms={'color': 'vec3'})
    with pytest.raises(ValueError, match='seeds'):
        ctx.program('fullscreen.vert', 'seeds.frag', uniforms={'seeds': 'float'})
    for bad in ['vec5', 'float[0]', 'float[', 'float[2]x', 3]:
        with pytest.raises(ValueError, match='invalid uniform type'):
            ctx.program('fullscreen.vert', 'color.frag', uniforms={'color': bad})


def test_debug_uniforms(ctx):
    ctx.shaders['read.frag'] = '''
            #version 330 core
            uniform int count;
            uniform bvec2 flags;
            uniform mat2x3 shear;
            uniform float seeds[4];
            layout (location = 0) out vec4 out_color;
            void main() {
                out_color = vec4(float(count) + shear[1][2] + seeds[1], flags.y ? 1.0 : 0.0, 0.0, 1.0);
            }
    '''
    uniforms = {'count': 'int', 'flags': 'bvec2', 'shear': 'mat2x3', 'seeds': 'float[4]', 'unused': 'vec2', 'more': 'float[2]'}
    program = ctx.program('fullscreen.vert', 'read.frag', uniforms=uniforms)
    program.use()
    program['count'] = -3
    program['flags'] = (False, True)
    program['shear'] = ((1.0, 2.0, 3.0), (4.0, 5.0, 6.0))
    program['seeds'] = [0.0, 1.0, 0.0, 0.0]
    values = program.debug()['uniforms']
    assert values['count'] == -3
    assert values['flags'] == (False, True)
    assert values['shear'] == ((1.0, 2.0, 3.0), (4.0, 5.0, 6.0))
    assert values['seeds'][:2] == [0.0, 1.0]
    assert len(values['seeds']) == 4
    assert values['unused'] is None
    assert values['more'] == [None, None]
    program['shear'] = values['shear']
    with pytest.raises(TypeError):
        program['unused'] = None


def test_program_layout(ctx):
    ctx.shaders['layout.vert'] = '''
        #version 330 core
        layout (std140) uniform Common {
            vec4 tint;
        };
        void main() {
            gl_Position = tint;
        }
    '''
    ctx.shaders['layout.frag'] = '''
        #version 330 core
        uniform sampler2D Texture;
        layout (location = 0) out vec4 out_color;
        void main() {
            out_color = texture(Texture, vec2(0.0));
        }
    '''
    ctx.program('layout.vert', 'layout.frag', samplers={'Texture': 0}, uniform_buffers={'Common': 0})
    ctx.program('layout.vert', 'layout.frag', samplers={'Texture': 15, 'Unused': 3}, uniform_buffers={'Common': 7})
    with pytest.raises(ValueError, match='Texture'):
        ctx.program('layout.vert', 'layout.frag', uniform_buffers={'Common': 0})
    with pytest.raises(ValueError, match='Common'):
        ctx.program('layout.vert', 'layout.frag', samplers={'Texture': 0})
    with pytest.raises(ValueError, match='Texture'):
        ctx.program('layout.vert', 'layout.frag', samplers={'Texture': 16}, uniform_buffers={'Common': 0})
    with pytest.raises(ValueError, match='Texture'):
        ctx.program('layout.vert', 'layout.frag', samplers={'Texture': -1}, uniform_buffers={'Common': 0})
    with pytest.raises(ValueError, match='Common'):
        ctx.program('layout.vert', 'layout.frag', samplers={'Texture': 0}, uniform_buffers={'Common': 8})
    with pytest.raises(ValueError, match='Texture'):
        ctx.program('layout.vert', 'layout.frag', uniform_buffers={'Common': 0, 'Texture': 0})
    with pytest.raises(ValueError, match='Common'):
        ctx.program('layout.vert', 'layout.frag', samplers={'Texture': 0, 'Common': 0})
    with pytest.raises(ValueError, match='Texture'):
        ctx.program('layout.vert', 'layout.frag', uniform_buffers={'Common': 0}, uniforms={'Texture': 'int'})
    with pytest.raises(ValueError, match='Common'):
        ctx.program('layout.vert', 'layout.frag', samplers={'Texture': 0}, uniforms={'Common': 'vec4'})
    program = ctx.program('layout.vert', 'layout.frag', samplers={'Texture': 15, 'Unused': 3}, uniform_buffers={'Common': 7, 'Missing': 1})
    assert program.debug() == {
        'uniforms': {},
        'samplers': {'Texture': 15, 'Unused': None},
        'uniform_buffers': {'Common': 7, 'Missing': None},
    }
    ctx.shaders['sampler_array.frag'] = '''
        #version 330 core
        uniform sampler2D Textures[2];
        layout (location = 0) out vec4 out_color;
        void main() {
            out_color = texture(Textures[0], vec2(0.0)) + texture(Textures[1], vec2(0.0));
        }
    '''
    with pytest.raises(ValueError, match=r'Textures\[1\]'):
        ctx.program('fullscreen.vert', 'sampler_array.frag', samplers={'Textures[0]': 0})
    with pytest.raises(ValueError, match=r'Textures\[0\]'):
        ctx.program('fullscreen.vert', 'sampler_array.frag', samplers={'Textures': 0})
    program = ctx.program('fullscreen.vert', 'sampler_array.frag', samplers={'Textures[0]': 2, 'Textures[1]': 5, 'Textures[2]': 6})
    assert program.debug()['samplers'] == {'Textures[0]': 2, 'Textures[1]': 5, 'Textures[2]': None}
    red = ctx.texture((1, 1), 'rgba8unorm', bytes([255, 0, 0, 255]))
    blue = ctx.texture((1, 1), 'rgba8unorm', bytes([0, 0, 255, 255]))
    color = ctx.texture((1, 1), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    ctx.sampler(red).use(2)
    ctx.sampler(blue).use(5)
    fbo.use()
    program.use()
    empty(ctx).render(vertices=3)
    assert color.read() == bytes([255, 0, 255, 255])


def test_program_errors(ctx):
    ctx.shaders['broken.glsl'] = '#version 330 core\nvoid main() { x; }'
    with pytest.raises(ValueError, match='Vertex Shader Error in "broken.glsl"'):
        ctx.program('broken.glsl', 'color.frag')
    with pytest.raises(ValueError, match='Fragment Shader Error in "broken.glsl"'):
        ctx.program('fullscreen.vert', 'broken.glsl')
    with pytest.raises(KeyError, match='missing.frag'):
        ctx.program('fullscreen.vert', 'missing.frag')
    program = ctx.program('fullscreen.vert', 'color.frag')
    fbo = ctx.framebuffer([ctx.texture((4, 4))])
    fbo.use()
    program.use()
    with pytest.raises(ValueError):
        empty(ctx).render(vertices=3, mode='quads')
    with pytest.raises(TypeError):
        empty(ctx).render(program)
    with pytest.raises(ValueError):
        empty(ctx).render(vertices=-1)


def test_gc(ctx):
    buffer = ctx.buffer(size=16)
    texture = ctx.texture((1, 1))
    assert buffer in ctx.gc() and texture in ctx.gc()
    ctx.release(buffer)
    assert buffer not in ctx.gc() and texture in ctx.gc()
    ctx.release(texture)


def test_external_buffer(ctx):
    buffer = ctx.buffer(b'abcdefgh')
    external = ctx.external_buffer(buffer.glo)
    assert external.size == 8 and external.kind == 'vertex_buffer'
    assert external not in ctx.gc()
    external.write(b'xy', offset=2)
    assert buffer.read() == b'abxyefgh'
    assert ctx.external_buffer(buffer.glo, 4, kind='copy_buffer').read() == b'abxy'
    ctx.release(external)
    ctx.end_frame()
    assert GL.glIsBuffer(buffer.glo)
    with pytest.raises(ValueError):
        ctx.external_buffer(buffer.glo, kind='storage')
    ctx.release(buffer)


def test_external_texture(ctx):
    texture = ctx.texture((2, 2), 'rgba8unorm', bytes(range(16)))
    external = ctx.external_texture(texture.glo, (2, 2), 'rgba8unorm')
    assert external.size == (2, 2) and external not in ctx.gc()
    assert external.read() == bytes(range(16))
    copy = ctx.texture((2, 2), 'rgba8unorm')
    ctx.copy_texture(external, copy)
    assert copy.read() == bytes(range(16))
    fbo = ctx.framebuffer([external])
    fbo.clear_color = (1.0, 0.0, 0.0, 1.0)
    fbo.clear()
    assert texture.read() == b'\xff\x00\x00\xff' * 4
    ctx.release(external)
    assert GL.glIsTexture(texture.glo)
    with pytest.raises(TypeError):
        ctx.external_texture(texture.glo, (2, 2), samples=4)
    with pytest.raises(ValueError):
        ctx.external_texture(texture.glo, (2, 2), 'rgb9')
    ctx.release(fbo)
    ctx.release(copy)
    ctx.release(texture)


def test_camera():
    def project(matrix, point):
        m = struct.unpack('16f', matrix)
        return [sum(m[col * 4 + row] * (list(point) + [1.0])[col] for col in range(4)) for row in range(4)]

    x, y, z, w = project(moderngl.camera((3.0, 2.0, 1.0), fov=60.0, aspect=1.5), (0.0, 0.0, 0.0))
    assert abs(x) < 1e-6 and abs(y) < 1e-6 and abs(w - 14.0 ** 0.5) < 1e-5
    x, y, z, w = project(moderngl.camera((0.0, 0.0, 5.0), up=(0.0, 1.0, 0.0), fov=0.0, size=2.0), (1.0, 2.0, 0.0))
    assert abs(x - 0.5) < 1e-6 and abs(y - 1.0) < 1e-6 and w == 1.0


def test_shader_cache(ctx):
    first = ctx.program('fullscreen.vert', 'color.frag', uniforms={'color': 'vec4'})
    ctx.release(first)
    second = ctx.program('fullscreen.vert', 'color.frag', uniforms={'color': 'vec4'})
    color = ctx.texture((1, 1), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    fbo.use()
    second.use()
    second['color'] = (0.0, 1.0, 0.0, 1.0)
    ctx.empty_vertex_array.render(vertices=3)
    assert color.read() == b'\x00\xff\x00\xff'
    ctx.shaders['color.frag'] = COLOR_FRAG.replace('out_color = color;', 'out_color = color.bgra;')
    third = ctx.program('fullscreen.vert', 'color.frag', uniforms={'color': 'vec4'})
    third.use()
    third['color'] = (1.0, 0.0, 0.0, 1.0)
    ctx.empty_vertex_array.render(vertices=3)
    assert color.read() == b'\x00\x00\xff\xff'
    ctx.shaders['color.frag'] = COLOR_FRAG
    ctx.release(second)
    ctx.release(third)


def test_empty_vertex_array(ctx):
    assert ctx.empty_vertex_array.vertices == 0
    assert ctx.empty_vertex_array not in ctx.gc()
    pipeline = ctx.pipeline(
        'fullscreen.vert',
        'color.frag',
        uniforms={'color': 'vec4'},
        vertex_array=ctx.empty_vertex_array,
        framebuffer=ctx.framebuffer([ctx.texture((2, 2), 'rgba8unorm')]),
        vertices=3,
    )
    pipeline['color'] = (1.0, 0.0, 0.0, 1.0)
    pipeline.run()
    texture = pipeline.framebuffer.attachments[0][0]
    assert texture.read() == b'\xff\x00\x00\xff' * 4
    ctx.release(pipeline)


class MissingLoaderMethod:
    pass


class BrokenLoaderResult:
    def load_opengl_function(self, name):
        return 'x'


class PartialLoader:
    def load_opengl_function(self, name):
        return 0 if name == 'glFlush' else 1


class HookLoader:
    def __init__(self):
        self.lib = ctypes.CDLL('libEGL.so.1')
        self.lib.eglGetProcAddress.restype = ctypes.c_void_p
        self.lib.eglGetProcAddress.argtypes = [ctypes.c_char_p]
        self.flushes = 0

        def flush():
            self.flushes += 1

        self.flush = ctypes.CFUNCTYPE(None)(flush)

    def load_opengl_function(self, name):
        if name == 'glFlush':
            return ctypes.cast(self.flush, ctypes.c_void_p).value
        return self.lib.eglGetProcAddress(name.encode())


def test_init(ctx):
    with pytest.raises(ValueError, match='load_opengl_function'):
        moderngl.init(MissingLoaderMethod())
    with pytest.raises(TypeError):
        moderngl.init(BrokenLoaderResult())
    with pytest.raises(RuntimeError, match='glFlush'):
        moderngl.init(PartialLoader())
    loader = HookLoader()
    moderngl.init(loader)
    ctx.end_frame()
    ctx.end_frame()
    assert loader.flushes == 2
    assert moderngl.get_proc_address('glFlush') == loader.load_opengl_function('glFlush')
    moderngl.init()
    ctx.end_frame()
    assert loader.flushes == 2
    assert moderngl.get_context() is ctx


def test_get_proc_address(ctx):
    import ctypes
    flush = ctypes.CFUNCTYPE(None)(moderngl.get_proc_address('glFlush'))
    flush()
    assert moderngl.get_proc_address('glDispatchCompute')


def test_source(ctx):
    ctx.shaders['source/common'] = 'float a;\n'
    ctx.shaders['source/main'] = '#version 330 core\n#include "source/common"\nvoid main() {}\n'
    assert ctx.source('source/main') == '#version 330 core\nfloat a;\n\n#line 3\nvoid main() {}\n'
    ctx.shaders['source/plain'] = 'void main() {}\n'
    assert ctx.source('source/plain') is ctx.shaders['source/plain']
    with pytest.raises(KeyError):
        ctx.source('source/missing')


def test_info(ctx):
    assert set(ctx.info) == {
        'vendor',
        'renderer',
        'version',
        'glsl',
        'max_samples',
        'max_vertex_attribs',
        'max_samplers',
        'max_uniform_buffers',
        'max_uniform_block_size',
        'uniform_buffer_offset_alignment',
    }
    assert isinstance(ctx.info['version'], str)
    assert 1 <= ctx.info['max_samplers'] <= 16


def test_context_once(ctx):
    with pytest.warns(DeprecationWarning), pytest.raises(RuntimeError):
        moderngl.create_context()
    assert moderngl.get_context() is ctx


def test_buffer(ctx):
    buffer = ctx.buffer(b'abcdefgh')
    assert buffer.size == 8
    assert buffer.read() == b'abcdefgh'
    buffer.write(b'XY', offset=2)
    assert buffer.read() == b'abXYefgh'
    assert buffer.read(size=3, offset=1) == b'bXY'


def test_buffer_kind(ctx):
    assert ctx.buffer(size=4).kind == 'vertex_buffer'
    for kind in ['index_buffer', 'uniform_buffer', 'copy_buffer']:
        buffer = ctx.buffer(b'abcd', kind=kind, usage='stream_draw')
        assert buffer.kind == kind
        buffer.write(b'xy', offset=1)
        assert buffer.read() == b'axyd'


def test_buffer_errors(ctx):
    with pytest.raises(ValueError):
        ctx.buffer()
    with pytest.raises(ValueError):
        ctx.buffer(b'abc', size=3)
    with pytest.raises(ValueError):
        ctx.buffer(b'')
    with pytest.raises(ValueError):
        ctx.buffer(size=4, kind='storage_buffer')
    with pytest.raises(ValueError):
        ctx.buffer(size=4, usage='static')
    buffer = ctx.buffer(size=4)
    with pytest.raises(ValueError):
        buffer.write(b'abcde')
    with pytest.raises(ValueError):
        buffer.write(b'ab', offset=3)
    with pytest.raises(ValueError):
        buffer.read(size=5)


def test_texture(ctx):
    texture = ctx.texture((4, 4), 'rgba8unorm', bytes(64))
    assert texture.size == (4, 4)
    assert texture.format == 'rgba8unorm'
    assert texture.samples == 1
    assert texture.renderbuffer is False
    texture.write(bytes(16), size=(2, 2), offset=(1, 1))


def test_texture_padding(ctx):
    texture = ctx.texture((3, 3), 'r8unorm')
    texture.write(bytes(12))
    with pytest.raises(ValueError):
        texture.write(bytes(9))


def test_texture_renderbuffer(ctx):
    assert ctx.texture((64, 64), samples=4).renderbuffer is True
    assert ctx.texture((64, 64), renderbuffer=True).renderbuffer is True
    assert ctx.texture((64, 64), renderbuffer=False).renderbuffer is False
    assert ctx.texture((64, 64), 'depth24plus').renderbuffer is False
    with pytest.raises(TypeError):
        ctx.texture((64, 64), samples=4, renderbuffer=False)
    with pytest.raises(TypeError):
        ctx.texture((64, 64), array=2, renderbuffer=True)
    with pytest.raises(ValueError):
        ctx.texture((64, 64), samples=4, data=bytes(64 * 64 * 4))


def test_texture_layers(ctx):
    array = ctx.texture((4, 4), 'rgba8unorm', bytes(64 * 3), array=3)
    array.write(bytes(64), layer=1)
    cubemap = ctx.texture((4, 4), 'rgba8unorm', bytes(64 * 6), cubemap=True)
    cubemap.write(bytes(64), layer=5)
    with pytest.raises(TypeError):
        ctx.texture((4, 4), cubemap=True, array=2)


def test_texture_levels(ctx):
    texture = ctx.texture((16, 16), levels=0)
    assert texture.levels == 5
    texture.write(bytes(16), level=3)
    texture.write(bytes(4), size=(1, 1), offset=(1, 1), level=3)
    with pytest.raises(ValueError):
        texture.write(bytes(64), size=(4, 4), level=3)
    texture.mipmaps()


def test_texture_errors(ctx):
    with pytest.raises(ValueError):
        ctx.texture((4, 4), 'rgb8unorm')
    with pytest.raises(ValueError):
        ctx.texture((0, 4))
    with pytest.raises(ValueError):
        ctx.texture((4, 8), cubemap=True)
    with pytest.raises(TypeError):
        ctx.texture((4, 4), 'depth24plus').write(bytes(64))


def test_copy_buffer(ctx):
    src = ctx.buffer(b'abcdefgh')
    dst = ctx.buffer(size=8, kind='copy_buffer')
    ctx.copy_buffer(src, dst)
    assert dst.read() == b'abcdefgh'
    dst.write(bytes(8))
    ctx.copy_buffer(src, dst, 3, src_offset=2, dst_offset=4)
    assert dst.read() == b'\x00\x00\x00\x00cde\x00'
    with pytest.raises(ValueError):
        ctx.copy_buffer(src, dst, src_offset=4, dst_offset=6)


def test_copy_buffer_kinds(ctx):
    vertex = ctx.buffer(size=16)
    uniform = ctx.buffer(size=16, kind='uniform_buffer')
    index = ctx.buffer(size=16, kind='index_buffer')
    other_index = ctx.buffer(size=16, kind='index_buffer')
    texture = ctx.texture((2, 2))
    ctx.copy_buffer(vertex, uniform)
    ctx.copy_buffer(index, other_index)
    ctx.copy_buffer_to_texture(vertex, texture)
    ctx.copy_texture_to_buffer(texture, uniform)
    with pytest.raises(TypeError):
        ctx.copy_buffer(index, vertex)
    with pytest.raises(TypeError):
        ctx.copy_buffer(vertex, index)
    with pytest.raises(TypeError):
        ctx.copy_buffer_to_texture(index, texture)
    with pytest.raises(TypeError):
        ctx.copy_texture_to_buffer(texture, index)


def test_texture_read(ctx):
    data = bytes(range(64))
    texture = ctx.texture((4, 4), 'rgba8unorm', data)
    assert texture.read() == data
    assert texture.read() == data
    padded = b'abc\x00def\x00ghi\x00'
    assert ctx.texture((3, 3), 'r8unorm', padded).read() == padded


def test_texture_read_layers_levels(ctx):
    array = ctx.texture((4, 4), 'rgba8unorm', bytes(64) + bytes(range(64)), array=2, levels=0)
    assert array.read(layer=1) == bytes(range(64))
    cubemap = ctx.texture((2, 2), 'rgba8unorm', bytes(range(96)), cubemap=True)
    assert cubemap.read(layer=5) == bytes(range(80, 96))
    texture = ctx.texture((4, 4), levels=0)
    texture.write(bytes(range(16)), level=1)
    assert texture.read(level=1) == bytes(range(16))
    with pytest.raises(ValueError):
        cubemap.read(layer=6)


def test_copy_texture_to_buffer(ctx):
    data = bytes(range(64))
    texture = ctx.texture((4, 4), 'rgba8unorm', data)
    buffer = ctx.buffer(size=80, kind='copy_buffer')
    ctx.copy_texture_to_buffer(texture, buffer, dst_offset=16)
    assert buffer.read(size=64, offset=16) == data
    with pytest.raises(ValueError):
        ctx.copy_texture_to_buffer(texture, buffer, dst_offset=20)
    with pytest.raises(TypeError):
        ctx.copy_texture_to_buffer(ctx.texture((4, 4), array=2), buffer)


def test_copy_buffer_to_texture(ctx):
    data = bytes(range(64))
    buffer = ctx.buffer(bytes(16) + data, kind='copy_buffer')
    texture = ctx.texture((4, 4), 'rgba8unorm')
    ctx.copy_buffer_to_texture(buffer, texture, src_offset=16)
    assert texture.read() == data
    cubemap = ctx.texture((4, 4), 'rgba8unorm', cubemap=True, levels=0)
    ctx.copy_buffer_to_texture(buffer, cubemap, src_offset=16, dst_layer=3)
    assert cubemap.read(layer=3) == data
    ctx.copy_buffer_to_texture(buffer, cubemap, src_offset=16, dst_layer=2, dst_level=1)
    assert cubemap.read(layer=2, level=1) == data[:16]
    with pytest.raises(ValueError):
        ctx.copy_buffer_to_texture(buffer, texture, src_offset=20)
    with pytest.raises(TypeError):
        ctx.copy_buffer_to_texture(buffer, ctx.texture((4, 4), renderbuffer=True))


def test_raw_bits(ctx):
    data = bytes(range(64))
    src = ctx.texture((4, 4), 'rgba8unorm', data)
    dst = ctx.texture((4, 4), 'r32uint')
    buffer = ctx.buffer(size=64, kind='copy_buffer')
    ctx.copy_texture_to_buffer(src, buffer)
    ctx.copy_buffer_to_texture(buffer, dst)
    assert dst.read() == data


def test_copy_texture(ctx):
    data = bytes(range(64))
    src = ctx.texture((4, 4), 'rgba8unorm', data)
    dst = ctx.texture((4, 4), 'rgba8unorm')
    ctx.copy_texture(src, dst)
    assert dst.read() == data
    with pytest.raises(ValueError):
        ctx.copy_texture(src, src)
    with pytest.raises(ValueError):
        ctx.copy_texture(ctx.texture((8, 8)), src)
    with pytest.raises(TypeError):
        ctx.copy_texture(src, ctx.texture((4, 4), 'rgba8uint'))
    with pytest.raises(TypeError):
        ctx.copy_texture(src, ctx.texture((4, 4), array=2))


def test_copy_texture_resolve(ctx):
    multisampled = ctx.texture((16, 16), 'rgba8unorm', samples=4)
    resolved = ctx.texture((16, 16), 'rgba8unorm')
    ctx.copy_texture(multisampled, resolved)
    with pytest.raises(TypeError):
        ctx.copy_texture(resolved, multisampled)
    with pytest.raises(TypeError):
        ctx.copy_texture(multisampled, ctx.texture((16, 16), 'rgba16float'))
    with pytest.raises(TypeError):
        multisampled.read()


def test_copy_texture_depth(ctx):
    src = ctx.texture((8, 8), 'depth24plus')
    ctx.copy_texture(src, ctx.texture((8, 8), 'depth24plus'))
    ctx.copy_texture(ctx.texture((8, 8), 'depth24plus-stencil8'), ctx.texture((8, 8), 'depth24plus-stencil8'))
    with pytest.raises(TypeError):
        ctx.copy_texture(src, ctx.texture((8, 8), 'depth32float'))
    with pytest.raises(TypeError):
        ctx.copy_texture(src, ctx.texture((8, 8), 'rgba8unorm'))


def test_texture_read_region(ctx):
    data = bytes(range(64))
    texture = ctx.texture((4, 4), 'rgba8unorm', data)
    assert texture.read(size=(2, 1), offset=(1, 2)) == data[36:44]
    with pytest.raises(ValueError):
        texture.read(offset=(1, 1))
    with pytest.raises(ValueError):
        texture.read(size=(2, 2), offset=(3, 3))


def test_copy_texture_region(ctx):
    data = bytes(range(64))
    src = ctx.texture((4, 4), 'rgba8unorm', data)
    dst = ctx.texture((8, 8), 'rgba8unorm')
    ctx.copy_texture(src, dst, (2, 2), src_offset=(1, 1), dst_offset=(5, 6))
    assert dst.read(size=(2, 2), offset=(5, 6)) == data[20:28] + data[36:44]
    with pytest.raises(ValueError):
        ctx.copy_texture(dst, src)
    with pytest.raises(ValueError):
        ctx.copy_texture(src, dst, dst_offset=(5, 5))
    ctx.copy_texture(src, dst, dst_offset=(4, 4))
    assert dst.read(size=(4, 4), offset=(4, 4)) == data


def test_copy_texture_resolve_region(ctx):
    multisampled = ctx.texture((16, 16), 'rgba8unorm', samples=4)
    resolved = ctx.texture((16, 16), 'rgba8unorm')
    ctx.copy_texture(multisampled, resolved, (8, 8), src_offset=(4, 4), dst_offset=(4, 4))
    with pytest.raises(ValueError):
        ctx.copy_texture(multisampled, resolved, (8, 8), src_offset=(4, 4), dst_offset=(0, 0))


def test_copy_buffer_texture_region(ctx):
    data = bytes(range(16))
    buffer = ctx.buffer(bytes(8) + data, kind='copy_buffer')
    texture = ctx.texture((4, 4), 'rgba8unorm')
    ctx.copy_buffer_to_texture(buffer, texture, (2, 2), src_offset=8, dst_offset=(1, 1))
    assert texture.read(size=(2, 2), offset=(1, 1)) == data
    out = ctx.buffer(size=24, kind='copy_buffer')
    ctx.copy_texture_to_buffer(texture, out, (2, 2), src_offset=(1, 1), dst_offset=8)
    assert out.read(size=16, offset=8) == data
    with pytest.raises(ValueError):
        ctx.copy_texture_to_buffer(texture, out, (2, 2), src_offset=(1, 1), dst_offset=12)
    with pytest.raises(ValueError):
        ctx.copy_buffer_to_texture(buffer, texture, (2, 2), dst_offset=(3, 3))


def test_attachment(ctx):
    texture = ctx.texture((4, 4), levels=0)
    assert texture.attachment() == (texture, 0, 0)
    assert texture.attachment(level=2) == (texture, 0, 2)
    cubemap = ctx.texture((4, 4), cubemap=True)
    assert cubemap.attachment(face=5) == (cubemap, 5, 0)
    array = ctx.texture((4, 4), array=3)
    assert array.attachment(layer=2) == (array, 2, 0)
    with pytest.raises(TypeError):
        cubemap.attachment(layer=1)
    with pytest.raises(TypeError):
        array.attachment(face=1)
    with pytest.raises(TypeError):
        texture.attachment(layer=1)
    with pytest.raises(ValueError):
        cubemap.attachment(face=6)
    with pytest.raises(ValueError):
        texture.attachment(level=3)


def test_framebuffer(ctx):
    color = ctx.texture((4, 4), 'rgba8unorm')
    depth = ctx.texture((4, 4), 'depth24plus')
    fbo = ctx.framebuffer([color, depth])
    assert fbo.size == (4, 4)
    assert fbo.samples == 1
    assert fbo.attachments == ((color, 0, 0), (depth, 0, 0))
    assert fbo.clear_color == (0.0, 0.0, 0.0, 0.0)
    assert fbo.clear_depth == 1.0
    assert fbo.clear_stencil == 0
    assert fbo.viewport == (0, 0, 4, 4)
    fbo.viewport = (1, 1, 2, 2)
    assert fbo.viewport == (1, 1, 2, 2)
    fbo.clear_color = (1.0, 0.0, 0.0, 1.0)
    fbo.clear()
    assert color.read() == b'\xff\x00\x00\xff' * 16


def test_framebuffer_clear_per_framebuffer(ctx):
    color = ctx.texture((4, 4), 'rgba8unorm')
    red = ctx.framebuffer([color])
    blue = ctx.framebuffer([color])
    red.clear_color = [1.0, 0.0, 0.0, 1.0]
    blue.clear_color = (0.0, 0.0, 1.0, 1.0)
    assert red.clear_color == (1.0, 0.0, 0.0, 1.0)
    red.clear()
    assert color.read() == b'\xff\x00\x00\xff' * 16
    blue.clear()
    assert color.read() == b'\x00\x00\xff\xff' * 16


def test_framebuffer_clear_types(ctx):
    floats = ctx.texture((2, 2), 'r32float')
    ints = ctx.texture((2, 2), 'r32sint')
    uints = ctx.texture((2, 2), 'r32uint')
    depth = ctx.texture((2, 2), 'depth24plus-stencil8')
    fbo = ctx.framebuffer([floats, ints, uints, depth])
    assert fbo.clear_color == (0.0, 0, 0)
    fbo.clear_color = (-7.5, -7, 9)
    fbo.clear_depth = 0.5
    fbo.clear_stencil = 3
    assert fbo.clear_color == (-7.5, -7, 9)
    assert (fbo.clear_depth, fbo.clear_stencil) == (0.5, 3)
    fbo.clear()
    assert floats.read() == struct.pack('4f', -7.5, -7.5, -7.5, -7.5)
    assert ints.read() == struct.pack('4i', -7, -7, -7, -7)
    assert uints.read() == struct.pack('4I', 9, 9, 9, 9)


def test_clear_color_errors(ctx):
    fbo = ctx.framebuffer([
        ctx.texture((2, 2), 'rgba8unorm'),
        ctx.texture((2, 2), 'r32sint'),
        ctx.texture((2, 2), 'depth24plus-stencil8'),
    ])
    assert fbo.clear_color == ((0.0, 0.0, 0.0, 0.0), 0)
    with pytest.raises(ValueError):
        fbo.clear_color = ((1.0, 1.0, 1.0, 1.0),)
    with pytest.raises(ValueError):
        fbo.clear_color = ((1.0, 1.0, 1.0), 0)
    with pytest.raises(TypeError):
        fbo.clear_color = (1.0, 0)
    with pytest.raises(TypeError):
        fbo.clear_color = ((1.0, 1.0, 1.0, 1.0), 1.5)
    with pytest.raises(TypeError):
        fbo.clear_color[0] = (1.0, 1.0, 1.0, 1.0)
    with pytest.raises(TypeError):
        fbo.clear_depth = 'x'
    with pytest.raises(TypeError):
        fbo.clear_stencil = 1.5
    fbo.clear_color = ((0.5, 0.5, 0.5, 0.5), 3)
    with pytest.raises(TypeError):
        fbo.clear_color = ((1.0, 1.0, 1.0, 1.0), 'x')
    assert fbo.clear_color == ((0.5, 0.5, 0.5, 0.5), 3)
    single = ctx.framebuffer([ctx.texture((2, 2), 'r32float')])
    single.clear_color = 2.5
    assert single.clear_color == 2.5
    with pytest.raises(ValueError):
        ctx.framebuffer([ctx.texture((2, 2), 'rgba8unorm')]).clear_color = (1.0, 1.0)
    depth_only = ctx.framebuffer([ctx.texture((2, 2), 'depth24plus')])
    assert depth_only.clear_color is None
    depth_only.clear_color = (1.0, 0.0, 0.0, 1.0)
    depth_only.clear_stencil = 5
    depth_only.clear()


def test_framebuffer_faces_levels(ctx):
    cubemap = ctx.texture((4, 4), 'rgba8unorm', cubemap=True)
    fbo = ctx.framebuffer([cubemap.attachment(face=3)])
    assert fbo.attachments == ((cubemap, 3, 0),)
    fbo.clear_color = (0.0, 1.0, 0.0, 1.0)
    fbo.clear()
    assert cubemap.read(layer=3) == b'\x00\xff\x00\xff' * 16
    assert cubemap.read(layer=2) == bytes(64)
    texture = ctx.texture((8, 8), 'rgba8unorm', levels=0)
    depth = ctx.texture((2, 2), 'depth24plus')
    fbo = ctx.framebuffer([texture.attachment(level=2), depth])
    assert fbo.size == (2, 2)
    assert fbo.attachments[0] == (texture, 0, 2)
    fbo.clear_color = (0.0, 0.0, 1.0, 1.0)
    fbo.clear()
    assert texture.read(level=2) == b'\x00\x00\xff\xff' * 4
    array = ctx.texture((4, 4), 'rgba8unorm', array=2)
    fbo = ctx.framebuffer([array.attachment(layer=1)])
    fbo.clear_color = (1.0, 1.0, 1.0, 1.0)
    fbo.clear()
    with pytest.raises(TypeError):
        ctx.framebuffer([array])
    with pytest.raises(TypeError):
        ctx.framebuffer([cubemap])
    ctx.framebuffer([(array, 0, 0)])
    assert array.read(layer=1) == b'\xff' * 64


def test_framebuffer_depth_only(ctx):
    depth = ctx.texture((16, 16), 'depth32float')
    fbo = ctx.framebuffer([depth])
    fbo.clear_depth = 0.25
    fbo.clear()


def test_framebuffer_multisample(ctx):
    color = ctx.texture((16, 16), 'rgba8unorm', samples=4)
    depth = ctx.texture((16, 16), 'depth24plus', samples=4)
    fbo = ctx.framebuffer([color, depth])
    assert fbo.samples == color.samples
    fbo.clear_color = (1.0, 1.0, 1.0, 1.0)
    fbo.clear()
    resolved = ctx.texture((16, 16), 'rgba8unorm')
    ctx.copy_texture(color, resolved)
    assert resolved.read() == b'\xff' * 1024


def test_framebuffer_errors(ctx):
    color = ctx.texture((4, 4))
    depth = ctx.texture((4, 4), 'depth24plus')
    with pytest.raises(ValueError):
        ctx.framebuffer([])
    with pytest.raises(ValueError):
        ctx.framebuffer([depth, color])
    with pytest.raises(ValueError):
        ctx.framebuffer([color, depth, ctx.texture((4, 4), 'depth24plus')])
    with pytest.raises(ValueError):
        ctx.framebuffer([color, ctx.texture((8, 8), 'depth24plus')])
    with pytest.raises(ValueError):
        ctx.framebuffer([color, ctx.texture((4, 4), 'depth24plus', samples=4)])
    with pytest.raises(ValueError):
        ctx.framebuffer([(color, 0, 1)])
    with pytest.raises(TypeError):
        ctx.framebuffer([b'abc'])
    with pytest.raises(TypeError):
        ctx.framebuffer([(color, 0)])
    with pytest.raises(ValueError):
        ctx.framebuffer([ctx.texture((4, 4)) for _ in range(9)])


def test_external_framebuffer(ctx):
    color = ctx.texture((4, 4), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    external = ctx.external_framebuffer(fbo.glo, (4, 4), ['rgba8unorm'])
    assert external.attachments == ()
    assert external.size == (4, 4)
    external.clear_color = (0.0, 1.0, 1.0, 1.0)
    external.clear()
    assert color.read() == b'\x00\xff\xff\xff' * 16
    default = ctx.external_framebuffer(0, (16, 16))
    assert default.clear_color == (0.0, 0.0, 0.0, 0.0)
    with pytest.raises(ValueError):
        ctx.external_framebuffer(0, (4, 4), ['depth24plus', 'rgba8unorm'])
    with pytest.raises(ValueError):
        ctx.external_framebuffer(0, (4, 4), ['rgb8unorm'])
    with pytest.raises(ValueError):
        ctx.external_framebuffer(0, (4, 4), [])


def test_screen(ctx):
    assert ctx.screen is ctx.screen


def test_framebuffer_clear_attachments(ctx):
    first = ctx.texture((2, 2), 'r32float')
    second = ctx.texture((2, 2), 'r32float')
    depth = ctx.texture((2, 2), 'depth24plus')
    fbo = ctx.framebuffer([first, second, depth])
    fbo.clear_color = (1.0, 2.0)
    fbo.clear()
    fbo.clear_color = (3.0, 4.0)
    fbo.clear([1, -1])
    assert first.read() == struct.pack('4f', 1.0, 1.0, 1.0, 1.0)
    assert second.read() == struct.pack('4f', 4.0, 4.0, 4.0, 4.0)
    fbo.clear([])
    fbo.clear(attachments=(0,))
    assert first.read() == struct.pack('4f', 3.0, 3.0, 3.0, 3.0)
    with pytest.raises(ValueError):
        fbo.clear([3])
    with pytest.raises(ValueError):
        fbo.clear([-4])
    with pytest.raises(ValueError):
        fbo.clear(['x'])
    with pytest.raises(TypeError):
        fbo.clear(5)


def test_buffer_bind(ctx):
    vbo = ctx.buffer(size=64)
    assert vbo.bind('3f 2f', 0, 1) == (
        (vbo, 0, 'float32x3', 0, 20, 0),
        (vbo, 1, 'float32x2', 12, 20, 0),
    )
    assert vbo.bind('4f 4x 4nu1 /i', 2, 3, offset=8) == (
        (vbo, 2, 'float32x4', 8, 24, 1),
        (vbo, 3, 'unorm8x4', 28, 24, 1),
    )
    assert vbo.bind('3f 3f', -1, 4) == ((vbo, 4, 'float32x3', 12, 24, 0),)
    with pytest.raises(ValueError):
        vbo.bind('3f 3f', 0)
    with pytest.raises(ValueError):
        vbo.bind('3f', 0, 1)
    with pytest.raises(ValueError):
        vbo.bind('3g', 0)
    with pytest.raises(TypeError):
        vbo.bind('3f', 'a')
    with pytest.raises(TypeError):
        vbo.bind('3f', 0, stride=4)


def test_vertex_array(ctx):
    vbo = ctx.buffer(size=64)
    instances = ctx.buffer(size=64)
    ibo = ctx.buffer(size=12, kind='index_buffer')
    attributes = [*vbo.bind('3f 3f', 0, 1), *instances.bind('4f /i', 2)]
    vao = ctx.vertex_array(attributes, index_buffer=ibo, index_size=2)
    attributes.clear()
    assert len(vao.attributes) == 3
    assert vao.index_buffer is ibo
    assert vao.index_size == 2
    empty = ctx.vertex_array([])
    assert empty.attributes == ()
    assert empty.index_buffer is None
    manual = ctx.vertex_array([(vbo, 0, 'uint32x2', 0, 8, 0)])
    assert manual.attributes == ((vbo, 0, 'uint32x2', 0, 8, 0),)


def test_vertex_array_vertices(ctx):
    vbo = ctx.buffer(size=100)
    instances = ctx.buffer(size=16)
    assert ctx.vertex_array([*vbo.bind('3f 2f', 0, 1), *instances.bind('4f /i', 2)]).vertices == 5
    assert ctx.vertex_array(vbo.bind('3f 2f', 0, 1, offset=20)).vertices == 4
    assert ctx.vertex_array([(vbo, 0, 'float32x3', 0, 0, 0)]).vertices == 8
    assert ctx.vertex_array(vbo.bind('3f', 0), index_buffer=ctx.buffer(size=36, kind='index_buffer')).vertices == 9
    assert ctx.vertex_array([]).vertices == 0
    vao = ctx.vertex_array(vbo.bind('3f', 0), vertices=3)
    assert vao.vertices == 3
    vao.vertices = 6
    assert vao.vertices == 6


def test_vertex_array_errors(ctx):
    vbo = ctx.buffer(size=64)
    ibo = ctx.buffer(size=12, kind='index_buffer')
    with pytest.raises(TypeError):
        ctx.vertex_array(ctx.buffer(size=64, kind='copy_buffer').bind('3f', 0))
    with pytest.raises(TypeError):
        ctx.vertex_array(ctx.buffer(size=64, kind='uniform_buffer').bind('3f', 0))
    with pytest.raises(TypeError):
        ctx.vertex_array(ibo.bind('3f', 0))
    with pytest.raises(TypeError):
        ctx.vertex_array(vbo.bind('3f', 0), index_buffer=vbo)
    with pytest.raises(ValueError):
        ctx.vertex_array(vbo.bind('3f', 0), index_buffer=ibo, index_size=3)
    with pytest.raises(ValueError):
        ctx.vertex_array([*vbo.bind('3f', 0), *vbo.bind('3f', 0)])
    with pytest.raises(ValueError):
        ctx.vertex_array(vbo.bind('3f', 1000))
    with pytest.raises(ValueError):
        ctx.vertex_array([(vbo, 0, 'float32x5', 0, 0, 0)])
    with pytest.raises(TypeError):
        ctx.vertex_array([(vbo, 0, 'float32x3')])
    with pytest.raises(TypeError, match=r'\*buffer\.bind'):
        ctx.vertex_array([vbo.bind('3f', 0), *vbo.bind('3f', 1)])


def test_copy_texture_to_framebuffer(ctx):
    data = bytes(range(64))
    src = ctx.texture((4, 4), 'rgba8unorm', data)
    color = ctx.texture((8, 8), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    ctx.copy_texture(src, fbo, (4, 4), dst_offset=(2, 2))
    assert color.read(size=(4, 4), offset=(2, 2)) == data
    multisampled = ctx.texture((8, 8), 'rgba8unorm', samples=4)
    ctx.copy_texture(multisampled, fbo)
    with pytest.raises(TypeError):
        ctx.copy_texture(src, ctx.framebuffer([ctx.texture((4, 4), 'depth24plus')]))
    with pytest.raises(TypeError):
        ctx.copy_texture(src, ctx.framebuffer([multisampled]))
    ctx.copy_texture(ctx.texture((16, 16)), fbo, dst_offset=(4, 4))
    assert color.read(size=(4, 4), offset=(4, 4)) == bytes(64)
    with pytest.raises(TypeError):
        ctx.copy_texture(src, b'abc')


def test_copy_texture_scaled(ctx):
    src = ctx.texture((2, 1), 'rgba8unorm', bytes([255, 0, 0, 255, 0, 0, 255, 255]))
    dst = ctx.texture((4, 2), 'rgba8unorm')
    ctx.copy_texture(src, dst, dst_size=(4, 2))
    assert dst.read() == bytes([255, 0, 0, 255]) * 2 + bytes([0, 0, 255, 255]) * 2 + bytes([255, 0, 0, 255]) * 2 + bytes([0, 0, 255, 255]) * 2
    fbo = ctx.framebuffer([ctx.texture((4, 1), 'rgba8unorm')])
    ctx.copy_texture(src, fbo, dst_size=(4, 1), filter='linear')
    row = fbo.attachments[0][0].read()
    assert row[0] == 255 and 0 < row[4] < 255 and 0 < row[8] < 255 and row[14] == 255
    with pytest.raises(ValueError):
        ctx.copy_texture(src, dst, dst_size=(8, 2))
    with pytest.raises(ValueError):
        ctx.copy_texture(src, dst, filter='cubic')
    with pytest.raises(ValueError, match='resolve'):
        ctx.copy_texture(ctx.texture((2, 1), 'rgba8unorm', samples=4), dst, dst_size=(4, 2))
    depth = ctx.texture((2, 1), 'depth24plus')
    with pytest.raises(TypeError, match='nearest'):
        ctx.copy_texture(depth, ctx.texture((2, 1), 'depth24plus'), filter='linear')


def test_shaders_include(ctx):
    ctx.shaders['common/color.glsl'] = '''
        uniform vec4 color;
        vec4 get_color() {
            return color;
        }
    '''
    ctx.shaders['include.frag'] = '''
        #version 330 core
        #include "common/color.glsl"
        layout (location = 0) out vec4 out_color;
        void main() {
            out_color = get_color();
        }
    '''
    program = ctx.program('fullscreen.vert', 'include.frag', uniforms={'color': 'vec4'})
    color = ctx.texture((2, 2), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    fbo.use()
    program.use()
    program['color'] = (0.0, 1.0, 0.0, 1.0)
    empty(ctx).render(vertices=3)
    assert color.read() == b'\x00\xff\x00\xff' * 4


def test_shaders_include_errors(ctx):
    ctx.shaders['loop.glsl'] = '#include "loop.glsl"\n'
    ctx.shaders['loop.frag'] = '#version 330 core\n#include "loop.glsl"\nvoid main() {}\n'
    with pytest.raises(ValueError, match='include chain'):
        ctx.program('fullscreen.vert', 'loop.frag')
    ctx.shaders['bad.frag'] = '#version 330 core\n#include common.glsl\nvoid main() {}\n'
    with pytest.raises(ValueError, match='invalid include'):
        ctx.program('fullscreen.vert', 'bad.frag')
    ctx.shaders['missing_include.frag'] = '#version 330 core\n#include "nope.glsl"\nvoid main() {}\n'
    with pytest.raises(KeyError, match='nope.glsl'):
        ctx.program('fullscreen.vert', 'missing_include.frag')
    ctx.shaders['bytes.frag'] = b'abc'
    with pytest.raises(TypeError):
        ctx.program('fullscreen.vert', 'bytes.frag')


def test_shaders_include_line_numbers(ctx):
    ctx.shaders['five_lines.glsl'] = 'float a;\nfloat b;\nfloat c;\nfloat d;\nfloat e;\n'
    ctx.shaders['line_error.frag'] = '#version 330 core\n#include "five_lines.glsl"\nvoid main() {}\nerror_here;\n'
    with pytest.raises(ValueError, match=r'0:4\('):
        ctx.program('fullscreen.vert', 'line_error.frag')
    ctx.shaders['plain_error.frag'] = '#version 330 core\nvoid main() {}\nerror_here;\n'
    with pytest.raises(ValueError, match=r'0:3\('):
        ctx.program('fullscreen.vert', 'plain_error.frag')



DEPTH_VERT = '''
    #version 330 core
    uniform float depth;
    vec2 positions[3] = vec2[](vec2(-1.0, -1.0), vec2(3.0, -1.0), vec2(-1.0, 3.0));
    void main() {
        gl_Position = vec4(positions[gl_VertexID], depth, 1.0);
    }
'''


def render_color(ctx, fbo, color, depth=0.0):
    if 'depth.vert' not in ctx.shaders:
        ctx.shaders['depth.vert'] = DEPTH_VERT
    program = ctx.program('depth.vert', 'color.frag', uniforms={'color': 'vec4', 'depth': 'float'})
    fbo.use()
    program.use()
    program['color'] = color
    program['depth'] = depth
    empty(ctx).render(vertices=3)
    ctx.release(program)


def test_depth_state(ctx):
    assert (ctx.depth_test, ctx.depth_write, ctx.depth_func) == (False, True, 'less')
    color = ctx.texture((2, 2), 'rgba8unorm')
    fbo = ctx.framebuffer([color, ctx.texture((2, 2), 'depth24plus')])
    try:
        ctx.depth_test = True
        fbo.clear()
        render_color(ctx, fbo, (1.0, 0.0, 0.0, 1.0), 0.0)
        render_color(ctx, fbo, (0.0, 1.0, 0.0, 1.0), 0.5)
        assert color.read() == b'\xff\x00\x00\xff' * 4
        ctx.depth_func = 'greater'
        assert ctx.depth_func == 'greater'
        render_color(ctx, fbo, (0.0, 1.0, 0.0, 1.0), 0.5)
        assert color.read() == b'\x00\xff\x00\xff' * 4
        ctx.depth_func = 'less'
        ctx.depth_write = False
        assert ctx.depth_write is False
        render_color(ctx, fbo, (0.0, 0.0, 1.0, 1.0), -0.5)
        render_color(ctx, fbo, (1.0, 1.0, 1.0, 1.0), 0.0)
        assert color.read() == b'\xff' * 16
        fbo.clear_depth = 0.25
        fbo.clear()
        render_color(ctx, fbo, (1.0, 0.0, 0.0, 1.0), 0.0)
        assert color.read() == bytes(16)
        render_color(ctx, fbo, (1.0, 0.0, 0.0, 1.0), -0.75)
        assert color.read() == b'\xff\x00\x00\xff' * 4
        with pytest.raises(ValueError):
            ctx.depth_func = '<='
    finally:
        ctx.depth_test = False
        ctx.depth_write = True
        ctx.depth_func = 'less'


def test_cull_face(ctx):
    assert ctx.cull_face is None
    color = ctx.texture((2, 2), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    try:
        ctx.cull_face = 'back'
        render_color(ctx, fbo, (1.0, 0.0, 0.0, 1.0))
        assert color.read() == b'\xff\x00\x00\xff' * 4
        ctx.cull_face = 'front'
        assert ctx.cull_face == 'front'
        fbo.clear()
        render_color(ctx, fbo, (1.0, 0.0, 0.0, 1.0))
        assert color.read() == bytes(16)
        with pytest.raises(ValueError):
            ctx.cull_face = 'left'
    finally:
        ctx.cull_face = None


def test_blend(ctx):
    color = ctx.texture((2, 2), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    fbo.clear_color = (0.0, 0.0, 1.0, 1.0)
    try:
        fbo.clear()
        ctx.blend('src_alpha', 'one_minus_src_alpha')
        render_color(ctx, fbo, (1.0, 0.0, 0.0, 0.5))
        r, g, b, a = color.read()[:4]
        assert abs(r - 128) <= 1 and g == 0 and abs(b - 127) <= 1
        fbo.clear()
        ctx.blend('one', 'one', equation='reverse_subtract', src_alpha='zero', dst_alpha='one')
        render_color(ctx, fbo, (0.0, 0.0, 0.25, 1.0))
        assert color.read()[:4] == bytes([0, 0, 191, 255])
        ctx.blend()
        render_color(ctx, fbo, (1.0, 0.0, 0.0, 0.5))
        assert color.read()[:4] == bytes([255, 0, 0, 128])
        with pytest.raises(ValueError):
            ctx.blend('src_alpha', 'nope')
        with pytest.raises(TypeError):
            ctx.blend('src_alpha')
    finally:
        ctx.blend()


def test_sampler(ctx):
    ctx.shaders['sample.frag'] = '''
        #version 330 core
        uniform sampler2D Texture;
        uniform vec2 uv;
        layout (location = 0) out vec4 out_color;
        void main() {
            out_color = texture(Texture, uv);
        }
    '''
    program = ctx.program('fullscreen.vert', 'sample.frag', samplers={'Texture': 3}, uniforms={'uv': 'vec2'})
    texture = ctx.texture((2, 1), 'rgba8unorm', bytes([255, 0, 0, 255, 0, 0, 255, 255]))
    color = ctx.texture((1, 1), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    nearest = ctx.sampler(texture, min_filter='nearest', mag_filter='nearest')
    linear = ctx.sampler(texture)
    clamp = ctx.sampler(texture, min_filter='nearest', mag_filter='nearest', wrap_x='clamp_to_edge')
    assert linear.texture is texture
    program.use()
    program['uv'] = (0.3, 0.5)
    nearest.use(3)
    fbo.use()
    program.use()
    empty(ctx).render(vertices=3)
    assert color.read() == bytes([255, 0, 0, 255])
    linear.use(3)
    fbo.use()
    program.use()
    empty(ctx).render(vertices=3)
    r, g, b, a = color.read()
    assert abs(r - 230) <= 2 and abs(b - 25) <= 2
    program['uv'] = (1.3, 0.5)
    nearest.use(3)
    fbo.use()
    program.use()
    empty(ctx).render(vertices=3)
    assert color.read() == bytes([255, 0, 0, 255])
    clamp.use(3)
    fbo.use()
    program.use()
    empty(ctx).render(vertices=3)
    assert color.read() == bytes([0, 0, 255, 255])


def test_sampler_errors(ctx):
    texture = ctx.texture((4, 4))
    with pytest.raises(TypeError):
        ctx.sampler(ctx.texture((4, 4), samples=4))
    with pytest.raises(ValueError):
        ctx.sampler(texture, min_filter='cubic')
    with pytest.raises(ValueError):
        ctx.sampler(texture, mag_filter='linear_mipmap_linear')
    with pytest.raises(ValueError):
        ctx.sampler(texture, wrap_y='clamp')
    with pytest.raises(ValueError):
        ctx.sampler(texture, compare_func='<')
    ctx.sampler(ctx.texture((4, 4), 'depth24plus'), compare_func='lequal')
    sampler = ctx.sampler(texture)
    with pytest.raises(ValueError):
        sampler.use(-1)
    with pytest.raises(ValueError):
        sampler.use(10000)


def test_uniform_buffer(ctx):
    ctx.shaders['block.frag'] = '''
        #version 330 core
        layout (std140) uniform Common {
            vec4 color;
        };
        layout (location = 0) out vec4 out_color;
        void main() {
            out_color = color;
        }
    '''
    program = ctx.program('fullscreen.vert', 'block.frag', uniform_buffers={'Common': 2})
    alignment = 256
    data = bytearray(alignment + 16)
    data[0:16] = struct.pack('4f', 1.0, 0.0, 0.0, 1.0)
    data[alignment:alignment + 16] = struct.pack('4f', 0.0, 1.0, 0.0, 1.0)
    buffer = ctx.buffer(data, kind='uniform_buffer')
    color = ctx.texture((1, 1), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    buffer.use(2, size=16)
    fbo.use()
    program.use()
    empty(ctx).render(vertices=3)
    assert color.read() == bytes([255, 0, 0, 255])
    buffer.use(2, alignment)
    fbo.use()
    program.use()
    empty(ctx).render(vertices=3)
    assert color.read() == bytes([0, 255, 0, 255])
    with pytest.raises(TypeError):
        ctx.buffer(size=16).use(0)
    with pytest.raises(ValueError):
        buffer.use(2, 1)
    with pytest.raises(ValueError):
        buffer.use(2, size=1000)
    with pytest.raises(ValueError):
        buffer.use(-1)



def test_stencil(ctx):
    ctx.shaders['flip.vert'] = '''
        #version 330 core
        uniform float flip;
        vec2 positions[3] = vec2[](vec2(-1.0, -1.0), vec2(3.0, -1.0), vec2(-1.0, 3.0));
        void main() {
            gl_Position = vec4(positions[gl_VertexID] * vec2(flip, 1.0), 0.0, 1.0);
        }
    '''
    color = ctx.texture((2, 1), 'rgba8unorm')
    fbo = ctx.framebuffer([color, ctx.texture((2, 1), 'depth24plus-stencil8')])
    red = b'\xff\x00\x00\xff'
    green = b'\x00\xff\x00\xff'
    blue = b'\x00\x00\xff\xff'
    try:
        fbo.clear()
        ctx.stencil('always', 1, depth_pass='replace')
        fbo.viewport = (0, 0, 1, 1)
        render_color(ctx, fbo, (1.0, 0.0, 0.0, 1.0))
        fbo.viewport = (0, 0, 2, 1)
        ctx.stencil('equal', 1)
        render_color(ctx, fbo, (0.0, 1.0, 0.0, 1.0))
        assert color.read() == green + bytes(4)
        ctx.stencil('always', 0, write_mask=0)
        fbo.clear_stencil = 1
        fbo.clear()
        ctx.stencil('equal', 1)
        render_color(ctx, fbo, (0.0, 0.0, 1.0, 1.0))
        assert color.read() == blue * 2
        fbo.clear_stencil = 0
        fbo.clear()
        ctx.stencil('always', 0, back_func='never')
        program = ctx.program('flip.vert', 'color.frag', uniforms={'color': 'vec4', 'flip': 'float'})
        fbo.use()
        program.use()
        program['color'] = (1.0, 0.0, 0.0, 1.0)
        program['flip'] = 1.0
        empty(ctx).render(vertices=3)
        assert color.read() == red * 2
        fbo.clear()
        program['flip'] = -1.0
        fbo.use()
        program.use()
        empty(ctx).render(vertices=3)
        assert color.read() == bytes(8)
        ctx.stencil()
        fbo.use()
        program.use()
        empty(ctx).render(vertices=3)
        assert color.read() == red * 2
        with pytest.raises(ValueError):
            ctx.stencil('equal', 1, depth_pass='increment')
        with pytest.raises(ValueError):
            ctx.stencil('==', 1)
    finally:
        ctx.stencil()


GL = ctypes.CDLL('libGL.so.1')

for name in ['glIsBuffer', 'glIsTexture', 'glIsRenderbuffer', 'glIsFramebuffer', 'glIsVertexArray', 'glIsProgram', 'glIsSampler']:
    getattr(GL, name).restype = ctypes.c_ubyte


def test_release(ctx):
    buffer = ctx.buffer(size=16)
    texture = ctx.texture((4, 4))
    renderbuffer = ctx.texture((4, 4), samples=4)
    framebuffer = ctx.framebuffer([texture])
    vertex_array = ctx.vertex_array(buffer.bind('4f', 0))
    program = ctx.program('fullscreen.vert', 'color.frag')
    sampler = ctx.sampler(texture)
    ctx.copy_texture_to_buffer(texture, ctx.buffer(size=64, kind='copy_buffer'))
    objects = [
        (buffer, GL.glIsBuffer),
        (texture, GL.glIsTexture),
        (renderbuffer, GL.glIsRenderbuffer),
        (framebuffer, GL.glIsFramebuffer),
        (vertex_array, GL.glIsVertexArray),
        (program, GL.glIsProgram),
        (sampler, GL.glIsSampler),
    ]
    for obj, is_alive in objects:
        assert is_alive(obj.glo)
        refcount = sys.getrefcount(obj)
        ctx.release(obj)
        assert sys.getrefcount(obj) == refcount - 1
        assert not is_alive(obj.glo)
        ctx.release(obj)
        assert sys.getrefcount(obj) == refcount - 1


def test_release_external(ctx):
    fbo = ctx.framebuffer([ctx.texture((4, 4))])
    external = ctx.external_framebuffer(fbo.glo, (4, 4), ['rgba8unorm'])
    ctx.release(external)
    assert GL.glIsFramebuffer(fbo.glo)
    with pytest.raises(TypeError):
        ctx.release('everything')
    with pytest.raises(TypeError):
        ctx.release(None)


def test_reset_state(ctx):
    texture = ctx.texture((4, 4))
    sampler = ctx.sampler(texture)
    buffer = ctx.buffer(size=16, kind='uniform_buffer')
    ctx.depth_test = True
    ctx.depth_write = False
    ctx.depth_func = 'always'
    ctx.cull_face = 'front'
    ctx.blend('src_alpha', 'one_minus_src_alpha')
    ctx.stencil('equal', 3, write_mask=0)
    sampler.use(15)
    buffer.use(7)
    with pytest.raises(ValueError):
        sampler.use(16)
    with pytest.raises(ValueError):
        buffer.use(8)
    ctx.reset_state()
    assert (ctx.depth_test, ctx.depth_write, ctx.depth_func, ctx.cull_face) == (False, True, 'less', None)
    value = ctypes.c_int()
    GL.glIsEnabled.restype = ctypes.c_ubyte
    assert not GL.glIsEnabled(0x0B71)
    assert not GL.glIsEnabled(0x0BE2)
    assert not GL.glIsEnabled(0x0B44)
    assert not GL.glIsEnabled(0x0B90)
    GL.glGetIntegerv(0x0B98, ctypes.byref(value))
    assert value.value & 0xFF == 0xFF
    GL.glGetIntegerv(0x0B72, ctypes.byref(value))
    assert value.value == 1
    GL.glActiveTexture(0x84C0 + 15)
    GL.glGetIntegerv(0x8919, ctypes.byref(value))
    assert value.value == 0
    GL.glGetIntegerv(0x8069, ctypes.byref(value))
    assert value.value == 0
    GL.glActiveTexture(0x84C0)
    GL.glGetIntegeri_v(0x8A28, 7, ctypes.byref(value))
    assert value.value == 0


def test_vertex_array_draw(ctx):
    ctx.shaders['draw.vert'] = '''
        #version 330 core
        layout (location = 0) in vec2 in_vertex;
        void main() {
            gl_Position = vec4(in_vertex, 0.0, 1.0);
        }
    '''
    left = [-1, -1, 0, -1, -1, 1, -1, 1, 0, -1, 0, 1]
    right = [0, -1, 1, -1, 0, 1, 0, 1, 1, -1, 1, 1]
    vbo = ctx.buffer(struct.pack('24f', *left, *right))
    vao = ctx.vertex_array(vbo.bind('2f', 0))
    program = ctx.program('draw.vert', 'color.frag', uniforms={'color': 'vec4'})
    color = ctx.texture((2, 1), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    fbo.clear()
    fbo.use()
    program.use()
    with pytest.raises(RuntimeError, match='vertex_array.use'):
        ctx.reset_state()
        fbo.use()
        program.use()
        vao.draw(6)
    vao.use()
    program['color'] = (1.0, 0.0, 0.0, 1.0)
    vao.draw(6)
    program['color'] = (0.0, 0.0, 1.0, 1.0)
    vao.draw(6, 6)
    assert color.read() == bytes([255, 0, 0, 255, 0, 0, 255, 255])
    with pytest.raises(ValueError):
        vao.draw(-1)
    with pytest.raises(TypeError):
        vao.draw()


def test_buffer_view(ctx):
    buffer = ctx.buffer(size=64)
    assert buffer.view() == (buffer, 0, 64)
    assert buffer.view(16, 8) == (buffer, 16, 8)
    with pytest.raises(ValueError):
        buffer.view(60, 8)
    with pytest.raises(ValueError):
        buffer.view(size=0)
    ubo = ctx.buffer(size=1024, kind='uniform_buffer')
    with pytest.raises(ValueError, match='multiple'):
        ubo.view(4)


def test_resources(ctx):
    ctx.shaders['resources.frag'] = '''
        #version 330 core
        layout (std140) uniform Tint {
            vec4 tint;
        };
        uniform sampler2D Left;
        uniform sampler2D Right;
        layout (location = 0) out vec4 out_color;
        void main() {
            vec4 sampled = gl_FragCoord.x < 1.0 ? texture(Left, vec2(0.5)) : texture(Right, vec2(0.5));
            out_color = sampled * tint;
        }
    '''
    program = ctx.program('fullscreen.vert', 'resources.frag', samplers={'Left': 2, 'Right': 5}, uniform_buffers={'Tint': 1})
    red = ctx.sampler(ctx.texture((1, 1), 'rgba8unorm', bytes([255, 0, 0, 255])))
    blue = ctx.sampler(ctx.texture((1, 1), 'rgba8unorm', bytes([0, 0, 255, 255])))
    ubo = ctx.buffer(bytes(256) + struct.pack('4f', 1.0, 1.0, 1.0, 1.0) * 48, kind='uniform_buffer')
    textures = ctx.resources(samplers={2: red, 5: blue})
    tint = ctx.resources(uniform_buffers={1: ubo.view(256, 32)})
    assert textures.samplers == {2: red, 5: blue}
    assert tint.uniform_buffers == {1: (ubo, 256, 32)}
    assert ctx.resources(uniform_buffers={0: ubo}).uniform_buffers == {0: (ubo, 0, 1024)}
    color = ctx.texture((2, 1), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    fbo.use()
    program.use()
    textures.use()
    tint.use()
    empty(ctx).render(vertices=3)
    assert color.read() == bytes([255, 0, 0, 255, 0, 0, 255, 255])
    swapped = ctx.resources(samplers={2: blue, 5: red})
    swapped.use()
    empty(ctx).render(vertices=3)
    assert color.read() == bytes([0, 0, 255, 255, 255, 0, 0, 255])
    with pytest.raises(ValueError):
        ctx.resources(samplers={16: red})
    with pytest.raises(ValueError):
        ctx.resources(uniform_buffers={8: ubo})
    with pytest.raises(TypeError):
        ctx.resources(samplers={0: ubo})
    with pytest.raises(TypeError):
        ctx.resources(uniform_buffers={0: ctx.buffer(size=16)})
    with pytest.raises(ValueError, match='multiple'):
        ctx.resources(uniform_buffers={0: (ubo, 4, 16)})


BLUR_VERT = '''
    #version 330 core
    out vec2 v_uv;
    void main() {
        v_uv = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
        gl_Position = vec4(v_uv * 2.0 - 1.0, 0.0, 1.0);
    }
'''

BLUR_FRAG = '''
    #version 330 core
    uniform sampler2D Source;
    uniform bool vertical;
    in vec2 v_uv;
    layout (location = 0) out vec4 out_color;
    void main() {
        vec2 texel = 1.0 / vec2(textureSize(Source, 0));
        vec2 offset = vertical ? vec2(0.0, texel.y) : vec2(texel.x, 0.0);
        out_color = texture(Source, v_uv) * 0.5 + texture(Source, v_uv + offset) * 0.25 + texture(Source, v_uv - offset) * 0.25;
    }
'''


def test_pipeline(ctx):
    ctx.shaders['blur.vert'] = BLUR_VERT
    ctx.shaders['blur.frag'] = BLUR_FRAG
    a = ctx.texture((5, 5), 'rgba8unorm')
    b = ctx.texture((5, 5), 'rgba8unorm')

    def blur(src, dst):
        return ctx.pipeline(
            'blur.vert',
            'blur.frag',
            samplers={'Source': 0},
            uniforms={'vertical': 'bool'},
            vertex_array=ctx.vertex_array([], vertices=3),
            framebuffer=ctx.framebuffer([dst]),
            resources=ctx.resources(
                samplers={
                    0: ctx.sampler(src, min_filter='nearest', mag_filter='nearest', wrap_x='clamp_to_edge', wrap_y='clamp_to_edge'),
                },
            ),
        )

    blur_x = blur(a, b)
    blur_y = blur(b, a)
    blur_x['vertical'] = False
    blur_y['vertical'] = True
    assert blur_x.vertices == 3
    pixels = bytearray(5 * 5 * 4)
    pixels[(2 * 5 + 2) * 4:(2 * 5 + 2) * 4 + 4] = b'\xff\xff\xff\xff'
    a.write(bytes(pixels))
    ctx.depth_test = True
    ctx.cull_face = 'front'
    ctx.blend('one', 'one')
    blur_x.run()
    blur_y.run()
    assert (ctx.depth_test, ctx.cull_face) == (False, None)
    assert ctx.bound_program is blur_y.program
    assert ctx.bound_framebuffer is blur_y.framebuffer
    out = a.read()
    row = [out[(2 * 5 + x) * 4] for x in range(5)]
    column = [out[(y * 5 + 2) * 4] for y in range(5)]
    assert row == column == [0, 32, 64, 32, 0]
    assert blur_x.program.debug()['uniforms'] == {'vertical': False}
    assert blur_y.program.debug()['uniforms'] == {'vertical': True}
    with pytest.raises(KeyError):
        blur_x['missing'] = 1
    with pytest.raises(TypeError):
        blur_x['vertical'] = 'x'
    ctx.release(blur_x)
    assert not GL.glIsProgram(blur_x.program.glo)


def test_pipeline_errors(ctx):
    ctx.shaders['blur.vert'] = BLUR_VERT
    ctx.shaders['blur.frag'] = BLUR_FRAG
    vao = ctx.vertex_array([], vertices=3)
    fbo = ctx.framebuffer([ctx.texture((1, 1), 'rgba8unorm')])
    other = ctx.resources(samplers={1: ctx.sampler(ctx.texture((1, 1), 'rgba8unorm'))})
    with pytest.raises(ValueError, match='Source'):
        ctx.pipeline('blur.vert', 'blur.frag', samplers={'Source': 0}, vertex_array=vao, framebuffer=fbo, resources=other)
    with pytest.raises(TypeError):
        ctx.pipeline('blur.vert', 'blur.frag', samplers={'Source': 0}, framebuffer=fbo)
    with pytest.raises(TypeError, match='combine'):
        ctx.pipeline('blur.vert', 'blur.frag', samplers={'Source': 1}, vertex_array=vao, framebuffer=fbo, resources=[other])
    with pytest.raises(TypeError):
        ctx.pipeline('blur.vert', 'blur.frag', samplers={'Source': 1}, vertex_array=vao, framebuffer=fbo, resources=other, blend='add')
    with pytest.raises(ValueError):
        ctx.pipeline('blur.vert', 'blur.frag', samplers={'Source': 1}, vertex_array=vao, framebuffer=fbo, resources=other, blend={'src': 'x', 'dst': 'one'})
    pipeline = ctx.pipeline(
        'fullscreen.vert',
        'color.frag',
        uniforms={'color': 'vec4'},
        vertex_array=vao,
        framebuffer=fbo,
        blend={'src': 'one', 'dst': 'one'},
        stencil={'func': 'always', 'ref': 1},
    )
    pipeline['color'] = (0.25, 0.0, 0.0, 1.0)
    fbo.clear()
    pipeline.run()
    pipeline.run()
    assert fbo.attachments[0][0].read()[0] in (127, 128)
    pipeline.instances = 0
    fbo.clear()
    pipeline.run()
    assert fbo.attachments[0][0].read() == bytes(4)
    ctx.blend()
    ctx.stencil()


def test_frame(ctx):
    program = ctx.program('fullscreen.vert', 'color.frag', uniforms={'color': 'vec4'})
    program.use()
    ctx.depth_test = True
    ctx.cull_face = 'back'
    ctx.start_frame()
    assert ctx.bound_program is None
    assert ctx.bound_framebuffer is ctx.screen
    assert (ctx.depth_test, ctx.cull_face) == (False, None)
    ctx.end_frame()
    ctx.release(program)


def test_resources_combine(ctx):
    a = ctx.sampler(ctx.texture((1, 1)))
    b = ctx.sampler(ctx.texture((1, 1)))
    c = ctx.sampler(ctx.texture((1, 1)))
    ubo = ctx.buffer(size=256, kind='uniform_buffer')
    textures = ctx.resources(samplers={0: a, 1: b})
    common = ctx.resources(uniform_buffers={0: ubo})
    combined = ctx.resources(textures, common)
    assert combined.samplers == {0: a, 1: b}
    assert combined.uniform_buffers == {0: (ubo, 0, 256)}
    replaced = ctx.resources(combined, samplers={1: c})
    assert replaced.samplers == {0: a, 1: c}
    assert combined.samplers == {0: a, 1: b}
    with pytest.raises(ValueError, match='more than one'):
        ctx.resources(textures, ctx.resources(samplers={1: c}))
    with pytest.raises(ValueError, match='more than one'):
        ctx.resources(common, common)
    with pytest.raises(TypeError):
        ctx.resources(a)
    assert ctx.resources().samplers == {}


def test_pipeline_viewport(ctx):
    color = ctx.texture((4, 4), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    pipeline = ctx.pipeline(
        'fullscreen.vert',
        'color.frag',
        uniforms={'color': 'vec4'},
        vertex_array=ctx.empty_vertex_array,
        vertices=3,
        framebuffer=fbo,
        viewport=(0, 0, 2, 2),
    )
    assert pipeline.viewport == (0, 0, 2, 2)
    pipeline['color'] = (1.0, 0.0, 0.0, 1.0)
    fbo.clear()
    pipeline.run()
    assert color.read(size=(2, 2)) == b'\xff\x00\x00\xff' * 4
    assert color.read(size=(2, 2), offset=(2, 2)) == bytes(16)
    fbo.viewport = (2, 2, 2, 2)
    pipeline.viewport = None
    fbo.clear()
    pipeline.run()
    assert color.read() == b'\xff\x00\x00\xff' * 16
    fbo.clear()
    program = ctx.program('fullscreen.vert', 'color.frag', uniforms={'color': 'vec4'})
    fbo.use()
    program.use()
    program['color'] = (0.0, 1.0, 0.0, 1.0)
    ctx.empty_vertex_array.render(vertices=3)
    assert color.read(size=(2, 2)) == bytes(16)
    assert color.read(size=(2, 2), offset=(2, 2)) == b'\x00\xff\x00\xff' * 4
    with pytest.raises(TypeError):
        pipeline.viewport = (1, 2)
    ctx.release(program)
    ctx.release(pipeline)


def test_external_framebuffer_size(ctx):
    external = ctx.external_framebuffer(0, (4, 4))
    external.viewport = (1, 1, 2, 2)
    external.size = (8, 6)
    assert external.size == (8, 6)
    assert external.viewport == (0, 0, 8, 6)
    with pytest.raises(TypeError):
        external.size = (0, 6)
    with pytest.raises(TypeError, match='external'):
        ctx.framebuffer([ctx.texture((2, 2))]).size = (4, 4)


def test_read_into(ctx):
    texture = ctx.texture((2, 2), 'rgba8unorm', bytes(range(16)))
    memory = bytearray(16)
    texture.read_into(memory)
    assert memory == bytes(range(16))
    larger = bytearray(20)
    texture.read_into(larger, (1, 1), (1, 1))
    assert larger[:4] == bytes(range(12, 16))
    view = memoryview(larger)[4:8]
    texture.read_into(view, (1, 1))
    assert larger[4:8] == bytes(range(4))
    with pytest.raises(ValueError, match='16 bytes'):
        texture.read_into(bytearray(8))
    with pytest.raises(BufferError):
        texture.read_into(b'x' * 16)
    buffer = ctx.buffer(b'abcdefgh')
    memory = bytearray(8)
    buffer.read_into(memory)
    assert memory == b'abcdefgh'
    buffer.read_into(memory, 3, 4)
    assert memory[:3] == b'efg'
    with pytest.raises(ValueError):
        buffer.read_into(bytearray(2))


def test_flip_y(ctx):
    src = ctx.texture((1, 2), 'rgba8unorm', bytes([1, 1, 1, 1, 2, 2, 2, 2]))
    dst = ctx.texture((1, 2), 'rgba8unorm')
    ctx.copy_texture(src, dst, flip_y=True)
    assert dst.read() == bytes([2, 2, 2, 2, 1, 1, 1, 1])
    msaa = ctx.texture((1, 2), 'rgba8unorm', samples=4)
    with pytest.raises(ValueError):
        ctx.copy_texture(msaa, dst, flip_y=True)


def test_bgra(ctx):
    bgra = ctx.texture((1, 1), 'bgra8unorm', bytes([0, 0, 255, 255]))
    assert bgra.read() == bytes([0, 0, 255, 255])
    rgba = ctx.texture((1, 1), 'rgba8unorm')
    ctx.copy_texture(bgra, rgba)
    assert rgba.read() == bytes([255, 0, 0, 255])
    ctx.copy_texture(ctx.texture((1, 1), 'rgba8unorm', bytes([0, 255, 128, 255])), bgra)
    assert bgra.read() == bytes([128, 255, 0, 255])


def test_bound_objects(ctx):
    ctx.reset_state()
    assert ctx.bound_framebuffer is ctx.screen
    assert ctx.bound_program is None
    assert ctx.bound_vertex_array is None
    with pytest.raises(RuntimeError, match='program.use'):
        empty(ctx).render(vertices=3)
    program = ctx.program('fullscreen.vert', 'color.frag', uniforms={'color': 'vec4'})
    assert ctx.bound_program is None
    color = ctx.texture((2, 2), 'rgba8unorm')
    other = ctx.texture((2, 2), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    other_fbo = ctx.framebuffer([other])
    fbo.use()
    program.use()
    program['color'] = (1.0, 0.0, 0.0, 1.0)
    assert ctx.bound_framebuffer is fbo
    assert ctx.bound_program is program
    other_fbo.clear()
    ctx.copy_texture(color, other)
    ctx.texture((2, 2)).read()
    ctx.buffer(size=4, kind='index_buffer').write(b'abcd')
    ctx.framebuffer([ctx.texture((2, 2))])
    ctx.vertex_array([])
    ctx.program('fullscreen.vert', 'color.frag')
    vao = ctx.vertex_array([], vertices=3)
    vao.render()
    assert ctx.bound_vertex_array is vao
    assert ctx.bound_program is program
    assert ctx.bound_framebuffer is fbo
    assert color.read() == b'\xff\x00\x00\xff' * 4
    assert other.read() == bytes(16)
    vao.use()
    assert ctx.bound_vertex_array is vao
    ctx.release(program)
    assert ctx.bound_program is None
    ctx.release(fbo)
    assert ctx.bound_framebuffer is ctx.screen
    ctx.release(vao)
    assert ctx.bound_vertex_array is None


def test_vertex_array_mode(ctx):
    vao = ctx.vertex_array([], vertices=3, mode='points')
    assert vao.mode == 'points'
    vao.mode = 'triangles'
    assert vao.mode == 'triangles'
    with pytest.raises(ValueError):
        vao.mode = 'quads'
    with pytest.raises(ValueError):
        ctx.vertex_array([], mode='quads')


def test_release_all(ctx):
    texture = ctx.texture((4, 4))
    framebuffer = ctx.framebuffer([texture])
    buffer = ctx.buffer(size=16)
    ctx.release('all')
    assert not GL.glIsTexture(texture.glo)
    assert not GL.glIsFramebuffer(framebuffer.glo)
    assert not GL.glIsBuffer(buffer.glo)
    ctx.texture((4, 4)).read()


def test_unsupported_uniform_type(ctx):
    ctx.shaders['double.frag'] = '''
        #version 330 core
        #extension GL_ARB_gpu_shader_fp64 : require
        uniform double value;
        layout (location = 0) out vec4 out_color;
        void main() {
            out_color = vec4(float(value));
        }
    '''
    with pytest.raises(ValueError, match='unsupported type'):
        ctx.program('fullscreen.vert', 'double.frag')


def test_link_error_message(ctx):
    ctx.shaders['link.vert'] = '''
        #version 330 core
        out vec4 v_missing;
        void main() {
            gl_Position = vec4(0.0);
            v_missing = vec4(0.0);
        }
    '''
    ctx.shaders['link.frag'] = '''
        #version 330 core
        in vec3 v_missing;
        layout (location = 0) out vec4 out_color;
        void main() {
            out_color = vec4(v_missing, 1.0);
        }
    '''
    with pytest.raises(ValueError) as info:
        ctx.program('link.vert', 'link.frag')
    assert 'Linker Error' in str(info.value) and '\\n' not in str(info.value)


def test_pipeline_restores_viewport(ctx):
    ctx.shaders['white.frag'] = '''
        #version 330 core
        layout (location = 0) out vec4 out_color;
        void main() {
            out_color = vec4(1.0);
        }
    '''
    color = ctx.texture((4, 4), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    pipeline = ctx.pipeline('fullscreen.vert', 'white.frag', vertex_array=ctx.empty_vertex_array, vertices=3, framebuffer=fbo, viewport=(0, 0, 2, 2))
    pipeline.run()
    assert color.read((1, 1), (3, 3)) == b'\x00\x00\x00\x00'
    pipeline.program.use()
    ctx.empty_vertex_array.render(vertices=3)
    assert color.read() == b'\xff' * 64


def test_other_thread():
    import subprocess
    script = '''
import threading
import moderngl

headless = moderngl.headless_context()
ctx = moderngl.get_context()
buffer = ctx.buffer(size=4)
buffer.write(b'abcd')
print('owner thread ok', flush=True)

def worker():
    buffer.write(b'efgh')

thread = threading.Thread(target=worker)
thread.start()
thread.join()
print('unreachable', flush=True)
'''
    res = subprocess.run([sys.executable, '-c', script], capture_output=True, text=True)
    assert res.returncode != 0
    assert 'owner thread ok' in res.stdout
    assert 'unreachable' not in res.stdout
    assert 'OpenGL called from a thread without a moderngl context' in res.stderr
    assert 'in worker' in res.stderr
