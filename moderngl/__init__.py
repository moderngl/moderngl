import warnings
from collections import deque
from typing import Any, Deque, Dict, Generator, List, Optional, Set, Tuple, Union

from _moderngl import Attribute, Error, InvalidObject, Subroutine, Uniform, UniformBlock, Varying, StorageBlock  # noqa

try:
    from moderngl import mgl  # type: ignore
except ImportError:
    pass

__version__ = '5.8.0'

# Context Flags

NOTHING = 0
BLEND = 1
DEPTH_TEST = 2
CULL_FACE = 4
RASTERIZER_DISCARD = 8
PROGRAM_POINT_SIZE = 16

# Primitive modes

POINTS = 0x0000
LINES = 0x0001
LINE_LOOP = 0x0002
LINE_STRIP = 0x0003
TRIANGLES = 0x0004
TRIANGLE_STRIP = 0x0005
TRIANGLE_FAN = 0x0006
LINES_ADJACENCY = 0x000A
LINE_STRIP_ADJACENCY = 0x000B
TRIANGLES_ADJACENCY = 0x000C
TRIANGLE_STRIP_ADJACENCY = 0x0000D
PATCHES = 0x000E

# Texture filters

NEAREST = 0x2600
LINEAR = 0x2601
NEAREST_MIPMAP_NEAREST = 0x2700
LINEAR_MIPMAP_NEAREST = 0x2701
NEAREST_MIPMAP_LINEAR = 0x2702
LINEAR_MIPMAP_LINEAR = 0x2703

# Blend function constants

ZERO = 0x0000
ONE = 0x0001
SRC_COLOR = 0x0300
ONE_MINUS_SRC_COLOR = 0x0301
SRC_ALPHA = 0x0302
ONE_MINUS_SRC_ALPHA = 0x0303
DST_ALPHA = 0x0304
ONE_MINUS_DST_ALPHA = 0x0305
DST_COLOR = 0x0306
ONE_MINUS_DST_COLOR = 0x0307

DEFAULT_BLENDING = (SRC_ALPHA, ONE_MINUS_SRC_ALPHA)
ADDITIVE_BLENDING = (ONE, ONE)
PREMULTIPLIED_ALPHA = (SRC_ALPHA, ONE)

# Blend equations

FUNC_ADD = 0x8006
FUNC_SUBTRACT = 0x800A
FUNC_REVERSE_SUBTRACT = 0x800B
MIN = 0x8007
MAX = 0x8008

# Provoking vertex

FIRST_VERTEX_CONVENTION = 0x8E4D
LAST_VERTEX_CONVENTION = 0x8E4E


class Buffer:
    def __init__(self):
        self.mglo = None
        self._size = None
        self._dynamic = None
        self._glo = None
        self.ctx = None
        self.extra = None
        raise TypeError()

    def __repr__(self) -> str:
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
    def size(self) -> int:
        return self.mglo.size()

    @property
    def dynamic(self) -> bool:
        return self._dynamic

    @property
    def glo(self) -> int:
        return self._glo

    def write(self, data: Any, *, offset: int = 0) -> None:
        self.mglo.write(data, offset)

    def write_chunks(self, data: Any, start: int, step: int, count: int) -> None:
        self.mglo.write_chunks(data, start, step, count)

    def read(self, size: int = -1, *, offset: int = 0) -> bytes:
        return self.mglo.read(size, offset)

    def read_into(self, buffer: Any, size: int = -1, *, offset: int = 0, write_offset: int = 0) -> None:
        return self.mglo.read_into(buffer, size, offset, write_offset)

    def read_chunks(self, chunk_size: int, start: int, step: int, count: int) -> bytes:
        return self.mglo.read_chunks(chunk_size, start, step, count)

    def read_chunks_into(
        self,
        buffer: Any,
        chunk_size: int,
        start: int,
        step: int,
        count: int,
        *,
        write_offset: int = 0
    ) -> None:
        return self.mglo.read(buffer, chunk_size, start, step, count, write_offset)

    def clear(self, size: int = -1, *, offset: int = 0, chunk: Any = None) -> None:
        self.mglo.clear(size, offset, chunk)

    def bind_to_uniform_block(self, binding: int = 0, *, offset: int = 0, size: int = -1) -> None:
        self.mglo.bind_to_uniform_block(binding, offset, size)

    def bind_to_storage_buffer(self, binding: int = 0, *, offset: int = 0, size: int = -1) -> None:
        self.mglo.bind_to_storage_buffer(binding, offset, size)

    def orphan(self, size: int = -1) -> None:
        self.mglo.orphan(size)

    def release(self) -> None:
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
            self.mglo = InvalidObject()

    def bind(self, *attribs, layout=None):
        return (self, layout, *attribs)

    def assign(self, index: int) -> Tuple["Buffer", int]:
        return (self, index)


class ConditionalRender:
    def __init__(self):
        self.mglo = None
        raise TypeError()

    def __repr__(self):
        return '<ConditionalRender>'

    def __enter__(self):
        self.mglo.begin_render()
        return self

    def __exit__(self, *args):
        self.mglo.end_render()


