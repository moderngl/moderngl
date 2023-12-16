.. py:currentmodule:: moderngl

Program
=======

ModernGL is different from standard plotting libraries.
You can define your own shader program to render stuff.
This could complicate things, but also provides freedom on how you render
your data.

Previously, we looked at creating a :doc:`vertex shader-only program <../low_start/shader_transform>` that could only read data from the input buffer and write the converted data to the output buffer. Now let's add a fragment shader to our program; it will allow us to create an algorithm for writing pixels into a texture, that is, perform the :doc:`main function <../low_start/shader_basics>` of the shader.

Here is a sample program that passes the input vertex coordinates as is to
screen coordinates.

Screen coordinates are in the [-1, 1], [-1, 1] range for x and y axes.
The (-1, -1) point is the lower left corner of the screen.

.. figure:: screen_coordinates.png
    :alt: Screen Coordinates
    :align: center
    :figclass: align-center

    The screen coordinates

The program will also process a color information.

.. rubric:: Entire source

.. literalinclude:: first.2.py
    :emphasize-lines: 5-
    :linenos:

.. rubric:: Vertex Shader

.. literalinclude:: first.2.py
    :language: glsl
    :dedent: 8
    :lines: 7-17

.. rubric:: Fragment Shader

.. literalinclude:: first.2.py
    :language: glsl
    :dedent: 8
    :lines: 20-28

Proceed to the :doc:`next step <buffer>`.
