from typing import Any, Tuple

__all__ = ['Query']


class Query:
    """This class represents a Query object."""

    __slots__ = ['mglo', 'crender', 'ctx', 'extra']

    def __init__(self):
        self.mglo = None  #: Internal representation for debug purposes only.
        self.crender = None  #: ConditionalRender: Can be used in a ``with`` statement.
        self.ctx = None  #: The context this object belongs to
        self.extra = None  #: Any - Attribute for storing user defined objects
        raise TypeError()

    def __repr__(self) -> str:
        return '<Query>'

    def __hash__(self) -> int:
        return id(self)

    def __enter__(self):
        self.mglo.begin()
        return self

    def __exit__(self, *args: Tuple[Any]):
        self.mglo.end()

    @property
    def samples(self) -> int:
        """int: The number of samples passed."""
        return self.mglo.samples

    @property
    def primitives(self) -> int:
        """int: The number of primitives generated."""
        return self.mglo.primitives

    @property
    def elapsed(self) -> int:
        """int: The time elapsed in nanoseconds."""
        return self.mglo.elapsed