class Query:
    def __init__(self):
        self.mglo = None
        self.crender = None
        self.ctx = None
        self.extra = None
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
        return self.mglo.samples

    @property
    def primitives(self) -> int:
        return self.mglo.primitives

    @property
    def elapsed(self) -> int:
        return self.mglo.elapsed


class ComputeShader:
    def __init__(self):
        self.mglo = None
        self._members = {}
        self._glo = None
        self.ctx = None
        self.extra = None
        raise TypeError()

    def __repr__(self) -> str:
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

    def __getitem__(self, key: str) -> Union[Uniform, UniformBlock, Subroutine, Attribute, Varying]:
        return self._members[key]

    def __setitem__(self, key: str, value: Any):
        self._members[key].value = value

    def __iter__(self) -> Generator[str, None, None]:
        yield from self._members

    @property
    def glo(self) -> int:
        return self._glo

    def run(self, group_x: int = 1, group_y: int = 1, group_z: int = 1) -> None:
        return self.mglo.run(group_x, group_y, group_z)

    def get(self, key: str, default: Any) -> Union[Uniform, UniformBlock, Subroutine, Attribute, Varying]:
        return self._members.get(key, default)

    def release(self) -> None:
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
            self.mglo = InvalidObject()


class Framebuffer:
    def __init__(self):
        self.mglo = None
        self._color_attachments = None
        self._depth_attachment = None
        self._size = (None, None)
        self._samples: int = None
        self._glo: int = None
        self.ctx: Context = None
        self._is_reference = None
        self.extra: Any = None
        raise TypeError()

    def __repr__(self) -> str:
        if hasattr(self, '_glo'):
            return '<Framebuffer: %d>' % self._glo
        else:
            return "<Framebuffer: INCOMPLETE>"

    def __eq__(self, other: Any) -> bool:
        return type(self) is type(other) and self.mglo is other.mglo

    def __hash__(self) -> int:
        return id(self)

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        # Don't delete default framebuffer or a reference
        if self._is_reference:
            return

        # If object was initialized properly (ctx present) and gc_mode is auto
        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    @property
    def viewport(self) -> Tuple[int, int, int, int]:
        return self.mglo.viewport

    @viewport.setter
    def viewport(self, value: Tuple[int, int, int, int]) -> None:
        x, y, w, h = value
        self.mglo.viewport = (int(x), int(y), int(w), int(h))

    @property
    def scissor(self) -> Tuple[int, int, int, int]:
        return self.mglo.scissor

    @scissor.setter
    def scissor(self, value: Tuple[int, int, int, int]) -> None:
        if value is None:
            self.mglo.scissor = None
        else:
            x, y, w, h = value
            self.mglo.scissor = (int(x), int(y), int(w), int(h))

    @property
    def color_mask(self) -> Tuple[bool, bool, bool, bool]:
        return self.mglo.color_mask

    @color_mask.setter
    def color_mask(self, value: Tuple[bool, bool, bool, bool]) -> None:
        self.mglo.color_mask = value

    @property
    def depth_mask(self) -> bool:
        return self.mglo.depth_mask

    @depth_mask.setter
    def depth_mask(self, value: bool) -> None:
        self.mglo.depth_mask = value

    @property
    def width(self) -> int:
        return self._size[0]

    @property
    def height(self) -> int:
        return self._size[1]

    @property
    def size(self) -> tuple:
        return self._size

    @property
    def samples(self) -> int:
        return self._samples

    @property
    def bits(self) -> Dict[str, str]:
        return self.mglo.bits

    @property
    def color_attachments(self) -> Tuple[Union['Texture', 'Renderbuffer'], ...]:
        return self._color_attachments

    @property
    def depth_attachment(self) -> Union['Texture', 'Renderbuffer']:
        return self._depth_attachment

    @property
    def glo(self) -> int:
        return self._glo

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
        if color is not None:
            red, green, blue, alpha, *_ = tuple(color) + (0.0, 0.0, 0.0, 0.0)

        if viewport is not None:
            viewport = tuple(viewport)  # type: ignore

        self.mglo.clear(red, green, blue, alpha, depth, viewport)

    def use(self) -> None:
        self.ctx.fbo = self
        self.mglo.use()

    def read(
        self,
        viewport: Optional[Union[Tuple[int, int], Tuple[int, int, int, int]]] = None,
        components: int = 3,
        *,
        attachment: int = 0,
        alignment: int = 1,
        dtype: str = 'f1',
        clamp: bool = False,
    ) -> bytes:
        return self.mglo.read(viewport, components, attachment, alignment, clamp, dtype)

    def read_into(
        self,
        buffer: Any,
        viewport: Optional[Union[Tuple[int, int], Tuple[int, int, int, int]]] = None,
        components: int = 3,
        *,
        attachment: int = 0,
        alignment: int = 1,
        dtype: str = 'f1',
        write_offset: int = 0,
    ) -> None:
        if type(buffer) is Buffer:
            buffer = buffer.mglo

        return self.mglo.read_into(buffer, viewport, components, attachment, alignment, dtype, write_offset)

    def release(self) -> None:
        if not isinstance(self.mglo, InvalidObject):
            self._color_attachments = None
            self._depth_attachment = None
            self.mglo.release()
            self.mglo = InvalidObject()


