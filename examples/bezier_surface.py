import struct

import extras
import moderngl
import numpy as np
import pygame

extras.init()

pygame.init()
pygame.display.set_mode((1280, 720), flags=pygame.OPENGL | pygame.DOUBLEBUF, vsync=True)

window_size = pygame.display.get_window_size()
window_aspect = window_size[0] / window_size[1]

ctx = moderngl.get_context()

image = ctx.texture(window_size, 'rgba8unorm', samples=4)
depth = ctx.texture(window_size, 'depth24plus', samples=4)
framebuffer = ctx.framebuffer([image, depth])
framebuffer.clear_color = (0.01, 0.01, 0.01, 1.0)

N = 33
uv = np.array([
    np.tile(np.linspace(0.0, 1.0, N), N),
    np.repeat(np.linspace(0.0, 1.0, N), N),
]).T

idx = np.full((N - 1, N * 2 + 1), -1)
idx[:, 0:-1:2] = np.arange((N - 1) * N).reshape(N - 1, N)
idx[:, 1:-1:2] = idx[:, 0:-1:2] + N

vertex_buffer = ctx.buffer(uv.astype('f4').tobytes())
index_buffer = ctx.buffer(idx.astype('i4').tobytes(), kind='index_buffer')

M = 5
idx = np.full((2, M, N + 1), -1)
idx[0, :, :-1] = (np.tile(np.arange(0, N), M) + np.repeat(np.arange(0, M) * N * (N - 1) // (M - 1), N)).reshape(M, N)
idx[1, :, :-1] = (np.tile(np.arange(0, N), M) * N + np.repeat(np.arange(0, M) * (N - 1) // (M - 1), N)).reshape(M, N)
wire_index_buffer = ctx.buffer(idx.astype('i4').tobytes(), kind='index_buffer')

uniform_buffer = ctx.buffer(size=96, kind='uniform_buffer')
control_points_buffer = ctx.buffer(size=256, kind='uniform_buffer')

resources = ctx.resources(
    uniform_buffers={
        0: uniform_buffer,
        1: control_points_buffer,
    },
)

ctx.shaders['common'] = '''
    layout (std140) uniform Common {
        mat4 mvp;
        vec4 eye_pos;
        vec4 light_pos;
    };
'''

ctx.shaders['surface'] = '''
    layout (std140) uniform ControlPoints {
        vec4 control_points[16];
    };
    float c3[3] = float[](1.0, 2.0, 1.0);
    float c4[4] = float[](1.0, 3.0, 3.0, 1.0);
    vec3 point(int i, int j) {
        return control_points[i * 4 + j].xyz;
    }
    vec3 tangent(int i, int j) {
        return point(i + 1, j) - point(i, j);
    }
    vec3 bitangent(int i, int j) {
        return point(i, j + 1) - point(i, j);
    }
    float safe_pow(float x, float y) {
        if (x == 0.0 && y == 0.0) {
            return 1.0;
        }
        return pow(x, y);
    }
    vec3 surface_vertex(vec2 uv) {
        vec3 v = vec3(0.0, 0.0, 0.0);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                float Bi = c4[i] * safe_pow(uv.x, float(i)) * safe_pow(1.0 - uv.x, float(3 - i));
                float Bj = c4[j] * safe_pow(uv.y, float(j)) * safe_pow(1.0 - uv.y, float(3 - j));
                v += point(i, j) * Bi * Bj;
            }
        }
        return v;
    }
    vec3 surface_tangent(vec2 uv) {
        vec3 v = vec3(0.0, 0.0, 0.0);
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 4; ++j) {
                float Bi = c3[i] * safe_pow(uv.x, float(i)) * safe_pow(1.0 - uv.x, float(2 - i));
                float Bj = c4[j] * safe_pow(uv.y, float(j)) * safe_pow(1.0 - uv.y, float(3 - j));
                v += tangent(i, j) * Bi * Bj;
            }
        }
        return v;
    }
    vec3 surface_bitangent(vec2 uv) {
        vec3 v = vec3(0.0, 0.0, 0.0);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 3; ++j) {
                float Bi = c4[i] * safe_pow(uv.x, float(i)) * safe_pow(1.0 - uv.x, float(3 - i));
                float Bj = c3[j] * safe_pow(uv.y, float(j)) * safe_pow(1.0 - uv.y, float(2 - j));
                v += bitangent(i, j) * Bi * Bj;
            }
        }
        return v;
    }
'''

ctx.shaders['blinn_phong'] = '''
    vec3 blinn_phong(
            vec3 vertex, vec3 normal, vec3 eye, vec3 light, float shininess, vec3 ambient_color, vec3 diffuse_color,
            vec3 light_color, vec3 spec_color, float light_power) {

        vec3 light_dir = light - vertex;
        float light_distance = length(light_dir);
        light_distance = light_distance * light_distance;
        light_dir = normalize(light_dir);

        float lambertian = max(dot(light_dir, normal), 0.0);
        float specular = 0.0;

        if (lambertian > 0.0) {
            vec3 view_dir = normalize(eye - vertex);
            vec3 half_dir = normalize(light_dir + view_dir);
            float spec_angle = max(dot(half_dir, normal), 0.0);
            specular = pow(spec_angle, shininess);
        }

        vec3 color_linear = ambient_color +
            diffuse_color * lambertian * light_color * light_power / light_distance +
            spec_color * specular * light_color * light_power / light_distance;

        return color_linear;
    }
'''

ctx.shaders['surface.vert'] = '''
    #version 330 core

    #include "common"
    #include "surface"

    layout (location = 0) in vec2 in_uv;

    out vec3 v_vertex;
    out vec3 v_normal;

    void main() {
        v_vertex = surface_vertex(in_uv);
        vec3 v_tangent = surface_tangent(in_uv);
        vec3 v_bitangent = surface_bitangent(in_uv);
        v_normal = normalize(cross(v_bitangent, v_tangent));
        gl_Position = mvp * vec4(v_vertex, 1.0);
    }
'''

ctx.shaders['surface.frag'] = '''
    #version 330 core

    #include "common"
    #include "blinn_phong"

    in vec3 v_vertex;
    in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 color = blinn_phong(
            v_vertex, v_normal, eye_pos.xyz, light_pos.xyz, 16.0,
            vec3(0.0, 0.01, 0.05), vec3(0.0, 0.1, 0.5), vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), 60.0
        );
        out_color = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
    }
'''

ctx.shaders['surface_wire.vert'] = '''
    #version 330 core

    #include "common"
    #include "surface"

    layout (location = 0) in vec2 in_uv;

    out vec3 v_vertex;

    void main() {
        v_vertex = surface_vertex(in_uv);
        gl_Position = mvp * vec4(v_vertex, 1.0);
    }
'''

ctx.shaders['surface_wire.frag'] = '''
    #version 330 core

    in vec3 v_vertex;

    layout (location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(0.0, 0.0, 0.0, 1.0);
        gl_FragDepth = gl_FragCoord.z - 1e-4;
    }
'''

ctx.shaders['points.vert'] = '''
    #version 330 core

    #include "common"
    #include "surface"

    vec3 vertices[42] = vec3[](
        vec3(0.000000, 0.000000, -1.000000),
        vec3(0.723607, -0.525725, -0.447220),
        vec3(-0.276388, -0.850649, -0.447220),
        vec3(-0.894426, 0.000000, -0.447216),
        vec3(-0.276388, 0.850649, -0.447220),
        vec3(0.723607, 0.525725, -0.447220),
        vec3(0.276388, -0.850649, 0.447220),
        vec3(-0.723607, -0.525725, 0.447220),
        vec3(-0.723607, 0.525725, 0.447220),
        vec3(0.276388, 0.850649, 0.447220),
        vec3(0.894426, 0.000000, 0.447216),
        vec3(0.000000, 0.000000, 1.000000),
        vec3(-0.162456, -0.499995, -0.850654),
        vec3(0.425323, -0.309011, -0.850654),
        vec3(0.262869, -0.809012, -0.525738),
        vec3(0.850648, 0.000000, -0.525736),
        vec3(0.425323, 0.309011, -0.850654),
        vec3(-0.525730, 0.000000, -0.850652),
        vec3(-0.688189, -0.499997, -0.525736),
        vec3(-0.162456, 0.499995, -0.850654),
        vec3(-0.688189, 0.499997, -0.525736),
        vec3(0.262869, 0.809012, -0.525738),
        vec3(0.951058, -0.309013, 0.000000),
        vec3(0.951058, 0.309013, 0.000000),
        vec3(0.000000, -1.000000, 0.000000),
        vec3(0.587786, -0.809017, 0.000000),
        vec3(-0.951058, -0.309013, 0.000000),
        vec3(-0.587786, -0.809017, 0.000000),
        vec3(-0.587786, 0.809017, 0.000000),
        vec3(-0.951058, 0.309013, 0.000000),
        vec3(0.587786, 0.809017, 0.000000),
        vec3(0.000000, 1.000000, 0.000000),
        vec3(0.688189, -0.499997, 0.525736),
        vec3(-0.262869, -0.809012, 0.525738),
        vec3(-0.850648, 0.000000, 0.525736),
        vec3(-0.262869, 0.809012, 0.525738),
        vec3(0.688189, 0.499997, 0.525736),
        vec3(0.162456, -0.499995, 0.850654),
        vec3(0.525730, 0.000000, 0.850652),
        vec3(-0.425323, -0.309011, 0.850654),
        vec3(-0.425323, 0.309011, 0.850654),
        vec3(0.162456, 0.499995, 0.850654)
    );

    int vertex_indices[240] = int[](
        0, 13, 12, 1, 13, 15, 0, 12, 17, 0, 17, 19, 0, 19, 16, 1, 15, 22, 2, 14, 24, 3, 18, 26, 4, 20, 28, 5, 21,
        30, 1, 22, 25, 2, 24, 27, 3, 26, 29, 4, 28, 31, 5, 30, 23, 6, 32, 37, 7, 33, 39, 8, 34, 40, 9, 35, 41, 10,
        36, 38, 38, 41, 11, 38, 36, 41, 36, 9, 41, 41, 40, 11, 41, 35, 40, 35, 8, 40, 40, 39, 11, 40, 34, 39, 34,
        7, 39, 39, 37, 11, 39, 33, 37, 33, 6, 37, 37, 38, 11, 37, 32, 38, 32, 10, 38, 23, 36, 10, 23, 30, 36, 30,
        9, 36, 31, 35, 9, 31, 28, 35, 28, 8, 35, 29, 34, 8, 29, 26, 34, 26, 7, 34, 27, 33, 7, 27, 24, 33, 24, 6,
        33, 25, 32, 6, 25, 22, 32, 22, 10, 32, 30, 31, 9, 30, 21, 31, 21, 4, 31, 28, 29, 8, 28, 20, 29, 20, 3, 29,
        26, 27, 7, 26, 18, 27, 18, 2, 27, 24, 25, 6, 24, 14, 25, 14, 1, 25, 22, 23, 10, 22, 15, 23, 15, 5, 23, 16,
        21, 5, 16, 19, 21, 19, 4, 21, 19, 20, 4, 19, 17, 20, 17, 3, 20, 17, 18, 3, 17, 12, 18, 12, 2, 18, 15, 16,
        5, 15, 13, 16, 13, 0, 16, 12, 14, 2, 12, 13, 14, 13, 1, 14
    );

    out vec3 v_vertex;
    out vec3 v_normal;

    void main() {
        float scale = 0.02;
        vec3 point = control_points[gl_InstanceID].xyz;
        v_vertex = point + vertices[vertex_indices[gl_VertexID]] * scale;
        v_normal = vertices[vertex_indices[gl_VertexID]];
        gl_Position = mvp * vec4(v_vertex, 1.0);
    }
'''

ctx.shaders['points.frag'] = '''
    #version 330 core

    #include "common"

    in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 color = vec3(1.0, 0.0, 0.0);
        float lum = dot(normalize(eye_pos.xyz), normalize(v_normal)) * 0.7 + 0.3;
        out_color = vec4(pow(color * lum, vec3(1.0 / 2.2)), 1.0);
    }
'''

ctx.shaders['lines.vert'] = '''
    #version 330 core

    #include "common"
    #include "surface"

    vec3 vertices[24] = vec3[](
        vec3(0.000000, 1.000000, 0.000000),
        vec3(0.000000, 1.000000, 1.00000),
        vec3(0.500000, 0.866025, 0.000000),
        vec3(0.500000, 0.866025, 1.00000),
        vec3(0.866025, 0.500000, 0.000000),
        vec3(0.866025, 0.500000, 1.00000),
        vec3(1.000000, -0.000000, 0.000000),
        vec3(1.000000, -0.000000, 1.00000),
        vec3(0.866025, -0.500000, 0.000000),
        vec3(0.866025, -0.500000, 1.00000),
        vec3(0.500000, -0.866025, 0.000000),
        vec3(0.500000, -0.866025, 1.00000),
        vec3(-0.000000, -1.000000, 0.000000),
        vec3(-0.000000, -1.000000, 1.00000),
        vec3(-0.500000, -0.866025, 0.000000),
        vec3(-0.500000, -0.866025, 1.00000),
        vec3(-0.866025, -0.500000, 0.000000),
        vec3(-0.866025, -0.500000, 1.00000),
        vec3(-1.000000, 0.000000, 0.000000),
        vec3(-1.000000, 0.000000, 1.00000),
        vec3(-0.866025, 0.500000, 0.000000),
        vec3(-0.866025, 0.500000, 1.00000),
        vec3(-0.500000, 0.866025, 0.000000),
        vec3(-0.500000, 0.866025, 1.00000)
    );

    vec3 normals[14] = vec3[](
        vec3(-0.0000, 1.0000, -0.0000),
        vec3(0.5000, 0.8660, -0.0000),
        vec3(0.8660, 0.5000, -0.0000),
        vec3(1.0000, -0.0000, -0.0000),
        vec3(0.8660, -0.5000, -0.0000),
        vec3(0.5000, -0.8660, -0.0000),
        vec3(-0.0000, -1.0000, -0.0000),
        vec3(-0.5000, -0.8660, -0.0000),
        vec3(-0.8660, -0.5000, -0.0000),
        vec3(-1.0000, -0.0000, -0.0000),
        vec3(-0.8660, 0.5000, -0.0000),
        vec3(-0.0000, -0.0000, 1.0000),
        vec3(-0.5000, 0.8660, -0.0000),
        vec3(-0.0000, -0.0000, -1.0000)
    );

    int vertex_indices[132] = int[](
        1, 2, 0, 3, 4, 2, 5, 6, 4, 7, 8, 6, 9, 10, 8, 11, 12, 10, 13, 14, 12, 15, 16, 14, 17, 18, 16, 19, 20, 18,
        21, 13, 5, 21, 22, 20, 23, 0, 22, 6, 14, 22, 1, 3, 2, 3, 5, 4, 5, 7, 6, 7, 9, 8, 9, 11, 10, 11, 13, 12, 13,
        15, 14, 15, 17, 16, 17, 19, 18, 19, 21, 20, 5, 3, 1, 1, 23, 21, 21, 19, 17, 17, 15, 13, 13, 11, 9, 9, 7, 5,
        5, 1, 21, 21, 17, 13, 13, 9, 5, 21, 23, 22, 23, 1, 0, 22, 0, 2, 2, 4, 6, 6, 8, 10, 10, 12, 14, 14, 16, 18,
        18, 20, 22, 22, 2, 6, 6, 10, 14, 14, 18, 22
    );

    int normal_indices[132] = int[](
        0, 1, 0, 1, 2, 1, 2, 3, 2, 3, 4, 3, 4, 5, 4, 5, 6, 5, 6, 7, 6, 7, 8, 7, 8, 9, 8, 9, 10, 9, 11, 11, 11, 10,
        12, 10, 12, 0, 12, 13, 13, 13, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8,
        9, 9, 9, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
        11, 11, 11, 11, 11, 10, 12, 12, 12, 0, 0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
        13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13
    );

    int point_pairs[48] = int[](
        0, 1, 1, 2, 2, 3,
        4, 5, 5, 6, 6, 7,
        8, 9, 9, 10, 10, 11,
        12, 13, 13, 14, 14, 15,
        0, 4, 4, 8, 8, 12,
        1, 5, 5, 9, 9, 13,
        2, 6, 6, 10, 10, 14,
        3, 7, 7, 11, 11, 15
    );

    out vec3 v_vertex;
    out vec3 v_normal;

    void main() {
        float scale = 0.005;
        vec3 a = control_points[point_pairs[gl_InstanceID * 2 + 0]].xyz;
        vec3 b = control_points[point_pairs[gl_InstanceID * 2 + 1]].xyz;
        vec3 forward = b - a;
        vec3 sideways = normalize(cross(forward, vec3(0.0, 0.0, 1.0)));
        vec3 upwards = normalize(cross(forward, sideways));
        mat3 basis = mat3(sideways * scale, upwards * scale, forward);
        v_vertex = a + basis * vertices[vertex_indices[gl_VertexID]];
        v_normal = basis * normals[normal_indices[gl_VertexID]];
        gl_Position = mvp * vec4(v_vertex, 1.0);
    }
'''

ctx.shaders['lines.frag'] = '''
    #version 330 core

    #include "common"

    in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 color = vec3(1.0, 0.5, 0.0);
        float lum = dot(normalize(eye_pos.xyz), normalize(v_normal)) * 0.7 + 0.3;
        out_color = vec4(pow(color * lum, vec3(1.0 / 2.2)), 1.0);
    }
'''

ctx.shaders['points_seethrough.frag'] = '''
    #version 330 core

    #include "common"

    in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 color = vec3(1.0, 0.0, 0.0);
        float lum = dot(normalize(eye_pos.xyz), normalize(v_normal)) * 0.7 + 0.3;
        out_color = vec4(pow(color * lum, vec3(1.0 / 2.2)), 0.05);
    }
'''

ctx.shaders['lines_seethrough.frag'] = '''
    #version 330 core

    #include "common"

    in vec3 v_normal;

    layout (location = 0) out vec4 out_color;

    void main() {
        vec3 color = vec3(1.0, 0.5, 0.0);
        float lum = dot(normalize(eye_pos.xyz), normalize(v_normal)) * 0.7 + 0.3;
        out_color = vec4(pow(color * lum, vec3(1.0 / 2.2)), 0.05);
    }
'''



def pipeline(name, vertex_array, shaders=None, **kwargs):
    vert, frag = shaders or (name, name)
    return ctx.pipeline(
        f'{vert}.vert',
        f'{frag}.frag',
        uniform_buffers={'Common': 0, 'ControlPoints': 1},
        vertex_array=vertex_array,
        framebuffer=framebuffer,
        resources=resources,
        depth_test=True,
        cull_face='back',
        **kwargs,
    )


seethrough = {
    'depth_write': False,
    'depth_func': 'greater',
    'blend': {'src': 'src_alpha', 'dst': 'one_minus_src_alpha'},
}

surface_pipeline = pipeline('surface', ctx.vertex_array(vertex_buffer.bind('2f', 0), index_buffer, mode='triangle_strip'))
surface_wire_pipeline = pipeline('surface_wire', ctx.vertex_array(vertex_buffer.bind('2f', 0), wire_index_buffer, mode='line_strip'))
points_pipeline = pipeline('points', ctx.empty_vertex_array, vertices=240, instances=16)
lines_pipeline = pipeline('lines', ctx.empty_vertex_array, vertices=132, instances=24)
points_pipeline_seethrough = pipeline('points', ctx.empty_vertex_array, ('points', 'points_seethrough'), vertices=240, instances=16, **seethrough)
lines_pipeline_seethrough = pipeline('lines', ctx.empty_vertex_array, ('lines', 'lines_seethrough'), vertices=132, instances=24, **seethrough)

height = np.random.uniform(-0.4, 0.4, 16)
offset = np.random.uniform(0.0, 2.0 * np.pi, 16)
radius = np.random.uniform(0.2, 0.5, 16)
speed = np.random.uniform(0.3, 0.7, 16)

t = 0.0

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    t = pygame.time.get_ticks() / 1000.0

    pts = np.array([
        np.tile(np.linspace(0.0, 1.0, 4), 4) - 0.5,
        np.repeat(np.linspace(0.0, 1.0, 4), 4) - 0.5,
        height + np.sin(offset + t * speed) * radius,
        np.zeros(16),
    ]).T

    ctx.start_frame()
    control_points_buffer.write(pts.astype('f4').tobytes())

    x, y = np.sin(t * 0.3) * 2.0, np.cos(t * 0.3) * 2.0
    camera = moderngl.camera((x, y, 1.1), (0.0, 0.0, 0.0), aspect=window_aspect, fov=45.0)
    uniform_buffer.write(camera + struct.pack('3f4x3f4x', x, y, 1.1, 4.0, 4.0, 10.0))

    framebuffer.clear()
    surface_pipeline.run()
    surface_wire_pipeline.run()
    lines_pipeline.run()
    points_pipeline.run()
    points_pipeline_seethrough.run()
    lines_pipeline_seethrough.run()
    ctx.present(image)

    pygame.display.flip()

pygame.quit()
