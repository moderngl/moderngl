import os
import re
import runpy
import subprocess
import sys
import types

from PIL import Image

FRAMES = {
    'examples/soft_body_simulation.py': 300,
    'examples/particle_system.py': 125,
    'examples/game_of_life.py': 120,
    'examples/points.py': 300,
    'examples/moderngl_logo.py': 1,
    'examples/tutorial/15_pillow_overlay.py': 490,
}

SEEDS = {
    'examples/bezier_surface.py': 0,
    'examples/bezier_curves.py': 2,
}

CROP = {
    'examples/font.py': (70, 60, 990, 578),
}

MOUSE = {
    'examples/particle_system.py': (500, 150),
    'examples/picking.py': (715, 570),
}


class Namespace(types.ModuleType):
    def __getattr__(self, name):
        return 0 if name.isupper() else lambda *args, **kwargs: 0


def capture(path, out, frames):
    from PIL import ImageOps

    state = {'size': (1280, 720), 'frame': 0}

    def set_mode(size, *args, **kwargs):
        state['size'] = tuple(size)
        state['headless'] = moderngl.headless_context()

    pygame = Namespace('pygame')
    pygame.display = Namespace('pygame.display')
    pygame.event = Namespace('pygame.event')
    pygame.mouse = Namespace('pygame.mouse')
    pygame.time = Namespace('pygame.time')
    pygame.display.set_mode = set_mode
    pygame.display.get_window_size = lambda: state['size']
    pygame.event.get = lambda: []
    pygame.mouse.get_pos = lambda: MOUSE.get(path, (0, 0))
    pygame.mouse.get_pressed = lambda num_buttons=3: (False, False, False)
    pygame.time.get_ticks = lambda: state['frame'] * 16
    pygame.time.Clock = lambda: Namespace('clock')
    sys.modules['pygame'] = pygame

    import moderngl
    import numpy as np

    def present(ctx, image):
        state['frame'] += 1
        if state['frame'] < frames:
            ctx.end_frame()
            return
        output = ctx.texture(image.size, 'rgba8unorm')
        ctx.copy_texture(image, output)
        w, h = image.size
        img = Image.frombytes('RGBA', (w, h), output.read().wait()).transpose(Image.FLIP_TOP_BOTTOM).convert('RGB')
        if path in CROP:
            img = img.crop(CROP[path])
            w, h = img.size
        canvas = Image.new('RGB', (h * 16 // 9, h)) if w * 9 < h * 16 else Image.new('RGB', (w, w * 9 // 16))
        canvas.paste(img, ((canvas.width - w) // 2, (canvas.height - h) // 2))
        os.makedirs(os.path.dirname(out), exist_ok=True)
        ImageOps.fit(canvas, (480, 270)).save(out)
        os._exit(0)

    moderngl.Context.present = present
    np.random.seed(SEEDS.get(path, 0))
    runpy.run_path(path, run_name='__main__')
    print('no present call', file=sys.stderr)
    os._exit(1)


def main():
    paths = re.findall(r'example=(examples/[\w/]+\.py)', open('docs/examples.rst').read())
    paths = [path for path in paths if not path.startswith('examples/tutorial/')] + ['examples/tutorial/15_pillow_overlay.py']
    failed = []
    for path in paths:
        out = 'docs/demo/images/' + path.removeprefix('examples/').removesuffix('.py') + '.png'
        res = subprocess.run([sys.executable, __file__, path, out, str(FRAMES.get(path, 60))], capture_output=True, text=True, timeout=300)
        print('ok' if res.returncode == 0 else 'FAILED', path)
        if res.returncode:
            failed.append(path)
            print(res.stderr[-2000:])
    logo = Image.open('docs/demo/images/moderngl_logo.png')
    logo.crop((105, 0, 375, 270)).resize((64, 64), Image.LANCZOS).save('docs/demo/images/favicon.png')
    sys.exit(1 if failed else 0)


if len(sys.argv) == 4:
    capture(sys.argv[1], sys.argv[2], int(sys.argv[3]))

main()
