# ModernGL

[![Join the chat at https://gitter.im/cprogrammer1994/ModernGL](https://badges.gitter.im/cprogrammer1994/ModernGL.svg)](https://gitter.im/cprogrammer1994/ModernGL?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

```
python -m pip install ModernGL
```

```
git clone https://github.com/cprogrammer1994/ModernGL
install.bat
```

# Supported Versions

![OpenGL-3.3](https://raw.githubusercontent.com/cprogrammer1994/libOpenGL.a/master/Badges/OpenGL-33.png)
![OpenGL-4.0](https://raw.githubusercontent.com/cprogrammer1994/libOpenGL.a/master/Badges/OpenGL-40.png)
![OpenGL-4.1](https://raw.githubusercontent.com/cprogrammer1994/libOpenGL.a/master/Badges/OpenGL-41.png)
![OpenGL-4.2](https://raw.githubusercontent.com/cprogrammer1994/libOpenGL.a/master/Badges/OpenGL-42.png)
![OpenGL-4.3](https://raw.githubusercontent.com/cprogrammer1994/libOpenGL.a/master/Badges/OpenGL-43.png)
![OpenGL-4.4](https://raw.githubusercontent.com/cprogrammer1994/libOpenGL.a/master/Badges/OpenGL-44.png)
![OpenGL-4.5](https://raw.githubusercontent.com/cprogrammer1994/libOpenGL.a/master/Badges/OpenGL-45.png)

Checkout the [OpenGL Registry](https://www.opengl.org/registry/) for core specifications.

# Description

OpenGL is a great environment for developing portable, platform independent,
interactive 2D and 3D graphics applications. The API implementation in Python
is cumbersome, resulting in applications with high latency. To solve this
problem we have developed ModernGL, a wrapper over OpenGL that simplifies the
creation of simple graphics applications like scientific simulations, small
games or user interfaces. Usually, acquiring in-depth knowledge of OpenGL
requires a steep learning curve. In contrast, ModernGL is easy to learn and
use, moreover it is capable of rendering with the same performance and
quality, with less code written.

# Docs

[Python Documentation](https://pythonhosted.org/ModernGL/)

```
InitializeModernGL(font = True)
GetError()
GetInfo()

Viewport(x, y, w, h)
Clear(r = 0, g = 0, b = 0, a = 255)

PointSize(size)
LineSize(size)

EnableOnly(mask)
  # Parameters:
    ENABLE_NOTHING
    ENABLE_BLEND
    ENABLE_CULL_FACE
    ENABLE_DEPTH_TEST
    ENABLE_MULTISAMPLE
  
  # Example:
    EnableOnly(ENABLE_CULL_FACE | ENABLE_DEPTH_TEST)

EnableBlend()
DisableBlend()
EnableCullFace()
DisableCullFace()
EnableDepthTest()
DisableDepthTest()
EnableMultisample()
DisableMultisample()

NewProgram(shaders)
  # Example:
    NewProgram([NewVertexShader(...), NewFragmentShader(...)])

DeleteProgram(program)
  # This will delete all the shaders linked to this program

UseProgram(program)
UseDefaultProgram()

NewFragmentShader(source)
NewGeometryShader(source)
NewVertexShader(source)
DeleteShader(shader)

CompilerLog()

AttributeLocation(program, name)
UniformLocation(program, name)
UniformBlockLocation(program, name)

Uniform1f(location, v0)
Uniform2f(location, v0, v1)
Uniform3f(location, v0, v1, v2)
Uniform4f(location, v0, v1, v2, v3)
Uniform1i(location, v0)
Uniform2i(location, v0, v1)
Uniform3i(location, v0, v1, v2)
Uniform4i(location, v0, v1, v2, v3)

UniformMatrix(location, matrix)
  # Example:
    UniformMatrix([1, 2, 3 ... 15, 16])

UniformTransposeMatrix(location, matrix)
UniformBlock(location, buffer)

NewTexture(width, height, data, components = 3)
DeleteTexture(texture)
UpdateTexture(texture, x, y, width, height, data, components = 3)
UseTexture(texture, location = 0)

SetTexturePixelated(texture)
SetTextureFiltered(texture)
SetTextureMipmapped(texture)

BuildMipmap(texture, base = 0, max = 1000)

NewVertexArray(format, attribs, indexBuffer = 0)
DeleteVertexArray(array)
EnableAttribute(vao, target)
DisableAttribute(vao, target)
EnableAttributes(vao, targets)
DisableAttributes(vao, targets)

NewVertexBuffer(data)
NewIndexBuffer(data)
NewUniformBuffer(data)
NewDynamicVertexBuffer(data)
NewDynamicIndexBuffer(data)
NewDynamicUniformBuffer(data)
DeleteBuffer(buffer)

UpdateVertexBuffer(buffer, offset, data)
UpdateIndexBuffer(buffer, offset, data)
UpdateUniformBuffer(buffer, offset, data)

RenderTriangles(vao, count, first = 0, instances = 1)
RenderTriangleStrip(vao, count, first = 0, instances = 1)
RenderTriangleFan(vao, count, first = 0, instances = 1)
RenderLines(vao, count, first = 0, instances = 1)
RenderLineStrip(vao, count, first = 0, instances = 1)
RenderLineLoop(vao, count, first = 0, instances = 1)
RenderPoints(vao, count, first = 0, instances = 1)
RenderLineStripAdjacency(vao, count, first = 0, instances = 1)
RenderLinesAdjacency(vao, count, first = 0, instances = 1)
RenderTriangleStripAdjacency(vao, count, first = 0, instances = 1)
RenderTrianglesAdjacency(vao, count, first = 0, instances = 1)

RenderIndexedTriangles(vao, count, first = 0, instances = 1)
RenderIndexedTriangleStrip(vao, count, first = 0, instances = 1)
RenderIndexedTriangleFan(vao, count, first = 0, instances = 1)
RenderIndexedLines(vao, count, first = 0, instances = 1)
RenderIndexedLineStrip(vao, count, first = 0, instances = 1)
RenderIndexedLineLoop(vao, count, first = 0, instances = 1)
RenderIndexedPoints(vao, count, first = 0, instances = 1)
RenderIndexedLineStripAdjacency(vao, count, first = 0, instances = 1)
RenderIndexedLinesAdjacency(vao, count, first = 0, instances = 1)
RenderIndexedTriangleStripAdjacency(vao, count, first = 0, instances = 1)
RenderIndexedTrianglesAdjacency(vao, count, first = 0, instances = 1)

NewFramebuffer(width, height, multisample = True)
DeleteFramebuffer(framebuffer)
UseFramebuffer(framebuffer)
UseDefaultFramebuffer()

ReadPixels(x, y, width, height, components = 3)
ReadDepthPixels(x, y, width, height)
ReadPixel(x, y)
ReadDepthPixel(x, y)
```

```
ExtensionActive()

UseTextureAsImage(texture, binding = 0, components = 3)

NewTessControlShader(source)
NewTessEvaluationShader(source)

NewComputeShader(source)
DeleteComputeShader(program)
RunComputeShader(program, x = 1, y = 1, z = 1)

NewStorageBuffer(data)
NewDynamicStorageBuffer(data)
UpdateStorageBuffer(buffer, offset, data)
UseStorageBuffer(buffer, binding = 0)
ReadStorageBuffer(buffer, offset, size)
```

# Concept

SOON
> ModernGL simply implements a set of OpenGL functions

# Example

SOON
> [Julia Fractal](https://gist.github.com/cprogrammer1994/ecd69bc181e159305ad6)
