import os

import math
import moderngl_window as mglw


class Example(mglw.WindowConfig):
    title = "ModernGL Window"
    resizable = False
    gl_version = (3, 3)
    window_size = (800, 800)
    aspect_ratio = 1.0
    resource_dir = os.path.normpath(os.path.join(__file__, '../data'))

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

    def on_render(self, time: float, frame_time: float):
        self.ctx.clear(
            (math.sin(time) + 1.0) / 2,
            (math.sin(time + 2) + 1.0) / 2,
            (math.sin(time + 3) + 1.0) / 2,
        )

    def on_resize(self, width: int, height: int):
        """
        Pick window resizes in case we need yo update
        internal states when this happens.
        """
        print("Window resized to", width, height)

    def on_iconify(self, iconified: bool):
        """Window hide/minimize and restore"""
        print("Window was iconified:", iconified)

    def on_key_event(self, key, action, modifiers):
        keys = self.wnd.keys

        # Key presses
        if action == keys.ACTION_PRESS:
            if key == keys.SPACE:
                print("SPACE key was pressed")

            # Using modifiers (shift and ctrl)

            if key == keys.Z and modifiers.shift:
                print("Shift + Z was pressed")

            if key == keys.Z and modifiers.ctrl:
                print("ctrl + Z was pressed")

        # Key releases
        elif action == self.wnd.keys.ACTION_RELEASE:
            if key == keys.SPACE:
                print("SPACE key was released")

        if action == keys.ACTION_PRESS:
            # Move the window around with AWSD
            if key == keys.A:
                self.wnd.position = self.wnd.position[0] - 20, self.wnd.position[1]
            if key == keys.D:
                self.wnd.position = self.wnd.position[0] + 20, self.wnd.position[1]
            if key == keys.W:
                self.wnd.position = self.wnd.position[0], self.wnd.position[1] - 20
            if key == keys.S:
                self.wnd.position = self.wnd.position[0], self.wnd.position[1] + 20

            # Resize window around with Shift + AWSD
            if self.wnd.modifiers.shift and key == keys.A:
                self.wnd.size = self.wnd.size[0] - 50, self.wnd.size[1]
            if self.wnd.modifiers.shift and key == keys.D:
                self.wnd.size = self.wnd.size[0] + 50, self.wnd.size[1]
            if self.wnd.modifiers.shift and key == keys.W:
                self.wnd.size = self.wnd.size[0], self.wnd.size[1] - 50
            if self.wnd.modifiers.shift and key == keys.S:
                self.wnd.size = self.wnd.size[0], self.wnd.size[1] + 50

            # toggle cursor
            if key == keys.C:
                self.wnd.cursor = not self.wnd.cursor

            # Shuffle window title
            if key == keys.T:
                title = list(self.wnd.title)
                random.shuffle(title)
                self.wnd.title = ''.join(title)

            # Toggle mouse exclusivity
            if key == keys.M:
                self.wnd.mouse_exclusivity = not self.wnd.mouse_exclusivity

    def on_mouse_position_event(self, x, y, dx, dy):
        print("Mouse position pos={} {} delta={} {}".format(x, y, dx, dy))

    def on_mouse_drag_event(self, x, y, dx, dy):
        print("Mouse drag pos={} {} delta={} {}".format(x, y, dx, dy))

    def on_mouse_scroll_event(self, x_offset, y_offset):
        print("mouse_scroll_event", x_offset, y_offset)

    def on_mouse_press_event(self, x, y, button):
        print("Mouse button {} pressed at {}, {}".format(button, x, y))
        print("Mouse states:", self.wnd.mouse_states)

    def on_mouse_release_event(self, x: int, y: int, button: int):
        print("Mouse button {} released at {}, {}".format(button, x, y))
        print("Mouse states:", self.wnd.mouse_states)

    def on_unicode_char_entered(self, char):
        print("unicode_char_entered:", char)


if __name__ == '__main__':
    Example.run()
