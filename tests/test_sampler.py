import moderngl

def test_create(ctx):
    sampler = ctx.sampler()
    sampler.use(location=0)
    sampler.clear(location=0)

def test_defaults(ctx):
    sampler = ctx.sampler()
    assert sampler.anisotropy <= 1.0
    assert sampler.repeat_x is True
    assert sampler.repeat_y is True
    assert sampler.repeat_z is True
    assert sampler.filter == (moderngl.LINEAR, moderngl.LINEAR)
    assert sampler.compare_func == '?'
    assert sampler.border_color == (0.0, 0.0, 0.0, 0.0)
    assert sampler.min_lod == -1000.0
    assert sampler.max_lod == 1000.0

def test_prop_changes(ctx):
    sampler = ctx.sampler()
    # Change values
    sampler.anisotropy = ctx.max_anisotropy
    sampler.filter = (moderngl.NEAREST_MIPMAP_NEAREST, moderngl.NEAREST)
    sampler.compare_func = "<="

    assert sampler.anisotropy == ctx.max_anisotropy
    assert sampler.filter == (moderngl.NEAREST_MIPMAP_NEAREST, moderngl.NEAREST)
    assert sampler.compare_func == "<="

    # Ensure repeat parameters are set correctly
    sampler.repeat_x = False
    assert (sampler.repeat_x, sampler.repeat_y, sampler.repeat_z) == (False, True, True)
    sampler.repeat_y = False
    assert (sampler.repeat_x, sampler.repeat_y, sampler.repeat_z) == (False, False, True)
    sampler.repeat_z = False
    assert (sampler.repeat_x, sampler.repeat_y, sampler.repeat_z) == (False, False, False)

def test_border_color(ctx):
    sampler = ctx.sampler()

    # Ensure border color values are set correctly
    colors = [
        (1.0, 0.0, 0.0, 0.0),
        (0.0, 1.0, 0.0, 0.0),
        (0.0, 0.0, 1.0, 0.0),
        (0.0, 0.0, 0.0, 1.0),
    ]
    for color in colors:
        sampler.border_color = color
        assert sampler.border_color == color

def test_lod(ctx):
    sampler = ctx.sampler()

    sampler.min_lod = 0.0
    assert sampler.min_lod == 0.0

    sampler.max_lod = 500.0
    assert sampler.max_lod == 500.0

def test_clear_samplers(ctx):
    ctx.clear_samplers(start=0, end=5)
    ctx.clear_samplers(start=5, end=10)
    ctx.clear_samplers(start=10, end=100)
