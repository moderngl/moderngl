'''
    ModernGL context
'''

# pylint: disable=too-many-public-methods

import re
from typing import Tuple

try:
    from . import mgl
except ImportError:
    from .mock import mgl

from .common import InvalidObject
from .buffers import Buffer, detect_format
from .programs import ComputeShader, Shader, Program
from .vertex_arrays import VertexArray
from .textures import Texture, Texture3D, TextureCube
from .renderbuffers import Renderbuffer
from .framebuffers import Framebuffer


def _size_from_str(value):
    size_map = {
        'KB': 1024,
        'MB': 1024 * 1024,
        'GB': 1024 * 1024 * 1024,
    }

    match = re.match(r'(\d+)([KMG]B)', value)
    return int(match.group(1)) * size_map[match.group(2)]


class Context:
    '''
        Create a :py:class:`Context` using:

        - :py:func:`ModernGL.create_context`
        - :py:func:`ModernGL.create_standalone_context`

        The :py:func:`create_context` must be used when rendering in a window.
        The :py:func:`create_standalone_context` must be used when rendering
        without a window.

        Members:

            - :py:meth:`Context.program`
            - :py:meth:`Context.vertex_shader`
            - :py:meth:`Context.fragment_shader`
            - :py:meth:`Context.geometry_shader`
            - :py:meth:`Context.tess_evaluation_shader`
            - :py:meth:`Context.tess_control_shader`
            - :py:meth:`Context.buffer`
            - :py:meth:`Context.simple_vertex_array`
            - :py:meth:`Context.vertex_array`
            - :py:meth:`Context.texture`
            - :py:meth:`Context.depth_texture`
            - :py:meth:`Context.renderbuffer`
            - :py:meth:`Context.depth_renderbuffer`
            - :py:meth:`Context.simple_framebuffer`
            - :py:meth:`Context.framebuffer`
    '''

    __slots__ = ['mglo']

    @staticmethod
    def new(obj):
        '''
            For internal use only.
        '''

        res = Context.__new__(Context)
        res.mglo = obj
        return res

    def __init__(self):
        self.mglo = None
        raise NotImplementedError()

    def __repr__(self):
        return '<Context>'

    def __eq__(self, other):
        return self.mglo is other.mglo

    def __ne__(self, other):
        return self.mglo is not other.mglo

    @property
    def line_width(self) -> float:
        '''
            float: Set the default line width.
        '''

        return self.mglo.line_width

    @line_width.setter
    def line_width(self, value):
        self.mglo.line_width = value

    @property
    def point_size(self) -> float:
        '''
            float: Set the default point size.
        '''

        return self.mglo.point_size

    @point_size.setter
    def point_size(self, value):
        self.mglo.point_size = value

    @property
    def viewport(self) -> Tuple[int, int, int, int]:
        '''
            tuple: The viewport.

            Reading this property may force the GPU to sync.
            Use this property to set the viewport only.
        '''

        return self.mglo.viewport

    @viewport.setter
    def viewport(self, value):
        self.mglo.viewport = tuple(value)

    @property
    def max_samples(self) -> int:
        '''
            int: The max samples.
        '''

        return self.mglo.max_samples

    @property
    def max_integer_samples(self) -> int:
        '''
            int: The max integer samples.
        '''

        return self.mglo.max_integer_samples

    @property
    def max_texture_units(self) -> int:
        '''
            int: The max texture units.
        '''

        return self.mglo.max_texture_units

    @property
    def default_texture_unit(self) -> int:
        '''
            int: The default texture unit.
        '''

        return self.mglo.default_texture_unit

    @default_texture_unit.setter
    def default_texture_unit(self, value):
        self.mglo.default_texture_unit = value

    @property
    def default_framebuffer(self) -> Framebuffer:
        '''
            Framebuffer: The default framebuffer.
        '''

        return Framebuffer.new(self.mglo.default_framebuffer)

    @property
    def wireframe(self) -> bool:
        '''
            bool: Wireframe settings for debugging.
        '''

        return self.mglo.wireframe

    @wireframe.setter
    def wireframe(self, value):
        self.mglo.wireframe = value

    @property
    def front_face(self) -> str:
        '''
            str: The front_face.
        '''

        return self.mglo.front_face

    @front_face.setter
    def front_face(self, value):
        self.mglo.front_face = str(value)

    @property
    def error(self) -> str:
        '''
            str: The result of glGetError() but human readable.
            This values is provided for debug purposes only.
        '''

        return self.mglo.error

    @property
    def vendor(self) -> str:
        '''
            str: The vendor.
        '''

        return self.mglo.vendor

    @property
    def renderer(self) -> str:
        '''
            str: The renderer.
        '''

        return self.mglo.renderer

    @property
    def version(self) -> str:
        '''
            str: The OpenGL version.
        '''

        return self.mglo.version

    @property
    def version_code(self) -> int:
        '''
            int: The OpenGL version.
        '''

        return self.mglo.version_code

    @property
    def info(self) -> dict:
        '''
            dict: The result of multiple glGet.
        '''

        return self.mglo.info

    def clear(self, red=0.0, green=0.0, blue=0.0, alpha=0.0, *, viewport=None) -> None:
        '''
            Clear the framebuffer.

            Values must be in ``(0, 255)`` range.
            If the `viewport` is not ``None`` then scrissor test
            will be used to clear the given viewport.

            If the `viewport` is a 2-tuple it will clear the
            ``(0, 0, width, height)`` where ``(width, height)`` is the 2-tuple.

            If the `viewport` is a 4-tuple it will clear the given viewport.

            Args:
                red (float): color component.
                green (float): color component.
                blue (float): color component.
                alpha (float): alpha component.

            Keyword Args:
                viewport (tuple): The viewport.
        '''

        if viewport is not None:
            viewport = tuple(viewport)

        self.mglo.clear(red, green, blue, alpha, viewport)

    def enable_only(self, flags) -> None:
        '''
            Enable flags.

            Valid flags are:

                - :py:data:`ModernGL.BLEND`
                - :py:data:`ModernGL.DEPTH_TEST`
                - :py:data:`ModernGL.CULL_FACE`

            Args:
                flags (EnableFlag): The flag to enable.
        '''

        self.mglo.enable_only(flags.flags)

    def enable(self, flags) -> None:
        '''
            Enable flags.

            Valid flags are:

                - :py:data:`ModernGL.BLEND`
                - :py:data:`ModernGL.DEPTH_TEST`
                - :py:data:`ModernGL.CULL_FACE`

            Args:
                flag (EnableFlag): The flag to enable.
        '''

        self.mglo.enable(flags.flags)

    def disable(self, flags) -> None:
        '''
            Disable flags.

            Valid flags are:

                - :py:data:`ModernGL.BLEND`
                - :py:data:`ModernGL.DEPTH_TEST`
                - :py:data:`ModernGL.CULL_FACE`

            Args:
                flag (EnableFlag): The flag to disable.
        '''

        self.mglo.disable(flags.flags)

    def finish(self) -> None:
        '''
            Wait for all drawing commands to finish.
        '''

        self.mglo.finish()

    def copy_buffer(self, dst, src, size=-1, *, read_offset=0, write_offset=0) -> None:
        '''
            Copy buffer content.

            Args:
                dst (Buffer): Destination buffer.
                src (Buffer): Source buffer.
                size (int): Size to copy.

            Keyword Args:
                read_offset (int): Read offset.
                write_offset (int): Write offset.
        '''

        self.mglo.copy_buffer(dst.mglo, src.mglo, size, read_offset, write_offset)

    def copy_framebuffer(self, dst, src) -> None:
        '''
            Copy framebuffer content.

            Use this method to:

                - blit framebuffers.
                - copy framebuffer content into a texture.
                - downsample framebuffers. (it will allow to read the framebuffer's content)
                - downsample a framebuffer directly to a texture.

            Args:
                dst (Framebuffer or Texture): Destination framebuffer or texture.
                src (Framebuffer): Source framebuffer.
        '''

        self.mglo.copy_framebuffer(dst.mglo, src.mglo)

    def detect_framebuffer(self, glo) -> Framebuffer:
        '''
            Detect framebuffer.

            Args:
                glo (int): Framebuffer object.

            Returns:
                Framebuffer: framebuffer.
        '''

        return self.mglo.detect_framebuffer(glo)

    def buffer(self, data=None, *, reserve=0, dynamic=False) -> Buffer:
        '''
            Create a :py:class:`Buffer`.

            Args:
                data (bytes): Content of the new buffer.

            Keyword Args:
                reserve (int): The number of bytes to reserve.
                dynamic (bool): Treat buffer as dynamic.

            Returns:
                Buffer: buffer
        '''

        if isinstance(reserve, str):
            reserve = _size_from_str(reserve)

        return Buffer.new(self.mglo.buffer(data, reserve, dynamic))

    def texture(self, size, components, data=None, *, samples=0, alignment=1, floats=False) -> Texture:
        '''
            Create a :py:class:`Texture`.

            Args:
                size (tuple): The width and height of the texture.
                components (int): The number of components 1, 2, 3 or 4.
                data (bytes): Content of the texture.

            Keyword Args:
                samples (int): The number of samples. Value `0` means no multisample format.
                alignment (int): The byte alignment 1, 2, 4 or 8.
                floats (bool): Use floating point precision.

            Returns:
                Texture: texture
        '''

        return Texture.new(self.mglo.texture(size, components, data, samples, alignment, floats))

    def texture3d(self, size, components, data=None, *, alignment=1, floats=False) -> Texture3D:
        '''
            Create a :py:class:`Texture3D`.

            Args:
                size (tuple): The width, height and depth of the texture.
                components (int): The number of components 1, 2, 3 or 4.
                data (bytes): Content of the texture.

            Keyword Args:
                alignment (int): The byte alignment 1, 2, 4 or 8.
                floats (bool): Use floating point precision.

            Returns:
                Texture3D: texture
        '''

        return Texture3D.new(self.mglo.texture3d(size, components, data, alignment, floats))

    def texture_cube(self, size, components, data=None, *, alignment=1, floats=False) -> TextureCube:
        '''
            Create a :py:class:`TextureCube`.

            Args:
                size (tuple): The width, height and depth of the texture.
                components (int): The number of components 1, 2, 3 or 4.
                data (bytes): Content of the texture.

            Keyword Args:
                alignment (int): The byte alignment 1, 2, 4 or 8.
                floats (bool): Use floating point precision.

            Returns:
                TextureCube: texture
        '''

        return TextureCube.new(self.mglo.texture_cube(size, components, data, alignment, floats))

    def depth_texture(self, size, data=None, *, samples=0, alignment=4) -> Texture:
        '''
            Create a :py:class:`Texture`.

            Args:
                size (tuple): The width and height of the texture.
                data (bytes): Content of the texture.

            Keyword Args:
                samples (int): The number of samples. Value `0` means no multisample format.
                alignment (int): The byte alignment 1, 2, 4 or 8.

            Returns:
                Texture: depth texture
        '''

        return Texture.new(self.mglo.depth_texture(size, data, samples, alignment))

    def vertex_array(self, program, content, index_buffer=None) -> VertexArray:
        '''
            Create a :py:class:`VertexArray`.

            Args:
                program (Program): The program used by :py:meth:`~VertexArray.render` and
                                   :py:meth:`~VertexArray.transform`.
                content (list): A list of (buffer, format, attributes).
                index_buffer (Buffer): An index buffer.

            Returns:
                VertexArray: vertex array
        '''

        if index_buffer is not None:
            index_buffer = index_buffer.mglo

        content = tuple((a.mglo, b, tuple(c)) for a, b, c in content)
        return VertexArray.new(self.mglo.vertex_array(program.mglo, content, index_buffer))

    def simple_vertex_array(self, program, buffer, attributes) -> VertexArray:
        '''
            Create a :py:class:`VertexArray`.

            This is an alias for::

                format = detect_format(program, attributes)
                vertex_array(program, [(buffer, format, attributes)])

            Args:
                program (Program): The program used by :py:meth:`~VertexArray.render` and
                                   :py:meth:`~VertexArray.transform`.
                buffer (Buffer): The buffer.
                attributes (list): A list of attribute names.

            Returns:
                VertexArray: vertex array
        '''

        content = [(buffer, detect_format(program, attributes), attributes)]
        return self.vertex_array(program, content, None)

    def program(self, shaders, varyings=()) -> Program:
        '''
            Create a :py:class:`Program` object.

            Only linked programs will be returned.

            For more information please see: :py:class:`Program` and :py:class:`Shader`

            A single shader in the `shaders` parameter is also accepted.
            The varyings are only used when a transform program is created.

            Args:
                shaders (list): A list of :py:class:`Shader` objects.
                varyings (list): A list of varying names.

            Returns:
                Program: program

            Examples:

                A simple program designed for rendering::

                    >>> my_render_program = ctx.program([
                    ... 	ctx.vertex_shader(\'\'\'
                    ... 		#version 330
                    ...
                    ... 		in vec2 vert;
                    ...
                    ... 		void main() {
                    ... 			gl_Position = vec4(vert, 0.0, 1.0);
                    ... 		}
                    ... 	\'\'\'),
                    ... 	ctx.fragment_shader(\'\'\'
                    ... 		#version 330
                    ...
                    ... 		out vec4 color;
                    ...
                    ... 		void main() {
                    ... 			color = vec4(0.3, 0.5, 1.0, 1.0);
                    ... 		}
                    ... 	\'\'\'),
                    ... ])

                A simple program designed for transforming::

                    >>> my_vertex_shader = ctx.vertex_shader(\'\'\'
                    ...     #version 330
                    ...
                    ...     in vec4 vert;
                    ...     out float vert_length;
                    ...
                    ...     void main() {
                    ...         vert_length = length(vert);
                    ...     }
                    ... \'\'\')

                    >>> my_transform_program = ctx.program(my_vertex_shader, ['vert_length'])
        '''

        if isinstance(shaders, Shader):
            shaders = (shaders.mglo,)

        else:
            shaders = tuple(x.mglo for x in shaders)

        if isinstance(varyings, str):
            varyings = (varyings,)

        else:
            varyings = tuple(x for x in varyings)

        return Program.new(self.mglo.program(shaders, varyings))

    def vertex_shader(self, source) -> Shader:
        '''
            The Vertex Shader is a programmable Shader stage in the rendering pipeline
            that handles the processing of individual vertices.

            Vertex shaders are fed Vertex Attribute data, as specified from a vertex array
            object by a drawing command. A vertex shader receives a single vertex from the
            vertex stream and generates a single vertex to the output vertex stream.

            Args:
                source (str): The source code in GLSL.

            Returns:
                Shader: vertex shader

            Examples:

                Create a simple vertex shader::

                    >>> my_vertex_shader = ctx.vertex_shader(\'\'\'
                    ...     #version 330
                    ...
                    ...     in vec2 vert;
                    ...
                    ...     void main() {
                    ...         gl_Position = vec4(vert, 0.0, 1.0);
                    ...     }
                    ... \'\'\')
        '''

        return Shader.new(self.mglo.vertex_shader(source))

    def fragment_shader(self, source) -> Shader:
        '''
            A Fragment Shader is the Shader stage that will process a Fragment generated by
            the Rasterization into a set of colors and a single depth value.

            Args:
                source (str): The source code in GLSL.

            Returns:
                Shader: fragment shader

            Examples:

                Create a simple fragment shader::

                    >>> my_fragment_shader = ctx.fragment_shader(\'\'\'
                    ...     #version 330
                    ...
                    ...     out vec4 color;
                    ...
                    ...     void main() {
                    ...         color = vec4(0.3, 0.5, 1.0, 1.0);
                    ...     }
                    ... \'\'\')
        '''

        return Shader.new(self.mglo.fragment_shader(source))

    def geometry_shader(self, source) -> Shader:
        '''
            A Geometry Shader is a Shader program written in GLSL that governs the processing of Primitives.
            Geometry shaders reside between the Vertex Shaders (or the optional Tessellation stage) and the
            fixed-function Vertex Post-Processing stage.

            A geometry shader is optional and does not have to be used.

            Args:
                source (str): The source code in GLSL.

            Returns:
                Shader: geometry shader
        '''

        return Shader.new(self.mglo.geometry_shader(source))

    def tess_evaluation_shader(self, source) -> Shader:
        '''
            Tessellation is the Vertex Processing stage in the OpenGL rendering pipeline where
            patches of vertex data are subdivided into smaller Primitives.

            The Tessellation Evaluation Shader takes the tessellated patch and computes
            the vertex values for each generated vertex.

            Args:
                source (str): The source code in GLSL.

            Returns:
                Shader: tesselation evaluation shader
        '''

        return Shader.new(self.mglo.tess_evaluation_shader(source))

    def tess_control_shader(self, source) -> Shader:
        '''
            The Tessellation Control Shader (TCS) determines how much tessellation to do.
            It can also adjust the actual patch data, as well as feed additional patch data to later stages.
            The Tessellation Control Shader is optional.

            Args:
                source (str): The source code in GLSL.

            Returns:
                Shader: tesselation control shader
        '''

        return Shader.new(self.mglo.tess_control_shader(source))

    def simple_framebuffer(self, size, components=4, *, samples=0, floats=False) -> Framebuffer:
        '''
            A :py:class:`Framebuffer` is a collection of buffers that can be used as the destination for rendering.
            The buffers for Framebuffer objects reference images from either Textures or Renderbuffers.

            Args:
                size (tuple): The width and height of the renderbuffer.
                components (int): The number of components 1, 2, 3 or 4.

            Keyword Args:
                samples (int): The number of samples. Value `0` means no multisample format.
                floats (bool): Use floating point precision.

            Returns:
                Framebuffer: framebuffer
        '''

        return self.framebuffer(
            self.renderbuffer(size, components, samples=samples, floats=floats),
            self.depth_renderbuffer(size, samples=samples),
        )

    def framebuffer(self, color_attachments, depth_attachment=None) -> Framebuffer:
        '''
            A :py:class:`Framebuffer` is a collection of buffers that can be used as the destination for rendering.
            The buffers for Framebuffer objects reference images from either Textures or Renderbuffers.

            Args:
                color_attachments (list): A list of `Texture` or `Renderbuffer` objects.
                depth_attachment (Renderbuffer or Texture): A `Texture` or `Renderbuffer` object.

            Returns:
                Framebuffer: framebuffer
        '''

        if isinstance(color_attachments, (Texture, Renderbuffer)):
            color_attachments = (color_attachments.mglo,)

        else:
            color_attachments = tuple(x.mglo for x in color_attachments)

        if depth_attachment is not None:
            depth_attachment = depth_attachment.mglo

        return Framebuffer.new(self.mglo.framebuffer(color_attachments, depth_attachment))

    def renderbuffer(self, size, components=4, *, samples=0, floats=False) -> Renderbuffer:
        '''
            :py:class:`Renderbuffer` objects are OpenGL objects that contain images.
            They are created and used specifically with :py:class:`Framebuffer` objects.

            Args:
                size (tuple): The width and height of the renderbuffer.
                components (int): The number of components 1, 2, 3 or 4.

            Keyword Args:
                samples (int): The number of samples. Value `0` means no multisample format.
                floats (bool): Use floating point precision.

            Returns:
                Renderbuffer: renderbuffer
        '''

        return Renderbuffer.new(self.mglo.renderbuffer(size, components, samples, floats))

    def depth_renderbuffer(self, size, *, samples=0) -> Renderbuffer:
        '''
            :py:class:`Renderbuffer` objects are OpenGL objects that contain images.
            They are created and used specifically with :py:class:`Framebuffer` objects.

            Args:
                size (tuple): The width and height of the renderbuffer.

            Keyword Args:
                samples (int): The number of samples. Value `0` means no multisample format.

            Returns:
                Renderbuffer: depth renderbuffer
        '''

        return Renderbuffer.new(self.mglo.depth_renderbuffer(size, samples))

    def compute_shader(self, source) -> ComputeShader:
        '''
            A :py:class:`ComputeShader` is a Shader Stage that is used entirely for computing arbitrary information.
            While it can do rendering, it is generally used for tasks not directly related to drawing.

            Args:
                source (str): The source of the compute shader.

            Returns:
                ComputeShader: compute shader program
        '''

        return ComputeShader.new(self.mglo.compute_shader(source))

    def release(self) -> None:
        '''
            Release the ModernGL object.
        '''

        self.mglo.release()
        self.__class__ = InvalidObject


def create_context(require=None) -> Context:
    '''
        Create a ModernGL context by loading OpenGL functions from an existing OpenGL context.
        An OpenGL context must exists. If rendering is done without a window please use the
        :py:func:`create_standalone_context` instead.

        Keyword Arguments:
            require (int): OpenGL version code.

        Returns:
            Context: context
    '''

    ctx = Context.new(mgl.create_context())

    if require is not None and ctx.version_code < require:
        raise Exception('The version required is not provided')

    return ctx


def create_standalone_context(size=(4, 4), require=None) -> Context:
    '''
        Create a standalone ModernGL context.
        This method will create a hidden window with the
        initial size given in the parameters. This will
        set the  initial viewport as well.

        Keyword Arguments:
            size (tuple): Initial framebuffer size.
            require (int): OpenGL version code.

        Returns:
            Context: context
    '''

    ctx = Context.new(mgl.create_standalone_context(*size))

    if require is not None and ctx.version_code < require:
        raise Exception('The version required is not provided')

    return ctx
