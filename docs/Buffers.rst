Buffers
*******

.. currentmodule:: ModernGL

.. automethod:: Context.buffer(data=None, reserve=0, dynamic=False) -> Buffer
	:noindex:

.. autoclass:: Buffer
	:members: size, dynamic

	.. automethod:: access(size=-1, offset=0, readonly=False) -> BufferAccess
	.. automethod:: write(data, offset=0)
	.. automethod:: read(size=-1, offset=0) -> bytes
	.. automethod:: read_into(buffer, size=-1, offset=0, write_offset=0)
	.. automethod:: write_chunks(data, start, step, count)
	.. automethod:: read_chunks(chunk_size, start, step, count) -> bytes
	.. automethod:: read_chunks_into(buffer, chunk_size, start, step, count, write_offset=0)
	.. automethod:: clear(self, size=-1, offset=0, chunk=None)
	.. automethod:: orphan()
	.. automethod:: bind_to_uniform_block(binding=0)
	.. automethod:: bind_to_storage_buffer(binding=0)

.. autoclass:: BufferAccess
	:members: size, offset, readonly

	.. automethod:: open()
	.. automethod:: close()
	.. automethod:: write(data, offset=0)
	.. automethod:: read(size=-1, offset=0) -> bytes
	.. automethod:: read_into(buffer, size=-1, offset=0, write_offset=0)
	.. automethod:: write_chunks(data, start, step, count)
	.. automethod:: read_chunks(chunk_size, start, step, count) -> bytes
	.. automethod:: read_chunks_into(buffer, chunk_size, start, step, count, write_offset=0)

.. toctree::
	:maxdepth: 4
