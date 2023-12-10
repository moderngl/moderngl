Buffer
======

.. py:class:: Buffer

    Returned by :py:meth:`Context.buffer`

    Buffer objects are OpenGL objects that store an array of unformatted memory \
    allocated by the OpenGL context, (data allocated on the GPU).

    These can be used to store vertex data, pixel data retrieved from images
    or the framebuffer, and a variety of other things.

    A Buffer object cannot be instantiated directly, it requires a context.
    Use :py:meth:`Context.buffer` to create one.

    Copy buffer content using :py:meth:`Context.copy_buffer`.

Methods
-------

.. py:method:: Buffer.write(data: Any, *, offset: int = 0) -> None:

    Write the content.

    :param bytes data: The data.
    :param int offset: The offset in bytes.

.. py:method:: Buffer.read(size: int = -1, *, offset: int = 0) -> bytes:

    Read the content.

    :param int size: The size in bytes. Value ``-1`` means all.
    :param int offset: The offset in bytes.

.. py:method:: Buffer.read_into(buffer: Any, size: int = -1, *, offset: int = 0, write_offset: int = 0) -> None:

    Read the content into a buffer.

    :param bytearray buffer: The buffer that will receive the content.
    :param int size: The size in bytes. Value ``-1`` means all.
    :param int offset: The read offset in bytes.
    :param int write_offset: The write offset in bytes.

.. py:method:: Buffer.clear(size: int = -1, *, offset: int = 0, chunk: Any = None) -> None:

    Clear the content.

    :param int size: The size. Value ``-1`` means all.
    :param int offset: The offset.
    :param bytes chunk: The chunk to use repeatedly.

.. py:method:: Buffer.bind_to_uniform_block(binding: int = 0, *, offset: int = 0, size: int = -1) -> None:

    Bind the buffer to a uniform block.

    :param int binding: The uniform block binding.
    :param int offset: The offset.
    :param int size: The size. Value ``-1`` means all.

.. py:method:: Buffer.bind_to_storage_buffer(binding: int = 0, *, offset: int = 0, size: int = -1) -> None:

    Bind the buffer to a shader storage buffer.

    :param int binding: The shader storage binding.
    :param int offset: The offset.
    :param int size: The size. Value ``-1`` means all.

.. py:method:: Buffer.release() -> None:

    Release the ModernGL object

.. py:method:: Buffer.bind(*attribs, layout=None) -> tuple:

    Helper method for binding a buffer in :py:meth:`Context.vertex_array`.

.. py:method:: Buffer.assign(index: int) -> tuple:

    Helper method for assigning a buffer to an index in :py:meth:`Context.scope`.

Attributes
----------

.. py:attribute:: Buffer.size
    :type: int

    The size of the buffer in bytes.

.. py:attribute:: Buffer.dynamic
    :type: bool

    The dynamic flag.

.. py:attribute:: Buffer.ctx
    :type: Context

    The context this object belongs to

.. py:attribute:: Buffer.glo
    :type: int

    The internal OpenGL object.
    This values is provided for interoperability and debug purposes only.

.. py:attribute:: Buffer.extra
    :type: Any

    User defined data.
