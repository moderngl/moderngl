.. _texture-format-label:

Texture Format
==============

.. py:currentmodule:: moderngl

Description
-----------

The format of a texture can be described by the ``dtype`` parameter
during texture creation. For example the :py:meth:`moderngl.Context.texture`.
The default ``dtype`` is ``f1``. Each component is an unsigned byte (0-255)
that is normalized when read in a shader into a value from 0.0 to 1.0.

The formats are based on the string formats used in numpy.

Some quick example of texture creation::

    # RGBA (4 component) f1 texture
    texture = ctx.texture((100, 100), 4)  # dtype f1 is default

    # R (1 component) f4 texture (32 bit float)
    texture = ctx.texture((100, 100), 1, dype="f4")

    # RG (2 component) u2 texture (16 bit unsigned integer)
    texture = ctx.texture((100, 100), 2, dtype="u2")


Texture contents can be passed in using the ``data`` parameter during
creation or by using the ``write()`` method. The object passed in
``data`` can be bytes or any object supporting the buffer protocol.

When writing data to texture the data type can be derived from
the internal format in the tables below. ``f1`` textures takes
unsigned bytes (``u1`` or ``numpy.uint8`` in numpy) while
``f2`` textures takes 16 bit floats (``f2`` or ``numpy.float16`` in numpy).


Float Textures
--------------

``f1`` textures are just unsigned bytes (8 bits per component) (``GL_UNSIGNED_BYTE``)

The ``f1`` texture is the most commonly used textures in OpenGL
and is currently the default. Each component takes 1 byte (4 bytes for RGBA).
This is not really a "real" float format, but a shader will read
normalized values from these textures. ``0-255`` (byte range) is read
as a value from ``0.0`` to ``1.0`` in shaders.

In shaders the sampler type should be ``sampler2D``, ``sampler2DArray``
``sampler3D``, ``samplerCube`` etc.

+----------+---------------+---------------+-------------------+
| **dtype**|  *Components* | *Base Format* | *Internal Format* |
+==========+===============+===============+===================+
| f1       |  1            | GL_RED        | GL_R8             |
+----------+---------------+---------------+-------------------+
| f1       |  2            | GL_RG         | GL_RG8            |
+----------+---------------+---------------+-------------------+
| f1       |  3            | GL_RGB        | GL_RGB8           |
+----------+---------------+---------------+-------------------+
| f1       |  4            | GL_RGBA       | GL_RGBA8          |
+----------+---------------+---------------+-------------------+

``f2`` textures stores 16 bit float values (``GL_HALF_FLOAT``).

+----------+---------------+---------------+-------------------+
| **dtype**|  *Components* | *Base Format* | *Internal Format* |
+==========+===============+===============+===================+
| f2       |  1            | GL_RED        | GL_R16F           |
+----------+---------------+---------------+-------------------+
| f2       |  2            | GL_RG         | GL_RG16F          |
+----------+---------------+---------------+-------------------+
| f2       |  3            | GL_RGB        | GL_RGB16F         |
+----------+---------------+---------------+-------------------+
| f2       |  4            | GL_RGBA       | GL_RGBA16F        |
+----------+---------------+---------------+-------------------+

``f4`` textures store 32 bit float values. (``GL_FLOAT``)
Note that some drivers do not like 3 components because of alignment.

+----------+---------------+---------------+-------------------+
| **dtype**|  *Components* | *Base Format* | *Internal Format* |
+==========+===============+===============+===================+
| f4       |  1            | GL_RED        | GL_R32F           |
+----------+---------------+---------------+-------------------+
| f4       |  2            | GL_RG         | GL_RG32F          |
+----------+---------------+---------------+-------------------+
| f4       |  3            | GL_RGB        | GL_RGB32F         |
+----------+---------------+---------------+-------------------+
| f4       |  4            | GL_RGBA       | GL_RGBA32F        |
+----------+---------------+---------------+-------------------+

Integer Textures
----------------

Integer textures come in a signed and unsigned version. The advantage
with integer textures is that shader can read the raw integer values
from them using for example ``usampler*`` (unsigned) or ``isampler*``
(signed).

Integer textures do not support ``LINEAR`` filtering (only ``NEAREST``).

Unsigned
~~~~~~~~

``u1`` textures store unsigned byte values (``GL_UNSIGNED_BYTE``).

In shaders the sampler type should be ``usampler2D``, ``usampler2DArray``
``usampler3D``, ``usamplerCube`` etc.

