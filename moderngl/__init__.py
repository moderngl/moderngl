import warnings
from collections import deque
from contextlib import contextmanager

from _moderngl import (
    Attribute,
    Error,
    InvalidObject,
    StorageBlock,
    Subroutine,
    Uniform,
    UniformBlock,
    Varying,
)
from _moderngl import parse_spv_inputs as _parse_spv

try:
    from moderngl import mgl
except ImportError:
    pass

__version__ = "5.12.0"

_GL_DEBUG_SOURCE_THIRD_PARTY = 0x8249
_GL_DEBUG_SOURCE_APPLICATION = 0x824A


def packager_imports():
    """some additional imports that code freezers (Pyinstaller,etc) should see."""

    import glcontext


class _store:
    default_context = None


# OpenGL Object type constants
_BUFFER = 0x82E0
_PROGRAM = 0x82E2
_QUERY = 0x82E3
_SAMPLER = 0x82E6
_TEXTURE = 0x1702
_VERTEX_ARRAY = 0x8074
_FRAMEBUFFER = 0x8D40
_RENDERBUFFER = 0x8D41


class Buffer:
    def __init__(self):
        self.mglo = None
        self._size = None
        self._dynamic = None
        self._glo = None
        self.ctx = None
        self.extra = None
        self._label = None  # Used only if no OpenGL label functions are available
        raise TypeError()

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    @property
    def size(self):
        return self.mglo.size()

    @property
    def dynamic(self):
        return self._dynamic

    @property
    def glo(self):
        return self._glo

    @property
    def label(self):
        if self.ctx.supports_labels:
            return self.ctx.mglo.get_label(_BUFFER, self._glo)
        else:
            return self._label

    @label.setter
    def label(self, value):
        if not (isinstance(value, str) or value is None):
            raise TypeError(
                f"Expected value to be a str or None, got {type(value).__name__}"
            )

        if self.ctx.supports_labels:
            self.ctx.mglo.set_label(_BUFFER, self._glo, value)
        else:
            self._label = value

    def write(self, data, offset=0):
        self.mglo.write(data, offset)

    def write_chunks(self, data, start, step, count):
        self.mglo.write_chunks(data, start, step, count)

    def read(self, size=-1, offset=0):
        return self.mglo.read(size, offset)

    def read_into(self, buffer, size=-1, offset=0, write_offset=0):
        return self.mglo.read_into(buffer, size, offset, write_offset)

    def read_chunks(self, chunk_size, start, step, count):
        return self.mglo.read_chunks(chunk_size, start, step, count)

    def read_chunks_into(self, buffer, chunk_size, start, step, count, write_offset=0):
        return self.mglo.read(buffer, chunk_size, start, step, count, write_offset)

    def clear(self, size=-1, offset=0, chunk=None):
        self.mglo.clear(size, offset, chunk)

    def bind_to_uniform_block(self, binding=0, offset=0, size=-1):
        self.mglo.bind_to_uniform_block(binding, offset, size)

    def bind_to_storage_buffer(self, binding=0, offset=0, size=-1):
        self.mglo.bind_to_storage_buffer(binding, offset, size)

    def orphan(self, size=-1):
        self.mglo.orphan(size)

    def release(self):
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
            self.mglo = InvalidObject()

    def bind(self, *attribs, layout=None):
        return (self, layout, *attribs)

    def assign(self, index):
        return (self, index)


class ConditionalRender:
    def __init__(self):
        self.mglo = None
        raise TypeError()

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
        self._label = None
        raise TypeError()

    def __enter__(self):
        self.mglo.begin()
        return self

    def __exit__(self, *args):
        self.mglo.end()

    @property
    def samples(self):
        return self.mglo.samples

    @property
    def primitives(self):
        return self.mglo.primitives

    @property
    def elapsed(self):
        return self.mglo.elapsed


class ComputeShader:
    def __init__(self):
        self.mglo = None
        self._members = {}
        self._glo = None
        self.ctx = None
        self.extra = None
        self._label = None
        raise TypeError()

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    def __getitem__(self, key):
        return self._members[key]

    def __setitem__(self, key, value):
        self._members[key].value = value

    def __iter__(self):
        yield from self._members

    @property
    def glo(self):
        return self._glo

    def run(self, group_x=1, group_y=1, group_z=1):
        return self.mglo.run(group_x, group_y, group_z)

    def run_indirect(self, buffer, offset=0):
        return self.mglo.run_indirect(buffer.mglo, offset)

    def get(self, key, default):
        return self._members.get(key, default)

    def release(self):
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
            self.mglo = InvalidObject()

    @property
    def label(self):
        if self.ctx.supports_labels:
            return self.ctx.mglo.get_label(_PROGRAM, self._glo)
        else:
            return self._label

    @label.setter
    def label(self, value):
        if not (isinstance(value, str) or value is None):
            raise TypeError(
                f"Expected value to be a str or None, got {type(value).__name__}"
            )

        if self.ctx.supports_labels:
            self.ctx.mglo.set_label(_PROGRAM, self._glo, value)
        else:
            self._label = value


