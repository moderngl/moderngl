# WebGL Water
# https://madebyevan.com/webgl-water/
# Copyright 2011 Evan Wallace
# Released under the MIT license

import random
import math
import moderngl
import moderngl_window as mglw
from light_gl import *
from light_raytracer import *
from water import *
from renderer import *
from enum import IntEnum


class EventMode(IntEnum):
    NoEvent = 0,
    MoveCamera = 1,
    MoveSpahere = 2,
    AddDrop = 3,


class WaterMain(mglw.WindowConfig):
    """ coordinate
      | y
      |
      |       x
      +--------
     /
    /z
    """
    title = "ModernGL Water"
    gl_version = (3, 3)
    window_size = (1280, 720)
    aspect_ratio = 16 / 9
    resizable = True

    @property
    def angle_y(self):
        return self._angle_y

    @angle_y.setter
    def angle_y(self, value: float):
        self._angle_y = min(89.99, max(-89.99, value))

    @property
    def eye(self) -> glm.vec3:
        rad = glm.vec2(self.angle_x, self.angle_y) / 180 * math.pi
        cos_y = math.cos(rad[1])
        return glm.normalize(glm.vec3(math.cos(rad[0])*cos_y, math.sin(rad[1]), math.sin(rad[0])*cos_y)) * self.eye_distance

    def dbg_tracer_mesh(ctx: moderngl.Context, tracer: RayTracer, ray: glm.vec3) -> Mesh:
        a = tracer.eye + tracer.ray00 * 0.01
        b = tracer.eye + tracer.ray01 * 0.01
        c = tracer.eye + tracer.ray11 * 0.01
        d = tracer.eye + tracer.ray10 * 0.01
        return MeshBuilder.seg_lines(tracer.eye, a,
                                     tracer.eye, b,
                                     tracer.eye, c,
                                     tracer.eye, d,
                                     tracer.eye, ray,
                                     a, b, b, c, c, d, d, a,
                                     ).build(ctx)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.lines_shader = Shader("""
            void main() {
                gl_Position = ftransform();
            }
            """, """
            void main() {
                gl_FragColor = vec4(0.3, 0.7, 0.4, 1.0);
            }
            """, self.ctx)
        self.coord_y = MeshBuilder.seg_lines(
            glm.vec3(0.0, 0.0, 0.0), glm.vec3(0.0, 1.0, 0.0)).build(self.ctx)

        self.eye_distance = 3.0
        self.angle_x = 30
        self.angle_y = 30

        self.matrices = Matrices(glm.translate((0.0, 0.5, 0.0)), glm.lookAt(self.eye, (0.0, 0.0, 0.0), (0.0, 1.0, 0.0)), glm.perspective(
            45, self.aspect_ratio, 0.01, 100))
        self.renderer = Renderer(self.ctx)
        self.water = Water(ctx=self.ctx)
        self.center = glm.vec3(-0.4, -0.75, 0.2)
        self.old_center = self.center
        self.velocity = glm.vec3(0.0)
        self.gravity = glm.vec3(0.0, -4.0, 0.0)
        self.radius = 0.25
        self.use_gravity = False
        self.mode: EventMode = EventMode.NoEvent
        self.tracer: RayTracer = None
        self.tracer_mesh: Mesh = None

        for i in range(20):
            self.water.add_drop(random.random() * 2.0 - 1.0,
                                random.random() * 2.0 - 1.0, 0.03, 0.01 if i % 1 else -0.01)

    def add_drop(self, x: float, y: float):
        ray = self.tracer.get_ray_for_pixel(x, y)
        point_on_panel = self.tracer.eye + (ray * (-self.tracer.eye.y / ray.y))
        self.water.add_drop(point_on_panel.x, point_on_panel.z, 0.03, 0.01)

    def move_sphere(self, x: int, y: int):
        ray = self.tracer.get_ray_for_pixel(x, y)
        t = glm.dot(self.plane_normal, self.prev_hit -
                    self.tracer.eye) / glm.dot(self.plane_normal, ray)
        next_hit = self.tracer.eye + (ray * t)
        center = self.center + (next_hit - self.prev_hit)
        center.x = max(self.radius - 1, min(1 - self.radius, center.x))
        center.z = max(self.radius - 1, min(1 - self.radius, center.z))
        center.y = max(self.radius - 1, min(10, center.y))
        self.center = center
        self.prev_hit = next_hit

    def on_mouse_release_event(self, x: int, y: int, button: int):
        if button == 1:
            self.mode = EventMode.NoEvent
            self.tracer = None

    def on_mouse_press_event(self, x: int, y: int, button: int):
        self.tracer = RayTracer(self.ctx.viewport, self.matrices)
        ray = self.tracer.get_ray_for_pixel(x, y)
        sphere_hit_test: HitTest = RayTracer.hit_test_sphere(
            self.tracer.eye, ray, self.center, self.radius)
        point_on_panel = self.tracer.eye + (ray * (-self.tracer.eye.y / ray.y))
        if button == 1:
            if sphere_hit_test is not None:
                self.mode = EventMode.MoveSpahere
                self.prev_hit = sphere_hit_test.hit
                self.plane_normal = -self.tracer.get_ray_for_pixel(
                    self.ctx.viewport[2] / 2, self.ctx.viewport[3] / 2)
            elif abs(point_on_panel.x) < 1 and abs(point_on_panel.z) < 1:
                self.mode = EventMode.AddDrop
                self.add_drop(x, y)
            else:
                self.mode = EventMode.MoveCamera
        else:
            self.tracer_mesh = WaterMain.dbg_tracer_mesh(
                self.ctx, self.tracer, point_on_panel)

    def on_mouse_drag_event(self, x: int, y: int, dx: int, dy: int):
        match self.mode:
            case EventMode.AddDrop:
                self.add_drop(x, y)
            case EventMode.MoveCamera:
                self.angle_x += dx
                self.angle_y += dy
            case EventMode.MoveSpahere:
                self.move_sphere(x, y)

    def on_key_event(self, key, action, modifiers):
        if key == ord('G') and action == 1:
            self.use_gravity = not self.use_gravity

    def update(self, seconds):
        if seconds > 1:
            return
        if self.mode == EventMode.MoveSpahere:
            self.velocity = glm.vec3(0.0)
        elif self.use_gravity:
            precent_under_water = max(
                0, min(1, (self.radius - self.center.y) / (2*self.radius)))
            self.velocity = self.velocity + \
                (self.gravity * (seconds - 1.1 * seconds * precent_under_water))
            self.velocity = self.velocity - \
                (glm.normalize(self.velocity) * (precent_under_water *
                                                 seconds * glm.dot(self.velocity, self.velocity)))
            self.center = self.center + self.velocity * seconds
            if self.center.y < self.radius - 1:
                self.center.y = self.radius - 1
                self.velocity.y = abs(self.velocity.y) * 0.7

        self.water.move_sphere(self.old_center, self.center, self.radius)
        self.old_center = self.center

        self.water.step_simulation()
        self.water.step_simulation()
        self.water.update_normals()
        self.renderer.update_caustics(self.matrices, self.water)

    def on_render(self, time, frame_time):
        self.matrices.view = glm.lookAt(
            self.eye, (0.0, 0.0, 0.0), (0.0, 1.0, 0.0))

        self.update(frame_time)
        self.ctx.enable(moderngl.DEPTH_TEST)
        self.ctx.screen.use()
        self.renderer.sphere_center = self.center
        self.renderer.sphere_radius = self.radius
        self.renderer.render_cude(self.matrices, self.water)
        self.renderer.render_water(self.matrices, self.water)
        self.renderer.render_sphere(self.matrices, self.water)
        self.ctx.disable(moderngl.DEPTH_TEST)
        if self.tracer_mesh is not None:
            self.lines_shader.draw_mesh(
                self.tracer_mesh, self.matrices, mode=moderngl.LINES)


mglw.run_window_config(WaterMain, args=["--window", "glfw"])