+----------+---------------+-----------------+-------------------+
| **dtype**|  *Components* | *Base Format*   | *Internal Format* |
+==========+===============+=================+===================+
| u1       |  1            | GL_RED_INTEGER  | GL_R8UI           |
+----------+---------------+-----------------+-------------------+
| u1       |  2            | GL_RG_INTEGER   | GL_RG8UI          |
+----------+---------------+-----------------+-------------------+
| u1       |  3            | GL_RGB_INTEGER  | GL_RGB8UI         |
+----------+---------------+-----------------+-------------------+
| u1       |  4            | GL_RGBA_INTEGER | GL_RGBA8UI        |
+----------+---------------+-----------------+-------------------+

``u2`` textures store 16 bit unsigned integers (``GL_UNSIGNED_SHORT``).

+----------+---------------+-----------------+-------------------+
| **dtype**|  *Components* | *Base Format*   | *Internal Format* |
+==========+===============+=================+===================+
| u2       |  1            | GL_RED_INTEGER  | GL_R16UI          |
+----------+---------------+-----------------+-------------------+
| u2       |  2            | GL_RG_INTEGER   | GL_RG16UI         |
+----------+---------------+-----------------+-------------------+
| u2       |  3            | GL_RGB_INTEGER  | GL_RGB16UI        |
+----------+---------------+-----------------+-------------------+
| u2       |  4            | GL_RGBA_INTEGER | GL_RGBA16UI       |
+----------+---------------+-----------------+-------------------+

``u4`` textures store 32 bit unsigned integers (``GL_UNSIGNED_INT``)

+----------+---------------+-----------------+-------------------+
| **dtype**|  *Components* | *Base Format*   | *Internal Format* |
+==========+===============+=================+===================+
| u4       |  1            | GL_RED_INTEGER  | GL_R32UI          |
+----------+---------------+-----------------+-------------------+
| u4       |  2            | GL_RG_INTEGER   | GL_RG32UI         |
+----------+---------------+-----------------+-------------------+
| u4       |  3            | GL_RGB_INTEGER  | GL_RGB32UI        |
+----------+---------------+-----------------+-------------------+
| u4       |  4            | GL_RGBA_INTEGER | GL_RGBA32UI       |
+----------+---------------+-----------------+-------------------+

Signed
~~~~~~

``i1`` textures store signed byte values (``GL_BYTE``).

In shaders the sampler type should be ``isampler2D``, ``isampler2DArray``
``isampler3D``, ``isamplerCube`` etc.

+----------+---------------+-----------------+-------------------+
| **dtype**|  *Components* | *Base Format*   | *Internal Format* |
+==========+===============+=================+===================+
| i1       |  1            | GL_RED_INTEGER  | GL_R8I            |
+----------+---------------+-----------------+-------------------+
| i1       |  2            | GL_RG_INTEGER   | GL_RG8I           |
+----------+---------------+-----------------+-------------------+
| i1       |  3            | GL_RGB_INTEGER  | GL_RGB8I          |
+----------+---------------+-----------------+-------------------+
| i1       |  4            | GL_RGBA_INTEGER | GL_RGBA8I         |
+----------+---------------+-----------------+-------------------+

``i2`` textures store 16 bit integers (``GL_SHORT``).

+----------+---------------+-----------------+-------------------+
| **dtype**|  *Components* | *Base Format*   | *Internal Format* |
+==========+===============+=================+===================+
| i2       |  1            | GL_RED_INTEGER  | GL_R16I           |
+----------+---------------+-----------------+-------------------+
| i2       |  2            | GL_RG_INTEGER   | GL_RG16I          |
+----------+---------------+-----------------+-------------------+
| i2       |  3            | GL_RGB_INTEGER  | GL_RGB16I         |
+----------+---------------+-----------------+-------------------+
| i2       |  4            | GL_RGBA_INTEGER | GL_RGBA16I        |
+----------+---------------+-----------------+-------------------+

``i4`` textures store 32 bit integers (``GL_INT``)

+----------+---------------+-----------------+-------------------+
| **dtype**|  *Components* | *Base Format*   | *Internal Format* |
+==========+===============+=================+===================+
| i4       |  1            | GL_RED_INTEGER  | GL_R32I           |
+----------+---------------+-----------------+-------------------+
| i4       |  2            | GL_RG_INTEGER   | GL_RG32I          |
+----------+---------------+-----------------+-------------------+
| i4       |  3            | GL_RGB_INTEGER  | GL_RGB32I         |
+----------+---------------+-----------------+-------------------+
| i4       |  4            | GL_RGBA_INTEGER | GL_RGBA32I        |
+----------+---------------+-----------------+-------------------+

