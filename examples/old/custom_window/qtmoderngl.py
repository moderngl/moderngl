from PyQt5 import QtGui, QtWidgets, QtCore, QtOpenGL

import moderngl

# pylint: disable=E0202


class QModernGLWidget(QtOpenGL.QGLWidget):
    def __init__(self):
        fmt = QtOpenGL.QGLFormat()
        fmt.setVersion(3, 3)
        fmt.setProfile(QtOpenGL.QGLFormat.CoreProfile)
        fmt.setSampleBuffers(True)
        self.timer = QtCore.QElapsedTimer()
        super(QModernGLWidget, self).__init__(fmt, None)

    def initializeGL(self):
        pass

    def paintGL(self):
        self.ctx = moderngl.create_context()
        self.screen = self.ctx.detect_framebuffer()
        self.init()
        self.render()
        self.paintGL = self.render

    def init(self):
        pass

    def on_render(self):
        pass


class QModernGLWidgetOld(QtWidgets.QOpenGLWidget):
    def __init__(self):
        super(QModernGLWidgetOld, self).__init__()
        fmt = QtGui.QSurfaceFormat()
        fmt.setVersion(3, 3)
        fmt.setProfile(QtGui.QSurfaceFormat.CoreProfile)
        fmt.setDepthBufferSize(24)
        fmt.setSwapInterval(0)
        fmt.setSamples(8)
        self.setFormat(fmt)

    def initializeGL(self):
        pass

    def paintGL(self):
        self.ctx = moderngl.create_context()
        self.screen = self.ctx.detect_framebuffer(self.defaultFramebufferObject())
        self.init()
        self.render()
        self.paintGL = self.render

    def init(self):
        pass

    def on_render(self):
        pass
