

def test_init_from_texture_and_params(ctx):
    params_tex = dict(
        size=(5, 10),
        components=4,
        samples=0,
        dtype="f1"
    )
    tex = ctx.texture(params_tex["size"], params_tex["components"])
    ext = ctx.external_texture(**{**params_tex, **dict(glo=tex.glo)})
    assert ext.size == params_tex["size"]
    assert ext.width == params_tex["size"][0]
    assert ext.height == params_tex["size"][1]
    assert ext.components == params_tex["components"]
    assert ext.glo == tex.glo
    assert ext.dtype == params_tex["dtype"]
    assert ext.samples == params_tex["samples"]
    assert ext.repeat_x is True
    assert ext.repeat_y is True
    assert ext.filter == (ctx.LINEAR, ctx.LINEAR)
    assert ext.swizzle == "RGBA"
    assert ext.depth is False
    assert ext.anisotropy == 0.0
    assert ext.mglo != tex.mglo
    assert ext.ctx == ctx
    assert ext.extra == None
