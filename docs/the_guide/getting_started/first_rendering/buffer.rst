.. py:currentmodule:: moderngl

Buffer
======

:py:class:`Buffer` is a dedicated area of GPU memory. Can store anything in bytes.

Creating a buffer is essentially allocating an area of memory into which data is later written. Therefore, when creating an empty buffer, it may contain memory fragments of deleted objects. Example of creating an empty buffer::

    buf = ctx.buffer(reserve=1)

The ``reserve`` parameter specifies how many bytes should be allocated, the minimum number is 1.

The buffer is cleared by writing zeros to the allocated memory area. However, this must be done manually::

    buf.clear()

ModernGL allows you to create 2 types of buffer: dynamic and non-dynamic. To do this, when creating a buffer, use the keyword argument ``dynamic=True/False`` (by default ``False``)::

    buf = ctx.buffer(reserve=32, dynamic=True)

.. Note::

    Using the ``dynamic=True`` parameter tells the GPU that actions with this :py:class:`Buffer` will be performed very often. This parameter is optional, but is recommended if the :py:class:`Buffer` is used frequently.

Later, using the :py:meth:`Buffer.orphan` function, you can change the buffer size at any time::

    buf.orphan(size=64)

After changing the buffer size, you will need to write data there. This is done using the :py:meth:`Buffer.write` function. This function will write data from RAM to GPU memory::

    buf.write(b'any bytes data')

However, if the size of this buffer was changed after it was added to :py:class:`VertexArray`, then when calling :py:meth:`VertexArray.render()` you will need to specify the new number of vertices in the ``vertices`` parameter. For example::

    # If from the contents of this buffer every 12 bytes fall on one vertex.
    vao.render(vertices=buf.size // 4 // 3)

The same will need to be done when calling the :py:meth:`VertexArray.transform` function::

    # If from the contents of this buffer every 12 bytes fall on one vertex.
    # output_buf - the buffer into which the transformation will be performed.
    vao.transform(output_buf, vertices=buf.size // 4 // 3)

After :py:meth:`VertexArray.transform` writes data to ``output_buf`` using a :doc:`vertex shader <../low_start/shader_transform>`, you may need to read it — use :py:meth:`Buffer.read`. This function will read data from GPU memory into RAM::

    bytes_data = buf.read()

.. Warning::

    Transferring data between RAM and GPU memory comes at a huge performance cost. It is recommended to use :py:meth:`Buffer.write` and :py:meth:`Buffer.read` as little as possible.

    If you just need to copy data between buffers, look towards the :py:meth:`Context.copy_buffer` function::

        ctx.copy_buffer(destination_buf, source_buf)

In our example, we simply create a static buffer and write data immediately when it is created::

    vbo = ctx.buffer(vertices.astype("f4").tobytes())

We called it `VBO`_ (Vertex Buffer Object) because we will store vertex data in this buffer.

.. Sidebar::

    .. Note::
        For the convenience of transferring data to the GPU memory [in a dedicated :py:class:`Buffer` area], here we use the `NumPy`_ library.

        `NumPy`_ installation::

            pip install numpy

        If you want fewer dependencies, you can try Python's built-in `struct`_ module with the ``struct.pack()`` and ``struct.unpack()`` methods.

.. rubric:: Entire source

.. literalinclude:: first.3.py
    :emphasize-lines: 2,33-
    :linenos:

Proceed to the :doc:`next step <vertex_array>`.

.. _NumPy: https://github.com/numpy/numpy
.. _struct: https://docs.python.org/3/library/struct.html
.. _VBO: https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