class Program:
    def __init__(self):
        self.mglo = None
        self._members = {}
        self._subroutines = None
        self._geom = (None, None, None)
        self._glo = None
        self._is_transform = None
        self.ctx = None
        self.extra = None
        raise TypeError()

    def __repr__(self):
        if hasattr(self, '_glo'):
            return f"<{self.__class__.__name__}: {self._glo}>"
        else:
            return f"<{self.__class__.__name__}: INCOMPLETE>"

    def __eq__(self, other: Any) -> bool:
        return type(self) is type(other) and self.mglo is other.mglo

    def __hash__(self) -> int:
        return id(self)

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    def __getitem__(self, key: str) -> Union[Uniform, UniformBlock, Subroutine, Attribute, Varying]:
        return self._members[key]

    def __setitem__(self, key: str, value: Any) -> None:
        self._members[key].value = value

    def __iter__(self) -> Generator[str, None, None]:
        yield from self._members

    @property
    def is_transform(self) -> bool:
        return self._is_transform

    @property
    def geometry_input(self) -> int:
        return self._geom[0]

    @property
    def geometry_output(self) -> int:
        return self._geom[1]

    @property
    def geometry_vertices(self) -> int:
        return self._geom[2]

    @property
    def subroutines(self) -> Tuple[str, ...]:
        return self._subroutines

    @property
    def glo(self) -> int:
        return self._glo

    def get(self, key: str, default: Any) -> Union[Uniform, UniformBlock, Subroutine, Attribute, Varying]:
        return self._members.get(key, default)

    def release(self) -> None:
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
            self.mglo = InvalidObject()


class Renderbuffer:
    def __init__(self):
        self.mglo = None
        self._size = (None, None)
        self._components = None
        self._samples = None
        self._depth = None
        self._dtype = None
        self._glo = None
        self.ctx = None
        self.extra = None
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
        return self._size[0]

    @property
    def height(self) -> int:
        return self._size[1]

    @property
    def size(self) -> tuple:
        return self._size

    @property
    def samples(self) -> int:
        return self._samples

    @property
    def components(self) -> int:
        return self._components

    @property
    def depth(self) -> bool:
        return self._depth

    @property
    def dtype(self) -> str:
        return self._dtype

    @property
    def glo(self) -> int:
        return self._glo

    def release(self) -> None:
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
            self.mglo = InvalidObject()


class Sampler:
    def __init__(self):
        self.mglo = None
        self._glo = None
        self.ctx = None
        self.extra = None
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
        if self.texture is not None:
            self.texture.use(location)
        self.mglo.use(location)

    def clear(self, location: int = 0) -> None:
        self.mglo.clear(location)

    def release(self) -> None:
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
            self.mglo = InvalidObject()

    @property
    def repeat_x(self) -> bool:
        return self.mglo.repeat_x

    @repeat_x.setter
    def repeat_x(self, value: bool) -> None:
        self.mglo.repeat_x = value

    @property
    def repeat_y(self) -> bool:
        return self.mglo.repeat_y

    @repeat_y.setter
    def repeat_y(self, value: bool) -> None:
        self.mglo.repeat_y = value

    @property
    def repeat_z(self) -> bool:
        return self.mglo.repeat_z

    @repeat_z.setter
    def repeat_z(self, value: bool) -> None:
        self.mglo.repeat_z = value

    @property
    def filter(self) -> Tuple[int, int]:
        return self.mglo.filter

    @filter.setter
    def filter(self, value: Tuple[int, int]) -> None:
        self.mglo.filter = value

    @property
    def compare_func(self) -> str:
        return self.mglo.compare_func

    @compare_func.setter
    def compare_func(self, value: str) -> None:
        self.mglo.compare_func = value

    @property
    def anisotropy(self) -> float:
        return self.mglo.anisotropy

    @anisotropy.setter
    def anisotropy(self, value: float) -> None:
        self.mglo.anisotropy = value

    @property
    def border_color(self) -> Tuple[float, float, float, float]:
        return self.mglo.border_color

    @border_color.setter
    def border_color(self, value: Tuple[float, float, float, float]) -> None:
        self.mglo.border_color = value

    @property
    def min_lod(self) -> float:
        return self.mglo.min_lod

    @min_lod.setter
    def min_lod(self, value: float) -> None:
        self.mglo.min_lod = value

    @property
    def max_lod(self) -> float:
        return self.mglo.max_lod

    @max_lod.setter
    def max_lod(self, value: float) -> None:
        self.mglo.max_lod = value

    def assign(self, index: int) -> Tuple["Sampler", int]:
        return (self, index)


class Scope:
    def __init__(self):
        self.mglo = None
        self.ctx = None
        # Keep references to keep this objects alive
        self._framebuffer = None
        self._textures = None
        self._uniform_buffers = None
        self._storage_buffers = None
        self._samplers = None
        self.extra = None
        raise TypeError()

    def __repr__(self):
        return '<Scope>'

    def __hash__(self) -> int:
        return id(self)

    def __enter__(self):
        self.mglo.begin()
        return self

    def __exit__(self, *args: Tuple[Any]):
        self.mglo.end()

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    def release(self) -> None:
        if not isinstance(self.mglo, InvalidObject):
            self._framebuffer = None
            self._textures = None
            self._uniform_buffers = None
            self._storage_buffers = None
            self._samplers = None
            self.mglo.release()
            self.mglo = InvalidObject()


