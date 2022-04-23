Program
=======

.. py:currentmodule:: moderngl

.. autoclass:: moderngl.Program

Create
------

.. automethod:: Context.program(vertex_shader: str, fragment_shader: Optional[str] = None, geometry_shader: Optional[str] = None, tess_control_shader: Optional[str] = None, tess_evaluation_shader: Optional[str] = None, varyings: Tuple[str, ...] = (), fragment_outputs: Optional[Dict[str, int]] = None, varyings_capture_mode: str = 'interleaved') -> Program
    :noindex:

Methods
-------

.. automethod:: Program.get(key: str, default: Any) -> Union[Uniform, UniformBlock, Subroutine, Attribute, Varying]
.. automethod:: Program.__getitem__(key: str) -> Union[Uniform, UniformBlock, Subroutine, Attribute, Varying]
.. automethod:: Program.__setitem__(key: str, value: Any)
.. automethod:: Program.__iter__() -> Generator[str, NoneType, NoneType]
.. automethod:: Program.__eq__(other: Any) -> bool
.. automethod:: Program.release()


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
