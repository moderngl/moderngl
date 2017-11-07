'''
    ModernGL: PyOpenGL alternative
'''

import os

try:
    from .mgl import Error

except ImportError:

    if os.environ.get('READTHEDOCS') == 'True':
        from .mock import Error

    else:
        _IMPORT_ERROR = '\n'.join([
            'No implementation found for ModernGL',
            'Are you sure the source code is compiled properly?',
            'Hint: python3 setup.py build_ext --inplace',
        ])

        raise ImportError(_IMPORT_ERROR)

from .buffers import Buffer, BufferAccess, detect_format
from .common import (
    BLEND, CULL_FACE, DEPTH_TEST, LINE_LOOP, LINE_STRIP, LINE_STRIP_ADJACENCY, LINEAR, LINES, LINES_ADJACENCY, MIPMAP,
    NEAREST, POINTS, TRIANGLE_FAN, TRIANGLE_STRIP, TRIANGLE_STRIP_ADJACENCY, TRIANGLES, TRIANGLES_ADJACENCY,
    InvalidObject, Primitive, TextureFilter
)
from .context import Context, create_context, create_standalone_context
from .framebuffers import Framebuffer
from .members import (
    Attribute, AttributeMap, ProgramStage, Subroutine, SubroutineMap, SubroutineUniform, SubroutineUniformMap, Uniform,
    UniformBlock, UniformBlockMap, UniformMap, Varying, VaryingMap
)
from .programs import ComputeShader, Program, Shader
from .renderbuffers import Renderbuffer
from .textures import Texture, Texture3D, TextureCube
from .vertex_arrays import VertexArray, VertexArrayAttribute

__version__ = '4.2.0'

__all__ = [
    'InvalidObject', 'EnableFlag', 'Primitive', 'Error', 'TextureFilter',
    'POINTS', 'LINES', 'LINE_STRIP', 'LINE_LOOP', 'TRIANGLES', 'TRIANGLE_STRIP', 'TRIANGLE_FAN',
    'LINES_ADJACENCY', 'LINE_STRIP_ADJACENCY', 'TRIANGLES_ADJACENCY', 'TRIANGLE_STRIP_ADJACENCY',
    'BLEND', 'DEPTH_TEST', 'CULL_FACE',
    'LINEAR', 'NEAREST', 'MIPMAP',
    'Subroutine', 'SubroutineMap', 'SubroutineUniform', 'SubroutineUniformMap',
    'Uniform', 'UniformMap', 'UniformBlock', 'UniformBlockMap',
    'Varying', 'VaryingMap', 'Attribute', 'AttributeMap',
    'Program', 'ProgramStage', 'Shader', 'ComputeShader',
    'Texture', 'Texture3D', 'TextureCube', 'Framebuffer', 'Renderbuffer',
    'VertexArray', 'VertexArrayAttribute',
    'Buffer', 'BufferAccess',
    'Context',
    'create_context', 'create_standalone_context',
    'detect_format',
]
