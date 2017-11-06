Context
*******

.. currentmodule:: ModernGL

.. autofunction:: ModernGL.create_context(require=None) -> Context
.. autofunction:: ModernGL.create_standalone_context(size=(256, 256), require=None) -> Context

.. autoclass:: Context
	:members: line_width, point_size, viewport, max_samples, max_integer_samples, max_texture_units, default_texture_unit, default_framebuffer, wireframe, error, vendor, renderer, version, version_code, info, front_face

	.. automethod:: clear(red=0.0, green=0.0, blue=0.0, alpha=0.0, viewport=None)
	.. automethod:: enable(flag)
	.. automethod:: enable_only(flags)
	.. automethod:: disable(flag)
	.. automethod:: finish()
	.. automethod:: copy_buffer(dst, src, size=-1, read_offset=0, write_offset=0)
	.. automethod:: copy_framebuffer(dst, src)
	.. automethod:: detect_framebuffer(glo) -> Framebuffer

	.. automethod:: buffer(data=None, reserve=0, dynamic=False) -> Buffer
	.. automethod:: texture(size, components, data=None, samples=0, floats=False) -> Texture
	.. automethod:: texture3d(size, components, data=None, floats=False) -> Texture3D
	.. automethod:: texture_cube(size, components, data=None, alignment=1, floats=False) -> TextureCube
	.. automethod:: depth_texture(size, data=None, samples=0) -> Texture
	.. automethod:: vertex_array(program, content, index_buffer=None) -> VertexArray
	.. automethod:: simple_vertex_array(program, buffer, attributes) -> VertexArray
	.. automethod:: program(shaders, varyings=()) -> Program
	.. automethod:: vertex_shader(source) -> Shader
	.. automethod:: fragment_shader(source) -> Shader
	.. automethod:: geometry_shader(source) -> Shader
	.. automethod:: tess_evaluation_shader(source) -> Shader
	.. automethod:: tess_control_shader(source) -> Shader
	.. automethod:: simple_framebuffer(size, components=4, samples=0, floats=False) -> Framebuffer
	.. automethod:: framebuffer(color_attachments, depth_attachment=None) -> Framebuffer
	.. automethod:: renderbuffer(size, components=4, samples=0, floats=False) -> Renderbuffer
	.. automethod:: depth_renderbuffer(size, samples=0) -> Renderbuffer
	.. automethod:: compute_shader(source) -> ComputeShader

.. toctree::
	:maxdepth: 4
