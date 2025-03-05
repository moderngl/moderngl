import moderngl
from basic_colors_and_texture import ColorsAndTexture


class RenderToTexture(ColorsAndTexture):
    title = "Render to Texture"
    gl_version = (3, 3)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.texture1 = self.texture
        self.texture2 = self.ctx.texture(self.wnd.size, 3)
        depth_attachment = self.ctx.depth_renderbuffer(self.wnd.size)
        self.fbo = self.ctx.framebuffer(self.texture2, depth_attachment)

    def on_render(self, time, frame_time):
        for mode in ['render_to_texture', 'render_to_window']:
            if mode == 'render_to_texture':
                self.texture = self.texture1
                self.fbo.clear(1.0, 1.0, 1.0)
                self.fbo.use()
            else:
                self.texture = self.texture2
                self.ctx.screen.use()
            super().render(time, frame_time)


if __name__ == '__main__':
    RenderToTexture.run()
