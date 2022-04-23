VertexArray
===========

.. py:currentmodule:: moderngl

.. autoclass:: moderngl.VertexArray

    .. note::
        Compared to OpenGL, :py:class:`VertexArray` objects have some additional responsibilities:

        - Binding a :py:class:`Program` when :py:meth:`VertexArray.render` or :py:meth:`VertexArray.transform` is called.
        - Subroutines can be assigned. Please see the example below.

Create
------

.. automethod:: Context.simple_vertex_array(program: Program, buffer: Buffer, *attributes: Union[List[str], Tuple[str, ...]], index_buffer: Optional[Buffer] = None, index_element_size: int = 4, mode: Optional[int] = None) -> VertexArray
    :noindex:

.. automethod:: Context.vertex_array(*args, **kwargs) -> VertexArray
    :noindex:

Methods
-------

.. automethod:: VertexArray.render(mode: Optional[int] = None, vertices: int = -1, first: int = 0, instances: int = -1)
.. automethod:: VertexArray.render_indirect(buffer: 'Buffer', mode: Optional[int] = None, count: int = -1, first: int = 0)
.. automethod:: VertexArray.transform(buffer: Union['Buffer', List['Buffer']], mode: int = None, vertices: int = -1, first: int = 0, instances: int = -1, buffer_offset: int = 0)
.. automethod:: VertexArray.bind(attribute: int, cls: str, buffer: 'Buffer', fmt: str, offset: int = 0, stride: int = 0, divisor: int = 0, normalize: bool = False)
.. automethod:: VertexArray.release()

Attributes
----------

.. autoattribute:: VertexArray.mode
.. autoattribute:: VertexArray.program
.. autoattribute:: VertexArray.index_buffer
.. autoattribute:: VertexArray.index_element_size
.. autoattribute:: VertexArray.scope
.. autoattribute:: VertexArray.vertices
.. autoattribute:: VertexArray.instances
.. autoattribute:: VertexArray.subroutines
.. autoattribute:: VertexArray.glo
.. autoattribute:: VertexArray.mglo
.. autoattribute:: VertexArray.extra
.. autoattribute:: VertexArray.ctx

.. toctree::
    :maxdepth: 2