class Framebuffer:
    def __init__(self):
        self.mglo = None
        self._color_attachments = None
        self._depth_attachment = None
        self._size = (None, None)
        self._samples = None
        self._glo = None
        self.ctx = None
        self._is_reference = None
        self.extra = None
        self._label = None
        raise TypeError()

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
    def viewport(self):
        return self.mglo.viewport

    @viewport.setter
    def viewport(self, value):
        x, y, w, h = value
        self.mglo.viewport = (int(x), int(y), int(w), int(h))

    @property
    def scissor(self):
        return self.mglo.scissor

    @scissor.setter
    def scissor(self, value):
        if value is None:
            self.mglo.scissor = None
        else:
            x, y, w, h = value
            self.mglo.scissor = (int(x), int(y), int(w), int(h))

    @property
    def color_mask(self):
        return self.mglo.color_mask

    @color_mask.setter
    def color_mask(self, value):
        self.mglo.color_mask = value

    @property
    def depth_mask(self):
        return self.mglo.depth_mask

    @depth_mask.setter
    def depth_mask(self, value):
        self.mglo.depth_mask = value

    @property
    def stencil_mask(self):
        return self.mglo.stencil_mask

    @stencil_mask.setter
    def stencil_mask(self, value):
        self.mglo.stencil_mask = value

    @property
    def width(self):
        return self._size[0]

    @property
    def height(self):
        return self._size[1]

    @property
    def size(self):
        return self._size

    @property
    def samples(self):
        return self._samples

    @property
    def bits(self):
        return self.mglo.bits

    @property
    def color_attachments(self):
        return self._color_attachments

    @property
    def depth_attachment(self):
        return self._depth_attachment

    @property
    def glo(self):
        return self._glo

    @property
    def label(self):
        if self.ctx.supports_labels:
            return self.ctx.mglo.get_label(_FRAMEBUFFER, self._glo)
        else:
            return self._label

    @label.setter
    def label(self, value):
        if not (isinstance(value, str) or value is None):
            raise TypeError(
                f"Expected value to be a str or None, got {type(value).__name__}"
            )

        if self.ctx.supports_labels:
            self.ctx.mglo.set_label(_FRAMEBUFFER, self._glo, value)
        else:
            self._label = value

    def clear(
        self,
        red=0.0,
        green=0.0,
        blue=0.0,
        alpha=0.0,
        depth=1.0,
        viewport=None,
        color=None,
        stencil=0
    ):
        if color is not None:
            red, green, blue, alpha, *_ = tuple(color) + (0.0, 0.0, 0.0, 0.0)

        if viewport is not None:
            viewport = tuple(viewport)

        self.mglo.clear(red, green, blue, alpha, depth, viewport, stencil)

    def use(self):
        self.ctx.fbo = self
        self.mglo.use()

    def read(
        self,
        viewport=None,
        components=3,
        attachment=0,
        alignment=1,
        dtype="f1",
        clamp=False,
    ):
        if viewport is None:
            viewport = (0, 0, self.width, self.height)
        if len(viewport) == 2:
            viewport = (0, 0, *viewport)
        res, mem = mgl.writable_bytes(
            mgl.expected_size(viewport[2], viewport[3], 1, components, alignment, dtype)
        )
        self.mglo.read_into(
            mem, viewport, components, attachment, alignment, clamp, dtype, 0
        )
        return res

    def read_into(
        self,
        buffer,
        viewport=None,
        components=3,
        attachment=0,
        alignment=1,
        dtype="f1",
        clamp=False,
        write_offset=0,
    ):
        if type(buffer) is Buffer:
            buffer = buffer.mglo

        return self.mglo.read_into(
            buffer,
            viewport,
            components,
            attachment,
            alignment,
            clamp,
            dtype,
            write_offset,
        )

    def release(self):
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
        self._attribute_locations = None
        self._attribute_types = None
        self.ctx = None
        self.extra = None
        self._label = None
        raise TypeError()

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    def __getitem__(self, key):
        return self._members[key]

    def __setitem__(self, key, value):
        self._members[key].value = value

    def __iter__(self):
        yield from self._members

    @property
    def is_transform(self):
        return self._is_transform

    @property
    def geometry_input(self):
        return self._geom[0]

    @property
    def geometry_output(self):
        return self._geom[1]

    @property
    def geometry_vertices(self):
        return self._geom[2]

    @property
    def subroutines(self):
        return self._subroutines

    @property
    def glo(self):
        return self._glo

    @property
    def label(self):
        if self.ctx.supports_labels:
            return self.ctx.mglo.get_label(_PROGRAM, self._glo)
        else:
            return self._label

    @label.setter
    def label(self, value):
        if not (isinstance(value, str) or value is None):
            raise TypeError(
                f"Expected value to be a str or None, got {type(value).__name__}"
            )

        if self.ctx.supports_labels:
            self.ctx.mglo.set_label(_PROGRAM, self._glo, value)
        else:
            self._label = value

    def get(self, key, default):
        return self._members.get(key, default)

    def draw_mesh_tasks(self, first, count):
        return self.mglo.draw_mesh_tasks(first, count)

    def draw_mesh_tasks_indirect(self, buffer, offset=0, drawcount=1, stride=0):
        return self.mglo.draw_mesh_tasks_indirect(buffer.mglo, offset, drawcount, stride)

    def draw_mesh_tasks_indirect_count(self, buffer, offset, drawcount_offset, maxdrawcount, stride=0):
        return self.mglo.draw_mesh_tasks_indirect_count(buffer.mglo, offset, drawcount_offset, maxdrawcount, stride)

    def release(self):
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
        self._label = None
        raise TypeError()

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    @property
    def width(self):
        return self._size[0]

    @property
    def height(self):
        return self._size[1]

    @property
    def size(self):
        return self._size

    @property
    def samples(self):
        return self._samples

    @property
    def components(self):
        return self._components

    @property
    def depth(self):
        return self._depth

    @property
    def dtype(self):
        return self._dtype

    @property
    def glo(self):
        return self._glo

    @property
    def label(self):
        if self.ctx.supports_labels:
            return self.ctx.mglo.get_label(_RENDERBUFFER, self._glo)
        else:
            return self._label

    @label.setter
    def label(self, value):
        if not (isinstance(value, str) or value is None):
            raise TypeError(
                f"Expected value to be a str or None, got {type(value).__name__}"
            )

        if self.ctx.supports_labels:
            self.ctx.mglo.set_label(_RENDERBUFFER, self._glo, value)
        else:
            self._label = value

    def release(self):
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
        self._label = None
        raise TypeError()

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    def use(self, location=0):
        if self.texture is not None:
            self.texture.use(location)
        self.mglo.use(location)

    def clear(self, location=0):
        self.mglo.clear(location)

    def release(self):
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
            self.mglo = InvalidObject()

    @property
    def repeat_x(self):
        return self.mglo.repeat_x

    @repeat_x.setter
    def repeat_x(self, value):
        self.mglo.repeat_x = value

    @property
    def repeat_y(self):
        return self.mglo.repeat_y

    @repeat_y.setter
    def repeat_y(self, value):
        self.mglo.repeat_y = value

    @property
    def repeat_z(self):
        return self.mglo.repeat_z

    @repeat_z.setter
    def repeat_z(self, value):
        self.mglo.repeat_z = value

    @property
    def filter(self):
        return self.mglo.filter

    @filter.setter
    def filter(self, value):
        self.mglo.filter = value

    @property
    def compare_func(self):
        return self.mglo.compare_func

    @compare_func.setter
    def compare_func(self, value):
        self.mglo.compare_func = value

    @property
    def anisotropy(self):
        return self.mglo.anisotropy

    @anisotropy.setter
    def anisotropy(self, value):
        self.mglo.anisotropy = value

    @property
    def border_color(self):
        return self.mglo.border_color

    @border_color.setter
    def border_color(self, value):
        self.mglo.border_color = value

    @property
    def min_lod(self):
        return self.mglo.min_lod

    @min_lod.setter
    def min_lod(self, value):
        self.mglo.min_lod = value

    @property
    def max_lod(self):
        return self.mglo.max_lod

    @max_lod.setter
    def max_lod(self, value):
        self.mglo.max_lod = value

    @property
    def label(self):
        if self.ctx.supports_labels:
            return self.ctx.mglo.get_label(_SAMPLER, self._glo)
        else:
            return self._label

    @label.setter
    def label(self, value):
        if not (isinstance(value, str) or value is None):
            raise TypeError(
                f"Expected value to be a str or None, got {type(value).__name__}"
            )

        if self.ctx.supports_labels:
            self.ctx.mglo.set_label(_SAMPLER, self._glo, value)
        else:
            self._label = value

    def assign(self, index):
        return (self, index)