class Texture:
    def __init__(self):
        self.mglo = None
        self._size = (None, None)
        self._components = None
        self._samples = None
        self._dtype = None
        self._depth = None
        self._glo = None
        self.ctx = None
        self.extra = None
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

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    @property
    def repeat_x(self) -> bool:
        return self.mglo.repeat_x

    @repeat_x.setter
    def repeat_x(self, value: bool) -> None:
        self.mglo.repeat_x = value

    @property
    def repeat_y(self) -> bool:
        return self.mglo.repeat_y

    @repeat_y.setter
    def repeat_y(self, value: bool) -> None:
        self.mglo.repeat_y = value

    @property
    def filter(self) -> Tuple[int, int]:
        return self.mglo.filter

    @filter.setter
    def filter(self, value: Tuple[int, int]) -> None:
        self.mglo.filter = value

    @property
    def anisotropy(self) -> float:
        return self.mglo.anisotropy

    @anisotropy.setter
    def anisotropy(self, value: float) -> None:
        self.mglo.anisotropy = value

    @property
    def swizzle(self) -> str:
        return self.mglo.swizzle

    @swizzle.setter
    def swizzle(self, value: str) -> None:
        self.mglo.swizzle = value

    @property
    def compare_func(self) -> str:
        return self.mglo.compare_func

    @compare_func.setter
    def compare_func(self, value: str) -> None:
        self.mglo.compare_func = value

    @property
    def width(self) -> int:
        return self._size[0]

    @property
    def height(self) -> int:
        return self._size[1]

    @property
    def size(self) -> tuple:
        return self._size

    @property
    def components(self) -> int:
        return self._components

    @property
    def samples(self) -> int:
        return self._samples

    @property
    def dtype(self) -> str:
        return self._dtype

    @property
    def depth(self) -> bool:
        return self._depth

    @property
    def glo(self) -> int:
        return self._glo

    def read(self, *, level: int = 0, alignment: int = 1) -> bytes:
        return self.mglo.read(level, alignment)

    def read_into(
        self,
        buffer: Any,
        *,
        level: int = 0,
        alignment: int = 1,
        write_offset: int = 0,
    ) -> None:
        if type(buffer) is Buffer:
            buffer = buffer.mglo

        return self.mglo.read_into(buffer, level, alignment, write_offset)

    def write(
        self,
        data: Any,
        viewport: Optional[Union[Tuple[int, int], Tuple[int, int, int, int]]] = None,
        *,
        level: int = 0,
        alignment: int = 1,
    ) -> None:
        if type(data) is Buffer:
            data = data.mglo

        self.mglo.write(data, viewport, level, alignment)

    def build_mipmaps(self, base: int = 0, max_level: int = 1000) -> None:
        self.mglo.build_mipmaps(base, max_level)

    def use(self, location: int = 0) -> None:
        self.mglo.use(location)

    def bind_to_image(self, unit: int, read: bool = True, write: bool = True, level: int = 0, format: int = 0) -> None:
        self.mglo.bind(unit, read, write, level, format)

    def release(self) -> None:
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
            self.mglo = InvalidObject()


class Texture3D:
    def __init__(self):
        self.mglo = None
        self._size = (None, None, None)
        self._components = None
        self._samples = None
        self._dtype = None
        self._glo = None
        self.ctx = None
        self.extra = None
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

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    @property
    def repeat_x(self) -> bool:
        return self.mglo.repeat_x

    @repeat_x.setter
    def repeat_x(self, value: bool) -> None:
        self.mglo.repeat_x = value

    @property
    def repeat_y(self) -> bool:
        return self.mglo.repeat_y

    @repeat_y.setter
    def repeat_y(self, value: bool) -> None:
        self.mglo.repeat_y = value

    @property
    def repeat_z(self) -> bool:
        return self.mglo.repeat_z

    @repeat_z.setter
    def repeat_z(self, value: bool) -> None:
        self.mglo.repeat_z = value

    @property
    def filter(self) -> Tuple[int, int]:
        return self.mglo.filter

    @filter.setter
    def filter(self, value: Tuple[int, int]) -> None:
        self.mglo.filter = value

    @property
    def swizzle(self) -> str:
        return self.mglo.swizzle

    @swizzle.setter
    def swizzle(self, value: str) -> None:
        self.mglo.swizzle = value

    @property
    def width(self) -> int:
        return self._size[0]

    @property
    def height(self) -> int:
        return self._size[1]

    @property
    def depth(self) -> int:
        return self._size[2]

    @property
    def size(self) -> tuple:
        return self._size

    @property
    def components(self) -> int:
        return self._components

    @property
    def dtype(self) -> str:
        return self._dtype

    @property
    def glo(self) -> int:
        return self._glo

    def read(self, *, alignment: int = 1) -> bytes:
        return self.mglo.read(alignment)

    def read_into(
        self,
        buffer: Any,
        *,
        alignment: int = 1,
        write_offset: int = 0,
    ) -> None:
        if type(buffer) is Buffer:
            buffer = buffer.mglo

        return self.mglo.read_into(buffer, alignment, write_offset)

    def write(
        self,
        data: Any,
        viewport: Optional[Union[Tuple[int, int, int], Tuple[int, int, int, int, int, int]]] = None,
        *,
        alignment: int = 1,
    ) -> None:
        if type(data) is Buffer:
            data = data.mglo

        self.mglo.write(data, viewport, alignment)

    def build_mipmaps(self, base: int = 0, max_level: int = 1000) -> None:
        self.mglo.build_mipmaps(base, max_level)

    def use(self, location: int = 0) -> None:
        self.mglo.use(location)

    def bind_to_image(self, unit: int, read: bool = True, write: bool = True, level: int = 0, format: int = 0) -> None:
        self.mglo.bind(unit, read, write, level, format)

    def release(self) -> None:
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
            self.mglo = InvalidObject()


