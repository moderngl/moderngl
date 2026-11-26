(wasm, device, context) => {
  const decoder = new TextDecoder();
  const encoder = new TextEncoder();

  const canvasFormat = context ? navigator.gpu.getPreferredCanvasFormat() : 'rgba8unorm';
  const textureUsage = GPUTextureUsage.TEXTURE_BINDING | GPUTextureUsage.COPY_SRC | GPUTextureUsage.COPY_DST;
  const bufferUsage = GPUBufferUsage.VERTEX | GPUBufferUsage.INDEX | GPUBufferUsage.UNIFORM | GPUBufferUsage.STORAGE | GPUBufferUsage.INDIRECT | GPUBufferUsage.COPY_SRC | GPUBufferUsage.COPY_DST;
  const floatFilterable = device.features.has('float32-filterable');

  if (context) {
    context.configure({ device, format: canvasFormat, alphaMode: 'opaque' });
  }

  const formats = {
    0x8229: ['r8unorm', 1, 'float'],
    0x822B: ['rg8unorm', 2, 'float'],
    0x8058: ['rgba8unorm', 4, 'float'],
    0x8F94: ['r8snorm', 1, 'float'],
    0x8F95: ['rg8snorm', 2, 'float'],
    0x8F97: ['rgba8snorm', 4, 'float'],
    0x8232: ['r8uint', 1, 'uint'],
    0x8238: ['rg8uint', 2, 'uint'],
    0x8D7C: ['rgba8uint', 4, 'uint'],
    0x8234: ['r16uint', 2, 'uint'],
    0x823A: ['rg16uint', 4, 'uint'],
    0x8D76: ['rgba16uint', 8, 'uint'],
    0x8236: ['r32uint', 4, 'uint'],
    0x823C: ['rg32uint', 8, 'uint'],
    0x8D70: ['rgba32uint', 16, 'uint'],
    0x8231: ['r8sint', 1, 'sint'],
    0x8237: ['rg8sint', 2, 'sint'],
    0x8D8E: ['rgba8sint', 4, 'sint'],
    0x8233: ['r16sint', 2, 'sint'],
    0x8239: ['rg16sint', 4, 'sint'],
    0x8D88: ['rgba16sint', 8, 'sint'],
    0x8235: ['r32sint', 4, 'sint'],
    0x823B: ['rg32sint', 8, 'sint'],
    0x8D82: ['rgba32sint', 16, 'sint'],
    0x822D: ['r16float', 2, 'float'],
    0x822F: ['rg16float', 4, 'float'],
    0x881A: ['rgba16float', 8, 'float'],
    0x822E: ['r32float', 4, floatFilterable ? 'float' : 'unfilterable-float'],
    0x8230: ['rg32float', 8, floatFilterable ? 'float' : 'unfilterable-float'],
    0x8814: ['rgba32float', 16, floatFilterable ? 'float' : 'unfilterable-float'],
    0x8059: ['rgb10a2unorm', 4, 'float'],
    0x81A5: ['depth16unorm', 2, 'depth'],
    0x81A6: ['depth24plus', 4, 'depth'],
    0x88F0: ['depth24plus-stencil8', 4, 'depth'],
    0x8CAC: ['depth32float', 4, 'depth'],
  };

  const compareFuncs = ['never', 'less', 'equal', 'less-equal', 'greater', 'not-equal', 'greater-equal', 'always'];
  const topologies = ['point-list', 'line-list', 'line-strip', 'line-strip', 'triangle-list', 'triangle-strip', 'triangle-strip'];
  const storageFormats = new Set(['rgba8unorm', 'rgba8snorm', 'rgba8uint', 'rgba8sint', 'rgba16uint', 'rgba16sint', 'rgba16float', 'r32uint', 'r32sint', 'r32float', 'rg32uint', 'rg32sint', 'rg32float', 'rgba32uint', 'rgba32sint', 'rgba32float']);
  const wraps = { 0x2901: 'repeat', 0x8370: 'mirror-repeat', 0x812F: 'clamp-to-edge' };

  const blendFactors = {
    0x0000: 'zero',
    0x0001: 'one',
    0x0300: 'src',
    0x0301: 'one-minus-src',
    0x0306: 'dst',
    0x0307: 'one-minus-dst',
    0x0302: 'src-alpha',
    0x0303: 'one-minus-src-alpha',
    0x0304: 'dst-alpha',
    0x0305: 'one-minus-dst-alpha',
    0x8001: 'constant',
    0x8002: 'one-minus-constant',
    0x8003: 'constant',
    0x8004: 'one-minus-constant',
    0x0308: 'src-alpha-saturated',
  };

  const blendOps = { 0x8006: 'add', 0x800A: 'subtract', 0x800B: 'reverse-subtract', 0x8007: 'min', 0x8008: 'max' };

  const stencilOps = {
    0x1E00: 'keep',
    0x0000: 'zero',
    0x1E01: 'replace',
    0x1E02: 'increment-clamp',
    0x1E03: 'decrement-clamp',
    0x150A: 'invert',
    0x8507: 'increment-wrap',
    0x8508: 'decrement-wrap',
  };

  const vertexTypes = {
    0x1400: ['sint8', 'snorm8', 1],
    0x1401: ['uint8', 'unorm8', 1],
    0x1402: ['sint16', 'snorm16', 2],
    0x1403: ['uint16', 'unorm16', 2],
    0x1404: ['sint32', 'sint32', 4],
    0x1405: ['uint32', 'uint32', 4],
    0x1406: ['float32', 'float32', 4],
    0x140B: ['float16', 'float16', 2],
  };

  const objects = new Map();
  const strings = new Map();
  const pipelines = new Map();
  const layouts = new Map();
  const blitPipelines = new Map();
  const pendingReads = new Set();
  let nextId = 1;

  const add = (obj) => {
    const id = nextId++;
    objects.set(id, obj);
    return id;
  };

  const get = (id) => objects.get(id);
  const pad4 = (x) => (x + 3) & ~3;
  const pad256 = (x) => (x + 255) & ~255;

  const getString = (ptr) => {
    const end = wasm.HEAPU8.indexOf(0, ptr);
    return decoder.decode(wasm.HEAPU8.subarray(ptr, end));
  };

  const createTexture = (tex, internalFormat, format, width, height, layers, levels, samples, cube) => {
    const [name, pixelSize, sampleType] = formats[internalFormat];
    tex.format = format === 0x80E1 ? 'bgra8unorm' : name;
    tex.pixelSize = pixelSize;
    tex.sampleType = sampleType;
    tex.width = width;
    tex.height = height;
    tex.layers = layers;
    tex.levels = levels;
    tex.samples = samples;
    tex.depth = sampleType === 'depth';
    tex.stencil = tex.format.includes('stencil');
    tex.viewDimension = cube ? 'cube' : tex.target === 0x8C1A ? '2d-array' : '2d';
    const renderable = tex.format.includes('snorm') ? 0 : GPUTextureUsage.RENDER_ATTACHMENT;
    const storage = storageFormats.has(tex.format) ? GPUTextureUsage.STORAGE_BINDING : 0;
    const usage = samples > 1 ? GPUTextureUsage.RENDER_ATTACHMENT : textureUsage | renderable | storage;
    tex.texture = device.createTexture({ size: [width, height, layers], format: tex.format, usage, mipLevelCount: levels, sampleCount: samples });
    if (samples === 1) {
      tex.view = tex.texture.createView({ dimension: tex.viewDimension, aspect: tex.depth ? 'depth-only' : 'all' });
    }
  };

  const screenColor = { screen: !!context, target: 0x8D41, view: null };
  const screenDepth = { target: 0x8D41 };
  const width = context ? context.canvas.width : 1;
  const height = context ? context.canvas.height : 1;

  if (context) {
    Object.assign(screenColor, { format: canvasFormat, pixelSize: 4, sampleType: 'float', width, height, layers: 1, levels: 1, samples: 1 });
  } else {
    createTexture(screenColor, 0x8058, 0, width, height, 1, 1, 1, false);
  }

  createTexture(screenDepth, 0x88F0, 0, width, height, 1, 1, 1, false);

  const textureOf = (tex) => (tex.screen ? context.getCurrentTexture() : tex.texture);

  const attachmentView = (att) => {
    if (att.tex.screen) {
      return context.getCurrentTexture().createView();
    }
    return att.view;
  };

  const newAttachment = (tex, level, layer) => {
    const view = tex.screen ? null : tex.texture.createView({ dimension: '2d', baseMipLevel: level, mipLevelCount: 1, baseArrayLayer: layer, arrayLayerCount: 1 });
    return { tex, level, layer, view };
  };

  const screenFramebuffer = { colors: [newAttachment(screenColor, 0, 0)], depth: newAttachment(screenDepth, 0, 0), colorCount: 1, read: 0 };
  const defaultVertexArray = { attribs: [], indexBuffer: 0 };

  const state = {
    arrayBuffer: 0,
    copyReadBuffer: 0,
    copyWriteBuffer: 0,
    packBuffer: 0,
    unpackBuffer: 0,
    uniformBuffer: 0,
    storageBuffer: 0,
    indirectBuffer: 0,
    uniformBuffers: [],
    storageBuffers: [],
    storageImages: [],
    activeUnit: 0,
    textures: [],
    samplers: [],
    program: 0,
    vertexArray: 0,
    drawFramebuffer: 0,
    readFramebuffer: 0,
    renderbuffer: 0,
    viewport: [0, 0, width, height],
    scissorTest: 0,
    scissor: [0, 0, width, height],
    pipeline: {
      depthTest: 0,
      depthWrite: 1,
      depthFunc: 0x0201,
      cull: 0,
      cullFace: 0x0405,
      blend: 0,
      blendFunc: [1, 0, 1, 0],
      blendEquation: [0x8006, 0x8006],
      stencil: 0,
      stencilFunc: [0x0207, 0x0207],
      stencilRef: [0, 0],
      stencilReadMask: [0xFF, 0xFF],
      stencilFail: [0x1E00, 0x1E00],
      stencilDepthFail: [0x1E00, 0x1E00],
      stencilDepthPass: [0x1E00, 0x1E00],
      stencilWriteMask: 0xFF,
      polygonOffset: 0,
      polygonOffsetFactor: 0,
      polygonOffsetUnits: 0,
    },
  };

  let commandEncoder = null;
  let pass = null;
  let passFramebuffer = -1;

  const getEncoder = () => {
    commandEncoder ??= device.createCommandEncoder();
    return commandEncoder;
  };

  const endPass = () => {
    if (pass) {
      pass.end();
      pass = null;
      passFramebuffer = -1;
    }
    return getEncoder();
  };

  const submit = () => {
    endPass();
    device.queue.submit([commandEncoder.finish()]);
    commandEncoder = null;
  };

  const getFramebuffer = (id) => (id ? get(id) : screenFramebuffer);
  const getVertexArray = () => (state.vertexArray ? get(state.vertexArray) : defaultVertexArray);

  const bufferSlot = (target) => {
    switch (target) {
      case 0x8892: return [state, 'arrayBuffer'];
      case 0x8893: return [getVertexArray(), 'indexBuffer'];
      case 0x8F36: return [state, 'copyReadBuffer'];
      case 0x8F37: return [state, 'copyWriteBuffer'];
      case 0x88EB: return [state, 'packBuffer'];
      case 0x88EC: return [state, 'unpackBuffer'];
      case 0x8A11: return [state, 'uniformBuffer'];
      case 0x90D2: return [state, 'storageBuffer'];
      case 0x8F3F: return [state, 'indirectBuffer'];
    }
    return [state, 'copyWriteBuffer'];
  };

  const boundBuffer = (target) => {
    const [obj, key] = bufferSlot(target);
    return get(obj[key]);
  };

  const boundTexture = () => get(state.textures[state.activeUnit]);

  const stagingBuffer = (size, fill) => {
    const buffer = device.createBuffer({ size: pad4(Math.max(size, 4)), usage: GPUBufferUsage.COPY_SRC, mappedAtCreation: true });
    fill(new Uint8Array(buffer.getMappedRange()));
    buffer.unmap();
    return buffer;
  };

  const copyBuffer = (src, srcOffset, dst, dstOffset, size) => {
    getEncoder().copyBufferToBuffer(src, srcOffset, dst.buffer, dstOffset, pad4(size));
    if (dst.read) {
      pendingReads.add(dst);
    }
  };

  const uploadBuffer = (buf, offset, data, size) => {
    const staging = stagingBuffer(size, (mapped) => mapped.set(wasm.HEAPU8.subarray(data, data + size)));
    endPass();
    copyBuffer(staging, 0, buf, offset, size);
  };

  const getSampler = (id) => {
    const sampler = id ? get(id) : null;
    if (!sampler) {
      return device.createSampler();
    }
    if (!sampler.sampler) {
      const mipmaps = sampler.minFilter >= 0x2700;
      const linear = sampler.minFilter === 0x2601 || sampler.minFilter === 0x2701 || sampler.minFilter === 0x2703;
      const filters = {
        magFilter: sampler.magFilter === 0x2601 ? 'linear' : 'nearest',
        minFilter: linear ? 'linear' : 'nearest',
        mipmapFilter: sampler.minFilter === 0x2702 || sampler.minFilter === 0x2703 ? 'linear' : 'nearest',
      };
      const anisotropic = sampler.anisotropy > 1 && filters.magFilter === 'linear' && filters.minFilter === 'linear' && filters.mipmapFilter === 'linear';
      sampler.filtering = filters.magFilter === 'linear' || filters.minFilter === 'linear' || filters.mipmapFilter === 'linear';
      sampler.sampler = device.createSampler({
        ...filters,
        addressModeU: wraps[sampler.wrap[0]],
        addressModeV: wraps[sampler.wrap[1]],
        addressModeW: wraps[sampler.wrap[2]],
        lodMinClamp: mipmaps ? Math.max(sampler.minLod, 0) : 0,
        lodMaxClamp: mipmaps ? Math.min(Math.max(sampler.maxLod, Math.max(sampler.minLod, 0)), 32) : 0,
        compare: sampler.compareMode ? compareFuncs[sampler.compareFunc - 0x0200] : undefined,
        maxAnisotropy: anisotropic ? Math.min(sampler.anisotropy, 16) : 1,
      });
    }
    return sampler.sampler;
  };

  const bindings = (compute) => {
    const layoutEntries = [];
    const groupEntries = [];
    const visibility = compute ? GPUShaderStage.COMPUTE : GPUShaderStage.VERTEX | GPUShaderStage.FRAGMENT;
    for (let i = 0; i < 8; ++i) {
      const range = state.uniformBuffers[i];
      if (range && range.buffer) {
        layoutEntries.push({ binding: i, visibility, buffer: { type: 'uniform' } });
        groupEntries.push({ binding: i, resource: { buffer: get(range.buffer).buffer, offset: range.offset, size: range.size } });
      }
    }
    for (let i = 0; i < 8; ++i) {
      const range = state.storageBuffers[i];
      if (range && range.buffer) {
        const type = compute ? 'storage' : 'read-only-storage';
        layoutEntries.push({ binding: 32 + i, visibility, buffer: { type } });
        groupEntries.push({ binding: 32 + i, resource: { buffer: get(range.buffer).buffer, offset: range.offset, size: range.size } });
      }
    }
    for (let i = 0; i < 16; ++i) {
      const tex = get(state.textures[i]);
      if (tex && tex.view) {
        const sampler = getSampler(state.samplers[i]);
        const samplerInfo = state.samplers[i] ? get(state.samplers[i]) : null;
        const comparison = samplerInfo && samplerInfo.compareMode;
        const filtering = samplerInfo && samplerInfo.filtering && tex.sampleType === 'float';
        const samplerType = comparison ? 'comparison' : filtering ? 'filtering' : 'non-filtering';
        layoutEntries.push({ binding: 64 + i, visibility, texture: { sampleType: tex.sampleType, viewDimension: tex.viewDimension } });
        layoutEntries.push({ binding: 96 + i, visibility, sampler: { type: samplerType } });
        groupEntries.push({ binding: 64 + i, resource: tex.view });
        groupEntries.push({ binding: 96 + i, resource: sampler });
      }
    }
    for (let i = 0; i < 4 && compute; ++i) {
      const image = state.storageImages[i];
      if (image) {
        const tex = image.tex;
        const access = /^r32/.test(tex.format) ? 'read-write' : 'write-only';
        const viewDimension = image.layered ? '2d-array' : '2d';
        const view = tex.texture.createView({
          dimension: viewDimension,
          baseMipLevel: image.level,
          mipLevelCount: 1,
          baseArrayLayer: image.layered ? 0 : image.layer,
          arrayLayerCount: image.layered ? tex.layers : 1,
        });
        layoutEntries.push({ binding: 128 + i, visibility, storageTexture: { access, format: tex.format, viewDimension } });
        groupEntries.push({ binding: 128 + i, resource: view });
      }
    }
    const key = JSON.stringify(layoutEntries);
    if (!layouts.has(key)) {
      const bindGroupLayout = device.createBindGroupLayout({ entries: layoutEntries });
      const pipelineLayout = device.createPipelineLayout({ bindGroupLayouts: [bindGroupLayout] });
      layouts.set(key, { bindGroupLayout, pipelineLayout });
    }
    const layout = layouts.get(key);
    const bindGroup = device.createBindGroup({ layout: layout.bindGroupLayout, entries: groupEntries });
    return { key, layout, bindGroup };
  };

  const vertexGroups = () => {
    const attribs = getVertexArray().attribs
      .map((attrib, location) => (attrib && attrib.enabled ? { ...attrib, location } : null))
      .filter((attrib) => attrib)
      .sort((a, b) => a.offset - b.offset);
    const groups = [];
    for (const attrib of attribs) {
      const group = groups.find((g) => g.buffer === attrib.buffer && g.stride === attrib.stride && g.rate === attrib.rate && attrib.offset - g.offset + attrib.size <= attrib.stride);
      if (group) {
        group.attributes.push({ shaderLocation: attrib.location, format: attrib.format, offset: attrib.offset - group.offset });
        continue;
      }
      groups.push({
        buffer: attrib.buffer,
        stride: attrib.stride,
        rate: attrib.rate,
        offset: attrib.offset,
        attributes: [{ shaderLocation: attrib.location, format: attrib.format, offset: 0 }],
      });
    }
    return groups;
  };

  const beginPass = () => {
    if (pass && passFramebuffer === state.drawFramebuffer) {
      return;
    }
    const enc = endPass();
    const framebuffer = getFramebuffer(state.drawFramebuffer);
    const colorAttachments = framebuffer.colors.slice(0, framebuffer.colorCount).map((att) => ({ view: attachmentView(att), loadOp: 'load', storeOp: 'store' }));
    let depthStencilAttachment;
    if (framebuffer.depth) {
      depthStencilAttachment = { view: framebuffer.depth.view, depthLoadOp: 'load', depthStoreOp: 'store' };
      if (framebuffer.depth.tex.stencil) {
        Object.assign(depthStencilAttachment, { stencilLoadOp: 'load', stencilStoreOp: 'store' });
      }
    }
    pass = enc.beginRenderPass({ colorAttachments, depthStencilAttachment });
    passFramebuffer = state.drawFramebuffer;
  };

  const stencilFace = (s, i) => ({
    compare: s.stencil ? compareFuncs[s.stencilFunc[i] - 0x0200] : 'always',
    failOp: s.stencil ? stencilOps[s.stencilFail[i]] : 'keep',
    depthFailOp: s.stencil ? stencilOps[s.stencilDepthFail[i]] : 'keep',
    passOp: s.stencil ? stencilOps[s.stencilDepthPass[i]] : 'keep',
  });

  const blendable = (format) => !/int/.test(format) && (!/32float/.test(format) || device.features.has('float32-blendable'));

  const prepareDraw = (mode, indexFormat) => {
    beginPass();
    const framebuffer = getFramebuffer(state.drawFramebuffer);
    const program = get(state.program);
    const colors = framebuffer.colors.slice(0, framebuffer.colorCount).map((att) => att.tex.format);
    const depth = framebuffer.depth ? framebuffer.depth.tex : null;
    const samples = (framebuffer.colorCount ? framebuffer.colors[0].tex : depth).samples;
    const groups = vertexGroups();
    const bound = bindings(false);
    const topology = topologies[mode];
    const stripIndexFormat = topology.endsWith('strip') ? indexFormat : undefined;
    const s = state.pipeline;
    const vertexLayout = groups.map((g) => ({ arrayStride: g.stride, stepMode: g.rate, attributes: g.attributes }));
    const key = JSON.stringify([state.program, topology, stripIndexFormat, colors, depth && depth.format, samples, vertexLayout, bound.key, s]);

    if (!pipelines.has(key)) {
      const blend = {
        color: { operation: blendOps[s.blendEquation[0]], srcFactor: blendFactors[s.blendFunc[0]], dstFactor: blendFactors[s.blendFunc[1]] },
        alpha: { operation: blendOps[s.blendEquation[1]], srcFactor: blendFactors[s.blendFunc[2]], dstFactor: blendFactors[s.blendFunc[3]] },
      };
      let depthStencil;
      if (depth) {
        depthStencil = {
          format: depth.format,
          depthWriteEnabled: !!(s.depthTest && s.depthWrite),
          depthCompare: s.depthTest ? compareFuncs[s.depthFunc - 0x0200] : 'always',
        };
        if (s.polygonOffset && topology.startsWith('triangle')) {
          Object.assign(depthStencil, { depthBias: Math.round(s.polygonOffsetUnits), depthBiasSlopeScale: s.polygonOffsetFactor });
        }
        if (depth.stencil) {
          Object.assign(depthStencil, {
            stencilFront: stencilFace(s, 0),
            stencilBack: stencilFace(s, 1),
            stencilReadMask: s.stencilReadMask[0],
            stencilWriteMask: s.stencilWriteMask,
          });
        }
      }
      pipelines.set(key, device.createRenderPipeline({
        layout: bound.layout.pipelineLayout,
        vertex: { module: program.vertex, buffers: vertexLayout },
        fragment: { module: program.fragment, targets: colors.map((format) => ({ format, blend: s.blend && blendable(format) ? blend : undefined })) },
        primitive: {
          topology,
          stripIndexFormat,
          frontFace: 'ccw',
          cullMode: s.cull ? { 0x0404: 'front', 0x0405: 'back' }[s.cullFace] ?? 'none' : 'none',
        },
        depthStencil,
        multisample: { count: samples },
      }));
    }

    pass.setPipeline(pipelines.get(key));
    pass.setBindGroup(0, bound.bindGroup);
    groups.forEach((g, i) => pass.setVertexBuffer(i, get(g.buffer).buffer, g.offset));
    const [x, y, w, h] = state.viewport;
    pass.setViewport(x, y, w, h, 0, 1);
    const target = framebuffer.colorCount ? framebuffer.colors[0] : framebuffer.depth;
    const targetWidth = Math.max(target.tex.width >> target.level, 1);
    const targetHeight = Math.max(target.tex.height >> target.level, 1);
    const [sx, sy, sw, sh] = state.scissorTest ? state.scissor : [0, 0, targetWidth, targetHeight];
    const left = Math.min(Math.max(sx, 0), targetWidth);
    const top = Math.min(Math.max(sy, 0), targetHeight);
    const right = Math.min(Math.max(sx + sw, left), targetWidth);
    const bottom = Math.min(Math.max(sy + sh, top), targetHeight);
    pass.setScissorRect(left, top, right - left, bottom - top);
    pass.setStencilReference(s.stencilRef[0]);
  };

  const indexFormat = (type) => (type === 0x1405 ? 'uint32' : 'uint16');
  const indexSize = (type) => (type === 0x1405 ? 4 : 2);

  const bindIndexBuffer = (type) => {
    pass.setIndexBuffer(get(getVertexArray().indexBuffer).buffer, indexFormat(type));
  };

  const attach = (target, attachment, id, level, layer) => {
    const framebuffer = getFramebuffer(target === 0x8CA8 ? state.readFramebuffer : state.drawFramebuffer);
    const value = id ? newAttachment(get(id), level, layer) : null;
    if (attachment === 0x8D00 || attachment === 0x821A) {
      framebuffer.depth = value;
      return;
    }
    framebuffer.colors[attachment - 0x8CE0] = value;
  };

  const clearPass = (att, color, depthValue, stencilValue) => {
    const enc = endPass();
    if (color) {
      enc.beginRenderPass({ colorAttachments: [{ view: attachmentView(att), loadOp: 'clear', storeOp: 'store', clearValue: color }] }).end();
      return;
    }
    const depthStencilAttachment = { view: att.view, depthLoadOp: 'clear', depthStoreOp: 'store', depthClearValue: depthValue };
    if (att.tex.stencil) {
      Object.assign(depthStencilAttachment, { stencilLoadOp: stencilValue === null ? 'load' : 'clear', stencilStoreOp: 'store', stencilClearValue: stencilValue ?? 0 });
    }
    enc.beginRenderPass({ colorAttachments: [], depthStencilAttachment }).end();
  };

  const blitShader = `
    @group(0) @binding(0) var<uniform> rect: vec4f;
    @group(0) @binding(1) var source: texture_2d<f32>;
    @group(0) @binding(2) var source_sampler: sampler;
    struct Varyings {
      @builtin(position) position: vec4f,
      @location(0) uv: vec2f,
    };
    @vertex fn vertex_main(@builtin(vertex_index) index: u32) -> Varyings {
      let p = vec2f(f32(index & 1u), f32(index >> 1u));
      var out: Varyings;
      out.position = vec4f(p * 2.0 - 1.0, 0.0, 1.0);
      out.uv = vec2f(mix(rect.x, rect.z, p.x), mix(rect.w, rect.y, p.y));
      return out;
    }
    @fragment fn fragment_main(in: Varyings) -> @location(0) vec4f {
      return textureSample(source, source_sampler, in.uv);
    }
  `;

  const blit = (src, srcLevel, srcLayer, sx0, sy0, sx1, sy1, dst, dx0, dy0, dx1, dy1, linear) => {
    if (!blitPipelines.has(dst.tex.format)) {
      const module = device.createShaderModule({ code: blitShader });
      blitPipelines.set(dst.tex.format, device.createRenderPipeline({
        layout: 'auto',
        vertex: { module, entryPoint: 'vertex_main' },
        fragment: { module, entryPoint: 'fragment_main', targets: [{ format: dst.tex.format }] },
        primitive: { topology: 'triangle-strip' },
      }));
    }
    const pipeline = blitPipelines.get(dst.tex.format);
    const w = Math.max(src.width >> srcLevel, 1);
    const h = Math.max(src.height >> srcLevel, 1);
    const flipX = (dx0 < dx1) !== (sx0 < sx1);
    const flipY = (dy0 < dy1) !== (sy0 < sy1);
    const [left, right] = flipX ? [Math.max(sx0, sx1), Math.min(sx0, sx1)] : [Math.min(sx0, sx1), Math.max(sx0, sx1)];
    const [top, bottom] = flipY ? [Math.max(sy0, sy1), Math.min(sy0, sy1)] : [Math.min(sy0, sy1), Math.max(sy0, sy1)];
    const rect = new Float32Array([left / w, top / h, right / w, bottom / h]);
    const uniform = device.createBuffer({ size: 16, usage: GPUBufferUsage.UNIFORM, mappedAtCreation: true });
    new Float32Array(uniform.getMappedRange()).set(rect);
    uniform.unmap();
    const view = src.texture.createView({ dimension: '2d', baseMipLevel: srcLevel, mipLevelCount: 1, baseArrayLayer: srcLayer, arrayLayerCount: 1 });
    const filter = linear ? 'linear' : 'nearest';
    const bindGroup = device.createBindGroup({
      layout: pipeline.getBindGroupLayout(0),
      entries: [
        { binding: 0, resource: { buffer: uniform } },
        { binding: 1, resource: view },
        { binding: 2, resource: device.createSampler({ minFilter: filter, magFilter: filter }) },
      ],
    });
    const enc = endPass();
    const blitPass = enc.beginRenderPass({ colorAttachments: [{ view: attachmentView(dst), loadOp: 'load', storeOp: 'store' }] });
    blitPass.setPipeline(pipeline);
    blitPass.setBindGroup(0, bindGroup);
    blitPass.setViewport(Math.min(dx0, dx1), Math.min(dy0, dy1), Math.abs(dx1 - dx0), Math.abs(dy1 - dy0), 0, 1);
    blitPass.draw(4);
    blitPass.end();
  };

  const setString = (name, text) => {
    if (!strings.has(name)) {
      const raw = encoder.encode(text);
      const ptr = wasm._malloc(raw.length + 1);
      wasm.HEAPU8.set(raw, ptr);
      wasm.HEAPU8[ptr + raw.length] = 0;
      strings.set(name, ptr);
    }
    return strings.get(name);
  };

  const setCapability = (cap, value) => {
    switch (cap) {
      case 0x0B71: state.pipeline.depthTest = value; break;
      case 0x0B44: state.pipeline.cull = value; break;
      case 0x0BE2: state.pipeline.blend = value; break;
      case 0x0B90: state.pipeline.stencil = value; break;
      case 0x0C11: state.scissorTest = value; break;
      case 0x8037:
        Object.assign(state.pipeline, { polygonOffset: value, polygonOffsetFactor: 0, polygonOffsetUnits: 0 });
        break;
    }
  };

  const setStencilFunc = (face, func, ref, mask) => {
    for (let i = 0; i < 2; ++i) {
      if (face === 0x0408 || face === (i ? 0x0405 : 0x0404)) {
        state.pipeline.stencilFunc[i] = func;
        state.pipeline.stencilRef[i] = ref;
        state.pipeline.stencilReadMask[i] = mask & 0xFF;
      }
    }
  };

  const setStencilOp = (face, fail, depthFail, depthPass) => {
    for (let i = 0; i < 2; ++i) {
      if (face === 0x0408 || face === (i ? 0x0405 : 0x0404)) {
        state.pipeline.stencilFail[i] = fail;
        state.pipeline.stencilDepthFail[i] = depthFail;
        state.pipeline.stencilDepthPass[i] = depthPass;
      }
    }
  };

  const genObjects = (n, ptr, create) => {
    for (let i = 0; i < n; ++i) {
      wasm.HEAPU32[(ptr >> 2) + i] = add(create());
    }
  };

  const deleteObjects = (n, ptr, destroy) => {
    if (commandEncoder) {
      submit();
    }
    for (let i = 0; i < n; ++i) {
      const id = wasm.HEAPU32[(ptr >> 2) + i];
      const obj = get(id);
      if (obj) {
        destroy(obj);
      }
      objects.delete(id);
    }
  };

  const setRange = (target, index, buffer, offset, size) => {
    const list = target === 0x8A11 ? state.uniformBuffers : state.storageBuffers;
    list[index] = { buffer, offset, size };
    const [obj, key] = bufferSlot(target);
    obj[key] = buffer;
  };

  const vertexAttrib = (index, size, type, integer, normalized, stride, offset) => {
    const vertexArray = getVertexArray();
    const [integerName, normalizedName, typeSize] = vertexTypes[type];
    const name = integer || !normalized ? integerName : normalizedName;
    const previous = vertexArray.attribs[index];
    vertexArray.attribs[index] = {
      buffer: state.arrayBuffer,
      format: size > 1 ? `${name}x${size}` : name,
      size: size * typeSize,
      stride: stride || size * typeSize,
      offset,
      rate: previous ? previous.rate : 'vertex',
      enabled: previous ? previous.enabled : false,
    };
  };

  const texSubImage = (tex, level, x, y, layer, width, height, depth, pixels) => {
    const row = pad4(width * tex.pixelSize);
    const padded = pad256(width * tex.pixelSize);
    const enc = endPass();
    let buffer;
    if (state.unpackBuffer) {
      const source = get(state.unpackBuffer).buffer;
      buffer = device.createBuffer({ size: padded * height * depth, usage: GPUBufferUsage.COPY_SRC | GPUBufferUsage.COPY_DST });
      for (let i = 0; i < height * depth; ++i) {
        enc.copyBufferToBuffer(source, pixels + i * row, buffer, i * padded, row);
      }
    } else {
      buffer = stagingBuffer(padded * height * depth, (mapped) => {
        for (let i = 0; i < height * depth; ++i) {
          mapped.set(wasm.HEAPU8.subarray(pixels + i * row, pixels + i * row + width * tex.pixelSize), i * padded);
        }
      });
    }
    enc.copyBufferToTexture({ buffer, bytesPerRow: padded, rowsPerImage: height }, { texture: tex.texture, mipLevel: level, origin: { x, y, z: layer } }, [width, height, depth]);
  };

  const texImage = (target, internalFormat, width, height, depth, format) => {
    const tex = boundTexture();
    if (tex.texture) {
      return;
    }
    const cube = target >= 0x8515 && target <= 0x851A;
    createTexture(tex, internalFormat, format, width, height, cube ? 6 : depth, (tex.maxLevel ?? 0) + 1, 1, cube);
  };

  const samplerParameter = (id, pname, value) => {
    const sampler = get(id);
    sampler.sampler = null;
    switch (pname) {
      case 0x2801: sampler.minFilter = value; break;
      case 0x2800: sampler.magFilter = value; break;
      case 0x2802: sampler.wrap[0] = value; break;
      case 0x2803: sampler.wrap[1] = value; break;
      case 0x8072: sampler.wrap[2] = value; break;
      case 0x884C: sampler.compareMode = value; break;
      case 0x884D: sampler.compareFunc = value; break;
      case 0x813A: sampler.minLod = value; break;
      case 0x813B: sampler.maxLod = value; break;
      case 0x84FE: sampler.anisotropy = value; break;
    }
  };

  const drawIndirect = (mode, type, offset, count, stride) => {
    prepareDraw(mode, type ? indexFormat(type) : undefined);
    const buffer = get(state.indirectBuffer).buffer;
    if (type) {
      bindIndexBuffer(type);
    }
    for (let i = 0; i < count; ++i) {
      if (type) {
        pass.drawIndexedIndirect(buffer, offset + i * (stride || 20));
      } else {
        pass.drawIndirect(buffer, offset + i * (stride || 16));
      }
    }
  };

  const noop = () => {};

  return {
    moderngl_glActiveTexture: (texture) => {
      state.activeUnit = texture - 0x84C0;
    },
    moderngl_glAttachShader: (program, shader) => get(program).shaders.push(get(shader)),
    moderngl_glBindBuffer: (target, buffer) => {
      const [obj, key] = bufferSlot(target);
      obj[key] = buffer;
    },
    moderngl_glBindBufferBase: (target, index, buffer) => setRange(target, index, buffer, 0, undefined),
    moderngl_glBindBufferRange: (target, index, buffer, offset, size) => setRange(target, index, buffer, offset, size),
    moderngl_glBindFramebuffer: (target, framebuffer) => {
      if (target !== 0x8CA8) {
        state.drawFramebuffer = framebuffer;
      }
      if (target !== 0x8CA9) {
        state.readFramebuffer = framebuffer;
      }
    },
    moderngl_glBindImageTexture: (unit, texture, level, layered, layer, access, format) => {
      state.storageImages[unit] = texture ? { tex: get(texture), level, layered, layer } : null;
    },
    moderngl_glBindRenderbuffer: (target, renderbuffer) => {
      state.renderbuffer = renderbuffer;
    },
    moderngl_glBindSampler: (unit, sampler) => {
      state.samplers[unit] = sampler;
    },
    moderngl_glBindTexture: (target, texture) => {
      if (texture) {
        const tex = get(texture);
        tex.target ??= target;
        state.textures[state.activeUnit] = texture;
        return;
      }
      const current = get(state.textures[state.activeUnit]);
      if (current && current.target === target) {
        state.textures[state.activeUnit] = 0;
      }
    },
    moderngl_glBindVertexArray: (array) => {
      state.vertexArray = array;
    },
    moderngl_glBlendEquationSeparate: (modeRgb, modeAlpha) => {
      state.pipeline.blendEquation = [modeRgb, modeAlpha];
    },
    moderngl_glBlendFuncSeparate: (srcRgb, dstRgb, srcAlpha, dstAlpha) => {
      state.pipeline.blendFunc = [srcRgb, dstRgb, srcAlpha, dstAlpha];
    },
    moderngl_glBlitFramebuffer: (sx0, sy0, sx1, sy1, dx0, dy0, dx1, dy1, mask, filter) => {
      const src = getFramebuffer(state.readFramebuffer);
      const dst = getFramebuffer(state.drawFramebuffer);
      const color = (mask & 0x4000) !== 0;
      const srcAtt = color ? src.colors[src.read] : src.depth;
      const targets = color ? dst.colors.slice(0, dst.colorCount) : [dst.depth];
      const enc = endPass();
      for (const dstAtt of targets) {
        if (srcAtt.tex.samples > 1) {
          enc.beginRenderPass({ colorAttachments: [{ view: srcAtt.view, resolveTarget: attachmentView(dstAtt), loadOp: 'load', storeOp: 'store' }] }).end();
          continue;
        }
        const sameSize = sx1 - sx0 === dx1 - dx0 && sy1 - sy0 === dy1 - dy0 && sx1 > sx0 && sy1 > sy0;
        if (sameSize && srcAtt.tex.format === dstAtt.tex.format && !dstAtt.tex.screen) {
          enc.copyTextureToTexture(
            { texture: textureOf(srcAtt.tex), mipLevel: srcAtt.level, origin: { x: sx0, y: sy0, z: srcAtt.layer } },
            { texture: textureOf(dstAtt.tex), mipLevel: dstAtt.level, origin: { x: dx0, y: dy0, z: dstAtt.layer } },
            [sx1 - sx0, sy1 - sy0, 1],
          );
          continue;
        }
        blit(srcAtt.tex, srcAtt.level, srcAtt.layer, sx0, sy0, sx1, sy1, dstAtt, dx0, dy0, dx1, dy1, filter === 0x2601);
      }
    },
    moderngl_glBufferData: (target, size, data, usage) => {
      const buf = boundBuffer(target);
      if (buf.buffer) {
        if (commandEncoder) {
          submit();
        }
        buf.buffer.destroy();
      }
      buf.read = usage === 0x88E1;
      buf.size = size;
      buf.buffer = device.createBuffer({ size: pad4(Math.max(size, 4)), usage: buf.read ? GPUBufferUsage.MAP_READ | GPUBufferUsage.COPY_DST : bufferUsage });
      if (data) {
        uploadBuffer(buf, 0, data, size);
      }
    },
    moderngl_glBufferSubData: (target, offset, size, data) => uploadBuffer(boundBuffer(target), offset, data, size),
    moderngl_glCheckFramebufferStatus: (target) => 0x8CD5,
    moderngl_glClearBufferfi: (buffer, drawbuffer, depth, stencil) => {
      clearPass(getFramebuffer(state.drawFramebuffer).depth, null, depth, stencil);
    },
    moderngl_glClearBufferfv: (buffer, drawbuffer, value) => {
      const framebuffer = getFramebuffer(state.drawFramebuffer);
      if (buffer === 0x1801) {
        clearPass(framebuffer.depth, null, wasm.HEAPF32[value >> 2], null);
        return;
      }
      clearPass(framebuffer.colors[drawbuffer], Array.from(wasm.HEAPF32.subarray(value >> 2, (value >> 2) + 4)));
    },
    moderngl_glClearBufferiv: (buffer, drawbuffer, value) => {
      clearPass(getFramebuffer(state.drawFramebuffer).colors[drawbuffer], Array.from(wasm.HEAP32.subarray(value >> 2, (value >> 2) + 4)));
    },
    moderngl_glClearBufferuiv: (buffer, drawbuffer, value) => {
      clearPass(getFramebuffer(state.drawFramebuffer).colors[drawbuffer], Array.from(wasm.HEAPU32.subarray(value >> 2, (value >> 2) + 4)));
    },
    moderngl_glClientWaitSync: (sync, flags, timeout) => (get(sync).done ? 0x911A : 0x911B),
    moderngl_glCompileShader: noop,
    moderngl_glCopyBufferSubData: (readTarget, writeTarget, readOffset, writeOffset, size) => {
      endPass();
      copyBuffer(boundBuffer(readTarget).buffer, readOffset, boundBuffer(writeTarget), writeOffset, size);
    },
    moderngl_glCreateProgram: () => add({ shaders: [] }),
    moderngl_glCreateShader: (type) => add({ type, source: '' }),
    moderngl_glCullFace: (mode) => {
      state.pipeline.cullFace = mode;
    },
    moderngl_glDeleteBuffers: (n, buffers) => deleteObjects(n, buffers, (x) => x.buffer && x.buffer.destroy()),
    moderngl_glDeleteFramebuffers: (n, framebuffers) => deleteObjects(n, framebuffers, noop),
    moderngl_glDeleteProgram: (program) => {
      for (const key of pipelines.keys()) {
        if (key.startsWith(`[${program},`)) {
          pipelines.delete(key);
        }
      }
      objects.delete(program);
    },
    moderngl_glDeleteRenderbuffers: (n, renderbuffers) => deleteObjects(n, renderbuffers, (x) => x.texture && x.texture.destroy()),
    moderngl_glDeleteSamplers: (n, samplers) => deleteObjects(n, samplers, noop),
    moderngl_glDeleteShader: (shader) => objects.delete(shader),
    moderngl_glDeleteSync: (sync) => objects.delete(sync),
    moderngl_glDeleteTextures: (n, textures) => deleteObjects(n, textures, (x) => x.texture && x.texture.destroy()),
    moderngl_glDeleteVertexArrays: (n, arrays) => deleteObjects(n, arrays, noop),
    moderngl_glDepthFunc: (func) => {
      state.pipeline.depthFunc = func;
    },
    moderngl_glDepthMask: (flag) => {
      state.pipeline.depthWrite = flag;
    },
    moderngl_glDetachShader: noop,
    moderngl_glDisable: (cap) => setCapability(cap, 0),
    moderngl_glDispatchCompute: (x, y, z) => {
      const program = get(state.program);
      const bound = bindings(true);
      program.computePipelines ??= new Map();
      if (!program.computePipelines.has(bound.key)) {
        program.computePipelines.set(bound.key, device.createComputePipeline({ layout: bound.layout.pipelineLayout, compute: { module: program.compute } }));
      }
      const computePass = endPass().beginComputePass();
      computePass.setPipeline(program.computePipelines.get(bound.key));
      computePass.setBindGroup(0, bound.bindGroup);
      computePass.dispatchWorkgroups(x, y, z);
      computePass.end();
    },
    moderngl_glDrawArraysInstanced: (mode, first, count, instances) => {
      prepareDraw(mode);
      pass.draw(count, instances, first, 0);
    },
    moderngl_glDrawArraysInstancedBaseInstance: (mode, first, count, instances, baseInstance) => {
      prepareDraw(mode);
      pass.draw(count, instances, first, baseInstance);
    },
    moderngl_glDrawBuffers: (n, bufs) => {
      const framebuffer = getFramebuffer(state.drawFramebuffer);
      framebuffer.colorCount = Array.from(wasm.HEAPU32.subarray(bufs >> 2, (bufs >> 2) + n)).filter((x) => x !== 0).length;
    },
    moderngl_glDrawElementsInstanced: (mode, count, type, offset, instances) => {
      prepareDraw(mode, indexFormat(type));
      bindIndexBuffer(type);
      pass.drawIndexed(count, instances, offset / indexSize(type), 0, 0);
    },
    moderngl_glDrawElementsInstancedBaseVertexBaseInstance: (mode, count, type, offset, instances, baseVertex, baseInstance) => {
      prepareDraw(mode, indexFormat(type));
      bindIndexBuffer(type);
      pass.drawIndexed(count, instances, offset / indexSize(type), baseVertex, baseInstance);
    },
    moderngl_glEnable: (cap) => setCapability(cap, 1),
    moderngl_glEnableVertexAttribArray: (index) => {
      getVertexArray().attribs[index].enabled = true;
    },
    moderngl_glFenceSync: (condition, flags) => {
      submit();
      const sync = { done: false };
      const reads = [...pendingReads];
      pendingReads.clear();
      const maps = reads.map((buf) => buf.buffer.mapAsync(GPUMapMode.READ).then(() => {
        buf.shadow = new Uint8Array(buf.buffer.getMappedRange()).slice();
        buf.buffer.unmap();
      }));
      Promise.all([device.queue.onSubmittedWorkDone(), ...maps]).then(() => {
        sync.done = true;
      });
      return add(sync);
    },
    moderngl_glFlush: () => {
      if (commandEncoder) {
        submit();
      }
    },
    moderngl_glFramebufferRenderbuffer: (target, attachment, renderbufferTarget, renderbuffer) => attach(target, attachment, renderbuffer, 0, 0),
    moderngl_glFramebufferTexture2D: (target, attachment, textureTarget, texture, level) => {
      const face = textureTarget >= 0x8515 && textureTarget <= 0x851A;
      attach(target, attachment, texture, level, face ? textureTarget - 0x8515 : 0);
    },
    moderngl_glFramebufferTextureLayer: (target, attachment, texture, level, layer) => attach(target, attachment, texture, level, layer),
    moderngl_glGenBuffers: (n, buffers) => genObjects(n, buffers, () => ({ buffer: null, size: 0, read: false })),
    moderngl_glGenFramebuffers: (n, framebuffers) => genObjects(n, framebuffers, () => ({ colors: [], depth: null, colorCount: 0, read: 0 })),
    moderngl_glGenRenderbuffers: (n, renderbuffers) => genObjects(n, renderbuffers, () => ({})),
    moderngl_glGenSamplers: (n, samplers) => genObjects(n, samplers, () => ({
      sampler: null,
      minFilter: 0x2702,
      magFilter: 0x2601,
      wrap: [0x2901, 0x2901, 0x2901],
      compareMode: 0,
      compareFunc: 0x0203,
      minLod: -1000,
      maxLod: 1000,
      anisotropy: 1,
    })),
    moderngl_glGenTextures: (n, textures) => genObjects(n, textures, () => ({})),
    moderngl_glGenVertexArrays: (n, arrays) => genObjects(n, arrays, () => ({ attribs: [], indexBuffer: 0 })),
    moderngl_glGenerateMipmap: (target) => {
      const tex = boundTexture();
      for (let layer = 0; layer < tex.layers; ++layer) {
        for (let level = 1; level < tex.levels; ++level) {
          const w = Math.max(tex.width >> level, 1);
          const h = Math.max(tex.height >> level, 1);
          const src = [Math.max(tex.width >> (level - 1), 1), Math.max(tex.height >> (level - 1), 1)];
          blit(tex, level - 1, layer, 0, 0, src[0], src[1], newAttachment(tex, level, layer), 0, 0, w, h, true);
        }
      }
    },
    moderngl_glGetActiveUniform: noop,
    moderngl_glGetActiveUniformBlockName: noop,
    moderngl_glGetActiveUniformBlockiv: noop,
    moderngl_glGetBufferParameteriv: (target, pname, params) => {
      wasm.HEAP32[params >> 2] = boundBuffer(target).size;
    },
    moderngl_glGetBufferSubData: (target, offset, size, data) => {
      wasm.HEAPU8.set(boundBuffer(target).shadow.subarray(offset, offset + size), data);
    },
    moderngl_glGetFloatv: noop,
    moderngl_glGetIntegeri_v: noop,
    moderngl_glIsEnabled: () => 0,
    moderngl_glGetIntegerv: (pname, data) => {
      const values = {
        0x0BA2: state.viewport,
        0x8869: [16],
        0x8B4D: [17],
        0x8A2F: [8],
        0x90DD: [8],
        0x8A30: [65536],
        0x8A34: [256],
        0x90DF: [256],
        0x8F38: [4],
      };
      wasm.HEAP32.set(values[pname] ?? [0], data >> 2);
    },
    moderngl_glGetProgramInfoLog: noop,
    moderngl_glGetProgramiv: (program, pname, params) => {
      wasm.HEAP32[params >> 2] = pname === 0x8B82 ? 1 : 0;
    },
    moderngl_glGetProgramInterfaceiv: (program, programInterface, pname, params) => {
      wasm.HEAP32[params >> 2] = 0;
    },
    moderngl_glGetProgramResourceIndex: (program, programInterface, name) => 0xFFFFFFFF,
    moderngl_glGetProgramResourceName: noop,
    moderngl_glGetShaderInfoLog: noop,
    moderngl_glGetShaderiv: (shader, pname, params) => {
      wasm.HEAP32[params >> 2] = 1;
    },
    moderngl_glGetString: (name) => {
      const values = { 0x1F00: 'WebGPU', 0x1F01: 'WebGPU', 0x1F02: 'WebGPU', 0x8B8C: 'WGSL' };
      return setString(name, values[name] ?? '');
    },
    moderngl_glGetUniformBlockIndex: (program, name) => 0xFFFFFFFF,
    moderngl_glGetUniformLocation: (program, name) => -1,
    moderngl_glGetUniformfv: noop,
    moderngl_glGetUniformiv: noop,
    moderngl_glGetUniformuiv: noop,
    moderngl_glLinkProgram: (program) => {
      const res = get(program);
      for (const shader of res.shaders) {
        const module = device.createShaderModule({ code: shader.source });
        if (shader.type === 0x8B31) {
          res.vertex = module;
        }
        if (shader.type === 0x8B30) {
          res.fragment = module;
        }
        if (shader.type === 0x91B9) {
          res.compute = module;
        }
      }
    },
    moderngl_glMemoryBarrier: (barriers) => endPass(),
    moderngl_glMultiDrawArraysIndirect: (mode, offset, count, stride) => drawIndirect(mode, 0, offset, count, stride),
    moderngl_glMultiDrawElementsIndirect: (mode, type, offset, count, stride) => drawIndirect(mode, type, offset, count, stride),
    moderngl_glPolygonOffset: (factor, units) => {
      Object.assign(state.pipeline, { polygonOffsetFactor: factor, polygonOffsetUnits: units });
    },
    moderngl_glReadBuffer: (src) => {
      getFramebuffer(state.readFramebuffer).read = src ? src - 0x8CE0 : 0;
    },
    moderngl_glReadPixels: (x, y, width, height, format, type, pixels) => {
      const framebuffer = getFramebuffer(state.readFramebuffer);
      const att = framebuffer.colors[framebuffer.read];
      const row = pad4(width * att.tex.pixelSize);
      const padded = pad256(width * att.tex.pixelSize);
      const enc = endPass();
      const temp = device.createBuffer({ size: padded * height, usage: GPUBufferUsage.COPY_SRC | GPUBufferUsage.COPY_DST });
      enc.copyTextureToBuffer({ texture: textureOf(att.tex), mipLevel: att.level, origin: { x, y, z: att.layer } }, { buffer: temp, bytesPerRow: padded, rowsPerImage: height }, [width, height, 1]);
      const dst = get(state.packBuffer);
      for (let i = 0; i < height; ++i) {
        copyBuffer(temp, i * padded, dst, pixels + i * row, row);
      }
    },
    moderngl_glRenderbufferStorageMultisample: (target, samples, internalFormat, width, height) => {
      const tex = get(state.renderbuffer);
      tex.target = 0x8D41;
      createTexture(tex, internalFormat, 0, width, height, 1, 1, samples > 1 ? samples : 1, false);
    },
    moderngl_glSamplerParameterf: samplerParameter,
    moderngl_glSamplerParameteri: samplerParameter,
    moderngl_glScissor: (x, y, width, height) => {
      state.scissor = [x, y, width, height];
    },
    moderngl_glShaderStorageBlockBinding: noop,
    moderngl_glShaderSource: (shader, count, string, length) => {
      get(shader).source = getString(wasm.HEAPU32[string >> 2]);
    },
    moderngl_glStencilFunc: (func, ref, mask) => setStencilFunc(0x0408, func, ref, mask),
    moderngl_glStencilFuncSeparate: setStencilFunc,
    moderngl_glStencilMask: (mask) => {
      state.pipeline.stencilWriteMask = mask & 0xFF;
    },
    moderngl_glStencilOp: (fail, depthFail, depthPass) => setStencilOp(0x0408, fail, depthFail, depthPass),
    moderngl_glStencilOpSeparate: setStencilOp,
    moderngl_glTexImage2D: (target, level, internalFormat, width, height, border, format, type, pixels) => texImage(target, internalFormat, width, height, 1, format),
    moderngl_glTexImage3D: (target, level, internalFormat, width, height, depth, border, format, type, pixels) => texImage(target, internalFormat, width, height, depth, format),
    moderngl_glTexParameteri: (target, pname, param) => {
      if (pname === 0x813D) {
        boundTexture().maxLevel = param;
      }
    },
    moderngl_glTexSubImage2D: (target, level, x, y, width, height, format, type, pixels) => {
      const face = target >= 0x8515 && target <= 0x851A;
      texSubImage(boundTexture(), level, x, y, face ? target - 0x8515 : 0, width, height, 1, pixels);
    },
    moderngl_glTexSubImage3D: (target, level, x, y, z, width, height, depth, format, type, pixels) => {
      texSubImage(boundTexture(), level, x, y, z, width, height, depth, pixels);
    },
    moderngl_glUniform1fv: noop,
    moderngl_glUniform1i: noop,
    moderngl_glUniform1iv: noop,
    moderngl_glUniform1uiv: noop,
    moderngl_glUniform2fv: noop,
    moderngl_glUniform2iv: noop,
    moderngl_glUniform2uiv: noop,
    moderngl_glUniform3fv: noop,
    moderngl_glUniform3iv: noop,
    moderngl_glUniform3uiv: noop,
    moderngl_glUniform4fv: noop,
    moderngl_glUniform4iv: noop,
    moderngl_glUniform4uiv: noop,
    moderngl_glUniformBlockBinding: noop,
    moderngl_glUniformMatrix2fv: noop,
    moderngl_glUniformMatrix2x3fv: noop,
    moderngl_glUniformMatrix2x4fv: noop,
    moderngl_glUniformMatrix3fv: noop,
    moderngl_glUniformMatrix3x2fv: noop,
    moderngl_glUniformMatrix3x4fv: noop,
    moderngl_glUniformMatrix4fv: noop,
    moderngl_glUniformMatrix4x2fv: noop,
    moderngl_glUniformMatrix4x3fv: noop,
    moderngl_glUseProgram: (program) => {
      state.program = program;
    },
    moderngl_glVertexAttribDivisor: (index, divisor) => {
      getVertexArray().attribs[index].rate = divisor ? 'instance' : 'vertex';
    },
    moderngl_glVertexAttribIPointer: (index, size, type, stride, offset) => vertexAttrib(index, size, type, true, false, stride, offset),
    moderngl_glVertexAttribPointer: (index, size, type, normalized, stride, offset) => vertexAttrib(index, size, type, false, normalized, stride, offset),
    moderngl_glViewport: (x, y, width, height) => {
      state.viewport = [x, y, width, height];
    },
  };
}
