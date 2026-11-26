import collections.abc
from typing import Any, Literal, Protocol, Sequence
from typing_extensions import deprecated

__version__: str

Data = collections.abc.Buffer
Vec2 = tuple[float, float]
Vec3 = tuple[float, float, float]
Vec4 = tuple[float, float, float, float]
IVec2 = tuple[int, int]
IVec3 = tuple[int, int, int]
IVec4 = tuple[int, int, int, int]

ImageFormat = Literal[
    'r8unorm',
    'rg8unorm',
    'rgba8unorm',
    'bgra8unorm',
    'r8snorm',
    'rg8snorm',
    'rgba8snorm',
    'r8uint',
    'rg8uint',
    'rgba8uint',
    'r16uint',
    'rg16uint',
    'rgba16uint',
    'r32uint',
    'rg32uint',
    'rgba32uint',
    'r8sint',
    'rg8sint',
    'rgba8sint',
    'r16sint',
    'rg16sint',
    'rgba16sint',
    'r32sint',
    'rg32sint',
    'rgba32sint',
    'r16float',
    'rg16float',
    'rgba16float',
    'r32float',
    'rg32float',
    'rgba32float',
    'rgb10a2unorm',
    'depth16unorm',
    'depth24plus',
    'depth24plus-stencil8',
    'depth32float',
]


BufferUsage = Literal[
    'stream_draw',
    'stream_read',
    'stream_copy',
    'static_draw',
    'static_read',
    'static_copy',
    'dynamic_draw',
    'dynamic_read',
    'dynamic_copy',
]

Mode = Literal['points', 'lines', 'line_loop', 'line_strip', 'triangles', 'triangle_strip', 'triangle_fan']

CompareFunc = Literal['never', 'less', 'equal', 'lequal', 'greater', 'notequal', 'gequal', 'always']

StencilOp = Literal['keep', 'zero', 'replace', 'incr', 'incr_wrap', 'decr', 'decr_wrap', 'invert']

CullFace = Literal['front', 'back', 'front_and_back']

BlendFactor = Literal[
    'zero',
    'one',
    'src_color',
    'one_minus_src_color',
    'dst_color',
    'one_minus_dst_color',
    'src_alpha',
    'one_minus_src_alpha',
    'dst_alpha',
    'one_minus_dst_alpha',
    'constant_color',
    'one_minus_constant_color',
    'constant_alpha',
    'one_minus_constant_alpha',
    'src_alpha_saturate',
]

BlendEquation = Literal['add', 'subtract', 'reverse_subtract', 'min', 'max']

Filter = Literal[
    'nearest',
    'linear',
    'nearest_mipmap_nearest',
    'linear_mipmap_nearest',
    'nearest_mipmap_linear',
    'linear_mipmap_linear',
]

Wrap = Literal['repeat', 'clamp_to_edge', 'mirrored_repeat']

UniformValue = float | int | bool | Sequence[Any] | Data

Attribute = tuple['Buffer', int, str, int, int, int]
Attachment = tuple['Texture', int, int]
BufferView = tuple['Buffer', int, int]
StorageImage = tuple['Texture', int | None, int]

class ReadRequest:
    size: int
    def is_ready(self) -> bool: ...
    def wait(self) -> bytes: ...

class Buffer:
    index: bool
    size: int
    glo: int
    def write(self, data: Data, offset: int = 0) -> None: ...
    def read(self, size: int | None = None, offset: int = 0) -> ReadRequest: ...
    def bind(self, layout: str, *locations: int, offset: int = 0) -> tuple[Attribute, ...]: ...
    def view(self, offset: int = 0, size: int | None = None) -> BufferView: ...
    def use(self, binding: int, offset: int = 0, size: int | None = None) -> None: ...

class Texture:
    size: IVec2
    format: ImageFormat
    multisampled: bool
    array: int
    cubemap: bool
    levels: int
    glo: int
    def write(
        self,
        data: Data,
        size: IVec2 | None = None,
        offset: IVec2 | None = None,
        layer: int | None = None,
        level: int = 0,
    ) -> None: ...
    def read(self, size: IVec2 | None = None, offset: IVec2 | None = None, layer: int = 0, level: int = 0) -> ReadRequest: ...
    def attachment(self, *, layer: int | None = None, level: int = 0, face: int | None = None) -> Attachment: ...
    def storage_image(self, *, layer: int | None = None, level: int = 0, face: int | None = None) -> StorageImage: ...
    def mipmaps(self) -> None: ...

class Sampler:
    texture: Texture
    glo: int
    def use(self, unit: int) -> None: ...

class Resources:
    samplers: dict[int, Sampler]
    uniform_buffers: dict[int, BufferView]
    storage_buffers: dict[int, BufferView]
    storage_images: dict[int, StorageImage]
    def use(self) -> None: ...

