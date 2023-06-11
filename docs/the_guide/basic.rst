.. py:currentmodule:: moderngl

Creating a Context
------------------

Before we can do anything with ModernGL we need a :py:class:`~moderngl.Context`.
The :py:class:`~moderngl.Context` object makes us able to create OpenGL resources.
ModernGL can only create headless contexts (no window), but it can also detect
and use contexts from a large range of window libraries. The `moderngl-window`_
library is a good start or reference for rendering to a window.

Most of the example code here assumes a ``ctx`` variable exists with a
headless context::

    # standalone=True makes a headless context
    ctx = moderngl.create_context(standalone=True)

Detecting an active context created by a window library is simply::

    ctx = moderngl.create_context()

More details about context creation can be found in the :ref:`context`
section.

.. moderngl-window: _https://github.com/moderngl/moderngl-window

ModernGL Types
--------------

Before throwing you into doing shaders we'll go through some of the
most important types/objects in ModernGL.

* :py:class:`Buffer` is an OpenGL buffer we can for example write
  vertex data into. This data will reside in graphics memory.
* :py:class:`Program` is a shader program. We can feed it GLSL
  source code as strings to set up our shader program
* :py:class:`VertexArray` is a light object responsible for
  communication between :py:class:`Buffer` and :py:class:`Program`
  so it can understand how to access the provided buffers
  and do the rendering call.
  These objects are currently immutable but are cheap to make.
* :py:class:`Texture`, :py:class:`TextureArray`, :py:class:`Texture3D`
  and :py:class:`TextureCube` represents the different texture types.
  :py:class:`Texture` is a 2d texture and is most commonly used.
* :py:class:`Framebuffer` is an offscreen render target. It supports
  different attachments types such as a :py:class:`Texture`
  and a depth texture/buffer.

All of the objects above can only be created from a :py:class:`Context` object:

* :py:meth:`Context.buffer`
* :py:meth:`Context.program`
* :py:meth:`Context.vertex_array`
* :py:meth:`Context.texture`
* :py:meth:`Context.texture_array`
* :py:meth:`Context.texture3d`
* :py:meth:`Context.texture_cube`
* :py:meth:`Context.framebuffer`

The ModernGL types cannot be extended as in; you cannot subclass them.
Extending them must be done through substitution and not inheritance.
This is related to performance. Most objects have an ``extra``
property that can contain any python object.

Shader Introduction
-------------------

Shaders are small programs running on the `GPU`_ (Graphics Processing Unit).
We are using a fairly simple language called `GLSL`_ (OpenGL Shading Language).
This is a C-style language, so it covers most of the features you would expect
with such a language. Control structures (for-loops, if-else statements, etc)
exist in GLSL, including the switch statement.

.. Note:: The name "shader" comes from the fact that these small GPU programs was
          originally created for shading (lighting) 3D scenes. This started
          as per-vertex lighting when the early shaders could only process 
          vertices and evolved into per-pixel lighting when the fragment
          shader was introduced.
          They are used in many other areas today, but the name have stuck around.

Examples of types are::

    bool value = true;
    int value = 1;
    uint value = 1;
    float value = 0.0;
    double value = 0.0;

Each type above also has a 2, 3 and 4 component version::

    // float (default) type
    vec2 value = vec2(0.0, 1.0);
    vec3 value = vec3(0.0, 1.0, 2.0);
    vec4 value = vec4(0.0);

    // signed and unsigned integer vectors
    ivec3 value = ivec3(0);
    uvec3 value = ivec3(0);
    // etc ..

More about GLSL `data types`_ can be found in the Khronos wiki.

The available functions are for example: ``radians``, ``degrees``
``sin``, ``cos``, ``tan``, ``asin``, ``acos``, ``atan``, ``pow``
``exp``, ``log``, ``exp2``, ``log2``, ``sqrt``, ``inversesqrt``,
``abs``, ``sign``, ``floor``, ``ceil``, ``fract``, ``mod``,
``min``, ``max``, ``clamp``, ``mix``, ``step``, ``smoothstep``,
``length``, ``distance``, ``dot``, ``cross``, ``normalize``,
``faceforward``, ``reflect``, ``refract``, ``any``, ``all`` etc.

All functions can be found in the `OpenGL Reference Page`_ 
(exclude functions starting with ``gl``).
Most of the functions exist in several overloaded versions
supporting different data types.

The basic setup for a shader is the following::

    #version 330

    void main() {
    }

The ``#version`` statement is mandatory and should at least be 330
(GLSL version 3.3 matching OpenGL version 3.3). The version statement
**should always be the first line in the source code**.
Higher version number is only needed if more fancy features are needed.
By the time you need those you probably know what you are doing.

What we also need to realize when working with shaders is that
they are executed in parallel across all the cores on your GPU.
This can be everything from tens, hundreds, thousands or more
cores. Even integrated GPUs today are very competent.

For those
who have not worked with shaders before it can be mind-boggling
to see the work they can get done in a matter of microseconds.
All shader executions / rendering calls are also asynchronous
running in the background while your python code is doing
other things (but certain operations can cause a "sync" stalling
until the shader program is done)

Vertex Shader (transforms)
--------------------------

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
Using modulus (``mod``) on ``gl_VertexID`` can get you pretty far.

.. _moderngl-window: https://github.com/moderngl/moderngl-window
.. _GPU: https://wikipedia.org/wiki/Graphics_processing_unit
.. _GLSL: https://www.khronos.org/opengl/wiki/OpenGL_Shading_Language
.. _data types: https://www.khronos.org/opengl/wiki/Data_Type_(GLSL)
.. _OpenGL Reference Page: https://www.khronos.org/registry/OpenGL-Refpages/gl4/
