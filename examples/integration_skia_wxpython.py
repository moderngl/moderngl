import wx
import skia
import moderngl
from wx import glcanvas

GL_RGBA8 = 0x8058


class DrawCanvas(glcanvas.GLCanvas):
    def __init__(self, parent, size):
        glcanvas.GLCanvas.__init__(self, parent, -1, size=size)

        self.size = None
        self.init = False
        self.ctx = None
        self.glcanvas = glcanvas.GLContext(self)
        self.size = (800, 800)

        self.x_pos = 0
        self.y_pos = 0

        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_PAINT, self.OnPaint)

    def OnEraseBackground(self, event):
        pass  # Do nothing, to avoid flashing on MSW.

    def OnSize(self, event):
        wx.CallAfter(self.DoSetViewport)
        event.Skip()

    def DoSetViewport(self):
        # Set the viewport size
        self.size = self.GetClientSize()
        self.SetCurrent(self.glcanvas)
        if not self.ctx is None:
            self.ctx.set_viewport(0, 0, self.Size.width, self.Size.height)

    def InitGL(self):
        # Initialize the skia context with the moderngl context
        self.ctx = moderngl.create_context()
        context = skia.GrDirectContext.MakeGL()
        backend_render_target = skia.GrBackendRenderTarget(
            self.size[0],
            self.size[1],
            0,  # sampleCnt
            0,  # stencilBits
            skia.GrGLFramebufferInfo(0, GL_RGBA8))
        self.surface = skia.Surface.MakeFromBackendRenderTarget(
            context, backend_render_target, skia.kBottomLeft_GrSurfaceOrigin,
            skia.kRGBA_8888_ColorType, skia.ColorSpace.MakeSRGB())
        self.canvas = self.surface.getCanvas()
        
    def OnPaint(self, event):
        dc = wx.PaintDC(self)
        self.SetCurrent(self.glcanvas)
        if not self.init:
            self.InitGL()
            self.init = True
        self.OnDraw()

    def OnDraw(self):
        self.SetContextViewport(0, 0, self.Size.width, self.Size.height)
        self.ctx.clear(255.0, 255.0, 255.0, 0.0)
        self.DrawContext()
        self.SwapBuffers()

    def DrawContext(self):
        # Do your drawing with skia here

        paint = skia.Paint(Color=skia.ColorGREEN) 
        paint.setAntiAlias(True)
        
        self.canvas.drawCircle(self.x_pos, self.y_pos, 40, paint)
        self.canvas.drawSimpleText("Hello ModernGL, Skia & wxPython!", 90, 200, 
                                skia.Font(skia.Typeface("Arial"), 40), 
                                skia.Paint(Color=skia.ColorBLUE))
        
        self.surface.flushAndSubmit()

    def SetContextViewport(self, x, y, width, height):
        self.ctx.viewport = (x, y, width, height)

    def SetXPos(self, x_pos):
        self.x_pos = x_pos
        self.Refresh(False)

    def SetYPos(self, y_pos):
        self.y_pos = y_pos
        self.Refresh(False)


class Frame(wx.Frame): 
    def __init__(self, parent, title): 
        super(Frame, self).__init__(parent, title=title, size=(900, 800))  

        self.SetBackgroundColour(wx.Colour("#eee"))

        main_sz = wx.BoxSizer(wx.HORIZONTAL)

        sz = wx.BoxSizer(wx.VERTICAL)
        panel = wx.Panel(self)
        panel.SetBackgroundColour(wx.Colour("#eee"))

        self.canvas = DrawCanvas(panel, size=(800, 800))

        self.slider_y = wx.Slider(self, 100, 25, 1, 800, size=(50, -1),
                            style=wx.SL_VERTICAL)
        self.slider_x = wx.Slider(panel, 100, 25, 1, 800, size=(50, -1), 
                                  style=wx.SL_HORIZONTAL)

        panel.SetSizer(sz)

        sz.Add(self.slider_x, 0, flag=wx.EXPAND|wx.ALL)
        sz.Add(self.canvas, 0, flag=wx.EXPAND|wx.ALL)

        main_sz.Add(self.slider_y, 0, flag=wx.EXPAND|wx.ALL)
        main_sz.Add(panel, 0, flag=wx.EXPAND|wx.ALL)

        self.SetSizer(main_sz)

        self.slider_x.Bind(wx.EVT_SLIDER, self.OnChangeX)
        self.slider_y.Bind(wx.EVT_SLIDER, self.OnChangeY)

        self.Center()

    def OnChangeX(self, event):
        self.canvas.SetXPos(self.slider_x.GetValue())
        event.Skip()

    def OnChangeY(self, event):
        self.canvas.SetYPos(self.slider_y.GetValue())
        event.Skip()
		
ex = wx.App() 
win = Frame(None, "ModernGL + Skia Python + wxPython") 
win.Show(True)
ex.MainLoop()