class TextureCube:
    def __init__(self):
        self.mglo = None
        self._size = (None, None)
        self._components = None
        self._dtype = None
        self._glo = None
        self.ctx = None
        self.extra = None
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
    def size(self) -> Tuple[int, int]:
        return self._size

    @property
    def components(self) -> int:
        return self._components

    @property
    def dtype(self) -> str:
        return self._dtype

    @property
    def filter(self) -> Tuple[int, int]:
        return self.mglo.filter

    @filter.setter
    def filter(self, value: Tuple[int, int]) -> None:
        self.mglo.filter = value

    @property
    def swizzle(self) -> str:
        return self.mglo.swizzle

    @swizzle.setter
    def swizzle(self, value: str) -> None:
        self.mglo.swizzle = value

    @property
    def anisotropy(self) -> float:
        return self.mglo.anisotropy

    @anisotropy.setter
    def anisotropy(self, value: float) -> None:
        self.mglo.anisotropy = value

    @property
    def glo(self) -> int:
        return self._glo

    def read(self, face: int, *, alignment: int = 1) -> bytes:
        return self.mglo.read(face, alignment)

    def read_into(
        self,
        buffer: Any,
        face: int,
        *,
        alignment: int = 1,
        write_offset: int = 0,
    ) -> None:
        if type(buffer) is Buffer:
            buffer = buffer.mglo

        return self.mglo.read_into(buffer, face, alignment, write_offset)

    def write(
        self,
        face: int,
        data: Any,
        viewport: Optional[Union[Tuple[int, int], Tuple[int, int, int, int]]] = None,
        *,
        alignment: int = 1,
    ) -> None:
        if type(data) is Buffer:
            data = data.mglo

        self.mglo.write(face, data, viewport, alignment)

    def use(self, location: int = 0) -> None:
        self.mglo.use(location)

    def bind_to_image(self, unit: int, read: bool = True, write: bool = True, level: int = 0, format: int = 0) -> None:
        self.mglo.bind(unit, read, write, level, format)

    def release(self) -> None:
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
            self.mglo = InvalidObject()


class TextureArray:
    def __init__(self):
        self.mglo = None
        self._size = (None, None, None)
        self._components = None
        self._samples = None
        self._dtype = None
        self._depth = None
        self._glo = None
        self.ctx = None
        self.extra = None
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

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    @property
    def repeat_x(self) -> bool:
        return self.mglo.repeat_x

    @repeat_x.setter
    def repeat_x(self, value: bool) -> None:
        self.mglo.repeat_x = value

    @property
    def repeat_y(self) -> bool:
        return self.mglo.repeat_y

    @repeat_y.setter
    def repeat_y(self, value: bool) -> None:
        self.mglo.repeat_y = value

    @property
    def filter(self) -> Tuple[int, int]:
        return self.mglo.filter

    @filter.setter
    def filter(self, value: Tuple[int, int]) -> None:
        self.mglo.filter = value

    @property
    def swizzle(self) -> str:
        return self.mglo.swizzle

    @swizzle.setter
    def swizzle(self, value: str) -> None:
        self.mglo.swizzle = value

    @property
    def anisotropy(self) -> float:
        return self.mglo.anisotropy

    @anisotropy.setter
    def anisotropy(self, value: float) -> None:
        self.mglo.anisotropy = value

    @property
    def width(self) -> int:
        return self._size[0]

    @property
    def height(self) -> int:
        return self._size[1]

    @property
    def layers(self) -> int:
        return self._size[2]

    @property
    def size(self) -> tuple:
        return self._size

    @property
    def components(self) -> int:
        return self._components

    @property
    def dtype(self) -> str:
        return self._dtype

    @property
    def glo(self) -> int:
        return self._glo

    def read(self, *, alignment: int = 1) -> bytes:
        return self.mglo.read(alignment)

    def read_into(
        self,
        buffer: Any,
        *,
        alignment: int = 1,
        write_offset: int = 0,
    ) -> None:
        if type(buffer) is Buffer:
            buffer = buffer.mglo

        return self.mglo.read_into(buffer, alignment, write_offset)

    def write(
        self,
        data: Any,
        viewport: Optional[Union[Tuple[int, int, int], Tuple[int, int, int, int, int, int]]] = None,
        *,
        alignment: int = 1,
    ) -> None:
        if type(data) is Buffer:
            data = data.mglo

        self.mglo.write(data, viewport, alignment)

    def build_mipmaps(self, base: int = 0, max_level: int = 1000) -> None:
        self.mglo.build_mipmaps(base, max_level)

    def use(self, location: int = 0) -> None:
        self.mglo.use(location)

    def bind_to_image(self, unit: int, read: bool = True, write: bool = True, level: int = 0, format: int = 0) -> None:
        self.mglo.bind(unit, read, write, level, format)

    def release(self) -> None:
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
            self.mglo = InvalidObject()


