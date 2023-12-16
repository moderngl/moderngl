.. py:currentmodule:: moderngl

Vertex Shader (transforms)
==========================

Let's get our hands dirty right away and jump into it by showing the
simplest forms of shaders in OpenGL. These are called transforms or
transform feedback. Instead of drawing to the screen we simply
capture the output of a shader into a :py:class:`Buffer`.

The example below shows shader program with only a vertex shader.
It has no input data, but we can still force it to run N times.
The ``gl_VertexID`` (int) variable is a built-in value in vertex
shaders containing an integer representing the vertex number
being processed.

Input variables in vertex shaders are called **attributes**
(we have no inputs in this example)
while output values are called **varyings**.

.. code::

    import struct
    import moderngl

    ctx = moderngl.create_context(standalone=True)

    program = ctx.program(
        vertex_shader="""
        #version 330

        // Output values for the shader. They end up in the buffer.
        out float value;
        out float product;

        void main() {
            // Implicit type conversion from int to float will happen here
            value = gl_VertexID;
            product = gl_VertexID * gl_VertexID;
        }
        """,
        # What out varyings to capture in our buffer!
        varyings=["value", "product"],
    )

    NUM_VERTICES = 10

    # We always need a vertex array in order to execute a shader program.
    # Our shader doesn't have any buffer inputs, so we give it an empty array.
    vao = ctx.vertex_array(program, [])

    # Create a buffer allocating room for 20 32 bit floats
    # num of vertices (10) * num of varyings per vertex (2) * size of float in bytes (4)
    buffer = ctx.buffer(reserve=NUM_VERTICES * 2 * 4)

    # Start a transform with buffer as the destination.
    # We force the vertex shader to run 10 times
    vao.transform(buffer, vertices=NUM_VERTICES)

    # Unpack the 20 float values from the buffer (copy from graphics memory to system memory).
    # Reading from the buffer will cause a sync (the python program stalls until the shader is done)
    data = struct.unpack("20f", buffer.read())
    for i in range(0, 20, 2):
        print("value = {}, product = {}".format(*data[i:i+2]))

Output of the program is::

    value = 0.0, product = 0.0
    value = 1.0, product = 1.0
    value = 2.0, product = 4.0
    value = 3.0, product = 9.0
    value = 4.0, product = 16.0
    value = 5.0, product = 25.0
    value = 6.0, product = 36.0
    value = 7.0, product = 49.0
    value = 8.0, product = 64.0
    value = 9.0, product = 81.0

The GPU is at the very least slightly offended by the meager amount
work we assigned it, but this at least shows the basic concept of transforms.
We would in most situations also not read the results back into
system memory because it's slow, but sometimes it is needed.

This shader program could for example be modified to generate some
geometry or data for any other purpose you might imagine useful.
Using modulus (``mod(number, divisor)``) on ``gl_VertexID`` can get you pretty far.

.. Warning::
    
    One known bug in many OpenGL drivers is related to the ``mod(number, divisor)`` function. It lies in the fact that if the first argument is exactly 2 times the second, then instead of ``0`` you will receive the value of the second argument [divisor]. To avoid this error, it is recommended to use the following additional function (insert it into the shader before ``void main()`` and then always call ``mod2()`` instead of ``mod()``)::
    
        float mod2(float number, float divisor) {
            float result = mod(number, divisor);
            if (result >= divisor) {
                result = 0.0;
            }
            return result;
        }

