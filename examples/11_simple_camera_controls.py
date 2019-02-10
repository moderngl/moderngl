'''
    Added a simple camera to an existing example.

    Controls:
        Move:
            Forward - "w"
            Backwards - "s"
    
        Strafe:
            Up - left "shift", "up arrow"
            Down - left "control", "down arrow"
            Left - "a", "left arrow"
            Right - "d", "right arrow"

        Rotate:
            Left - "q"
            Right - "e"
        
        Zoom:
            In - numpad "+"
            Out - numpad "-"
    
    author: Alex Zakrividoroga
'''

import moderngl
import numpy as np
from pyrr import Matrix44, Vector3, vector, Quaternion
from example_window import Example, run_example

class Camera():
    
    def __init__(self, ratio):
        self._zoom_step = 0.1
        self._move_vertically = 0.1
        self._move_horizontally = 0.1
        self._rotate_horizontally = 0.1
        self._rotate_vertically = 0.1

        self._field_of_view_degrees = 60.0
        self._z_near = 0.1
        self._z_far = 100
        self._ratio = ratio
        self.build_projection()

        self._camera_position = Vector3 ([0.0, 0.0, -40.0])
        self._camera_front = Vector3 ([0.0, 0.0, 1.0])
        self._camera_up = Vector3 ([0.0, 1.0, 0.0])
        self._cameras_target = (self._camera_position + self._camera_front)
        self.build_look_at()

    def zoom_in(self):
        self._field_of_view_degrees = self._field_of_view_degrees - self._zoom_step
        self.build_projection()

    def zoom_out(self):
        self._field_of_view_degrees = self._field_of_view_degrees + self._zoom_step
        self.build_projection()

    def move_forward(self):
        self._camera_position = self._camera_position + self._camera_front * self._move_horizontally
        self.build_look_at()

    def move_backwards(self):
        self._camera_position = self._camera_position - self._camera_front * self._move_horizontally
        self.build_look_at()

    def strafe_left(self):
        self._camera_position = self._camera_position - vector.normalize(self._camera_front ^ self._camera_up) * self._move_horizontally
        self.build_look_at()

    def strafe_right(self):
        self._camera_position = self._camera_position + vector.normalize(self._camera_front ^ self._camera_up) * self._move_horizontally
        self.build_look_at()

    def strafe_up(self):
        self._camera_position = self._camera_position + self._camera_up * self._move_vertically
        self.build_look_at()

    def strafe_down(self):
        self._camera_position = self._camera_position - self._camera_up * self._move_vertically
        self.build_look_at()

    def rotate_left(self):
        rotation = Quaternion.from_y_rotation(2 * float (self._rotate_horizontally) * np.pi / 180)
        self._camera_front = rotation * self._camera_front
        self.build_look_at()

    def rotate_right(self):
        rotation = Quaternion.from_y_rotation(- 2 * float (self._rotate_horizontally) * np.pi / 180)
        self._camera_front = rotation * self._camera_front
        self.build_look_at()

    def mouse(self, pos_x, pos_y):
        pass

    def build_look_at(self):
        self._cameras_target = (self._camera_position + self._camera_front)   
        self.mat_lookat = Matrix44.look_at(
            self._camera_position,
            self._cameras_target,
            self._camera_up)

    def build_projection(self):
        self.mat_projection = Matrix44.perspective_projection(
            self._field_of_view_degrees,
            self._ratio,
            self._z_near,
            self._z_far)

class QTKeyDecoder():

    def __init__(self, keys):
        self._keys = keys

    def up(self):
        return self._keys[38]

    def down(self):
        return self._keys[40]

    def left(self):
        return self._keys[37]

    def right(self):
        return self._keys[39]

    def num_plus(self):
        return self._keys[107]

    def num_minus(self):
        return self._keys[109]

    def key_w(self):
        return self._keys[87]

    def key_s(self):
        return self._keys[83]

    def key_a(self):
        return self._keys[65]

    def key_d(self):
        return self._keys[68]

    def left_shift(self):
        return self._keys[16]

    def left_control(self):
        return self._keys[17]

    def key_e(self):
        return self._keys[69]

    def key_q(self):
        return self._keys[81]


def grid(size, steps):
    u = np.repeat(np.linspace(-size, size, steps), 2)
    v = np.tile([-size, size], steps)
    w = np.zeros(steps * 2)
    return np.concatenate([np.dstack([u, v, w]), np.dstack([v, u, w])])
  

class PerspectiveProjection(Example):
    def __init__(self):
        self.ctx = moderngl.create_context()

        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                uniform mat4 Mvp;

                in vec3 in_vert;

                void main() {
                    gl_Position = Mvp * vec4(in_vert, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330

                out vec4 f_color;

                void main() {
                    f_color = vec4(0.1, 0.1, 0.1, 1.0);
                }
            ''',
        )

        self.camera = Camera(self.wnd.ratio)

        self.mvp = self.prog['Mvp']

        self.vbo = self.ctx.buffer(grid(15, 10).astype('f4').tobytes())
        self.vao = self.ctx.simple_vertex_array(self.prog, self.vbo, 'in_vert')


    def input_update(self):
        #Keyboard Processing
        qt_keys = QTKeyDecoder(self.wnd.keys)
        
        if qt_keys.up() == True:
            self.camera.strafe_up()

        if qt_keys.down() == True:
            self.camera.strafe_down()

        if qt_keys.left() == True:
            self.camera.strafe_left()

        if qt_keys.right() == True:
            self.camera.strafe_right()

        if qt_keys.num_plus() == True:
           self.camera.zoom_in()

        if qt_keys.num_minus() == True:
            self.camera.zoom_out()

        if qt_keys.key_w() == True:
            self.camera.move_forward()

        if qt_keys.key_s() == True:
            self.camera.move_backwards()

        if qt_keys.key_a() == True:
            self.camera.strafe_left()

        if qt_keys.key_d() == True:
            self.camera.strafe_right()

        if qt_keys.left_shift() == True:
            self.camera.strafe_up()

        if qt_keys.left_control() == True:
            self.camera.strafe_down()

        if qt_keys.key_q() == True:
            self.camera.rotate_left()

        if qt_keys.key_e() == True:
            self.camera.rotate_right()

        #Mouse Processing
        self.camera.mouse(self.wnd.mouse[0], self.wnd.mouse[1])


    def render(self):
        self.input_update()
        
        self.ctx.viewport = self.wnd.viewport
        self.ctx.clear(1.0, 1.0, 1.0)
        self.ctx.enable(moderngl.DEPTH_TEST)

        self.mvp.write((self.camera.mat_projection * self.camera.mat_lookat).astype('f4').tobytes())
        self.vao.render(moderngl.LINES)


run_example(PerspectiveProjection)