class VertexArray:
    def __init__(self):
        self.mglo = None
        self._program = None
        self._index_buffer = None
        self._content = None
        self._index_element_size = None
        self._glo = None
        self._mode = None
        self.ctx = None
        self.extra = None
        self.scope = None
        raise TypeError()

    def __repr__(self) -> str:
        if hasattr(self, 'mglo'):
            return '<VertexArray: %d>' % self.glo
        else:
            return '<VertexArray: INCOMPLETE>'

    def __eq__(self, other: Any) -> bool:
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
    def mode(self) -> int:
        return self._mode

    @mode.setter
    def mode(self, value: int) -> None:
        self._mode = value

    @property
    def program(self) -> 'Program':
        return self._program

    @property
    def index_buffer(self) -> 'Buffer':
        return self._index_buffer

    @property
    def index_element_size(self) -> int:
        return self._index_element_size

    @property
    def vertices(self) -> int:
        return self.mglo.vertices

    @vertices.setter
    def vertices(self, value: int) -> None:
        self.mglo.vertices = int(value)

    @property
    def instances(self) -> int:
        return self.mglo.instances

    @instances.setter
    def instances(self, value: int) -> None:
        self.mglo.instances = int(value)

    @property
    def subroutines(self) -> Tuple[int, ...]:
        return self.mglo.subroutines

    @subroutines.setter
    def subroutines(self, value: Tuple[int, ...]) -> None:
        self.mglo.subroutines = tuple(value)

    @property
    def glo(self) -> int:
        return self._glo

    def render(
        self,
        mode: Optional[int] = None,
        vertices: int = -1,
        *,
        first: int = 0,
        instances: int = -1,
    ) -> None:
        if mode is None:
            mode = self._mode

        if self.scope:
            with self.scope:
                self.mglo.render(mode, vertices, first, instances)
        else:
            self.mglo.render(mode, vertices, first, instances)

    def render_indirect(
        self,
        buffer: "Buffer",
        mode: Optional[int] = None,
        count: int = -1,
        *,
        first: int = 0,
    ) -> None:
        if mode is None:
            mode = self._mode

        if self.scope:
            with self.scope:
                self.mglo.render_indirect(buffer.mglo, mode, count, first)
        else:
            self.mglo.render_indirect(buffer.mglo, mode, count, first)

    def transform(
        self,
        buffer: Union["Buffer", List["Buffer"]],
        mode: Optional[int] = None,
        vertices: int = -1,
        *,
        first: int = 0,
        instances: int = -1,
        buffer_offset: int = 0,
    ) -> None:
        if mode is None:
            mode = self._mode

        if isinstance(buffer, (list, tuple)):
            outputs = [buf.mglo for buf in buffer]
        else:
            outputs = [buffer.mglo]

        if self.scope:
            with self.scope:
                self.mglo.transform(outputs, mode, vertices, first, instances, buffer_offset)
        else:
            self.mglo.transform(outputs, mode, vertices, first, instances, buffer_offset)

    def bind(
        self,
        attribute: int,
        cls: str,
        buffer: "Buffer",
        fmt: str,
        *,
        offset: int = 0,
        stride: int = 0,
        divisor: int = 0,
        normalize: bool = False,
    ) -> None:
        self.mglo.bind(attribute, cls, buffer.mglo, fmt, offset, stride, divisor, normalize)

    def release(self) -> None:
        if not isinstance(self.mglo, InvalidObject):
            self._program = None
            self._index_buffer = None
            self._content = None
            self.mglo.release()
            self.mglo = InvalidObject()


