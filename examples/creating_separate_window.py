# Example of creating a window without enclosing code in modengl_window.WindowConfig class
# Description: This example shows how to create a window as a separate object.
# Note: Only one window can be used.

import moderngl as gl
import moderngl_window as glw
import struct


# Create window
window_cls = glw.get_local_window_cls('pyglet')
window = window_cls(
    size=(1024, 512), fullscreen=False, title='Separate window',
    resizable=False, vsync=True, gl_version=(4, 3))
ctx = window.ctx
glw.activate_context(window, ctx=ctx)
window.clear()

# Window prog
prog = ctx.program(
    vertex_shader='''
    #version 330
    in vec3 in_vert;
    in vec4 in_color;
    
    out vec4 o_color;
    out float distance;
    
    void main() {
        gl_Position = vec4(in_vert.x, in_vert.y, in_vert.z, in_vert.z);
        o_color = in_color;
        distance = in_vert.z;
    }
    ''',
    fragment_shader='''
    #version 330
    
    in vec4 o_color;
    in float distance;
    
    out vec4 f_color;
    
    void main() {
        f_color = o_color;
        gl_FragDepth = distance;
    }
    ''')

# Preparing
content = [
    .3, .3, .5, 1, 0, 0, 1,
    .3, -.3, .5, 1, 0, 0, 1,
    -.3, .3, .4, 1, 0, 0, 1,

    .3, .4, .5, 0, 0, 1, 1,
    .3, -.2, .5, 0, 0, 1, 1,
    -.3, .4, .45, 0, 0, 1, 1
]
buf = ctx.buffer(struct.pack(f'{len(content)}f', *content))
vao = ctx.vertex_array(prog, buf, 'in_vert', 'in_color')

# Render loop
ctx.enable(gl.DEPTH_TEST)
while not window.is_closing:
    window.fbo.use()
    window.fbo.clear()
    vao.render()
    window.swap_buffers()
