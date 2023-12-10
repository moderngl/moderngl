.. py:currentmodule:: moderngl

Vertex Array
============

:py:class:`VertexArray` is something like a pipeline, where as arguments it takes a :py:class:`Program`, a :py:class:`Buffer` with input data, and the names of input variables for this program.

:py:class:`VertexArray` in ModernGL can be initialized in two ways: one buffer for all input variables or one buffer for each input variable.

One input buffer for all input variables (simple :py:class:`VertexArray` version)::
    
    vao = ctx.vertex_array(program, buffer, 'input_var1', 'input_var2')

One buffer for each input variable::
    
    vao = ctx.vertex_array(
        program,
        [
            (buffer1, '3f', 'input_var1'),
            (buffer2, '2f', 'input_var2')
        ]
    )
    
You can understand ``'3f'`` and ``'2f'`` as the type of the input variable, that is, 3 floats, which form ``vec3`` and 2 floats, which form ``vec2``.

In our example we use a simple implementation of this method.

.. rubric:: Entire source

.. literalinclude:: first.4.py
    :emphasize-lines: 42
    :linenos:

Proceed to the :doc:`next step <rendering>`.
