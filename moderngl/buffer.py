from typing import Any, Tuple

from moderngl.mgl import InvalidObject  # type: ignore

__all__ = ['Buffer']


class Buffer:
    """
    Buffer objects are OpenGL objects that store an array of unformatted memory \
    allocated by the OpenGL context, (data allocated on the GPU).

    These can be used to store vertex data, pixel data retrieved from images
    or the framebuffer, and a variety of other things.

    A Buffer object cannot be instantiated directly, it requires a context.
    Use :py:meth:`Context.buffer` to create one.

    Copy buffer content using :py:meth:`Context.copy_buffer`.
    """

    __slots__ = ['mglo', '_size', '_dynamic', '_glo', 'ctx', 'extra']

    def __init__(self):
        self.mglo = None  #: Internal representation for debug purposes only.
        self._size = None  #: Original buffer size during creation
        self._dynamic = None
        self._glo = None
        self.ctx = None  #: The context this object belongs to
        self.extra = None  #: Any - Attribute for storing user defined objects
        raise TypeError()

    def __repr__(self) -> str:
        if hasattr(self, '_glo'):
            return f"<{self.__class__.__name__}: {self._glo}>"
        else:
            return f"<{self.__class__.__name__}: INCOMPLETE>"

    def __eq__(self, other: Any):
        return type(self) is type(other) and self.mglo is other.mglo

    def __hash__(self) -> int:
        return id(self)

    def __del__(self) -> None:
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    @property
    def size(self) -> int:
        """int: The size of the buffer in bytes."""
        return self.mglo.size()

    @property
    def dynamic(self) -> bool:
        """bool: Is the buffer created with the dynamic flag?."""
        return self._dynamic

    @property
    def glo(self) -> int:
        """
        int: The internal OpenGL object.

        This values is provided for debug purposes only.
        """
        return self._glo

    def write(self, data: Any, *, offset: int = 0) -> None:
        """
        Write the content.

        Args:
            data (bytes): The data.

        Keyword Args:
            offset (int): The offset in bytes.
        """
        self.mglo.write(data, offset)

    def write_chunks(self, data: Any, start: int, step: int, count: int) -> None:
        """
        Split data to count equal parts.

        Write the chunks using offsets calculated from start, step and stop.

        Args:
            data (bytes): The data.
            start (int): First offset in bytes.
            step (int): Offset increment in bytes.
            count (int): The number of offsets.
        """
        self.mglo.write_chunks(data, start, step, count)

    def read(self, size: int = -1, *, offset: int = 0) -> bytes:
        """
        Read the content.

        Args:
            size (int): The size in bytes. Value ``-1`` means all.

        Keyword Args:
            offset (int): The offset in bytes.

        Returns:
            bytes
        """
        return self.mglo.read(size, offset)

    def read_into(self, buffer: Any, size: int = -1, *, offset: int = 0, write_offset: int = 0) -> None:
        """
        Read the content into a buffer.

        Args:
            buffer (bytearray): The buffer that will receive the content.
            size (int): The size in bytes. Value ``-1`` means all.

        Keyword Args:
            offset (int): The read offset in bytes.
            write_offset (int): The write offset in bytes.
        """
        return self.mglo.read_into(buffer, size, offset, write_offset)

    def read_chunks(self, chunk_size: int, start: int, step: int, count: int) -> bytes:
        """
        Read the content.

        Read and concatenate the chunks of size chunk_size
        using offsets calculated from start, step and stop.

        Args:
            chunk_size (int): The chunk size in bytes.
            start (int): First offset in bytes.
            step (int): Offset increment  in bytes.
            count (int): The number of offsets.

        Returns:
            bytes
        """
        return self.mglo.read_chunks(chunk_size, start, step, count)

    def read_chunks_into(
        self,
        buffer: Any,
        chunk_size: int,
        start: int,
        step: int,
        count: int,
        *,
        write_offset: int = 0
    ) -> None:
        """
        Read the content.

        Read and concatenate the chunks of size chunk_size
        using offsets calculated from start, step and stop.

        Args:
            buffer (bytearray): The buffer that will receive the content.
            chunk_size (int): The chunk size.
            start (int): First offset.
            step (int): Offset increment.
            count (int): The number of offsets.

        Keyword Args:
            write_offset (int): The write offset.
        """
        return self.mglo.read(buffer, chunk_size, start, step, count, write_offset)

    def clear(self, size: int = -1, *, offset: int = 0, chunk: Any = None) -> None:
        """
        Clear the content.

        Args:
            size (int): The size. Value ``-1`` means all.

        Keyword Args:
            offset (int): The offset.
            chunk (bytes): The chunk to use repeatedly.
        """
        self.mglo.clear(size, offset, chunk)

    def bind_to_uniform_block(self, binding: int = 0, *, offset: int = 0, size: int = -1) -> None:
        """
        Bind the buffer to a uniform block.

        Args:
            binding (int): The uniform block binding.

        Keyword Args:
            offset (int): The offset.
            size (int): The size. Value ``-1`` means all.
        """
        self.mglo.bind_to_uniform_block(binding, offset, size)

    def bind_to_storage_buffer(self, binding: int = 0, *, offset: int = 0, size: int = -1) -> None:
        """
        Bind the buffer to a shader storage buffer.

        Args:
            binding (int): The shader storage binding.

        Keyword Args:
            offset (int): The offset.
            size (int): The size. Value ``-1`` means all.
        """
        self.mglo.bind_to_storage_buffer(binding, offset, size)

    def orphan(self, size: int = -1) -> None:
        """
        Orphan the buffer with the option to specify a new size.

        It is also called buffer re-specification.

        Reallocate the buffer object before you start modifying it.

        Since allocating storage is likely faster than the implicit synchronization,
        you gain significant performance advantages over synchronization.

        The old storage will still be used by the OpenGL commands that have been sent previously.
        It is likely that the GL driver will not be doing any allocation at all,
        but will just be pulling an old free block off the unused buffer queue and use it,
        so it is likely to be very efficient.

        Keyword Args:
            size (int): The new byte size if the buffer. If not supplied
                        the buffer size will be unchanged.

        .. rubric:: Example

        .. code-block:: python

            # For simplicity the VertexArray creation is omitted

            >>> vbo = ctx.buffer(reserve=1024)

            # Fill the buffer

            >>> vbo.write(some_temporary_data)

            # Issue a render call that uses the vbo

            >>> vao.render(...)

            # Orphan the buffer

            >>> vbo.orphan()

            # Issue another render call without waiting for the previous one

            >>> vbo.write(some_temporary_data)
            >>> vao.render(...)

            # We can also resize the buffer. In this case we double the size

            >> vbo.orphan(vbo.size * 2)
        """
        self.mglo.orphan(size)

    def release(self) -> None:
        """Release the ModernGL object."""
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()

    def bind(self, *attribs, layout=None):
        """
        Helper method for binding a buffer.

        Returns:
            (self, layout, attribs) tuple
        """
        return (self, layout, *attribs)

    def assign(self, index: int) -> Tuple["Buffer", int]:
        """
        Helper method for assigning a buffer.

        Returns:
            (self, index) tuple
        """
        return (self, index)