class Scope:
    def __init__(self):
        self.mglo = None
        self.ctx = None
        self._framebuffer = None
        self._textures = None
        self._uniform_buffers = None
        self._storage_buffers = None
        self._samplers = None
        self.extra = None
        raise TypeError()

    def __enter__(self):
        self.mglo.begin()
        return self

    def __exit__(self, *args):
        self.mglo.end()

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    def release(self):
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
        self._use_stencilbuffer = None
        self._glo = None
        self.ctx = None
        self.extra = None
        self._label = None
        raise TypeError()

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    @property
    def repeat_x(self):
        return self.mglo.repeat_x

    @repeat_x.setter
    def repeat_x(self, value):
        self.mglo.repeat_x = value

    @property
    def repeat_y(self):
        return self.mglo.repeat_y

    @repeat_y.setter
    def repeat_y(self, value):
        self.mglo.repeat_y = value

    @property
    def filter(self):
        return self.mglo.filter

    @filter.setter
    def filter(self, value):
        self.mglo.filter = value

    @property
    def anisotropy(self):
        return self.mglo.anisotropy

    @anisotropy.setter
    def anisotropy(self, value):
        self.mglo.anisotropy = value

    @property
    def swizzle(self):
        return self.mglo.swizzle

    @swizzle.setter
    def swizzle(self, value):
        self.mglo.swizzle = value

    @property
    def compare_func(self):
        return self.mglo.compare_func

    @compare_func.setter
    def compare_func(self, value):
        self.mglo.compare_func = value

    @property
    def width(self):
        return self._size[0]

    @property
    def height(self):
        return self._size[1]

    @property
    def size(self):
        return self._size

    @property
    def components(self):
        return self._components

    @property
    def samples(self):
        return self._samples

    @property
    def dtype(self):
        return self._dtype

    @property
    def depth(self):
        return self._depth

    @property
    def use_stencil_buffer(self):
        return self._use_stencilbuffer

    @property
    def glo(self):
        return self._glo

    @property
    def label(self):
        if self.ctx.supports_labels:
            return self.ctx.mglo.get_label(_TEXTURE, self._glo)
        else:
            return self._label

    @label.setter
    def label(self, value):
        if not (isinstance(value, str) or value is None):
            raise TypeError(
                f"Expected value to be a str or None, got {type(value).__name__}"
            )

        if self.ctx.supports_labels:
            self.ctx.mglo.set_label(_TEXTURE, self._glo, value)
        else:
            self._label = value

    def read(self, level=0, alignment=1):
        return self.mglo.read(level, alignment)

    def read_into(self, buffer, level=0, alignment=1, write_offset=0):
        if type(buffer) is Buffer:
            buffer = buffer.mglo

        return self.mglo.read_into(buffer, level, alignment, write_offset)

    def write(self, data, viewport=None, level=0, alignment=1):
        if type(data) is Buffer:
            data = data.mglo

        self.mglo.write(data, viewport, level, alignment)

    def build_mipmaps(self, base=0, max_level=1000):
        self.mglo.build_mipmaps(base, max_level)

    def use(self, location=0):
        self.mglo.use(location)

    def bind_to_image(self, unit, read=True, write=True, level=0, format=0):
        self.mglo.bind(unit, read, write, level, format)

    def get_handle(self, resident=True):
        return self.mglo.get_handle(resident)

    def release(self):
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
        self._label = None
        raise TypeError()

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    @property
    def repeat_x(self):
        return self.mglo.repeat_x

    @repeat_x.setter
    def repeat_x(self, value):
        self.mglo.repeat_x = value

    @property
    def repeat_y(self):
        return self.mglo.repeat_y

    @repeat_y.setter
    def repeat_y(self, value):
        self.mglo.repeat_y = value

    @property
    def repeat_z(self):
        return self.mglo.repeat_z

    @repeat_z.setter
    def repeat_z(self, value):
        self.mglo.repeat_z = value

    @property
    def filter(self):
        return self.mglo.filter

    @filter.setter
    def filter(self, value):
        self.mglo.filter = value

    @property
    def swizzle(self):
        return self.mglo.swizzle

    @swizzle.setter
    def swizzle(self, value):
        self.mglo.swizzle = value

    @property
    def width(self):
        return self._size[0]

    @property
    def height(self):
        return self._size[1]

    @property
    def depth(self):
        return self._size[2]

    @property
    def size(self):
        return self._size

    @property
    def components(self):
        return self._components

    @property
    def dtype(self):
        return self._dtype

    @property
    def glo(self):
        return self._glo

    @property
    def label(self):
        if self.ctx.supports_labels:
            return self.ctx.mglo.get_label(_TEXTURE, self._glo)
        else:
            return self._label

    @label.setter
    def label(self, value):
        if not (isinstance(value, str) or value is None):
            raise TypeError(
                f"Expected value to be a str or None, got {type(value).__name__}"
            )

        if self.ctx.supports_labels:
            self.ctx.mglo.set_label(_TEXTURE, self._glo, value)
        else:
            self._label = value

    def read(self, alignment=1):
        return self.mglo.read(alignment)

    def read_into(self, buffer, alignment=1, write_offset=0):
        if type(buffer) is Buffer:
            buffer = buffer.mglo

        return self.mglo.read_into(buffer, alignment, write_offset)

    def write(self, data, viewport=None, alignment=1):
        if type(data) is Buffer:
            data = data.mglo

        self.mglo.write(data, viewport, alignment)

    def build_mipmaps(self, base=0, max_level=1000):
        self.mglo.build_mipmaps(base, max_level)

    def use(self, location=0):
        self.mglo.use(location)

    def bind_to_image(self, unit, read=True, write=True, level=0, format=0):
        self.mglo.bind(unit, read, write, level, format)

    def get_handle(self, resident=True):
        return self.mglo.get_handle(resident)

    def release(self):
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
            self.mglo = InvalidObject()


