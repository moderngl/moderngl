import os

import moderngl
import numpy as np
import pygame

os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'
pygame.display.init()
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 3)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
pygame.display.gl_set_attribute(pygame.GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, 1)

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

size = pygame.display.get_window_size()

ctx = moderngl.get_context()

image = ctx.texture(size, 'rgba8unorm', multisampled=True)
framebuffer = ctx.framebuffer([image])
framebuffer.clear_color = (0.05, 0.05, 0.05, 1.0)

N = 16

points_data = (np.array([
    np.repeat(np.linspace(-0.7, 0.7, N), N),
    np.tile(np.linspace(-0.7, 0.7, N), N),
]).T + np.random.uniform(-0.01, 0.01, (N * N, 2))).astype('f4')

edges_data = np.full((N * N, 8), -1, 'i4')

for i in range(N):
    for j in range(N):
        t = []
        if i > 0:
            t.append((i - 1) * N + j)
        if j > 0:
            t.append(i * N + (j - 1))
        if i < N - 1:
            t.append((i + 1) * N + j)
        if j < N - 1:
            t.append(i * N + (j + 1))
        if i > 0 and j > 0:
            t.append((i - 1) * N + (j - 1))
        if i < N - 1 and j < N - 1:
            t.append((i + 1) * N + (j + 1))
        if i > 0 and j < N - 1:
            t.append((i - 1) * N + (j + 1))
        if i < N - 1 and j > 0:
            t.append((i + 1) * N + (j - 1))
        edges_data[i * N + j] = (t + [-1] * 8)[:8]

edge_lengths_data = np.zeros((N * N, 8), 'f4')
for i in range(N):
    for j in range(N):
        for k in range(8):
            a = i * N + j
            b = edges_data[a, k]
            if b < 0:
                edge_lengths_data[a, k] = 0.0
            else:
                edge_lengths_data[a, k] = np.sqrt(np.sum((points_data[a] - points_data[b]) ** 2))

points = ctx.texture((N, N), 'rg32float', points_data)
points_temp1 = ctx.texture((N, N), 'rg32float')
points_temp2 = ctx.texture((N, N), 'rg32float')
edges = ctx.texture((8, N * N), 'r32sint', edges_data)
edge_lengths = ctx.texture((8, N * N), 'r32float', edge_lengths_data)


def nearest(texture):
    return ctx.sampler(texture, min_filter='nearest', mag_filter='nearest')


points_framebuffer = ctx.framebuffer([points])

ctx.shaders['get_point'] = f'''
    const int N = {N};

    vec2 get_point(int idx) {{
        return texelFetch(Points, ivec2(idx % {N}, idx / {N}), 0).xy;
    }}
'''

ctx.shaders['edges.vert'] = '''
    #version 330 core
    uniform sampler2D Points;
    uniform isampler2D Edges;

    #include "get_point"

    out vec3 v_color;

    void main() {
        if (gl_VertexID % 2 == 1) {
            int a = texelFetch(Edges, ivec2(gl_VertexID / 2, gl_InstanceID), 0).r;
            if (a < 0) {
                a = gl_InstanceID;
            }
            gl_Position = vec4(get_point(a), 0.0, 1.0);
        } else {
            gl_Position = vec4(get_point(gl_InstanceID), 0.0, 1.0);
        }
        v_color = vec3(0.55, 0.75, 1.0);
    }
'''

ctx.shaders['edges.frag'] = '''
    #version 330 core
    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(v_color, 1.0);
    }
'''

ctx.shaders['points.vert'] = '''
    #version 330 core
    uniform sampler2D Points;

    #include "get_point"

    out vec3 v_color;

    void main() {
        gl_PointSize = 5.0;
        gl_Position = vec4(get_point(gl_VertexID), 0.0, 1.0);
        v_color = vec3(1.0, 1.0, 1.0);
    }
'''

ctx.shaders['points.frag'] = '''
    #version 330 core
    in vec3 v_color;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(v_color, 1.0);
    }
'''

