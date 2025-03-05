import os
import math

import moderngl
import glm
import numpy as np
import moderngl_window as mglw


class Example(mglw.WindowConfig):
    title = "ModernGL Window"
    resizable = False
    gl_version = (4, 6)
    window_size = (900, 900)
    aspect_ratio = 1.0

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.program = self.ctx.program(
            task_shader="""
                #version 460

                #extension GL_NV_mesh_shader : require
                #extension GL_NV_shader_thread_group : require
                #extension GL_NV_gpu_shader5 : require

                layout(local_size_x = 6) in;

                layout(std430, binding = 0) buffer inputBlockData {
                    uint block_types[32 * 32 * 32];
                };

                const int faceBlockArrayOffsets[] = {-1024, 1024, -32, 32, -1, 1};
                const ivec3 faceBlockArrayDirections[] = {
                    ivec3(0, 0, -1),
                    ivec3(0, 0, 1),
                    ivec3(0, -1, 0),
                    ivec3(0, 1, 0),
                    ivec3(-1, 0, 0),
                    ivec3(1, 0, 0)
                };

                taskNV out Task {
                    uint      block_position;
                    uint8_t   faceIds[6];
                } OUT;

                void main() {
                    int position = int(gl_WorkGroupID.x);
                    uint block_type = block_types[position];
                    bool isNotEmpty = block_type != 0;
                    uint vote = ballotThreadNV(isNotEmpty);
                    uint faceCount = bitCount(vote);
                    gl_TaskCountNV = faceCount;
                    OUT.block_position = position;
                    uint idxOffset = bitCount(vote & gl_ThreadLtMaskNV);
                    if (isNotEmpty) {
                        OUT.faceIds[idxOffset] = uint8_t(gl_LocalInvocationID.x);
                    }
                }
            """,
            mesh_shader="""
                #version 460

                #extension GL_NV_mesh_shader : require
                #extension GL_NV_shader_thread_group : require
                #extension GL_NV_gpu_shader5 : require

                layout(local_size_x = 6) in;
                layout(triangles, max_vertices = 6, max_primitives = 2) out;

                taskNV in Task {
                    uint      block_position;
                    uint8_t   faceIds[6];
                } IN;

                out PerVertexData {
                    vec4 color;
                } v_out[];

                const vec3 cubeFaceVertices[6][6] = {
                    {
                        vec3(1.0, 0.0, 0.0),
                        vec3(1.0, 0.0, 1.0),
                        vec3(0.0, 0.0, 1.0),
                        vec3(0.0, 0.0, 1.0),
                        vec3(0.0, 0.0, 0.0),
                        vec3(1.0, 0.0, 0.0)
                    },
                    {
                        vec3(0.0, 1.0, 0.0),
                        vec3(0.0, 1.0, 1.0),
                        vec3(1.0, 1.0, 1.0),
                        vec3(1.0, 1.0, 1.0),
                        vec3(1.0, 1.0, 0.0),
                        vec3(0.0, 1.0, 0.0)
                    },
                    {
                        vec3(0.0, 1.0, 0.0),
                        vec3(1.0, 1.0, 0.0),
                        vec3(1.0, 0.0, 0.0),
                        vec3(1.0, 0.0, 0.0),
                        vec3(0.0, 0.0, 0.0),
                        vec3(0.0, 1.0, 0.0)
                    },
                    {
                        vec3(0.0, 0.0, 1.0),
                        vec3(1.0, 0.0, 1.0),
                        vec3(1.0, 1.0, 1.0),
                        vec3(1.0, 1.0, 1.0),
                        vec3(0.0, 1.0, 1.0),
                        vec3(0.0, 0.0, 1.0)
                    },
                    {
                        vec3(0.0, 0.0, 0.0),
                        vec3(0.0, 0.0, 1.0),
                        vec3(0.0, 1.0, 1.0),
                        vec3(0.0, 1.0, 1.0),
                        vec3(0.0, 1.0, 0.0),
                        vec3(0.0, 0.0, 0.0)
                    },
                    {
                        vec3(1.0, 1.0, 0.0),
                        vec3(1.0, 1.0, 1.0),
                        vec3(1.0, 0.0, 1.0),
                        vec3(1.0, 0.0, 1.0),
                        vec3(1.0, 0.0, 0.0),
                        vec3(1.0, 1.0, 0.0)
                    }
                };

                layout(location = 0) uniform mat4 modelViewProjection;
                layout(location = 1) uniform float time;

                #define PI 3.1415926

                void main() {
                    uint position = IN.block_position;
                    uint8_t face = IN.faceIds[gl_WorkGroupID.x];
                    uint vertexId = gl_LocalInvocationID.x;
                    float x = float(position % 32);
                    float y = float((position / 32) % 32);
                    float z = float(position / 1024);
                    vec3 blockPos = vec3(x, y, z);
                    gl_MeshVerticesNV[vertexId].gl_Position = modelViewProjection * vec4(cubeFaceVertices[face][vertexId] * abs(sin(time + (x+y+z)/24*PI)) + blockPos, 1.0);
                    v_out[vertexId].color = vec4(blockPos / 32.0, 1.0);
                    // gl_MeshVerticesNV[vertexId].gl_Position = modelViewProjection * vec4(blockPos, 1.0);
                    gl_PrimitiveIndicesNV[vertexId] = vertexId;
                    gl_PrimitiveCountNV = 2;
                    // gl_PrimitiveCountNV = 1;
                }
            """,
            fragment_shader="""
            #version 460

            out vec4 fragColor;

            in PerVertexData {
                vec4 color;
            } fragIn;

            void main() {
                fragColor = fragIn.color;
            }
        """,
        )
        self.voxel_buffer = self.ctx.buffer(np.ones(32**3, dtype=np.uint32))
        self.voxel_buffer.bind_to_storage_buffer(0)
        self.indirect_buffer = self.ctx.buffer(np.array([32 * 32 * 32, 0], dtype=np.uint32))

        self.ctx.enable(moderngl.DEPTH_TEST | moderngl.BLEND | moderngl.CULL_FACE)

        self.camera = self.program["modelViewProjection"]
        self.u_time = self.program["time"]

    def on_render(self, time: float, frame_time: float):
        self.ctx.clear(alpha=1)

        proj = glm.perspective((90.0) / 180.0 * math.pi, 1.0, 0.1, 1000.0)
        look = glm.lookAt(
            (
                (48.0 - 16.0) * math.sin(time / 8) + 16.0,
                (48.0 - 16.0) * math.cos(time / 8) + 16.0,
                36.0,
            ),
            (16.0, 16.0, 16.0),
            (0.0, 0.0, 1.0),
        )

        self.u_time.value = time
        self.camera.write(proj * look)
        self.program.draw_mesh_tasks_indirect(self.indirect_buffer) 
        # Optional, without indirect:
        # self.program.draw_mesh_tasks(0, 32 * 32 * 32)


if __name__ == "__main__":
    Example.run()
