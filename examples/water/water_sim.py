import random
import numpy as np
import moderngl
import moderngl_window as mglw
from light_gl import *
from water import *
from renderer import *


class WaterSimulation(mglw.WindowConfig):
    title = "WaterSimulation"
    gl_version = (3, 3)
    window_size = (1280, 720)
    aspect_ratio = 16 / 9
    resizable = True

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.matrix = Matrices()
        self.shader = Shader("""
                uniform sampler2D Heightmap;
                void main() {
                    vec2 coord = gl_Vertex.xy * 0.5 + 0.5;
                    float height = texture(Heightmap, coord).r * 0.5;
                    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xy, height, 1.0);
                }
            """, """
                uniform vec3 w_volor;
                void main() {
                    gl_FragColor = vec4(w_volor, 1.0);
                }
            """)
        self.shader.program["w_volor"] = glm.vec3(0.3, 0.5, 0.7)
        self.panel = MeshBuilder.panel(detail=64).build(self.ctx)

        self.water = Water()
        self._heavy = 5

    @property
    def heavy(self):
        return self._heavy

    @heavy.setter
    def heavy(self, value):
        self._heavy = min(10, max(0, value))

    def on_key_event(self, key, action, modifiers):
        if action == 1:
            if key == 265: # up arrow
                self.heavy += 1
            elif key == 264: # down arrow
                self.heavy -= 1

    def on_render(self, time, frame_time):
        angle = time * 0.2
        self.matrix.projection = glm.perspective(
            45.0, self.aspect_ratio, 0.1, 1000.0)
        self.matrix.view = glm.lookAt((np.cos(angle), np.sin(angle), 2.2),
                                      (0.0, 0.0, 0.0),
                                      (0.0, 0.0, 1.0),)

        if random.randint(0, 50) < self._heavy:
            strength = random.choice([1, -1]) * 0.4
            self.water.add_drop(random.random() * 2.0 - 1.0,
                                random.random() * 2.0 - 1.0, 0.04, strength)

        self.water.step_simulation()
        self.water.step_simulation()
        self.water.update_normals()

        self.ctx.enable(moderngl.DEPTH_TEST)
        self.ctx.screen.use()
        self.water.texture_a.use()
        self.shader.draw_mesh(self.panel, self.matrix, mode=moderngl.LINES)
        self.ctx.disable(moderngl.DEPTH_TEST)


if __name__ == '__main__':
    mglw.run_window_config(WaterSimulation, args=["--window", "glfw"])