ctx.shaders['move_points.vert'] = '''
    #version 330 core
    vec2 positions[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2(3.0, -1.0),
        vec2(-1.0, 3.0)
    );

    void main() {
        gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
    }
'''

ctx.shaders['move_points.frag'] = '''
    #version 330 core
    uniform sampler2D PrevPoints;
    uniform sampler2D Points;

    layout (location = 0) out vec2 out_point;

    void main() {
        vec2 prev_point = texelFetch(PrevPoints, ivec2(gl_FragCoord.xy), 0).xy;
        vec2 point = texelFetch(Points, ivec2(gl_FragCoord.xy), 0).xy;
        vec2 velocity = point - prev_point + vec2(0.0, -0.0001);
        out_point = point + velocity;
        if (out_point.y < -0.9) {
            out_point.y = -0.9;
        }
    }
'''

ctx.shaders['constraint_edges.vert'] = '''
    #version 330 core
    vec2 positions[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2(3.0, -1.0),
        vec2(-1.0, 3.0)
    );

    void main() {
        gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
    }
'''

ctx.shaders['constraint_edges.frag'] = '''
    #version 330 core
    uniform sampler2D Points;
    uniform isampler2D Edges;
    uniform sampler2D EdgeLengths;

    #include "get_point"

    layout (location = 0) out vec2 out_point;

    void main() {
        int point_edges = int(gl_FragCoord.x) + int(gl_FragCoord.y) * N;
        vec2 point = texelFetch(Points, ivec2(gl_FragCoord.xy), 0).xy;
        vec2 new_point = point;
        for (int i = 0; i < 8; ++i) {
            int a = texelFetch(Edges, ivec2(i, point_edges), 0).r;
            if (a < 0) {
                break;
            }
            vec2 other_point = get_point(a);
            vec2 dir = point - other_point;
            float lng = length(dir);
            float target_lng = texelFetch(EdgeLengths, ivec2(i, point_edges), 0).r;
            new_point += (dir / lng) * (target_lng - lng) * 0.1;
        }
        out_point = new_point;
    }
'''

edges_pipeline = ctx.pipeline(
    'edges.vert',
    'edges.frag',
    samplers={'Points': 0, 'Edges': 1},
    vertex_array=ctx.empty_vertex_array,
    vertices=16,
    mode='lines',
    framebuffer=framebuffer,
    resources=ctx.resources(samplers={0: nearest(points), 1: nearest(edges)}),
    instances=N * N,
)

points_pipeline = ctx.pipeline(
    'points.vert',
    'points.frag',
    samplers={'Points': 0},
    vertex_array=ctx.empty_vertex_array,
    vertices=N * N,
    mode='points',
    framebuffer=framebuffer,
    resources=ctx.resources(samplers={0: nearest(points)}),
)

move_points_pipeline = ctx.pipeline(
    'move_points.vert',
    'move_points.frag',
    samplers={'PrevPoints': 0, 'Points': 1},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=points_framebuffer,
    resources=ctx.resources(samplers={0: nearest(points_temp1), 1: nearest(points_temp2)}),
)

constraint_edges_pipeline = ctx.pipeline(
    'constraint_edges.vert',
    'constraint_edges.frag',
    samplers={'Points': 0, 'Edges': 1, 'EdgeLengths': 2},
    vertex_array=ctx.empty_vertex_array,
    vertices=3,
    framebuffer=points_framebuffer,
    resources=ctx.resources(samplers={0: nearest(points_temp1), 1: nearest(edges), 2: nearest(edge_lengths)}),
)

ctx.copy_texture(points, points_temp1)
ctx.copy_texture(points, points_temp2)

sim_time = pygame.time.get_ticks()

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    ctx.start_frame()
    now = pygame.time.get_ticks()
    sim_time = max(sim_time, now - 100)
    while sim_time < now:
        sim_time += 16
        ctx.copy_texture(points_temp2, points_temp1)
        ctx.copy_texture(points, points_temp2)
        move_points_pipeline.run()
        ctx.copy_texture(points, points_temp1)
        constraint_edges_pipeline.run()
    framebuffer.clear()
    edges_pipeline.run()
    points_pipeline.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
