Query
=====

.. py:currentmodule:: moderngl

.. autoclass:: moderngl.Query

Create
------

.. automethod:: Context.query
   :noindex:

Attributes
----------

.. autoattribute:: Query.samples
.. autoattribute:: Query.primitives
.. autoattribute:: Query.elapsed
.. autoattribute:: Query.crender
.. autoattribute:: Query.extra
.. autoattribute:: Query.mglo
.. autoattribute:: Query.ctx

Examples
--------

.. rubric:: Simple query example

.. code-block:: python
    :linenos:

    import moderngl
    import numpy as np

    ctx = moderngl.create_standalone_context()
    prog = ctx.program(
        vertex_shader='''
            #version 330

            in vec2 in_vert;

            void main() {
                gl_Position = vec4(in_vert, 0.0, 1.0);
            }
        ''',
        fragment_shader='''
            #version 330

            out vec4 color;

            void main() {
                color = vec4(1.0, 0.0, 0.0, 1.0);
            }
        ''',
    )

    vertices = np.array([
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
    ], dtype='f4')

    vbo = ctx.buffer(vertices.tobytes())
    vao = ctx.simple_vertex_array(prog, vbo, 'in_vert')

    fbo = ctx.simple_framebuffer((64, 64))
    fbo.use()

    query = ctx.query(samples=True, time=True)

    with query:
        vao.render()

    print('It took %d nanoseconds' % query.elapsed)
    print('to render %d samples' % query.samples)

.. rubric:: Output

.. code-block:: text

    It took 13529 nanoseconds
    to render 496 samples

.. toctree::
    :maxdepth: 2
