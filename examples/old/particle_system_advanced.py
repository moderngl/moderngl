from typing import Optional, Tuple, Final

import moderngl
import numpy as np

from _example import Example

NB_PARTICLES: Final[int] = 2 << 10  # 1024
POINT_SIZE: Final[int] = 2 << 3  # 16
# https://www.toutes-les-couleurs.com/code-couleur-rvb.php
CHAMPAGNE_COLOR: Final[Tuple[float, float, float, float]] = (251 / 255., 242 / 255., 183 / 255., 1.0)


class Particles(Example):
    title = "Particle System - Champagne Bubbles"
    gl_version = (3, 3)

    def __init__(
            self,
            shader_transform: Optional[moderngl.Program] = None,
            nb_max_particles: int = NB_PARTICLES,
            **kwargs
    ):
        super().__init__(**kwargs)

        self.prog = self.ctx.program(
            vertex_shader='''
                 #version 330
                 
                 in vec2 in_vert;
                 in float in_hot;

                 out float out_hot;
                 
                 void main() {
                     out_hot = in_hot;
                     gl_Position = vec4(in_vert.xy, 0.0, 1.0);
                 }
             ''',
            fragment_shader='''
                 #version 330
                 
                 uniform vec4 u_color_particle;

                 in float out_hot;

                 out vec4 f_color;

                 void main() {
                     float d_to_center = distance(gl_PointCoord.xy, vec2(0.5, 0.5));
                     if (d_to_center > out_hot*0.5)
                        discard;
                     f_color = vec4(1.0 - vec3(d_to_center), 1.0);

                     f_color *= u_color_particle;
                 }
             ''',
        )

        if shader_transform is None:
            self.transform = self.ctx.program(
                vertex_shader='''
                   #version 330
                   
                   #define M_PI 3.14159265358979323846264
                   // Golden Ratio
                   #define PHI  1.61803398874989484820459 * 00000.1
                   #define PI   M_PI * 00000.1
                   #define SQ2  1.41421356237309504880169 * 10000.0
                   // Gravity factor
                   #define G    0.980
                   // Rate of Decline (particle life)
                   #define ROD  0.006
                   
                   uniform vec2 u_acc;
                   uniform float u_time;

                   in vec2 in_pos;
                   in float in_hot;
                   in vec2 in_vel;
                   in float in_id;

                   out vec2 out_pos;
                   out float out_hot;
                   out vec2 out_vel;
                   out float out_id;
                   
                   // https://www.shadertoy.com/view/ltB3zD
                   float gold_noise(in vec2 coordinate, in float seed){
                       return fract(sin(dot(coordinate*(seed+PHI), vec2(PHI, PI)))*SQ2);
                   }

                   float map(float value, float inMin, float inMax, float outMin, float outMax) {
                     return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
                   }

                   void respawn() {
                       out_hot = map(gold_noise(vec2(in_id, u_time), 25), 0.0, 1.0, 0.4, 1.0);
                       out_pos = vec2(
                           map(gold_noise(vec2(in_id, u_time), 0), 0, 1, -1, +1), 
                           map(gold_noise(vec2(in_id, u_time), 50), 0, 1, -1, +1)
                       );
                       float r = gold_noise(vec2(in_id, u_time), 100) * 0.015;
                       out_vel = vec2(0.0, r);
                   }

                   void update() {
                       out_hot = in_hot - ROD;
                       out_pos = in_pos + in_vel + u_acc;
                       out_vel = in_vel * G;
                   }

                   void main() {
                       // particle is "dead" (out of life) -> respawn
                       if(in_hot <= 0.0) respawn();
                       // particle is out of screen -> respawn
                       else if(abs(in_pos.x) > 1.0) respawn();
                       else if(abs(in_pos.y) > 1.0) respawn();
                       // otherwise -> update particle: position, velocity, live, ...
                       else update();

                       out_id = in_id;
                   }
                ''',
                varyings=['out_pos', 'out_hot', 'out_vel', 'out_id']
            )
        else:
            self.transform = shader_transform

        self.color_particle = self.prog['u_color_particle']
        # https://www.toutes-les-couleurs.com/code-couleur-rvb.php
        self.color_particle.value = CHAMPAGNE_COLOR

        self.nb_particles = nb_max_particles

        self.acc = self.transform['u_acc']
        self.acc.value = (0.0, -0.0001)

        # at init => particles at center with random velocities
        self.vbo1 = self.ctx.buffer(
            b''.join(
                Particles.gen_particle(range_angle_axis=np.pi * 2.0, id_particle=i)
                for i in range(self.nb_particles)
            )
        )
        # vertex buffer/array object use for updating particles system
        self.vbo2 = self.ctx.buffer(reserve=self.vbo1.size)

        self.vao1 = self.ctx.simple_vertex_array(
            self.transform, self.vbo1,
            'in_pos', 'in_hot', 'in_vel', 'in_id'
        )

        self.render_vao = self.ctx.vertex_array(self.prog, [
            # - '2f' => 2 floats ~ vec2: in_vert
            # - '1f' => 1 float: in_hot
            # - '3x4' => drop 3 * 4bytes = 3 floats ~ vec2 + float: in_vel, in_id
            (self.vbo1, '2f 1f 3x4', 'in_vert', 'in_hot'),
        ], skip_errors=False)

        self.u_time = self.transform['u_time']
        self.u_time.value = 0.0

        self.ctx.point_size = float(POINT_SIZE)

    @staticmethod
    def gen_particle(
            position: Tuple[float, float] = (0.0, 0.0),
            angle_main_axis: float = np.pi / 2.0,
            range_angle_axis: float = np.pi / 8.0,
            id_particle: int = 0,
    ) -> bytes:
        """
        Generate particle with random velocity (describe by a random (non unit) vector)
        """
        # random velocity (orientation and velocity)
        a = np.random.uniform(angle_main_axis - range_angle_axis, angle_main_axis + range_angle_axis)
        r = np.random.uniform(0.0, 0.02)
        velocity_x = np.cos(a) * r
        velocity_y = np.sin(a) * r
        # particle at center with a random velocity
        return np.array(
            [
                *position,  # position[vec2]
                1.0,  # hotness[float]
                velocity_x, velocity_y,  # velocity[vec2]
                float(id_particle)  # id particle[float]
            ]
        ).astype('f4').tobytes()  # encoded float32

    def render_particles(self) -> None:
        """
        Render particles system.
        Using a static vertex array object and rendering point sprites.
        """
        self.render_vao.render(moderngl.POINTS, self.nb_particles)

    def update_particles(self, time) -> None:
        """
        Update particles system on GPU
        """
        self.u_time.value = time
        self.vao1.transform(self.vbo2, moderngl.POINTS, self.nb_particles)
        self.ctx.copy_buffer(self.vbo1, self.vbo2)

    def on_render(self, time, _frame_time) -> None:
        # Physic pass: update particles system
        self.update_particles(time)
        # Rendering pass: render particles system
        self.ctx.clear(*CHAMPAGNE_COLOR)
        self.render_particles()


if __name__ == '__main__':
    Particles.run()
