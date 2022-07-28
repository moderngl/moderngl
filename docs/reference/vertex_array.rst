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

.. automethod:: Context.simple_vertex_array
   :noindex:

.. automethod:: Context.vertex_array
   :noindex:

Methods
-------

.. automethod:: VertexArray.render
.. automethod:: VertexArray.render_indirect
.. automethod:: VertexArray.transform
.. automethod:: VertexArray.bind
.. automethod:: VertexArray.release

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
