.. py:currentmodule:: moderngl

Vertex Array
============

:py:class:`VertexArray` is something like a pipeline, where as arguments :py:meth:`Context.vertex_array` takes a :py:class:`Program`, a :py:class:`Buffer` with input data, and the names of input variables for this program.

:py:class:`VertexArray` in ModernGL can be initialized in two ways: one buffer for all input variables or multiple buffers for all input variables.

One input buffer for all input variables (simple :py:class:`VertexArray` version)::
    
    vao = ctx.vertex_array(program, buffer, 'input_var1', 'input_var2')

Multiple input buffers for all input variables::
    
    vao = ctx.vertex_array(
        program,
        [
            (buffer1, '3f 2f', 'input_var1', 'input_var2'),
            (buffer2, '4f', 'input_var3')
        ]
    )
    
You can understand ``'3f 2f'`` and ``'4f'`` as the type of the input variable (or variables), that is, 3 floats and 2 floats, which form, for example, ``vec3 + vec2`` and 4 floats, which form ``vec4``.


In our example we use a simple implementation of this method.

.. rubric:: Entire source

.. literalinclude:: first.4.py
    :emphasize-lines: 42
    :linenos:

Proceed to the :doc:`next step <rendering>`.
