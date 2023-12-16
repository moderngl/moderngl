.. py:currentmodule:: moderngl

Shader Introduction
===================

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
until the shader program is done).

Let's try to use the shader in the :doc:`simplest way (next step) <shader_transform>`.

.. _GPU: https://wikipedia.org/wiki/Graphics_processing_unit
.. _GLSL: https://www.khronos.org/opengl/wiki/OpenGL_Shading_Language
.. _data types: https://www.khronos.org/opengl/wiki/Data_Type_(GLSL)
.. _OpenGL Reference Page: https://www.khronos.org/registry/OpenGL-Refpages/gl4/
