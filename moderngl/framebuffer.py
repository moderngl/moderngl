from typing import Any, Dict, Optional, TYPE_CHECKING, Tuple, Union

from moderngl.mgl import InvalidObject  # type: ignore

from .buffer import Buffer
from .renderbuffer import Renderbuffer
from .texture import Texture

if TYPE_CHECKING:
    from .context import Context

__all__ = ['Framebuffer']


class Framebuffer:
    """
    A :py:class:`Framebuffer` is a collection of buffers that can be used as the destination for rendering.

    The buffers for Framebuffer objects reference images from either Textures or Renderbuffers.
    Create a :py:class:`Framebuffer` using :py:meth:`Context.framebuffer`.
    """

    __slots__ = [
        'mglo', '_color_attachments', '_depth_attachment', '_size', '_samples', '_glo',
        'ctx', '_is_reference', 'extra'
    ]

    def __init__(self):
        self.mglo = None  #: Internal representation for debug purposes only.
        self._color_attachments = None
        self._depth_attachment = None
        self._size = (None, None)
        self._samples: int = None
        self._glo: int = None
        self.ctx: Context = None  #: The context this object belongs to
        self._is_reference = None  #: Detected framebuffers we should not delete
        self.extra: Any = None  #: Attribute for storing user defined objects
        raise TypeError()

    def __repr__(self) -> str:
        if hasattr(self, '_glo'):
            return '<Framebuffer: %d>' % self._glo
        else:
            return "<Framebuffer: INCOMPLETE>"

    def __eq__(self, other: Any) -> bool:
        return type(self) is type(other) and self.mglo is other.mglo

    def __hash__(self) -> int:
        return id(self)

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        # Don't delete default framebuffer or a reference
        if self._is_reference:
            return

        # If object was initialized properly (ctx present) and gc_mode is auto
        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    @property
    def viewport(self) -> Tuple[int, int, int, int]:
        """tuple: Get or set the viewport of the framebuffer."""
        return self.mglo.viewport

    @viewport.setter
    def viewport(self, value: Tuple[int, int, int, int]) -> None:
        self.mglo.viewport = tuple(value)

    @property
    def scissor(self) -> Tuple[int, int, int, int]:
        """
        tuple: Get or set the scissor box of the framebuffer.

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
        return self.mglo.scissor

    @scissor.setter
    def scissor(self, value: Tuple[int, int, int, int]) -> None:
        if value is None:
            self.mglo.scissor = None
        else:
            self.mglo.scissor = tuple(value)

    @property
    def color_mask(self) -> Tuple[bool, bool, bool, bool]:
        """
        tuple: The color mask of the framebuffer.

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
        return self.mglo.color_mask

    @color_mask.setter
    def color_mask(self, value: Tuple[bool, bool, bool, bool]) -> None:
        self.mglo.color_mask = value

    @property
    def depth_mask(self) -> bool:
        """
        bool: The depth mask of the framebuffer.

        Depth mask enables or disables write operations to the depth buffer.
        This also applies when clearing the framebuffer.
        If depth testing is enabled fragments will still be culled, but
        the depth buffer will not be updated with new values. This is
        a very useful tool in many rendering techniques.

        Default value: ``True``
        """
        return self.mglo.depth_mask

    @depth_mask.setter
    def depth_mask(self, value: bool) -> None:
        self.mglo.depth_mask = value

    @property
    def width(self) -> int:
        """
        int: The width of the framebuffer.

        Framebuffers created by a window will only report its initial size.
        It's better get size information from the window itself.
        """
        return self._size[0]

    @property
    def height(self) -> int:
        """
        int: The height of the framebuffer.

        Framebuffers created by a window will only report its initial size.
        It's better get size information from the window itself.
        """
        return self._size[1]

    @property
    def size(self) -> tuple:
        """
        tuple: The size of the framebuffer.

        Framebuffers created by a window will only report its initial size.
        It's better get size information from the window itself.
        """
        return self._size

    @property
    def samples(self) -> int:
        """int: The samples of the framebuffer."""
        return self._samples

    @property
    def bits(self) -> Dict[str, str]:
        """dict: The bits of the framebuffer."""
        return self.mglo.bits

    @property
    def color_attachments(self) -> Tuple[Union[Texture, Renderbuffer], ...]:
        """tuple: The color attachments of the framebuffer."""
        return self._color_attachments

    @property
    def depth_attachment(self) -> Union[Texture, Renderbuffer]:
        """Texture or Renderbuffer: The depth attachment of the framebuffer."""
        return self._depth_attachment

    @property
    def glo(self) -> int:
        """
        int: The internal OpenGL object.

        This values is provided for debug purposes only.
        """
        return self._glo

    def clear(
        self,
        red: float = 0.0,
        green: float = 0.0,
        blue: float = 0.0,
        alpha: float = 0.0,
        depth: float = 1.0,
        *,
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
        if color is not None:
            red, green, blue, alpha, *_ = tuple(color) + (0.0, 0.0, 0.0, 0.0)

        if viewport is not None:
            viewport = tuple(viewport)

        self.mglo.clear(red, green, blue, alpha, depth, viewport)

    def use(self) -> None:
        """Bind the framebuffer. Sets the target for rendering commands."""
        self.ctx.fbo = self
        self.mglo.use()

    def read(
        self,
        viewport: Optional[Union[Tuple[int, int], Tuple[int, int, int, int]]] = None,
        components: int = 3,
        *,
        attachment: int = 0,
        alignment: int = 1,
        dtype: str = 'f1',
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
        return self.mglo.read(viewport, components, attachment, alignment, clamp, dtype)

    def read_into(
        self,
        buffer: Any,
        viewport: Optional[Union[Tuple[int, int], Tuple[int, int, int, int]]] = None,
        components: int = 3,
        *,
        attachment: int = 0,
        alignment: int = 1,
        dtype: str = 'f1',
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
        if type(buffer) is Buffer:
            buffer = buffer.mglo

        return self.mglo.read_into(buffer, viewport, components, attachment, alignment, dtype, write_offset)

    def release(self) -> None:
        """Release the ModernGL object."""
        if not isinstance(self.mglo, InvalidObject):
            self._color_attachments = None
            self._depth_attachment = None
            self.mglo.release()
