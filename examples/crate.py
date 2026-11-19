import math
import struct

import extras
import moderngl
import pygame
from PIL import Image


class Crate:
    def __init__(self, size, texture=None, samples=4):
        self.ctx = moderngl.get_context()
        self.image = self.ctx.texture(size, 'rgba8unorm', samples=samples)
        self.depth = self.ctx.texture(size, 'depth24plus', samples=samples)
        self.framebuffer = self.ctx.framebuffer([self.image, self.depth])
        self.output = self.image if self.image.samples == 1 else self.ctx.texture(size, 'rgba8unorm')

        model = extras.load_obj('examples/tutorial/data/models/crate.obj')
        self.vertex_buffer = self.ctx.buffer(model)

        self.texture = texture
        if self.texture is None:
            img = Image.open('examples/tutorial/data/textures/crate.png').convert('RGBA')
            self.texture = self.ctx.texture(img.size, 'rgba8unorm', img.tobytes())

        self.ubo = bytearray(80)
        self.uniform_buffer = self.ctx.buffer(self.ubo, kind='uniform_buffer')

        self.ctx.shaders['crate.vert'] = '''
            #version 330 core
            precision highp float;

            layout (std140) uniform Common {
                mat4 mvp;
                vec3 light;
            };

            layout (location = 0) in vec3 in_vert;
            layout (location = 1) in vec3 in_norm;
            layout (location = 2) in vec2 in_text;

            out vec3 v_vert;
            out vec3 v_norm;
            out vec2 v_text;

            void main() {
                gl_Position = mvp * vec4(in_vert, 1.0);
                v_vert = in_vert;
                v_norm = in_norm;
                v_text = in_text;
            }
        '''

        self.ctx.shaders['crate.frag'] = '''
            #version 330 core
            precision highp float;

            layout (std140) uniform Common {
                mat4 mvp;
                vec3 light;
            };

            uniform sampler2D Texture;

            in vec3 v_vert;
            in vec3 v_norm;
            in vec2 v_text;

            layout (location = 0) out vec4 out_color;

            void main() {
                float lum = clamp(dot(normalize(light - v_vert), normalize(v_norm)), 0.0, 1.0) * 0.6 + 0.4;
                out_color = vec4(texture(Texture, v_text).rgb * lum, 1.0);
            }
        '''

        self.pipeline = self.ctx.pipeline(
            'crate.vert',
            'crate.frag',
            samplers={'Texture': 0},
            uniform_buffers={'Common': 0},
            vertex_array=self.ctx.vertex_array(self.vertex_buffer.bind('3f 3f 2f', 0, 1, 2)),
            framebuffer=self.framebuffer,
            resources=self.ctx.resources(
                samplers={
                    0: self.ctx.sampler(self.texture),
                },
                uniform_buffers={
                    0: self.uniform_buffer,
                },
            ),
            depth_test=True,
            cull_face='back',
        )

        self.aspect = size[0] / size[1]

    def render(self):
        eye = (math.cos(yaw) * math.cos(pitch) * 3.354, math.sin(yaw) * math.cos(pitch) * 3.354, math.sin(pitch) * 3.354)
        self.ubo[:64] = moderngl.camera(eye, (0.0, 0.0, 0.5), aspect=self.aspect, fov=45.0)
        self.ubo[64:] = struct.pack('3f4x', *eye)
        self.uniform_buffer.write(self.ubo)
        self.framebuffer.clear()
        self.pipeline.run()
        if self.image != self.output:
            self.ctx.copy_texture(self.image, self.output)


extras.init()
pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

ctx = moderngl.get_context()
scene = Crate(pygame.display.get_window_size())

yaw, pitch, spin = 0.0, 0.46, 0.3

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
