import warnings
from collections import deque
from typing import Any, Deque, Dict, List, Optional, Set, Tuple, Union

from moderngl.mgl import InvalidObject  # type: ignore

from .buffer import Buffer
from .compute_shader import ComputeShader
from .conditional_render import ConditionalRender
from .framebuffer import Framebuffer
from .program import Program, detect_format
from .program_members import (
    Attribute,
    Subroutine,
    Uniform,
    UniformBlock,
    Varying,
)
from .query import Query
from .renderbuffer import Renderbuffer
from .sampler import Sampler
from .scope import Scope
from .texture import Texture
from .texture_3d import Texture3D
from .texture_array import TextureArray
from .texture_cube import TextureCube
from .vertex_array import VertexArray

try:
    from moderngl import mgl
except ImportError:
    pass


__all__ = ['Context', 'create_context', 'create_standalone_context',
           'NOTHING', 'BLEND', 'DEPTH_TEST', 'CULL_FACE', 'RASTERIZER_DISCARD', 'PROGRAM_POINT_SIZE',
           'ZERO', 'ONE', 'SRC_COLOR', 'ONE_MINUS_SRC_COLOR', 'SRC_ALPHA', 'ONE_MINUS_SRC_ALPHA',
           'DST_ALPHA', 'ONE_MINUS_DST_ALPHA', 'DST_COLOR', 'ONE_MINUS_DST_COLOR',
           'FUNC_ADD', 'FUNC_SUBTRACT', 'FUNC_REVERSE_SUBTRACT', 'MIN', 'MAX',
           'DEFAULT_BLENDING', 'ADDITIVE_BLENDING', 'PREMULTIPLIED_ALPHA',
           'FIRST_VERTEX_CONVENTION', 'LAST_VERTEX_CONVENTION']

# Context Flags

#: Represents no states. Can be used with :py:meth:`Context.enable_only` to disable all states.
NOTHING = 0
#: Enable/disable blending
BLEND = 1
#: Enable/disable depth testing
DEPTH_TEST = 2
#: Enable/disable face culling
CULL_FACE = 4
#: Enable/disable rasterization
RASTERIZER_DISCARD = 8
#: Context flag: Enables ``gl_PointSize`` in vertex or geometry shaders.
#:
#: When enabled we can write to ``gl_PointSize`` in the vertex shader to specify the point size
#: for each individual point.
#:
#: If this value is not set in the shader the behavior is undefined. This means the points may
#: or may not appear depending if the drivers enforce some default value for ``gl_PointSize``.
#:
#: When disabled :py:attr:`Context.point_size` is used.
PROGRAM_POINT_SIZE = 16

# Blend functions
#: (0,0,0,0)
ZERO = 0x0000
#: (1,1,1,1)
ONE = 0x0001
#: (Rs0/kR,Gs0/kG,Bs0/kB,As0/kA)
SRC_COLOR = 0x0300
#: (1,1,1,1) − (Rs0/kR,Gs0/kG,Bs0/kB,As0/kA)
ONE_MINUS_SRC_COLOR = 0x0301
#: (As0/kA,As0/kA,As0/kA,As0/kA)
SRC_ALPHA = 0x0302
#: (1,1,1,1) − (As0/kA,As0/kA,As0/kA,As0/kA)
ONE_MINUS_SRC_ALPHA = 0x0303
#: (Ad/kA,Ad/kA,Ad/kA,Ad/kA)
DST_ALPHA = 0x0304
#: (1,1,1,1) − (Ad/kA,Ad/kA,Ad/kA,Ad/kA)
ONE_MINUS_DST_ALPHA = 0x0305
#: (Rd/kR,Gd/kG,Bd/kB,Ad/kA)
DST_COLOR = 0x0306
#: (1,1,1,1) − (Rd/kR,Gd/kG,Bd/kB,Ad/kA)
ONE_MINUS_DST_COLOR = 0x0307

# Blend equations
#: source + destination
FUNC_ADD = 0x8006
#: source - destination
FUNC_SUBTRACT = 0x800A
#: destination - source
FUNC_REVERSE_SUBTRACT = 0x800B
#: Minimum of source and destination
MIN = 0x8007
#: Maximum of source and destination
MAX = 0x8008

#: Shotcut for the default blending ``SRC_ALPHA, ONE_MINUS_SRC_ALPHA``
DEFAULT_BLENDING = (SRC_ALPHA, ONE_MINUS_SRC_ALPHA)
#: Shotcut for additive blending ``ONE, ONE``
ADDITIVE_BLENDING = (ONE, ONE)
#: Shotcut for blend mode when using premultiplied alpha ``SRC_ALPHA, ONE``
PREMULTIPLIED_ALPHA = (SRC_ALPHA, ONE)

#: Specifies the first vertex should be used as the source of data for flat shaded varyings.
#: Used with :py:attr:`Context.provoking_vertex`.
FIRST_VERTEX_CONVENTION = 0x8E4D
#: Specifies the last vertex should be used as the source of data for flat shaded varyings.
#: Used with :py:attr:`Context.provoking_vertex`.
LAST_VERTEX_CONVENTION = 0x8E4E


