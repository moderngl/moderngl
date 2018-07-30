# Change Log

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## [Unreleased]

### Fixed

- missing `ComputeShader` uniforms and uniform blocks
- wrong `DrawBuffer` call when clearing the screen
- reading depth values from textures
- broken comapre functions

### Added

- `index_element_size` parameter for `Context.vertex_array` and `Context.simple_vertex_array`
- Sampler objects and the `Context.sampler` method
- `Context.max_anisotropy` property
- `Context.clear_samplers` method
- `Framebuffer.read` and `Framebuffer.read_into` now accepts `attachment=-1` to read the depth attachment
- `Texture.compare_func` property
- `Texture.anisotropy` property
- docs and tests

## [5.3.0] - 2018-06-27

### Added

- `index_element_size` for `Context.vertex_array` method

### Fixed

- comapre fuctions

## [5.2.1] - 2018-05-10

### Fixed

- markdown readme issues

## [5.2.0] - 2018-05-10

### Added

- `Context.core_profile_check` method
- `GL_CONTEXT_PROFILE_MASK` to `Context.info`
- `GL_PATCHES` constant for drawing patches
- getter and setter for `GL_PATCH_VERTICES`
- markdown readme

### Fixed

- tesselation control and tesselation evaluation shader being swapped when creating a program

## [5.1.0] - 2018-04-28

### Added

- offset and size parameter to `Buffer.bind_to_uniform_block`
- offset and size parameter to `Buffer.bind_to_storage_buffer`
- `GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT` to `Context.info`

## [5.0.7] - 2018-04-08

### Fixed

- minor bugs in Buffer Format

## [5.0.6] - 2018-03-22

### Fixed

