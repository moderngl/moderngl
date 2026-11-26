import os
import shutil
import struct
import subprocess
import tempfile

import moderngl
import pytest


class VulkanLoader:
    backend = 'vulkan'


def spirv(source, stage):
    with tempfile.TemporaryDirectory() as tmp:
        path = os.path.join(tmp, 'shader.' + stage)
        open(path, 'w').write(source)
        subprocess.run(['glslangValidator', '-V', '--target-env', 'vulkan1.3', path, '-o', path + '.spv'], check=True, capture_output=True)
        return open(path + '.spv', 'rb').read()


FULLSCREEN_VERT = '''
    #version 450
    vec2 positions[3] = vec2[](vec2(-1.0, -1.0), vec2(3.0, -1.0), vec2(-1.0, 3.0));
    void main() {
        gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    }
'''

COLOR_FRAG = '''
    #version 450
    layout (set = 0, binding = 0) uniform Color {
        vec4 color;
    };
    layout (location = 0) out vec4 out_color;
    void main() {
        out_color = color;
    }
'''

VERTEX_VERT = '''
    #version 450
    layout (location = 0) in vec3 in_vertex;
    layout (location = 1) in vec4 in_color;
    layout (location = 0) out vec4 v_color;
    void main() {
        gl_Position = vec4(in_vertex, 1.0);
        v_color = in_color;
    }
'''

VERTEX_FRAG = '''
    #version 450
    layout (location = 0) in vec4 v_color;
    layout (location = 0) out vec4 out_color;
    void main() {
        out_color = v_color;
    }
'''

TEXTURE_FRAG = '''
    #version 450
    layout (set = 0, binding = 65) uniform sampler2D Texture;
    layout (location = 0) out vec4 out_color;
    void main() {
        out_color = texelFetch(Texture, ivec2(gl_FragCoord.xy), 0) * 0.5;
    }
'''

DOUBLE_COMP = '''
    #version 450
    layout (local_size_x = 4) in;
    layout (set = 0, binding = 32) buffer Values {
        float values[];
    };
    layout (set = 0, binding = 0) uniform Offset {
        float offset;
    };
    void main() {
        values[gl_GlobalInvocationID.x] = values[gl_GlobalInvocationID.x] * 2.0 + offset;
    }
'''

IMAGE_COMP = '''
    #version 450
    layout (local_size_x = 2, local_size_y = 2) in;
    layout (set = 0, binding = 128, rgba8) uniform writeonly image2D Output;
    layout (set = 0, binding = 129, r32ui) uniform uimage2DArray Counters;
    void main() {
        ivec2 at = ivec2(gl_GlobalInvocationID.xy);
        imageStore(Output, at, vec4(vec2(at) / 255.0, 0.0, 1.0));
        imageAtomicAdd(Counters, ivec3(at, 1), uint(at.x + 1));
    }
'''

HITS_FRAG = '''
    #version 450
    layout (set = 0, binding = 128, r32ui) uniform uimage2D Hits;
    layout (location = 0) out vec4 out_color;
    void main() {
        imageAtomicAdd(Hits, ivec2(gl_FragCoord.xy) / 2, 1u);
        out_color = vec4(1.0);
    }
'''

INSTANCE_VERT = '''
    #version 450
    vec2 positions[3] = vec2[](vec2(0.0, 0.0), vec2(2.0, 0.0), vec2(0.0, 2.0));
    layout (location = 0) flat out int v_instance;
    void main() {
        gl_Position = vec4(positions[gl_VertexIndex] - 1.0, 0.0, 1.0);
        v_instance = gl_InstanceIndex;
    }
'''

INSTANCE_FRAG = '''
    #version 450
    layout (location = 0) flat in int v_instance;
    layout (location = 0) out uvec4 out_color;
    void main() {
        out_color = uvec4(v_instance + 1, 0, 0, 0);
    }
'''


