'''
example of using compute shader.

requirements:
    - numpy
    - pillow (for output)
'''
from string import Template
import moderngl
import numpy as np
import PIL.Image


COMPUTE_SOURCE = """
// author: minu jeong

#version 430

#define X $X
#define Y $Y
#define Z $Z
#define W $W
#define H $H

layout(local_size_x=X, local_size_y=Y, local_size_z=Z) in;
layout (std430, binding=0) buffer in_0
{
    vec4 inxs[1];
};

layout (std430, binding=1) buffer out_0
{
    vec4 outxs[1];
};

layout (std430, binding=2) buffer uv_0
{
    vec2 uvs[1];
};

#define win_width 5
#define win_height 5
#define win_wh 25
vec4 window[win_wh] = {
    // should manually initialize this
    vec4(0), vec4(0), vec4(0), vec4(0), vec4(0),
    vec4(0), vec4(0), vec4(0), vec4(0), vec4(0),
    vec4(0), vec4(0), vec4(0), vec4(0), vec4(0),
    vec4(0), vec4(0), vec4(0), vec4(0), vec4(0),
    vec4(0), vec4(0), vec4(0), vec4(0), vec4(0)
};

void main()
{
    // define consts
    const int x = int(gl_LocalInvocationID.x);
    const int y = int(gl_WorkGroupID.x);
    const int frag_i = x + y * W;

    int ignored = 0;
    // read window
    for (int win_x = 0; win_x < win_width; win_x++)
    {
        for (int win_y = 0; win_y < win_height; win_y++)
        {
            int win_i = win_y * win_width + win_x;
            int wox = win_x - win_width / 2;
            int woy = win_y - win_height / 2;
            int src_i = x + wox + (y + woy) * W;
            if (src_i < 0 || src_i > W * H)
            {
                window[win_i] = vec4(0, 0, 0, 0);
                ignored++;
                continue;
            }

            window[win_i] = inxs[src_i];
        }
    }

    // simple bubble sort to find median
    while(true)
    {
        bool is_swapped = false;
        for (int win_ii = win_wh - 1; win_ii > 1; win_ii--)
        {
            vec4 now = window[win_ii];
            if (now.w == 0.0) { continue; }
            if (length(window[win_ii - 1]) > length(now))
            {
                // swap
                window[win_ii] = window[win_ii - 1];
                window[win_ii - 1] = now;
                is_swapped = true;
            }
        }

        if (!is_swapped)
        {
            break;
        }
    }
    int median_i = win_wh / 2 + ignored / 2;
    vec4 median = window[median_i];

    // write to buffer
    outxs[frag_i] = vec4(median.xyz, 1.0);
}
"""

# W = X * Y  // for each run, handles a row of pixels
# execute compute shader for H times to complete
W = 512
H = 256
X = W
Y = 1
Z = 1
FRAMES = 50

context = moderngl.create_standalone_context(require=430)
compute_shader = context.compute_shader(
    Template(COMPUTE_SOURCE).substitute(
        W=W,
        H=H,
        X=X + 1,
        Y=Y,
        Z=Z,
    )
)

# init buffers
buffer_a = context.buffer(data=np.random.uniform(0.0, 1.0, (H, W, 4)).astype("f4"))
buffer_b = context.buffer(data=np.zeros((H, W, 4)).astype('f4'))

imgs = []
last_buffer = buffer_b

for i in range(FRAMES):
    toggle = True if i % 2 else False
    buffer_a.bind_to_storage_buffer(1 if toggle else 0)
    buffer_b.bind_to_storage_buffer(0 if toggle else 1)

    # toggle 2 buffers as input and output
    last_buffer = buffer_a if toggle else buffer_b

    # local invocation id x -> pixel x
    # work group_id x -> pixel y
    # eg) buffer[x, y] = gl_LocalInvocationID.x + gl_WorkGroupID.x * W
    compute_shader.run(group_x=H, group_y=1)

    # print out
    output = np.frombuffer(last_buffer.read(), dtype=np.float32)
    # output = output.reshape((H, W, 4))
    output = np.multiply(output, 255).astype(np.uint8)
    imgs.append(PIL.Image.frombuffer("RGBA", (W, H), output))


out_path = "debug.gif"
print("saving to", out_path)
imgs[0].save(out_path, save_all=True, append_images=imgs[1:], duration=15, loop=0)
