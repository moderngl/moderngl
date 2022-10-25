Program
=======

.. py:currentmodule:: moderngl

.. autoclass:: moderngl.Program

Create
------

.. automethod:: Context.program
   :noindex:

Methods
-------

.. automethod:: Program.get
.. automethod:: Program.__getitem__
.. automethod:: Program.__setitem__
.. automethod:: Program.__iter__
.. automethod:: Program.__eq__
.. automethod:: Program.release

Attributes
----------

.. autoattribute:: Program.geometry_input
.. autoattribute:: Program.geometry_output
.. autoattribute:: Program.geometry_vertices
.. autoattribute:: Program.subroutines
.. autoattribute:: Program.glo
.. autoattribute:: Program.mglo
.. autoattribute:: Program.extra
.. autoattribute:: Program.is_transform
.. autoattribute:: Program.ctx

Examples
--------

.. rubric:: A simple program designed for rendering

.. code-block:: python
    :linenos:

    my_render_program = ctx.program(
        vertex_shader='''
            #version 330

            in vec2 vert;

            void main() {
                gl_Position = vec4(vert, 0.0, 1.0);
            }
        ''',
        fragment_shader='''
            #version 330

            out vec4 color;

            void main() {
                color = vec4(0.3, 0.5, 1.0, 1.0);
            }
        ''',
    )

.. rubric:: A simple program designed for transforming

.. code-block:: python
    :linenos:

    my_transform_program = ctx.program(
        vertex_shader='''
            #version 330

            in vec4 vert;
            out float vert_length;

            void main() {
                vert_length = length(vert);
            }
        ''',
        varyings=['vert_length']
    )

Program Members
---------------

.. toctree::
    :maxdepth: 2

    uniform.rst
    uniform_block.rst
    subroutine.rst
    attribute.rst
    varying.rst