class TextureCube:
    def __init__(self):
        self.mglo = None
        self._size = (None, None)
        self._components = None
        self._depth = None
        self._dtype = None
        self._glo = None
        self.ctx = None
        self.extra = None
        self._label = None
        raise TypeError()

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    @property
    def size(self):
        return self._size

    @property
    def components(self):
        return self._components

    @property
    def dtype(self):
        return self._dtype

    @property
    def filter(self):
        return self.mglo.filter

    @filter.setter
    def filter(self, value):
        self.mglo.filter = value

    @property
    def swizzle(self):
        return self.mglo.swizzle

    @swizzle.setter
    def swizzle(self, value):
        self.mglo.swizzle = value

    @property
    def compare_func(self):
        return self.mglo.compare_func

    @compare_func.setter
    def compare_func(self, value):
        self.mglo.compare_func = value

    @property
    def anisotropy(self):
        return self.mglo.anisotropy

    @anisotropy.setter
    def anisotropy(self, value):
        self.mglo.anisotropy = value

    @property
    def depth(self):
        return self._depth

    @property
    def glo(self):
        return self._glo

    @property
    def label(self):
        if self.ctx.supports_labels:
            return self.ctx.mglo.get_label(_TEXTURE, self._glo)
        else:
            return self._label

    @label.setter
    def label(self, value):
        if not (isinstance(value, str) or value is None):
            raise TypeError(
                f"Expected value to be a str or None, got {type(value).__name__}"
            )

        if self.ctx.supports_labels:
            self.ctx.mglo.set_label(_TEXTURE, self._glo, value)
        else:
            self._label = value

    def read(self, face, alignment=1):
        return self.mglo.read(face, alignment)

    def read_into(self, buffer, face, alignment=1, write_offset=0):
        if type(buffer) is Buffer:
            buffer = buffer.mglo

        return self.mglo.read_into(buffer, face, alignment, write_offset)

    def write(self, face, data, viewport=None, alignment=1):
        if type(data) is Buffer:
            data = data.mglo

        self.mglo.write(face, data, viewport, alignment)

    def build_mipmaps(self, base=0, max_level=1000):
        self.mglo.build_mipmaps(base, max_level)

    def use(self, location=0):
        self.mglo.use(location)

    def bind_to_image(self, unit, read=True, write=True, level=0, format=0):
        self.mglo.bind(unit, read, write, level, format)

    def get_handle(self, resident=True):
        return self.mglo.get_handle(resident)

    def release(self):
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
        self._label = None
        raise TypeError()

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    @property
    def repeat_x(self):
        return self.mglo.repeat_x

    @repeat_x.setter
    def repeat_x(self, value):
        self.mglo.repeat_x = value

    @property
    def repeat_y(self):
        return self.mglo.repeat_y

    @repeat_y.setter
    def repeat_y(self, value):
        self.mglo.repeat_y = value

    @property
    def filter(self):
        return self.mglo.filter

    @filter.setter
    def filter(self, value):
        self.mglo.filter = value

    @property
    def swizzle(self):
        return self.mglo.swizzle

    @swizzle.setter
    def swizzle(self, value):
        self.mglo.swizzle = value

    @property
    def anisotropy(self):
        return self.mglo.anisotropy

    @anisotropy.setter
    def anisotropy(self, value):
        self.mglo.anisotropy = value

    @property
    def width(self):
        return self._size[0]

    @property
    def height(self):
        return self._size[1]

    @property
    def layers(self):
        return self._size[2]

    @property
    def size(self):
        return self._size

    @property
    def components(self):
        return self._components

    @property
    def dtype(self):
        return self._dtype

    @property
    def glo(self):
        return self._glo

    @property
    def label(self):
        if self.ctx.supports_labels:
            return self.ctx.mglo.get_label(_TEXTURE, self._glo)
        else:
            return self._label

    @label.setter
    def label(self, value):
        if not (isinstance(value, str) or value is None):
            raise TypeError(
                f"Expected value to be a str or None, got {type(value).__name__}"
            )

        if self.ctx.supports_labels:
            self.ctx.mglo.set_label(_TEXTURE, self._glo, value)
        else:
            self._label = value

    def read(self, alignment=1):
        return self.mglo.read(alignment)

    def read_into(self, buffer, alignment=1, write_offset=0):
        if type(buffer) is Buffer:
            buffer = buffer.mglo

        return self.mglo.read_into(buffer, alignment, write_offset)

    def write(self, data, viewport=None, alignment=1):
        if type(data) is Buffer:
            data = data.mglo

        self.mglo.write(data, viewport, alignment)

    def build_mipmaps(self, base=0, max_level=1000):
        self.mglo.build_mipmaps(base, max_level)

    def use(self, location=0):
        self.mglo.use(location)

    def bind_to_image(self, unit, read=True, write=True, level=0, format=0):
        self.mglo.bind(unit, read, write, level, format)

    def get_handle(self, resident=True):
        return self.mglo.get_handle(resident)

    def release(self):
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
        self._label = None
        raise TypeError()

    def __del__(self):
        if not hasattr(self, "ctx"):
            return

        if self.ctx.gc_mode == "auto":
            self.release()
        elif self.ctx.gc_mode == "context_gc":
            self.ctx.objects.append(self.mglo)

    @property
    def mode(self):
        return self._mode

    @mode.setter
    def mode(self, value):
        self._mode = value

    @property
    def program(self):
        return self._program

    @property
    def index_buffer(self):
        return self._index_buffer

    @property
    def index_element_size(self):
        return self._index_element_size

    @property
    def vertices(self):
        return self.mglo.vertices

    @vertices.setter
    def vertices(self, value):
        self.mglo.vertices = int(value)

    @property
    def instances(self):
        return self.mglo.instances

    @instances.setter
    def instances(self, value):
        self.mglo.instances = int(value)

    @property
    def subroutines(self):
        return self.mglo.subroutines

    @subroutines.setter
    def subroutines(self, value):
        self.mglo.subroutines = tuple(value)

    @property
    def glo(self):
        return self._glo

    @property
    def label(self):
        if self.ctx.supports_labels:
            return self.ctx.mglo.get_label(_VERTEX_ARRAY, self._glo)
        else:
            return self._label

    @label.setter
    def label(self, value):
        if not (isinstance(value, str) or value is None):
            raise TypeError(
                f"Expected value to be a str or None, got {type(value).__name__}"
            )

        if self.ctx.supports_labels:
            self.ctx.mglo.set_label(_VERTEX_ARRAY, self._glo, value)
        else:
            self._label = value

    def render(self, mode=None, vertices=-1, first=0, instances=-1):
        if mode is None:
            mode = self._mode

        if self.scope:
            with self.scope:
                self.mglo.render(mode, vertices, first, instances)
        else:
            self.mglo.render(mode, vertices, first, instances)

    def render_indirect(self, buffer, mode=None, count=-1, first=0):
        if mode is None:
            mode = self._mode

        if self.scope:
            with self.scope:
                self.mglo.render_indirect(buffer.mglo, mode, count, first)
        else:
            self.mglo.render_indirect(buffer.mglo, mode, count, first)

    def transform(
        self, buffer, mode=None, vertices=-1, first=0, instances=-1, buffer_offset=0
    ):
        if mode is None:
            mode = self._mode

        if isinstance(buffer, (list, tuple)):
            outputs = [buf.mglo for buf in buffer]
        else:
            outputs = [buffer.mglo]

        if self.scope:
            with self.scope:
                self.mglo.transform(
                    outputs, mode, vertices, first, instances, buffer_offset
                )
        else:
            self.mglo.transform(
                outputs, mode, vertices, first, instances, buffer_offset
            )

    def bind(
        self,
        attribute,
        cls,
        buffer,
        fmt,
        offset=0,
        stride=0,
        divisor=0,
        normalize=False,
    ):
        self.mglo.bind(
            attribute, cls, buffer.mglo, fmt, offset, stride, divisor, normalize
        )

    def release(self):
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
    STENCIL_TEST = 32

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

    # Stencil op constants

    KEEP = 0x1E00
    REPLACE = 0x1E01
    INCR = 0x1E02
    INCR_WRAP = 0x8507
    DECR = 0x1E03
    DECR_WRAP = 0x8508
    INVERT = 0x150A

    # Provoking vertex

    FIRST_VERTEX_CONVENTION = 0x8E4D
    LAST_VERTEX_CONVENTION = 0x8E4E

    # Memory barrier

    VERTEX_ATTRIB_ARRAY_BARRIER_BIT = 0x00000001
    ELEMENT_ARRAY_BARRIER_BIT = 0x00000002
    UNIFORM_BARRIER_BIT = 0x00000004
    TEXTURE_FETCH_BARRIER_BIT = 0x00000008
    SHADER_IMAGE_ACCESS_BARRIER_BIT = 0x00000020
    COMMAND_BARRIER_BIT = 0x00000040
    PIXEL_BUFFER_BARRIER_BIT = 0x00000080
    TEXTURE_UPDATE_BARRIER_BIT = 0x00000100
    BUFFER_UPDATE_BARRIER_BIT = 0x00000200
    FRAMEBUFFER_BARRIER_BIT = 0x00000400
    TRANSFORM_FEEDBACK_BARRIER_BIT = 0x00000800
    ATOMIC_COUNTER_BARRIER_BIT = 0x00001000
    SHADER_STORAGE_BARRIER_BIT = 0x00002000
    ALL_BARRIER_BITS = 0xFFFFFFFF

    def __init__(self):
        self.mglo = None
        self._screen = None
        self._info = None
        self._extensions = None
        self.version_code = None
        self.fbo = None
        self.extra = None
        self._gc_mode = None
        self._objects = deque()
        raise TypeError()

    def __del__(self):
        if hasattr(self, "_gc_mode") and self._gc_mode == "auto":
            self.release()

    @property
    def gc_mode(self):
        return self._gc_mode

    @gc_mode.setter
    def gc_mode(self, value):
        if value not in self._valid_gc_modes:
            raise ValueError("Valid  gc modes:", self._valid_gc_modes)

        self._gc_mode = value

    @property
    def objects(self):
        return self._objects

    def gc(self):
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
    def line_width(self):
        return self.mglo.line_width

    @line_width.setter
    def line_width(self, value):
        self.mglo.line_width = value

    @property
    def point_size(self):
        return self.mglo.point_size

    @point_size.setter
    def point_size(self, value):
        self.mglo.point_size = value

    @property
    def depth_func(self):
        raise NotImplementedError()

    @depth_func.setter
    def depth_func(self, value):
        self.mglo.depth_func = value

    @property
    def stencil_func(self):
        raise NotImplementedError()

    @stencil_func.setter
    def stencil_func(self, value):
        self.mglo.stencil_func = tuple(value)

    @property
    def stencil_op(self):
        raise NotImplementedError()

    @stencil_op.setter
    def stencil_op(self, value):
        self.mglo.stencil_op = tuple(value)

    @property
    def blend_func(self):
        raise NotImplementedError()

    @blend_func.setter
    def blend_func(self, value):
        self.mglo.blend_func = tuple(value)

    @property
    def blend_equation(self):
        raise NotImplementedError()

    @blend_equation.setter
    def blend_equation(self, value):
        if not isinstance(value, tuple):
            self.mglo.blend_equation = tuple([value])
        else:
            self.mglo.blend_equation = tuple(value)

    @property
    def depth_clamp_range(self):
        raise NotImplementedError()

    @depth_clamp_range.setter
    def depth_clamp_range(self, value):
        self.mglo.depth_clamp_range = value

    @property
    def multisample(self):
        raise NotImplementedError()

    @multisample.setter
    def multisample(self, value):
        self.mglo.multisample = value

    @property
    def provoking_vertex(self):
        raise NotImplementedError()

    @provoking_vertex.setter
    def provoking_vertex(self, value):
        self.mglo.provoking_vertex = value

    @property
    def polygon_offset(self):
        return self.mglo.polygon_offset

    @polygon_offset.setter
    def polygon_offset(self, value):
        factor, units = value
        self.mglo.polygon_offset = (float(factor), float(units))

    @property
    def viewport(self):
        return self.mglo.fbo.viewport

    @viewport.setter
    def viewport(self, value):
        x, y, w, h = value
        self.mglo.fbo.viewport = (int(x), int(y), int(w), int(h))

    @property
    def scissor(self):
        return self.mglo.fbo.scissor

    @scissor.setter
    def scissor(self, value):
        if value is None:
            self.mglo.fbo.scissor = None
        else:
            x, y, w, h = value
            self.mglo.fbo.scissor = (int(x), int(y), int(w), int(h))

    @property
    def max_samples(self):
        return self.mglo.max_samples

    @property
    def max_integer_samples(self):
        return self.mglo.max_integer_samples

    @property
    def max_texture_units(self):
        return self.mglo.max_texture_units

    @property
    def max_label_length(self):
        return self.mglo.max_label_length

    @property
    def max_debug_message_length(self):
        return self.mglo.max_debug_message_length

    @property
    def max_debug_group_stack_depth(self):
        return self.mglo.max_debug_group_stack_depth

    @property
    def default_texture_unit(self):
        return self.mglo.default_texture_unit

    @default_texture_unit.setter
    def default_texture_unit(self, value):
        self.mglo.default_texture_unit = value

    @property
    def max_anisotropy(self):
        return self.mglo.max_anisotropy

    @property
    def screen(self):
        return self._screen

    @property
    def wireframe(self):
        return self.mglo.wireframe

    @wireframe.setter
    def wireframe(self, value):
        self.mglo.wireframe = value

    @property
    def front_face(self):
        return self.mglo.front_face

    @front_face.setter
    def front_face(self, value):
        self.mglo.front_face = str(value)

    @property
    def cull_face(self):
        return self.mglo.cull_face

    @cull_face.setter
    def cull_face(self, value):
        self.mglo.cull_face = str(value)

    @property
    def patch_vertices(self):
        return self.mglo.patch_vertices

    @patch_vertices.setter
    def patch_vertices(self, value):
        self.mglo.patch_vertices = value

    @property
    def error(self):
        return self.mglo.error

    @property
    def extensions(self):
        if self._extensions is None:
            self._extensions = self.mglo.extensions

        return self._extensions

    @property
    def supports_labels(self):
        if self.version_code >= 430:
            return True

        if "GL_KHR_debug" in self.extensions:
            return True

        if "GL_EXT_debug_label" in self.extensions:
            return True

        return False

    @property
    def supports_debug_scopes(self):
        if self.version_code >= 430:
            return True

        if "GL_KHR_debug" in self.extensions:
            return True

        if "GL_EXT_debug_marker" in self.extensions:
            return True

        return False

    @property
    def info(self):
        if self._info is None:
            self._info = self.mglo.info

        return self._info

    @property
    def includes(self):
        return self.mglo.includes

    def clear(
        self,
        red=0.0,
        green=0.0,
        blue=0.0,
        alpha=0.0,
        depth=1.0,
        viewport=None,
        color=None,
        stencil=0
    ):
        if color is not None:
            red, green, blue, alpha, *_ = tuple(color) + (0.0, 0.0, 0.0, 0.0)

        self.mglo.fbo.clear(red, green, blue, alpha, depth, viewport, stencil)

    def enable_only(self, flags):
        self.mglo.enable_only(flags)

    def enable(self, flags):
        self.mglo.enable(flags)

    def disable(self, flags):
        self.mglo.disable(flags)

    def enable_direct(self, enum):
        self.mglo.enable_direct(enum)

    def disable_direct(self, enum):
        self.mglo.disable_direct(enum)

    def finish(self):
        self.mglo.finish()

    def copy_buffer(
        self, dst: Buffer, src: Buffer, size=-1, read_offset=0, write_offset=0
    ):
        self.mglo.copy_buffer(dst.mglo, src.mglo, size, read_offset, write_offset)

    def copy_framebuffer(self, dst, src):
        self.mglo.copy_framebuffer(dst.mglo, src.mglo)

    def detect_framebuffer(self, glo=None):
        res = Framebuffer.__new__(Framebuffer)
        res.mglo, res._size, res._samples, res._glo = self.mglo.detect_framebuffer(glo)
        res._color_attachments = None
        res._depth_attachment = None
        res.ctx = self
        res._is_reference = True
        res.extra = None
        return res

    def buffer(self, data=None, reserve=0, dynamic=False):
        if type(reserve) is str:
            reserve = mgl.strsize(reserve)

        res = Buffer.__new__(Buffer)
        res.mglo, res._size, res._glo = self.mglo.buffer(data, reserve, dynamic)
        res._dynamic = dynamic
        res.ctx = self
        res.extra = None
        return res

    def external_buffer(self, glo, size):
        res = Buffer.__new__(Buffer)
        res.mglo, res._size, res._glo = self.mglo.external_buffer(glo, size)
        res._dynamic = False
        res.ctx = self
        res.extra = None
        return res

    def external_texture(self, glo, size, components, samples, dtype):
        res = Texture.__new__(Texture)
        res.mglo, res._glo = self.mglo.external_texture(
            glo, size, components, samples, dtype
        )
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
        size,
        components,
        data=None,
        samples=0,
        alignment=1,
        dtype="f1",
        internal_format=None,
        renderbuffer=False,
    ):
        res = Texture.__new__(Texture)
        res.mglo, res._glo = self.mglo.texture(
            size,
            components,
            data,
            samples,
            alignment,
            dtype,
            internal_format or 0,
            renderbuffer,
        )
        res._size = size
        res._components = components
        res._samples = samples
        res._dtype = dtype
        res._depth = False
        res.ctx = self
        res.extra = None
        return res

    def texture_array(self, size, components, data=None, alignment=1, dtype="f1"):
        res = TextureArray.__new__(TextureArray)
        res.mglo, res._glo = self.mglo.texture_array(
            size, components, data, alignment, dtype
        )
        res._size = size
        res._components = components
        res._dtype = dtype
        res.ctx = self
        res.extra = None
        return res

    def texture3d(self, size, components, data=None, alignment=1, dtype="f1"):
        res = Texture3D.__new__(Texture3D)
        res._size = size
        res._components = components
        res._dtype = dtype
        res.mglo, res._glo = self.mglo.texture3d(
            size, components, data, alignment, dtype
        )
        res.ctx = self
        res.extra = None
        return res

    def texture_cube(
        self, size, components, data=None, alignment=1, dtype="f1", internal_format=None
    ):
        res = TextureCube.__new__(TextureCube)
        res.mglo, res._glo = self.mglo.texture_cube(
            size, components, data, alignment, dtype, internal_format or 0
        )
        res._size = size
        res._components = components
        res._dtype = dtype
        res.ctx = self
        res.extra = None
        return res

    def depth_texture(
        self, size, data=None, samples=0, alignment=4, renderbuffer=False, use_stencilbuffer=False
    ):
        res = Texture.__new__(Texture)
        res.mglo, res._glo = self.mglo.depth_texture(
            size, data, samples, alignment, renderbuffer, use_stencilbuffer
        )
        res._size = size
        res._components = 1
        res._samples = samples
        res._dtype = "f4"
        res._depth = True
        res.ctx = self
        res.extra = None
        return res

    def depth_texture_cube(self, size, data=None, alignment=4):
        res = TextureCube.__new__(TextureCube)
        res.mglo, res._glo = self.mglo.depth_texture_cube(size, data, alignment)
        res._size = size
        res._components = 1
        res._dtype = "f4"
        res._depth = True
        res.ctx = self
        res.extra = None
        return res

    def vertex_array(self, *args, **kwargs):
        if len(args) > 2 and type(args[1]) is Buffer:
            return self.simple_vertex_array(*args, **kwargs)
        return self._vertex_array(*args, **kwargs)

    def _vertex_array(
        self,
        program,
        content,
        index_buffer=None,
        index_element_size=4,
        skip_errors=False,
        mode=None,
    ):
        locations = program._attribute_locations
        types = program._attribute_types
        index_buffer_mglo = None if index_buffer is None else index_buffer.mglo
        mgl_content = []

        for buffer, layout, *attribs in content:
            if layout is None:
                layout = detect_format(program, attribs)
            if skip_errors:
                attribs = [
                    (
                        types.get(x, None)
                        if type(x) is int
                        else types.get(locations.get(x, -1), None)
                    )
                    for x in attribs
                ]
            else:
                attribs = [
                    types[x] if type(x) is int else types[locations[x]] for x in attribs
                ]
            mgl_content.append((buffer.mglo, layout, *attribs))

        res = VertexArray.__new__(VertexArray)
        res.mglo, res._glo = self.mglo.vertex_array(
            program.mglo,
            tuple(mgl_content),
            index_buffer_mglo,
            index_element_size,
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
        program,
        buffer,
        *attributes,
        index_buffer=None,
        index_element_size=4,
        mode=None,
    ):
        if type(buffer) is list:
            raise SyntaxError("Change simple_vertex_array to vertex_array")

        content = [(buffer, detect_format(program, attributes)) + attributes]
        return self._vertex_array(
            program, content, index_buffer, index_element_size, mode=mode
        )

    def program(
        self,
        vertex_shader=None,
        fragment_shader=None,
        geometry_shader=None,
        tess_control_shader=None,
        tess_evaluation_shader=None,
        task_shader=None,
        mesh_shader=None,
        varyings=(),
        fragment_outputs=None,
        attributes=None,
        varyings_capture_mode="interleaved",
    ):
        if varyings_capture_mode not in ("interleaved", "separate"):
            raise ValueError("varyings_capture_mode must be interleaved or separate")

        if type(varyings) is str:
            varyings = (varyings,)

        varyings = tuple(varyings)

        if fragment_outputs is None:
            fragment_outputs = {}

        if isinstance(vertex_shader, str):
            vertex_shader = vertex_shader.strip()

        if isinstance(fragment_shader, str):
            fragment_shader = fragment_shader.strip()

        res = Program.__new__(Program)
        res.mglo, _members, res._subroutines, res._geom, res._glo = self.mglo.program(
            vertex_shader,
            fragment_shader,
            geometry_shader,
            tess_control_shader,
            tess_evaluation_shader,
            None,
            task_shader,
            mesh_shader,
            varyings,
            fragment_outputs,
            varyings_capture_mode == "interleaved",
        )
        res._members, res._attribute_locations, res._attribute_types = _members

        if (
            isinstance(vertex_shader, bytes)
            and int.from_bytes(vertex_shader[:4], "little") == 0x07230203
        ):
            res._attribute_types = _parse_spv(res._glo, vertex_shader)
            for info in res._attribute_types.values():
                res._attribute_locations[info.name] = info.location

        if attributes is not None:
            res._attribute_locations = {}
            for i, name in enumerate(attributes):
                res._attribute_locations[name] = i

        res._is_transform = fragment_shader is None
        res.ctx = self
        res.extra = None
        return res

    def query(self, samples=False, any_samples=False, time=False, primitives=False):
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
        framebuffer=None,
        enable_only=None,
        textures=(),
        uniform_buffers=(),
        storage_buffers=(),
        samplers=(),
        enable=None,
    ):
        if enable is not None:
            enable_only = enable

        if framebuffer is None:
            framebuffer = self.screen
            if framebuffer is None:
                raise RuntimeError("A framebuffer must be specified")

        mgl_textures = tuple((tex.mglo, idx) for tex, idx in textures)
        mgl_uniform_buffers = tuple((buf.mglo, idx) for buf, idx in uniform_buffers)
        mgl_storage_buffers = tuple((buf.mglo, idx) for buf, idx in storage_buffers)

        res = Scope.__new__(Scope)
        res.mglo = self.mglo.scope(
            framebuffer.mglo,
            enable_only,
            mgl_textures,
            mgl_uniform_buffers,
            mgl_storage_buffers,
            samplers,
        )
        res.ctx = self
        res._framebuffer = framebuffer
        res._textures = textures
        res._uniform_buffers = uniform_buffers
        res._storage_buffers = storage_buffers
        res._samplers = samplers
        res.extra = None
        return res

    def simple_framebuffer(self, size, components=4, samples=0, dtype="f1"):
        return self.framebuffer(
            self.renderbuffer(size, components, samples=samples, dtype=dtype),
            self.depth_renderbuffer(size, samples=samples),
        )

    def framebuffer(self, color_attachments=(), depth_attachment=None):
        if (
            type(color_attachments) is Texture
            or type(color_attachments) is Renderbuffer
        ):
            color_attachments = (color_attachments,)

        ca_mglo = tuple(x.mglo for x in color_attachments)
        da_mglo = depth_attachment.mglo if depth_attachment else None

        res = Framebuffer.__new__(Framebuffer)
        res.mglo, res._size, res._samples, res._glo = self.mglo.framebuffer(
            ca_mglo, da_mglo
        )
        res._color_attachments = tuple(color_attachments)
        res._depth_attachment = depth_attachment
        res.ctx = self
        res._is_reference = False
        res.extra = None
        return res

    def empty_framebuffer(self, size, layers=0, samples=0):
        res = Framebuffer.__new__(Framebuffer)
        res.mglo, res._size, res._samples, res._glo = self.mglo.empty_framebuffer(
            size, layers, samples
        )
        res._color_attachments = ()
        res._depth_attachment = None
        res.ctx = self
        res._is_reference = False
        res.extra = None
        return res

    def renderbuffer(self, size, components=4, samples=0, dtype="f1"):
        res = Renderbuffer.__new__(Renderbuffer)
        res.mglo, res._glo = self.mglo.texture(
            size, components, None, samples, 1, dtype, 0, True
        )
        res._size = size
        res._components = components
        res._samples = samples
        res._dtype = dtype
        res._depth = False
        res.ctx = self
        res.extra = None
        return res

    def depth_renderbuffer(self, size, samples=0):
        res = Renderbuffer.__new__(Renderbuffer)
        res.mglo, res._glo = self.mglo.depth_texture(size, None, samples, 1, True, False)
        res._size = size
        res._components = 1
        res._samples = samples
        res._dtype = "f4"
        res._depth = True
        res.ctx = self
        res.extra = None
        return res

    def compute_shader(self, source):
        res = ComputeShader.__new__(ComputeShader)
        res.mglo, _members, _, _, res._glo = self.mglo.program(
            None,
            None,
            None,
            None,
            None,
            source,
            None,
            None,
            (),
            {},
            False,
        )
        res._members = _members[0]

        res.ctx = self
        res.extra = None
        return res

    def sampler(
        self,
        repeat_x=True,
        repeat_y=True,
        repeat_z=True,
        filter=None,
        anisotropy=1.0,
        compare_func="?",
        border_color=None,
        min_lod=-1000.0,
        max_lod=1000.0,
        texture=None,
    ):
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

    def memory_barrier(self, barriers=ALL_BARRIER_BITS, by_region=False):
        self.mglo.memory_barrier(barriers, by_region)

    def clear_samplers(self, start=0, end=-1):
        self.mglo.clear_samplers(start, end)

    def core_profile_check(self):
        profile_mask = self.info["GL_CONTEXT_PROFILE_MASK"]
        if profile_mask != 1:
            warnings.warn("The window should request a CORE OpenGL profile")

        version_code = self.version_code
        if not version_code:
            major, minor = map(int, self.info["GL_VERSION"].split(".", 2)[:2])
            version_code = major * 100 + minor * 10

        if version_code < 330:
            warnings.warn(
                "The window should support OpenGL 3.3+ (version_code=%d)" % version_code
            )

    def __enter__(self):
        self.mglo.__enter__()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.mglo.__exit__(exc_type, exc_val, exc_tb)

    def release(self):
        if _store.default_context is self:
            _store.default_context = None
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.release()
            self.mglo = InvalidObject()

    def clear_errors(self):
        if not isinstance(self.mglo, InvalidObject):
            self.mglo.clear_errors()

    @contextmanager
    def debug_scope(self, label, group_id=None, source="application"):
        if not isinstance(label, str):
            raise TypeError(
                f"Expected label to be a string, got {type(label).__name__}"
            )

        if not isinstance(group_id, (int, type(None))):
            raise TypeError(
                f"Expected group_id to be an int or None, got {type(group_id).__name__}"
            )

        if source == "application":
            source_id = _GL_DEBUG_SOURCE_APPLICATION
        elif source == "external":
            source_id = _GL_DEBUG_SOURCE_THIRD_PARTY
        else:
            raise ValueError(
                f"source must be 'application' or 'external', got '{source}'"
            )

        if not self.supports_debug_scopes:
            # If we don't support debug scopes, silently do nothing
            # so that client code doesn't have to check for support
            yield
            return

        group_pushed = False
        try:
            group_id = hash(label) if group_id is None else group_id
            self.mglo.push_debug_scope(source_id, group_id, label)
            group_pushed = True
            yield
        finally:
            if group_pushed:
                # We raise an exception if glPushDebugGroup (or equivalent) fails,
                # so we only want to pop the group if it was successfully pushed.
                # Otherwise, could pop a scope that was pushed somewhere else
                # and debugging that would be a pain in the ass.
                self.mglo.pop_debug_scope()