class Framebuffer:
    attachments: tuple[Attachment, ...]
    size: IVec2
    multisampled: bool
    viewport: IVec4
    scissor: IVec4 | None
    clear_color: Any
    clear_depth: float
    clear_stencil: int
    glo: int
    def clear(self, attachments: Sequence[int] | None = None) -> None: ...
    def use(self) -> None: ...

class VertexArray:
    attributes: tuple[Attribute, ...]
    index_buffer: Buffer | None
    index_size: int
    vertices: int
    mode: Mode
    glo: int
    def use(self) -> None: ...
    def render(
        self,
        *,
        mode: Mode | None = None,
        vertices: int | None = None,
        instances: int = 1,
        first: int = 0,
        first_instance: int = 0,
    ) -> None: ...
    def draw(self, vertices: int, first: int = 0, *, instances: int = 1, first_instance: int = 0, mode: Mode | None = None) -> None: ...
    def draw_indirect(self, buffer: Buffer, count: int = 1, offset: int = 0, *, mode: Mode | None = None) -> None: ...

class Program:
    uniforms: dict[str, int]
    glo: int
    def use(self) -> None: ...
    def __setitem__(self, key: str, value: UniformValue) -> None: ...

class Pipeline:
    program: Program
    vertex_array: VertexArray | None
    framebuffer: Framebuffer | None
    resources: Resources
    vertices: int
    instances: int
    first: int
    first_instance: int
    group_count: IVec3
    viewport: IVec4 | None
    scissor: IVec4 | None
    def run(self) -> None: ...
    def use(self) -> None: ...
    def __setitem__(self, key: str, value: UniformValue) -> None: ...

