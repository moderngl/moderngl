# Change Log

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## [main](https://github.com/moderngl/moderngl/compare/5.10.0...main)

## [5.10.0](https://github.com/moderngl/moderngl/compare/5.9.0...5.10.0)

- Adding pre-built wheels for MacOS ARM
- Adding size verify when writing to Uniforms
- Adding a new methods: [get_context()](https://moderngl.readthedocs.io/en/latest/reference/moderngl.html#moderngl.moderngl.get_context) and `init_context()` (documentation is not ready yet)
- Complementing old and adding new items to ["The Guide"](https://moderngl.readthedocs.io/en/latest/the_guide/index.html)
- Redesign of ["The Guide"](https://moderngl.readthedocs.io/en/latest/the_guide/index.html) in the documentation
- Adding shadow mapping example (see `examples/*`)
- Improving code reliability (robustness)

## [5.9.0](https://github.com/moderngl/moderngl/compare/5.8.0...5.9.0)

- Removing Python 3.7 support, adding Python 3.12 support
- Fixing memory leaks
- Transferring constants from `moderngl` to `moderngl.Context` (safely)
- Dividing documentation into annotations (`moderngl-stubs/__init__.pyi`) and full documentation (`docs/*`)
- Implemented new property: [Context.depth_clamp_range](https://moderngl.readthedocs.io/en/latest/reference/context.html#Context.depth_clamp_range)
- Adding shader includes
- Adding an example of creating a separate window object
- Adding access to input attributes in SPIR-V programs (via SPIR-V parsing) (example: `vao = ctx.vertex_array(spv_program, buffer, 0, 1)` (if your SPIR-V compiler did not erase attribute names, they can also be used))
- Adding access to program attributes by their location numbers (example: `vao = ctx.vertex_array(program, buffer, 0, 1)`)
- Adding descriptions for [Context.memory_barrier()](https://moderngl.readthedocs.io/en/latest/reference/context.html#Context.memory_barrier) and [Context.depth_clamp_range](https://moderngl.readthedocs.io/en/latest/reference/context.html#Context.depth_clamp_range) in the documentation
- Fixed inaccuracies in documentation
- Implemented depth cube maps: [Context.depth_texture_cube()](https://moderngl.readthedocs.io/en/latest/reference/context.html#Context.depth_texture_cube)
- Update build documentation command (sphinx) (see in `README.md`)
- Minor changes to `README.md`

## [5.8.0](https://github.com/moderngl/moderngl/compare/5.7.4...5.8.0)

This version includes the following structural changes in moderngl:

- The typehints and inline documentation was moved to moderngl-stubs.
- The Python layer was concatenated but moderngl itself stays a package.
- Most of the protected class members were removed.
- Most of the property aliases became ordinary class members.
- Slots were removed.

## [5.7.4](https://github.com/moderngl/moderngl/compare/5.7.3...5.7.4)

...

## [5.7.3](https://github.com/moderngl/moderngl/compare/5.7.2...5.7.3)

...

## [5.7.2](https://github.com/moderngl/moderngl/compare/5.7.1...5.7.2)

...

## [5.7.1](https://github.com/moderngl/moderngl/compare/5.7.0...5.7.1)

...

## [5.7.0](https://github.com/moderngl/moderngl/compare/5.6.4...5.7.0)

* Python 3.11 support
* Added support for configurable garbage collection mode. ``Context.gc_mode`` controls this.
  * ``None``: (default) No garbage collection is performed. Objects needs to to be
    manually released like in previous versions of moderngl.
  * ``"context_gc"``: Dead objects are collected in ``Context.objects``.
    These can periodically be released using ``Context.gc()``.
  * ``"auto"``: Dead objects are destroyed automatically like we would expect in python.
* Added support for `glPolygonOffset`. The factors and units can be set using `Context.polygon_offset`
* Added support for normalized signed and unsigned integer textures. `ni1`, `nu1`, `ni2` and `nu2`
  are the new dtypes for these.
* Added ``Context.external_texture`` for creating textures from existing OpenGL textures.
* Added `TextureCube.bind_to_image` so we can easily access cube textures in compute shaders
* Added `Texture3D.bind_to_image` so we can easily access 3D textures in compute shaders
* Added `TextureArray.bind_to_image` so we can easily access texture arrays in compute shaders
* Added support for specifying a custom internalformat for cube maps. This can be used
  for sRGB and texture compression formats.
* Integer textures now use `NEAREST` interpolation by default. This was causing
  issues with some drivers.
* Added support for writing to multiple buffers in transforms.
* ``Context.program`` now accepts a dictionary for mapping sampler names to specific texture units.
* Added `Program.is_transform` exposing if the program has a fragment shader or not
* Added `VertexArray.mode` and a `mode` argument in `Context.vertex_array`.
  This is now the default rendering mode when no mode parameter is passed
  in `render()` or `transform()`
* Added support for 1D sampler in `Uniform`
* Added direct access to `glEnable` / `glDisable` in `Context.enable_direct` / `Context.disable_direct`.
  This can be used to enabled capabilities not supported by ModernGL.
* `Framebuffer.read()` now has a `clamp` (bool) parameter. If enabled, floating point data
  will clamp to `[0.0, 1.0]`. Clamping is disabled by default.
* Fixed a bug causing ``copy_framebuffer`` to not work with multisampled framebuffers
  with multiple color attachments.
* Fixed a bug in ``copy_framebuffer`` that caused the draw buffers to be permanently
  changed.
* VertexArray: Removed "the first vertex attribute must not be a per instance attribute" limitation
* Fixed a crash when reading `ctx.provoking_vertex`
* Added workaround for MacOS requiring a framebuffer to be bound in headless mode even for
  transform shaders. We simply create and bind a framebuffer internally.
* Added ``GL_MAX_GEOMETRY_OUTPUT_VERTICES`` to ``Context.info``
* Docstring improvements
* Documentation improvements

## [5.6.4](https://github.com/moderngl/moderngl/compare/5.6.3...5.6.4)

* Transform feedback now supports all primitive modes both for
  vertex and geometry shaders
* `VertexArray.transform()` now raises sane errors when using the wrong primitive mode with geometry shaders
* `VertexArray.transform()` now automatically detects output primitive based on
  the input mode.
* `GL_TRIANGLES_ADJACENCY` enum incorrectly mapped (`D0` instead of `0D`)
* Primitive modes are now accessible in the `Context` instance.
  For example: `ctx.POINTS` and `ctx.TRIANGLES`
* Texture filers are now also available in the `Context` instance.
  For example: `ctx.NEAREST` and `ctx.LINEAR`
* Docstring and documentation improvements

## [5.6.3](https://github.com/moderngl/moderngl/compare/5.6.2...5.6.3)

* Added `Context.extensions`. This is a `set` containing the
  OpenGL extensions supported by the context.
* Added support for overridable internal_format for `Texture`.
  This opens up for sRGB and compressed formats if needed.
* Docstring and documentation improvements

## [5.6.2](https://github.com/moderngl/moderngl/compare/5.6.1...5.6.2)

### Bugs

* `TextureArray.build_mimpmaps()` used the wrong target when building mipmaps.
  This caused the texture to corrupt with some drivers. Thanks to [Nir Aides](https://github.com/nir)
  for discovering this bug.
* Docstring improvements

## [5.6.1](https://github.com/moderngl/moderngl/compare/5.6.0...5.6.1)

### Improvements

* Added `Context.cull_face`. (`glCullFace`). An alternative to `Context.front_face` (`glFrontFace`)
* Added support for signed and unsigned integer sampler uniforms (`isampler*`, `usampler*`)
* Added support for multisampled texture array samplers
* Doc improvements

### Bugs

* Transform feedback with geo shader should now property set the out primitive type.
  It incorrectly assumed `POINTS` in some cases causing a gl error.
* Moderngl will now propagate the default error messages when objects cannot
  be read using the buffer protocol.

## [5.6.0](https://github.com/moderngl/moderngl/compare/5.5.4...5.6.0) - 2020-02-01

From ModernGL 5.6 context creation is done by the [glcontext](https://github.com/moderngl/glcontext)
package. This means we can keep improving context creation without releasing new
moderngl versions. The bar for contributing to context creation is also lower.
Backends can be written in C++ or by simply using ctypes.

This version also has a lot of improvements to docsstrings, api docs and documentation in general.
Still we have a lot more work to do in this area.

### Added

- Standalone `Context` can now be destroyed on all platforms
- Compute shaders can now write to textures using `Texture*.bind_to_image`
- `VertexBuffer.transform` now as a `buffer_offset` parameter
  for setting the start byte offset of the output buffer.
  This can be used in many creative ways in for example particle
  emitting.
- `Framebuffer` supports having only depth buffer. This makes things like shadow
  mapping easier to work with.
- Support for `PROGRAM_POINT_SIZE` context flag so point sizes can be set in
  the vertex shader
- Scissor test support. `Framebuffer.scissor` and `Context.scissor` can now be used
  to set and enable scissor testing per framebuffer.
- The `Framebuffer.clear` have an optional `color` parameter to replace the
  old `red, green, blue, alpha` parameters.
- Samplers and Buffers have an `assign` method to simplify the scope creation.
- Samplers have an optional `texture` parameter that will be used together with
  the sampler when given.
- The Scope's `enable_only` keyword argument is deprecated in favor of the
  `enable` keyword argument. 
- Buffers have a `bind` method that replaces the tuples in the VertexArray creation.
- VertexArrays have an optional `scope` property that will be used when rendering.
- The VertexArray's `vertices` property is now writeable.
- VertexArrays have an `instances` property to control the default number of
  instances when rendering.
- The Context object contains the constants provided by the moderngl module.
  The constants are: (TRIANGLE, LINES, DEPTH_TEST, ...)

### Changed

- The `framebuffer` parameter is optional in Scopes.
- The `ctx.simple_vertex_array` is deprecated in favor of using `ctx.vertex_array` with the same parameters.
- The `prog[uniform].value = value` is deprecated in favor of using `prog[uniform] = value`.
- The `prog[uniform].write(bytes_value)` is deprecated in favor of using the `prog[uniform] = bytes_value`.

### Bugs

- Fixed an issue with attribute format detection causing integer and double types to fail
- Several minor bug fixes

## [5.5.4](https://github.com/moderngl/moderngl/compare/5.5.3...5.5.4) - 2019-11-10

This version was originally meant to only add python 3.8 support,
but a few additions and fixes where cherry-picked from the 5.6 branch.

- Python 3.8 support
- `Vertexarray`s can now be crated without any content.
- `Context.blend_func` now supports separate blend functions for rgb and alpha.
- Added `Context.blend_equation` supporting separate blend equations for rgb and alpha.

## [5.5.3](https://github.com/moderngl/moderngl/compare/5.5.2...5.5.3) - 2019-08-16

### Fixed

- Scope object uniform buffer bindings.

## [5.5.2](https://github.com/moderngl/moderngl/compare/5.5.1...5.5.2) - 2019-06-13

### Fixed

- missing package in setup.py

## [5.5.1](https://github.com/moderngl/moderngl/compare/5.5.0...5.5.1) - 2019-06-12

### Fixed

- offset fix for vao render

## [5.5.0](https://github.com/moderngl/moderngl/compare/5.4.2...5.5.0) - 2019-01-22

### Fixed

- depth texture filter and compare func
- scope restores old enable flags properly
- clear gl errors as the final step of initialization
- fix default framebuffer's number of attachments

### Added

- documentation for `extra` properties
- improved documentation
- improved build on Mac

## [5.4.2](https://github.com/moderngl/moderngl/compare/5.4.1...5.4.2) - 2018-08-01

### Fixed

- included `moderngl.program_members` in the package list

## [5.4.1](https://github.com/moderngl/moderngl/compare/5.4.0...5.4.1) - 2018-07-30

### Fixed

- broken description on PyPI

## [5.4.0](https://github.com/moderngl/moderngl/compare/5.3.0...5.4.0) - 2018-07-30

### Added

- Sampler object support
- `Context.sampler` method
- `Context.max_anisotropy` property
- `Context.clear_samplers` method
- `Framebuffer.read` and `Framebuffer.read_into` now accepts `attachment=-1` to read the depth attachment
- `Texture.compare_func` property
- `Texture.anisotropy` property
- docs and tests

### Fixed

- missing `ComputeShader` uniforms and uniform blocks
- wrong `DrawBuffer` call when clearing the screen
- reading depth values from textures
- broken compare functions

## [5.3.0](https://github.com/moderngl/moderngl/compare/5.2.1...5.3.0) - 2018-06-27

### Added

- `index_element_size` parameter for `Context.vertex_array` and `Context.simple_vertex_array`

### Fixed

- comapre fuctions

## [5.2.1](https://github.com/moderngl/moderngl/compare/5.2.0...5.2.1) - 2018-05-10

### Fixed

- markdown readme issues

## [5.2.0](https://github.com/moderngl/moderngl/compare/5.1.0...5.2.0) - 2018-05-10

### Added

- `Context.core_profile_check` method
- `GL_CONTEXT_PROFILE_MASK` to `Context.info`
- `GL_PATCHES` constant for drawing patches
- getter and setter for `GL_PATCH_VERTICES`
- markdown readme

### Fixed

- tesselation control and tesselation evaluation shader being swapped when creating a program

## [5.1.0](https://github.com/moderngl/moderngl/compare/5.0.7...5.1.0) - 2018-04-28

### Added

- offset and size parameter to `Buffer.bind_to_uniform_block`
- offset and size parameter to `Buffer.bind_to_storage_buffer`
- `GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT` to `Context.info`

## [5.0.7](https://github.com/moderngl/moderngl/compare/5.0.6...5.0.7) - 2018-04-08

### Fixed

- minor bugs in Buffer Format

## [5.0.6](https://github.com/moderngl/moderngl/compare/5.0.5...5.0.6) - 2018-03-22

### Fixed

- `standalone_context` on linux [issue #173](https://github.com/moderngl/moderngl/issues/173)

## [5.0.5](https://github.com/moderngl/moderngl/compare/5.0.4...5.0.5) - 2018-03-21

### Fixed

- Standalone context on linux fails when the default display is nonzero.

## [5.0.4](https://github.com/moderngl/moderngl/compare/5.0.3...5.0.4) - 2018-03-07

### Fixed

- Standalone context on windows fails when old GPU driver is installed.

## [5.0.3](https://github.com/moderngl/moderngl/compare/5.0.2...5.0.3) - 2018-02-28

### Added

- TextureArray objects
- Indirect drawing

### Changed

- Restored Error class from ModernGL4

### Fixed

- Properly set subroutines in VertexArray render

## [5.0.2](https://github.com/moderngl/moderngl/compare/5.0.1...5.0.2) - 2018-02-20

### Fixed

- Wrong module name in some windows based wheel files

## [5.0.1](https://github.com/moderngl/moderngl/compare/5.0.0...5.0.1) - 2018-02-19

### Fixed

- Build errors and warnings for older python versions

## [5.0.0](https://github.com/moderngl/moderngl/tree/5.0.0) - 2018-02-18

For more information please see: [Differences between ModernGL5 and ModernGL4](https://moderngl.readthedocs.io/en/stable/misc/mgl5_vs_mgl4.html)

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

## [4.2.2](https://github.com/moderngl/moderngl/compare/4.2.1...4.2.2) - 2018-01-06

### Added

- Program.\_\_getitem\_\_ returns Uniforms, UniformBuffers, Attributes and Varyings

## [4.2.1](https://github.com/moderngl/moderngl/compare/4.2.0...4.2.1) - 2017-11-25

### Added

- UniformMap.get, AttributeMap.get, ... methods

### Fixed

- Context.info `GL_MAX_COMPUTE_WORK_GROUP_COUNT` value
- Missing TextureCube class
- Buffer.clear return type hint

## [4.2.0](https://github.com/moderngl/moderngl/compare/4.1.12...4.2.0) - 2017-10-17

### Added

- Default framebuffer bits attribute
- Context.detect_framebuffer
- Support reserving buffers with human readable sizes

### Fixed

- Standalone context on linux requires 24 bits for the depth buffer at least
- Bound framebuffer reference count

## [4.1.12](https://github.com/moderngl/moderngl/compare/4.1.11...4.1.12) - 2017-08-22

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

## [4.1.11](https://github.com/moderngl/moderngl/compare/4.1.10...4.1.11) - 2017-07-02

### Added

- Context `front_face` attribute

### Fixed

- VertexArray `program` member type
- Context `viewport` attribute missing `tuple` cast
- Misplaced `GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS`
- Compile warnings (strict-prototypes)
- Initial value of `wireframe`

## [4.1.10](https://github.com/moderngl/moderngl/compare/4.1.9...4.1.10) - 2017-06-23

### Added

- Documentation improvements
- Context `max_samples` attribute

### Fixed

- Validate the number of samples using wrong values

## [4.1.9](https://github.com/moderngl/moderngl/compare/4.1.8...4.1.9) - 2017-06-23

### Added

- Context `info` attribute
- Support Textures in `copy_framebuffer`
- Texture `floats` attribute

### Fixed

- 3D texture `depth` attribute type

## [4.1.8](https://github.com/moderngl/moderngl/compare/4.1.7...4.1.8) - 2017-06-19

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

## [4.1.7](https://github.com/moderngl/moderngl/compare/4.1.6...4.1.7) - 2017-06-13

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

## [4.1.6](https://github.com/moderngl/moderngl/compare/4.1.5...4.1.6) - 2017-06-12

### Fixed

- Broken uniform setter in 4.1.5

## [4.1.5](https://github.com/moderngl/moderngl/compare/4.1.4...4.1.5) - 2017-06-09

### Fixed

- Modifying framebuffer properties has no effect unless the framebuffer is explicitly bound

## [4.1.4](https://github.com/moderngl/moderngl/compare/4.1.3...4.1.4) - 2017-06-07

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

## [4.1.3](https://github.com/moderngl/moderngl/compare/4.1.2...4.1.3) - 2017-06-04

### Added

- TextureFilter and TextureWrap classes and constants
- Texture `build_mipmaps` method
- Texture `repeat_x`, `repeat_y` and `filter` attributes
- Texture `swizzle` getter

## [4.1.2](https://github.com/moderngl/moderngl/compare/4.1.1...4.1.2) - 2017-05-31

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

## [4.1.1](https://github.com/moderngl/moderngl/compare/4.1.0...4.1.1) - 2017-05-27

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

## [4.1.0](https://github.com/moderngl/moderngl/compare/4.0.0...4.1.0) - 2017-05-24

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

## [4.0.0](https://github.com/moderngl/moderngl/tree/4.0.0) - 2017-05-20

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

## [3.1.5](https://github.com/moderngl/moderngl/compare/3.1.0...3.1.5) - 2017-05-07

### Added

- Standalone ModernGL context
- Sphinx generated docs
- Linting with pylint mock file
- Mac OSX support

## [3.1.0](https://github.com/moderngl/moderngl/tree/3.1.0) - 2017-03-01

### Added

- Wrapping OpenGL objects with python types

### Changed

- Improved Linux support

### Removed

- Direct access to OpenGL objects

## [2.4.1](https://github.com/moderngl/moderngl/tree/2.4.1) - 2016-08-30

### Added

- setup.py and PyPI compatible packaging
- Direct access to OpenGL objects
- Linux support

## 1.0.0 - 2016-03-03

- It was a tool for me to develop games in python
