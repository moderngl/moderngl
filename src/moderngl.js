(wasm, gl) => {
  gl.getExtension('EXT_color_buffer_float');
  gl.getExtension('EXT_float_blend');
  const decoder = new TextDecoder();
  const encoder = new TextEncoder();

  const objects = new Map();
  const ids = new WeakMap();
  const locations = new Map();
  const strings = new Map();
  let nextId = 1;
  let packBuffer = 0;
  let unpackBuffer = 0;

  const add = (obj) => {
    const id = nextId++;
    objects.set(id, obj);
    if (obj !== null && typeof obj === 'object') {
      ids.set(obj, id);
    }
    return id;
  };

  const get = (id) => objects.get(id) ?? null;

  const genObjects = (n, ptr, create) => {
    for (let i = 0; i < n; ++i) {
      wasm.HEAPU32[(ptr >> 2) + i] = add(create());
    }
  };

  const deleteObjects = (n, ptr, destroy) => {
    for (let i = 0; i < n; ++i) {
      const id = wasm.HEAPU32[(ptr >> 2) + i];
      destroy(get(id));
      objects.delete(id);
    }
  };

  const getString = (ptr) => {
    const end = wasm.HEAPU8.indexOf(0, ptr);
    return decoder.decode(wasm.HEAPU8.subarray(ptr, end));
  };

  const setString = (ptr, bufSize, lengthPtr, text) => {
    const raw = encoder.encode(text).subarray(0, bufSize - 1);
    wasm.HEAPU8.set(raw, ptr);
    wasm.HEAPU8[ptr + raw.length] = 0;
    if (lengthPtr) {
      wasm.HEAP32[lengthPtr >> 2] = raw.length;
    }
  };

  const setValues = (heap, ptr, value) => {
    const values = typeof value === 'object' ? Array.from(value, Number) : [Number(value)];
    heap.set(values, ptr >> 2);
  };

  const setIntegers = (ptr, value) => {
    const values = typeof value === 'object' ? Array.from(value, Number) : [Number(value)];
    wasm.HEAP32.set(values.map((x) => Math.min(x, 0x7fffffff)), ptr >> 2);
  };

  const getParameter = (value) => (value !== null && typeof value === 'object' && !ArrayBuffer.isView(value) && !Array.isArray(value) ? ids.get(value) ?? 0 : value);

  const pixelHeap = (type) => {
    switch (type) {
      case 0x1400: return [wasm.HEAP8, 0];
      case 0x1401: return [wasm.HEAPU8, 0];
      case 0x1402: return [wasm.HEAP16, 1];
      case 0x1403: return [wasm.HEAPU16, 1];
      case 0x1404: return [wasm.HEAP32, 2];
      case 0x1405: return [wasm.HEAPU32, 2];
      case 0x1406: return [wasm.HEAPF32, 2];
      case 0x140B: return [wasm.HEAPU16, 1];
      case 0x8368: return [wasm.HEAPU32, 2];
      case 0x84FA: return [wasm.HEAPU32, 2];
    }
  };

  const ignoredCaps = new Set([0x8D69, 0x8642, 0x884F]);

  const shaderHeader = `#version 300 es
precision highp float;
precision highp int;
precision highp sampler3D;
precision highp sampler2DArray;
precision highp sampler2DShadow;
precision highp sampler2DArrayShadow;
precision highp samplerCubeShadow;
precision highp isampler2D;
precision highp isampler3D;
precision highp isamplerCube;
precision highp isampler2DArray;
precision highp usampler2D;
precision highp usampler3D;
precision highp usamplerCube;
precision highp usampler2DArray;`;

  return {
    moderngl_glActiveTexture: (texture) => gl.activeTexture(texture),
    moderngl_glAttachShader: (program, shader) => gl.attachShader(get(program), get(shader)),
    moderngl_glBindBuffer: (target, buffer) => {
      if (target === 0x88EB) {
        packBuffer = buffer;
      }
      if (target === 0x88EC) {
        unpackBuffer = buffer;
      }
      gl.bindBuffer(target, get(buffer));
    },
    moderngl_glBindBufferBase: (target, index, buffer) => gl.bindBufferBase(target, index, get(buffer)),
    moderngl_glBindBufferRange: (target, index, buffer, offset, size) => gl.bindBufferRange(target, index, get(buffer), offset, size),
    moderngl_glBindFramebuffer: (target, framebuffer) => gl.bindFramebuffer(target, get(framebuffer)),
    moderngl_glBindRenderbuffer: (target, renderbuffer) => gl.bindRenderbuffer(target, get(renderbuffer)),
    moderngl_glBindSampler: (unit, sampler) => gl.bindSampler(unit, get(sampler)),
    moderngl_glBindTexture: (target, texture) => gl.bindTexture(target, get(texture)),
    moderngl_glBindVertexArray: (array) => gl.bindVertexArray(get(array)),
    moderngl_glBlendEquationSeparate: (modeRgb, modeAlpha) => gl.blendEquationSeparate(modeRgb, modeAlpha),
    moderngl_glBlendFuncSeparate: (srcRgb, dstRgb, srcAlpha, dstAlpha) => gl.blendFuncSeparate(srcRgb, dstRgb, srcAlpha, dstAlpha),
    moderngl_glBlitFramebuffer: (sx0, sy0, sx1, sy1, dx0, dy0, dx1, dy1, mask, filter) => gl.blitFramebuffer(sx0, sy0, sx1, sy1, dx0, dy0, dx1, dy1, mask, filter),
    moderngl_glBufferData: (target, size, data, usage) => gl.bufferData(target, size, usage),
    moderngl_glBufferSubData: (target, offset, size, data) => gl.bufferSubData(target, offset, wasm.HEAPU8, data, size),
    moderngl_glCheckFramebufferStatus: (target) => gl.checkFramebufferStatus(target),
    moderngl_glClearBufferfi: (buffer, drawbuffer, depth, stencil) => gl.clearBufferfi(buffer, drawbuffer, depth, stencil),
    moderngl_glClearBufferfv: (buffer, drawbuffer, value) => gl.clearBufferfv(buffer, drawbuffer, wasm.HEAPF32, value >> 2),
    moderngl_glClearBufferiv: (buffer, drawbuffer, value) => gl.clearBufferiv(buffer, drawbuffer, wasm.HEAP32, value >> 2),
    moderngl_glClearBufferuiv: (buffer, drawbuffer, value) => gl.clearBufferuiv(buffer, drawbuffer, wasm.HEAPU32, value >> 2),
    moderngl_glClientWaitSync: (sync, flags, timeout) => gl.clientWaitSync(get(sync), flags, 0),
    moderngl_glCompileShader: (shader) => gl.compileShader(get(shader)),
    moderngl_glCopyBufferSubData: (readTarget, writeTarget, readOffset, writeOffset, size) => gl.copyBufferSubData(readTarget, writeTarget, readOffset, writeOffset, size),
    moderngl_glCreateProgram: () => add(gl.createProgram()),
    moderngl_glCreateShader: (type) => add(gl.createShader(type)),
    moderngl_glCullFace: (mode) => gl.cullFace(mode),
    moderngl_glDeleteBuffers: (n, buffers) => deleteObjects(n, buffers, (x) => gl.deleteBuffer(x)),
    moderngl_glDeleteFramebuffers: (n, framebuffers) => deleteObjects(n, framebuffers, (x) => gl.deleteFramebuffer(x)),
    moderngl_glDeleteProgram: (program) => {
      for (const [key, id] of locations) {
        if (key.startsWith(`${program}:`)) {
          objects.delete(id);
          locations.delete(key);
        }
      }
      gl.deleteProgram(get(program));
      objects.delete(program);
    },
    moderngl_glDeleteRenderbuffers: (n, renderbuffers) => deleteObjects(n, renderbuffers, (x) => gl.deleteRenderbuffer(x)),
    moderngl_glDeleteSamplers: (n, samplers) => deleteObjects(n, samplers, (x) => gl.deleteSampler(x)),
    moderngl_glDeleteShader: (shader) => {
      gl.deleteShader(get(shader));
      objects.delete(shader);
    },
    moderngl_glDeleteSync: (sync) => {
      gl.deleteSync(get(sync));
      objects.delete(sync);
    },
    moderngl_glDeleteTextures: (n, textures) => deleteObjects(n, textures, (x) => gl.deleteTexture(x)),
    moderngl_glDeleteVertexArrays: (n, arrays) => deleteObjects(n, arrays, (x) => gl.deleteVertexArray(x)),
    moderngl_glDepthFunc: (func) => gl.depthFunc(func),
    moderngl_glDepthMask: (flag) => gl.depthMask(!!flag),
    moderngl_glDetachShader: (program, shader) => gl.detachShader(get(program), get(shader)),
    moderngl_glDisable: (cap) => ignoredCaps.has(cap) || gl.disable(cap),
    moderngl_glDrawArraysInstanced: (mode, first, count, instances) => gl.drawArraysInstanced(mode, first, count, instances),
    moderngl_glDrawBuffers: (n, bufs) => gl.drawBuffers(wasm.HEAPU32.subarray(bufs >> 2, (bufs >> 2) + n)),
    moderngl_glDrawElementsInstanced: (mode, count, type, offset, instances) => gl.drawElementsInstanced(mode, count, type, offset, instances),
    moderngl_glEnable: (cap) => ignoredCaps.has(cap) || gl.enable(cap),
    moderngl_glEnableVertexAttribArray: (index) => gl.enableVertexAttribArray(index),
    moderngl_glFenceSync: (condition, flags) => add(gl.fenceSync(condition, flags)),
    moderngl_glFlush: () => gl.flush(),
    moderngl_glFramebufferRenderbuffer: (target, attachment, renderbufferTarget, renderbuffer) => gl.framebufferRenderbuffer(target, attachment, renderbufferTarget, get(renderbuffer)),
    moderngl_glFramebufferTexture2D: (target, attachment, textureTarget, texture, level) => gl.framebufferTexture2D(target, attachment, textureTarget, get(texture), level),
    moderngl_glFramebufferTextureLayer: (target, attachment, texture, level, layer) => gl.framebufferTextureLayer(target, attachment, get(texture), level, layer),
    moderngl_glGenBuffers: (n, buffers) => genObjects(n, buffers, () => gl.createBuffer()),
    moderngl_glGenFramebuffers: (n, framebuffers) => genObjects(n, framebuffers, () => gl.createFramebuffer()),
    moderngl_glGenRenderbuffers: (n, renderbuffers) => genObjects(n, renderbuffers, () => gl.createRenderbuffer()),
    moderngl_glGenSamplers: (n, samplers) => genObjects(n, samplers, () => gl.createSampler()),
    moderngl_glGenTextures: (n, textures) => genObjects(n, textures, () => gl.createTexture()),
    moderngl_glGenVertexArrays: (n, arrays) => genObjects(n, arrays, () => gl.createVertexArray()),
    moderngl_glGenerateMipmap: (target) => gl.generateMipmap(target),
    moderngl_glGetActiveUniform: (program, index, bufSize, length, size, type, name) => {
      const info = gl.getActiveUniform(get(program), index);
      setString(name, bufSize, length, info.name);
      wasm.HEAP32[size >> 2] = info.size;
      wasm.HEAPU32[type >> 2] = info.type;
    },
    moderngl_glGetActiveUniformBlockName: (program, index, bufSize, length, name) => {
      setString(name, bufSize, length, gl.getActiveUniformBlockName(get(program), index));
    },
    moderngl_glGetActiveUniformBlockiv: (program, index, pname, params) => {
      setIntegers(params, gl.getActiveUniformBlockParameter(get(program), index, pname));
    },
    moderngl_glGetBufferParameteriv: (target, pname, params) => setIntegers(params, gl.getBufferParameter(target, pname)),
    moderngl_glGetBufferSubData: (target, offset, size, data) => gl.getBufferSubData(target, offset, wasm.HEAPU8, data, size),
    moderngl_glGetFloatv: (pname, data) => setValues(wasm.HEAPF32, data, gl.getParameter(pname)),
    moderngl_glGetIntegeri_v: (target, index, data) => setIntegers(data, getParameter(gl.getIndexedParameter(target, index))),
    moderngl_glGetIntegerv: (pname, data) => setIntegers(data, getParameter(gl.getParameter(pname))),
    moderngl_glGetProgramInfoLog: (program, bufSize, length, log) => setString(log, bufSize, length, gl.getProgramInfoLog(get(program))),
    moderngl_glGetProgramiv: (program, pname, params) => setIntegers(params, gl.getProgramParameter(get(program), pname)),
    moderngl_glGetShaderInfoLog: (shader, bufSize, length, log) => setString(log, bufSize, length, gl.getShaderInfoLog(get(shader))),
    moderngl_glGetShaderiv: (shader, pname, params) => setIntegers(params, gl.getShaderParameter(get(shader), pname)),
    moderngl_glGetString: (name) => {
      if (!strings.has(name)) {
        const raw = encoder.encode(gl.getParameter(name));
        const ptr = wasm._malloc(raw.length + 1);
        wasm.HEAPU8.set(raw, ptr);
        wasm.HEAPU8[ptr + raw.length] = 0;
        strings.set(name, ptr);
      }
      return strings.get(name);
    },
    moderngl_glGetUniformBlockIndex: (program, name) => gl.getUniformBlockIndex(get(program), getString(name)),
    moderngl_glGetUniformLocation: (program, name) => {
      const key = `${program}:${getString(name)}`;
      if (!locations.has(key)) {
        const location = gl.getUniformLocation(get(program), getString(name));
        locations.set(key, location === null ? -1 : add(location));
      }
      return locations.get(key);
    },
    moderngl_glGetUniformfv: (program, location, params) => setValues(wasm.HEAPF32, params, gl.getUniform(get(program), get(location))),
    moderngl_glGetUniformiv: (program, location, params) => setValues(wasm.HEAP32, params, gl.getUniform(get(program), get(location))),
    moderngl_glGetUniformuiv: (program, location, params) => setValues(wasm.HEAPU32, params, gl.getUniform(get(program), get(location))),
    moderngl_glIsEnabled: (cap) => (gl.isEnabled(cap) ? 1 : 0),
    moderngl_glLinkProgram: (program) => gl.linkProgram(get(program)),
    moderngl_glPolygonOffset: (factor, units) => gl.polygonOffset(factor, units),
    moderngl_glReadBuffer: (src) => gl.readBuffer(src),
    moderngl_glReadPixels: (x, y, width, height, format, type, pixels) => {
      if (packBuffer) {
        gl.readPixels(x, y, width, height, format, type, pixels);
        return;
      }
      const [heap, shift] = pixelHeap(type);
      gl.readPixels(x, y, width, height, format, type, heap, pixels >> shift);
    },
    moderngl_glRenderbufferStorageMultisample: (target, samples, internalFormat, width, height) => gl.renderbufferStorageMultisample(target, samples, internalFormat, width, height),
    moderngl_glSamplerParameterf: (sampler, pname, param) => gl.samplerParameterf(get(sampler), pname, param),
    moderngl_glSamplerParameteri: (sampler, pname, param) => gl.samplerParameteri(get(sampler), pname, param),
    moderngl_glScissor: (x, y, width, height) => gl.scissor(x, y, width, height),
    moderngl_glShaderSource: (shader, count, string, length) => {
      const source = getString(wasm.HEAPU32[string >> 2]).replace(/^\s*#version 330 core/, shaderHeader);
      gl.shaderSource(get(shader), source);
    },
    moderngl_glStencilFunc: (func, ref, mask) => gl.stencilFunc(func, ref, mask),
    moderngl_glStencilFuncSeparate: (face, func, ref, mask) => gl.stencilFuncSeparate(face, func, ref, mask),
    moderngl_glStencilMask: (mask) => gl.stencilMask(mask),
    moderngl_glStencilOp: (fail, depthFail, depthPass) => gl.stencilOp(fail, depthFail, depthPass),
    moderngl_glStencilOpSeparate: (face, fail, depthFail, depthPass) => gl.stencilOpSeparate(face, fail, depthFail, depthPass),
    moderngl_glTexImage2D: (target, level, internalFormat, width, height, border, format, type, pixels) => {
      gl.texImage2D(target, level, internalFormat, width, height, border, format, type, null);
    },
    moderngl_glTexImage3D: (target, level, internalFormat, width, height, depth, border, format, type, pixels) => {
      gl.texImage3D(target, level, internalFormat, width, height, depth, border, format, type, null);
    },
    moderngl_glTexParameteri: (target, pname, param) => gl.texParameteri(target, pname, param),
    moderngl_glTexSubImage2D: (target, level, x, y, width, height, format, type, pixels) => {
      if (unpackBuffer) {
        gl.texSubImage2D(target, level, x, y, width, height, format, type, pixels);
        return;
      }
      const [heap, shift] = pixelHeap(type);
      gl.texSubImage2D(target, level, x, y, width, height, format, type, heap, pixels >> shift);
    },
    moderngl_glTexSubImage3D: (target, level, x, y, z, width, height, depth, format, type, pixels) => {
      if (unpackBuffer) {
        gl.texSubImage3D(target, level, x, y, z, width, height, depth, format, type, pixels);
        return;
      }
      const [heap, shift] = pixelHeap(type);
      gl.texSubImage3D(target, level, x, y, z, width, height, depth, format, type, heap, pixels >> shift);
    },
    moderngl_glUniform1fv: (location, count, value) => gl.uniform1fv(get(location), wasm.HEAPF32, value >> 2, count),
    moderngl_glUniform1i: (location, value) => gl.uniform1i(get(location), value),
    moderngl_glUniform1iv: (location, count, value) => gl.uniform1iv(get(location), wasm.HEAP32, value >> 2, count),
    moderngl_glUniform1uiv: (location, count, value) => gl.uniform1uiv(get(location), wasm.HEAPU32, value >> 2, count),
    moderngl_glUniform2fv: (location, count, value) => gl.uniform2fv(get(location), wasm.HEAPF32, value >> 2, count * 2),
    moderngl_glUniform2iv: (location, count, value) => gl.uniform2iv(get(location), wasm.HEAP32, value >> 2, count * 2),
    moderngl_glUniform2uiv: (location, count, value) => gl.uniform2uiv(get(location), wasm.HEAPU32, value >> 2, count * 2),
    moderngl_glUniform3fv: (location, count, value) => gl.uniform3fv(get(location), wasm.HEAPF32, value >> 2, count * 3),
    moderngl_glUniform3iv: (location, count, value) => gl.uniform3iv(get(location), wasm.HEAP32, value >> 2, count * 3),
    moderngl_glUniform3uiv: (location, count, value) => gl.uniform3uiv(get(location), wasm.HEAPU32, value >> 2, count * 3),
    moderngl_glUniform4fv: (location, count, value) => gl.uniform4fv(get(location), wasm.HEAPF32, value >> 2, count * 4),
    moderngl_glUniform4iv: (location, count, value) => gl.uniform4iv(get(location), wasm.HEAP32, value >> 2, count * 4),
    moderngl_glUniform4uiv: (location, count, value) => gl.uniform4uiv(get(location), wasm.HEAPU32, value >> 2, count * 4),
    moderngl_glUniformBlockBinding: (program, index, binding) => gl.uniformBlockBinding(get(program), index, binding),
    moderngl_glUniformMatrix2fv: (location, count, transpose, value) => gl.uniformMatrix2fv(get(location), false, wasm.HEAPF32, value >> 2, count * 4),
    moderngl_glUniformMatrix2x3fv: (location, count, transpose, value) => gl.uniformMatrix2x3fv(get(location), false, wasm.HEAPF32, value >> 2, count * 6),
    moderngl_glUniformMatrix2x4fv: (location, count, transpose, value) => gl.uniformMatrix2x4fv(get(location), false, wasm.HEAPF32, value >> 2, count * 8),
    moderngl_glUniformMatrix3fv: (location, count, transpose, value) => gl.uniformMatrix3fv(get(location), false, wasm.HEAPF32, value >> 2, count * 9),
    moderngl_glUniformMatrix3x2fv: (location, count, transpose, value) => gl.uniformMatrix3x2fv(get(location), false, wasm.HEAPF32, value >> 2, count * 6),
    moderngl_glUniformMatrix3x4fv: (location, count, transpose, value) => gl.uniformMatrix3x4fv(get(location), false, wasm.HEAPF32, value >> 2, count * 12),
    moderngl_glUniformMatrix4fv: (location, count, transpose, value) => gl.uniformMatrix4fv(get(location), false, wasm.HEAPF32, value >> 2, count * 16),
    moderngl_glUniformMatrix4x2fv: (location, count, transpose, value) => gl.uniformMatrix4x2fv(get(location), false, wasm.HEAPF32, value >> 2, count * 8),
    moderngl_glUniformMatrix4x3fv: (location, count, transpose, value) => gl.uniformMatrix4x3fv(get(location), false, wasm.HEAPF32, value >> 2, count * 12),
    moderngl_glUseProgram: (program) => gl.useProgram(get(program)),
    moderngl_glVertexAttribDivisor: (index, divisor) => gl.vertexAttribDivisor(index, divisor),
    moderngl_glVertexAttribIPointer: (index, size, type, stride, offset) => gl.vertexAttribIPointer(index, size, type, stride, offset),
    moderngl_glVertexAttribPointer: (index, size, type, normalized, stride, offset) => gl.vertexAttribPointer(index, size, type, !!normalized, stride, offset),
    moderngl_glViewport: (x, y, width, height) => gl.viewport(x, y, width, height),
  };
}
