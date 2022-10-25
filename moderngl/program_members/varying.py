__all__ = ['Varying']


class Varying:
    """This class represents a program varying."""

    __slots__ = ['_number', '_array_length', '_dimension', '_name', 'extra']

    def __init__(self):
        self._number = None
        self._array_length = None
        self._dimension = None
        self._name = None
        self.extra = None  #: Any - Attribute for storing user defined objects
        raise TypeError()

    def __repr__(self) -> str:
        return '<Varying: %d>' % self.number

    def __hash__(self) -> int:
        return id(self)

    @property
    def number(self) -> int:
        """int: The number of the varying."""
        return self._number

    @property
    def name(self) -> str:
        """str: The name of the varying."""
        return self._name
