Query
=====

.. py:class:: Query

    Returned by :py:meth:`Context.query`

    This class represents a Query object.

Methods
-------

.. py:method:: Query.release() -> None

    Release the ModernGL object.

    The OpenGL query objects are deleted. Like for the other objects this is only
    done automatically when :py:attr:`Context.gc_mode` is set, see there.

Attributes
----------

.. py:attribute:: Query.samples
    :type: int

    The number of samples passed.

.. py:attribute:: Query.primitives
    :type: int

    The number of primitives generated.

.. py:attribute:: Query.elapsed
    :type: int

    The time elapsed in nanoseconds.

.. py:attribute:: Query.crender
    :type: ConditionalRender

.. py:attribute:: Query.ctx
    :type: Context

    The context this object belongs to

.. py:attribute:: Query.extra
    :type: Any

    User defined data.

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
    :maxdepth: 1

    conditional_render.rst
