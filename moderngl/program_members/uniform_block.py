__all__ = ['UniformBlock']


class UniformBlock:
    """Uniform Block metadata."""

    __slots__ = ['mglo', '_index', '_size', '_name', 'extra']

    def __init__(self):
        self.mglo = None  #: Internal representation for debug purposes only.
        self._index = None
        self._size = None
        self._name = None
        self.extra = None  #: Any - Attribute for storing user defined objects
        raise TypeError()

    def __repr__(self):
        return '<UniformBlock: %d>' % self._index

    def __hash__(self) -> int:
        return id(self)

    @property
    def binding(self) -> int:
        """int: The binding of the uniform block."""
        return self.mglo.binding

    @binding.setter
    def binding(self, binding: int) -> None:
        self.mglo.binding = binding

    @property
    def value(self) -> int:
        """int: The value of the uniform block."""
        return self.mglo.value

    @value.setter
    def value(self, value: int) -> None:
        self.mglo.binding = value

    @property
    def name(self) -> str:
        """str: The name of the uniform block."""
        return self._name

    @property
    def index(self) -> int:
        """int: The index of the uniform block."""
        return self._index

    @property
    def size(self) -> int:
        """int: The size of the uniform block."""
        return self._size
