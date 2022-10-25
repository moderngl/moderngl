.. _buffer-format-label:

Buffer Format
=============

.. py:currentmodule:: moderngl

Description
-----------

A buffer format is a short string describing the layout of data in a vertex
buffer object (VBO).

A VBO often contains a homogeneous array of C-like structures. The buffer
format describes what each element of the array looks like. For example,
a buffer containing an array of high-precision 2D vertex positions might have
the format ``"2f8"`` - each element of the array consists of two floats, each
float being 8 bytes wide, ie. a double.

Buffer formats are used in the :py:meth:`Context.vertex_array()` constructor,
as the 2nd component of the `content` arg.
See the :ref:`example-of-simple-usage-label` below.

Syntax
------

A buffer format looks like:

    ``[count]type[size] [[count]type[size]...] [/usage]``

Where:

- ``count`` is an optional integer. If omitted, it defaults to ``1``.
- ``type`` is a single character indicating the data type:

   - ``f`` float
   - ``i`` int
   - ``u`` unsigned int
   - ``x`` padding
- ``size`` is an optional number of bytes used to store the type.
  If omitted, it defaults to 4 for numeric types, or to 1 for padding bytes.

  A format may contain multiple, space-separated ``[count]type[size]`` triples
  (See the :ref:`example-of-single-interleaved-array-label`), followed by:


- ``/usage`` is optional. It should be preceded by a space, and then consists
  of a slash followed by a single character, indicating how successive values
  in the buffer should be passed to the shader:

   - ``/v`` per vertex.
     Successive values from the buffer are passed to each vertex.
     This is the default behavior if usage is omitted.
   - ``/i`` per instance.
     Successive values from the buffer are passed to each instance.
   - ``/r`` per render.
     the first buffer value is passed to every vertex of every instance.
     ie. behaves like a uniform.

  When passing multiple VBOs to a VAO, the first one must be of usage ``/v``,
  as shown in the :ref:`example-of-multiple-arrays-with-differing-usage-label`.

Valid combinations of type and size are:

+----------+---------------+-------------------+-----------------+---------+
|          |                 size                                          |
+==========+===============+===================+=================+=========+
| **type** | 1             | 2                 | 4               | 8       |
+----------+---------------+-------------------+-----------------+---------+
| f        | Unsigned byte | Half float        | Float           | Double  |
|          | (normalized)  |                   |                 |         |
+----------+---------------+-------------------+-----------------+---------+
| i        | Byte          | Short             | Int             | \-      |
+----------+---------------+-------------------+-----------------+---------+
| u        | Unsigned byte | Unsigned short    | Unsigned int    | \-      |
+----------+---------------+-------------------+-----------------+---------+
| x        | 1 byte        | 2 bytes           | 4 bytes         | 8 bytes |
+----------+---------------+-------------------+-----------------+---------+

The entry ``f1`` has two unusual properties:

1. Its type is ``f`` (for float), but it defines a buffer containing unsigned
   bytes. For this size of floats only, the values are `normalized`, ie.
   unsigned bytes from 0 to 255 in the buffer are converted to float values
   from 0.0 to 1.0 by the time they reach the vertex shader. This is intended
   for passing in colors as unsigned bytes.
2. Three unsigned bytes, with a format of ``3f1``,
   may be assigned to a ``vec3`` attribute, as one would expect.
   But, from ModernGL v6.0,
   they can alternatively be passed to a ``vec4`` attribute.
   This is intended for passing a buffer of 3-byte RGB values
   into an attribute which also contains an alpha channel.

There are no size 8 variants for types ``i`` and ``u``.

This buffer format syntax is specific to ModernGL. As seen in the usage
examples below, the formats sometimes look similar to the format strings passed
to ``struct.pack``, but that is a different syntax (documented here_.)

.. _here: https://docs.python.org/3.7/library/struct.html

Buffer formats can represent a wide range of vertex attribute formats.
For rare cases of specialized attribute formats that are not expressible
using buffer formats, there is a :py:meth:`VertexArray.bind()` method, to
manually configure the underlying OpenGL binding calls. This is not generally
recommended.

Examples
--------

Example buffer formats
......................

``"2f"`` has a count of ``2`` and a type of ``f`` (float). Hence it describes
two floats, passed to a vertex shader's ``vec2`` attribute. The size of the
floats is unspecified, so defaults to ``4`` bytes. The usage of the buffer is
unspecified, so defaults to ``/v`` (vertex), meaning each successive pair of
floats in the array are passed to successive vertices during the render call.

``"3i2/i"`` means three ``i`` (integers). The size of each integer is ``2``
bytes, ie. they are shorts, passed to an ``ivec3`` attribute.
The trailing ``/i`` means that consecutive values
in the buffer are passed to successive `instances` during an instanced render
call. So the same value is passed to every vertex within a particular instance.

Buffers contining interleaved values are represented by multiple space
separated count-type-size triples. Hence:

