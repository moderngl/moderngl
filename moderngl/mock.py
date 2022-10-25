# flake8: noqa
# type: ignore
"""Mock linters."""

# pylint: skip-file

__all__ = ['mgl']


class Implementation:
    class Error(Exception):
        """ModernGL Error."""
        filename = None
        function = None
        line = None

    def strsize(self, *args) -> int:
        """strsize."""
        return 0

    def create_context(self, *args) -> 'Context':
        """create_context."""
        return (None, 0)

    def create_standalone_context(self, *args) -> 'Context':
        """create_standalone_context."""

        return (None, 0)


mgl = Implementation()
