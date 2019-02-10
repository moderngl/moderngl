import time

import moderngl as mgl
import numpy as np
from PyQt5 import QtOpenGL, QtWidgets, QtCore
from PyQt5.QtGui import QSurfaceFormat

class WindowInfo:
    def __init__(self):
        self.size = (0, 0)
        self.mouse = (0, 0)
        self.wheel = 0
        self.time = 0
        self.ratio = 1.0
        self.viewport = (0, 0, 0, 0)
        self.keys = np.full(256, False)
        self.old_keys = np.copy(self.keys)

    def key_down(self, key):
        return self.keys[key]

    def key_pressed(self, key):
        return self.keys[key] and not self.old_keys[key]

    def key_released(self, key):
        return not self.keys[key] and self.old_keys[key]


class ExampleWindow(QtWidgets.QOpenGLWidget):
    def __init__(self, size, title):
        super(ExampleWindow, self).__init__()

        qformat = QSurfaceFormat()
        qformat.setVersion(3, 3)
        qformat.setProfile(QSurfaceFormat.CoreProfile)
        qformat.setSwapInterval(1)
        qformat.setSamples(16)
        qformat.setDepthBufferSize(24)
        self.setFormat(qformat)
        
        self.setFixedSize(size[0], size[1])
        self.move(QtWidgets.QDesktopWidget().rect().center() - self.rect().center())
        self.setWindowTitle(title)

        self.start_time = time.clock()
        self.example = lambda: None
        self.ex = None

        self.wnd = WindowInfo()
        self.wnd.viewport = (0, 0) + (size[0] * self.devicePixelRatio(), size[1] * self.devicePixelRatio())
        self.wnd.ratio = size[0] / size[1]
        self.wnd.size = size

    def keyPressEvent(self, event):
        # Quit when ESC is pressed
        if event.key() == QtCore.Qt.Key_Escape:
            QtCore.QCoreApplication.instance().quit()

        self.wnd.keys[event.nativeVirtualKey() & 0xFF] = True

    def keyReleaseEvent(self, event):
        self.wnd.keys[event.nativeVirtualKey() & 0xFF] = False

    def mouseMoveEvent(self, event):
        self.wnd.mouse = (event.x(), event.y())

    def wheelEvent(self, event):
        self.wnd.wheel += event.angleDelta().y()

    def paintGL(self):
        if self.ex is None:
            self.ex = self.example()
        self.wnd.time = time.clock() - self.start_time
        self.ex.render()
        self.wnd.old_keys = np.copy(self.wnd.keys)
        self.wnd.wheel = 0
        self.update()


def run_example(example):
    app = QtWidgets.QApplication([])
    widget = ExampleWindow(example.WINDOW_SIZE, getattr(example, 'WINDOW_TITLE', example.__name__))
    example.wnd = widget.wnd
    widget.example = example
    widget.show()
    app.exec_()
    del app


class Example:
    WINDOW_SIZE = (1280, 720)
    wnd = None  # type: WindowInfo

    def render(self):
        pass
