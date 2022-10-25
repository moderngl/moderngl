from typing import Any, Generator, Union

from moderngl.mgl import InvalidObject  # type: ignore

from .program_members import (
    Attribute,
    Subroutine,
    Uniform,
    UniformBlock,
    Varying,
)

__all__ = ['ComputeShader']


class ComputeShader:
    """
    A Compute Shader is a Shader Stage that is used entirely for computing \
    arbitrary information.

    While it can do rendering, it is generally used
    for tasks not directly related to drawing.

    - Compute shaders support uniforms are other member object just like a
        :py:class:`moderngl.Program`.
    - Storage buffers can be bound using :py:meth:`Buffer.bind_to_storage_buffer`.
    - Uniform buffers can be bound using :py:meth:`Buffer.bind_to_uniform_block`.
    - Images can be bound using :py:meth:`Texture.bind_to_image`.
    """

    __slots__ = ['mglo', '_members', '_glo', 'ctx', 'extra']

    def __init__(self):
        self.mglo = None  #: Internal representation for debug purposes only.
        self._members = {}
        self._glo = None
        self.ctx = None  #: The context this object belongs to
        self.extra = None  #: Any - Attribute for storing user defined objects
        raise TypeError()

    def __repr__(self) -> str:
        if hasattr(self, '_glo'):
            return f"<{self.__class__.__name__}: {self._glo}>"
        else:
            return f"<{self.__class__.__name__}: INCOMPLETE>"

    def __eq__(self, other: Any):
        """Compares to compute shaders ensuring the internal opengl name/id is the same."""
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

    def __getitem__(self, key: str) -> Union[Uniform, UniformBlock, Subroutine, Attribute, Varying]:
        """
        Get a member such as uniforms, uniform blocks, subroutines, \
        attributes and varyings by name.

        .. code-block:: python

            # Get a uniform
            uniform = program['color']

            # Uniform values can be set on the returned object
            # or the `__setitem__` shortcut can be used.
            program['color'].value = 1.0, 1.0, 1.0, 1.0

            # Still when writing byte data we need to use the `write()` method
            program['color'].write(buffer)
        """
        return self._members[key]

    def __setitem__(self, key: str, value: Any):
        """
        Set a value of uniform or uniform block.

        .. code-block:: python

            # Set a vec4 uniform
            uniform['color'] = 1.0, 1.0, 1.0, 1.0

            # Optionally we can store references to a member and set the value directly
            uniform = program['color']
            uniform.value = 1.0, 0.0, 0.0, 0.0

            uniform = program['cameraMatrix']
            uniform.write(camera_matrix)
        """
        self._members[key].value = value

    def __iter__(self) -> Generator[str, None, None]:
        """
        Yields the internal members names as strings.

        This includes all members such as uniforms, attributes etc.
        """
        yield from self._members

    # @property
    # def source(self) -> str:
    #     '''
    #         str: The source code of the compute shader.
    #     '''

    #     return self.mglo.source

    @property
    def glo(self) -> int:
        """
        int: The internal OpenGL object.

        This values is provided for debug purposes only.
        """
        return self._glo

    def run(self, group_x: int = 1, group_y: int = 1, group_z: int = 1) -> None:
        """
        Run the compute shader.

        Args:
            group_x (int): The number of work groups to be launched in the X dimension.
            group_y (int): The number of work groups to be launched in the Y dimension.
            group_z (int): The number of work groups to be launched in the Z dimension.
        """
        return self.mglo.run(group_x, group_y, group_z)

    def get(self, key: str, default: Any) -> Union[Uniform, UniformBlock, Subroutine, Attribute, Varying]:
        """
        Returns a Uniform, UniformBlock, Subroutine, Attribute or Varying.

        Args:
            default: This is the value to be returned in case key does not exist.

        Returns:
            :py:class:`Uniform`, :py:class:`UniformBlock`, :py:class:`Subroutine`,
            :py:class:`Attribute` or :py:class:`Varying`
        """
        return self._members.get(key, default)

    def release(self) -> None:
        """Release the ModernGL object."""
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