class Context:
    """
    Class exposing OpenGL features.

    ModernGL objects can be created from this class.
    """

    _valid_gc_modes = [None, "context_gc", "auto"]
    # Context Flags
    #: Represents no states. Can be used with :py:meth:`Context.enable_only` to disable all states.
    NOTHING = 0
    #: Enable/disable blending
    BLEND = 1
    #: Enable/disable depth testing
    DEPTH_TEST = 2
    #: Enable/disable face culling
    CULL_FACE = 4
    #: Enable/disable rasterization
    RASTERIZER_DISCARD = 8
    #: Context flag: Enables ``gl_PointSize`` in vertex or geometry shaders.
    #:
    #: When enabled we can write to ``gl_PointSize`` in the vertex shader to specify the point size
    #: for each individual point.
    #:
    #: If this value is not set in the shader the behavior is undefined. This means the points may
    #: or may not appear depending if the drivers enforce some default value for ``gl_PointSize``.
    #:
    #: When disabled :py:attr:`Context.point_size` is used.
    PROGRAM_POINT_SIZE = 16

    # Primitive modes

    #: Each vertex represents a point
    POINTS = 0x0000
    #: Vertices 0 and 1 are considered a line. Vertices 2 and 3 are considered a line.
    #: And so on. If the user specifies a non-even number of vertices, then the extra vertex is ignored.
    LINES = 0x0001
    #: As line strips, except that the first and last vertices are also used as a line.
    #: Thus, you get n lines for n input vertices. If the user only specifies 1 vertex,
    #: the drawing command is ignored. The line between the first and last vertices happens
    #: after all of the previous lines in the sequence.
    LINE_LOOP = 0x0002
    #: The adjacent vertices are considered lines. Thus, if you pass n vertices, you will get n-1 lines.
    #: If the user only specifies 1 vertex, the drawing command is ignored.
    LINE_STRIP = 0x0003
    #: Vertices 0, 1, and 2 form a triangle. Vertices 3, 4, and 5 form a triangle. And so on.
    TRIANGLES = 0x0004
    #: Every group of 3 adjacent vertices forms a triangle. The face direction of the
    #: strip is determined by the winding of the first triangle. Each successive triangle
    #: will have its effective face order reversed, so the system compensates for that
    #: by testing it in the opposite way. A vertex stream of n length will generate n-2 triangles.
    TRIANGLE_STRIP = 0x0005
    #: The first vertex is always held fixed. From there on, every group of 2 adjacent
    #: vertices form a triangle with the first. So with a vertex stream, you get a list
    #: of triangles like so: (0, 1, 2) (0, 2, 3), (0, 3, 4), etc. A vertex stream of
    #: n length will generate n-2 triangles.
    TRIANGLE_FAN = 0x0006
    #: These are special primitives that are expected to be used specifically with
    #: geomtry shaders. These primitives give the geometry shader more vertices
    #: to work with for each input primitive. Data needs to be duplicated in buffers.
    LINES_ADJACENCY = 0x000A
    #: These are special primitives that are expected to be used specifically with
    #: geomtry shaders. These primitives give the geometry shader more vertices
    #: to work with for each input primitive. Data needs to be duplicated in buffers.
    LINE_STRIP_ADJACENCY = 0x000B
    #: These are special primitives that are expected to be used specifically with
    #: geomtry shaders. These primitives give the geometry shader more vertices
    #: to work with for each input primitive. Data needs to be duplicated in buffers.
    TRIANGLES_ADJACENCY = 0x000C
    #: These are special primitives that are expected to be used specifically with
    #: geomtry shaders. These primitives give the geometry shader more vertices
    #: to work with for each input primitive. Data needs to be duplicated in buffers.
    TRIANGLE_STRIP_ADJACENCY = 0x0000D
    #: primitive type can only be used when Tessellation is active. It is a primitive
    #: with a user-defined number of vertices, which is then tessellated based on the
    #: control and evaluation shaders into regular points, lines, or triangles, depending
    #: on the TES's settings.
    PATCHES = 0x000E

    # Texture filters
    #: Returns the value of the texture element that is nearest
    #: (in Manhattan distance) to the specified texture coordinates.
    NEAREST = 0x2600
    #: Returns the weighted average of the four texture elements
    #: that are closest to the specified texture coordinates.
    #: These can include items wrapped or repeated from other parts
    #: of a texture, depending on the values of texture repeat mode,
    #: and on the exact mapping.
    LINEAR = 0x2601
    #: Chooses the mipmap that most closely matches the size of the
    #: pixel being textured and uses the ``NEAREST`` criterion (the texture
    #: element closest to the specified texture coordinates) to produce
    #: a texture value.
    NEAREST_MIPMAP_NEAREST = 0x2700
    #: Chooses the mipmap that most closely matches the size of the pixel
    #: being textured and uses the ``LINEAR`` criterion (a weighted average
    #: of the four texture elements that are closest to the specified
    #: texture coordinates) to produce a texture value.
    LINEAR_MIPMAP_NEAREST = 0x2701
    #: Chooses the two mipmaps that most closely match the size of the
    #: pixel being textured and uses the ``NEAREST`` criterion (the texture
    #: element closest to the specified texture coordinates ) to produce
    #: a texture value from each mipmap. The final texture value is a
    #: weighted average of those two values.
    NEAREST_MIPMAP_LINEAR = 0x2702
    #: Chooses the two mipmaps that most closely match the size of the pixel
    #: being textured and uses the ``LINEAR`` criterion (a weighted average
    #: of the texture elements that are closest to the specified texture
    #: coordinates) to produce a texture value from each mipmap.
    #: The final texture value is a weighted average of those two values.
    LINEAR_MIPMAP_LINEAR = 0x2703

    # Blend functions
    #: (0,0,0,0)
    ZERO = 0x0000
    #: (1,1,1,1)
    ONE = 0x0001
    #: (Rs0/kR,Gs0/kG,Bs0/kB,As0/kA)
    SRC_COLOR = 0x0300
    #: (1,1,1,1) − (Rs0/kR,Gs0/kG,Bs0/kB,As0/kA)
    ONE_MINUS_SRC_COLOR = 0x0301
    #: (As0/kA,As0/kA,As0/kA,As0/kA)
    SRC_ALPHA = 0x0302
    #: (1,1,1,1) − (As0/kA,As0/kA,As0/kA,As0/kA)
    ONE_MINUS_SRC_ALPHA = 0x0303
    #: (Ad/kA,Ad/kA,Ad/kA,Ad/kA)
    DST_ALPHA = 0x0304
    #: (1,1,1,1) − (Ad/kA,Ad/kA,Ad/kA,Ad/kA)
    ONE_MINUS_DST_ALPHA = 0x0305
    #: (Rd/kR,Gd/kG,Bd/kB,Ad/kA)
    DST_COLOR = 0x0306
    #: (1,1,1,1) − (Rd/kR,Gd/kG,Bd/kB,Ad/kA)
    ONE_MINUS_DST_COLOR = 0x0307

    #: Shotcut for the default blending ``SRC_ALPHA, ONE_MINUS_SRC_ALPHA``
    DEFAULT_BLENDING = (SRC_ALPHA, ONE_MINUS_SRC_ALPHA)
    #: Shotcut for additive blending ``ONE, ONE``
    ADDITIVE_BLENDING = (ONE, ONE)
    #: Shotcut for blend mode when using premultiplied alpha ``SRC_ALPHA, ONE``
    PREMULTIPLIED_ALPHA = (SRC_ALPHA, ONE)

    # Blend equations
    #: source + destination
    FUNC_ADD = 0x8006
    #: source - destination
    FUNC_SUBTRACT = 0x800A
    #: destination - source
    FUNC_REVERSE_SUBTRACT = 0x800B
    #: Minimum of source and destination
    MIN = 0x8007
    #: Maximum of source and destination
    MAX = 0x8008

    # Provoking vertex

    #: Specifies the first vertex should be used as the source of data for flat shaded varyings.
    #: Used with :py:attr:`Context.provoking_vertex`.
    FIRST_VERTEX_CONVENTION = 0x8E4D
    #: Specifies the last vertex should be used as the source of data for flat shaded varyings.
    #: Used with :py:attr:`Context.provoking_vertex`.
    LAST_VERTEX_CONVENTION = 0x8E4E

    __slots__ = ['mglo', '_screen', '_info', '_extensions', 'version_code', 'fbo', '_gc_mode', '_objects', 'extra']

    def __init__(self):
        self.mglo = None  #: Internal representation for debug purposes only.
        self._screen = None
        self._info = None
        self._extensions = None
        self.version_code = None  #: int: The OpenGL version code. Reports ``410`` for OpenGL 4.1
        #: Framebuffer: The active framebuffer.
        #: Set every time :py:meth:`Framebuffer.use()` is called.
        self.fbo = None
        self.extra = None  #: Any - Attribute for storing user defined objects
        self._gc_mode = None
        self._objects: Deque[Any] = deque()
        raise TypeError()

    def __repr__(self) -> str:
        return f"<Context {id(self)} version_code={self.version_code}>"

    def __eq__(self, other: Any):
        return type(self) is type(other) and self.mglo is other.mglo

    def __hash__(self) -> int:
        return id(self)

    def __del__(self):
        if hasattr(self, "_gc_mode") and self._gc_mode == "auto":
            self.release()

    @property
    def gc_mode(self) -> Optional[str]:
        """
        Optional[str]: The garbage collection mode.

        The default mode is ``None`` meaning no automatic
        garbage collection is done. Other modes are ``auto``
        and ``context_gc``. Please see documentation for
        the appropriate configuration.

        Examples::

            # Disable automatic garbage collection.
            # Each objects needs to be explicitly released.
            ctx.gc_mode = None

            # Collect all dead objects in the context and
            # release them by calling Context.gc()
            ctx.gc_mode = "context_gc"
            ctx.gc()  # Deletes the collected objects

            # Enable automatic garbage collection like
            # we normally expect in python.
            ctx.gc_mode = "auto"
        """
        return self._gc_mode

    @gc_mode.setter
    def gc_mode(self, value: Optional[str]) -> None:
        if value not in self._valid_gc_modes:
            raise ValueError("Valid  gc modes:", self._valid_gc_modes)

        self._gc_mode = value

    @property
    def objects(self) -> Deque[Any]:
        """
        Moderngl objects scheduled for deletion.

        These are deleted when calling :py:meth:`Context.gc`.
        """
        return self._objects

    def gc(self) -> int:
        """
        Deletes OpenGL objects.

        This method must be called to garbage collect
        OpenGL resources when ``gc_mode`` is ``"context_gc"```.

        Calling this method with any other ``gc_mode`` configuration
        has no effect and is perfectly safe.

        Returns:
            int: Number of objects deleted
        """
        count = 0
        # Keep iterating until there are no more objects.
        # An object deletion can trigger new objects to be added
        while self._objects:
            # Remove the oldest objects first
            obj = self._objects.popleft()
            obj.release()
            count += 1

        return count

    @property
    def line_width(self) -> float:
        """
        float: Set the default line width.

        .. Warning:: A line width other than 1.0 is not guaranteed to work
                     across different OpenGL implementations. For wide
                     lines you should be using geometry shaders.

        """
        return self.mglo.line_width

    @line_width.setter
    def line_width(self, value: float) -> None:
        self.mglo.line_width = value

    @property
    def point_size(self) -> float:
        """
        float: Set/get the point size.

        Point size changes the pixel size of rendered points. The min and max values
        are limited by ``POINT_SIZE_RANGE``.
        This value usually at least ``(1, 100)``, but this depends on the drivers/vendors.

        If variable point size is needed you can enable ``PROGRAM_POINT_SIZE``
        and write to ``gl_PointSize`` in the vertex or geometry shader.

        .. Note::

            Using a geometry shader to create triangle strips from points is often a safer
            way to render large points since you don't have have any size restrictions.
        """
        return self.mglo.point_size

    @point_size.setter
    def point_size(self, value: float) -> None:
        self.mglo.point_size = value

    @property
    def depth_func(self) -> str:
        """
        str: Set the default depth func.

        Example::

            ctx.depth_func = '<='  # GL_LEQUAL
            ctx.depth_func = '<'   # GL_LESS
            ctx.depth_func = '>='  # GL_GEQUAL
            ctx.depth_func = '>'   # GL_GREATER
            ctx.depth_func = '=='  # GL_EQUAL
            ctx.depth_func = '!='  # GL_NOTEQUAL
            ctx.depth_func = '0'   # GL_NEVER
            ctx.depth_func = '1'   # GL_ALWAYS
        """
        raise NotImplementedError()

    @depth_func.setter
    def depth_func(self, value: str) -> None:
        self.mglo.depth_func = value

    @property
    def blend_func(self) -> Tuple[int, int]:
        """
        tuple: Set the blend func (write only).

        Blend func can be set for rgb and alpha separately if needed.

        Supported blend functions are::

            moderngl.ZERO
            moderngl.ONE
            moderngl.SRC_COLOR
            moderngl.ONE_MINUS_SRC_COLOR
            moderngl.DST_COLOR
            moderngl.ONE_MINUS_DST_COLOR
            moderngl.SRC_ALPHA
            moderngl.ONE_MINUS_SRC_ALPHA
            moderngl.DST_ALPHA
            moderngl.ONE_MINUS_DST_ALPHA

            # Shortcuts
            moderngl.DEFAULT_BLENDING     # (SRC_ALPHA, ONE_MINUS_SRC_ALPHA)
            moderngl.ADDITIVE_BLENDING    # (ONE, ONE)
            moderngl.PREMULTIPLIED_ALPHA  # (SRC_ALPHA, ONE)

        Example::

            # For both rgb and alpha
            ctx.blend_func = moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA

            # Separate for rgb and alpha
            ctx.blend_func = (
                moderngl.SRC_ALPHA, moderngl.ONE_MINUS_SRC_ALPHA,
                moderngl.ONE, moderngl.ONE
            )
        """
        raise NotImplementedError()

    @blend_func.setter
    def blend_func(self, value: Tuple[int, int]) -> None:
        self.mglo.blend_func = tuple(value)

    @property
    def blend_equation(self) -> Tuple[int, int]:
        """
        tuple: Set the blend equation (write only).

        Blend equations specify how source and destination colors are combined
        in blending operations. By default ``FUNC_ADD`` is used.

        Blend equation can be set for rgb and alpha separately if needed.

        Supported functions are::

            moderngl.FUNC_ADD               # source + destination
            moderngl.FUNC_SUBTRACT          # source - destination
            moderngl.FUNC_REVERSE_SUBTRACT  # destination - source
            moderngl.MIN                    # Minimum of source and destination
            moderngl.MAX                    # Maximum of source and destination

        Example::

            # For both rgb and alpha channel
            ctx.blend_equation = moderngl.FUNC_ADD

            # Separate for rgb and alpha channel
            ctx.blend_equation = moderngl.FUNC_ADD, moderngl.MAX
        """
        raise NotImplementedError()

    @blend_equation.setter
    def blend_equation(self, value: Tuple[int, int]) -> None:
        if not isinstance(value, tuple):
            self.mglo.blend_equation = tuple([value])
        else:
            self.mglo.blend_equation = tuple(value)

    @property
    def multisample(self) -> bool:
        """
        bool: Enable/disable multisample mode (``GL_MULTISAMPLE``).

        This property is write only.

        Example::

            # Enable
            ctx.multisample = True
            # Disable
            ctx.multisample = False
        """
        raise NotImplementedError()

    @multisample.setter
    def multisample(self, value: bool) -> None:
        self.mglo.multisample = value

    @property
    def provoking_vertex(self) -> int:
        """
        int: Specifies the vertex to be used as the source of data for flat shaded varyings.

        Flatshading a vertex shader varying output (ie. ``flat out vec3 pos``) means to assign
        all vetices of the primitive the same value for that output. The vertex from which
        these values is derived is known as the provoking vertex.

        It can be configured to be the first or the last vertex.

        This property is write only.

        Example::

            # Use first vertex
            ctx.provoking_vertex = moderngl.FIRST_VERTEX_CONVENTION

            # Use last vertex
            ctx.provoking_vertex = moderngl.LAST_VERTEX_CONVENTION
        """
        raise NotImplementedError()

    @provoking_vertex.setter
    def provoking_vertex(self, value: int) -> None:
        self.mglo.provoking_vertex = value

    @property
    def polygon_offset(self) -> Tuple[float, float]:
        """
        tuple: Get or set the current polygon offset.

        The tuple values represents two float values: ``unit`` and a ``factor``::

            ctx.polygon_offset = unit, factor

        When drawing polygons, lines or points directly on top of
        exiting geometry the result is often not visually pleasant.
        We can experience z-fighting or partially fading fragments
        due to different primitives not being rasterized in the exact
        same way or simply depth buffer precision issues.

        For example when visualizing polygons drawing a wireframe
        version on top of the original mesh, these issues are
        immediately apparent. Applying decals to surfaces is
        another common example.

        The official documentation states the following::

            When enabled, the depth value of each fragment is added
            to a calculated offset value. The offset is added before
            the depth test is performed and before the depth value
            is written into the depth buffer. The offset value o is calculated by:
            o = m * factor + r * units
            where m is the maximum depth slope of the polygon and r is the smallest
            value guaranteed to produce a resolvable difference in window coordinate
            depth values. The value r is an implementation-specific constant.

        In simpler terms: We use polygon offset to either add a positive offset to
        the geometry (push it away from you) or a negative offset to geometry
        (pull it towards you).

        * ``units`` is a constant offset to depth and will do the job alone
          if we are working with geometry parallel to the near/far plane.
        * The ``factor`` helps you handle sloped geometry (not parallel to near/far plane).

        In most cases you can get away with ``[-1.0, 1.0]`` for both factor and units,
        but definitely play around with the values. When both values are set to ``0``
        polygon offset is disabled internally.

        To just get started with something you can try::

            # Either push the geomtry away or pull it towards you
            # with support for handling small to medium sloped geometry
            ctx.polygon_offset = 1.0, 1.0
            ctx.polygon_offset = -1.0, -1.0

            # Disable polygon offset
            ctx.polygon_offset = 0, 0
        """
        return self.mglo.polygon_offset

    @polygon_offset.setter
    def polygon_offset(self, value: Tuple[float, float]) -> None:
        factor, units = value
        self.mglo.polygon_offset = (float(factor), float(units))

    @property
    def viewport(self) -> Tuple[int, int, int, int]:
        """
        tuple: Get or set the viewport of the active framebuffer.

        Example::

            >>> ctx.viewport
            (0, 0, 1280, 720)
            >>> ctx.viewport = (0, 0, 640, 360)
            >>> ctx.viewport
            (0, 0, 640, 360)

        If no framebuffer is bound ``(0, 0, 0, 0)`` will be returned.
        """
        return self.mglo.fbo.viewport

    @viewport.setter
    def viewport(self, value: Tuple[int, int, int, int]) -> None:
        self.mglo.fbo.viewport = tuple(value)

    @property
    def scissor(self) -> Optional[Tuple[int, int, int, int]]:
        """
        tuple: Get or set the scissor box for the active framebuffer.

        When scissor testing is enabled fragments outside
        the defined scissor box will be discarded. This
        applies to rendered geometry or :py:meth:`Context.clear`.

        Setting is value enables scissor testing in the framebuffer.
        Setting the scissor to ``None`` disables scissor testing
        and reverts the scissor box to match the framebuffer size.

        Example::

            # Enable scissor testing
            >>> ctx.scissor = 100, 100, 200, 100
            # Disable scissor testing
            >>> ctx.scissor = None

        If no framebuffer is bound ``(0, 0, 0, 0)`` will be returned.
        """
        return self.mglo.fbo.scissor

    @scissor.setter
    def scissor(self, value: Optional[Tuple[int, int, int, int]]) -> None:
        if value is None:
            self.mglo.fbo.scissor = None
        else:
            self.mglo.fbo.scissor = tuple(value)

    @property
    def max_samples(self) -> int:
        """int: The maximum supported number of samples for multisampling."""
        return self.mglo.max_samples

    @property
    def max_integer_samples(self) -> int:
        """int: The max integer samples."""
        return self.mglo.max_integer_samples

    @property
    def max_texture_units(self) -> int:
        """int: The max texture units."""
        return self.mglo.max_texture_units

    @property
    def default_texture_unit(self) -> int:
        """int: The default texture unit."""
        return self.mglo.default_texture_unit

    @default_texture_unit.setter
    def default_texture_unit(self, value: int) -> None:
        self.mglo.default_texture_unit = value

    @property
    def max_anisotropy(self) -> float:
        """float: The maximum value supported for anisotropic filtering."""
        return self.mglo.max_anisotropy

    @property
    def screen(self) -> 'Framebuffer':
        """
        Framebuffer: A Framebuffer instance representing the screen.

        Normally set when creating a context with ``create_context()`` attaching to
        an existing context. This is the special system framebuffer
        represented by framebuffer ``id=0``.

        When creating a standalone context this property is not set since
        there are no default framebuffer.
        """
        return self._screen

    @property
    def wireframe(self) -> bool:
        """bool: Wireframe settings for debugging."""
        return self.mglo.wireframe

    @wireframe.setter
    def wireframe(self, value: bool) -> None:
        self.mglo.wireframe = value

    @property
    def front_face(self) -> str:
        """
        str: The front_face. Acceptable values are ``'ccw'`` (default) or ``'cw'``.

        Face culling must be enabled for this to have any effect:
        ``ctx.enable(moderngl.CULL_FACE)``.

        Example::

            # Triangles winded counter-clockwise considered front facing
            ctx.front_face = 'ccw'
            # Triangles winded clockwise considered front facing
            ctx.front_face = 'cw'
        """
        return self.mglo.front_face

    @front_face.setter
    def front_face(self, value: str) -> None:
        self.mglo.front_face = str(value)

    @property
    def cull_face(self) -> str:
        """
        str: The face side to cull. Acceptable values are ``'back'`` (default) ``'front'`` or ``'front_and_back'``.

        This is similar to :py:meth:`Context.front_face`

        Face culling must be enabled for this to have any effect:
        ``ctx.enable(moderngl.CULL_FACE)``.

        Example::

            ctx.cull_face = 'front'
            ctx.cull_face = 'back'
            ctx.cull_face = 'front_and_back'
        """
        return self.mglo.cull_face

    @cull_face.setter
    def cull_face(self, value: str) -> None:
        self.mglo.cull_face = str(value)

    @property
    def patch_vertices(self) -> int:
        """int: The number of vertices that will be used to make up a single patch primitive."""
        return self.mglo.patch_vertices

    @patch_vertices.setter
    def patch_vertices(self, value: int) -> None:
        self.mglo.patch_vertices = value

    @property
    def error(self) -> str:
        """
        str: The result of ``glGetError()`` but human readable.

        This values is provided for debug purposes only and is likely to
        reduce performace when used in a draw loop.
        """
        return self.mglo.error

    @property
    def extensions(self) -> Set[str]:
        """
        Set[str]: The extensions supported by the context.

        All extensions names have a ``GL_`` prefix, so if the spec refers to ``ARB_compute_shader``
        we need to look for ``GL_ARB_compute_shader``::

            # If compute shaders are supported ...
            >> "GL_ARB_compute_shader" in ctx.extensions
            True

        Example data::

            {
                'GL_ARB_multi_bind',
                'GL_ARB_shader_objects',
                'GL_ARB_half_float_vertex',
                'GL_ARB_map_buffer_alignment',
                'GL_ARB_arrays_of_arrays',
                'GL_ARB_pipeline_statistics_query',
                'GL_ARB_provoking_vertex',
                'GL_ARB_gpu_shader5',
                'GL_ARB_uniform_buffer_object',
                'GL_EXT_blend_equation_separate',
                'GL_ARB_tessellation_shader',
                'GL_ARB_multi_draw_indirect',
                'GL_ARB_multisample',
                .. etc ..
            }
        """
        if self._extensions is None:
            self._extensions = self.mglo.extensions

        return self._extensions

    @property
    def info(self) -> Dict[str, Any]:
        """
        dict: OpenGL Limits and information about the context.

        Example::

            # The maximum width and height of a texture
            >> ctx.info["GL_MAX_TEXTURE_SIZE"]
            16384

            # Vendor and renderer
            >> ctx.info["GL_VENDOR"]
            NVIDIA Corporation
            >> ctx.info["GL_RENDERER"]
            NVIDIA GeForce GT 650M OpenGL Engine

        Example data::

            {
                'GL_VENDOR': 'NVIDIA Corporation',
                'GL_RENDERER': 'NVIDIA GeForce GT 650M OpenGL Engine',
                'GL_VERSION': '4.1 NVIDIA-10.32.0 355.11.10.10.40.102',
                'GL_POINT_SIZE_RANGE': (1.0, 2047.0),
                'GL_SMOOTH_LINE_WIDTH_RANGE': (0.5, 1.0),
                'GL_ALIASED_LINE_WIDTH_RANGE': (1.0, 1.0),
                'GL_POINT_FADE_THRESHOLD_SIZE': 1.0,
                'GL_POINT_SIZE_GRANULARITY': 0.125,
                'GL_SMOOTH_LINE_WIDTH_GRANULARITY': 0.125,
                'GL_MIN_PROGRAM_TEXEL_OFFSET': -8.0,
                'GL_MAX_PROGRAM_TEXEL_OFFSET': 7.0,
                'GL_MINOR_VERSION': 1,
                'GL_MAJOR_VERSION': 4,
                'GL_SAMPLE_BUFFERS': 0,
                'GL_SUBPIXEL_BITS': 8,
                'GL_CONTEXT_PROFILE_MASK': 1,
                'GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT': 256,
                'GL_DOUBLEBUFFER': False,
                'GL_STEREO': False,
                'GL_MAX_VIEWPORT_DIMS': (16384, 16384),
                'GL_MAX_3D_TEXTURE_SIZE': 2048,
                'GL_MAX_ARRAY_TEXTURE_LAYERS': 2048,
                'GL_MAX_CLIP_DISTANCES': 8,
                'GL_MAX_COLOR_ATTACHMENTS': 8,
                'GL_MAX_COLOR_TEXTURE_SAMPLES': 8,
                'GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS': 233472,
                'GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS': 231424,
                'GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS': 80,
                'GL_MAX_COMBINED_UNIFORM_BLOCKS': 70,
                'GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS': 233472,
                'GL_MAX_CUBE_MAP_TEXTURE_SIZE': 16384,
                'GL_MAX_DEPTH_TEXTURE_SAMPLES': 8,
                'GL_MAX_DRAW_BUFFERS': 8,
                'GL_MAX_DUAL_SOURCE_DRAW_BUFFERS': 1,
                'GL_MAX_ELEMENTS_INDICES': 150000,
                'GL_MAX_ELEMENTS_VERTICES': 1048575,
                'GL_MAX_FRAGMENT_INPUT_COMPONENTS': 128,
                'GL_MAX_FRAGMENT_UNIFORM_COMPONENTS': 4096,
                'GL_MAX_FRAGMENT_UNIFORM_VECTORS': 1024,
                'GL_MAX_FRAGMENT_UNIFORM_BLOCKS': 14,
                'GL_MAX_GEOMETRY_INPUT_COMPONENTS': 128,
                'GL_MAX_GEOMETRY_OUTPUT_COMPONENTS': 128,
                'GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS': 16,
                'GL_MAX_GEOMETRY_UNIFORM_BLOCKS': 14,
                'GL_MAX_GEOMETRY_UNIFORM_COMPONENTS': 2048,
                'GL_MAX_INTEGER_SAMPLES': 1,
                'GL_MAX_SAMPLES': 8,
                'GL_MAX_RECTANGLE_TEXTURE_SIZE': 16384,
                'GL_MAX_RENDERBUFFER_SIZE': 16384,
                'GL_MAX_SAMPLE_MASK_WORDS': 1,
                'GL_MAX_SERVER_WAIT_TIMEOUT': -1,
                'GL_MAX_TEXTURE_BUFFER_SIZE': 134217728,
                'GL_MAX_TEXTURE_IMAGE_UNITS': 16,
                'GL_MAX_TEXTURE_LOD_BIAS': 15,
                'GL_MAX_TEXTURE_SIZE': 16384,
                'GL_MAX_UNIFORM_BUFFER_BINDINGS': 70,
                'GL_MAX_UNIFORM_BLOCK_SIZE': 65536,
                'GL_MAX_VARYING_COMPONENTS': 0,
                'GL_MAX_VARYING_VECTORS': 31,
                'GL_MAX_VARYING_FLOATS': 0,
                'GL_MAX_VERTEX_ATTRIBS': 16,
                'GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS': 16,
                'GL_MAX_VERTEX_UNIFORM_COMPONENTS': 4096,
                'GL_MAX_VERTEX_UNIFORM_VECTORS': 1024,
                'GL_MAX_VERTEX_OUTPUT_COMPONENTS': 128,
                'GL_MAX_VERTEX_UNIFORM_BLOCKS': 14,
                'GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET': 0,
                'GL_MAX_VERTEX_ATTRIB_BINDINGS': 0,
                'GL_VIEWPORT_BOUNDS_RANGE': (-32768, 32768),
                'GL_VIEWPORT_SUBPIXEL_BITS': 0,
                'GL_MAX_VIEWPORTS': 16
            }
        """
        if self._info is None:
            self._info = self.mglo.info

        return self._info

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
        Clear the bound framebuffer.

        If a `viewport` passed in, a scissor test will be used to clear the given viewport.
        This viewport take prescense over the framebuffers :py:attr:`~moderngl.Framebuffer.scissor`.
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
            color (tuple): Optional rgba color tuple
        """
        if color is not None:
            red, green, blue, alpha, *_ = tuple(color) + (0.0, 0.0, 0.0, 0.0)

        self.mglo.fbo.clear(red, green, blue, alpha, depth, viewport)

    def enable_only(self, flags: int) -> None:
        """
        Clears all existing flags applying new ones.

        Note that the enum values defined in moderngl
        are not the same as the ones in opengl.
        These are defined as bit flags so we can logical
        `or` them together.

        Available flags:

        - :py:data:`moderngl.NOTHING`
        - :py:data:`moderngl.BLEND`
        - :py:data:`moderngl.DEPTH_TEST`
        - :py:data:`moderngl.CULL_FACE`
        - :py:data:`moderngl.RASTERIZER_DISCARD`
        - :py:data:`moderngl.PROGRAM_POINT_SIZE`

        Examples::

            # Disable all flags
            ctx.enable_only(moderngl.NOTHING)

            # Ensure only depth testing and face culling is enabled
            ctx.enable_only(moderngl.DEPTH_TEST | moderngl.CULL_FACE)

        Args:
            flags (EnableFlag): The flags to enable
        """
        self.mglo.enable_only(flags)

    def enable(self, flags: int) -> None:
        """
        Enable flags.

        Note that the enum values defined in moderngl
        are not the same as the ones in opengl.
        These are defined as bit flags so we can logical
        `or` them together.

        For valid flags, please see :py:meth:`enable_only`.

        Examples::

            # Enable a single flag
            ctx.enable(moderngl.DEPTH_TEST)

            # Enable multiple flags
            ctx.enable(moderngl.DEPTH_TEST | moderngl.CULL_FACE | moderngl.BLEND)

        Args:
            flag (int): The flags to enable.
        """
        self.mglo.enable(flags)

    def disable(self, flags: int) -> None:
        """
        Disable flags.

        For valid flags, please see :py:meth:`enable_only`.

        Examples::

            # Only disable depth testing
            ctx.disable(moderngl.DEPTH_TEST)

            # Disable depth testing and face culling
            ctx.disable(moderngl.DEPTH_TEST | moderngl.CULL_FACE)

        Args:
            flag (int): The flags to disable.
        """
        self.mglo.disable(flags)

    def enable_direct(self, enum: int) -> None:
        """
        Gives direct access to ``glEnable`` so unsupported capabilities in ModernGL can be enabled.

        Do not use this to set already supported context flags.

        Example::

            # Enum value from the opengl registry
            GL_CONSERVATIVE_RASTERIZATION_NV = 0x9346
            ctx.enable_direct(GL_CONSERVATIVE_RASTERIZATION_NV)
        """
        self.mglo.enable_direct(enum)

    def disable_direct(self, enum: int) -> None:
        """
        Gives direct access to ``glDisable`` so unsupported capabilities in ModernGL can be disabled.

        Do not use this to set already supported context flags.

        Example::

            # Enum value from the opengl registry
            GL_CONSERVATIVE_RASTERIZATION_NV = 0x9346
            ctx.disable_direct(GL_CONSERVATIVE_RASTERIZATION_NV)
        """
        self.mglo.disable_direct(enum)

    def finish(self) -> None:
        """Wait for all drawing commands to finish."""
        self.mglo.finish()

    def copy_buffer(
        self,
        dst: Buffer,
        src: Buffer,
        size: int = -1,
        *,
        read_offset: int = 0,
        write_offset: int = 0
    ) -> None:
        """
        Copy buffer content.

        Args:
            dst (Buffer): The destination buffer.
            src (Buffer): The source buffer.
            size (int): The number of bytes to copy.

        Keyword Args:
            read_offset (int): The read offset.
            write_offset (int): The write offset.
        """
        self.mglo.copy_buffer(dst.mglo, src.mglo, size, read_offset, write_offset)

    def copy_framebuffer(
        self,
        dst: Union[Framebuffer, Texture],
        src: Framebuffer,
    ) -> None:
        """
        Copy framebuffer content.

        Use this method to:

            - blit framebuffers.
            - copy framebuffer content into a texture.
            - downsample framebuffers. (it will allow to read the framebuffer's content)
            - downsample a framebuffer directly to a texture.

        Args:
            dst (Framebuffer or Texture): Destination framebuffer or texture.
            src (Framebuffer): Source framebuffer.
        """
        self.mglo.copy_framebuffer(dst.mglo, src.mglo)

    def detect_framebuffer(self, glo: Optional[int] = None) -> 'Framebuffer':
        """
        Detect a framebuffer.

        This is already done when creating a context,
        but if the underlying window library for some changes the default framebuffer
        during the lifetime of the application this might be necessary.

        Args:
            glo (int): Framebuffer object.

        Returns:
            :py:class:`Framebuffer` object
        """
        res = Framebuffer.__new__(Framebuffer)
        res.mglo, res._size, res._samples, res._glo = self.mglo.detect_framebuffer(glo)
        res._color_attachments = None
        res._depth_attachment = None
        res.ctx = self
        res._is_reference = True
        res.extra = None
        return res

    def buffer(
        self,
        data: Optional[Any] = None,
        *,
        reserve: int = 0,
        dynamic: bool = False,
    ) -> Buffer:
        """
        Create a :py:class:`Buffer` object.

        Args:
            data (bytes): Content of the new buffer.

        Keyword Args:
            reserve (int): The number of bytes to reserve.
            dynamic (bool): Treat buffer as dynamic.

        Returns:
            :py:class:`Buffer` object
        """
        if type(reserve) is str:
            reserve = mgl.strsize(reserve)

        res = Buffer.__new__(Buffer)
        res.mglo, res._size, res._glo = self.mglo.buffer(data, reserve, dynamic)
        res._dynamic = dynamic
        res.ctx = self
        res.extra = None
        return res

    def external_texture(
        self,
        glo: int,
        size: Tuple[int, int],
        components: int,
        samples: int,
        dtype: str,
    ) -> 'Texture':
        """
        Create a :py:class:`Texture` object from an existing OpenGL texture object.

        Args:
            glo (int): External OpenGL texture object.
            size (tuple): The width and height of the texture.
            components (int): The number of components 1, 2, 3 or 4.
            samples (int): The number of samples. Value 0 means no multisample format.
            dtype (str): Data type.
        """

        res = Texture.__new__(Texture)
        res.mglo, res._glo = self.mglo.external_texture(glo, size, components, samples, dtype)
        res._size = size
        res._components = components
        res._samples = samples
        res._dtype = dtype
        res._depth = False
        res.ctx = self
        res.extra = None
        return res

    def texture(
        self,
        size: Tuple[int, int],
        components: int,
        data: Optional[Any] = None,
        *,
        samples: int = 0,
        alignment: int = 1,
        dtype: str = 'f1',
        internal_format: Optional[int] = None,
    ) -> 'Texture':
        """
        Create a :py:class:`Texture` object.

        .. Warning:: Do not play with ``internal_format`` unless you know exactly
                        you are doing. This is an override to support sRGB and
                        compressed textures if needed.

        Args:
            size (tuple): The width and height of the texture.
            components (int): The number of components 1, 2, 3 or 4.
            data (bytes): Content of the texture.

        Keyword Args:
            samples (int): The number of samples. Value 0 means no multisample format.
            alignment (int): The byte alignment 1, 2, 4 or 8.
            dtype (str): Data type.
            internal_format (int): Override the internalformat of the texture (IF needed)

        Returns:
            :py:class:`Texture` object
        """
        res = Texture.__new__(Texture)
        res.mglo, res._glo = self.mglo.texture(size, components, data, samples, alignment, dtype, internal_format or 0)
        res._size = size
        res._components = components
        res._samples = samples
        res._dtype = dtype
        res._depth = False
        res.ctx = self
        res.extra = None
        return res

    def texture_array(
        self,
        size: Tuple[int, int, int],
        components: int,
        data: Optional[Any] = None,
        *,
        alignment: int = 1,
        dtype: str = 'f1',
    ) -> 'TextureArray':
        """
        Create a :py:class:`TextureArray` object.

        Args:
            size (tuple): The ``(width, height, layers)`` of the texture.
            components (int): The number of components 1, 2, 3 or 4.
            data (bytes): Content of the texture. The size must be ``(width, height * layers)``
                            so each layer is stacked vertically.

        Keyword Args:
            alignment (int): The byte alignment 1, 2, 4 or 8.
            dtype (str): Data type.

        Returns:
            :py:class:`Texture3D` object
        """
        res = TextureArray.__new__(TextureArray)
        res.mglo, res._glo = self.mglo.texture_array(size, components, data, alignment, dtype)
        res._size = size
        res._components = components
        res._dtype = dtype
        res.ctx = self
        res.extra = None
        return res

    def texture3d(
        self,
        size: Tuple[int, int, int],
        components: int,
        data: Optional[Any] = None,
        *,
        alignment: int = 1,
        dtype: str = 'f1',
    ) -> 'Texture3D':
        """
        Create a :py:class:`Texture3D` object.

        Args:
            size (tuple): The width, height and depth of the texture.
            components (int): The number of components 1, 2, 3 or 4.
            data (bytes): Content of the texture.

        Keyword Args:
            alignment (int): The byte alignment 1, 2, 4 or 8.
            dtype (str): Data type.

        Returns:
            :py:class:`Texture3D` object
        """
        res = Texture3D.__new__(Texture3D)
        res.mglo, res._glo = self.mglo.texture3d(size, components, data, alignment, dtype)
        res.ctx = self
        res.extra = None
        return res

    def texture_cube(
        self,
        size: Tuple[int, int],
        components: int,
        data: Optional[Any] = None,
        *,
        alignment: int = 1,
        dtype: str = 'f1',
        internal_format: Optional[int] = None,
    ) -> 'TextureCube':
        """
        Create a :py:class:`TextureCube` object.

        Note that the width and height of the cubemap must be the same
        unless you are using a non-standard extension.

        Args:
            size (tuple): The width, height of the texture. Each side of
                            the cube will have this size.
            components (int): The number of components 1, 2, 3 or 4.
            data (bytes): Content of the texture. The data should be have the following ordering:
                            positive_x, negative_x, positive_y, negative_y, positive_z, negative_z

        Keyword Args:
            alignment (int): The byte alignment 1, 2, 4 or 8.
            dtype (str): Data type.
            internal_format (int): Override the internalformat of the texture (IF needed)

        Returns:
            :py:class:`TextureCube` object
        """
        res = TextureCube.__new__(TextureCube)
        res.mglo, res._glo = self.mglo.texture_cube(size, components, data, alignment, dtype, internal_format or 0)
        res._size = size
        res._components = components
        res._dtype = dtype
        res.ctx = self
        res.extra = None
        return res

    def depth_texture(
        self,
        size: Tuple[int, int],
        data: Optional[Any] = None,
        *,
        samples: int = 0,
        alignment: int = 4,
    ) -> 'Texture':
        """
        Create a :py:class:`Texture` object.

        Args:
            size (tuple): The width and height of the texture.
            data (bytes): Content of the texture.

        Keyword Args:
            samples (int): The number of samples. Value 0 means no multisample format.
            alignment (int): The byte alignment 1, 2, 4 or 8.

        Returns:
            :py:class:`Texture` object
        """
        res = Texture.__new__(Texture)
        res.mglo, res._glo = self.mglo.depth_texture(size, data, samples, alignment)
        res._size = size
        res._components = 1
        res._samples = samples
        res._dtype = 'f4'
        res._depth = True
        res.ctx = self
        res.extra = None
        return res

    def vertex_array(self, *args, **kwargs) -> 'VertexArray':
        """
        Create a :py:class:`VertexArray` object.

        The vertex array describes how buffers are read by a shader program.
        We need to supply buffer formats and attributes names. The attribute names
        are defined by the user in the glsl code and can be anything.

        Examples::

            # Empty vertext array (no attribute input)
            vao = ctx.vertex_array(program)

            # Simple version with a single buffer
            vao = ctx.vertex_array(program, buffer, "in_position", "in_normal")
            vao = ctx.vertex_array(program, buffer, "in_position", "in_normal", index_buffer=ibo)

            # Multiple buffers
            vao = ctx.vertex_array(program, [
                (buffer1, '3f', 'in_position'),
                (buffer2, '3f', 'in_normal'),
            ])
            vao = ctx.vertex_array(program, [
                    (buffer1, '3f', 'in_position'),
                    (buffer2, '3f', 'in_normal'),
                ],
                index_buffer=ibo,
                index_element_size=2,  # 16 bit / 'u2' index buffer
            )

        This method also supports arguments for :py:meth:`Context.simple_vertex_array`.

        Args:
            program (Program): The program used when rendering
            content (list): A list of (buffer, format, attributes).
                            See :ref:`buffer-format-label`.

        Keyword Args:
            index_buffer (Buffer): An index buffer (optional)
            index_element_size (int): byte size of each index element, 1, 2 or 4.
            skip_errors (bool): Ignore errors during creation
            mode (int): The default draw mode (for example: ``TRIANGLES``)

        Returns:
            :py:class:`VertexArray` object
        """
        if len(args) > 2 and type(args[1]) is Buffer:
            return self.simple_vertex_array(*args, **kwargs)
        return self._vertex_array(*args, **kwargs)

    def _vertex_array(
        self,
        program: Program,
        content: Any,
        index_buffer: Optional[Buffer] = None,
        index_element_size: int = 4,
        *,
        skip_errors: bool = False,
        mode: Optional[int] = None,
    ) -> 'VertexArray':
        """
        Create a :py:class:`VertexArray` object.

        Args:
            program (Program): The program used when rendering.
            content (list): A list of (buffer, format, attributes).
                            See :ref:`buffer-format-label`.
            index_buffer (Buffer): An index buffer.

        Keyword Args:
            index_element_size (int): byte size of each index element, 1, 2 or 4.
            skip_errors (bool): Ignore skip_errors varyings.
            mode (int): The default draw mode (for example: ``TRIANGLES``)

        Returns:
            :py:class:`VertexArray` object
        """
        members = program._members
        index_buffer_mglo = None if index_buffer is None else index_buffer.mglo
        mgl_content = tuple(
            (a.mglo, b) + tuple(getattr(members.get(x), 'mglo', None) for x in c)
            for a, b, *c in content
        )

        res = VertexArray.__new__(VertexArray)
        res.mglo, res._glo = self.mglo.vertex_array(
            program.mglo, mgl_content, index_buffer_mglo,
            index_element_size, skip_errors,
        )
        res._program = program
        res._index_buffer = index_buffer
        res._content = content
        res._index_element_size = index_element_size
        if mode is not None:
            res._mode = mode
        else:
            res._mode = self.POINTS if program.is_transform else self.TRIANGLES
        res.ctx = self
        res.extra = None
        res.scope = None
        return res

    def simple_vertex_array(
        self,
        program: Program,
        buffer: Buffer,
        *attributes: Union[List[str], Tuple[str, ...]],
        index_buffer: Optional[Buffer] = None,
        index_element_size: int = 4,
        mode: Optional[int] = None,
    ) -> 'VertexArray':
        """
        Create a :py:class:`VertexArray` object.

        .. Warning:: This method is deprecated and may be removed in the future.
                        Use :py:meth:`Context.vertex_array` instead. It also supports
                        the argument format this method describes.

        Args:
            program (Program): The program used when rendering.
            buffer (Buffer): The buffer.
            attributes (list): A list of attribute names.

        Keyword Args:
            index_element_size (int): byte size of each index element, 1, 2 or 4.
            index_buffer (Buffer): An index buffer.
            mode (int): The default draw mode (for example: ``TRIANGLES``)

        Returns:
            :py:class:`VertexArray` object
        """
        if type(buffer) is list:
            raise SyntaxError('Change simple_vertex_array to vertex_array')

        content = [(buffer, detect_format(program, attributes)) + attributes]
        return self._vertex_array(program, content, index_buffer, index_element_size, mode=mode)

    def program(
        self,
        *,
        vertex_shader: str,
        fragment_shader: Optional[str] = None,
        geometry_shader: Optional[str] = None,
        tess_control_shader: Optional[str] = None,
        tess_evaluation_shader: Optional[str] = None,
        varyings: Tuple[str, ...] = (),
        fragment_outputs: Optional[Dict[str, int]] = None,
        varyings_capture_mode: str = 'interleaved',
    ) -> 'Program':
        """
        Create a :py:class:`Program` object.

        The ``varyings`` are only used when a transform program is created
        to specify the names of the output varyings to capture in the output buffer.

        ``fragment_outputs`` can be used to programmatically map named fragment
        shader outputs to a framebuffer attachment numbers. This can also be done
        by using ``layout(location=N)`` in the fragment shader.

        Args:
            vertex_shader (str): The vertex shader source.
            fragment_shader (str): The fragment shader source.
            geometry_shader (str): The geometry shader source.
            tess_control_shader (str): The tessellation control shader source.
            tess_evaluation_shader (str): The tessellation evaluation shader source.
            varyings (list): A list of varyings.
            fragment_outputs (dict): A dictionary of fragment outputs.
        Returns:
            :py:class:`Program` object
        """

        if varyings_capture_mode not in ('interleaved', 'separate'):
            raise ValueError('varyings_capture_mode must be interleaved or separate')

        if type(varyings) is str:
            varyings = (varyings,)

        varyings = tuple(varyings)

        if fragment_outputs is None:
            fragment_outputs = {}

        res = Program.__new__(Program)
        res.mglo, ls1, ls2, ls3, ls4, ls5, res._subroutines, res._geom, res._glo = self.mglo.program(
            vertex_shader, fragment_shader, geometry_shader, tess_control_shader, tess_evaluation_shader,
            varyings, fragment_outputs, varyings_capture_mode == 'interleaved'
        )

        members = {}

        for item in ls1:
            obj = Attribute.__new__(Attribute)
            obj.mglo, obj._location, obj._array_length, obj._dimension, obj._shape, obj._name = item
            members[obj.name] = obj

        for item in ls2:
            obj = Varying.__new__(Varying)
            obj._number, obj._array_length, obj._dimension, obj._name = item
            members[obj.name] = obj

        for item in ls3:
            obj = Uniform.__new__(Uniform)
            obj.mglo, obj._location, obj._array_length, obj._dimension, obj._name = item
            members[obj.name] = obj

        for item in ls4:
            obj = UniformBlock.__new__(UniformBlock)
            obj.mglo, obj._index, obj._size, obj._name = item
            members[obj.name] = obj

        for item in ls5:
            obj = Subroutine.__new__(Subroutine)
            obj._index, obj._name = item
            members[obj.name] = obj

        res._members = members
        res._is_transform = fragment_shader is None
        res.ctx = self
        res.extra = None
        return res

    def query(
        self,
        *,
        samples: bool = False,
        any_samples: bool = False,
        time: bool = False,
        primitives: bool = False,
    ) -> 'Query':
        """
        Create a :py:class:`Query` object.

        Keyword Args:
            samples (bool): Query ``GL_SAMPLES_PASSED`` or not.
            any_samples (bool): Query ``GL_ANY_SAMPLES_PASSED`` or not.
            time (bool): Query ``GL_TIME_ELAPSED`` or not.
            primitives (bool): Query ``GL_PRIMITIVES_GENERATED`` or not.
        """
        res = Query.__new__(Query)
        res.mglo = self.mglo.query(samples, any_samples, time, primitives)
        res.crender = None

        if samples or any_samples:
            res.crender = ConditionalRender.__new__(ConditionalRender)
            res.crender.mglo = res.mglo

        res.ctx = self
        res.extra = None
        return res

    def scope(
        self,
        framebuffer: Optional[Framebuffer] = None,
        enable_only: Optional[int] = None,
        *,
        textures: Tuple[Tuple[Texture, int], ...] = (),
        uniform_buffers: Tuple[Tuple[Buffer, int], ...] = (),
        storage_buffers: Tuple[Tuple[Buffer, int], ...] = (),
        samplers: Tuple[Tuple[Sampler, int], ...] = (),
        enable: Optional[int] = None,
    ) -> 'Scope':
        """
        Create a :py:class:`Scope` object.

        Args:
            framebuffer (Framebuffer): The framebuffer to use when entering.
            enable_only (int): The enable_only flags to set when entering.

        Keyword Args:
            textures (tuple): List of (texture, binding) tuples.
            uniform_buffers (tuple): Tuple of (buffer, binding) tuples.
            storage_buffers (tuple): Tuple of (buffer, binding) tuples.
            samplers (tuple): Tuple of sampler bindings
            enable (int): Flags to enable for this vao such as depth testing and blending
        """
        if enable is not None:
            enable_only = enable

        if framebuffer is None:
            framebuffer = self.screen

        mgl_textures = tuple((tex.mglo, idx) for tex, idx in textures)
        mgl_uniform_buffers = tuple((buf.mglo, idx) for buf, idx in uniform_buffers)
        mgl_storage_buffers = tuple((buf.mglo, idx) for buf, idx in storage_buffers)

        res = Scope.__new__(Scope)
        res.mglo = self.mglo.scope(framebuffer.mglo, enable_only, mgl_textures,
                                   mgl_uniform_buffers, mgl_storage_buffers, samplers)
        res.ctx = self
        res._framebuffer = framebuffer
        res._textures = textures
        res._uniform_buffers = uniform_buffers
        res._storage_buffers = storage_buffers
        res._samplers = samplers
        res.extra = None
        return res

    def simple_framebuffer(
        self,
        size: Tuple[int, int],
        components: int = 4,
        *,
        samples: int = 0,
        dtype: str = 'f1',
    ) -> 'Framebuffer':
        """
        Creates a :py:class:`Framebuffer` with a single color attachment \
        and depth buffer using :py:class:`moderngl.Renderbuffer` attachments.

        Args:
            size (tuple): The width and height of the renderbuffer.
            components (int): The number of components 1, 2, 3 or 4.

        Keyword Args:
            samples (int): The number of samples. Value 0 means no multisample format.
            dtype (str): Data type.

        Returns:
            :py:class:`Framebuffer` object
        """
        return self.framebuffer(
            self.renderbuffer(size, components, samples=samples, dtype=dtype),
            self.depth_renderbuffer(size, samples=samples),
        )

    def framebuffer(
        self,
        color_attachments: Any = (),
        depth_attachment: Optional[Union[Texture, Renderbuffer]] = None,
    ) -> 'Framebuffer':
        """
        A :py:class:`Framebuffer` is a collection of buffers that can be \
        used as the destination for rendering. The buffers for Framebuffer \
        objects reference images from either Textures or Renderbuffers.

        Args:
            color_attachments (list): A list of :py:class:`Texture` or
                                        :py:class:`Renderbuffer` objects.
            depth_attachment (Renderbuffer or Texture): The depth attachment.

        Returns:
            :py:class:`Framebuffer` object
        """
        if type(color_attachments) is Texture or type(color_attachments) is Renderbuffer:
            color_attachments = (color_attachments,)

        ca_mglo = tuple(x.mglo for x in color_attachments)
        da_mglo = None if depth_attachment is None else depth_attachment.mglo

        res = Framebuffer.__new__(Framebuffer)
        res.mglo, res._size, res._samples, res._glo = self.mglo.framebuffer(ca_mglo, da_mglo)
        res._color_attachments = tuple(color_attachments)
        res._depth_attachment = depth_attachment
        res.ctx = self
        res._is_reference = False
        res.extra = None
        return res

    def renderbuffer(
        self,
        size: Tuple[int, int],
        components: int = 4,
        *,
        samples: int = 0,
        dtype: str = 'f1',
    ) -> 'Renderbuffer':
        """
        :py:class:`Renderbuffer` objects are OpenGL objects that contain images. \
        They are created and used specifically with :py:class:`Framebuffer` objects.

        Args:
            size (tuple): The width and height of the renderbuffer.
            components (int): The number of components 1, 2, 3 or 4.

        Keyword Args:
            samples (int): The number of samples. Value 0 means no multisample format.
            dtype (str): Data type.

        Returns:
            :py:class:`Renderbuffer` object
        """
        res = Renderbuffer.__new__(Renderbuffer)
        res.mglo, res._glo = self.mglo.renderbuffer(size, components, samples, dtype)
        res._size = size
        res._components = components
        res._samples = samples
        res._dtype = dtype
        res._depth = False
        res.ctx = self
        res.extra = None
        return res

    def depth_renderbuffer(
        self,
        size: Tuple[int, int],
        *,
        samples: int = 0
    ) -> 'Renderbuffer':
        """
        :py:class:`Renderbuffer` objects are OpenGL objects that contain images. \
        They are created and used specifically with :py:class:`Framebuffer` objects.

        Args:
            size (tuple): The width and height of the renderbuffer.

        Keyword Args:
            samples (int): The number of samples. Value 0 means no multisample format.

        Returns:
            :py:class:`Renderbuffer` object
        """
        res = Renderbuffer.__new__(Renderbuffer)
        res.mglo, res._glo = self.mglo.depth_renderbuffer(size, samples)
        res._size = size
        res._components = 1
        res._samples = samples
        res._dtype = 'f4'
        res._depth = True
        res.ctx = self
        res.extra = None
        return res

    def compute_shader(self, source: str) -> 'ComputeShader':
        """
        A :py:class:`ComputeShader` is a Shader Stage that is used entirely \
        for computing arbitrary information. While it can do rendering, it \
        is generally used for tasks not directly related to drawing.

        Args:
            source (str): The source of the compute shader.

        Returns:
            :py:class:`ComputeShader` object
        """
        res = ComputeShader.__new__(ComputeShader)
        res.mglo, ls1, ls2, ls3, ls4, res._glo = self.mglo.compute_shader(source)

        members = {}

        for item in ls1:
            obj = Uniform.__new__(Uniform)
            obj.mglo, obj._location, obj._array_length, obj._dimension, obj._name = item
            members[obj.name] = obj

        for item in ls2:
            obj = UniformBlock.__new__(UniformBlock)
            obj.mglo, obj._index, obj._size, obj._name = item
            members[obj.name] = obj

        res._members = members
        res.ctx = self
        res.extra = None
        return res

    def sampler(
        self,
        repeat_x: bool = True,
        repeat_y: bool = True,
        repeat_z: bool = True,
        filter: Tuple[int, int] = None,
        anisotropy: float = 1.0,
        compare_func: str = '?',
        border_color: Tuple[float, float, float, float] = None,
        min_lod: float = -1000.0,
        max_lod: float = 1000.0,
        texture: Optional[Texture] = None,
    ) -> Sampler:
        """
        Create a :py:class:`Sampler` object.

        Keyword Arguments:
            repeat_x (bool): Repeat texture on x
            repeat_y (bool): Repeat texture on y
            repeat_z (bool): Repeat texture on z
            filter (tuple): The min and max filter
            anisotropy (float): Number of samples for anisotropic filtering.
                                Any value greater than 1.0 counts as a use of
                                anisotropic filtering
            compare_func: Compare function for depth textures
            border_color (tuple): The (r, g, b, a) color for the texture border.
                                    When this value is set the ``repeat_`` values are
                                    overridden setting the texture wrap to return the
                                    border color when outside ``[0, 1]`` range.
            min_lod (float): Minimum level-of-detail parameter (Default ``-1000.0``).
                                This floating-point value limits the selection of highest
                                resolution mipmap (lowest mipmap level)
            max_lod (float): Minimum level-of-detail parameter (Default ``1000.0``).
                                This floating-point value limits the selection of the
                                lowest resolution mipmap (highest mipmap level)
            texture (Texture): The texture for this sampler
        """
        res = Sampler.__new__(Sampler)
        res.mglo, res._glo = self.mglo.sampler()
        res.ctx = self
        res.repeat_x = repeat_x
        res.repeat_y = repeat_y
        res.repeat_z = repeat_z
        res.filter = filter or (9729, 9729)
        res.anisotropy = anisotropy
        res.compare_func = compare_func
        if border_color:
            res.border_color = border_color
        res.min_lod = min_lod
        res.max_lod = max_lod
        res.extra = None
        res.texture = texture
        return res

    def clear_samplers(self, start: int = 0, end: int = -1) -> None:
        """
        Unbinds samplers from texture units.

        Sampler bindings do clear automatically between every frame,
        but lingering samplers can still be a source of weird bugs during
        the frame rendering. This methods provides a fairly brute force
        and efficient way to ensure texture units are clear.

        Keyword Arguments:

            start (int): The texture unit index to start the clearing samplers
            stop (int): The texture unit index to stop clearing samplers

        Example::

            # Clear texture unit 0, 1, 2, 3, 4
            ctx.clear_samplers(start=0, end=5)

            # Clear texture unit 4, 5, 6, 7
            ctx.clear_samplers(start=4, end=8)
        """
        self.mglo.clear_samplers(start, end)

    def core_profile_check(self) -> None:
        """
        Core profile check.

        FOR DEBUG PURPOSES ONLY
        """
        profile_mask = self.info['GL_CONTEXT_PROFILE_MASK']
        if profile_mask != 1:
            warnings.warn('The window should request a CORE OpenGL profile')

        version_code = self.version_code
        if not version_code:
            major, minor = map(int, self.info['GL_VERSION'].split('.', 2)[:2])
            version_code = major * 100 + minor * 10

        if version_code < 330:
            warnings.warn('The window should support OpenGL 3.3+ (version_code=%d)' % version_code)

    def __enter__(self):
        """
        Enters the context.

        This should ideally be used with the ``with`` statement::

            with other_context as ctx:
                # Do something in this context

        When exiting the context the previously bound context is activated again.

        .. Warning:: Context switching can be risky unless you know what you are doing.
                     Use with care.
        """
        self.mglo.__enter__()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """
        Exit the context.

        See :py:meth:`Context.__enter__`
        """
        self.mglo.__exit__(exc_type, exc_val, exc_tb)

    def release(self) -> None:
        """
        Release the ModernGL context.

        If the context is not standalone the standard
        backends in ``glcontext`` will not do anything
        because the context was not created by moderngl.

        Standalone contexts can normally be released.
        """
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()


def create_context(
    require: Optional[int] = None,
    standalone: bool = False,
    share: bool = False,
    **settings: Dict[str, Any],
) -> Context:
    """
    Create a ModernGL context by loading OpenGL functions from an existing OpenGL context. \
    An OpenGL context must exists.

    Example::

        # Accept the current context version
        ctx = moderngl.create_context()

        # Require at least OpenGL 4.3
        ctx = moderngl.create_context(require=430)

        # Create a headless context requiring OpenGL 4.3
        ctx = moderngl.create_context(require=430, standalone=True)

    Keyword Arguments:
        require (int): OpenGL version code (default: 330)
        standalone (bool): Headless flag
        share (bool): Attempt to create a shared context
        **settings: Other backend specific settings

    Returns:
        :py:class:`Context` object
    """
    if require is None:
        require = 330

    mode = 'standalone' if standalone is True else 'detect'
    if share is True:
        mode = 'share'

    from moderngl import mgl

    ctx = Context.__new__(Context)
    ctx.mglo, ctx.version_code = mgl.create_context(glversion=require, mode=mode, **settings)
    ctx._info = None
    ctx._extensions = None
    ctx.extra = None
    ctx._gc_mode = None
    ctx._objects = deque()

    if ctx.version_code < require:
        raise ValueError('Requested OpenGL version {0}, got version {1}'.format(
            require, ctx.version_code))

    if standalone:
        ctx._screen = None
        ctx.fbo = None
    else:
        ctx._screen = ctx.detect_framebuffer(0)  # Default framebuffer
        ctx.fbo = ctx.detect_framebuffer()  # Currently bound framebuffer
        ctx.mglo.fbo = ctx.fbo.mglo

    return ctx


def create_standalone_context(
    require: Optional[int] = None,
    share: bool = False,
    **settings: Dict[str, Any],
) -> 'Context':
    """
    Create a standalone/headless ModernGL context.

    The preferred way of making a context is through :py:func:`moderngl.create_context`.

    Example::

        # Create a context with highest possible supported version
        ctx = moderngl.create_context()

        # Require at least OpenGL 4.3
        ctx = moderngl.create_context(require=430)

    Keyword Arguments:
        require (int): OpenGL version code.
        share (bool): Attempt to create a shared context
        settings: keyword config values for the context backend

    Returns:
        :py:class:`Context` object
    """
    if require is None:
        require = 330

    mode = 'share' if share is True else 'standalone'

    ctx = Context.__new__(Context)
    ctx.mglo, ctx.version_code = mgl.create_context(glversion=require, mode=mode, **settings)
    ctx._screen = None
    ctx.fbo = None
    ctx._info = None
    ctx._extensions = None
    ctx.extra = None
    ctx._gc_mode = None
    ctx._objects = deque()

    if require is not None and ctx.version_code < require:
        raise ValueError('Requested OpenGL version {0}, got version {1}'.format(
            require, ctx.version_code))

    return ctx
