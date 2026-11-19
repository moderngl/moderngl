
import extras
import moderngl
import numpy as np
import pygame

N = 128

extras.init()

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

window_size = pygame.display.get_window_size()

ctx = moderngl.get_context()

image = ctx.texture(window_size, 'rgba8unorm', samples=4)
framebuffer = ctx.framebuffer([image])
framebuffer.clear_color = (0.0, 0.0, 0.0, 1.0)

uniform_buffer = ctx.buffer(size=16, kind='uniform_buffer')

points_data = np.random.uniform((-0.7, 0.3), (-0.2, 0.9), (N, N, 2)).astype('f4')
velocity_data = np.random.normal((0.004, 0.0), (0.0015, 0.0005), (N, N, 2)).astype('f4')

points = [ctx.texture((N, N), 'rg32float') for _ in range(3)]
points[0].write(points_data)
points[1].write(points_data + velocity_data)


def nearest(texture):
    return ctx.sampler(texture, min_filter='nearest', mag_filter='nearest')


ctx.shaders['get_point'] = f'''
    const int N = {N};

    vec2 get_point(int idx) {{
        return texelFetch(Points, ivec2(idx % {N}, idx / {N}), 0).xy;
    }}
'''

ctx.shaders['simulate.vert'] = '''
    #version 330 core
    precision highp float;

    vec2 positions[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2(3.0, -1.0),
        vec2(-1.0, 3.0)
    );

    void main() {
        gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
    }
'''

ctx.shaders['simulate.frag'] = '''
    #version 330 core
    precision highp float;

    layout (std140) uniform Common {
        vec2 cursor;
        float seed;
    };

    uniform sampler2D PreviousPoints;
    uniform sampler2D Points;

    layout (location = 0) out vec2 out_point;

    void main() {
        vec2 prev_point = texelFetch(PreviousPoints, ivec2(gl_FragCoord.xy), 0).xy;
        vec2 point = texelFetch(Points, ivec2(gl_FragCoord.xy), 0).xy;
        vec2 velocity = (point - prev_point) * 0.999 + vec2(0.0, -0.0002);
        vec2 dir = (cursor - point) * vec2(1.777, 1.0);
        velocity += dir / (dot(dir, dir) + 0.05) * 0.0002;
        vec2 noise = fract(sin(vec2(dot(gl_FragCoord.xy, vec2(12.9898, 78.233)), dot(gl_FragCoord.xy, vec2(39.346, 11.135))) + seed) * 43758.5453);
        velocity += (noise - 0.5) * 0.0004;
        float speed = length(velocity);
        if (speed > 0.03) {
            velocity *= 0.03 / speed;
        }
        float bounce = 0.4 + 0.5 * fract(sin(dot(gl_FragCoord.xy, vec2(12.9898, 78.233))) * 43758.5453);
        vec2 next = point + velocity;
        if (abs(next.x) > 1.0) {
            next.x = sign(next.x) * (1.0 + bounce - abs(next.x) * bounce);
        }
        if (abs(next.y) > 1.0) {
            next.y = sign(next.y) * (1.0 + bounce - abs(next.y) * bounce);
        }
        out_point = next;
    }
'''

ctx.shaders['render_points.vert'] = '''
    #version 330 core
    precision highp float;

    uniform sampler2D Points;

    #include "get_point"

    out vec3 v_color;

    void main() {
        gl_PointSize = 2.0;
        gl_Position = vec4(get_point(gl_VertexID), 0.0, 1.0);
        v_color = vec3(0.4, 0.7, 1.0);
    }
'''

ctx.shaders['render_points.frag'] = '''
    #version 330 core
    precision highp float;

    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(v_color, 1.0);
    }
'''

simulate_pipeline = ctx.pipeline(
    'simulate.vert',
    'simulate.frag',
    samplers={'PreviousPoints': 0, 'Points': 1},
    uniform_buffers={'Common': 0},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=ctx.framebuffer([points[2]]),
    resources=ctx.resources(
        samplers={
            0: nearest(points[0]),
            1: nearest(points[1]),
        },
        uniform_buffers={
            0: uniform_buffer,
        },
    ),
)

render_pipeline = ctx.pipeline(
    'render_points.vert',
    'render_points.frag',
    samplers={'Points': 0},
    vertex_array=ctx.empty_vertex_array,
    vertices=N * N,
    mode='points',
    framebuffer=framebuffer,
    resources=ctx.resources(
        samplers={
            0: nearest(points[1]),
        },
    ),
)

cx, cy = -1.0, 1.0
sim_time = pygame.time.get_ticks()

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    mouse_pos = pygame.mouse.get_pos()
    cx = cx * 0.95 + (mouse_pos[0] / window_size[0] * 2.0 - 1.0) * 0.05
    cy = cy * 0.95 + -(mouse_pos[1] / window_size[1] * 2.0 - 1.0) * 0.05

    ctx.start_frame()
    now = pygame.time.get_ticks()
    sim_time = max(sim_time, now - 100)
    while sim_time < now:
        sim_time += 16
        uniform_buffer.write(np.array([cx, cy, sim_time % 1000, 0.0], 'f4'))
        simulate_pipeline.run()
        ctx.copy_texture(points[1], points[0])
        ctx.copy_texture(points[2], points[1])
    framebuffer.clear()
    render_pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
