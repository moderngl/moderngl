from __future__ import annotations

from typing import Any, Deque, Dict, Generator, List, Optional, Protocol, Set, Tuple, Union

class ConvertibleToShaderSource(Protocol):
    def to_shader_source(self) -> str | bytes: ...

POINTS: int
"""ctx.POINTS"""

LINES: int
"""ctx.LINES"""

LINE_LOOP: int
"""ctx.LINE_LOOP"""

LINE_STRIP: int
"""ctx.LINE_STRIP"""

TRIANGLES: int
"""ctx.TRIANGLES"""

TRIANGLE_STRIP: int
"""ctx.TRIANGLE_STRIP"""

TRIANGLE_FAN: int
"""ctx.TRIANGLE_FAN"""

LINES_ADJACENCY: int
"""ctx.LINES_ADJACENCY"""

LINE_STRIP_ADJACENCY: int
"""ctx.LINE_STRIP_ADJACENCY"""

TRIANGLES_ADJACENCY: int
"""ctx.TRIANGLES_ADJACENCY"""

TRIANGLE_STRIP_ADJACENCY: int
"""ctx.TRIANGLE_STRIP_ADJACENCY"""

PATCHES: int
"""ctx.PATCHES"""

NEAREST: int
"""ctx.NEAREST"""

LINEAR: int
"""ctx.LINEAR"""

NEAREST_MIPMAP_NEAREST: int
"""ctx.NEAREST_MIPMAP_NEAREST"""

LINEAR_MIPMAP_NEAREST: int
"""ctx.LINEAR_MIPMAP_NEAREST"""

NEAREST_MIPMAP_LINEAR: int
"""ctx.NEAREST_MIPMAP_LINEAR"""

LINEAR_MIPMAP_LINEAR: int
"""ctx.LINEAR_MIPMAP_LINEAR"""

NOTHING: int
"""ctx.NOTHING"""

BLEND: int
"""ctx.BLEND"""

DEPTH_TEST: int
"""ctx.DEPTH_TEST"""

CULL_FACE: int
"""ctx.CULL_FACE"""

RASTERIZER_DISCARD: int
"""ctx.RASTERIZER_DISCARD"""

PROGRAM_POINT_SIZE: int
"""ctx.PROGRAM_POINT_SIZE"""

ZERO: int
"""ctx.ZERO"""

ONE: int
"""ctx.ONE"""

SRC_COLOR: int
"""ctx.SRC_COLOR"""

ONE_MINUS_SRC_COLOR: int
"""ctx.ONE_MINUS_SRC_COLOR"""

SRC_ALPHA: int
"""ctx.SRC_ALPHA"""

ONE_MINUS_SRC_ALPHA: int
"""ctx.ONE_MINUS_SRC_ALPHA"""

DST_ALPHA: int
"""ctx.DST_ALPHA"""

ONE_MINUS_DST_ALPHA: int
"""ctx.ONE_MINUS_DST_ALPHA"""

DST_COLOR: int
"""ctx.DST_COLOR"""

ONE_MINUS_DST_COLOR: int
"""ctx.ONE_MINUS_DST_COLOR"""

FUNC_ADD: int
"""ctx.FUNC_ADD"""

FUNC_SUBTRACT: int
"""ctx.FUNC_SUBTRACT"""

FUNC_REVERSE_SUBTRACT: int
"""ctx.FUNC_REVERSE_SUBTRACT"""

MIN: int
"""ctx.MIN"""

MAX: int
"""ctx.MAX"""

DEFAULT_BLENDING: int
"""ctx.DEFAULT_BLENDING"""

ADDITIVE_BLENDING: int
"""ctx.ADDITIVE_BLENDING"""

PREMULTIPLIED_ALPHA: int
"""ctx.PREMULTIPLIED_ALPHA"""

FIRST_VERTEX_CONVENTION: int
"""ctx.FIRST_VERTEX_CONVENTION"""

LAST_VERTEX_CONVENTION: int
"""ctx.LAST_VERTEX_CONVENTION"""

VERTEX_ATTRIB_ARRAY_BARRIER_BIT: int
"""ctx.VERTEX_ATTRIB_ARRAY_BARRIER_BIT"""

ELEMENT_ARRAY_BARRIER_BIT: int
"""ctx.ELEMENT_ARRAY_BARRIER_BIT"""

UNIFORM_BARRIER_BIT: int
"""ctx.UNIFORM_BARRIER_BIT"""

TEXTURE_FETCH_BARRIER_BIT: int
"""ctx.TEXTURE_FETCH_BARRIER_BIT"""

SHADER_IMAGE_ACCESS_BARRIER_BIT: int
"""ctx.SHADER_IMAGE_ACCESS_BARRIER_BIT"""

COMMAND_BARRIER_BIT: int
"""ctx.COMMAND_BARRIER_BIT"""

PIXEL_BUFFER_BARRIER_BIT: int
"""ctx.PIXEL_BUFFER_BARRIER_BIT"""

TEXTURE_UPDATE_BARRIER_BIT: int
"""ctx.TEXTURE_UPDATE_BARRIER_BIT"""

BUFFER_UPDATE_BARRIER_BIT: int
"""ctx.BUFFER_UPDATE_BARRIER_BIT"""

FRAMEBUFFER_BARRIER_BIT: int
"""ctx.FRAMEBUFFER_BARRIER_BIT"""

TRANSFORM_FEEDBACK_BARRIER_BIT: int
"""ctx.TRANSFORM_FEEDBACK_BARRIER_BIT"""

ATOMIC_COUNTER_BARRIER_BIT: int
"""ctx.ATOMIC_COUNTER_BARRIER_BIT"""

SHADER_STORAGE_BARRIER_BIT: int
"""ctx.SHADER_STORAGE_BARRIER_BIT"""

ALL_BARRIER_BITS: int
"""ctx.ALL_BARRIER_BITS"""

class Attribute:
    """
    Represents a program attribute.
    """

    location: int
    """
    The location of the attribute.
    The result of the glGetAttribLocation.
    """

    array_length: int
    """
    The length of the array and `1` for scalars.
    """

    dimension: int
    """
    The attribute dimension.
    """

    shape: int
    """
    A single character, representing the scalar type of the attribute.
    """

    name: str
    """
    The Attribute name without array index suffixes.
    """

    extra: Any
    """
    User Data
    """

    mglo: Any
    """
    Internal moderngl core object
    """

class Uniform:
    """
    A uniform is a global GLSL variable declared with the 'uniform' storage qualifier.

    These act as parameters that the user of a shader program can pass to that program.

    In ModernGL, Uniforms can be accessed using :py:meth:`Program.__getitem__`
    or :py:meth:`Program.__iter__`
    """

    location: int
    """
    The location of the uniform.
    """

    dimension: int
    """
    The dimension of the uniform.
    """

    array_length: int
    """
    The length of the array of the uniform.
    The array_length is `1` for non array uniforms.
    """

    name: str
    """
    The name of the uniform.

    The name does not contain leading `[0]`.
    The name may contain `[ ]` when the uniform is part of a struct.
    """

    value: Any
    """
    The value of the uniform.

    Reading the value of the uniform may force the GPU to sync.

    The value must be a tuple for non array uniforms.
    The value must be a list of tuples for array uniforms.
    """

    handle: int
    """
    The handle of the uniform.

    Only valid for uniform textures when using Bindless Textures.
    """

    extra: Any
    """
    Attribute for storing user defined objects
    """

    mglo: Any
    """
    Internal moderngl core object
    """

    def read(self) -> bytes:
        """
        Read the value of the uniform.
        """
    def write(self, data: Any) -> None:
        """
        Write the value of the uniform.
        """

class UniformBlock:
    """
    Uniform Block metadata
    """

    binding: int
    """
    The binding of the uniform block.
    """

    value: int
    """
    The value of the uniform block.
    """

    name: str
    """
    The name of the uniform block.
    """

    index: int
    """
    The index of the uniform block.
    """

    size: int
    """
    The size of the uniform block.
    """

    extra: Any
    """
    Attribute for storing user defined objects
    """

    mglo: Any
    """
    Internal moderngl core object
    """

class StorageBlock:
    """
    Storage Block metadata
    """

    binding: int
    """
    The binding of the storage block
    """

    value: int
    """
    The value of the storage block.
    """

    name: str
    """
    The name of the storage block.
    """

    index: int
    """
    The index of the storage block.
    """

    extra: Any
    """
    Attribute for storing user defined objects
    """

    mglo: Any
    """
    Internal moderngl core object
    """

class Varying:
    """
    This class represents a program varying.
    """

    number: int
    """
    The number of the varying.
    """

    name: str
    """
    The name of the varying.
    """

    extra: Any
    """
    Attribute for storing user defined objects
    """

    mglo: Any
    """
    Internal moderngl core object
    """

class Error(Exception):
    """
    Generic moderngl error.
    """