Normalized Integer Textures
---------------------------

Normalized integers are integer texture, but texel reads in a shader
returns normalized values (``[0.0, 1.0]``). For example an unsigned 16
bit fragment with the value ``2**16-1`` will be read as ``1.0``.

Normalized integer textures should use the `sampler2D` sampler
type. Also note that there's no standard for normalized 32 bit
integer textures because a float32 doesn't have enough precision
to express a 32 bit integer as a number between 0.0 and 1.0.

Unsigned
~~~~~~~~

``nu1`` textures is really the same as an ``f1``. Each component
is a ``GL_UNSIGNED_BYTE``, but are read by the shader in normalized
form ``[0.0, 1.0]``.

+----------+---------------+-----------------+-------------------+
| **dtype**|  *Components* | *Base Format*   | *Internal Format* |
+==========+===============+=================+===================+
| nu1      |  1            | GL_RED          | GL_R8             |
+----------+---------------+-----------------+-------------------+
| nu1      |  2            | GL_RG           | GL_RG8            |
+----------+---------------+-----------------+-------------------+
| nu1      |  3            | GL_RGB          | GL_RGB8           |
+----------+---------------+-----------------+-------------------+
| nu1      |  4            | GL_RGBA         | GL_RGBA8          |
+----------+---------------+-----------------+-------------------+

``nu2`` textures store 16 bit unsigned integers (``GL_UNSIGNED_SHORT``).
The value range ``[0, 2**16-1]`` will be normalized into ``[0.0, 1.0]``.

+----------+---------------+-----------------+-------------------+
| **dtype**|  *Components* | *Base Format*   | *Internal Format* |
+==========+===============+=================+===================+
| nu2      |  1            | GL_RED          | GL_R16            |
+----------+---------------+-----------------+-------------------+
| nu2      |  2            | GL_RG           | GL_RG16           |
+----------+---------------+-----------------+-------------------+
| nu2      |  3            | GL_RGB          | GL_RGB16          |
+----------+---------------+-----------------+-------------------+
| nu2      |  4            | GL_RGBA         | GL_RGBA16         |
+----------+---------------+-----------------+-------------------+

Signed
~~~~~~

``ni1`` textures store 8 bit signed integers (``GL_BYTE``).
The value range ``[0, 127]`` will be normalized into ``[0.0, 1.0]``.
Negative values will be clamped.

+----------+---------------+-----------------+-------------------+
| **dtype**|  *Components* | *Base Format*   | *Internal Format* |
+==========+===============+=================+===================+
| ni1      |  1            | GL_RED          | GL_R8             |
+----------+---------------+-----------------+-------------------+
| ni1      |  2            | GL_RG           | GL_RG8            |
+----------+---------------+-----------------+-------------------+
| ni1      |  3            | GL_RGB          | GL_RGB8           |
+----------+---------------+-----------------+-------------------+
| ni1      |  4            | GL_RGBA         | GL_RGBA8          |
+----------+---------------+-----------------+-------------------+

``ni2`` textures store 16 bit signed integers (``GL_SHORT``).
The value range ``[0, 2**15-1]`` will be normalized into ``[0.0, 1.0]``.
Negative values will be clamped.

+----------+---------------+-----------------+-------------------+
| **dtype**|  *Components* | *Base Format*   | *Internal Format* |
+==========+===============+=================+===================+
| ni2      |  1            | GL_RED          | GL_R16            |
+----------+---------------+-----------------+-------------------+
| ni2      |  2            | GL_RG           | GL_RG16           |
+----------+---------------+-----------------+-------------------+
| ni2      |  3            | GL_RGB          | GL_RGB16          |
+----------+---------------+-----------------+-------------------+
| ni2      |  4            | GL_RGBA         | GL_RGBA16         |
+----------+---------------+-----------------+-------------------+

Overriding internalformat
-------------------------

:py:meth:`Context.texture` supports overriding the internalformat
of the texture. This is only necessary when needing a different
internal formats from the tables above. This can for
example be ``GL_SRGB8 = 0x8C41`` or some compressed format.
You may also need to look up in :py:attr:`Context.extensions`
to ensure the context supports internalformat you are using.
We do not provide the enum values for these alternative internalformats.
They can be looked up in the registry : https://raw.githubusercontent.com/KhronosGroup/OpenGL-Registry/master/xml/gl.xml

Example::

    texture = ctx.texture(image.size, 3, data=srbg_data, internal_format=GL_SRGB8)
