__all__ = ['Subroutine']


class Subroutine:
    """This class represents a program subroutine."""

    __slots__ = ['_index', '_name', 'extra']

    def __init__(self):
        self._index = None
        self._name = None
        self.extra = None  #: Any - Attribute for storing user defined objects
        raise TypeError()

    def __repr__(self) -> str:
        return '<Subroutine: %d>' % self._index

    def __hash__(self) -> int:
        return id(self)

    @property
    def index(self) -> int:
        """int: The index of the subroutine."""
        return self._index

    @property
    def name(self) -> str:
        """str: The name of the subroutine."""
        return self._name
