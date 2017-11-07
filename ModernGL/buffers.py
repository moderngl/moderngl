'''
    ModernGL buffers
'''

from .common import InvalidObject


class BufferAccess:
    '''
        BufferAccess objects are designed to access
        a :py:class:`Buffer` object's content inside a ``with`` statement.
        The buffer is mapped and unmapped only once.

        Use :py:meth:`Buffer.access` to get a BufferAccess object.
    '''

    __slots__ = ['mglo']

    def __init__(self):
        self.mglo = None
        raise NotImplementedError()

    def __repr__(self):
        return '<BufferAccess>'

    def __enter__(self):
        self.mglo.open()
        return self

    def __exit__(self, *args):
        self.mglo.close()

    def __eq__(self, other):
        return type(self) is type(other) and self.mglo is other.mglo

    def __ne__(self, other):
        return type(self) is not type(other) or self.mglo is not other.mglo

    def open(self) -> None:
        '''
            Map the buffer.
            This method is called by ``__enter__``.
        '''

        self.mglo.open()

    def close(self) -> None:
        '''
            Unmap the buffer.
            This method is called by ``__exit__``.
        '''

        self.mglo.close()

    def read(self, size=-1, *, offset=0) -> bytes:
        '''
            Read the content.

            Args:
                size (int): The size. Value `-1` means all.

            Keyword Args:
                offset (int): The offset.

            Returns:
                bytes: binary data
        '''

        return self.mglo.read(size, offset)

    def read_into(self, buffer, size=-1, *, offset=0, write_offset=0) -> None:
        '''
            Read the content.

            Args:
                buffer (bytarray): The buffer that will receive the content.
                size (int): The size. Value `-1` means all.

            Keyword Args:
                offset (int): The read offset.
                write_offset (int): The write offset.

            Returns:
                bytes: binary data
        '''

        return self.mglo.read_into(buffer, size, offset, write_offset)

    def write(self, data, *, offset=0) -> None:
        '''
            Write the content.

            Args:
                data (bytes): The data.

            Keyword Args:
                offset (int): The offset.
        '''

        self.mglo.write(data, offset)

    def write_chunks(self, data, start, step, count) -> None:
        '''
            Split `data` to `count` equal parts.
            Write the chunks using offsets calculated from `start`, `step` and `stop`.

            Args:
                data (bytes): The data.
                start (int): First offset.
                step (int): Offset increment.
                count (int): The number of offsets.
        '''

        self.mglo.write_chunks(data, start, step, count)

    def read_chunks(self, chunk_size, start, step, count) -> bytes:
        '''
            Read the content.

            Args:
                chunk_size (int): The chunk size.
                start (int): First offset.
                step (int): Offset increment.
                count (int): The number of offsets.

            Returns:
                bytes: binary data
        '''

        return self.mglo.read_chunks(chunk_size, start, step, count)

    def read_chunks_into(self, buffer, chunk_size, start, step, count, *, write_offset=0) -> None:
        '''
            Read the content.

            Args:
                buffer (bytarray): The buffer that will receive the content.
                chunk_size (int): The chunk size.
                start (int): First offset.
                step (int): Offset increment.
                count (int): The number of offsets.

            Keyword Args:
                write_offset (int): The write offset.

            Returns:
                bytes: binary data
        '''

        return self.mglo.read(buffer, chunk_size, start, step, count, write_offset)

    @property
    def offset(self) -> int:
        '''
            int: The offset.
        '''

        return self.mglo.offset

    @property
    def size(self) -> int:
        '''
            int: The size.
        '''

        return self.mglo.size

    @property
    def readonly(self) -> bool:
        '''
            bool: Is readonly.
        '''

        return self.mglo.readonly


