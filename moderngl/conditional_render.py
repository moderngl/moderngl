__all__ = ['ConditionalRender']


class ConditionalRender:
    """
    This class represents a ConditionalRender object.

    ConditionalRender objects can only be accessed from :py:class:`Query` objects.
    """

    __slots__ = ['mglo']

    def __init__(self):
        self.mglo = None  #: Internal representation for debug purposes only.
        raise TypeError()

    def __repr__(self):
        return '<ConditionalRender>'

    def __enter__(self):
        self.mglo.begin_render()
        return self

    def __exit__(self, *args):
        self.mglo.end_render()
