from typing import Any

from moderngl.mgl import InvalidObject  # type: ignore

__all__ = ['Renderbuffer']


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

    __slots__ = ['mglo', '_size', '_components', '_samples', '_depth', '_dtype', '_glo', 'ctx', 'extra']

    def __init__(self):
        self.mglo = None  #: Internal representation for debug purposes only.
        self._size = (None, None)
        self._components = None
        self._samples = None
        self._depth = None
        self._dtype = None
        self._glo = None
        self.ctx = None  #: The context this object belongs to
        self.extra = None  #: Any - Attribute for storing user defined objects
        raise TypeError()

    def __repr__(self):
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
    def width(self) -> int:
        """int: The width of the renderbuffer."""
        return self._size[0]

    @property
    def height(self) -> int:
        """int: The height of the renderbuffer."""
        return self._size[1]

    @property
    def size(self) -> tuple:
        """tuple: The size of the renderbuffer."""
        return self._size

    @property
    def samples(self) -> int:
        """int: The samples of the renderbuffer."""
        return self._samples

    @property
    def components(self) -> int:
        """int: The components of the renderbuffer."""
        return self._components

    @property
    def depth(self) -> bool:
        """bool: Is the renderbuffer a depth renderbuffer?."""
        return self._depth

    @property
    def dtype(self) -> str:
        """str: Data type."""
        return self._dtype

    @property
    def glo(self) -> int:
        """
        int: The internal OpenGL object.

        This values is provided for debug purposes only.
        """
        return self._glo

    def release(self) -> None:
        """Release the ModernGL object."""
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