def create_context(require=None, standalone=False, share=False, **settings):
    if require is None:
        require = 330

    if not standalone and not share and not settings and _store.default_context is None:
        ctx = get_context()

        if ctx.version_code < require:
            raise ValueError(
                "Requested OpenGL version {0}, got version {1}".format(
                    require, ctx.version_code
                )
            )

        return ctx

    mode = "standalone" if standalone else "detect"
    if share:
        mode = "share"

    ctx = Context.__new__(Context)
    ctx.mglo, ctx.version_code = mgl.create_context(
        glversion=require, mode=mode, **settings
    )
    ctx._info = None
    ctx._extensions = None
    ctx.extra = None
    ctx._gc_mode = None
    ctx._objects = deque()

    if ctx.version_code < require:
        raise ValueError(
            "Requested OpenGL version {0}, got version {1}".format(
                require, ctx.version_code
            )
        )

    if standalone:
        ctx._screen = None
        ctx.fbo = None
    else:
        # Default framebuffer
        ctx._screen = ctx.detect_framebuffer(0)
        # Currently bound framebuffer
        ctx.fbo = ctx.detect_framebuffer()
        ctx.mglo.fbo = ctx.fbo.mglo

    _store.default_context = ctx
    return ctx


def init_context(loader=None):
    if loader is None:
        from _moderngl import DefaultLoader

        loader = DefaultLoader()

    ctx = Context.__new__(Context)
    ctx.mglo, ctx.version_code = mgl.create_context(context=loader)
    ctx._info = None
    ctx._extensions = None
    ctx.extra = None
    ctx._gc_mode = None
    ctx._objects = deque()

    ctx._screen = ctx.detect_framebuffer(0)
    ctx.fbo = ctx.detect_framebuffer()
    ctx.mglo.fbo = ctx.fbo.mglo
    _store.default_context = ctx


