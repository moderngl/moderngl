from typing import Tuple

from moderngl.mgl import InvalidObject  # type: ignore

__all__ = ['Sampler']


class Sampler:
    """
    A Sampler Object is an OpenGL Object that stores the sampling parameters for a Texture access inside of a shader.

    When a sampler object is bound to a texture image unit,
    the internal sampling parameters for a texture bound to the same image unit are all ignored.
    Instead, the sampling parameters are taken from this sampler object.

    Unlike textures, a samplers state can also be changed freely be at any time
    without the sampler object being bound/in use.

    Samplers are bound to a texture unit and not a texture itself. Be careful with leaving
    samplers bound to texture units as it can cause texture incompleteness issues
    (the texture bind is ignored).

    Sampler bindings do clear automatically between every frame so a texture unit
    need at least one bind/use per frame.
    """

    __slots__ = ['mglo', '_glo', 'ctx', 'extra', 'texture']

    def __init__(self):
        self.mglo = None  #: Internal representation for debug purposes only.
        self._glo = None
        self.ctx = None  #: The context this object belongs to
        self.extra = None  #: Any - Attribute for storing user defined objects
        self.texture = None
        raise TypeError()

    def __hash__(self) -> int:
        return id(self)

    def __repr__(self):
        if hasattr(self, '_glo'):
            return f"<{self.__class__.__name__}: {self._glo}>"
        else:
            return f"<{self.__class__.__name__}: INCOMPLETE>"

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    def use(self, location: int = 0) -> None:
        """
        Bind the sampler to a texture unit.

        Args:
            location (int): The texture unit
        """
        if self.texture is not None:
            self.texture.use(location)
        self.mglo.use(location)

    def clear(self, location: int = 0) -> None:
        """
        Clear the sampler binding on a texture unit.

        Args:
            location (int): The texture unit
        """
        self.mglo.clear(location)

    def release(self) -> None:
        """Release/destroy the ModernGL object."""
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()

    @property
    def repeat_x(self) -> bool:
        """
        bool: The x repeat flag for the sampler (Default ``True``).

        Example::

            # Enable texture repeat (GL_REPEAT)
            sampler.repeat_x = True

            # Disable texture repeat (GL_CLAMP_TO_EDGE)
            sampler.repeat_x = False
        """
        return self.mglo.repeat_x

    @repeat_x.setter
    def repeat_x(self, value: bool) -> None:
        self.mglo.repeat_x = value

    @property
    def repeat_y(self) -> bool:
        """
        bool: The y repeat flag for the sampler (Default ``True``).

        Example::

            # Enable texture repeat (GL_REPEAT)
            sampler.repeat_y = True

            # Disable texture repeat (GL_CLAMP_TO_EDGE)
            sampler.repeat_y = False
        """
        return self.mglo.repeat_y

    @repeat_y.setter
    def repeat_y(self, value: bool) -> None:
        self.mglo.repeat_y = value

    @property
    def repeat_z(self) -> bool:
        """
        bool: The z repeat flag for the sampler (Default ``True``).

        Example::

            # Enable texture repeat (GL_REPEAT)
            sampler.repeat_z = True

            # Disable texture repeat (GL_CLAMP_TO_EDGE)
            sampler.repeat_z = False
        """
        return self.mglo.repeat_z

    @repeat_z.setter
    def repeat_z(self, value: bool) -> None:
        self.mglo.repeat_z = value

    @property
    def filter(self) -> Tuple[int, int]:
        """
        tuple: The minification and magnification filter for the sampler.

        (Default ``(moderngl.LINEAR. moderngl.LINEAR)``)

        Example::

            sampler.filter == (moderngl.NEAREST, moderngl.NEAREST)
            sampler.filter == (moderngl.LINEAR_MIPMAP_LINEAR, moderngl.LINEAR)
            sampler.filter == (moderngl.NEAREST_MIPMAP_LINEAR, moderngl.NEAREST)
            sampler.filter == (moderngl.LINEAR_MIPMAP_NEAREST, moderngl.NEAREST)
        """
        return self.mglo.filter

    @filter.setter
    def filter(self, value: Tuple[int, int]) -> None:
        self.mglo.filter = value

    @property
    def compare_func(self) -> str:
        """
        tuple: The compare function for a depth textures (Default ``'?'``).

        By default samplers don't have depth comparison mode enabled.
        This means that depth texture values can be read as a ``sampler2D``
        using ``texture()`` in a GLSL shader by default.

        When setting this property to a valid compare mode, ``GL_TEXTURE_COMPARE_MODE``
        is set to ``GL_COMPARE_REF_TO_TEXTURE`` so that texture lookup
        functions in GLSL will return a depth comparison result instead
        of the actual depth value.

        Accepted compare functions::

            .compare_func = ''    # Disale depth comparison completely
            sampler.compare_func = '<='  # GL_LEQUAL
            sampler.compare_func = '<'   # GL_LESS
            sampler.compare_func = '>='  # GL_GEQUAL
            sampler.compare_func = '>'   # GL_GREATER
            sampler.compare_func = '=='  # GL_EQUAL
            sampler.compare_func = '!='  # GL_NOTEQUAL
            sampler.compare_func = '0'   # GL_NEVER
            sampler.compare_func = '1'   # GL_ALWAYS
        """
        return self.mglo.compare_func

    @compare_func.setter
    def compare_func(self, value: str) -> None:
        self.mglo.compare_func = value

    @property
    def anisotropy(self) -> float:
        """
        float: Number of samples for anisotropic filtering (Default ``1.0``).

        The value will be clamped in range ``1.0`` and ``ctx.max_anisotropy``.

        Any value greater than 1.0 counts as a use of anisotropic filtering::

            # Disable anisotropic filtering
            sampler.anisotropy = 1.0

            # Enable anisotropic filtering suggesting 16 samples as a maximum
            sampler.anisotropy = 16.0
        """
        return self.mglo.anisotropy

    @anisotropy.setter
    def anisotropy(self, value: float) -> None:
        self.mglo.anisotropy = value

    @property
    def border_color(self) -> Tuple[float, float, float, float]:
        """
        tuple: The (r, g, b, a) color for the texture border (Default ``(0.0, 0.0, 0.0, 0.0)``).

        When setting this value the ``repeat_`` values are overridden setting the texture wrap to return
        the border color when outside [0, 1] range.

        Example::

            # Red border color
            sampler.border_color = (1.0, 0.0, 0.0, 0.0)
        """
        return self.mglo.border_color

    @border_color.setter
    def border_color(self, value: Tuple[float, float, float, float]) -> None:
        self.mglo.border_color = value

    @property
    def min_lod(self) -> float:
        """
        float: Minimum level-of-detail parameter (Default ``-1000.0``).

        This floating-point value limits the selection of highest resolution mipmap (lowest mipmap level)
        """
        return self.mglo.min_lod

    @min_lod.setter
    def min_lod(self, value: float) -> None:
        self.mglo.min_lod = value

    @property
    def max_lod(self) -> float:
        """
        float: Minimum level-of-detail parameter (Default ``1000.0``).

        This floating-point value limits the selection of the lowest resolution mipmap (highest mipmap level)
        """
        return self.mglo.max_lod

    @max_lod.setter
    def max_lod(self, value: float) -> None:
        self.mglo.max_lod = value

    def assign(self, index: int) -> Tuple["Sampler", int]:
        """
        Helper method for assigning samplers to scopes.

        Example::

            s1 = ctx.sampler(...)
            s2 = ctx.sampler(...)
            ctx.scope(samplers=(s1.assign(0), s1.assign(1)), ...)

        Returns:
            (self, index) tuple
        """
        return (self, index)