@pytest.fixture(scope='module')
def ctx(request):
    if any(not item.nodeid.startswith('tests/test_vulkan.py') for item in request.session.items):
        pytest.skip('run alone: pytest tests/test_vulkan.py')
    if not shutil.which('glslangValidator'):
        pytest.skip('glslangValidator not found')
    try:
        moderngl.init(VulkanLoader())
    except RuntimeError as error:
        pytest.skip(str(error))
    ctx = moderngl.get_context()
    for name, source in [
        ('fullscreen.vert', FULLSCREEN_VERT),
        ('color.frag', COLOR_FRAG),
        ('vertex.vert', VERTEX_VERT),
        ('vertex.frag', VERTEX_FRAG),
        ('texture.frag', TEXTURE_FRAG),
        ('double.comp', DOUBLE_COMP),
        ('instance.vert', INSTANCE_VERT),
        ('instance.frag', INSTANCE_FRAG),
        ('image.comp', IMAGE_COMP),
        ('hits.frag', HITS_FRAG),
    ]:
        ctx.shaders[name] = spirv(source, name.split('.')[1])
    return ctx


def color_pipeline(ctx, fbo, color, **kwargs):
    ubo = ctx.buffer(struct.pack('4f', *color))
    return ctx.pipeline(
        'fullscreen.vert',
        'color.frag',
        vertex_array=ctx.vertex_array([]),
        framebuffer=fbo,
        resources=ctx.resources(uniform_buffers={0: ubo}),
        vertices=3,
        **kwargs,
    )


def test_info(ctx):
    assert ctx.info['backend'] == 'vulkan'
    assert ctx.info['compute'] and ctx.info['first_instance'] and ctx.info['draw_indirect']
    assert ctx.info['max_samplers'] == 16
    assert ctx.info['storage_images'] and ctx.info['max_storage_images'] == 8


def test_clear(ctx):
    color = ctx.texture((4, 4), 'rgba8unorm')
    depth = ctx.texture((4, 4), 'depth24plus-stencil8')
    fbo = ctx.framebuffer([color, depth])
    fbo.clear_color = (1.0, 0.0, 0.0, 1.0)
    fbo.clear()
    assert color.read().wait() == bytes([255, 0, 0, 255]) * 16
    uint = ctx.texture((2, 2), 'r32uint')
    ifbo = ctx.framebuffer([uint])
    ifbo.clear_color = 7
    ifbo.clear()
    assert uint.read().wait() == struct.pack('4I', 7, 7, 7, 7)