def get_context():
    if _store.default_context is None:
        init_context()
    return _store.default_context


def create_standalone_context(**kwargs):
    return create_context(standalone=True, **kwargs)


def detect_format(program, attributes, mode="mgl"):
    def fmt(attr):
        # Translate shape format into attribute format
        mgl_fmt = {"d": "f8", "I": "u"}
        # moderngl attribute format uses f, i and u
        if mode == "mgl":
            return (
                attr.array_length * attr.dimension,
                mgl_fmt.get(attr.shape) or attr.shape,
            )
        # struct attribute format uses f, d, i and I
        elif mode == "struct":
            return attr.array_length * attr.dimension, attr.shape
        else:
            raise ValueError("invalid format mode: {0}".format(mode))

    locations = program._attribute_locations
    types = program._attribute_types
    return " ".join(
        "%d%s" % fmt(types[x] if type(x) is int else types[locations[x]])
        for x in attributes
    )


def _resolve_module_constants(scope):
    _constants = [
        "NOTHING",
        "BLEND",
        "DEPTH_TEST",
        "CULL_FACE",
        "RASTERIZER_DISCARD",
        "PROGRAM_POINT_SIZE",
        "STENCIL_TEST",
        "POINTS",
        "LINES",
        "LINE_LOOP",
        "LINE_STRIP",
        "TRIANGLES",
        "TRIANGLE_STRIP",
        "TRIANGLE_FAN",
        "LINES_ADJACENCY",
        "LINE_STRIP_ADJACENCY",
        "TRIANGLES_ADJACENCY",
        "TRIANGLE_STRIP_ADJACENCY",
        "PATCHES",
        "NEAREST",
        "LINEAR",
        "NEAREST_MIPMAP_NEAREST",
        "LINEAR_MIPMAP_NEAREST",
        "NEAREST_MIPMAP_LINEAR",
        "LINEAR_MIPMAP_LINEAR",
        "ZERO",
        "ONE",
        "SRC_COLOR",
        "ONE_MINUS_SRC_COLOR",
        "SRC_ALPHA",
        "ONE_MINUS_SRC_ALPHA",
        "DST_ALPHA",
        "ONE_MINUS_DST_ALPHA",
        "DST_COLOR",
        "ONE_MINUS_DST_COLOR",
        "DEFAULT_BLENDING",
        "ADDITIVE_BLENDING",
        "PREMULTIPLIED_ALPHA",
        "FUNC_ADD",
        "FUNC_SUBTRACT",
        "FUNC_REVERSE_SUBTRACT",
        "MIN",
        "MAX",
        "FIRST_VERTEX_CONVENTION",
        "LAST_VERTEX_CONVENTION",
        "VERTEX_ATTRIB_ARRAY_BARRIER_BIT",
        "ELEMENT_ARRAY_BARRIER_BIT",
        "UNIFORM_BARRIER_BIT",
        "TEXTURE_FETCH_BARRIER_BIT",
        "SHADER_IMAGE_ACCESS_BARRIER_BIT",
        "COMMAND_BARRIER_BIT",
        "PIXEL_BUFFER_BARRIER_BIT",
        "TEXTURE_UPDATE_BARRIER_BIT",
        "BUFFER_UPDATE_BARRIER_BIT",
        "FRAMEBUFFER_BARRIER_BIT",
        "TRANSFORM_FEEDBACK_BARRIER_BIT",
        "ATOMIC_COUNTER_BARRIER_BIT",
        "SHADER_STORAGE_BARRIER_BIT",
        "ALL_BARRIER_BITS",
        "KEEP",
        "ZERO",
        "REPLACE",
        "INCR",
        "INCR_WRAP",
        "DECR",
        "DECR_WRAP",
        "INVERT",
    ]

    for c in _constants:
        scope[c] = getattr(Context, c)


_resolve_module_constants(locals())