class Buffer:
    '''
        Buffer objects are OpenGL objects that store an array of unformatted memory
        allocated by the OpenGL context, (data allocated on the GPU).
        These can be used to store vertex data, pixel data retrieved from images
        or the framebuffer, and a variety of other things.

        A Buffer object cannot be instantiated directly, it requires a context.
        Use :py:meth:`Context.buffer` to create one.

        Copy buffer content using :py:meth:`Context.copy_buffer`.
    '''

    __slots__ = ['mglo']

    def __init__(self):
        self.mglo = None
        raise NotImplementedError()

    def __repr__(self):
        return '<Buffer: %d>' % self.glo

    def __eq__(self, other):
        return type(self) is type(other) and self.mglo is other.mglo

    def __ne__(self, other):
        return type(self) is not type(other) or self.mglo is not other.mglo

    @property
    def size(self) -> int:
        '''
            int: The size of the buffer.
        '''

        return self.mglo.size

    @property
    def dynamic(self) -> bool:
        '''
            bool: Is the buffer created with the dynamic flag?
        '''

        return self.mglo.dynamic

    @property
    def glo(self) -> int:
        '''
            int: The internal OpenGL object.
            This values is provided for debug purposes only.
        '''

        return self.mglo.glo

    def access(self, *, size=-1, offset=0, readonly=False) -> BufferAccess:
        '''
            Create a :py:class:`BufferAccess` object.

            Keyword Args:
                size (int): The size. Value `-1` means all.
                offset (int): The offset.
                readonly (bool): The readonly.

            Examples:

                Simple ``with`` statement::

                    # The buffer will be mapped once and accessed multiple times.

                    >>> with buffer.access() as access:
                    ...     access.read(...)
                    ...     access.write(...)
        '''

        res = BufferAccess.__new__(BufferAccess)
        res.mglo = self.mglo.access(size, offset, readonly)
        return res

    def read(self, size=-1, *, offset=0) -> bytes:
        '''
            Read the content.

            Args:
                size (int): The size. Value ``-1`` means all.

            Keyword Args:
                offset (int): The offset.

            Returns:
                bytes: The content of the buffer.
        '''

        return self.mglo.read(size, offset)

    def read_into(self, buffer, size=-1, *, offset=0, write_offset=0) -> None:
        '''
            Read the content into a buffer.

            Args:
                buffer (bytarray): The buffer that will receive the content.
                size (int): The size. Value ``-1`` means all.

            Keyword Args:
                offset (int): The read offset.
                write_offset (int): The write offset.

            Returns:
                bytes: The content of the buffer.
        '''

        return self.mglo.read_into(buffer, size, offset, write_offset)

    def write(self, data, *, offset=0) -> None:
        '''
            Write the content.

            Args:
                data (bytes): The data.

            Keyword Args:
                offset (int): The offset.
        '''

        self.mglo.write(data, offset)

    def write_chunks(self, data, start, step, count) -> None:
        '''
            Split `data` to `count` equal parts.
            Write the chunks using offsets calculated from `start`, `step` and `stop`.

            Args:
                data (bytes): The data.
                start (int): First offset.
                step (int): Offset increment.
                count (int): The number of offsets.
        '''

        self.mglo.write_chunks(data, start, step, count)

    def read_chunks(self, chunk_size, start, step, count) -> bytes:
        '''
            Read the content.

            Args:
                chunk_size (int): The chunk size.
                start (int): First offset.
                step (int): Offset increment.
                count (int): The number of offsets.

            Returns:
                bytes: binary data
        '''

        return self.mglo.read_chunks(chunk_size, start, step, count)

    def read_chunks_into(self, buffer, chunk_size, start, step, count, *, write_offset=0) -> None:
        '''
            Read the content.

            Args:
                buffer (bytarray): The buffer that will receive the content.
                chunk_size (int): The chunk size.
                start (int): First offset.
                step (int): Offset increment.
                count (int): The number of offsets.

            Keyword Args:
                write_offset (int): The write offset.

            Returns:
                bytes: binary data
        '''

        return self.mglo.read(buffer, chunk_size, start, step, count, write_offset)

    def clear(self, size=-1, *, offset=0, chunk=None) -> None:
        '''
            Clear the content.

            Args:
                size (int): The size. Value ``-1`` means all.

            Keyword Args:
                offset (int): The offset.
                chunk (bytes): The chunk to use repeatedly.
        '''

        self.mglo.clear(size, offset, chunk)

    def orphan(self) -> None:
        '''
            Orphan the buffer.

            It is also called buffer re-specification.

            Reallocate the buffer object before you start modifying it.

            Since allocating storage is likely faster than the implicit synchronization,
            you gain significant performance advantages over synchronization.

            The old storage will still be used by the OpenGL commands that have been sent previously.
            It is likely that the GL driver will not be doing any allocation at all,
            but will just be pulling an old free block off the unused buffer queue and use it,
            so it is likely to be very efficient.

            Examples:

                Simple orphaning example::

                    # For simplicity the VertexArray creation is omitted

                    >>> vbo = ctx.buffer(reserve=1024)

                    # Fill the buffer

                    >>> vbo.write(some_temorary_data)

                    # Issue a render call that uses the vbo

                    >>> vao.render(...)

                    # Orphan the buffer

                    >>> vbo.orphan()

                    # Issue another render call without waiting for the previous one

                    >>> vbo.write(some_temorary_data)
                    >>> vao.render(...)
        '''

        self.mglo.orphan()

    def bind_to_uniform_block(self, binding=0) -> None:
        '''
            Bind the buffer to a uniform block.

            Args:
                binding (int): The uniform block binding.
        '''

        self.mglo.bind_to_uniform_block(binding)

    def bind_to_storage_buffer(self, binding=0) -> None:
        '''
            Bind the buffer to a shader storage buffer.

            Args:
                binding (int): The shader storage binding.
        '''

        self.mglo.bind_to_storage_buffer(binding)

    def release(self) -> None:
        '''
            Release the ModernGL object.
        '''

        self.mglo.release()
        self.__class__ = InvalidObject


def detect_format(program, attributes) -> str:
    '''
        Detect format for vertex attributes.
        The format returned does not contain padding.

        Args:
            program (Program): The program.
            attributes (list): A list of attribute names.

        Returns:
            str: The tightly packed format for the attributes.
    '''

    def fmt(attr):
        '''
            For internal use only.
        '''

        return attr.array_length * attr.dimension, attr.shape

    return ''.join('%d%s' % fmt(program.attributes[a]) for a in attributes)