class InvalidObject:
    """
    DEPRECATED
    """

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

    size: int
    """The size of the buffer in bytes."""

    dynamic: bool
    """Is the buffer created with the dynamic flag?."""

    mglo: Any
    """Internal representation for debug purposes only."""

    ctx: Context
    """The context this object belongs to"""

    extra: Any
    """Attribute for storing user defined objects"""

    glo: int
    """
    The internal OpenGL object.

    This values is provided for debug purposes only.
    """

    def write(self, data: Any, offset: int = 0) -> None:
        """
        Write the content.

        Args:
            data (bytes): The data.

        Keyword Args:
            offset (int): The offset in bytes.
        """
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
    def read(self, size: int = -1, offset: int = 0) -> bytes:
        """
        Read the content.

        Args:
            size (int): The size in bytes. Value ``-1`` means all.

        Keyword Args:
            offset (int): The offset in bytes.

        Returns:
            bytes
        """
    def read_into(self, buffer: Any, size: int = -1, offset: int = 0, write_offset: int = 0) -> None:
        """
        Read the content into a buffer.

        Args:
            buffer (bytearray): The buffer that will receive the content.
            size (int): The size in bytes. Value ``-1`` means all.

        Keyword Args:
            offset (int): The read offset in bytes.
            write_offset (int): The write offset in bytes.
        """
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
    def read_chunks_into(
        self,
        buffer: Any,
        chunk_size: int,
        start: int,
        step: int,
        count: int,
        write_offset: int = 0,
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
    def clear(self, size: int = -1, offset: int = 0, chunk: Any = None) -> None:
        """
        Clear the content.

        Args:
            size (int): The size. Value ``-1`` means all.

        Keyword Args:
            offset (int): The offset.
            chunk (bytes): The chunk to use repeatedly.
        """
    def bind_to_uniform_block(self, binding: int = 0, offset: int = 0, size: int = -1) -> None:
        """
        Bind the buffer to a uniform block.

        Args:
            binding (int): The uniform block binding.

        Keyword Args:
            offset (int): The offset.
            size (int): The size. Value ``-1`` means all.
        """
    def bind_to_storage_buffer(self, binding: int = 0, offset: int = 0, size: int = -1) -> None:
        """
        Bind the buffer to a shader storage buffer.

        Args:
            binding (int): The shader storage binding.

        Keyword Args:
            offset (int): The offset.
            size (int): The size. Value ``-1`` means all.
        """
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
    def release(self) -> None:
        """Release the ModernGL object."""
    def bind(self, *attribs, layout=None):
        """
        Helper method for binding a buffer.

        Returns:
            (self, layout, attribs) tuple
        """
    def assign(self, index: int) -> Tuple[Buffer, int]:
        """
        Helper method for assigning a buffer.

        Returns:
            (self, index) tuple
        """

class ComputeShader:
    """
    A Compute Shader is a Shader Stage that is used entirely for computing arbitrary information.

    While it can do rendering, it is generally used
    for tasks not directly related to drawing.

    - Compute shaders support uniforms are other member object just like a
        :py:class:`moderngl.Program`.
    - Storage buffers can be bound using :py:meth:`Buffer.bind_to_storage_buffer`.
    - Uniform buffers can be bound using :py:meth:`Buffer.bind_to_uniform_block`.
    - Images can be bound using :py:meth:`Texture.bind_to_image`.
    """

    mglo: Any
    """Internal representation for debug purposes only."""

    ctx: Context
    """The context this object belongs to"""

    extra: Any
    """Attribute for storing user defined objects"""

    glo: int
    """
    The internal OpenGL object.

    This values is provided for debug purposes only.
    """

    def __getitem__(self, key: str) -> Union[Uniform, UniformBlock, StorageBlock]:
        """
        Get a member such as uniforms, uniform blocks and storage blocks.

        .. code-block:: python

            # Get a uniform
            uniform = program['color']

            # Uniform values can be set on the returned object
            # or the `__setitem__` shortcut can be used.
            program['color'].value = 1.0, 1.0, 1.0, 1.0

            # Still when writing byte data we need to use the `write()` method
            program['color'].write(buffer)

            # Set binding for a storage block (if supported)
            program['DataBlock'].binding = 0
        """
    def __setitem__(self, key: str, value: Any):
        """
        Set a value of uniform or uniform block.

        .. code-block:: python

            # Set a vec4 uniform
            uniform['color'] = 1.0, 1.0, 1.0, 1.0

            # Optionally we can store references to a member and set the value directly
            uniform = program['color']
            uniform.value = 1.0, 0.0, 0.0, 0.0

            uniform = program['cameraMatrix']
            uniform.write(camera_matrix)

            # Set binding for a storage block (if supported)
            program['DataBlock'].binding = 0
        """
    def __iter__(self) -> Generator[str, None, None]:
        """
        Yields the internal members names as strings.

        Example::

            for member in program:
                obj = program[member]
                print(member, obj)
                if isinstance(obj, moderngl.StorageBlock):
                    print('This is a storage block member')

        This includes all members such as uniforms, uniform blocks and storage blocks.
        """
    def run(self, group_x: int = 1, group_y: int = 1, group_z: int = 1) -> None:
        """
        Run the compute shader.

        Args:
            group_x (int): The number of work groups to be launched in the X dimension.
            group_y (int): The number of work groups to be launched in the Y dimension.
            group_z (int): The number of work groups to be launched in the Z dimension.
        """
    def run_indirect(self, buffer: Buffer, offset: int = 0) -> None:
        """
        Run the compute shader.
        """
    def get(self, key: str, default: Any) -> Union[Uniform, UniformBlock, Attribute, Varying]:
        """
        Returns a Uniform, UniformBlock, Attribute or Varying.

        Args:
            default: This is the value to be returned in case key does not exist.

        Returns:
            :py:class:`Uniform`, :py:class:`UniformBlock`, :py:class:`Attribute` or :py:class:`Varying`
        """
    def release(self) -> None:
        """Release the ModernGL object."""

class ConditionalRender:
    """
    This class represents a ConditionalRender object.

    ConditionalRender objects can only be accessed from :py:class:`Query` objects.
    """

    mglo: Any
    """Internal representation for debug purposes only."""

    extra: Any
    """Attribute for storing user defined objects"""

    def __enter__(self): ...
    def __exit__(self, *args): ...

class Context:
    """
    Class exposing OpenGL features.

    ModernGL objects can be created from this class.
    """

    NOTHING: int
    """Represents no states. Can be used with :py:meth:`Context.enable_only` to disable all states."""

    BLEND: int
    """Enable/disable blending"""

    DEPTH_TEST: int
    """Enable/disable depth testing"""

    CULL_FACE: int
    """Enable/disable face culling"""

    RASTERIZER_DISCARD: int
    """Enable/disable rasterization

    Context flag: Enables ``gl_PointSize`` in vertex or geometry shaders.

    When enabled we can write to ``gl_PointSize`` in the vertex shader to specify the point size
    for each individual point.

    If this value is not set in the shader the behavior is undefined. This means the points may
    or may not appear depending if the drivers enforce some default value for ``gl_PointSize``."""

    PROGRAM_POINT_SIZE: int
    """When disabled :py:attr:`Context.point_size` is used."""

    POINTS: int
    """Each vertex represents a point"""

    LINES: int
    """Vertices 0 and 1 are considered a line. Vertices 2 and 3 are considered a line.
    And so on. If the user specifies a non-even number of vertices, then the extra vertex is ignored."""

    LINE_LOOP: int
    """As line strips, except that the first and last vertices are also used as a line.
    Thus, you get n lines for n input vertices. If the user only specifies 1 vertex,
    the drawing command is ignored. The line between the first and last vertices happens
    after all of the previous lines in the sequence."""

    LINE_STRIP: int
    """The adjacent vertices are considered lines. Thus, if you pass n vertices, you will get n-1 lines.
    If the user only specifies 1 vertex, the drawing command is ignored."""

    TRIANGLES: int
    """Vertices 0, 1, and 2 form a triangle. Vertices 3, 4, and 5 form a triangle. And so on."""

    TRIANGLE_STRIP: int
    """Every group of 3 adjacent vertices forms a triangle. The face direction of the
    strip is determined by the winding of the first triangle. Each successive triangle
    will have its effective face order reversed, so the system compensates for that
    by testing it in the opposite way. A vertex stream of n length will generate n-2 triangles."""

    TRIANGLE_FAN: int
    """The first vertex is always held fixed. From there on, every group of 2 adjacent
    vertices form a triangle with the first. So with a vertex stream, you get a list
    of triangles like so: (0, 1, 2) (0, 2, 3), (0, 3, 4), etc. A vertex stream of
    n length will generate n-2 triangles."""

    LINES_ADJACENCY: int
    """These are special primitives that are expected to be used specifically with
    geomtry shaders. These primitives give the geometry shader more vertices
    to work with for each input primitive. Data needs to be duplicated in buffers."""

    LINE_STRIP_ADJACENCY: int
    """These are special primitives that are expected to be used specifically with
    geomtry shaders. These primitives give the geometry shader more vertices
    to work with for each input primitive. Data needs to be duplicated in buffers."""

    TRIANGLES_ADJACENCY: int
    """These are special primitives that are expected to be used specifically with
    geomtry shaders. These primitives give the geometry shader more vertices
    to work with for each input primitive. Data needs to be duplicated in buffers."""

    TRIANGLE_STRIP_ADJACENCY: int
    """These are special primitives that are expected to be used specifically with
    geomtry shaders. These primitives give the geometry shader more vertices
    to work with for each input primitive. Data needs to be duplicated in buffers."""

    PATCHES: int
    """primitive type can only be used when Tessellation is active. It is a primitive
    with a user-defined number of vertices, which is then tessellated based on the
    control and evaluation shaders into regular points, lines, or triangles, depending
    on the TES's settings."""

    NEAREST: int
    """Returns the value of the texture element that is nearest
    (in Manhattan distance) to the specified texture coordinates."""

    LINEAR: int
    """Returns the weighted average of the four texture elements
    that are closest to the specified texture coordinates.
    These can include items wrapped or repeated from other parts
    of a texture, depending on the values of texture repeat mode,
    and on the exact mapping."""

    NEAREST_MIPMAP_NEAREST: int
    """Chooses the mipmap that most closely matches the size of the
    pixel being textured and uses the ``NEAREST`` criterion (the texture
    element closest to the specified texture coordinates) to produce
    a texture value."""

    LINEAR_MIPMAP_NEAREST: int
    """Chooses the mipmap that most closely matches the size of the pixel
    being textured and uses the ``LINEAR`` criterion (a weighted average
    of the four texture elements that are closest to the specified
    texture coordinates) to produce a texture value."""

    NEAREST_MIPMAP_LINEAR: int
    """Chooses the two mipmaps that most closely match the size of the
    pixel being textured and uses the ``NEAREST`` criterion (the texture
    element closest to the specified texture coordinates ) to produce
    a texture value from each mipmap. The final texture value is a
    weighted average of those two values."""

    LINEAR_MIPMAP_LINEAR: int
    """Chooses the two mipmaps that most closely match the size of the pixel
    being textured and uses the ``LINEAR`` criterion (a weighted average
    of the texture elements that are closest to the specified texture
    coordinates) to produce a texture value from each mipmap.
    The final texture value is a weighted average of those two values."""

    ZERO: int
    """(0,0,0,0)"""

    ONE: int
    """(1,1,1,1)"""

    SRC_COLOR: int
    """(Rs0/kR,Gs0/kG,Bs0/kB,As0/kA)"""

    ONE_MINUS_SRC_COLOR: int
    """(1,1,1,1) - (Rs0/kR,Gs0/kG,Bs0/kB,As0/kA)"""

    SRC_ALPHA: int
    """(As0/kA,As0/kA,As0/kA,As0/kA)"""

    ONE_MINUS_SRC_ALPHA: int
    """(1,1,1,1) - (As0/kA,As0/kA,As0/kA,As0/kA)"""

    DST_ALPHA: int
    """(Ad/kA,Ad/kA,Ad/kA,Ad/kA)"""

    ONE_MINUS_DST_ALPHA: int
    """(1,1,1,1) - (Ad/kA,Ad/kA,Ad/kA,Ad/kA)"""

    DST_COLOR: int
    """(Rd/kR,Gd/kG,Bd/kB,Ad/kA)"""

    ONE_MINUS_DST_COLOR: int
    """(1,1,1,1) - (Rd/kR,Gd/kG,Bd/kB,Ad/kA)"""

    DEFAULT_BLENDING: int
    """Shotcut for the default blending ``SRC_ALPHA, ONE_MINUS_SRC_ALPHA``"""

    ADDITIVE_BLENDING: int
    """Shotcut for additive blending ``ONE, ONE``"""

    PREMULTIPLIED_ALPHA: int
    """Shotcut for blend mode when using premultiplied alpha ``SRC_ALPHA, ONE``"""

    FUNC_ADD: int
    """source + destination"""

    FUNC_SUBTRACT: int
    """source - destination"""

    FUNC_REVERSE_SUBTRACT: int
    """destination - source"""

    MIN: int
    """Minimum of source and destination"""

    MAX: int
    """Maximum of source and destination"""

    FIRST_VERTEX_CONVENTION: int
    """Specifies the first vertex should be used as the source of data for flat shaded varyings.
    Used with :py:attr:`Context.provoking_vertex`."""

    LAST_VERTEX_CONVENTION: int
    """Specifies the last vertex should be used as the source of data for flat shaded varyings.
    Used with :py:attr:`Context.provoking_vertex`."""

    VERTEX_ATTRIB_ARRAY_BARRIER_BIT: int
    """
    VERTEX_ATTRIB_ARRAY_BARRIER_BIT
    """

    ELEMENT_ARRAY_BARRIER_BIT: int
    """
    ELEMENT_ARRAY_BARRIER_BIT
    """

    UNIFORM_BARRIER_BIT: int
    """
    UNIFORM_BARRIER_BIT
    """

    TEXTURE_FETCH_BARRIER_BIT: int
    """
    TEXTURE_FETCH_BARRIER_BIT
    """

    SHADER_IMAGE_ACCESS_BARRIER_BIT: int
    """
    SHADER_IMAGE_ACCESS_BARRIER_BIT
    """

    COMMAND_BARRIER_BIT: int
    """
    COMMAND_BARRIER_BIT
    """

    PIXEL_BUFFER_BARRIER_BIT: int
    """
    PIXEL_BUFFER_BARRIER_BIT
    """

    TEXTURE_UPDATE_BARRIER_BIT: int
    """
    TEXTURE_UPDATE_BARRIER_BIT
    """

    BUFFER_UPDATE_BARRIER_BIT: int
    """
    BUFFER_UPDATE_BARRIER_BIT
    """

    FRAMEBUFFER_BARRIER_BIT: int
    """
    FRAMEBUFFER_BARRIER_BIT
    """

    TRANSFORM_FEEDBACK_BARRIER_BIT: int
    """
    TRANSFORM_FEEDBACK_BARRIER_BIT
    """

    ATOMIC_COUNTER_BARRIER_BIT: int
    """
    ATOMIC_COUNTER_BARRIER_BIT
    """

    SHADER_STORAGE_BARRIER_BIT: int
    """
    SHADER_STORAGE_BARRIER_BIT
    """

    ALL_BARRIER_BITS: int
    """
    ALL_BARRIER_BITS
    """

    version_code: int
    """The OpenGL version code. Reports ``410`` for OpenGL 4.1"""

    gc_mode: str
    """
    The garbage collection mode.

    The default mode is ``None`` meaning no automatic
    garbage collection is done. Other modes are ``auto``
    and ``context_gc``. Please see documentation for
    the appropriate configuration.

    Examples::

        # Disable automatic garbage collection.
        # Each objects needs to be explicitly released.
        ctx.gc_mode = None

        # Collect all dead objects in the context and
        # release them by calling Context.gc()
        ctx.gc_mode = 'context_gc'
        ctx.gc()  # Deletes the collected objects

        # Enable automatic garbage collection like
        # we normally expect in python.
        ctx.gc_mode = 'auto'
    """

    objects: Deque[Any]
    """
    Moderngl objects scheduled for deletion.

    These are deleted when calling :py:meth:`Context.gc`.
    """

    def gc(self) -> int:
        """
        Deletes OpenGL objects.

        This method must be called to garbage collect
        OpenGL resources when ``gc_mode`` is ``'context_gc'```.

        Calling this method with any other ``gc_mode`` configuration
        has no effect and is perfectly safe.

        Returns:
            int: Number of objects deleted
        """
    line_width: float
    """
    Set the default line width.

    .. Warning:: A line width other than 1.0 is not guaranteed to work
                    across different OpenGL implementations. For wide
                    lines you should be using geometry shaders.

    """

    point_size: float
    """
    Set/get the point size.

    Point size changes the pixel size of rendered points. The min and max values
    are limited by ``POINT_SIZE_RANGE``.
    This value usually at least ``(1, 100)``, but this depends on the drivers/vendors.

    If variable point size is needed you can enable ``PROGRAM_POINT_SIZE``
    and write to ``gl_PointSize`` in the vertex or geometry shader.

    .. Note::

        Using a geometry shader to create triangle strips from points is often a safer
        way to render large points since you don't have have any size restrictions.
    """

    depth_func: str
    """
    Set the default depth func.

    Example::

        ctx.depth_func = '<='  # GL_LEQUAL
        ctx.depth_func = '<'   # GL_LESS
        ctx.depth_func = '>='  # GL_GEQUAL
        ctx.depth_func = '>'   # GL_GREATER
        ctx.depth_func = '=='  # GL_EQUAL
        ctx.depth_func = '!='  # GL_NOTEQUAL
        ctx.depth_func = '0'   # GL_NEVER
        ctx.depth_func = '1'   # GL_ALWAYS
    """

    depth_clamp_range: Union[Tuple[float, float], None]
    """
    Setting up depth clamp range (write only, by default ``None``).

    ``ctx.depth_clamp_range`` offers uniform use of GL_DEPTH_CLAMP and glDepthRange.

    ``GL_DEPTH_CLAMP`` is needed to disable clipping of fragments outside
    near limit of projection matrix.
    For example, this will allow you to draw between 0 and 1 in the Z (depth) coordinate,
    even if ``near`` is set to 0.5 in the projection matrix.

    .. note::

        All fragments outside the ``near`` of the projection matrix will have a depth of ``near``.

    See https://www.khronos.org/opengl/wiki/Vertex_Post-Processing#Depth_clamping for more info.

    ``glDepthRange(nearVal, farVal)`` is needed to specify mapping of depth values from normalized device coordinates to window coordinates.
    See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDepthRange.xhtml for more info.

    Example::
        
        # For glDisable(GL_DEPTH_CLAMP) and glDepthRange(0, 1)
        ctx.depth_clamp_range = None

        # For glEnable(GL_DEPTH_CLAMP) and glDepthRange(near, far)
        ctx.depth_clamp_range = (near, far)
    """

    blend_func: Tuple[int, int]
    """
    tuple: Set the blend func (write only).

    Blend func can be set for rgb and alpha separately if needed.

    Supported blend functions are::

        moderngl.ZERO
        moderngl.ONE
        moderngl.SRC_COLOR
        moderngl.ONE_MINUS_SRC_COLOR
        moderngl.DST_COLOR
        moderngl.ONE_MINUS_DST_COLOR
        moderngl.SRC_ALPHA
        moderngl.ONE_MINUS_SRC_ALPHA
        moderngl.DST_ALPHA
        moderngl.ONE_MINUS_DST_ALPHA

        # Shortcuts
        moderngl.DEFAULT_BLENDING     # (SRC_ALPHA, ONE_MINUS_SRC_ALPHA)
        moderngl.ADDITIVE_BLENDING    # (ONE, ONE)
        moderngl.PREMULTIPLIED_ALPHA  # (SRC_ALPHA, ONE)

    Example::

        # For both rgb and alpha
        ctx.blend_func = moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA

        # Separate for rgb and alpha
        ctx.blend_func = (
            moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA,
            moderngl.ONE, moderngl.ONE
        )
    """

    blend_equation: Tuple[int, int]
    """
    tuple: Set the blend equation (write only).

    Blend equations specify how source and destination colors are combined
    in blending operations. By default ``FUNC_ADD`` is used.

    Blend equation can be set for rgb and alpha separately if needed.

    Supported functions are::

        moderngl.FUNC_ADD               # source + destination
        moderngl.FUNC_SUBTRACT          # source - destination
        moderngl.FUNC_REVERSE_SUBTRACT  # destination - source
        moderngl.MIN                    # Minimum of source and destination
        moderngl.MAX                    # Maximum of source and destination

    Example::

        # For both rgb and alpha channel
        ctx.blend_equation = moderngl.FUNC_ADD

        # Separate for rgb and alpha channel
        ctx.blend_equation = moderngl.FUNC_ADD, moderngl.MAX
    """

    multisample: bool
    """
    bool: Enable/disable multisample mode (``GL_MULTISAMPLE``).

    This property is write only.

    Example::

        # Enable
        ctx.multisample = True
        # Disable
        ctx.multisample = False
    """

    provoking_vertex: int
    """
    Specifies the vertex to be used as the source of data for flat shaded varyings.

    Flatshading a vertex shader varying output (ie. ``flat out vec3 pos``) means to assign
    all vetices of the primitive the same value for that output. The vertex from which
    these values is derived is known as the provoking vertex.

    It can be configured to be the first or the last vertex.

    This property is write only.

    Example::

        # Use first vertex
        ctx.provoking_vertex = moderngl.FIRST_VERTEX_CONVENTION

        # Use last vertex
        ctx.provoking_vertex = moderngl.LAST_VERTEX_CONVENTION
    """

    polygon_offset: Tuple[float, float]
    """
    tuple: Get or set the current polygon offset.

    The tuple values represents two float values: ``unit`` and a ``factor``::

        ctx.polygon_offset = unit, factor

    When drawing polygons, lines or points directly on top of
    exiting geometry the result is often not visually pleasant.
    We can experience z-fighting or partially fading fragments
    due to different primitives not being rasterized in the exact
    same way or simply depth buffer precision issues.

    For example when visualizing polygons drawing a wireframe
    version on top of the original mesh, these issues are
    immediately apparent. Applying decals to surfaces is
    another common example.

    The official documentation states the following::

        When enabled, the depth value of each fragment is added
        to a calculated offset value. The offset is added before
        the depth test is performed and before the depth value
        is written into the depth buffer. The offset value o is calculated by:
        o = m * factor + r * units
        where m is the maximum depth slope of the polygon and r is the smallest
        value guaranteed to produce a resolvable difference in window coordinate
        depth values. The value r is an implementation-specific int.

    In simpler terms: We use polygon offset to either add a positive offset to
    the geometry (push it away from you) or a negative offset to geometry
    (pull it towards you).

    * ``units`` is a int offset to depth and will do the job alone
        if we are working with geometry parallel to the near/far plane.
    * The ``factor`` helps you handle sloped geometry (not parallel to near/far plane).

    In most cases you can get away with ``[-1.0, 1.0]`` for both factor and units,
    but definitely play around with the values. When both values are set to ``0``
    polygon offset is disabled internally.

    To just get started with something you can try::

        # Either push the geomtry away or pull it towards you
        # with support for handling small to medium sloped geometry
        ctx.polygon_offset = 1.0, 1.0
        ctx.polygon_offset = -1.0, -1.0

        # Disable polygon offset
        ctx.polygon_offset = 0, 0
    """

    viewport: Tuple[int, int, int, int]
    """
    tuple: Get or set the viewport of the active framebuffer.

    Example::

        >>> ctx.viewport
        (0, 0, 1280, 720)
        >>> ctx.viewport = (0, 0, 640, 360)
        >>> ctx.viewport
        (0, 0, 640, 360)

    If no framebuffer is bound ``(0, 0, 0, 0)`` will be returned.
    """

    scissor: Optional[Tuple[int, int, int, int]]
    """
    tuple: Get or set the scissor box for the active framebuffer.

    When scissor testing is enabled fragments outside
    the defined scissor box will be discarded. This
    applies to rendered geometry or :py:meth:`Context.clear`.

    Setting is value enables scissor testing in the framebuffer.
    Setting the scissor to ``None`` disables scissor testing
    and reverts the scissor box to match the framebuffer size.

    Example::

        # Enable scissor testing
        >>> ctx.scissor = 100, 100, 200, 100
        # Disable scissor testing
        >>> ctx.scissor = None

    If no framebuffer is bound ``(0, 0, 0, 0)`` will be returned.
    """

    max_samples: int
    """The maximum supported number of samples for multisampling."""

    max_integer_samples: int
    """The max integer samples."""

    max_texture_units: int
    """The max texture units."""

    max_anisotropy: float
    """The maximum value supported for anisotropic filtering."""

    default_texture_unit: int
    """The default texture unit."""

    screen: Framebuffer
    """
    Framebuffer: A Framebuffer instance representing the screen.

    Normally set when creating a context with ``create_context()`` attaching to
    an existing context. This is the special system framebuffer
    represented by framebuffer ``id=0``.

    When creating a standalone context this property is not set since
    there are no default framebuffer.
    """

    wireframe: bool
    """Wireframe settings for debugging."""

    front_face: str
    """
    The front_face. Acceptable values are ``'ccw'`` (default) or ``'cw'``.

    Face culling must be enabled for this to have any effect:
    ``ctx.enable(moderngl.CULL_FACE)``.

    Example::

        # Triangles winded counter-clockwise considered front facing
        ctx.front_face = 'ccw'
        # Triangles winded clockwise considered front facing
        ctx.front_face = 'cw'
    """

    cull_face: str
    """
    The face side to cull. Acceptable values are ``'back'`` (default) ``'front'`` or ``'front_and_back'``.

    This is similar to :py:meth:`Context.front_face`

    Face culling must be enabled for this to have any effect:
    ``ctx.enable(moderngl.CULL_FACE)``.

    Example::

        ctx.cull_face = 'front'
        ctx.cull_face = 'back'
        ctx.cull_face = 'front_and_back'
    """

    patch_vertices: int
    """The number of vertices that will be used to make up a single patch primitive."""

    error: str
    """
    The result of ``glGetError()`` but human readable.

    This values is provided for debug purposes only and is likely to
    reduce performace when used in a draw loop.
    """

    extensions: Set[str]
    """
    Set[str]: The extensions supported by the context.

    All extensions names have a ``GL_`` prefix, so if the spec refers to ``ARB_compute_shader``
    we need to look for ``GL_ARB_compute_shader``::

        # If compute shaders are supported ...
        >> 'GL_ARB_compute_shader' in ctx.extensions
        True

    Example data::

        {
            'GL_ARB_multi_bind',
            'GL_ARB_shader_objects',
            'GL_ARB_half_float_vertex',
            'GL_ARB_map_buffer_alignment',
            'GL_ARB_arrays_of_arrays',
            'GL_ARB_pipeline_statistics_query',
            'GL_ARB_provoking_vertex',
            'GL_ARB_gpu_shader5',
            'GL_ARB_uniform_buffer_object',
            'GL_EXT_blend_equation_separate',
            'GL_ARB_tessellation_shader',
            'GL_ARB_multi_draw_indirect',
            'GL_ARB_multisample',
            .. etc ..
        }
    """

    info: Dict[str, Any]
    """
    dict: OpenGL Limits and information about the context.

    Example::

        # The maximum width and height of a texture
        >> ctx.info['GL_MAX_TEXTURE_SIZE']
        16384

        # Vendor and renderer
        >> ctx.info['GL_VENDOR']
        NVIDIA Corporation
        >> ctx.info['GL_RENDERER']
        NVIDIA GeForce GT 650M OpenGL Engine

    Example data::

        {
            'GL_VENDOR': 'NVIDIA Corporation',
            'GL_RENDERER': 'NVIDIA GeForce GT 650M OpenGL Engine',
            'GL_VERSION': '4.1 NVIDIA-10.32.0 355.11.10.10.40.102',
            'GL_POINT_SIZE_RANGE': (1.0, 2047.0),
            'GL_SMOOTH_LINE_WIDTH_RANGE': (0.5, 1.0),
            'GL_ALIASED_LINE_WIDTH_RANGE': (1.0, 1.0),
            'GL_POINT_FADE_THRESHOLD_SIZE': 1.0,
            'GL_POINT_SIZE_GRANULARITY': 0.125,
            'GL_SMOOTH_LINE_WIDTH_GRANULARITY': 0.125,
            'GL_MIN_PROGRAM_TEXEL_OFFSET': -8.0,
            'GL_MAX_PROGRAM_TEXEL_OFFSET': 7.0,
            'GL_MINOR_VERSION': 1,
            'GL_MAJOR_VERSION': 4,
            'GL_SAMPLE_BUFFERS': 0,
            'GL_SUBPIXEL_BITS': 8,
            'GL_CONTEXT_PROFILE_MASK': 1,
            'GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT': 256,
            'GL_DOUBLEBUFFER': False,
            'GL_STEREO': False,
            'GL_MAX_VIEWPORT_DIMS': (16384, 16384),
            'GL_MAX_3D_TEXTURE_SIZE': 2048,
            'GL_MAX_ARRAY_TEXTURE_LAYERS': 2048,
            'GL_MAX_CLIP_DISTANCES': 8,
            'GL_MAX_COLOR_ATTACHMENTS': 8,
            'GL_MAX_COLOR_TEXTURE_SAMPLES': 8,
            'GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS': 233472,
            'GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS': 231424,
            'GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS': 80,
            'GL_MAX_COMBINED_UNIFORM_BLOCKS': 70,
            'GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS': 233472,
            'GL_MAX_CUBE_MAP_TEXTURE_SIZE': 16384,
            'GL_MAX_DEPTH_TEXTURE_SAMPLES': 8,
            'GL_MAX_DRAW_BUFFERS': 8,
            'GL_MAX_DUAL_SOURCE_DRAW_BUFFERS': 1,
            'GL_MAX_ELEMENTS_INDICES': 150000,
            'GL_MAX_ELEMENTS_VERTICES': 1048575,
            'GL_MAX_FRAGMENT_INPUT_COMPONENTS': 128,
            'GL_MAX_FRAGMENT_UNIFORM_COMPONENTS': 4096,
            'GL_MAX_FRAGMENT_UNIFORM_VECTORS': 1024,
            'GL_MAX_FRAGMENT_UNIFORM_BLOCKS': 14,
            'GL_MAX_GEOMETRY_INPUT_COMPONENTS': 128,
            'GL_MAX_GEOMETRY_OUTPUT_COMPONENTS': 128,
            'GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS': 16,
            'GL_MAX_GEOMETRY_UNIFORM_BLOCKS': 14,
            'GL_MAX_GEOMETRY_UNIFORM_COMPONENTS': 2048,
            'GL_MAX_INTEGER_SAMPLES': 1,
            'GL_MAX_SAMPLES': 8,
            'GL_MAX_RECTANGLE_TEXTURE_SIZE': 16384,
            'GL_MAX_RENDERBUFFER_SIZE': 16384,
            'GL_MAX_SAMPLE_MASK_WORDS': 1,
            'GL_MAX_SERVER_WAIT_TIMEOUT': -1,
            'GL_MAX_TEXTURE_BUFFER_SIZE': 134217728,
            'GL_MAX_TEXTURE_IMAGE_UNITS': 16,
            'GL_MAX_TEXTURE_LOD_BIAS': 15,
            'GL_MAX_TEXTURE_SIZE': 16384,
            'GL_MAX_UNIFORM_BUFFER_BINDINGS': 70,
            'GL_MAX_UNIFORM_BLOCK_SIZE': 65536,
            'GL_MAX_VARYING_COMPONENTS': 0,
            'GL_MAX_VARYING_VECTORS': 31,
            'GL_MAX_VARYING_FLOATS': 0,
            'GL_MAX_VERTEX_ATTRIBS': 16,
            'GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS': 16,
            'GL_MAX_VERTEX_UNIFORM_COMPONENTS': 4096,
            'GL_MAX_VERTEX_UNIFORM_VECTORS': 1024,
            'GL_MAX_VERTEX_OUTPUT_COMPONENTS': 128,
            'GL_MAX_VERTEX_UNIFORM_BLOCKS': 14,
            'GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET': 0,
            'GL_MAX_VERTEX_ATTRIB_BINDINGS': 0,
            'GL_VIEWPORT_BOUNDS_RANGE': (-32768, 32768),
            'GL_VIEWPORT_SUBPIXEL_BITS': 0,
            'GL_MAX_VIEWPORTS': 16
        }
    """

    includes: Dict[str, str]
    """Mapping used for include statements."""

    mglo: Any
    """Internal representation for debug purposes only."""

    extra: Any
    """Attribute for storing user defined objects"""

    fbo: Framebuffer
    """The active framebuffer. Set every time :py:meth:`Framebuffer.use()` is called."""

    def clear(
        self,
        red: float = 0.0,
        green: float = 0.0,
        blue: float = 0.0,
        alpha: float = 0.0,
        depth: float = 1.0,
        viewport: Optional[Union[Tuple[int, int], Tuple[int, int, int, int]]] = None,
        color: Optional[Tuple[float, float, float, float]] = None,
    ) -> None:
        """
        Clear the bound framebuffer.

        If a `viewport` passed in, a scissor test will be used to clear the given viewport.
        This viewport take prescense over the framebuffers :py:attr:`~moderngl.Framebuffer.scissor`.
        Clearing can still be done with scissor if no viewport is passed in.

        This method also respects the
        :py:attr:`~moderngl.Framebuffer.color_mask` and
        :py:attr:`~moderngl.Framebuffer.depth_mask`. It can for example be used to only clear
        the depth or color buffer or specific components in the color buffer.

        If the `viewport` is a 2-tuple it will clear the
        ``(0, 0, width, height)`` where ``(width, height)`` is the 2-tuple.

        If the `viewport` is a 4-tuple it will clear the given viewport.

        Args:
            red (float): color component.
            green (float): color component.
            blue (float): color component.
            alpha (float): alpha component.
            depth (float): depth value.

        Keyword Args:
            viewport (tuple): The viewport.
            color (tuple): Optional rgba color tuple
        """
    def enable_only(self, flags: int) -> None:
        """
        Clears all existing flags applying new ones.

        Note that the enum values defined in moderngl
        are not the same as the ones in opengl.
        These are defined as bit flags so we can logical
        `or` them together.

        Available flags:

        - :py:data:`moderngl.NOTHING`
        - :py:data:`moderngl.BLEND`
        - :py:data:`moderngl.DEPTH_TEST`
        - :py:data:`moderngl.CULL_FACE`
        - :py:data:`moderngl.RASTERIZER_DISCARD`
        - :py:data:`moderngl.PROGRAM_POINT_SIZE`

        Examples::

            # Disable all flags
            ctx.enable_only(moderngl.NOTHING)

            # Ensure only depth testing and face culling is enabled
            ctx.enable_only(moderngl.DEPTH_TEST | moderngl.CULL_FACE)

        Args:
            flags (EnableFlag): The flags to enable
        """
    def enable(self, flags: int) -> None:
        """
        Enable flags.

        Note that the enum values defined in moderngl
        are not the same as the ones in opengl.
        These are defined as bit flags so we can logical
        `or` them together.

        For valid flags, please see :py:meth:`enable_only`.

        Examples::

            # Enable a single flag
            ctx.enable(moderngl.DEPTH_TEST)

            # Enable multiple flags
            ctx.enable(moderngl.DEPTH_TEST | moderngl.CULL_FACE | moderngl.BLEND)

        Args:
            flag (int): The flags to enable.
        """
    def disable(self, flags: int) -> None:
        """
        Disable flags.

        For valid flags, please see :py:meth:`enable_only`.

        Examples::

            # Only disable depth testing
            ctx.disable(moderngl.DEPTH_TEST)

            # Disable depth testing and face culling
            ctx.disable(moderngl.DEPTH_TEST | moderngl.CULL_FACE)

        Args:
            flag (int): The flags to disable.
        """
    def enable_direct(self, enum: int) -> None:
        """
        Gives direct access to ``glEnable`` so unsupported capabilities in ModernGL can be enabled.

        Do not use this to set already supported context flags.

        Example::

            # Enum value from the opengl registry
            GL_CONSERVATIVE_RASTERIZATION_NV = 0x9346
            ctx.enable_direct(GL_CONSERVATIVE_RASTERIZATION_NV)
        """
    def disable_direct(self, enum: int) -> None:
        """
        Gives direct access to ``glDisable`` so unsupported capabilities in ModernGL can be disabled.

        Do not use this to set already supported context flags.

        Example::

            # Enum value from the opengl registry
            GL_CONSERVATIVE_RASTERIZATION_NV = 0x9346
            ctx.disable_direct(GL_CONSERVATIVE_RASTERIZATION_NV)
        """
    def finish(self) -> None:
        """Wait for all drawing commands to finish."""
    def copy_buffer(
        self,
        dst: Buffer,
        src: Buffer,
        size: int = -1,
        read_offset: int = 0,
        write_offset: int = 0,
    ) -> None:
        """
        Copy buffer content.

        Args:
            dst (Buffer): The destination buffer.
            src (Buffer): The source buffer.
            size (int): The number of bytes to copy.

        Keyword Args:
            read_offset (int): The read offset.
            write_offset (int): The write offset.
        """
    def copy_framebuffer(self, dst: Union[Framebuffer, Texture], src: Framebuffer) -> None:
        """
        Copy framebuffer content.

        Use this method to:

            - blit framebuffers.
            - copy framebuffer content into a texture.
            - downsample framebuffers. (it will allow to read the framebuffer's content)
            - downsample a framebuffer directly to a texture.

        Args:
            dst (Framebuffer or Texture): Destination framebuffer or texture.
            src (Framebuffer): Source framebuffer.
        """
    def detect_framebuffer(self, glo: Optional[int] = None) -> Framebuffer:
        """
        Detect a framebuffer.

        This is already done when creating a context,
        but if the underlying window library for some changes the default framebuffer
        during the lifetime of the application this might be necessary.

        Args:
            glo (int): Framebuffer object.

        Returns:
            :py:class:`Framebuffer` object
        """
    def memory_barrier(self, barriers: Optional[int] = None, by_region: Optional[bool] = False) -> None:
        """
        Applying a memory barrier.

        The memory barrier is needed in particular to correctly change buffers or textures
        between each shader. If the same buffer is changed in two shaders,
        it can cause an effect like 'depth fighting' on a buffer or texture.

        The method should be used between :py:class:`Program` -s, between :py:class:`ComputeShader` -s,
        and between :py:class:`Program` -s and :py:class:`ComputeShader` -s.

        Keyword Args:
            barriers (int): Affected barriers, default moderngl.ALL_BARRIER_BITS.
            by_region (bool): Memory barrier mode by region. More read on https://registry.khronos.org/OpenGL-Refpages/gl4/html/glMemoryBarrier.xhtml
        """
    def buffer(self, data: Any = None, reserve: int = 0, dynamic: bool = False) -> Buffer:
        """
        Create a :py:class:`Buffer` object.

        Args:
            data (bytes): Content of the new buffer.

        Keyword Args:
            reserve (int): The number of bytes to reserve.
            dynamic (bool): Treat buffer as dynamic.

        Returns:
            :py:class:`Buffer` object
        """
    def external_texture(
        self,
        glo: int,
        size: Tuple[int, int],
        components: int,
        samples: int,
        dtype: str,
    ) -> Texture:
        """
        Create a :py:class:`Texture` object from an existing OpenGL texture object.

        Args:
            glo (int): External OpenGL texture object.
            size (tuple): The width and height of the texture.
            components (int): The number of components 1, 2, 3 or 4.
            samples (int): The number of samples. Value 0 means no multisample format.
            dtype (str): Data type.
        """
    def texture(
        self,
        size: Tuple[int, int],
        components: int,
        data: Optional[Any] = None,
        samples: int = 0,
        alignment: int = 1,
        dtype: str = "f1",
        internal_format: Optional[int] = None,
    ) -> Texture:
        """
        Create a :py:class:`Texture` object.

        .. Warning:: Do not play with ``internal_format`` unless you know exactly
                        you are doing. This is an override to support sRGB and
                        compressed textures if needed.

        Args:
            size (tuple): The width and height of the texture.
            components (int): The number of components 1, 2, 3 or 4.
            data (bytes): Content of the texture.

        Keyword Args:
            samples (int): The number of samples. Value 0 means no multisample format.
            alignment (int): The byte alignment 1, 2, 4 or 8.
            dtype (str): Data type.
            internal_format (int): Override the internalformat of the texture (IF needed)

        Returns:
            :py:class:`Texture` object
        """
    def texture_array(
        self,
        size: Tuple[int, int, int],
        components: int,
        data: Optional[Any] = None,
        alignment: int = 1,
        dtype: str = "f1",
    ) -> TextureArray:
        """
        Create a :py:class:`TextureArray` object.

        Args:
            size (tuple): The ``(width, height, layers)`` of the texture.
            components (int): The number of components 1, 2, 3 or 4.
            data (bytes): Content of the texture. The size must be ``(width, height * layers)``
                            so each layer is stacked vertically.

        Keyword Args:
            alignment (int): The byte alignment 1, 2, 4 or 8.
            dtype (str): Data type.

        Returns:
            :py:class:`Texture3D` object
        """
    def texture3d(
        self,
        size: Tuple[int, int, int],
        components: int,
        data: Optional[Any] = None,
        alignment: int = 1,
        dtype: str = "f1",
    ) -> Texture3D:
        """
        Create a :py:class:`Texture3D` object.

        Args:
            size (tuple): The width, height and depth of the texture.
            components (int): The number of components 1, 2, 3 or 4.
            data (bytes): Content of the texture.

        Keyword Args:
            alignment (int): The byte alignment 1, 2, 4 or 8.
            dtype (str): Data type.

        Returns:
            :py:class:`Texture3D` object
        """
    def texture_cube(
        self,
        size: Tuple[int, int],
        components: int,
        data: Optional[Any] = None,
        alignment: int = 1,
        dtype: str = "f1",
        internal_format: Optional[int] = None,
    ) -> TextureCube:
        """
        Create a :py:class:`TextureCube` object.

        Note that the width and height of the cubemap must be the same
        unless you are using a non-standard extension.

        Args:
            size (tuple): The width, height of the texture. Each side of
                            the cube will have this size.
            components (int): The number of components 1, 2, 3 or 4.
            data (bytes): Content of the texture. The data should be have the following ordering:
                            positive_x, negative_x, positive_y, negative_y, positive_z, negative_z

        Keyword Args:
            alignment (int): The byte alignment 1, 2, 4 or 8.
            dtype (str): Data type.
            internal_format (int): Override the internalformat of the texture (IF needed)

        Returns:
            :py:class:`TextureCube` object
        """
    def depth_texture(
        self,
        size: Tuple[int, int],
        data: Optional[Any] = None,
        samples: int = 0,
        alignment: int = 4,
    ) -> Texture:
        """
        Create a :py:class:`Texture` object.

        Args:
            size (tuple): The width and height of the texture.
            data (bytes): Content of the texture.

        Keyword Args:
            samples (int): The number of samples. Value 0 means no multisample format.
            alignment (int): The byte alignment 1, 2, 4 or 8.

        Returns:
            :py:class:`Texture` object
        """
    def depth_texture_cube(
        self,
        size: Tuple[int, int],
        data: Optional[Any] = None,
        alignment: int = 4,
    ) -> TextureCube:
        """
        Create a :py:class:`TextureCube` object

        Args:
            size (tuple): The width and height of the texture.
            data (bytes): Content of the texture.

        Keyword Args:
            alignment (int): The byte alignment 1, 2, 4 or 8.

        Returns:
            :py:class:`TextureCube` object
        """
    def vertex_array(self, *args, **kwargs) -> "VertexArray":
        """
        Create a :py:class:`VertexArray` object.

        The vertex array describes how buffers are read by a shader program.
        We need to supply buffer formats and attributes names. The attribute names
        are defined by the user in the glsl code and can be anything.

        Examples::

            # Empty vertext array (no attribute input)
            vao = ctx.vertex_array(program)

            # Simple version with a single buffer
            vao = ctx.vertex_array(program, buffer, 'in_position', 'in_normal')
            vao = ctx.vertex_array(program, buffer, 'in_position', 'in_normal', index_buffer=ibo)

            # Multiple buffers
            vao = ctx.vertex_array(program, [
                (buffer1, '3f', 'in_position'),
                (buffer2, '3f', 'in_normal'),
            ])
            vao = ctx.vertex_array(program, [
                    (buffer1, '3f', 'in_position'),
                    (buffer2, '3f', 'in_normal'),
                ],
                index_buffer=ibo,
                index_element_size=2,  # 16 bit / 'u2' index buffer
            )

        This method also supports arguments for :py:meth:`Context.simple_vertex_array`.

        Args:
            program (Program): The program used when rendering
            content (list): A list of (buffer, format, attributes).
                            See :ref:`buffer-format-label`.

        Keyword Args:
            index_buffer (Buffer): An index buffer (optional)
            index_element_size (int): byte size of each index element, 1, 2 or 4.
            skip_errors (bool): Ignore errors during creation
            mode (int): The default draw mode (for example: ``TRIANGLES``)

        Returns:
            :py:class:`VertexArray` object
        """
    def _vertex_array(
        self,
        program: Program,
        content: Any,
        index_buffer: Optional[Buffer] = None,
        index_element_size: int = 4,
        skip_errors: bool = False,
        mode: Optional[int] = None,
    ) -> "VertexArray":
        """
        Create a :py:class:`VertexArray` object.

        Args:
            program (Program): The program used when rendering.
            content (list): A list of (buffer, format, attributes).
                            See :ref:`buffer-format-label`.
            index_buffer (Buffer): An index buffer.

        Keyword Args:
            index_element_size (int): byte size of each index element, 1, 2 or 4.
            skip_errors (bool): Ignore skip_errors varyings.
            mode (int): The default draw mode (for example: ``TRIANGLES``)

        Returns:
            :py:class:`VertexArray` object
        """
    def simple_vertex_array(
        self,
        program: Program,
        buffer: Buffer,
        *attributes: str,
        index_buffer: Optional[Buffer] = None,
        index_element_size: int = 4,
        mode: Optional[int] = None,
    ) -> "VertexArray":
        """
        Create a :py:class:`VertexArray` object.

        .. Warning:: This method is deprecated and may be removed in the future.
                        Use :py:meth:`Context.vertex_array` instead. It also supports
                        the argument format this method describes.

        Args:
            program (Program): The program used when rendering.
            buffer (Buffer): The buffer.
            attributes (list): A list of attribute names.

        Keyword Args:
            index_element_size (int): byte size of each index element, 1, 2 or 4.
            index_buffer (Buffer): An index buffer.
            mode (int): The default draw mode (for example: ``TRIANGLES``)

        Returns:
            :py:class:`VertexArray` object
        """
    def program(
        self,
        vertex_shader: str | bytes | ConvertibleToShaderSource,
        fragment_shader: str | bytes | ConvertibleToShaderSource | None = None,
        geometry_shader: str | bytes | ConvertibleToShaderSource | None = None,
        tess_control_shader: str | bytes | ConvertibleToShaderSource | None = None,
        tess_evaluation_shader: str | bytes | ConvertibleToShaderSource | None = None,
        varyings: Tuple[str, ...] = (),
        fragment_outputs: Optional[Dict[str, int]] = None,
        varyings_capture_mode: str = "interleaved",
    ) -> Program:
        """
        Create a :py:class:`Program` object.

        The ``varyings`` are only used when a transform program is created
        to specify the names of the output varyings to capture in the output buffer.

        ``fragment_outputs`` can be used to programmatically map named fragment
        shader outputs to a framebuffer attachment numbers. This can also be done
        by using ``layout(location=N)`` in the fragment shader.

        Args:
            vertex_shader (str): The vertex shader source.
            fragment_shader (str): The fragment shader source.
            geometry_shader (str): The geometry shader source.
            tess_control_shader (str): The tessellation control shader source.
            tess_evaluation_shader (str): The tessellation evaluation shader source.
            varyings (list): A list of varyings.
            fragment_outputs (dict): A dictionary of fragment outputs.
        Returns:
            :py:class:`Program` object
        """
    def query(
        self,
        samples: bool = False,
        any_samples: bool = False,
        time: bool = False,
        primitives: bool = False,
    ) -> "Query":
        """
        Create a :py:class:`Query` object.

        Keyword Args:
            samples (bool): Query ``GL_SAMPLES_PASSED`` or not.
            any_samples (bool): Query ``GL_ANY_SAMPLES_PASSED`` or not.
            time (bool): Query ``GL_TIME_ELAPSED`` or not.
            primitives (bool): Query ``GL_PRIMITIVES_GENERATED`` or not.
        """
    def scope(
        self,
        framebuffer: Optional[Framebuffer] = None,
        enable_only: Optional[int] = None,
        textures: Tuple[Tuple[Texture, int], ...] = (),
        uniform_buffers: Tuple[Tuple[Buffer, int], ...] = (),
        storage_buffers: Tuple[Tuple[Buffer, int], ...] = (),
        samplers: Tuple[Tuple["Sampler", int], ...] = (),
        enable: Optional[int] = None,
    ) -> "Scope":
        """
        Create a :py:class:`Scope` object.

        Args:
            framebuffer (Framebuffer): The framebuffer to use when entering.
            enable_only (int): The enable_only flags to set when entering.

        Keyword Args:
            textures (tuple): List of (texture, binding) tuples.
            uniform_buffers (tuple): Tuple of (buffer, binding) tuples.
            storage_buffers (tuple): Tuple of (buffer, binding) tuples.
            samplers (tuple): Tuple of sampler bindings
            enable (int): Flags to enable for this vao such as depth testing and blending
        """
    def simple_framebuffer(
        self,
        size: Tuple[int, int],
        components: int = 4,
        samples: int = 0,
        dtype: str = "f1",
    ) -> Framebuffer:
        """
        Creates a :py:class:`Framebuffer` with a single color attachment \
        and depth buffer using :py:class:`moderngl.Renderbuffer` attachments.

        Args:
            size (tuple): The width and height of the renderbuffer.
            components (int): The number of components 1, 2, 3 or 4.

        Keyword Args:
            samples (int): The number of samples. Value 0 means no multisample format.
            dtype (str): Data type.

        Returns:
            :py:class:`Framebuffer` object
        """
    def framebuffer(
        self,
        color_attachments: Any = (),
        depth_attachment: Optional[Union[Texture, "Renderbuffer"]] = None,
    ) -> Framebuffer:
        """
        A :py:class:`Framebuffer` is a collection of buffers that can be \
        used as the destination for rendering. The buffers for Framebuffer \
        objects reference images from either Textures or Renderbuffers.

        Args:
            color_attachments (list): A list of :py:class:`Texture` or
                                        :py:class:`Renderbuffer` objects.
            depth_attachment (Renderbuffer or Texture): The depth attachment.

        Returns:
            :py:class:`Framebuffer` object
        """
    def empty_framebuffer(
        self,
        size: Tuple[int, int],
        layers: Optional[int] = 0,
        samples: Optional[int] = 0,
    ) -> Framebuffer:
        """
        Empty Framebuffer
        """
    def renderbuffer(
        self,
        size: Tuple[int, int],
        components: int = 4,
        samples: int = 0,
        dtype: str = "f1",
    ) -> "Renderbuffer":
        """
        :py:class:`Renderbuffer` objects are OpenGL objects that contain images. \
        They are created and used specifically with :py:class:`Framebuffer` objects.

        Args:
            size (tuple): The width and height of the renderbuffer.
            components (int): The number of components 1, 2, 3 or 4.

        Keyword Args:
            samples (int): The number of samples. Value 0 means no multisample format.
            dtype (str): Data type.

        Returns:
            :py:class:`Renderbuffer` object
        """
    def depth_renderbuffer(self, size: Tuple[int, int], samples: int = 0) -> "Renderbuffer":
        """
        :py:class:`Renderbuffer` objects are OpenGL objects that contain images. \
        They are created and used specifically with :py:class:`Framebuffer` objects.

        Args:
            size (tuple): The width and height of the renderbuffer.

        Keyword Args:
            samples (int): The number of samples. Value 0 means no multisample format.

        Returns:
            :py:class:`Renderbuffer` object
        """
    def compute_shader(self, source: str | bytes | ConvertibleToShaderSource) -> "ComputeShader":
        """
        A :py:class:`ComputeShader` is a Shader Stage that is used entirely \
        for computing arbitrary information. While it can do rendering, it \
        is generally used for tasks not directly related to drawing.

        Args:
            source (str): The source of the compute shader.

        Returns:
            :py:class:`ComputeShader` object
        """
    def sampler(
        self,
        repeat_x: bool = True,
        repeat_y: bool = True,
        repeat_z: bool = True,
        filter: Optional[Tuple[int, int]] = None,
        anisotropy: float = 1.0,
        compare_func: str = "?",
        border_color: Optional[Tuple[float, float, float, float]] = None,
        min_lod: float = -1000.0,
        max_lod: float = 1000.0,
        texture: Optional[Texture] = None,
    ) -> "Sampler":
        """
        Create a :py:class:`Sampler` object.

        Keyword Arguments:
            repeat_x (bool): Repeat texture on x
            repeat_y (bool): Repeat texture on y
            repeat_z (bool): Repeat texture on z
            filter (tuple): The min and max filter
            anisotropy (float): Number of samples for anisotropic filtering.
                                Any value greater than 1.0 counts as a use of
                                anisotropic filtering
            compare_func: Compare function for depth textures
            border_color (tuple): The (r, g, b, a) color for the texture border.
                                    When this value is set the ``repeat_`` values are
                                    overridden setting the texture wrap to return the
                                    border color when outside ``[0, 1]`` range.
            min_lod (float): Minimum level-of-detail parameter (Default ``-1000.0``).
                                This floating-point value limits the selection of highest
                                resolution mipmap (lowest mipmap level)
            max_lod (float): Minimum level-of-detail parameter (Default ``1000.0``).
                                This floating-point value limits the selection of the
                                lowest resolution mipmap (highest mipmap level)
            texture (Texture): The texture for this sampler
        """
    def clear_samplers(self, start: int = 0, end: int = -1) -> None:
        """
        Unbinds samplers from texture units.

        Sampler bindings do clear automatically between every frame,
        but lingering samplers can still be a source of weird bugs during
        the frame rendering. This methods provides a fairly brute force
        and efficient way to ensure texture units are clear.

        Keyword Arguments:

            start (int): The texture unit index to start the clearing samplers
            stop (int): The texture unit index to stop clearing samplers

        Example::

            # Clear texture unit 0, 1, 2, 3, 4
            ctx.clear_samplers(start=0, end=5)

            # Clear texture unit 4, 5, 6, 7
            ctx.clear_samplers(start=4, end=8)
        """
    def core_profile_check(self) -> None:
        """
        Core profile check.

        FOR DEBUG PURPOSES ONLY
        """
    def __enter__(self):
        """
        Enters the context.

        This should ideally be used with the ``with`` statement::

            with other_context as ctx:
                # Do something in this context

        When exiting the context the previously bound context is activated again.

        .. Warning:: Context switching can be risky unless you know what you are doing.
                     Use with care.
        """
    def __exit__(self, exc_type, exc_val, exc_tb):
        """
        Exit the context.

        See :py:meth:`Context.__enter__`
        """
    def release(self) -> None:
        """
        Release the ModernGL context.

        If the context is not standalone the standard
        backends in ``glcontext`` will not do anything
        because the context was not created by moderngl.

        Standalone contexts can normally be released.
        """

def create_context(
    require: Optional[int] = None,
    standalone: bool = False,
    share: bool = False,
    **settings: Dict[str, Any],
) -> Context:
    """
    Create a ModernGL context by loading OpenGL functions from an existing OpenGL context. \
    An OpenGL context must exists.

    Example::

        # Accept the current context version
        ctx = moderngl.create_context()

        # Require at least OpenGL 4.3
        ctx = moderngl.create_context(require=430)

        # Create a headless context requiring OpenGL 4.3
        ctx = moderngl.create_context(require=430, standalone=True)

    Keyword Arguments:
        require (int): OpenGL version code (default: 330)
        standalone (bool): Headless flag
        share (bool): Attempt to create a shared context
        **settings: Other backend specific settings

    Returns:
        :py:class:`Context` object
    """

def create_standalone_context(
    require: Optional[int] = None,
    share: bool = False,
    **settings: Dict[str, Any],
) -> "Context":
    """
    Create a standalone/headless ModernGL context.

    The preferred way of making a context is through :py:func:`moderngl.create_context`.

    Example::

        # Create a context with highest possible supported version
        ctx = moderngl.create_context()

        # Require at least OpenGL 4.3
        ctx = moderngl.create_context(require=430)

    Keyword Arguments:
        require (int): OpenGL version code.
        share (bool): Attempt to create a shared context
        settings: keyword config values for the context backend

    Returns:
        :py:class:`Context` object
    """

def init_context(loader=None) -> None:
    """
        Initialize the default moderngl context
    """

def get_context() -> Context:
    """
        Return the previously created context
    """

class Framebuffer:
    """
    A :py:class:`Framebuffer` is a collection of buffers that can be used as the destination for rendering.

    The buffers for Framebuffer objects reference images from either Textures or Renderbuffers.
    Create a :py:class:`Framebuffer` using :py:meth:`Context.framebuffer`.
    """

    viewport: Tuple[int, int, int, int]
    """Get or set the viewport of the framebuffer."""

    scissor: Tuple[int, int, int, int]
    """
    Get or set the scissor box of the framebuffer.

    When scissor testing is enabled fragments outside
    the defined scissor box will be discarded. This
    applies to rendered geometry or :py:meth:`Framebuffer.clear`.

    Setting is value enables scissor testing in the framebuffer.
    Setting the scissor to ``None`` disables scissor testing
    and reverts the scissor box to match the framebuffer size.

    Example::

        # Enable scissor testing
        >>> ctx.scissor = 100, 100, 200, 100
        # Disable scissor testing
        >>> ctx.scissor = None
    """

    color_mask: Tuple[bool, bool, bool, bool]
    """
    The color mask of the framebuffer.

    Color masking controls what components in color attachments will be
    affected by fragment write operations.
    This includes rendering geometry and clearing the framebuffer.

    Default value: ``(True, True, True, True)``.

    Examples::

        # Block writing to all color components (rgba) in color attachments
        fbo.color_mask = False, False, False, False

        # Re-enable writing to color attachments
        fbo.color_mask = True, True, True, True

        # Block fragment writes to alpha channel
        fbo.color_mask = True, True, True, False
    """

    depth_mask: bool
    """
    The depth mask of the framebuffer.

    Depth mask enables or disables write operations to the depth buffer.
    This also applies when clearing the framebuffer.
    If depth testing is enabled fragments will still be culled, but
    the depth buffer will not be updated with new values. This is
    a very useful tool in many rendering techniques.

    Default value: ``True``
    """

    width: int
    """
    The width of the framebuffer.

    Framebuffers created by a window will only report its initial size.
    It's better get size information from the window itself.
    """

    height: int
    """
    The height of the framebuffer.

    Framebuffers created by a window will only report its initial size.
    It's better get size information from the window itself.
    """

    size: Tuple[int, int]
    """
    Tuple[int, int]: The size of the framebuffer.

    Framebuffers created by a window will only report its initial size.
    It's better get size information from the window itself.
    """

    samples: int
    """The samples of the framebuffer."""

    bits: Dict[str, str]
    """The bits of the framebuffer."""

    color_attachments: Tuple[Union[Texture, Renderbuffer], ...]
    """The color attachments of the framebuffer."""

    depth_attachment: Union[Texture, Renderbuffer]
    """The depth attachment of the framebuffer."""

    mglo: Any
    """Internal representation for debug purposes only."""

    ctx: "Context"
    """The context this object belongs to"""

    extra: Any
    """Attribute for storing user defined objects"""

    glo: int
    """
    The internal OpenGL object.

    This values is provided for debug purposes only.
    """

    def clear(
        self,
        red: float = 0.0,
        green: float = 0.0,
        blue: float = 0.0,
        alpha: float = 0.0,
        depth: float = 1.0,
        viewport: Optional[Union[Tuple[int, int], Tuple[int, int, int, int]]] = None,
        color: Optional[Tuple[float, float, float, float]] = None,
    ) -> None:
        """
        Clear the framebuffer.

        If a `viewport` passed in, a scissor test will be used to clear the given viewport.
        This viewport take presence over the framebuffers :py:attr:`~moderngl.Framebuffer.scissor`.
        Clearing can still be done with scissor if no viewport is passed in.

        This method also respects the
        :py:attr:`~moderngl.Framebuffer.color_mask` and
        :py:attr:`~moderngl.Framebuffer.depth_mask`. It can for example be used to only clear
        the depth or color buffer or specific components in the color buffer.

        If the `viewport` is a 2-tuple it will clear the
        ``(0, 0, width, height)`` where ``(width, height)`` is the 2-tuple.

        If the `viewport` is a 4-tuple it will clear the given viewport.

        Args:
            red (float): color component.
            green (float): color component.
            blue (float): color component.
            alpha (float): alpha component.
            depth (float): depth value.

        Keyword Args:
            viewport (tuple): The viewport.
            color (tuple): Optional tuple replacing the red, green, blue and alpha arguments
        """
    def use(self) -> None:
        """Bind the framebuffer. Sets the target for rendering commands."""
    def read(
        self,
        viewport: Optional[Union[Tuple[int, int], Tuple[int, int, int, int]]] = None,
        components: int = 3,
        attachment: int = 0,
        alignment: int = 1,
        dtype: str = "f1",
        clamp: bool = False,
    ) -> bytes:
        """
        Read the content of the framebuffer.

        .. code:: python

            # Read the first color attachment's RGBA data
            data = fbo.read(components=4)
            # Read the second color attachment's RGB data
            data = fbo.read(attachment=1)
            # Read the depth attachment
            data = fbo.read(attachment=-1)
            # Read the lower left 10 x 10 pixels from the first color attachment
            data = fbo.read(viewport=(0, 0, 10, 10))

        Args:
            viewport (tuple): The viewport.
            components (int): The number of components to read.

        Keyword Args:
            attachment (int): The color attachment number. -1 for the depth attachment
            alignment (int): The byte alignment of the pixels.
            dtype (str): Data type.
            clamp (bool): Clamps floating point values to ``[0.0, 1.0]``

        Returns:
            bytes
        """
    def read_into(
        self,
        buffer: Any,
        viewport: Optional[Union[Tuple[int, int], Tuple[int, int, int, int]]] = None,
        components: int = 3,
        attachment: int = 0,
        alignment: int = 1,
        dtype: str = "f1",
        write_offset: int = 0,
    ) -> None:
        """
        Read the content of the framebuffer into a buffer.

        Args:
            buffer (bytearray): The buffer that will receive the pixels.
            viewport (tuple): The viewport.
            components (int): The number of components to read.

        Keyword Args:
            attachment (int): The color attachment.
            alignment (int): The byte alignment of the pixels.
            dtype (str): Data type.
            write_offset (int): The write offset.
        """
    def release(self) -> None:
        """Release the ModernGL object."""

class Program:
    """
    A Program object represents fully processed executable code in the OpenGL Shading Language, \
    for one or more Shader stages.

    In ModernGL, a Program object can be assigned to :py:class:`VertexArray` objects.
    The VertexArray object  is capable of binding the Program object once the
    :py:meth:`VertexArray.render` or :py:meth:`VertexArray.transform` is called.

    Program objects has no method called ``use()``, VertexArrays encapsulate this mechanism.

    A Program object cannot be instantiated directly, it requires a context.
    Use :py:meth:`Context.program` to create one.

    Uniform buffers can be bound using :py:meth:`Buffer.bind_to_uniform_block`
    or can be set individually. For more complex binding yielding higher
    performance consider using :py:class:`moderngl.Scope`.
    """

    def __getitem__(self, key: str) -> Union[Uniform, UniformBlock, Attribute, Varying]:
        """
        Get a member such as uniforms, uniform blocks, attributes and varyings by name.

        .. code-block:: python

            # Get a uniform
            uniform = program['color']

            # Uniform values can be set on the returned object
            # or the `__setitem__` shortcut can be used.
            program['color'].value = 1.0, 1.0, 1.0, 1.0

            # Still when writing byte data we need to use the `write()` method
            program['color'].write(buffer)
        """
    def __setitem__(self, key: str, value: Any) -> None:
        """
        Set a value of uniform or uniform block.

        .. code-block:: python

            # Set a vec4 uniform
            uniform['color'] = 1.0, 1.0, 1.0, 1.0

            # Optionally we can store references to a member and set the value directly
            uniform = program['color']
            uniform.value = 1.0, 0.0, 0.0, 0.0

            uniform = program['cameraMatrix']
            uniform.write(camera_matrix)
        """
    def __iter__(self) -> Generator[str, None, None]:
        """
        Yields the internal members names as strings.

        This includes all members such as uniforms, attributes etc.

        Example::

            # Print member information
            for name in program:
                member = program[name]
                print(name, type(member), member)

        Output::

            vert <class 'moderngl.program_members.attribute.Attribute'> <Attribute: 0>
            vert_color <class 'moderngl.program_members.attribute.Attribute'> <Attribute: 1>
            gl_InstanceID <class 'moderngl.program_members.attribute.Attribute'> <Attribute: -1>
            rotation <class 'moderngl.program_members.uniform.Uniform'> <Uniform: 0>
            scale <class 'moderngl.program_members.uniform.Uniform'> <Uniform: 1>

        We can filter on member type if needed::

            for name in prog:
                member = prog[name]
                if isinstance(member, moderngl.Uniform):
                    print('Uniform', name, member)

        or a less verbose version using dict comprehensions::

            uniforms = {name: self.prog[name] for name in self.prog
                        if isinstance(self.prog[name], moderngl.Uniform)}
            print(uniforms)

        Output::

            {'rotation': <Uniform: 0>, 'scale': <Uniform: 1>}

        """
    is_transform: bool
    """If this is a tranform program (no fragment shader)."""

    geometry_input: int
    """
    The geometry input primitive.

    The GeometryShader's input primitive if the GeometryShader exists.
    The geometry input primitive will be used for validation.
    (from ``layout(input_primitive) in;``)

    This can only be ``POINTS``, ``LINES``, ``LINES_ADJACENCY``, ``TRIANGLES``, ``TRIANGLE_ADJACENCY``.
    """

    geometry_output: int
    """
    The geometry output primitive.

    The GeometryShader's output primitive if the GeometryShader exists.
    This can only be ``POINTS``, ``LINE_STRIP`` and ``TRIANGLE_STRIP``
    (from ``layout(output_primitive, max_vertices = vert_count) out;``)
    """

    geometry_vertices: int
    """
    The maximum number of vertices that.

    the geometry shader will output.
    (from ``layout(output_primitive, max_vertices = vert_count) out;``)
    """

    mglo: Any
    """Internal representation for debug purposes only."""

    ctx: "Context"
    """The context this object belongs to"""

    extra: Any
    """Attribute for storing user defined objects"""

    glo: int
    """
    The internal OpenGL object.

    This values is provided for debug purposes only.
    """

    def get(self, key: str, default: Any) -> Union[Uniform, UniformBlock, Attribute, Varying]:
        """
        Returns a Uniform, UniformBlock, Attribute or Varying.

        Args:
            default: This is the value to be returned in case key does not exist.

        Returns:
            :py:class:`Uniform`, :py:class:`UniformBlock`, :py:class:`Attribute` or :py:class:`Varying`
        """
    def release(self) -> None:
        """Release the ModernGL object."""

def detect_format(
    program: Program,
    attributes: Any,
    mode: str = "mgl",
) -> str:
    """
    Detect format for vertex attributes.

    The format returned does not contain padding.

    Args:
        program (Program): The program.
        attributes (list): A list of attribute names.

    Returns:
        str
    """

class Query:
    """This class represents a Query object."""

    crender: Optional["ConditionalRender"]
    """Can be used in a ``with`` statement."""

    samples: int
    """The number of samples passed."""

    primitives: int
    """The number of primitives generated."""

    elapsed: int
    """The time elapsed in nanoseconds."""

    mglo: Any
    """Internal representation for debug purposes only."""

    ctx: "Context"
    """The context this object belongs to"""

    extra: Any
    """Attribute for storing user defined objects"""

    def __enter__(self): ...
    def __exit__(self, *args: Tuple[Any]): ...

class Renderbuffer:
    """
    Renderbuffer objects are OpenGL objects that contain images.

    They are created and used specifically with :py:class:`Framebuffer` objects.
    They are optimized for use as render targets, while :py:class:`Texture` objects
    may not be, and are the logical choice when you do not need to sample
    from the produced image. If you need to resample, use Textures instead.
    Renderbuffer objects also natively accommodate multisampling.

    A Renderbuffer object cannot be instantiated directly, it requires a context.
    Use :py:meth:`Context.renderbuffer` or :py:meth:`Context.depth_renderbuffer`
    to create one.
    """

    width: int
    """The width of the renderbuffer."""

    height: int
    """The height of the renderbuffer."""

    size: tuple
    """The size of the renderbuffer."""

    samples: int
    """The samples of the renderbuffer."""

    components: int
    """The components of the renderbuffer."""

    depth: bool
    """Is the renderbuffer a depth renderbuffer?."""

    dtype: str
    """Data type."""

    mglo: Any
    """Internal representation for debug purposes only."""

    ctx: "Context"
    """The context this object belongs to"""

    extra: Any
    """Attribute for storing user defined objects"""

    glo: int
    """
    The internal OpenGL object.

    This values is provided for debug purposes only.
    """

    def release(self) -> None:
        """Release the ModernGL object."""

class Sampler:
    """
    A Sampler Object is an OpenGL Object that stores the sampling parameters for a Texture access inside of a shader.

    When a sampler object is bound to a texture image unit,
    the internal sampling parameters for a texture bound to the same image unit are all ignored.
    Instead, the sampling parameters are taken from this sampler object.

    Unlike textures, a samplers state can also be changed freely be at any time
    without the sampler object being bound/in use.

    Samplers are bound to a texture unit and not a texture itself. Be careful with leaving
    samplers bound to texture units as it can cause texture incompleteness issues
    (the texture bind is ignored).

    Sampler bindings do clear automatically between every frame so a texture unit
    need at least one bind/use per frame.
    """

    repeat_x: bool
    """
    bool: The x repeat flag for the sampler (Default ``True``).

    Example::

        # Enable texture repeat (GL_REPEAT)
        sampler.repeat_x = True

        # Disable texture repeat (GL_CLAMP_TO_EDGE)
        sampler.repeat_x = False
    """

    repeat_y: bool
    """
    bool: The y repeat flag for the sampler (Default ``True``).

    Example::

        # Enable texture repeat (GL_REPEAT)
        sampler.repeat_y = True

        # Disable texture repeat (GL_CLAMP_TO_EDGE)
        sampler.repeat_y = False
    """

    repeat_z: bool
    """
    bool: The z repeat flag for the sampler (Default ``True``).

    Example::

        # Enable texture repeat (GL_REPEAT)
        sampler.repeat_z = True

        # Disable texture repeat (GL_CLAMP_TO_EDGE)
        sampler.repeat_z = False
    """

    filter: Tuple[int, int]
    """
    tuple: The minification and magnification filter for the sampler.

    (Default ``(moderngl.LINEAR. moderngl.LINEAR)``)

    Example::

        sampler.filter == (moderngl.NEAREST, moderngl.NEAREST)
        sampler.filter == (moderngl.LINEAR_MIPMAP_LINEAR, moderngl.LINEAR)
        sampler.filter == (moderngl.NEAREST_MIPMAP_LINEAR, moderngl.NEAREST)
        sampler.filter == (moderngl.LINEAR_MIPMAP_NEAREST, moderngl.NEAREST)
    """

    compare_func: str
    """
    tuple: The compare function for a depth textures (Default ``'?'``).

    By default samplers don't have depth comparison mode enabled.
    This means that depth texture values can be read as a ``sampler2D``
    using ``texture()`` in a GLSL shader by default.

    When setting this property to a valid compare mode, ``GL_TEXTURE_COMPARE_MODE``
    is set to ``GL_COMPARE_REF_TO_TEXTURE`` so that texture lookup
    functions in GLSL will return a depth comparison result instead
    of the actual depth value.

    Accepted compare functions::

        .compare_func = ''    # Disale depth comparison completely
        sampler.compare_func = '<='  # GL_LEQUAL
        sampler.compare_func = '<'   # GL_LESS
        sampler.compare_func = '>='  # GL_GEQUAL
        sampler.compare_func = '>'   # GL_GREATER
        sampler.compare_func = '=='  # GL_EQUAL
        sampler.compare_func = '!='  # GL_NOTEQUAL
        sampler.compare_func = '0'   # GL_NEVER
        sampler.compare_func = '1'   # GL_ALWAYS
    """

    anisotropy: float
    """
    Number of samples for anisotropic filtering (Default ``1.0``).

    The value will be clamped in range ``1.0`` and ``ctx.max_anisotropy``.

    Any value greater than 1.0 counts as a use of anisotropic filtering::

        # Disable anisotropic filtering
        sampler.anisotropy = 1.0

        # Enable anisotropic filtering suggesting 16 samples as a maximum
        sampler.anisotropy = 16.0
    """

    border_color: Tuple[float, float, float, float]
    """
    tuple: The (r, g, b, a) color for the texture border (Default ``(0.0, 0.0, 0.0, 0.0)``).

    When setting this value the ``repeat_`` values are overridden setting the texture wrap to return
    the border color when outside [0, 1] range.

    Example::

        # Red border color
        sampler.border_color = (1.0, 0.0, 0.0, 0.0)
    """

    min_lod: float
    """
    Minimum level-of-detail parameter (Default ``-1000.0``).

    This floating-point value limits the selection of highest resolution mipmap (lowest mipmap level)
    """

    max_lod: float
    """
    Minimum level-of-detail parameter (Default ``1000.0``).

    This floating-point value limits the selection of the lowest resolution mipmap (highest mipmap level)
    """

    texture: Any
    """texture"""

    mglo: Any
    """Internal representation for debug purposes only."""

    ctx: "Context"
    """The context this object belongs to"""

    extra: Any
    """Attribute for storing user defined objects"""

    glo: int
    """
    The internal OpenGL object.

    This values is provided for debug purposes only.
    """

    def use(self, location: int = 0) -> None:
        """
        Bind the sampler to a texture unit.

        Args:
            location (int): The texture unit
        """
    def clear(self, location: int = 0) -> None:
        """
        Clear the sampler binding on a texture unit.

        Args:
            location (int): The texture unit
        """
    def release(self) -> None:
        """Release/destroy the ModernGL object."""
    def assign(self, index: int) -> Tuple["Sampler", int]:
        """
        Helper method for assigning samplers to scopes.

        Example::

            s1 = ctx.sampler(...)
            s2 = ctx.sampler(...)
            ctx.scope(samplers=(s1.assign(0), s1.assign(1)), ...)

        Returns:
            (self, index) tuple
        """

class Scope:
    """
    This class represents a Scope object.

    Responsibilities on enter:

    - Set the enable flags.
    - Bind the framebuffer.
    - Assigning textures to texture locations.
    - Assigning buffers to uniform buffers.
    - Assigning buffers to shader storage buffers.

    Responsibilities on exit:

    - Restore the enable flags.
    - Restore the framebuffer.
    """

    def __enter__(self): ...
    def __exit__(self, *args: Tuple[Any]): ...
    def release(self) -> None:
        """Destroy the Scope object."""
    mglo: Any
    """Internal representation for debug purposes only."""

    ctx: "Context"
    """The context this object belongs to"""

    extra: Any
    """Attribute for storing user defined objects"""

class Texture3D:
    """
    A Texture is an OpenGL object that contains one or more images that all have the same image format.

    A texture can be used in two ways. It can
    be the source of a texture access from a Shader, or it can be used
    as a render target.

    A Texture3D object cannot be instantiated directly, it requires a context.
    Use :py:meth:`Context.texture3d` to create one.
    """

    repeat_x: bool
    """
    bool: The x repeat flag for the texture (Default ``True``).

    Example::

        # Enable texture repeat (GL_REPEAT)
        texture.repeat_x = True

        # Disable texture repeat (GL_CLAMP_TO_EDGE)
        texture.repeat_x = False
    """

    repeat_y: bool
    """
    bool: The y repeat flag for the texture (Default ``True``).

    Example::

        # Enable texture repeat (GL_REPEAT)
        texture.repeat_y = True

        # Disable texture repeat (GL_CLAMP_TO_EDGE)
        texture.repeat_y = False
    """

    repeat_z: bool
    """
    bool: The z repeat flag for the texture (Default ``True``).

    Example::

        # Enable texture repeat (GL_REPEAT)
        texture.repeat_z = True

        # Disable texture repeat (GL_CLAMP_TO_EDGE)
        texture.repeat_z = False
    """

    filter: Tuple[int, int]
    """The filter of the texture."""

    swizzle: str
    """
    str: The swizzle mask of the texture (Default ``'RGBA'``).

    The swizzle mask change/reorder the ``vec4`` value returned by the ``texture()`` function
    in a GLSL shaders. This is represented by a 4 character string were each
    character can be::

        'R' GL_RED
        'G' GL_GREEN
        'B' GL_BLUE
        'A' GL_ALPHA
        '0' GL_ZERO
        '1' GL_ONE

    Example::

        # Alpha channel will always return 1.0
        texture.swizzle = 'RGB1'

        # Only return the red component. The rest is masked to 0.0
        texture.swizzle = 'R000'

        # Reverse the components
        texture.swizzle = 'ABGR'
    """

    width: int
    """The width of the texture."""

    height: int
    """The height of the texture."""

    depth: int
    """The depth of the texture."""

    size: tuple
    """The size of the texture."""

    components: int
    """The number of components of the texture."""

    dtype: str
    """Data type."""

    mglo: Any
    """Internal representation for debug purposes only."""

    ctx: "Context"
    """The context this object belongs to"""

    extra: Any
    """Attribute for storing user defined objects"""

    glo: int
    """
    The internal OpenGL object.

    This values is provided for debug purposes only.
    """

    def read(self, alignment: int = 1) -> bytes:
        """
        Read the pixel data as bytes into system memory.

        Keyword Args:
            alignment (int): The byte alignment of the pixels.

        Returns:
            bytes
        """
    def read_into(
        self,
        buffer: Any,
        alignment: int = 1,
        write_offset: int = 0,
    ) -> None:
        """
        Read the content of the texture into a bytearray or :py:class:`~moderngl.Buffer`.

        The advantage of reading into a :py:class:`~moderngl.Buffer` is that pixel data
        does not need to travel all the way to system memory::

            # Reading pixel data into a bytearray
            data = bytearray(8)
            texture = ctx.texture3d((2, 2, 2), 1)
            texture.read_into(data)

            # Reading pixel data into a buffer
            data = ctx.buffer(reserve=8)
            texture = ctx.texture3d((2, 2, 2), 1)
            texture.read_into(data)

        Args:
            buffer (Union[bytearray, Buffer]): The buffer that will receive the pixels.

        Keyword Args:
            alignment (int): The byte alignment of the pixels.
            write_offset (int): The write offset.
        """
    def write(
        self,
        data: Any,
        viewport: Optional[Union[Tuple[int, int, int], Tuple[int, int, int, int, int, int]]] = None,
        alignment: int = 1,
    ) -> None:
        r"""
        Update the content of the texture from byte data or a moderngl :py:class:`~moderngl.Buffer`.

        Examples::

            # Write data from a moderngl Buffer
            data = ctx.buffer(reserve=8)
            texture = ctx.texture3d((2, 2, 2), 1)
            texture.write(data)

            # Write data from bytes
            data = b'\xff\xff\xff\xff\xff\xff\xff\xff'
            texture = ctx.texture3d((2, 2), 1)
            texture.write(data)

        Args:
            data (bytes): The pixel data.
            viewport (tuple): The viewport.

        Keyword Args:
            alignment (int): The byte alignment of the pixels.
        """
    def build_mipmaps(self, base: int = 0, max_level: int = 1000) -> None:
        """
        Generate mipmaps.

        This also changes the texture filter to ``LINEAR_MIPMAP_LINEAR, LINEAR``
        (Will be removed in ``6.x``)

        Keyword Args:
            base (int): The base level
            max_level (int): The maximum levels to generate
        """
    def use(self, location: int = 0) -> None:
        """
        Bind the texture to a texture unit.

        The location is the texture unit we want to bind the texture.
        This should correspond with the value of the ``sampler3D``
        uniform in the shader because samplers read from the texture
        unit we assign to them::

            # Define what texture unit our two sampler3D uniforms should represent
            program['texture_a'] = 0
            program['texture_b'] = 1
            # Bind textures to the texture units
            first_texture.use(location=0)
            second_texture.use(location=1)

        Args:
            location (int): The texture location/unit.
        """
    def bind_to_image(
        self,
        unit: int,
        read: bool = True,
        write: bool = True,
        level: int = 0,
        format: int = 0,
    ) -> None:
        """
        Bind a texture to an image unit (OpenGL 4.2 required).

        This is used to bind textures to image units for shaders.
        The idea with image load/store is that the user can bind
        one of the images in a Texture to a number of image binding points
        (which are separate from texture image units). Shaders can read
        information from these images and write information to them,
        in ways that they cannot with textures.

        It's important to specify the right access type for the image.
        This can be set with the ``read`` and ``write`` arguments.
        Allowed combinations are:

        - **Read-only**: ``read=True`` and ``write=False``
        - **Write-only**: ``read=False`` and ``write=True``
        - **Read-write**: ``read=True`` and ``write=True``

        ``format`` specifies the format that is to be used when performing
        formatted stores into the image from shaders. ``format`` must be
        compatible with the texture's internal format. **By default the format
        of the texture is passed in. The format parameter is only needed
        when overriding this behavior.**

        Note that we bind the 3D textured layered making the entire texture
        readable and writable. It is possible to bind a specific 2D section
        in the future.

        More information:

        - https://www.khronos.org/opengl/wiki/Image_Load_Store
        - https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindImageTexture.xhtml

        Args:
            unit (int): Specifies the index of the image unit to which to bind the texture
            texture (:py:class:`moderngl.Texture`): The texture to bind
        Keyword Args:
            read (bool): Allows the shader to read the image (default: ``True``)
            write (bool): Allows the shader to write to the image (default: ``True``)
            level (int): Level of the texture to bind (default: ``0``).
            format (int): (optional) The OpenGL enum value representing the format (defaults to the texture's format)
        """
    def get_handle(self, resident: bool = True) -> int:
        """
        Handle for Bindless Textures.

        Once a handle is created its parameters cannot be changed.
        Attempting to do so will have no effect. (filter, wrap etc).
        There is no way to undo this immutability.

        Handles cannot be used by shaders until they are resident.
        This method can be called multiple times to move a texture
        in and out of residency::

            >> texture.get_handle(resident=False)
            4294969856
            >> texture.get_handle(resident=True)
            4294969856

        Ths same handle is returned if the handle already exists.

        .. note:: Limitations from the OpenGL wiki

            The amount of storage available for resident images/textures may be less
            than the total storage for textures that is available. As such, you should
            attempt to minimize the time a texture spends being resident. Do not attempt
            to take steps like making textures resident/unresident every frame or something.
            But if you are finished using a texture for some time, make it unresident.

        Keyword Args:
            resident (bool): Make the texture resident.
        """
    def release(self) -> None:
        """Release the ModernGL object."""

class TextureArray:
    """
    An Array Texture is a Texture where each mipmap level contains an array of images of the same size.

    Array textures may have Mipmaps, but each mipmap
    in the texture has the same number of levels.

    A TextureArray object cannot be instantiated directly, it requires a context.
    Use :py:meth:`Context.texture_array` to create one.
    """

    repeat_x: bool
    """
    bool: The x repeat flag for the texture (Default ``True``).

    Example::

        # Enable texture repeat (GL_REPEAT)
        texture.repeat_x = True

        # Disable texture repeat (GL_CLAMP_TO_EDGE)
        texture.repeat_x = False
    """

    repeat_y: bool
    """
    bool: The y repeat flag for the texture (Default ``True``).

    Example::

        # Enable texture repeat (GL_REPEAT)
        texture.repeat_y = True

        # Disable texture repeat (GL_CLAMP_TO_EDGE)
        texture.repeat_y = False
    """

    filter: Tuple[int, int]
    """
    tuple: The minification and magnification filter for the texture.

    (Default ``(moderngl.LINEAR. moderngl.LINEAR)``)

    Example::

        texture.filter == (moderngl.NEAREST, moderngl.NEAREST)
        texture.filter == (moderngl.LINEAR_MIPMAP_LINEAR, moderngl.LINEAR)
        texture.filter == (moderngl.NEAREST_MIPMAP_LINEAR, moderngl.NEAREST)
        texture.filter == (moderngl.LINEAR_MIPMAP_NEAREST, moderngl.NEAREST)
    """

    swizzle: str
    """
    str: The swizzle mask of the texture (Default ``'RGBA'``).

    The swizzle mask change/reorder the ``vec4`` value returned by the ``texture()`` function
    in a GLSL shaders. This is represented by a 4 character string were each
    character can be::

        'R' GL_RED
        'G' GL_GREEN
        'B' GL_BLUE
        'A' GL_ALPHA
        '0' GL_ZERO
        '1' GL_ONE

    Example::

        # Alpha channel will always return 1.0
        texture.swizzle = 'RGB1'

        # Only return the red component. The rest is masked to 0.0
        texture.swizzle = 'R000'

        # Reverse the components
        texture.swizzle = 'ABGR'
    """

    anisotropy: float
    """
    Number of samples for anisotropic filtering (Default ``1.0``).

    The value will be clamped in range ``1.0`` and ``ctx.max_anisotropy``.

    Any value greater than 1.0 counts as a use of anisotropic filtering::

        # Disable anisotropic filtering
        texture.anisotropy = 1.0

        # Enable anisotropic filtering suggesting 16 samples as a maximum
        texture.anisotropy = 16.0
    """

    width: int
    """The width of the texture array."""

    height: int
    """The height of the texture array."""

    layers: int
    """The number of layers of the texture array."""

    size: tuple
    """The size of the texture array."""

    components: int
    """The number of components of the texture array."""

    dtype: str
    """Data type."""

    mglo: Any
    """Internal representation for debug purposes only."""

    ctx: "Context"
    """The context this object belongs to"""

    extra: Any
    """Attribute for storing user defined objects"""

    glo: int
    """
    The internal OpenGL object.

    This values is provided for debug purposes only.
    """

    def read(self, alignment: int = 1) -> bytes:
        """
        Read the pixel data as bytes into system memory.

        Keyword Args:
            alignment (int): The byte alignment of the pixels.

        Returns:
            bytes
        """
    def read_into(
        self,
        buffer: Any,
        alignment: int = 1,
        write_offset: int = 0,
    ) -> None:
        """
        Read the content of the texture array into a bytearray or :py:class:`~moderngl.Buffer`.

        The advantage of reading into a :py:class:`~moderngl.Buffer` is that pixel data
        does not need to travel all the way to system memory::

            # Reading pixel data into a bytearray
            data = bytearray(8)
            texture = ctx.texture((2, 2, 2), 1)
            texture.read_into(data)

            # Reading pixel data into a buffer
            data = ctx.buffer(reserve=8)
            texture = ctx.texture((2, 2, 2), 1)
            texture.read_into(data)

        Args:
            buffer (Union[bytearray, Buffer]): The buffer that will receive the pixels.

        Keyword Args:
            alignment (int): The byte alignment of the pixels.
            write_offset (int): The write offset.
        """
    def write(
        self,
        data: Any,
        viewport: Optional[Union[Tuple[int, int, int], Tuple[int, int, int, int, int, int]]] = None,
        alignment: int = 1,
    ) -> None:
        r"""
        Update the content of the texture array from byte data or a moderngl :py:class:`~moderngl.Buffer`.

        The ``viewport`` can be used for finer control of where the
        data should be written in the array. The valid versions are::

            # Writing multiple layers from the begining of the texture
            texture.write(data, viewport=(width, hight, num_layers))

            # Writing sub-sections of the array
            texture.write(data, viewport=(x, y, layer, width, height, num_layers))

        Like with other texture types we can also use bytes or :py:class:`~moderngl.Buffer`
        as a source::

            # Using a moderngl buffer
            data = ctx.buffer(reserve=8)
            texture = ctx.texture_array((2, 2, 2), 1)
            texture.write(data)

            # Using byte data from system memory
            data = b'\xff\xff\xff\xff\xff\xff\xff\xff'
            texture = ctx.texture_array((2, 2, 2), 1)
            texture.write(data)

        Args:
            data (bytes): The pixel data.
            viewport (tuple): The viewport.

        Keyword Args:
            alignment (int): The byte alignment of the pixels.
        """
    def build_mipmaps(self, base: int = 0, max_level: int = 1000) -> None:
        """
        Generate mipmaps.

        This also changes the texture filter to ``LINEAR_MIPMAP_LINEAR, LINEAR``
        (Will be removed in ``6.x``)

        Keyword Args:
            base (int): The base level
            max_level (int): The maximum levels to generate
        """
    def use(self, location: int = 0) -> None:
        """
        Bind the texture to a texture unit.

        The location is the texture unit we want to bind the texture.
        This should correspond with the value of the ``sampler2DArray``
        uniform in the shader because samplers read from the texture
        unit we assign to them::

            # Define what texture unit our two sampler2DArray uniforms should represent
            program['texture_a'] = 0
            program['texture_b'] = 1
            # Bind textures to the texture units
            first_texture.use(location=0)
            second_texture.use(location=1)

        Args:
            location (int): The texture location/unit.
        """
    def bind_to_image(
        self,
        unit: int,
        read: bool = True,
        write: bool = True,
        level: int = 0,
        format: int = 0,
    ) -> None:
        """
        Bind a texture to an image unit (OpenGL 4.2 required).

        This is used to bind textures to image units for shaders.
        The idea with image load/store is that the user can bind
        one of the images in a Texture to a number of image binding points
        (which are separate from texture image units). Shaders can read
        information from these images and write information to them,
        in ways that they cannot with textures.

        It's important to specify the right access type for the image.
        This can be set with the ``read`` and ``write`` arguments.
        Allowed combinations are:

        - **Read-only**: ``read=True`` and ``write=False``
        - **Write-only**: ``read=False`` and ``write=True``
        - **Read-write**: ``read=True`` and ``write=True``

        ``format`` specifies the format that is to be used when performing
        formatted stores into the image from shaders. ``format`` must be
        compatible with the texture's internal format. **By default the format
        of the texture is passed in. The format parameter is only needed
        when overriding this behavior.**

        Note that we bind the texture array as layered to make
        all the layers accessible. This can be updated to map
        single layers in the future.

        More information:

        - https://www.khronos.org/opengl/wiki/Image_Load_Store
        - https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindImageTexture.xhtml

        Args:
            unit (int): Specifies the index of the image unit to which to bind the texture
            texture (:py:class:`moderngl.Texture`): The texture to bind
        Keyword Args:
            read (bool): Allows the shader to read the image (default: ``True``)
            write (bool): Allows the shader to write to the image (default: ``True``)
            level (int): Level of the texture to bind (default: ``0``).
            format (int): (optional) The OpenGL enum value representing the format (defaults to the texture's format)
        """
    def get_handle(self, resident: bool = True) -> int:
        """
        Handle for Bindless Textures.

        Once a handle is created its parameters cannot be changed.
        Attempting to do so will have no effect. (filter, wrap etc).
        There is no way to undo this immutability.

        Handles cannot be used by shaders until they are resident.
        This method can be called multiple times to move a texture
        in and out of residency::

            >> texture.get_handle(resident=False)
            4294969856
            >> texture.get_handle(resident=True)
            4294969856

        Ths same handle is returned if the handle already exists.

        .. note:: Limitations from the OpenGL wiki

            The amount of storage available for resident images/textures may be less
            than the total storage for textures that is available. As such, you should
            attempt to minimize the time a texture spends being resident. Do not attempt
            to take steps like making textures resident/unresident every frame or something.
            But if you are finished using a texture for some time, make it unresident.

        Keyword Args:
            resident (bool): Make the texture resident.
        """
    def release(self) -> None:
        """Release the ModernGL object."""

class TextureCube:
    """
    Cubemaps are a texture using the type GL_TEXTURE_CUBE_MAP.

    They are similar to 2D textures in that they have two dimensions.
    However, each mipmap level has 6 faces, with each face having the
    same size as the other faces.

    The width and height of a cubemap must be the same (ie: cubemaps are squares),
    but these sizes need not be powers of two.

    .. Note:: ModernGL enables ``GL_TEXTURE_CUBE_MAP_SEAMLESS`` globally
                to ensure filtering will be done across the cube faces.

    A Texture3D object cannot be instantiated directly, it requires a context.
    Use :py:meth:`Context.texture_cube` to create one.
    """

    size: Tuple[int, int]
    """The size of the texture cube (single face)."""

    components: int
    """The number of components of the texture."""

    dtype: str
    """Data type."""

    filter: Tuple[int, int]
    """
    tuple: The minification and magnification filter for the texture.

    (Default ``(moderngl.LINEAR. moderngl.LINEAR)``)

    Example::

        texture.filter == (moderngl.NEAREST, moderngl.NEAREST)
        texture.filter == (moderngl.LINEAR_MIPMAP_LINEAR, moderngl.LINEAR)
        texture.filter == (moderngl.NEAREST_MIPMAP_LINEAR, moderngl.NEAREST)
        texture.filter == (moderngl.LINEAR_MIPMAP_NEAREST, moderngl.NEAREST)
    """

    swizzle: str
    """
    str: The swizzle mask of the texture (Default ``'RGBA'``).

    The swizzle mask change/reorder the ``vec4`` value returned by the ``texture()`` function
    in a GLSL shaders. This is represented by a 4 character string were each
    character can be::

        'R' GL_RED
        'G' GL_GREEN
        'B' GL_BLUE
        'A' GL_ALPHA
        '0' GL_ZERO
        '1' GL_ONE

    Example::

        # Alpha channel will always return 1.0
        texture.swizzle = 'RGB1'

        # Only return the red component. The rest is masked to 0.0
        texture.swizzle = 'R000'

        # Reverse the components
        texture.swizzle = 'ABGR'
    """

    compare_func: str
    """
    tuple: The compare function of the depth texture (Default ``'<='``).

    By default depth textures have ``GL_TEXTURE_COMPARE_MODE`` set to
    ``GL_COMPARE_REF_TO_TEXTURE``, meaning any texture lookup will
    return a depth comparison value.

    If you need to read the actual depth value in shaders, setting
    ``compare_func`` to a blank string will set ``GL_TEXTURE_COMPARE_MODE`` to
    ``GL_NONE`` making you able to read the depth texture as a ``sampler2D``::

        uniform sampler2D depth;
        out vec4 fragColor;
        in vec2 uv;

        void main() {
            float raw_depth_nonlinear = texture(depth, uv);
            fragColor = vec4(raw_depth_nonlinear);
        }

    Accepted compare functions::

        texture.compare_func = ''    # Disable depth comparison completely
        texture.compare_func = '<='  # GL_LEQUAL
        texture.compare_func = '<'   # GL_LESS
        texture.compare_func = '>='  # GL_GEQUAL
        texture.compare_func = '>'   # GL_GREATER
        texture.compare_func = '=='  # GL_EQUAL
        texture.compare_func = '!='  # GL_NOTEQUAL
        texture.compare_func = '0'   # GL_NEVER
        texture.compare_func = '1'   # GL_ALWAYS
    """

    anisotropy: float
    """
    Number of samples for anisotropic filtering (Default ``1.0``).

    The value will be clamped in range ``1.0`` and ``ctx.max_anisotropy``.

    Any value greater than 1.0 counts as a use of anisotropic filtering::

        # Disable anisotropic filtering
        texture.anisotropy = 1.0

        # Enable anisotropic filtering suggesting 16 samples as a maximum
        texture.anisotropy = 16.0
    """

    mglo: Any
    """Internal representation for debug purposes only."""

    ctx: "Context"
    """The context this object belongs to"""

    depth: bool
    """Is the texture a depth texture?."""

    extra: Any
    """Attribute for storing user defined objects"""

    glo: int
    """
    The internal OpenGL object.

    This values is provided for debug purposes only.
    """

    def read(self, face: int, alignment: int = 1) -> bytes:
        """
        Read a face from the cubemap as bytes into system memory.

        Face values are::

            0: Positive X
            1: Negative X
            2: Positive Y
            3: Negative Y
            4: Positive Z
            5: Negative Z

        Args:
            face (int): The face to read.

        Keyword Args:
            alignment (int): The byte alignment of the pixels.
        """
    def read_into(
        self,
        buffer: Any,
        face: int,
        alignment: int = 1,
        write_offset: int = 0,
    ) -> None:
        """
        Read a face from the cubemap texture.

        Read a face of the cubemap into a bytearray or :py:class:`~moderngl.Buffer`.
        The advantage of reading into a :py:class:`~moderngl.Buffer` is that pixel data
        does not need to travel all the way to system memory::

            # Reading pixel data into a bytearray
            data = bytearray(4)
            texture = ctx.texture_cube((2, 2), 1)
            texture.read_into(data, 0)

            # Reading pixel data into a buffer
            data = ctx.buffer(reserve=4)
            texture = ctx.texture_cube((2, 2), 1)
            texture.read_into(data, 0)

        Args:
            buffer (bytearray): The buffer that will receive the pixels.
            face (int): The face to read.

        Keyword Args:
            alignment (int): The byte alignment of the pixels.
            write_offset (int): The write offset.
        """
    def write(
        self,
        face: int,
        data: Any,
        viewport: Optional[Union[Tuple[int, int], Tuple[int, int, int, int]]] = None,
        alignment: int = 1,
    ) -> None:
        r"""
        Update the content of the texture.

        Update the content of a face in the cubemap from byte data
        or a moderngl :py:class:`~moderngl.Buffer`::

            # Write data from a moderngl Buffer
            data = ctx.buffer(reserve=4)
            texture = ctx.texture_cube((2, 2), 1)
            texture.write(0, data)

            # Write data from bytes
            data = b'\xff\xff\xff\xff'
            texture = ctx.texture_cube((2, 2), 1)
            texture.write(0, data)

        Args:
            face (int): The face to update.
            data (bytes): The pixel data.
            viewport (tuple): The viewport.

        Keyword Args:
            alignment (int): The byte alignment of the pixels.
        """
    def build_mipmaps(self, base: int = 0, max_level: int = 1000) -> None:
        """
        Generate mipmaps.

        This also changes the texture filter to ``LINEAR_MIPMAP_LINEAR, LINEAR``
        (Will be removed in ``6.x``)

        Keyword Args:
            base (int): The base level
            max_level (int): The maximum levels to generate
        """
    def use(self, location: int = 0) -> None:
        """
        Bind the texture to a texture unit.

        The location is the texture unit we want to bind the texture.
        This should correspond with the value of the ``samplerCube``
        uniform in the shader because samplers read from the texture
        unit we assign to them::

            # Define what texture unit our two samplerCube uniforms should represent
            program['texture_a'] = 0
            program['texture_b'] = 1
            # Bind textures to the texture units
            first_texture.use(location=0)
            second_texture.use(location=1)

        Args:
            location (int): The texture location/unit.
        """
    def bind_to_image(
        self,
        unit: int,
        read: bool = True,
        write: bool = True,
        level: int = 0,
        format: int = 0,
    ) -> None:
        """
        Bind a texture to an image unit (OpenGL 4.2 required).

        This is used to bind textures to image units for shaders.
        The idea with image load/store is that the user can bind
        one of the images in a Texture to a number of image binding points
        (which are separate from texture image units). Shaders can read
        information from these images and write information to them,
        in ways that they cannot with textures.

        It's important to specify the right access type for the image.
        This can be set with the ``read`` and ``write`` arguments.
        Allowed combinations are:

        - **Read-only**: ``read=True`` and ``write=False``
        - **Write-only**: ``read=False`` and ``write=True``
        - **Read-write**: ``read=True`` and ``write=True``

        ``format`` specifies the format that is to be used when performing
        formatted stores into the image from shaders. ``format`` must be
        compatible with the texture's internal format. **By default the format
        of the texture is passed in. The format parameter is only needed
        when overriding this behavior.**

        Note that we bind the texture cube as layered to make
        all the faces accessible. This can be updated to map
        single faces in the future. The Z component in imageLoad/Store
        represents the face id we are writing to (0-5).

        More information:

        - https://www.khronos.org/opengl/wiki/Image_Load_Store
        - https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindImageTexture.xhtml

        Args:
            unit (int): Specifies the index of the image unit to which to bind the texture
            texture (:py:class:`moderngl.Texture`): The texture to bind
        Keyword Args:
            read (bool): Allows the shader to read the image (default: ``True``)
            write (bool): Allows the shader to write to the image (default: ``True``)
            level (int): Level of the texture to bind (default: ``0``).
            format (int): (optional) The OpenGL enum value representing the format (defaults to the texture's format)
        """
    def get_handle(self, resident: bool = True) -> int:
        """
        Handle for Bindless Textures.

        Once a handle is created its parameters cannot be changed.
        Attempting to do so will have no effect. (filter, wrap etc).
        There is no way to undo this immutability.

        Handles cannot be used by shaders until they are resident.
        This method can be called multiple times to move a texture
        in and out of residency::

            >> texture.get_handle(resident=False)
            4294969856
            >> texture.get_handle(resident=True)
            4294969856

        Ths same handle is returned if the handle already exists.

        .. note:: Limitations from the OpenGL wiki

            The amount of storage available for resident images/textures may be less
            than the total storage for textures that is available. As such, you should
            attempt to minimize the time a texture spends being resident. Do not attempt
            to take steps like making textures resident/unresident every frame or something.
            But if you are finished using a texture for some time, make it unresident.

        Keyword Args:
            resident (bool): Make the texture resident.
        """
    def release(self) -> None:
        """Release the ModernGL object."""

class Texture:
    """
    A Texture is an OpenGL object that contains one or more images that all have the same image format.

    A texture can be used in two ways. It can
    be the source of a texture access from a Shader, or it can be used
    as a render target.

    A Texture object cannot be instantiated directly, it requires a context.
    Use :py:meth:`Context.texture` or :py:meth:`Context.depth_texture`
    to create one.
    """

    repeat_x: bool
    """
    bool: The x repeat flag for the texture (Default ``True``).

    Example::

        # Enable texture repeat (GL_REPEAT)
        texture.repeat_x = True

        # Disable texture repeat (GL_CLAMP_TO_EDGE)
        texture.repeat_x = False
    """

    repeat_y: bool
    """
    bool: The y repeat flag for the texture (Default ``True``).

    Example::

        # Enable texture repeat (GL_REPEAT)
        texture.repeat_y = True

        # Disable texture repeat (GL_CLAMP_TO_EDGE)
        texture.repeat_y = False
    """

    filter: Tuple[int, int]
    """
    tuple: The minification and magnification filter for the texture.

    (Default ``(moderngl.LINEAR. moderngl.LINEAR)``)

    Example::

        texture.filter == (moderngl.NEAREST, moderngl.NEAREST)
        texture.filter == (moderngl.LINEAR_MIPMAP_LINEAR, moderngl.LINEAR)
        texture.filter == (moderngl.NEAREST_MIPMAP_LINEAR, moderngl.NEAREST)
        texture.filter == (moderngl.LINEAR_MIPMAP_NEAREST, moderngl.NEAREST)
    """

    anisotropy: float
    """
    Number of samples for anisotropic filtering (Default ``1.0``).

    The value will be clamped in range ``1.0`` and ``ctx.max_anisotropy``.

    Any value greater than 1.0 counts as a use of anisotropic filtering::

        # Disable anisotropic filtering
        texture.anisotropy = 1.0

        # Enable anisotropic filtering suggesting 16 samples as a maximum
        texture.anisotropy = 16.0
    """

    swizzle: str
    """
    str: The swizzle mask of the texture (Default ``'RGBA'``).

    The swizzle mask change/reorder the ``vec4`` value returned by the ``texture()`` function
    in a GLSL shaders. This is represented by a 4 character string were each
    character can be::

        'R' GL_RED
        'G' GL_GREEN
        'B' GL_BLUE
        'A' GL_ALPHA
        '0' GL_ZERO
        '1' GL_ONE

    Example::

        # Alpha channel will always return 1.0
        texture.swizzle = 'RGB1'

        # Only return the red component. The rest is masked to 0.0
        texture.swizzle = 'R000'

        # Reverse the components
        texture.swizzle = 'ABGR'
    """

    compare_func: str
    """
    tuple: The compare function of the depth texture (Default ``'<='``).

    By default depth textures have ``GL_TEXTURE_COMPARE_MODE`` set to
    ``GL_COMPARE_REF_TO_TEXTURE``, meaning any texture lookup will
    return a depth comparison value.

    If you need to read the actual depth value in shaders, setting
    ``compare_func`` to a blank string will set ``GL_TEXTURE_COMPARE_MODE`` to
    ``GL_NONE`` making you able to read the depth texture as a ``sampler2D``::

        uniform sampler2D depth;
        out vec4 fragColor;
        in vec2 uv;

        void main() {
            float raw_depth_nonlinear = texture(depth, uv);
            fragColor = vec4(raw_depth_nonlinear);
        }

    Accepted compare functions::

        texture.compare_func = ''    # Disable depth comparison completely
        texture.compare_func = '<='  # GL_LEQUAL
        texture.compare_func = '<'   # GL_LESS
        texture.compare_func = '>='  # GL_GEQUAL
        texture.compare_func = '>'   # GL_GREATER
        texture.compare_func = '=='  # GL_EQUAL
        texture.compare_func = '!='  # GL_NOTEQUAL
        texture.compare_func = '0'   # GL_NEVER
        texture.compare_func = '1'   # GL_ALWAYS
    """

    width: int
    """The width of the texture."""

    height: int
    """The height of the texture."""

    size: tuple
    """The size of the texture."""

    components: int
    """The number of components of the texture."""

    samples: int
    """The number of samples set for the texture used in multisampling."""

    dtype: str
    """Data type."""

    depth: bool
    """Is the texture a depth texture?."""

    mglo: Any
    """Internal representation for debug purposes only."""

    ctx: "Context"
    """The context this object belongs to"""

    extra: Any
    """Attribute for storing user defined objects"""

    glo: int
    """
    The internal OpenGL object.

    This values is provided for debug purposes only.
    """

    def read(self, level: int = 0, alignment: int = 1) -> bytes:
        """
        Read the pixel data as bytes into system memory.

        The texture can also be attached to a :py:class:`Framebuffer`
        to gain access to :py:meth:`Framebuffer.read` for additional
        features such ad reading a subsection or converting to
        another ``dtype``.

        Keyword Args:
            level (int): The mipmap level.
            alignment (int): The byte alignment of the pixels.

        Returns:
            bytes
        """
    def read_into(
        self,
        buffer: Any,
        level: int = 0,
        alignment: int = 1,
        write_offset: int = 0,
    ) -> None:
        """
        Read the content of the texture into a bytearray or :py:class:`~moderngl.Buffer`.

        The advantage of reading into a :py:class:`~moderngl.Buffer` is that pixel data
        does not need to travel all the way to system memory::

            # Reading pixel data into a bytearray
            data = bytearray(4)
            texture = ctx.texture((2, 2), 1)
            texture.read_into(data)

            # Reading pixel data into a buffer
            data = ctx.buffer(reserve=4)
            texture = ctx.texture((2, 2), 1)
            texture.read_into(data)

        Args:
            buffer (Union[bytearray, Buffer]): The buffer that will receive the pixels.

        Keyword Args:
            level (int): The mipmap level.
            alignment (int): The byte alignment of the pixels.
            write_offset (int): The write offset.
        """
    def write(
        self,
        data: Any,
        viewport: Optional[Union[Tuple[int, int], Tuple[int, int, int, int]]] = None,
        level: int = 0,
        alignment: int = 1,
    ) -> None:
        r"""
        Update the content of the texture from byte data or a moderngl :py:class:`~moderngl.Buffer`.

        Examples::

            # Write data from a moderngl Buffer
            data = ctx.buffer(reserve=4)
            texture = ctx.texture((2, 2), 1)
            texture.write(data)

            # Write data from bytes
            data = b'\xff\xff\xff\xff'
            texture = ctx.texture((2, 2), 1)
            texture.write(data)

            # Write to a sub-section of the texture using viewport
            texture = ctx.texture((100, 100), 4)
            # Fill the lower left 50x50 pixels with new data
            texture.write(data, viewport=(0, 0, 50, 50))

        Args:
            data (Union[bytes, Buffer]): The pixel data.
            viewport (tuple): The sub-section of the texture to update
                                in viewport coordinates. The data size
                                must match the size of the area.

        Keyword Args:
            level (int): The mipmap level.
            alignment (int): The byte alignment of the pixels.
        """
    def build_mipmaps(self, base: int = 0, max_level: int = 1000) -> None:
        """
        Generate mipmaps.

        This also changes the texture filter to ``LINEAR_MIPMAP_LINEAR, LINEAR``
        (Will be removed in ``6.x``)

        Keyword Args:
            base (int): The base level
            max_level (int): The maximum levels to generate
        """
    def use(self, location: int = 0) -> None:
        """
        Bind the texture to a texture unit.

        The location is the texture unit we want to bind the texture.
        This should correspond with the value of the ``sampler2D``
        uniform in the shader because samplers read from the texture
        unit we assign to them::

            # Define what texture unit our two sampler2D uniforms should represent
            program['texture_a'] = 0
            program['texture_b'] = 1
            # Bind textures to the texture units
            first_texture.use(location=0)
            second_texture.use(location=1)

        Args:
            location (int): The texture location/unit.
        """
    def bind_to_image(
        self,
        unit: int,
        read: bool = True,
        write: bool = True,
        level: int = 0,
        format: int = 0,
    ) -> None:
        """
        Bind a texture to an image unit (OpenGL 4.2 required).

        This is used to bind textures to image units for shaders.
        The idea with image load/store is that the user can bind
        one of the images in a Texture to a number of image binding points
        (which are separate from texture image units). Shaders can read
        information from these images and write information to them,
        in ways that they cannot with textures.

        It's important to specify the right access type for the image.
        This can be set with the ``read`` and ``write`` arguments.
        Allowed combinations are:

        - **Read-only**: ``read=True`` and ``write=False``
        - **Write-only**: ``read=False`` and ``write=True``
        - **Read-write**: ``read=True`` and ``write=True``

        ``format`` specifies the format that is to be used when performing
        formatted stores into the image from shaders. ``format`` must be
        compatible with the texture's internal format. **By default the format
        of the texture is passed in. The format parameter is only needed
        when overriding this behavior.**

        More information:

        - https://www.khronos.org/opengl/wiki/Image_Load_Store
        - https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindImageTexture.xhtml

        Args:
            unit (int): Specifies the index of the image unit to which to bind the texture
            texture (:py:class:`moderngl.Texture`): The texture to bind
        Keyword Args:
            read (bool): Allows the shader to read the image (default: ``True``)
            write (bool): Allows the shader to write to the image (default: ``True``)
            level (int): Level of the texture to bind (default: ``0``).
            format (int): (optional) The OpenGL enum value representing the format (defaults to the texture's format)
        """
    def get_handle(self, resident: bool = True) -> int:
        """
        Handle for Bindless Textures.

        Once a handle is created its parameters cannot be changed.
        Attempting to do so will have no effect. (filter, wrap etc).
        There is no way to undo this immutability.

        Handles cannot be used by shaders until they are resident.
        This method can be called multiple times to move a texture
        in and out of residency::

            >> texture.get_handle(resident=False)
            4294969856
            >> texture.get_handle(resident=True)
            4294969856

        Ths same handle is returned if the handle already exists.

        .. note:: Limitations from the OpenGL wiki

            The amount of storage available for resident images/textures may be less
            than the total storage for textures that is available. As such, you should
            attempt to minimize the time a texture spends being resident. Do not attempt
            to take steps like making textures resident/unresident every frame or something.
            But if you are finished using a texture for some time, make it unresident.

        Keyword Args:
            resident (bool): Make the texture resident.
        """
    def release(self) -> None:
        """Release the ModernGL object."""

class VertexArray:
    """
    A VertexArray object is an OpenGL object that stores all of the state needed to supply vertex data.

    It stores the format of the vertex data
    as well as the Buffer objects providing the vertex data arrays.

    In ModernGL, the VertexArray object also stores a reference
    for a :py:class:`Program` object.

    A VertexArray object cannot be instantiated directly, it requires a context.
    Use :py:meth:`Context.vertex_array` or :py:meth:`Context.simple_vertex_array`
    to create one.
    """

    scope: Optional["Scope"]
    """The scope to use while rendering."""

    mode: int
    """
    Get or set the default rendering mode.

    This value is used when ``mode`` is not passed in rendering calls.

    Examples::

        vao.mode = moderngl.TRIANGLE_STRIPS
    """

    program: Program
    """
    Program: The program assigned to the VertexArray.

    The program used when rendering or transforming primitives.
    """

    index_buffer: Buffer
    """Index buffer"""

    index_element_size: int
    """The size of an index in bytes"""

    vertices: int
    """
    The number of vertices detected.

    This is the minimum of the number of vertices possible per Buffer.
    The size of the index_buffer determines the number of vertices.
    Per instance vertex attributes does not affect this number.
    """

    instances: int
    """Get or set the number of instances to render."""

    mglo: Any
    """Internal representation for debug purposes only."""

    ctx: "Context"
    """The context this object belongs to"""

    extra: Any
    """Attribute for storing user defined objects"""

    glo: int
    """
    The internal OpenGL object.

    This values is provided for debug purposes only.
    """

    def render(
        self,
        mode: Optional[int] = None,
        vertices: int = -1,
        first: int = 0,
        instances: int = -1,
    ) -> None:
        """
        The render primitive (mode) must be the same as the input primitive of the GeometryShader.

        Args:
            mode (int): By default :py:data:`TRIANGLES` will be used.
            vertices (int): The number of vertices to transform.

        Keyword Args:
            first (int): The index of the first vertex to start with.
            instances (int): The number of instances.
        """
    def render_indirect(
        self,
        buffer: Buffer,
        mode: Optional[int] = None,
        count: int = -1,
        first: int = 0,
    ) -> None:
        """
        The render primitive (mode) must be the same as the input primitive of the GeometryShader.

        The draw commands are 5 integers: (count, instanceCount, firstIndex, baseVertex, baseInstance).

        Args:
            buffer (Buffer): Indirect drawing commands.
            mode (int): By default :py:data:`TRIANGLES` will be used.
            count (int): The number of draws.

        Keyword Args:
            first (int): The index of the first indirect draw command.
        """
    def transform(
        self,
        buffer: Union[Buffer, List[Buffer]],
        mode: Optional[int] = None,
        vertices: int = -1,
        first: int = 0,
        instances: int = -1,
        buffer_offset: int = 0,
    ) -> None:
        """
        Transform vertices.

        Stores the output in a single buffer.
        The transform primitive (mode) must be the same as
        the input primitive of the GeometryShader.

        Args:
            buffer (Buffer): The buffer to store the output.
            mode (int): By default :py:data:`POINTS` will be used.
            vertices (int): The number of vertices to transform.

        Keyword Args:
            first (int): The index of the first vertex to start with.
            instances (int): The number of instances.
            buffer_offset (int): Byte offset for the output buffer
        """
    def bind(
        self,
        attribute: int,
        cls: str,
        buffer: Buffer,
        fmt: str,
        offset: int = 0,
        stride: int = 0,
        divisor: int = 0,
        normalize: bool = False,
    ) -> None:
        """
        Bind individual attributes to buffers.

        Args:
            location (int): The attribute location.
            cls (str): The attribute class. Valid values are ``f``, ``i`` or ``d``.
            buffer (Buffer): The buffer.
            format (str): The buffer format.

        Keyword Args:
            offset (int): The offset.
            stride (int): The stride.
            divisor (int): The divisor.
            normalize (bool): The normalize parameter, if applicable.
        """
    def release(self) -> None:
        """Release the ModernGL object."""
