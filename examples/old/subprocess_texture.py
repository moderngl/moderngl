"""
Using a subprocess in python to calculate data on a separate core.
Threads in python are not really suitable for this task because
of the GIL and we would just be slowing down the main thread.

This is a fairly simple example spawning a deamon thread that
throws new random textures on a queue that can be read back by
the main process.

There are probably better ways of doing this, but this is a good start.
For example: We might want spawn multiple processes that writes to
the same queue.
"""
from time import sleep
import multiprocessing as mp
from queue import Empty
import struct
import random
import moderngl_window
from moderngl_window import geometry
from pyrr import matrix44


class SubprocessTest(moderngl_window.WindowConfig):
    window_size = 512, 512
    aspect_ratio = 1.0

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.texture = self.ctx.texture((100, 100), 3, data=gen_texture())
        self.texture.filter = self.ctx.NEAREST, self.ctx.NEAREST
        self.quad = geometry.quad_2d(size=(1.3, 1.3))
        self.program = self.ctx.program(
            vertex_shader="""
            #version 330
            uniform mat4 model;
            in vec2 in_position;
            in vec2 in_texcoord_0;
            out vec2 uv;
            void main () {
                gl_Position = model * vec4(in_position, 0.0, 1.0);
                uv = in_texcoord_0;
            }
            """,
            fragment_shader="""
            #version 330
            uniform sampler2D tex;
            in vec2 uv;
            out vec4 f_color;
            void main() {
                f_color = texture(tex, uv);
            }
            """,
        )
        self.generator = TextureGenerator()
        self.generator.start()

    def next_texture(self):
        """Attempt to get a new texture from the queue"""
        try:
            data = self.generator.queue.get(block=False)
            self.texture.write(data)
        except Empty:
            pass

    def on_render(self, time: float, frame_time: float):
        self.ctx.clear()
        self.texture.use()
        self.program["model"].write(matrix44.create_from_axis_rotation([0.0, 0.0, 1.0], time, dtype="f4"))
        self.quad.render(self.program)

        self.next_texture()

    def close(self):
        self.generator.stop()


class TextureGenerator:

    def __init__(self):
        self.queue = mp.Queue()
        self.process = mp.Process(
            target=self.do_work,
            args=(self.queue,),
            daemon=True,
        )

    def do_work(self, queue):
        while True:
            # Back off a little bit to avoid overloading the queue
            if self.queue.qsize() > 60:
                sleep(0.5)

            queue.put(gen_texture(), block=False)

    def start(self):
        self.process.start()

    def stop(self):
        self.process.terminate()


def gen_texture():
    """
    An inefficient way of generating random texture.
    The goal is to make the process reasonably busy.
    """
    num_frags = 100 * 100 * 3
    data = [random.randint(0, 255) for _ in range(num_frags)]
    return struct.pack(f"{num_frags}B", *data)


if __name__ == "__main__":
    SubprocessTest.run()
