import math
import struct

import extras
import moderngl
import pygame


class Scene:
    def __init__(self, size, samples=4):
        self.ctx = moderngl.get_context()
        self.image = self.ctx.texture(size, 'rgba8unorm', samples=samples)
        self.depth = self.ctx.texture(size, 'depth24plus', samples=samples)
        self.framebuffer = self.ctx.framebuffer([self.image, self.depth])
        self.output = self.image if self.image.samples == 1 else self.ctx.texture(size, 'rgba8unorm')

        model = struct.pack('3f3f3f', -0.866, -0.5, 0.0, 0.866, -0.5, 0.0, 0.0, 1.0, 0.0)
        self.vertex_buffer = self.ctx.buffer(model)
        self.uniform_buffer = self.ctx.buffer(size=64, kind='uniform_buffer')

        self.ctx.shaders['empty_3d.vert'] = '''
            #version 330 core

            layout (std140) uniform Common {
                mat4 mvp;
            };

            layout (location = 0) in vec3 in_vert;

            void main() {
                gl_Position = mvp * vec4(in_vert, 1.0);
            }
        '''

        self.ctx.shaders['empty_3d.frag'] = '''
            #version 330 core

            layout (location = 0) out vec4 out_color;

            void main() {
                out_color = vec4(1.0, 1.0, 1.0, 1.0);
            }
        '''

        self.pipeline = self.ctx.pipeline(
            'empty_3d.vert',
            'empty_3d.frag',
            uniform_buffers={'Common': 0},
            vertex_array=self.ctx.vertex_array(self.vertex_buffer.bind('3f', 0)),
            framebuffer=self.framebuffer,
            resources=self.ctx.resources(
                uniform_buffers={
                    0: self.uniform_buffer,
                },
            ),
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
scene = Scene(pygame.display.get_window_size())

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
