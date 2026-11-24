import js
import pyodide_js
from pyodide.ffi import create_proxy

import moderngl

OPENGL = 0x2
DOUBLEBUF = 0x40000000
RESIZABLE = 0x10
HIDDEN = 0x80

QUIT = 0x100
KEYDOWN = 0x300
KEYUP = 0x301
MOUSEMOTION = 0x400
MOUSEBUTTONDOWN = 0x401
MOUSEBUTTONUP = 0x402
MOUSEWHEEL = 0x403
VIDEORESIZE = 0x8000

GL_CONTEXT_MAJOR_VERSION = 17
GL_CONTEXT_MINOR_VERSION = 18
GL_CONTEXT_FLAGS = 19
GL_CONTEXT_PROFILE_MASK = 20
GL_CONTEXT_PROFILE_CORE = 1
GL_CONTEXT_FORWARD_COMPATIBLE_FLAG = 2
GL_CONTEXT_DEBUG_FLAG = 1

K_ESCAPE = 27
K_SPACE = 32
K_UP = 1073741906
K_DOWN = 1073741905
K_LEFT = 1073741904
K_RIGHT = 1073741903

KEYS = {'Escape': K_ESCAPE, ' ': K_SPACE, 'ArrowUp': K_UP, 'ArrowDown': K_DOWN, 'ArrowLeft': K_LEFT, 'ArrowRight': K_RIGHT}


class Event:
    def __init__(self, type, **kwargs):
        self.type = type
        self.__dict__.update(kwargs)


class Namespace:
    pass


state = Namespace()
state.size = (800, 600)
state.events = []
state.mouse = (0, 0)
state.buttons = [False, False, False]
state.start = js.performance.now()
state.listeners = []


def init():
    pass


def quit():
    pass


def listen(target, name, callback):
    proxy = create_proxy(callback)
    target.addEventListener(name, proxy)
    state.listeners.append(proxy)


def canvas_position(e):
    canvas = state.canvas
    return (int(e.offsetX * canvas.width / max(canvas.clientWidth, 1)), int(e.offsetY * canvas.height / max(canvas.clientHeight, 1)))


def on_mouse_move(e):
    pos = canvas_position(e)
    rel = (pos[0] - state.mouse[0], pos[1] - state.mouse[1])
    state.mouse = pos
    state.events.append(Event(MOUSEMOTION, pos=pos, rel=rel, buttons=tuple(int(x) for x in state.buttons)))


def on_mouse_down(e):
    if e.button < 3:
        state.buttons[e.button] = True
    state.events.append(Event(MOUSEBUTTONDOWN, pos=canvas_position(e), button=e.button + 1))


def on_mouse_up(e):
    if e.button < 3:
        state.buttons[e.button] = False
    state.events.append(Event(MOUSEBUTTONUP, pos=canvas_position(e), button=e.button + 1))


def on_wheel(e):
    state.events.append(Event(MOUSEWHEEL, x=0, y=-1 if e.deltaY > 0 else 1))


def on_key(kind):
    def callback(e):
        state.events.append(Event(kind, key=KEYS.get(e.key, ord(e.key[0].lower()) if len(e.key) == 1 else 0)))
    return callback


def set_mode(size, flags=0, vsync=False):
    canvas = js.document.getElementById('canvas')
    canvas.width, canvas.height = size
    state.size = tuple(size)
    state.canvas = canvas
    gl = canvas.getContext('webgl2', antialias=False, depth=True, stencil=True, alpha=True)
    symbols = js.window.eval(moderngl._extern_gl)(pyodide_js._module, gl)
    pyodide_js._module.mergeLibSymbols(symbols)
    listen(canvas, 'mousemove', on_mouse_move)
    listen(canvas, 'mousedown', on_mouse_down)
    listen(js.window, 'mouseup', on_mouse_up)
    listen(canvas, 'wheel', on_wheel)
    listen(js.window, 'keydown', on_key(KEYDOWN))
    listen(js.window, 'keyup', on_key(KEYUP))


display = Namespace()
display.init = init
display.set_mode = set_mode
display.gl_set_attribute = lambda attr, value: None
display.get_window_size = lambda: state.size
display.flip = lambda: None
display.set_caption = lambda title: None

event = Namespace()


def get_events():
    res = state.events
    state.events = []
    return res


event.get = get_events

mouse = Namespace()
mouse.get_pos = lambda: state.mouse
mouse.get_pressed = lambda num_buttons=3: tuple(state.buttons)


class Clock:
    def __init__(self):
        self.last = js.performance.now()
        self.fps = 0.0

    def tick(self, framerate=0):
        now = js.performance.now()
        ms = now - self.last
        self.last = now
        self.fps = 1000.0 / ms if ms else 0.0
        return int(ms)

    def get_fps(self):
        return self.fps


time = Namespace()
time.get_ticks = lambda: int(js.performance.now() - state.start)
time.Clock = Clock
