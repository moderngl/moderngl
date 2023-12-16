.. py:currentmodule:: moderngl

ModernGL Types
==============

Before throwing you into doing shaders we'll go through some of the
most important types/objects in ModernGL.

* :py:class:`Buffer` is an OpenGL buffer we can for example write
  vertex data into. This data will reside in graphics memory.
* :py:class:`Program` is a shader program. We can feed it GLSL
  source code as strings to set up our shader program
* :py:class:`VertexArray` is a light object responsible for
  communication between :py:class:`Buffer` and :py:class:`Program`
  so it can understand how to access the provided buffers
  and do the rendering call.
  These objects are currently immutable but are cheap to make.
* :py:class:`Texture`, :py:class:`TextureArray`, :py:class:`Texture3D`
  and :py:class:`TextureCube` represents the different texture types.
  :py:class:`Texture` is a 2d texture and is most commonly used.
* :py:class:`Framebuffer` is an offscreen render target. It supports
  different attachments types such as a :py:class:`Texture`
  and a depth texture/buffer.

All of the objects above can only be created from a :py:class:`Context` object:

* :py:meth:`Context.buffer`
* :py:meth:`Context.program`
* :py:meth:`Context.vertex_array`
* :py:meth:`Context.texture`
* :py:meth:`Context.texture_array`
* :py:meth:`Context.texture3d`
* :py:meth:`Context.texture_cube`
* :py:meth:`Context.framebuffer`

The ModernGL types cannot be extended as in; you cannot subclass them.
Extending them must be done through substitution and not inheritance.
This is related to performance. Most objects have an ``extra``
property that can contain any python object.