- `standalone_context` on linux [issue #173](https://github.com/cprogrammer1994/ModernGL/issues/173)

## [5.0.5] - 2018-03-21

### Fixed

- Standalone context on linux fails when the default display is nonzero.

## [5.0.4] - 2018-03-07

### Fixed

- Standalone context on windows fails when old GPU driver is installed.

## [5.0.3] - 2018-02-28

### Added

- TextureArray objects
- Indirect drawing

### Changed

- Restored Error class from ModernGL4

### Fixed

- Properly set subroutines in VertexArray render

## [5.0.2] - 2018-02-20

### Fixed

- Wrong module name in some windows based wheel files

## [5.0.1] - 2018-02-19

### Fixed

- Build errors and warnings for older python versions

## [5.0.0] - 2018-02-18

For more information please see: [Differneces between ModernGL5 and ModernGL4](https://moderngl.readthedocs.io/en/stable/misc/mgl5_vs_mgl4.html)

### Added

- Query objects
- Scope objects
- Conditional Rendering
- New `RASTERIZER_DISCARD` enable flag
- Context fbo attribute
- Context depth_func and blend_func
- Depth Texture compare_func

### Changed

- Most of the constants became integers
- Renamed `default_framebuffer` to `screen` and always points to framebuffer 0
- VertexArray's buffer formats
- Program creation
- Program members
- Replace floats parameter with dtype

### Removed

- Classes and methods that are no longer needed

## [4.2.2] - 2018-01-06

### Added

- Program.\_\_getitem\_\_ returns Uniforms, UniformBuffers, Attributes and Varyings

## [4.2.1] - 2017-11-25

### Added

- UniformMap.get, AttributeMap.get, ... methods

### Fixed

- Contex.info `GL_MAX_COMPUTE_WORK_GROUP_COUNT` value
- Missing TextureCube class
- Buffer.clear return type hint

## [4.2.0] - 2017-10-17

### Added

- Default framebuffer bits attribute
- Context.detect_framebuffer
- Support reserving buffers with human readable sizes

### Fixed

- Standalone context on linux requires 24 bits for the depth buffer at least
- Bound framebuffer reference count

## [4.1.12] - 2017-08-22

### Added

- TextureCube class and Context `texture_cube` method
- Context `enable_only` method

### Changed

- Enable flags support bitwise `__or__`, `__and__` and `__inv__`
- Framebuffer does not create a Renderbuffer when the depth_attachment is None

### Removed

- `ModernGL.MULTISAMPLE` constant
- Version class and Version constants

### Fixed

- Texture3D created with wrong type

## [4.1.11] - 2017-07-02

### Added

- Context `front_face` attribute

### Fixed

- VertexArray `program` member type
- Context `viewport` attribute missing `tuple` cast
- Misplaced `GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS`
- Compile warnings (strict-prototypes)
- Initial value of `wireframe`

## [4.1.10] - 2017-06-23

### Added

- Documentation improvements
- Context `max_samples` attribute

### Fixed

- Validate the number of samples using wrong values

## [4.1.9] - 2017-06-23

### Added

- Context `info` attribute
- Support Textures in `copy_framebuffer`
- Texture `floats` attribute

### Fixed

- 3D texture `depth` attribute type

## [4.1.8] - 2017-06-19

### Added

- 3D texture support (`Texture3D` class and Context `texture3d` method)
- Context `max_integer_samples` attribute
- Fallback display for linux

### Removed

- Unused texture's `read` method's `viewport` parameter

### Fixed

- Broken framebuffer `color_attachments` attribute
- Segmentation fault when creating a texture using string data by mistake
- Validate Renderbuffer and Texture samples

## [4.1.7] - 2017-06-13

### Added

- Uniform `read` and `write` methods
- `write_offset` parameter for the `read_into` methods
- Read framebuffer's pixels to a buffer
- Read texture's pixels to a buffer
- Write texture's from a buffer
- Pixel Buffer Object

### Fixed

- Default framebuffer wrong drawbuffers
- Framebuffer non zero viewport settings
- Uniform setter error messages

## [4.1.6] - 2017-06-12

### Fixed

- Broken uniform setter in 4.1.5

## [4.1.5] - 2017-06-09

### Fixed

- Modifying framebuffer properties has no effect unless the framebuffer is explicitly bound

## [4.1.4] - 2017-06-07

### Added

- Framebuffer `color_mask` attribute
- Framebuffer `depth_mask` attribute
- Framebuffer `viewport` attribute

### Fixed

- `copy_framebuffer` using `default_framebuffer`
- Failing `__repr__` in VertexArrays

### Changed

- Framebuffer methods do not affect the bound framebuffer

### Removed

- Invalid texture sampling parameters for multisample textures
- Invalid texture sampling parameters for depth textures

## [4.1.3] - 2017-06-04

### Added

- TextureFilter and TextureWrap classes and constants
- Texture `build_mipmaps` method
- Texture `repeat_x`, `repeat_y` and `filter` attributes
- Texture `swizzle` getter

## [4.1.2] - 2017-05-31

### Added

- Read any color attachments of the Framebuffer
- Framebuffer, Buffer, BufferAccess and Texture `read_into` method
- `__slots__` for ModernGL classes
- Framebuffer read different color attachments
- Tkinter connector for windows using ctypes

### Fixed

- Multiple render targets
- Non-float renderbuffers are not readable
- Buffer `read` keyword only `offset` argument

### Changed

- `Context.renderbuffer` will use `floats=False` by default
- `bind_to_uniform_block` `location` parameter was renamed to `binding`
- `bind_to_storage_buffer` `location` parameter was renamed to `binding`

## [4.1.1] - 2017-05-27

### Added

- UniformBlock `binding` and `size` attribute
- Framebuffer `width`, `height` and `samples` attribute
- Texture `swizzle` attribute
- Context `error` attribute
- OpenGL objects as `glo` attributes
- Buffer `clear()` method
- Missing `release()` methods
- Iterable program member maps
- Human readable program members
- Pixel pack and unpack alignments
- Subroutines

### Removed

- UniformBlock `array_size` and `location` attribute
- VertexArrayAttribute `default` attribute

### Fixed

- BufferAccess unused `offset` parameter
- Binding buffer to a UniformBlock
- Broken RTD docs

## [4.1.0] - 2017-05-24

### Added

- Exception handling when importing the implementation
- Standalone context on **linux** and **mac** with fallback mechanism
- Require version at context creation
- Context `version_code` property
- Framebuffer `clear()` method

### Changed

- Context `clear()` method accepts floats

### Removed

- Required `-std=c++11` for **linux** and **mac**
- ModernGL `VERSION` constant

## Fixed

- `Framebuffer` and `Texture` can have uninitialized x, y values
- Uniform setters

## [4.0.0] - 2017-05-20

### Added

- Full linting support
- Restructure docs
- Examples in docs
- pep8 style
- VSCode snippets
- Changelog

### Changed

- Wrap the C/C++ module with a pure python module
- Rename internal module to avoid ambiguosity
- Rename class like members from the Context class

## [3.1.5] - 2017-05-07

### Added

- Standalone ModernGL context
- Sphinx generated docs
- Linting with pylint mock file
- Mac OSX support

## [3.1.0] - 2017-03-01

### Added

- Wrapping OpenGL objects with python types

### Changed

- Improved Linux support

### Removed

- Direct access to OpenGL objects

## [2.4.1] - 2016-08-30

### Added

- setup.py and PyPI compatible packaging
- Direct access to OpenGL objects
- Linux support

## 1.0.0 - 2016-03-03

- It was a tool for me to develop games in python

[Unreleased]: https://github.com/cprogrammer1994/ModernGL/compare/5.2.1...master
[5.2.1]: https://github.com/cprogrammer1994/ModernGL/compare/5.2.0...5.2.1
[5.2.0]: https://github.com/cprogrammer1994/ModernGL/compare/5.1.0...5.2.0
[5.1.0]: https://github.com/cprogrammer1994/ModernGL/compare/5.0.7...5.1.0
[5.0.7]: https://github.com/cprogrammer1994/ModernGL/compare/5.0.6...5.0.7
[5.0.6]: https://github.com/cprogrammer1994/ModernGL/compare/5.0.5...5.0.6
[5.0.5]: https://github.com/cprogrammer1994/ModernGL/compare/5.0.4...5.0.5
[5.0.4]: https://github.com/cprogrammer1994/ModernGL/compare/5.0.3...5.0.4
[5.0.3]: https://github.com/cprogrammer1994/ModernGL/compare/5.0.2...5.0.3
[5.0.2]: https://github.com/cprogrammer1994/ModernGL/compare/5.0.1...5.0.2
[5.0.1]: https://github.com/cprogrammer1994/ModernGL/compare/5.0.0...5.0.1
[5.0.0]: https://github.com/cprogrammer1994/ModernGL/tree/5.0.0
[4.2.2]: https://github.com/cprogrammer1994/ModernGL/compare/4.2.1...4.2.2
[4.2.1]: https://github.com/cprogrammer1994/ModernGL/compare/4.2.0...4.2.1
[4.2.0]: https://github.com/cprogrammer1994/ModernGL/compare/4.1.12...4.2.0
[4.1.12]: https://github.com/cprogrammer1994/ModernGL/compare/4.1.11...4.1.12
[4.1.11]: https://github.com/cprogrammer1994/ModernGL/compare/4.1.10...4.1.11
[4.1.10]: https://github.com/cprogrammer1994/ModernGL/compare/4.1.9...4.1.10
[4.1.9]: https://github.com/cprogrammer1994/ModernGL/compare/4.1.8...4.1.9
[4.1.8]: https://github.com/cprogrammer1994/ModernGL/compare/4.1.7...4.1.8
[4.1.7]: https://github.com/cprogrammer1994/ModernGL/compare/4.1.6...4.1.7
[4.1.6]: https://github.com/cprogrammer1994/ModernGL/compare/4.1.5...4.1.6
[4.1.5]: https://github.com/cprogrammer1994/ModernGL/compare/4.1.4...4.1.5
[4.1.4]: https://github.com/cprogrammer1994/ModernGL/compare/4.1.3...4.1.4
[4.1.3]: https://github.com/cprogrammer1994/ModernGL/compare/4.1.2...4.1.3
[4.1.2]: https://github.com/cprogrammer1994/ModernGL/compare/4.1.1...4.1.2
[4.1.1]: https://github.com/cprogrammer1994/ModernGL/compare/4.1.0...4.1.1
[4.1.0]: https://github.com/cprogrammer1994/ModernGL/compare/4.0.0...4.1.0
[4.0.0]: https://github.com/cprogrammer1994/ModernGL/tree/4.0.0
[3.1.5]: https://github.com/cprogrammer1994/ModernGL/compare/3.1.0...3.1.5
[3.1.0]: https://github.com/cprogrammer1994/ModernGL/tree/3.1.0
[2.4.1]: https://github.com/cprogrammer1994/ModernGL/tree/2.4.1