class Context:
    _valid_gc_modes = [None, "context_gc", "auto"]

    # Context Flags

    NOTHING = 0
    BLEND = 1
    DEPTH_TEST = 2
    CULL_FACE = 4
    RASTERIZER_DISCARD = 8
    PROGRAM_POINT_SIZE = 16

    # Primitive modes

    POINTS = 0x0000
    LINES = 0x0001
    LINE_LOOP = 0x0002
    LINE_STRIP = 0x0003
    TRIANGLES = 0x0004
    TRIANGLE_STRIP = 0x0005
    TRIANGLE_FAN = 0x0006
    LINES_ADJACENCY = 0x000A
    LINE_STRIP_ADJACENCY = 0x000B
    TRIANGLES_ADJACENCY = 0x000C
    TRIANGLE_STRIP_ADJACENCY = 0x0000D
    PATCHES = 0x000E

    # Texture filters

    NEAREST = 0x2600
    LINEAR = 0x2601
    NEAREST_MIPMAP_NEAREST = 0x2700
    LINEAR_MIPMAP_NEAREST = 0x2701
    NEAREST_MIPMAP_LINEAR = 0x2702
    LINEAR_MIPMAP_LINEAR = 0x2703

    # Blend function constants

    ZERO = 0x0000
    ONE = 0x0001
    SRC_COLOR = 0x0300
    ONE_MINUS_SRC_COLOR = 0x0301
    SRC_ALPHA = 0x0302
    ONE_MINUS_SRC_ALPHA = 0x0303
    DST_ALPHA = 0x0304
    ONE_MINUS_DST_ALPHA = 0x0305
    DST_COLOR = 0x0306
    ONE_MINUS_DST_COLOR = 0x0307

    DEFAULT_BLENDING = (SRC_ALPHA, ONE_MINUS_SRC_ALPHA)
    ADDITIVE_BLENDING = (ONE, ONE)
    PREMULTIPLIED_ALPHA = (SRC_ALPHA, ONE)

    # Blend equations

    FUNC_ADD = 0x8006
    FUNC_SUBTRACT = 0x800A
    FUNC_REVERSE_SUBTRACT = 0x800B
    MIN = 0x8007
    MAX = 0x8008

    # Provoking vertex

    FIRST_VERTEX_CONVENTION = 0x8E4D
    LAST_VERTEX_CONVENTION = 0x8E4E

    def __init__(self):
        self.mglo = None
        self._screen = None
        self._info = None
        self._extensions = None
        self.version_code = None
        self.fbo = None
        self.extra = None
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
        return self._gc_mode

    @gc_mode.setter
    def gc_mode(self, value: Optional[str]) -> None:
        if value not in self._valid_gc_modes:
            raise ValueError("Valid  gc modes:", self._valid_gc_modes)

        self._gc_mode = value

    @property
    def objects(self) -> Deque[Any]:
        return self._objects

    def gc(self) -> int:
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
        return self.mglo.line_width

    @line_width.setter
    def line_width(self, value: float) -> None:
        self.mglo.line_width = value

    @property
    def point_size(self) -> float:
        return self.mglo.point_size

    @point_size.setter
    def point_size(self, value: float) -> None:
        self.mglo.point_size = value

    @property
    def depth_func(self) -> str:
        raise NotImplementedError()

    @depth_func.setter
    def depth_func(self, value: str) -> None:
        self.mglo.depth_func = value

    @property
    def blend_func(self) -> Tuple[int, int]:
        raise NotImplementedError()

    @blend_func.setter
    def blend_func(self, value: Tuple[int, int]) -> None:
        self.mglo.blend_func = tuple(value)

    @property
    def blend_equation(self) -> Tuple[int, int]:
        raise NotImplementedError()

    @blend_equation.setter
    def blend_equation(self, value: Tuple[int, int]) -> None:
        if not isinstance(value, tuple):
            self.mglo.blend_equation = tuple([value])
        else:
            self.mglo.blend_equation = tuple(value)

    @property
    def multisample(self) -> bool:
        raise NotImplementedError()

    @multisample.setter
    def multisample(self, value: bool) -> None:
        self.mglo.multisample = value

    @property
    def provoking_vertex(self) -> int:
        raise NotImplementedError()

    @provoking_vertex.setter
    def provoking_vertex(self, value: int) -> None:
        self.mglo.provoking_vertex = value

    @property
    def polygon_offset(self) -> Tuple[float, float]:
        return self.mglo.polygon_offset

    @polygon_offset.setter
    def polygon_offset(self, value: Tuple[float, float]) -> None:
        factor, units = value
        self.mglo.polygon_offset = (float(factor), float(units))

    @property
    def viewport(self) -> Tuple[int, int, int, int]:
        return self.mglo.fbo.viewport

    @viewport.setter
    def viewport(self, value: Tuple[int, int, int, int]) -> None:
        x, y, w, h = value
        self.mglo.fbo.viewport = (int(x), int(y), int(w), int(h))

    @property
    def scissor(self) -> Optional[Tuple[int, int, int, int]]:
        return self.mglo.fbo.scissor

    @scissor.setter
    def scissor(self, value: Optional[Tuple[int, int, int, int]]) -> None:
        if value is None:
            self.mglo.fbo.scissor = None
        else:
            x, y, w, h = value
            self.mglo.fbo.scissor = (int(x), int(y), int(w), int(h))

    @property
    def max_samples(self) -> int:
        return self.mglo.max_samples

    @property
    def max_integer_samples(self) -> int:
        return self.mglo.max_integer_samples

    @property
    def max_texture_units(self) -> int:
        return self.mglo.max_texture_units

    @property
    def default_texture_unit(self) -> int:
        return self.mglo.default_texture_unit

    @default_texture_unit.setter
    def default_texture_unit(self, value: int) -> None:
        self.mglo.default_texture_unit = value

    @property
    def max_anisotropy(self) -> float:
        return self.mglo.max_anisotropy

    @property
    def screen(self) -> 'Framebuffer':
        return self._screen

    @property
    def wireframe(self) -> bool:
        return self.mglo.wireframe

    @wireframe.setter
    def wireframe(self, value: bool) -> None:
        self.mglo.wireframe = value

    @property
    def front_face(self) -> str:
        return self.mglo.front_face

    @front_face.setter
    def front_face(self, value: str) -> None:
        self.mglo.front_face = str(value)

    @property
    def cull_face(self) -> str:
        return self.mglo.cull_face

    @cull_face.setter
    def cull_face(self, value: str) -> None:
        self.mglo.cull_face = str(value)

    @property
    def patch_vertices(self) -> int:
        return self.mglo.patch_vertices

    @patch_vertices.setter
    def patch_vertices(self, value: int) -> None:
        self.mglo.patch_vertices = value

    @property
    def error(self) -> str:
        return self.mglo.error

    @property
    def extensions(self) -> Set[str]:
        if self._extensions is None:
            self._extensions = self.mglo.extensions

        return self._extensions

    @property
    def info(self) -> Dict[str, Any]:
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
        if color is not None:
            red, green, blue, alpha, *_ = tuple(color) + (0.0, 0.0, 0.0, 0.0)

        self.mglo.fbo.clear(red, green, blue, alpha, depth, viewport)

    def enable_only(self, flags: int) -> None:
        self.mglo.enable_only(flags)

    def enable(self, flags: int) -> None:
        self.mglo.enable(flags)

    def disable(self, flags: int) -> None:
        self.mglo.disable(flags)

    def enable_direct(self, enum: int) -> None:
        self.mglo.enable_direct(enum)

    def disable_direct(self, enum: int) -> None:
        self.mglo.disable_direct(enum)

    def finish(self) -> None:
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
        self.mglo.copy_buffer(dst.mglo, src.mglo, size, read_offset, write_offset)

    def copy_framebuffer(
        self,
        dst: Union[Framebuffer, Texture],
        src: Framebuffer,
    ) -> None:
        self.mglo.copy_framebuffer(dst.mglo, src.mglo)

    def detect_framebuffer(self, glo: Optional[int] = None) -> 'Framebuffer':
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

        res = Texture3D.__new__(Texture3D)
        res._size = size
        res._components = components
        res._dtype = dtype
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
        members = program._members
        index_buffer_mglo = None if index_buffer is None else index_buffer.mglo
        mgl_content = tuple(
            (a.mglo, b) + tuple(members.get(x) for x in c)
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

        if varyings_capture_mode not in ('interleaved', 'separate'):
            raise ValueError('varyings_capture_mode must be interleaved or separate')

        if type(varyings) is str:
            varyings = (varyings,)  # type: ignore

        varyings = tuple(varyings)

        if fragment_outputs is None:
            fragment_outputs = {}

        res = Program.__new__(Program)
        res.mglo, res._members, res._subroutines, res._geom, res._glo = self.mglo.program(
            vertex_shader, fragment_shader, geometry_shader, tess_control_shader, tess_evaluation_shader,
            varyings, fragment_outputs, varyings_capture_mode == 'interleaved'
        )

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
        if enable is not None:
            enable_only = enable

        if framebuffer is None:
            framebuffer = self.screen
            if framebuffer is None:
                raise RuntimeError('A framebuffer must be specified')

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
        return self.framebuffer(
            self.renderbuffer(size, components, samples=samples, dtype=dtype),
            self.depth_renderbuffer(size, samples=samples),
        )

    def framebuffer(
        self,
        color_attachments: Any = (),
        depth_attachment: Optional[Union[Texture, Renderbuffer]] = None,
    ) -> 'Framebuffer':
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
        res = ComputeShader.__new__(ComputeShader)
        res.mglo, res._members, res._glo = self.mglo.compute_shader(source)

        res.ctx = self
        res.extra = None
        return res

    def sampler(
        self,
        repeat_x: bool = True,
        repeat_y: bool = True,
        repeat_z: bool = True,
        filter: Optional[Tuple[int, int]] = None,
        anisotropy: float = 1.0,
        compare_func: str = '?',
        border_color: Optional[Tuple[float, float, float, float]] = None,
        min_lod: float = -1000.0,
        max_lod: float = 1000.0,
        texture: Optional[Texture] = None,
    ) -> Sampler:
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
        self.mglo.clear_samplers(start, end)

    def core_profile_check(self) -> None:
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
        self.mglo.__enter__()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.mglo.__exit__(exc_type, exc_val, exc_tb)

    def release(self) -> None:
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
            self.mglo = InvalidObject()


def create_context(
    require: Optional[int] = None,
    standalone: bool = False,
    share: bool = False,
    **settings: Dict[str, Any],
) -> Context:
    if require is None:
        require = 330

    mode = 'standalone' if standalone is True else 'detect'
    if share is True:
        mode = 'share'

    from moderngl import mgl  # type: ignore

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
    **settings,
) -> 'Context':
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


def detect_format(
    program: Program,
    attributes: Any,
    mode: str = 'mgl',
) -> str:
    def fmt(attr: Any) -> Tuple[int, str]:
        # Translate shape format into attribute format
        mgl_fmt = {
            'd': 'f8',
            'I': 'u'
        }
        # moderngl attribute format uses f, i and u
        if mode == 'mgl':
            return attr.array_length * attr.dimension, mgl_fmt.get(attr.shape) or attr.shape
        # struct attribute format uses f, d, i and I
        elif mode == 'struct':
            return attr.array_length * attr.dimension, attr.shape
        else:
            raise ValueError("invalid format mode: {0}".format(mode))

    return ' '.join('%d%s' % fmt(program[a]) for a in attributes)