``"2f 3u x /v"`` means:

    * ``2f``: two floats, passed to a ``vec2`` attribute, followed by
    * ``3u``: three unsigned bytes, passed to a ``uvec3``, then
    * ``x``: a single byte of padding, for alignment.

The ``/v`` indicates successive elements in the buffer are passed to successive
vertices during the render. This is the default, so the ``/v`` could be
omitted.

.. _example-of-simple-usage-label:

Example of simple usage
.......................

Consider a VBO containing 2D vertex positions, forming a single triangle::

    # a 2D triangle (ie. three (x, y) vertices)
    verts = [
         0.0, 0.9,
        -0.5, 0.0,
         0.5, 0.0,
    ]

    # pack all six values into a binary array of C-like floats
    verts_buffer = struct.pack("6f", *verts)

    # put the array into a VBO
    vbo = ctx.buffer(verts_buffer)

    # use the VBO in a VAO
    vao = ctx.vertex_array(
        shader_program,
        [
            (vbo, "2f", "in_vert"), # <---- the "2f" is the buffer format
        ]
        index_buffer_object
    )

The line ``(vbo, "2f", "in_vert")``, known as the VAO content, indicates that
``vbo`` contains an array of values, each of which consists of two floats.
These values are passed to an ``in_vert`` attribute,
declared in the vertex shader as::

    in vec2 in_vert;

The ``"2f"`` format omits a ``size`` component, so the floats default to
4-bytes each. The format also omits the trailing ``/usage`` component, which
defaults to ``/v``, so successive (x, y) rows from the buffer are passed to
successive vertices during the render call.

.. _example-of-single-interleaved-array-label:

Example of single interleaved array
...................................

A buffer array might contain elements consisting of multiple interleaved
values.

For example, consider a buffer array, each element of which contains a 2D
vertex position as floats, an RGB color as unsigned ints, and a single byte of
padding for alignment:

+-------+-------+----------+----------+----------+---------+
| position      | color                          | padding |
+-------+-------+----------+----------+----------+---------+
| x     | y     | r        | g        | b        | \-      |
+-------+-------+----------+----------+----------+---------+
| float | float | unsigned | unsigned | unsigned | byte    |
|       |       | byte     | byte     | byte     |         |
+-------+-------+----------+----------+----------+---------+

Such a buffer, however you choose to construct it, would then be passed into
a VAO using::

    vao = ctx.vertex_array(
        shader_program,
        [
            (vbo, "2f 3f1 x", "in_vert", "in_color")
        ]
        index_buffer_object
    )

The format starts with ``2f``, for the two position floats, which will
be passed to the shader's ``in_vert`` attribute, declared as::

    in vec2 in_vert;

Next, after a space, is ``3f1``, for the three color unsigned bytes, which
get normalized to floats by ``f1``. These floats will be passed to the shader's
``in_color`` attribute::

    in vec3 in_color;

Finally, the format ends with ``x``, a single byte of padding, which needs
no shader attribute name.

.. _example-of-multiple-arrays-with-differing-usage-label:

Example of multiple arrays with differing ``/usage``
....................................................

To illustrate the trailing ``/usage`` portion, consider rendering a dozen cubes
with instanced rendering. We will use:

* ``vbo_verts_normals`` contains vertices (3 floats) and normals (3 floats)
  for the vertices within a single cube.
* ``vbo_offset_orientation`` contains offsets (3 floats) and orientations (9
  float matrices) that are used to position and orient each cube.
* ``vbo_colors`` contains colors (3 floats). In this example, there is only
  one color in the buffer, that will be used for every vertex of every cube.

Our shader will take all the above values as attributes.

We bind the above VBOs in a single VAO, to prepare for an instanced rendering
call::

    vao = ctx.vertex_array(
        shader_program,
        [
            (vbo_verts_normals,      "3f 3f /v", "in_vert", "in_norm"),
            (vbo_offset_orientation, "3f 9f /i", "in_offset", "in_orientation"),
            (vbo_colors,             "3f /r",    "in_color"),
        ]
        index_buffer_object
    )

So, the vertices and normals, using ``/v``, are passed to each vertex within
an instance. This fulfills the rule that the first VBO in a VAO must have usage
``/v``. These are passed to vertex attributes as::

    in vec3 in_vert;
    in vec3 in_norm;

The offsets and orientations pass the same value to each vertex within an
instance, but then pass the next value in the buffer to the vertices of the
next instance. Passed as::

    in vec3 in_offset;
    in mat3 in_orientation;

The single color is passed to every vertex of every instance.
If we had stored the color with ``/v`` or ``/i``, then we would have had to
store duplicate identical color values in vbo_colors - one per instance or
one per vertex. To render all our cubes in a single color, this is needless
duplication. Using ``/r``, only one color is require the buffer, and it is
passed to every vertex of every instance for the whole render call::

    in vec3 in_color;

An alternative approach would be to pass in the color as a uniform, since
it is constant. But doing it as an attribute is more flexible. It allows us to
reuse the same shader program, bound to a different buffer, to pass in color
data which varies per instance, or per vertex.

.. toctree::
    :maxdepth: 2