def test_uniform_buffer(ctx):
    color = ctx.texture((4, 4), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    fbo.clear()
    color_pipeline(ctx, fbo, (0.0, 1.0, 0.0, 1.0)).run()
    assert color.read().wait() == bytes([0, 255, 0, 255]) * 16


def test_vertex_and_index_buffers(ctx):
    color = ctx.texture((4, 4), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    vertices = ctx.buffer(struct.pack('12f', -1, -1, 0, 1, -1, 0, -1, 1, 0, 1, 1, 0))
    colors = ctx.buffer(bytes([0, 0, 255, 255]) * 4)
    for short_index, fmt in [(True, '6H'), (False, '6I')]:
        indices = ctx.buffer(struct.pack(fmt, 0, 1, 2, 2, 1, 3), index=True)
        vao = ctx.vertex_array(vertices.bind('3f', 0) + colors.bind('4nu1', 1), indices, short_index=short_index)
        fbo.clear()
        ctx.pipeline('vertex.vert', 'vertex.frag', vertex_array=vao, framebuffer=fbo).run()
        assert color.read().wait() == bytes([0, 0, 255, 255]) * 16


def test_triangle_orientation(ctx):
    color = ctx.texture((4, 4), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    vertices = ctx.buffer(struct.pack('9f', -1, -1, 0, 1, -1, 0, -1, 1, 0))
    colors = ctx.buffer(bytes([255, 255, 255, 255]) * 3)
    vao = ctx.vertex_array(vertices.bind('3f', 0) + colors.bind('4nu1', 1))
    fbo.clear()
    ctx.pipeline('vertex.vert', 'vertex.frag', vertex_array=vao, framebuffer=fbo, cull_face='back').run()
    data = color.read().wait()
    assert data[:4] == bytes([255, 255, 255, 255])
    assert data[-4:] == bytes(4)
    fbo.clear()
    ctx.pipeline('vertex.vert', 'vertex.frag', vertex_array=vao, framebuffer=fbo, cull_face='front').run()
    assert color.read().wait() == bytes(64)


def test_texture_sampling(ctx):
    pixels = bytes(range(64))
    source = ctx.texture((4, 4), 'rgba8unorm', pixels)
    color = ctx.texture((4, 4), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    fbo.clear()
    ctx.pipeline(
        'fullscreen.vert',
        'texture.frag',
        vertex_array=ctx.vertex_array([]),
        framebuffer=fbo,
        resources=ctx.resources(samplers={1: ctx.sampler(source)}),
        vertices=3,
    ).run()
    assert all(abs(a - b // 2) <= 1 for a, b in zip(color.read().wait(), pixels))


def test_depth_and_stencil(ctx):
    color = ctx.texture((4, 4), 'rgba8unorm')
    depth = ctx.texture((4, 4), 'depth24plus-stencil8')
    fbo = ctx.framebuffer([color, depth])
    near = ctx.buffer(struct.pack('12f', -1, -1, -0.5, 1, -1, -0.5, -1, 1, -0.5, 1, 1, -0.5))
    far = ctx.buffer(struct.pack('12f', -1, -1, 0.5, 1, -1, 0.5, -1, 1, 0.5, 1, 1, 0.5))
    red = ctx.buffer(bytes([255, 0, 0, 255]) * 4)
    green = ctx.buffer(bytes([0, 255, 0, 255]) * 4)
    indices = ctx.buffer(struct.pack('6I', 0, 1, 2, 2, 1, 3), index=True)
    fbo.clear()
    for vbo, cbo in [(near, red), (far, green)]:
        vao = ctx.vertex_array(vbo.bind('3f', 0) + cbo.bind('4nu1', 1), indices)
        ctx.pipeline('vertex.vert', 'vertex.frag', vertex_array=vao, framebuffer=fbo, depth_test=True).run()
    assert color.read().wait() == bytes([255, 0, 0, 255]) * 16
    fbo.clear()
    color_pipeline(ctx, fbo, (1.0, 1.0, 1.0, 1.0), viewport=(0, 0, 2, 4), stencil={'func': 'always', 'ref': 1, 'depth_pass': 'replace'}).run()
    color_pipeline(ctx, fbo, (0.0, 0.0, 1.0, 1.0), stencil={'func': 'notequal', 'ref': 1}).run()
    row = color.read().wait()[:16]
    assert row == bytes([255, 255, 255, 255]) * 2 + bytes([0, 0, 255, 255]) * 2


def test_blending(ctx):
    color = ctx.texture((2, 2), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    fbo.clear()
    pipeline = color_pipeline(ctx, fbo, (0.25, 0.0, 0.0, 1.0), blend={'src': 'one', 'dst': 'one'})
    pipeline.run()
    pipeline.run()
    assert color.read().wait()[0] in (127, 128)


def test_multisampled_resolve(ctx):
    multisampled = ctx.texture((8, 8), 'rgba8unorm', multisampled=True)
    depth = ctx.texture((8, 8), 'depth24plus', multisampled=True)
    resolved = ctx.texture((8, 8), 'rgba8unorm')
    fbo = ctx.framebuffer([multisampled, depth])
    fbo.clear()
    color_pipeline(ctx, fbo, (1.0, 0.0, 1.0, 1.0), depth_test=True).run()
    ctx.copy_texture(multisampled, resolved)
    assert resolved.read().wait() == bytes([255, 0, 255, 255]) * 64


def test_compute(ctx):
    values = ctx.buffer(struct.pack('8f', *range(8)))
    offset = ctx.buffer(struct.pack('4f', 0.5, 0.0, 0.0, 0.0))
    compute = ctx.compute('double.comp', resources=ctx.resources(storage_buffers={0: values}, uniform_buffers={0: offset}), group_count=(2, 1, 1))
    compute.run()
    ctx.memory_barrier()
    assert struct.unpack('8f', values.read().wait()) == tuple(x * 2.0 + 0.5 for x in range(8))


def test_instances_and_indirect(ctx):
    color = ctx.texture((2, 2), 'r32uint')
    fbo = ctx.framebuffer([color])
    vao = ctx.vertex_array([])
    pipeline = ctx.pipeline('instance.vert', 'instance.frag', vertex_array=vao, framebuffer=fbo, vertices=3, first_instance=4)
    fbo.clear()
    pipeline.run()
    assert struct.unpack('4I', color.read().wait())[0] == 5
    commands = ctx.buffer(struct.pack('4I', 3, 1, 0, 9))
    fbo.clear()
    pipeline.use()
    vao.draw_indirect(commands)
    assert struct.unpack('4I', color.read().wait())[0] == 10


def test_layers_and_mipmaps(ctx):
    array = ctx.texture((2, 2), 'rgba8unorm', bytes(range(32)), array=2)
    assert array.read(layer=1).wait() == bytes(range(16, 32))
    cube = ctx.texture((2, 2), 'rgba8unorm', bytes(range(96)), cubemap=True)
    assert cube.read(layer=5).wait() == bytes(range(80, 96))
    mipmapped = ctx.texture((4, 4), 'rgba8unorm', bytes([200, 100, 50, 255]) * 16, levels=3)
    mipmapped.mipmaps()
    assert mipmapped.read(level=2).wait() == bytes([200, 100, 50, 255])


def test_copies(ctx):
    a = ctx.buffer(bytes(range(16)))
    b = ctx.buffer(size=16)
    ctx.copy_buffer(a, b, size=8, dst_offset=8)
    assert b.read().wait() == bytes(8) + bytes(range(8))
    texture = ctx.texture((2, 2), 'rgba8unorm')
    ctx.copy_buffer_to_texture(a, texture)
    assert texture.read().wait() == bytes(range(16))
    c = ctx.buffer(size=16)
    ctx.copy_texture_to_buffer(texture, c)
    assert c.read().wait() == bytes(range(16))
    flipped = ctx.texture((2, 2), 'rgba8unorm')
    ctx.copy_texture(texture, flipped, flip_y=True)
    assert flipped.read().wait() == bytes(range(8, 16)) + bytes(range(8))
    b.write(b'abcd', offset=4)
    assert b.read(size=8).wait() == bytes(4) + b'abcd'


def test_errors(ctx):
    ctx.shaders['glsl.frag'] = '#version 450\nvoid main() {}\n'
    with pytest.raises(ValueError, match='SPIR-V'):
        ctx.program('fullscreen.vert', 'glsl.frag')
    ctx.shaders['glsl_bytes.frag'] = b'#version 450\nvoid main() {}\n'
    with pytest.raises(ValueError, match='SPIR-V'):
        ctx.program('fullscreen.vert', 'glsl_bytes.frag')
    with pytest.raises(RuntimeError, match='uniforms are not supported'):
        ctx.program('fullscreen.vert', 'color.frag', uniforms={'color': 'vec4'})
    with pytest.raises(RuntimeError, match='debug is not supported'):
        ctx.debug()


def test_proc_address(ctx):
    import ctypes
    handles = (ctypes.c_uint64 * 5)()
    ctypes.CFUNCTYPE(None, ctypes.c_void_p)(moderngl.get_proc_address('vkglHandles'))(handles)
    assert all(handles[:4])
    image = ctypes.CFUNCTYPE(ctypes.c_uint64, ctypes.c_uint32)(moderngl.get_proc_address('vkglImage'))
    assert image(ctx.texture((2, 2), 'rgba8unorm').glo)
    assert moderngl.get_proc_address('vkGetInstanceProcAddr')
    assert moderngl.get_proc_address('glDrawArraysInstanced')
    assert not moderngl.get_proc_address('glUniform1fv')
    assert not moderngl.get_proc_address('glMissing')


def test_scissor(ctx):
    color = ctx.texture((4, 4), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    pipeline = color_pipeline(ctx, fbo, (1.0, 0.0, 0.0, 1.0), scissor=(2, 2, 2, 2))
    fbo.clear()
    pipeline.run()
    assert color.read(size=(2, 2)).wait() == bytes(16)
    assert color.read(size=(2, 2), offset=(2, 2)).wait() == bytes([255, 0, 0, 255]) * 4
    pipeline.scissor = (-8, -8, 10, 9)
    fbo.clear()
    pipeline.run()
    assert color.read(size=(2, 1)).wait() == bytes([255, 0, 0, 255]) * 2
    assert color.read(size=(2, 1), offset=(2, 0)).wait() == bytes(8)
    pipeline.scissor = None
    fbo.clear()
    pipeline.run()
    assert color.read().wait() == bytes([255, 0, 0, 255]) * 16


def test_polygon_offset(ctx):
    color = ctx.texture((2, 2), 'rgba8unorm')
    fbo = ctx.framebuffer([color, ctx.texture((2, 2), 'depth24plus')])
    vertices = ctx.buffer(struct.pack('12f', -1, -1, 0, 1, -1, 0, -1, 1, 0, 1, 1, 0))
    indices = ctx.buffer(struct.pack('6I', 0, 1, 2, 2, 1, 3), index=True)
    red = ctx.buffer(bytes([255, 0, 0, 255]) * 4)
    green = ctx.buffer(bytes([0, 255, 0, 255]) * 4)
    fbo.clear()
    for cbo, offset in [(red, None), (green, None), (green, (0.0, -4.0))]:
        vao = ctx.vertex_array(vertices.bind('3f', 0) + cbo.bind('4nu1', 1), indices)
        ctx.pipeline('vertex.vert', 'vertex.frag', vertex_array=vao, framebuffer=fbo, depth_test=True, polygon_offset=offset).run()
        expected = bytes([0, 255, 0, 255]) if offset else bytes([255, 0, 0, 255])
        assert color.read().wait() == expected * 4


def test_primitive_restart(ctx):
    positions = [(-1, -1), (-0.5, -1), (-1, 1), (-0.5, 1), (0.5, -1), (1, -1), (0.5, 1), (1, 1)]
    data = bytearray(65536 * 12)
    for i, (x, y) in enumerate(positions):
        data[i * 12:i * 12 + 12] = struct.pack('3f', x, y, 0)
    data[-12:] = struct.pack('3f', 0.25, -1, 0)
    vertices = ctx.buffer(data)
    colors = ctx.buffer(bytes([255]) * 4 * 65536)
    color = ctx.texture((4, 1), 'rgba8unorm')
    fbo = ctx.framebuffer([color])
    for short_index, fmt, restart in [(True, '9H', 0xFFFF), (False, '9I', 0xFFFFFFFF)]:
        index = ctx.buffer(struct.pack(fmt, 0, 1, 2, 3, restart, 4, 5, 6, 7), index=True)
        vao = ctx.vertex_array(vertices.bind('3f', 0) + colors.bind('4nu1', 1), index, short_index=short_index, mode='triangle_strip')
        fbo.clear()
        ctx.pipeline('vertex.vert', 'vertex.frag', vertex_array=vao, framebuffer=fbo).run()
        assert color.read().wait() == b'\xff' * 4 + bytes(8) + b'\xff' * 4


def test_storage_images(ctx):
    output = ctx.texture((4, 4), 'rgba8unorm', levels=2)
    counters = ctx.texture((4, 4), 'r32uint', bytes(128), array=2)
    resources = ctx.resources(storage_images={0: output.storage_image(level=1), 1: counters})
    compute = ctx.compute('image.comp', resources=resources, group_count=(1, 1, 1))
    compute.run()
    compute.run()
    ctx.memory_barrier()
    assert output.read(level=1).wait() == bytes([0, 0, 0, 255, 1, 0, 0, 255, 0, 1, 0, 255, 1, 1, 0, 255])
    assert counters.read(layer=0).wait() == bytes(64)
    assert struct.unpack('16I', counters.read(layer=1).wait())[:6] == (2, 4, 0, 0, 2, 4)


def test_storage_images_fragment(ctx):
    hits = ctx.texture((2, 2), 'r32uint', bytes(16))
    target = ctx.texture((4, 4), 'rgba8unorm')
    ctx.pipeline(
        'fullscreen.vert',
        'hits.frag',
        vertex_array=ctx.vertex_array([]),
        framebuffer=ctx.framebuffer([target]),
        resources=ctx.resources(storage_images={0: hits}),
        vertices=3,
    ).run()
    ctx.memory_barrier()
    assert struct.unpack('4I', hits.read().wait()) == (4, 4, 4, 4)
