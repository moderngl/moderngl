import math

import extras
import moderngl
import pygame


class BoxGrid:
    def __init__(self, size, samples=4):
        self.ctx = moderngl.get_context()
        self.image = self.ctx.texture(size, 'rgba8unorm', samples=samples)
        self.depth = self.ctx.texture(size, 'depth24plus', samples=samples)
        self.framebuffer = self.ctx.framebuffer([self.image, self.depth])
        self.output = self.image if self.image.samples == 1 else self.ctx.texture(size, 'rgba8unorm')

        self.uniform_buffer = self.ctx.buffer(size=64, kind='uniform_buffer')

        self.ctx.shaders['box_grid.vert'] = '''
            #version 330 core

            layout (std140) uniform Common {
                mat4 mvp;
            };

            vec3 vertices[36] = vec3[](
                vec3(-0.5, -0.5, -0.5),
                vec3(-0.5, 0.5, -0.5),
                vec3(0.5, 0.5, -0.5),
                vec3(0.5, 0.5, -0.5),
                vec3(0.5, -0.5, -0.5),
                vec3(-0.5, -0.5, -0.5),
                vec3(-0.5, -0.5, 0.5),
                vec3(0.5, -0.5, 0.5),
                vec3(0.5, 0.5, 0.5),
                vec3(0.5, 0.5, 0.5),
                vec3(-0.5, 0.5, 0.5),
                vec3(-0.5, -0.5, 0.5),
                vec3(-0.5, -0.5, -0.5),
                vec3(0.5, -0.5, -0.5),
                vec3(0.5, -0.5, 0.5),
                vec3(0.5, -0.5, 0.5),
                vec3(-0.5, -0.5, 0.5),
                vec3(-0.5, -0.5, -0.5),
                vec3(0.5, -0.5, -0.5),
                vec3(0.5, 0.5, -0.5),
                vec3(0.5, 0.5, 0.5),
                vec3(0.5, 0.5, 0.5),
                vec3(0.5, -0.5, 0.5),
                vec3(0.5, -0.5, -0.5),
                vec3(0.5, 0.5, -0.5),
                vec3(-0.5, 0.5, -0.5),
                vec3(-0.5, 0.5, 0.5),
                vec3(-0.5, 0.5, 0.5),
                vec3(0.5, 0.5, 0.5),
                vec3(0.5, 0.5, -0.5),
                vec3(-0.5, 0.5, -0.5),
                vec3(-0.5, -0.5, -0.5),
                vec3(-0.5, -0.5, 0.5),
                vec3(-0.5, -0.5, 0.5),
                vec3(-0.5, 0.5, 0.5),
                vec3(-0.5, 0.5, -0.5)
            );

            out vec3 v_color;

            vec3 hsv2rgb(vec3 c) {
                vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
                vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
                return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
            }

            float hash13(vec3 p3) {
                p3 = fract(p3 * 0.1031);
                p3 += dot(p3, p3.zyx + 31.32);
                return fract((p3.x + p3.y) * p3.z);
            }

            void main() {
                int N = 10;
                float px = float(gl_InstanceID % N);
                float py = float(gl_InstanceID / N % N);
                float pz = float(gl_InstanceID / N / N % N);
                vec3 position = vec3(px, py, pz) - float(N - 1) / 2.0;
                float scale = 0.1;
                gl_Position = mvp * vec4(position + vertices[gl_VertexID] * scale, 1.0);
                v_color = hsv2rgb(vec3(hash13(position), 1.0, 0.5));
            }
        '''

        self.ctx.shaders['box_grid.frag'] = '''
            #version 330 core

            in vec3 v_color;

            layout (location = 0) out vec4 out_color;

            void main() {
                out_color = vec4(v_color, 1.0);
                out_color.rgb = pow(out_color.rgb, vec3(1.0 / 2.2));
            }
        '''

        self.pipeline = self.ctx.pipeline(
            'box_grid.vert',
            'box_grid.frag',
            uniform_buffers={'Common': 0},
            vertex_array=self.ctx.empty_vertex_array,
            vertices=36,
            framebuffer=self.framebuffer,
            resources=self.ctx.resources(
                uniform_buffers={
                    0: self.uniform_buffer,
                },
            ),
            instances=1000,
            depth_test=True,
            cull_face='back',
        )

        self.aspect = size[0] / size[1]

    def render(self):
        eye = (math.cos(yaw) * math.cos(pitch) * 5.385, math.sin(yaw) * math.cos(pitch) * 5.385, math.sin(pitch) * 5.385)
        camera = moderngl.camera(eye, (0.0, 0.0, 0.0), aspect=self.aspect, fov=45.0)
        self.uniform_buffer.write(camera)
        self.framebuffer.clear()
        self.pipeline.run()
        if self.image != self.output:
            self.ctx.copy_texture(self.image, self.output)


extras.init()
pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()
scene = BoxGrid(pygame.display.get_window_size())

yaw, pitch, spin = 0.0, 0.38, 0.3

running = True
while running:
    if spin:
        yaw = pygame.time.get_ticks() / 1000.0 * spin
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.MOUSEMOTION and event.buttons[0]:
            spin = 0.0
            yaw -= event.rel[0] * 0.004
            pitch = min(max(pitch + event.rel[1] * 0.004, 0.05), 1.4)

    ctx.start_frame()
    scene.render()
    ctx.present(scene.output)
    pygame.display.flip()

pygame.quit()
