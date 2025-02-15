Framebuffer
===========

.. py:class:: Framebuffer

    Returned by :py:meth:`Context.framebuffer`

    A :py:class:`Framebuffer` is a collection of buffers that can be used as the destination for rendering.

    The buffers for Framebuffer objects reference images from either Textures or Renderbuffers.

Methods
-------

.. py:method:: Framebuffer.clear(red: float = 0.0, green: float = 0.0, blue: float = 0.0, alpha: float = 0.0, depth: float = 1.0, viewport=..., color=...) -> None

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

    :param float red: color component.
    :param float green: color component.
    :param float blue: color component.
    :param float alpha: alpha component.
    :param float depth: depth value.
    :param tuple viewport: The viewport.
    :param tuple color: Optional tuple replacing the red, green, blue and alpha arguments

.. py:method:: Framebuffer.read(viewport=..., components: int = 3, attachment: int = 0, alignment: int = 1, dtype: str = 'f1', clamp: bool = False, gil: bool = False) -> bytes

    Read the content of the framebuffer.

    :param tuple viewport: The viewport.
    :param int components: The number of components to read.
    :param int attachment: The color attachment number. -1 for the depth attachment
    :param int alignment: The byte alignment of the pixels.
    :param str dtype: Data type.
    :param bool clamp: Clamps floating point values to ``[0.0, 1.0]``
    :param bool gil: Whether to hold the GIL while waiting for the data.

    .. code:: python

        # Read the first color attachment's RGBA data
        data = fbo.read(components=4)
        # Read the second color attachment's RGB data
        data = fbo.read(attachment=1)
        # Read the depth attachment
        data = fbo.read(attachment=-1)
        # Read the lower left 10 x 10 pixels from the first color attachment
        data = fbo.read(viewport=(0, 0, 10, 10))

.. py:method:: Framebuffer.read_into(buffer, viewport, components: int = 3, attachment: int = 0, alignment: int = 1, dtype: str = 'f1', write_offset: int = 0, gil: bool = False) -> None

    Read the content of the framebuffer into a buffer.

    :param bytearray buffer: The buffer that will receive the pixels.
    :param tuple viewport: The viewport.
    :param int components: The number of components to read.
    :param int attachment: The color attachment.
    :param int alignment: The byte alignment of the pixels.
    :param str dtype: Data type.
    :param int write_offset: The write offset.
    :param bool gil: Whether to hold the GIL while waiting for the data.

.. py:method:: Framebuffer.use()

    Bind the framebuffer.

.. py:method:: Framebuffer.release

Attributes
----------

.. py:attribute:: Framebuffer.viewport
    :type: tuple

    Get or set the viewport of the framebuffer.

.. py:attribute:: Framebuffer.scissor
    :type: tuple

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

.. py:attribute:: Framebuffer.color_mask
    :type: tuple

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

.. py:attribute:: Framebuffer.depth_mask
    :type: bool

    The depth mask of the framebuffer.

    Depth mask enables or disables write operations to the depth buffer.
    This also applies when clearing the framebuffer.
    If depth testing is enabled fragments will still be culled, but
    the depth buffer will not be updated with new values. This is
    a very useful tool in many rendering techniques.

    Default value: ``True``

.. py:attribute:: Framebuffer.width
    :type: int

    The width of the framebuffer.

    Framebuffers created by a window will only report its initial size.
    It's better get size information from the window itself.

.. py:attribute:: Framebuffer.height
    :type: int

    The height of the framebuffer.

    Framebuffers created by a window will only report its initial size.
    It's better get size information from the window itself.

.. py:attribute:: Framebuffer.size
    :type: Tuple[int, int]

    The size of the framebuffer.

    Framebuffers created by a window will only report its initial size.
    It's better get size information from the window itself.

.. py:attribute:: Framebuffer.samples
    :type: int

    The samples of the framebuffer.

.. py:attribute:: Framebuffer.bits
    :type: int

    The bits of the framebuffer.

.. py:attribute:: Framebuffer.color_attachments
    :type: tuple

.. py:attribute:: Framebuffer.depth_attachment
    :type: tuple

.. py:attribute:: Framebuffer.ctx
    :type: Context

    The context this object belongs to

.. py:attribute:: Framebuffer.glo
    :type: int

    The internal OpenGL object.
    This values is provided for interoperability and debug purposes only.

.. py:attribute:: Framebuffer.extra
    :type: Any

    User defined data.