class Context:
    shaders: dict[str, str]
    info: dict[str, Any]
    screen: Framebuffer
    empty_vertex_array: VertexArray
    bound_program: Program | None
    bound_framebuffer: Framebuffer | None
    bound_vertex_array: VertexArray | None
    depth_test: bool
    depth_write: bool
    depth_func: CompareFunc
    cull_face: CullFace | None
    polygon_offset: tuple[float, float] | None
    def buffer(
        self,
        data: Data | None = None,
        *,
        size: int | None = None,
        index: bool = False,
        usage: BufferUsage | None = None,
    ) -> Buffer: ...
    def texture(
        self,
        size: IVec2,
        format: ImageFormat = 'rgba8unorm',
        data: Data | None = None,
        *,
        multisampled: bool = False,
        array: int = 0,
        levels: int = 1,
        cubemap: bool = False,
    ) -> Texture: ...
    def framebuffer(self, attachments: Sequence[Texture | Attachment]) -> Framebuffer: ...
    def vertex_array(
        self,
        attributes: Sequence[Attribute],
        index_buffer: Buffer | None = None,
        *,
        index_size: int = 4,
        vertices: int | None = None,
        mode: Mode = 'triangles',
    ) -> VertexArray: ...
    def program(
        self,
        vertex_shader: str,
        fragment_shader: str,
        *,
        samplers: dict[str, int] | None = None,
        uniform_buffers: dict[str, int] | None = None,
        storage_buffers: dict[str, int] | None = None,
        storage_images: dict[str, int] | None = None,
        uniforms: dict[str, str] | None = None,
    ) -> Program: ...
    def sampler(
        self,
        texture: Texture,
        *,
        min_filter: Filter | None = None,
        mag_filter: Literal['nearest', 'linear'] = 'linear',
        wrap_x: Wrap = 'repeat',
        wrap_y: Wrap = 'repeat',
        wrap_z: Wrap = 'repeat',
        min_lod: float = -1000.0,
        max_lod: float = 1000.0,
        lod_bias: float = 0.0,
        compare_func: CompareFunc | None = None,
        anisotropy: float = 1.0,
    ) -> Sampler: ...
    def resources(
        self,
        *bases: Resources,
        samplers: dict[int, Sampler] | None = None,
        uniform_buffers: dict[int, Buffer | BufferView] | None = None,
        storage_buffers: dict[int, Buffer | BufferView] | None = None,
        storage_images: dict[int, Texture | StorageImage] | None = None,
    ) -> Resources: ...
    def pipeline(
        self,
        vertex_shader: str,
        fragment_shader: str,
        *,
        samplers: dict[str, int] | None = None,
        uniform_buffers: dict[str, int] | None = None,
        storage_buffers: dict[str, int] | None = None,
        storage_images: dict[str, int] | None = None,
        uniforms: dict[str, str] | None = None,
        vertex_array: VertexArray,
        framebuffer: Framebuffer,
        resources: Resources | None = None,
        vertices: int | None = None,
        instances: int = 1,
        first: int = 0,
        first_instance: int = 0,
        mode: Mode | None = None,
        viewport: IVec4 | None = None,
        scissor: IVec4 | None = None,
        polygon_offset: tuple[float, float] | None = None,
        depth_test: bool = False,
        depth_write: bool = True,
        depth_func: CompareFunc = 'less',
        cull_face: CullFace | None = None,
        blend: dict[str, str] | None = None,
        stencil: dict[str, Any] | None = None,
    ) -> Pipeline: ...
    def compute(
        self,
        compute_shader: str,
        *,
        samplers: dict[str, int] | None = None,
        uniform_buffers: dict[str, int] | None = None,
        storage_buffers: dict[str, int] | None = None,
        storage_images: dict[str, int] | None = None,
        uniforms: dict[str, str] | None = None,
        resources: Resources | None = None,
        group_count: IVec3 = (1, 1, 1),
    ) -> Pipeline: ...
    def memory_barrier(self) -> None: ...
    def external_buffer(self, buffer: int, size: int | None = None, *, index: bool = False) -> Buffer: ...
    def external_texture(
        self,
        texture: int,
        size: IVec2,
        format: ImageFormat = 'rgba8unorm',
        *,
        multisampled: bool = False,
        array: int = 0,
        levels: int = 1,
        cubemap: bool = False,
    ) -> Texture: ...
    def external_framebuffer(
        self,
        framebuffer: int,
        size: IVec2,
        formats: Sequence[ImageFormat] | None = None,
        *,
        multisampled: bool = False,
    ) -> Framebuffer: ...
    def copy_buffer(
        self,
        src: Buffer,
        dst: Buffer,
        size: int | None = None,
        *,
        src_offset: int = 0,
        dst_offset: int = 0,
    ) -> None: ...
    def copy_texture(
        self,
        src: Texture,
        dst: Texture | Framebuffer,
        size: IVec2 | None = None,
        *,
        src_offset: IVec2 | None = None,
        dst_offset: IVec2 | None = None,
        dst_size: IVec2 | None = None,
        filter: Literal['nearest', 'linear'] = 'nearest',
        flip_y: bool = False,
    ) -> None: ...
    def copy_buffer_to_texture(
        self,
        src: Buffer,
        dst: Texture,
        size: IVec2 | None = None,
        *,
        src_offset: int = 0,
        dst_offset: IVec2 | None = None,
        dst_layer: int = 0,
        dst_level: int = 0,
    ) -> None: ...
    def copy_texture_to_buffer(
        self,
        src: Texture,
        dst: Buffer,
        size: IVec2 | None = None,
        *,
        src_offset: IVec2 | None = None,
        dst_offset: int = 0,
    ) -> None: ...
    def blend(
        self,
        src: BlendFactor | None = None,
        dst: BlendFactor | None = None,
        *,
        equation: BlendEquation | None = None,
        src_alpha: BlendFactor | None = None,
        dst_alpha: BlendFactor | None = None,
        equation_alpha: BlendEquation | None = None,
    ) -> None: ...
    def stencil(
        self,
        func: CompareFunc | None = None,
        ref: int = 0,
        *,
        read_mask: int = 0xFF,
        write_mask: int = 0xFF,
        fail: StencilOp | None = None,
        depth_fail: StencilOp | None = None,
        depth_pass: StencilOp | None = None,
        back_func: CompareFunc | None = None,
        back_fail: StencilOp | None = None,
        back_depth_fail: StencilOp | None = None,
        back_depth_pass: StencilOp | None = None,
    ) -> None: ...
    def release(self, obj: Buffer | Texture | Framebuffer | VertexArray | Program | Sampler | Pipeline | Literal['all', 'shader_cache'] | None = None) -> None: ...
    def source(self, name: str) -> str: ...
    def gc(self) -> list[Any]: ...
    def debug(self) -> dict[str, Any]: ...
    def reset_state(self) -> None: ...
    def start_frame(self) -> None: ...
    def end_frame(self) -> None: ...
    def present(self, texture: Texture) -> None: ...

class Loader(Protocol):
    def load_opengl_function(self, name: str) -> int: ...

class HeadlessContext:
    def load_opengl_function(self, name: str) -> int: ...
    def release(self) -> None: ...

def init(loader: Loader | None = None) -> None: ...
def get_proc_address(name: str) -> int: ...
@deprecated('use get_context()')
def create_context() -> Context: ...
def get_context() -> Context: ...
def headless_context() -> HeadlessContext: ...
def camera(
    eye: Vec3,
    target: Vec3 = (0.0, 0.0, 0.0),
    up: Vec3 = (0.0, 0.0, 1.0),
    fov: float = 60.0,
    aspect: float = 1.0,
    near: float = 0.1,
    far: float = 1000.0,
    size: float = 1.0,
    clip: bool = False,
) -> bytes: ...
