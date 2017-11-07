#include "GLMethods.hpp"

#include "Types.hpp"

void AddToLog(PyObject ** pstr, const char * function) {
	PyObject * name = PyUnicode_FromFormat("- %s\n", function);
	PyUnicode_Append(pstr, name);
	Py_DECREF(name);
}

#define INVALID_METHOD(method) (!(method))

void GLAPI FakeGetProgramStageiv(GLuint program, GLenum shadertype, GLenum pname, GLint * values) {
	values[0] = 0;
}

#if defined(_WIN32) || defined(_WIN64)

#define PREFIX ""

#include <Windows.h>

#ifdef MemoryBarrier
#undef MemoryBarrier
#endif

void * LoadMethod(const char * method) {
	static HMODULE opengl32 = LoadLibrary("opengl32.dll");

	void * proc = (void *)GetProcAddress(opengl32, method);

	if (proc) {
		return proc;
	}

	proc = (void *)wglGetProcAddress(method);

	if (proc) {
		return proc;
	}

	// printf("%s NOT found!\n", method);
	return 0;
}

#elif defined(__APPLE__)

#define PREFIX "_"

#import <mach-o/dyld.h>
#import <stdlib.h>
#import <string.h>

void * LoadMethod(const char * method) {
	NSSymbol symbol = 0;

	if (NSIsSymbolNameDefined(method)) {
		symbol = NSLookupAndBindSymbol(method);
	}

	return symbol ? NSAddressOfSymbol(symbol) : 0;
}

#else

#define PREFIX ""

#include <dlfcn.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef const void * (* PROC_glXGetProcAddress)(const char *);

void * LoadMethod(const char * method) {
	static void * libgl = dlopen("libGL.so.1", RTLD_LAZY);
	static PROC_glXGetProcAddress glXGetProcAddress = (PROC_glXGetProcAddress)dlsym(libgl, "glXGetProcAddress");

	void * proc = (void *)dlsym(libgl, method);

	if (proc) {
		// printf("%s found!\n", method);
		return proc;
	}

	proc = (void *)glXGetProcAddress(method);

	if (proc) {
		// printf("%s found!\n", method);
		return proc;
	}

	// printf("%s NOT found!\n", method);
	return 0;
}

#endif

bool GLMethods::load() {
	this->ActiveShaderProgram = (PROC_glActiveShaderProgram)LoadMethod(PREFIX "glActiveShaderProgram");
	this->ActiveTexture = (PROC_glActiveTexture)LoadMethod(PREFIX "glActiveTexture");
	this->AttachShader = (PROC_glAttachShader)LoadMethod(PREFIX "glAttachShader");
	this->BeginConditionalRender = (PROC_glBeginConditionalRender)LoadMethod(PREFIX "glBeginConditionalRender");
	this->BeginQuery = (PROC_glBeginQuery)LoadMethod(PREFIX "glBeginQuery");
	this->BeginQueryIndexed = (PROC_glBeginQueryIndexed)LoadMethod(PREFIX "glBeginQueryIndexed");
	this->BeginTransformFeedback = (PROC_glBeginTransformFeedback)LoadMethod(PREFIX "glBeginTransformFeedback");
	this->BindAttribLocation = (PROC_glBindAttribLocation)LoadMethod(PREFIX "glBindAttribLocation");
	this->BindBuffer = (PROC_glBindBuffer)LoadMethod(PREFIX "glBindBuffer");
	this->BindBufferBase = (PROC_glBindBufferBase)LoadMethod(PREFIX "glBindBufferBase");
	this->BindBufferRange = (PROC_glBindBufferRange)LoadMethod(PREFIX "glBindBufferRange");
	this->BindBuffersBase = (PROC_glBindBuffersBase)LoadMethod(PREFIX "glBindBuffersBase");
	this->BindBuffersRange = (PROC_glBindBuffersRange)LoadMethod(PREFIX "glBindBuffersRange");
	this->BindFragDataLocation = (PROC_glBindFragDataLocation)LoadMethod(PREFIX "glBindFragDataLocation");
	this->BindFragDataLocationIndexed = (PROC_glBindFragDataLocationIndexed)LoadMethod(PREFIX "glBindFragDataLocationIndexed");
	this->BindFramebuffer = (PROC_glBindFramebuffer)LoadMethod(PREFIX "glBindFramebuffer");
	this->BindImageTexture = (PROC_glBindImageTexture)LoadMethod(PREFIX "glBindImageTexture");
	this->BindImageTextures = (PROC_glBindImageTextures)LoadMethod(PREFIX "glBindImageTextures");
	this->BindProgramPipeline = (PROC_glBindProgramPipeline)LoadMethod(PREFIX "glBindProgramPipeline");
	this->BindRenderbuffer = (PROC_glBindRenderbuffer)LoadMethod(PREFIX "glBindRenderbuffer");
	this->BindSampler = (PROC_glBindSampler)LoadMethod(PREFIX "glBindSampler");
	this->BindSamplers = (PROC_glBindSamplers)LoadMethod(PREFIX "glBindSamplers");
	this->BindTexture = (PROC_glBindTexture)LoadMethod(PREFIX "glBindTexture");
	this->BindTextureUnit = (PROC_glBindTextureUnit)LoadMethod(PREFIX "glBindTextureUnit");
	this->BindTextures = (PROC_glBindTextures)LoadMethod(PREFIX "glBindTextures");
	this->BindTransformFeedback = (PROC_glBindTransformFeedback)LoadMethod(PREFIX "glBindTransformFeedback");
	this->BindVertexArray = (PROC_glBindVertexArray)LoadMethod(PREFIX "glBindVertexArray");
	this->BindVertexBuffer = (PROC_glBindVertexBuffer)LoadMethod(PREFIX "glBindVertexBuffer");
	this->BindVertexBuffers = (PROC_glBindVertexBuffers)LoadMethod(PREFIX "glBindVertexBuffers");
	this->BlendColor = (PROC_glBlendColor)LoadMethod(PREFIX "glBlendColor");
	this->BlendEquation = (PROC_glBlendEquation)LoadMethod(PREFIX "glBlendEquation");
	this->BlendEquationSeparate = (PROC_glBlendEquationSeparate)LoadMethod(PREFIX "glBlendEquationSeparate");
	this->BlendEquationSeparatei = (PROC_glBlendEquationSeparatei)LoadMethod(PREFIX "glBlendEquationSeparatei");
	this->BlendEquationi = (PROC_glBlendEquationi)LoadMethod(PREFIX "glBlendEquationi");
	this->BlendFunc = (PROC_glBlendFunc)LoadMethod(PREFIX "glBlendFunc");
	this->BlendFuncSeparate = (PROC_glBlendFuncSeparate)LoadMethod(PREFIX "glBlendFuncSeparate");
	this->BlendFuncSeparatei = (PROC_glBlendFuncSeparatei)LoadMethod(PREFIX "glBlendFuncSeparatei");
	this->BlendFunci = (PROC_glBlendFunci)LoadMethod(PREFIX "glBlendFunci");
	this->BlitFramebuffer = (PROC_glBlitFramebuffer)LoadMethod(PREFIX "glBlitFramebuffer");
	this->BlitNamedFramebuffer = (PROC_glBlitNamedFramebuffer)LoadMethod(PREFIX "glBlitNamedFramebuffer");
	this->BufferData = (PROC_glBufferData)LoadMethod(PREFIX "glBufferData");
	this->BufferStorage = (PROC_glBufferStorage)LoadMethod(PREFIX "glBufferStorage");
	this->BufferSubData = (PROC_glBufferSubData)LoadMethod(PREFIX "glBufferSubData");
	this->CheckFramebufferStatus = (PROC_glCheckFramebufferStatus)LoadMethod(PREFIX "glCheckFramebufferStatus");
	this->CheckNamedFramebufferStatus = (PROC_glCheckNamedFramebufferStatus)LoadMethod(PREFIX "glCheckNamedFramebufferStatus");
	this->ClampColor = (PROC_glClampColor)LoadMethod(PREFIX "glClampColor");
	this->Clear = (PROC_glClear)LoadMethod(PREFIX "glClear");
	this->ClearBufferData = (PROC_glClearBufferData)LoadMethod(PREFIX "glClearBufferData");
	this->ClearBufferSubData = (PROC_glClearBufferSubData)LoadMethod(PREFIX "glClearBufferSubData");
	this->ClearBufferfi = (PROC_glClearBufferfi)LoadMethod(PREFIX "glClearBufferfi");
	this->ClearBufferfv = (PROC_glClearBufferfv)LoadMethod(PREFIX "glClearBufferfv");
	this->ClearBufferiv = (PROC_glClearBufferiv)LoadMethod(PREFIX "glClearBufferiv");
	this->ClearBufferuiv = (PROC_glClearBufferuiv)LoadMethod(PREFIX "glClearBufferuiv");
	this->ClearColor = (PROC_glClearColor)LoadMethod(PREFIX "glClearColor");
	this->ClearDepth = (PROC_glClearDepth)LoadMethod(PREFIX "glClearDepth");
	this->ClearDepthf = (PROC_glClearDepthf)LoadMethod(PREFIX "glClearDepthf");
	this->ClearNamedBufferData = (PROC_glClearNamedBufferData)LoadMethod(PREFIX "glClearNamedBufferData");
	this->ClearNamedBufferSubData = (PROC_glClearNamedBufferSubData)LoadMethod(PREFIX "glClearNamedBufferSubData");
	this->ClearNamedFramebufferfi = (PROC_glClearNamedFramebufferfi)LoadMethod(PREFIX "glClearNamedFramebufferfi");
	this->ClearNamedFramebufferfv = (PROC_glClearNamedFramebufferfv)LoadMethod(PREFIX "glClearNamedFramebufferfv");
	this->ClearNamedFramebufferiv = (PROC_glClearNamedFramebufferiv)LoadMethod(PREFIX "glClearNamedFramebufferiv");
	this->ClearNamedFramebufferuiv = (PROC_glClearNamedFramebufferuiv)LoadMethod(PREFIX "glClearNamedFramebufferuiv");
	this->ClearStencil = (PROC_glClearStencil)LoadMethod(PREFIX "glClearStencil");
	this->ClearTexImage = (PROC_glClearTexImage)LoadMethod(PREFIX "glClearTexImage");
	this->ClearTexSubImage = (PROC_glClearTexSubImage)LoadMethod(PREFIX "glClearTexSubImage");
	this->ClientWaitSync = (PROC_glClientWaitSync)LoadMethod(PREFIX "glClientWaitSync");
	this->WaitSync = (PROC_glWaitSync)LoadMethod(PREFIX "glWaitSync");
	this->ClipControl = (PROC_glClipControl)LoadMethod(PREFIX "glClipControl");
	this->ColorMask = (PROC_glColorMask)LoadMethod(PREFIX "glColorMask");
	this->ColorMaski = (PROC_glColorMaski)LoadMethod(PREFIX "glColorMaski");
	this->CompileShader = (PROC_glCompileShader)LoadMethod(PREFIX "glCompileShader");
	this->CompressedTexImage1D = (PROC_glCompressedTexImage1D)LoadMethod(PREFIX "glCompressedTexImage1D");
	this->CompressedTexImage2D = (PROC_glCompressedTexImage2D)LoadMethod(PREFIX "glCompressedTexImage2D");
	this->CompressedTexImage3D = (PROC_glCompressedTexImage3D)LoadMethod(PREFIX "glCompressedTexImage3D");
	this->CompressedTexSubImage1D = (PROC_glCompressedTexSubImage1D)LoadMethod(PREFIX "glCompressedTexSubImage1D");
	this->CompressedTexSubImage2D = (PROC_glCompressedTexSubImage2D)LoadMethod(PREFIX "glCompressedTexSubImage2D");
	this->CompressedTexSubImage3D = (PROC_glCompressedTexSubImage3D)LoadMethod(PREFIX "glCompressedTexSubImage3D");
	this->CompressedTextureSubImage1D = (PROC_glCompressedTextureSubImage1D)LoadMethod(PREFIX "glCompressedTextureSubImage1D");
	this->CompressedTextureSubImage2D = (PROC_glCompressedTextureSubImage2D)LoadMethod(PREFIX "glCompressedTextureSubImage2D");
	this->CompressedTextureSubImage3D = (PROC_glCompressedTextureSubImage3D)LoadMethod(PREFIX "glCompressedTextureSubImage3D");
	this->CopyBufferSubData = (PROC_glCopyBufferSubData)LoadMethod(PREFIX "glCopyBufferSubData");
	this->CopyImageSubData = (PROC_glCopyImageSubData)LoadMethod(PREFIX "glCopyImageSubData");
	this->CopyNamedBufferSubData = (PROC_glCopyNamedBufferSubData)LoadMethod(PREFIX "glCopyNamedBufferSubData");
	this->CopyTexImage1D = (PROC_glCopyTexImage1D)LoadMethod(PREFIX "glCopyTexImage1D");
	this->CopyTexImage2D = (PROC_glCopyTexImage2D)LoadMethod(PREFIX "glCopyTexImage2D");
	this->CopyTexSubImage1D = (PROC_glCopyTexSubImage1D)LoadMethod(PREFIX "glCopyTexSubImage1D");
	this->CopyTexSubImage2D = (PROC_glCopyTexSubImage2D)LoadMethod(PREFIX "glCopyTexSubImage2D");
	this->CopyTexSubImage3D = (PROC_glCopyTexSubImage3D)LoadMethod(PREFIX "glCopyTexSubImage3D");
	this->CopyTextureSubImage1D = (PROC_glCopyTextureSubImage1D)LoadMethod(PREFIX "glCopyTextureSubImage1D");
	this->CopyTextureSubImage2D = (PROC_glCopyTextureSubImage2D)LoadMethod(PREFIX "glCopyTextureSubImage2D");
	this->CopyTextureSubImage3D = (PROC_glCopyTextureSubImage3D)LoadMethod(PREFIX "glCopyTextureSubImage3D");
	this->CreateBuffers = (PROC_glCreateBuffers)LoadMethod(PREFIX "glCreateBuffers");
	this->CreateFramebuffers = (PROC_glCreateFramebuffers)LoadMethod(PREFIX "glCreateFramebuffers");
	this->CreateProgram = (PROC_glCreateProgram)LoadMethod(PREFIX "glCreateProgram");
	this->CreateProgramPipelines = (PROC_glCreateProgramPipelines)LoadMethod(PREFIX "glCreateProgramPipelines");
	this->CreateQueries = (PROC_glCreateQueries)LoadMethod(PREFIX "glCreateQueries");
	this->CreateRenderbuffers = (PROC_glCreateRenderbuffers)LoadMethod(PREFIX "glCreateRenderbuffers");
	this->CreateSamplers = (PROC_glCreateSamplers)LoadMethod(PREFIX "glCreateSamplers");
	this->CreateShader = (PROC_glCreateShader)LoadMethod(PREFIX "glCreateShader");
	this->CreateShaderProgramv = (PROC_glCreateShaderProgramv)LoadMethod(PREFIX "glCreateShaderProgramv");
	this->CreateTextures = (PROC_glCreateTextures)LoadMethod(PREFIX "glCreateTextures");
	this->CreateTransformFeedbacks = (PROC_glCreateTransformFeedbacks)LoadMethod(PREFIX "glCreateTransformFeedbacks");
	this->CreateVertexArrays = (PROC_glCreateVertexArrays)LoadMethod(PREFIX "glCreateVertexArrays");
	this->CullFace = (PROC_glCullFace)LoadMethod(PREFIX "glCullFace");
	this->DebugMessageCallback = (PROC_glDebugMessageCallback)LoadMethod(PREFIX "glDebugMessageCallback");
	this->DebugMessageControl = (PROC_glDebugMessageControl)LoadMethod(PREFIX "glDebugMessageControl");
	this->DebugMessageInsert = (PROC_glDebugMessageInsert)LoadMethod(PREFIX "glDebugMessageInsert");
	this->DeleteBuffers = (PROC_glDeleteBuffers)LoadMethod(PREFIX "glDeleteBuffers");
	this->DeleteFramebuffers = (PROC_glDeleteFramebuffers)LoadMethod(PREFIX "glDeleteFramebuffers");
	this->DeleteProgram = (PROC_glDeleteProgram)LoadMethod(PREFIX "glDeleteProgram");
	this->DeleteProgramPipelines = (PROC_glDeleteProgramPipelines)LoadMethod(PREFIX "glDeleteProgramPipelines");
	this->DeleteQueries = (PROC_glDeleteQueries)LoadMethod(PREFIX "glDeleteQueries");
	this->DeleteRenderbuffers = (PROC_glDeleteRenderbuffers)LoadMethod(PREFIX "glDeleteRenderbuffers");
	this->DeleteSamplers = (PROC_glDeleteSamplers)LoadMethod(PREFIX "glDeleteSamplers");
	this->DeleteShader = (PROC_glDeleteShader)LoadMethod(PREFIX "glDeleteShader");
	this->DeleteSync = (PROC_glDeleteSync)LoadMethod(PREFIX "glDeleteSync");
	this->DeleteTextures = (PROC_glDeleteTextures)LoadMethod(PREFIX "glDeleteTextures");
	this->DeleteTransformFeedbacks = (PROC_glDeleteTransformFeedbacks)LoadMethod(PREFIX "glDeleteTransformFeedbacks");
	this->DeleteVertexArrays = (PROC_glDeleteVertexArrays)LoadMethod(PREFIX "glDeleteVertexArrays");
	this->DepthFunc = (PROC_glDepthFunc)LoadMethod(PREFIX "glDepthFunc");
	this->DepthMask = (PROC_glDepthMask)LoadMethod(PREFIX "glDepthMask");
	this->DepthRange = (PROC_glDepthRange)LoadMethod(PREFIX "glDepthRange");
	this->DepthRangeArrayv = (PROC_glDepthRangeArrayv)LoadMethod(PREFIX "glDepthRangeArrayv");
	this->DepthRangeIndexed = (PROC_glDepthRangeIndexed)LoadMethod(PREFIX "glDepthRangeIndexed");
	this->DepthRangef = (PROC_glDepthRangef)LoadMethod(PREFIX "glDepthRangef");
	this->DetachShader = (PROC_glDetachShader)LoadMethod(PREFIX "glDetachShader");
	this->Disable = (PROC_glDisable)LoadMethod(PREFIX "glDisable");
	this->DisableVertexArrayAttrib = (PROC_glDisableVertexArrayAttrib)LoadMethod(PREFIX "glDisableVertexArrayAttrib");
	this->DisableVertexAttribArray = (PROC_glDisableVertexAttribArray)LoadMethod(PREFIX "glDisableVertexAttribArray");
	this->Disablei = (PROC_glDisablei)LoadMethod(PREFIX "glDisablei");
	this->DispatchCompute = (PROC_glDispatchCompute)LoadMethod(PREFIX "glDispatchCompute");
	this->DispatchComputeIndirect = (PROC_glDispatchComputeIndirect)LoadMethod(PREFIX "glDispatchComputeIndirect");
	this->DrawArrays = (PROC_glDrawArrays)LoadMethod(PREFIX "glDrawArrays");
	this->DrawArraysIndirect = (PROC_glDrawArraysIndirect)LoadMethod(PREFIX "glDrawArraysIndirect");
	this->DrawArraysInstanced = (PROC_glDrawArraysInstanced)LoadMethod(PREFIX "glDrawArraysInstanced");
	this->DrawArraysInstancedBaseInstance = (PROC_glDrawArraysInstancedBaseInstance)LoadMethod(PREFIX "glDrawArraysInstancedBaseInstance");
	this->DrawBuffer = (PROC_glDrawBuffer)LoadMethod(PREFIX "glDrawBuffer");
	this->DrawBuffers = (PROC_glDrawBuffers)LoadMethod(PREFIX "glDrawBuffers");
	this->DrawElements = (PROC_glDrawElements)LoadMethod(PREFIX "glDrawElements");
	this->DrawElementsBaseVertex = (PROC_glDrawElementsBaseVertex)LoadMethod(PREFIX "glDrawElementsBaseVertex");
	this->DrawElementsIndirect = (PROC_glDrawElementsIndirect)LoadMethod(PREFIX "glDrawElementsIndirect");
	this->DrawElementsInstanced = (PROC_glDrawElementsInstanced)LoadMethod(PREFIX "glDrawElementsInstanced");
	this->DrawElementsInstancedBaseInstance = (PROC_glDrawElementsInstancedBaseInstance)LoadMethod(PREFIX "glDrawElementsInstancedBaseInstance");
	this->DrawElementsInstancedBaseVertex = (PROC_glDrawElementsInstancedBaseVertex)LoadMethod(PREFIX "glDrawElementsInstancedBaseVertex");
	this->DrawElementsInstancedBaseVertexBaseInstance = (PROC_glDrawElementsInstancedBaseVertexBaseInstance)LoadMethod(PREFIX "glDrawElementsInstancedBaseVertexBaseInstance");
	this->DrawRangeElements = (PROC_glDrawRangeElements)LoadMethod(PREFIX "glDrawRangeElements");
	this->DrawRangeElementsBaseVertex = (PROC_glDrawRangeElementsBaseVertex)LoadMethod(PREFIX "glDrawRangeElementsBaseVertex");
	this->DrawTransformFeedback = (PROC_glDrawTransformFeedback)LoadMethod(PREFIX "glDrawTransformFeedback");
	this->DrawTransformFeedbackInstanced = (PROC_glDrawTransformFeedbackInstanced)LoadMethod(PREFIX "glDrawTransformFeedbackInstanced");
	this->DrawTransformFeedbackStream = (PROC_glDrawTransformFeedbackStream)LoadMethod(PREFIX "glDrawTransformFeedbackStream");
	this->DrawTransformFeedbackStreamInstanced = (PROC_glDrawTransformFeedbackStreamInstanced)LoadMethod(PREFIX "glDrawTransformFeedbackStreamInstanced");
	this->Enable = (PROC_glEnable)LoadMethod(PREFIX "glEnable");
	this->EnableVertexArrayAttrib = (PROC_glEnableVertexArrayAttrib)LoadMethod(PREFIX "glEnableVertexArrayAttrib");
	this->EnableVertexAttribArray = (PROC_glEnableVertexAttribArray)LoadMethod(PREFIX "glEnableVertexAttribArray");
	this->Enablei = (PROC_glEnablei)LoadMethod(PREFIX "glEnablei");
	this->EndConditionalRender = (PROC_glEndConditionalRender)LoadMethod(PREFIX "glEndConditionalRender");
	this->EndQuery = (PROC_glEndQuery)LoadMethod(PREFIX "glEndQuery");
	this->EndQueryIndexed = (PROC_glEndQueryIndexed)LoadMethod(PREFIX "glEndQueryIndexed");
	this->EndTransformFeedback = (PROC_glEndTransformFeedback)LoadMethod(PREFIX "glEndTransformFeedback");
	this->FenceSync = (PROC_glFenceSync)LoadMethod(PREFIX "glFenceSync");
	this->Finish = (PROC_glFinish)LoadMethod(PREFIX "glFinish");
	this->Flush = (PROC_glFlush)LoadMethod(PREFIX "glFlush");
	this->FlushMappedBufferRange = (PROC_glFlushMappedBufferRange)LoadMethod(PREFIX "glFlushMappedBufferRange");
	this->FlushMappedNamedBufferRange = (PROC_glFlushMappedNamedBufferRange)LoadMethod(PREFIX "glFlushMappedNamedBufferRange");
	this->FramebufferParameteri = (PROC_glFramebufferParameteri)LoadMethod(PREFIX "glFramebufferParameteri");
	this->FramebufferRenderbuffer = (PROC_glFramebufferRenderbuffer)LoadMethod(PREFIX "glFramebufferRenderbuffer");
	this->FramebufferTexture = (PROC_glFramebufferTexture)LoadMethod(PREFIX "glFramebufferTexture");
	this->FramebufferTexture1D = (PROC_glFramebufferTexture1D)LoadMethod(PREFIX "glFramebufferTexture1D");
	this->FramebufferTexture2D = (PROC_glFramebufferTexture2D)LoadMethod(PREFIX "glFramebufferTexture2D");
	this->FramebufferTexture3D = (PROC_glFramebufferTexture3D)LoadMethod(PREFIX "glFramebufferTexture3D");
	this->FramebufferTextureLayer = (PROC_glFramebufferTextureLayer)LoadMethod(PREFIX "glFramebufferTextureLayer");
	this->FrontFace = (PROC_glFrontFace)LoadMethod(PREFIX "glFrontFace");
	this->GenBuffers = (PROC_glGenBuffers)LoadMethod(PREFIX "glGenBuffers");
	this->GenFramebuffers = (PROC_glGenFramebuffers)LoadMethod(PREFIX "glGenFramebuffers");
	this->GenProgramPipelines = (PROC_glGenProgramPipelines)LoadMethod(PREFIX "glGenProgramPipelines");
	this->GenQueries = (PROC_glGenQueries)LoadMethod(PREFIX "glGenQueries");
	this->GenRenderbuffers = (PROC_glGenRenderbuffers)LoadMethod(PREFIX "glGenRenderbuffers");
	this->GenSamplers = (PROC_glGenSamplers)LoadMethod(PREFIX "glGenSamplers");
	this->GenTextures = (PROC_glGenTextures)LoadMethod(PREFIX "glGenTextures");
	this->GenTransformFeedbacks = (PROC_glGenTransformFeedbacks)LoadMethod(PREFIX "glGenTransformFeedbacks");
	this->GenVertexArrays = (PROC_glGenVertexArrays)LoadMethod(PREFIX "glGenVertexArrays");
	this->GenerateMipmap = (PROC_glGenerateMipmap)LoadMethod(PREFIX "glGenerateMipmap");
	this->GenerateTextureMipmap = (PROC_glGenerateTextureMipmap)LoadMethod(PREFIX "glGenerateTextureMipmap");
	this->GetActiveAtomicCounterBufferiv = (PROC_glGetActiveAtomicCounterBufferiv)LoadMethod(PREFIX "glGetActiveAtomicCounterBufferiv");
	this->GetActiveAttrib = (PROC_glGetActiveAttrib)LoadMethod(PREFIX "glGetActiveAttrib");
	this->GetActiveSubroutineName = (PROC_glGetActiveSubroutineName)LoadMethod(PREFIX "glGetActiveSubroutineName");
	this->GetActiveSubroutineUniformName = (PROC_glGetActiveSubroutineUniformName)LoadMethod(PREFIX "glGetActiveSubroutineUniformName");
	this->GetActiveSubroutineUniformiv = (PROC_glGetActiveSubroutineUniformiv)LoadMethod(PREFIX "glGetActiveSubroutineUniformiv");
	this->GetActiveUniform = (PROC_glGetActiveUniform)LoadMethod(PREFIX "glGetActiveUniform");
	this->GetActiveUniformBlockName = (PROC_glGetActiveUniformBlockName)LoadMethod(PREFIX "glGetActiveUniformBlockName");
	this->GetActiveUniformBlockiv = (PROC_glGetActiveUniformBlockiv)LoadMethod(PREFIX "glGetActiveUniformBlockiv");
	this->GetActiveUniformName = (PROC_glGetActiveUniformName)LoadMethod(PREFIX "glGetActiveUniformName");
	this->GetActiveUniformsiv = (PROC_glGetActiveUniformsiv)LoadMethod(PREFIX "glGetActiveUniformsiv");
	this->GetAttachedShaders = (PROC_glGetAttachedShaders)LoadMethod(PREFIX "glGetAttachedShaders");
	this->GetAttribLocation = (PROC_glGetAttribLocation)LoadMethod(PREFIX "glGetAttribLocation");
	this->GetBooleani_v = (PROC_glGetBooleani_v)LoadMethod(PREFIX "glGetBooleani_v");
	this->GetBooleanv = (PROC_glGetBooleanv)LoadMethod(PREFIX "glGetBooleanv");
	this->GetBufferParameteri64v = (PROC_glGetBufferParameteri64v)LoadMethod(PREFIX "glGetBufferParameteri64v");
	this->GetBufferParameteriv = (PROC_glGetBufferParameteriv)LoadMethod(PREFIX "glGetBufferParameteriv");
	this->GetBufferPointerv = (PROC_glGetBufferPointerv)LoadMethod(PREFIX "glGetBufferPointerv");
	this->GetBufferSubData = (PROC_glGetBufferSubData)LoadMethod(PREFIX "glGetBufferSubData");
	this->GetCompressedTexImage = (PROC_glGetCompressedTexImage)LoadMethod(PREFIX "glGetCompressedTexImage");
	this->GetCompressedTextureImage = (PROC_glGetCompressedTextureImage)LoadMethod(PREFIX "glGetCompressedTextureImage");
	this->GetCompressedTextureSubImage = (PROC_glGetCompressedTextureSubImage)LoadMethod(PREFIX "glGetCompressedTextureSubImage");
	this->GetDebugMessageLog = (PROC_glGetDebugMessageLog)LoadMethod(PREFIX "glGetDebugMessageLog");
	this->GetDoublei_v = (PROC_glGetDoublei_v)LoadMethod(PREFIX "glGetDoublei_v");
	this->GetDoublev = (PROC_glGetDoublev)LoadMethod(PREFIX "glGetDoublev");
	this->GetError = (PROC_glGetError)LoadMethod(PREFIX "glGetError");
	this->GetFloati_v = (PROC_glGetFloati_v)LoadMethod(PREFIX "glGetFloati_v");
	this->GetFloatv = (PROC_glGetFloatv)LoadMethod(PREFIX "glGetFloatv");
	this->GetFragDataIndex = (PROC_glGetFragDataIndex)LoadMethod(PREFIX "glGetFragDataIndex");
	this->GetFragDataLocation = (PROC_glGetFragDataLocation)LoadMethod(PREFIX "glGetFragDataLocation");
	this->GetFramebufferAttachmentParameteriv = (PROC_glGetFramebufferAttachmentParameteriv)LoadMethod(PREFIX "glGetFramebufferAttachmentParameteriv");
	this->GetFramebufferParameteriv = (PROC_glGetFramebufferParameteriv)LoadMethod(PREFIX "glGetFramebufferParameteriv");
	this->GetGraphicsResetStatus = (PROC_glGetGraphicsResetStatus)LoadMethod(PREFIX "glGetGraphicsResetStatus");
	this->GetInteger64i_v = (PROC_glGetInteger64i_v)LoadMethod(PREFIX "glGetInteger64i_v");
	this->GetInteger64v = (PROC_glGetInteger64v)LoadMethod(PREFIX "glGetInteger64v");
	this->GetIntegeri_v = (PROC_glGetIntegeri_v)LoadMethod(PREFIX "glGetIntegeri_v");
	this->GetIntegerv = (PROC_glGetIntegerv)LoadMethod(PREFIX "glGetIntegerv");
	this->GetInternalformati64v = (PROC_glGetInternalformati64v)LoadMethod(PREFIX "glGetInternalformati64v");
	this->GetInternalformativ = (PROC_glGetInternalformativ)LoadMethod(PREFIX "glGetInternalformativ");
	this->GetMultisamplefv = (PROC_glGetMultisamplefv)LoadMethod(PREFIX "glGetMultisamplefv");
	this->GetNamedBufferParameteri64v = (PROC_glGetNamedBufferParameteri64v)LoadMethod(PREFIX "glGetNamedBufferParameteri64v");
	this->GetNamedBufferParameteriv = (PROC_glGetNamedBufferParameteriv)LoadMethod(PREFIX "glGetNamedBufferParameteriv");
	this->GetNamedBufferPointerv = (PROC_glGetNamedBufferPointerv)LoadMethod(PREFIX "glGetNamedBufferPointerv");
	this->GetNamedBufferSubData = (PROC_glGetNamedBufferSubData)LoadMethod(PREFIX "glGetNamedBufferSubData");
	this->GetNamedFramebufferAttachmentParameteriv = (PROC_glGetNamedFramebufferAttachmentParameteriv)LoadMethod(PREFIX "glGetNamedFramebufferAttachmentParameteriv");
	this->GetNamedFramebufferParameteriv = (PROC_glGetNamedFramebufferParameteriv)LoadMethod(PREFIX "glGetNamedFramebufferParameteriv");
	this->GetNamedRenderbufferParameteriv = (PROC_glGetNamedRenderbufferParameteriv)LoadMethod(PREFIX "glGetNamedRenderbufferParameteriv");
	this->GetObjectLabel = (PROC_glGetObjectLabel)LoadMethod(PREFIX "glGetObjectLabel");
	this->GetObjectPtrLabel = (PROC_glGetObjectPtrLabel)LoadMethod(PREFIX "glGetObjectPtrLabel");
	this->GetPointerv = (PROC_glGetPointerv)LoadMethod(PREFIX "glGetPointerv");
	this->GetProgramBinary = (PROC_glGetProgramBinary)LoadMethod(PREFIX "glGetProgramBinary");
	this->GetProgramInfoLog = (PROC_glGetProgramInfoLog)LoadMethod(PREFIX "glGetProgramInfoLog");
	this->GetProgramInterfaceiv = (PROC_glGetProgramInterfaceiv)LoadMethod(PREFIX "glGetProgramInterfaceiv");
	this->GetProgramPipelineInfoLog = (PROC_glGetProgramPipelineInfoLog)LoadMethod(PREFIX "glGetProgramPipelineInfoLog");
	this->GetProgramPipelineiv = (PROC_glGetProgramPipelineiv)LoadMethod(PREFIX "glGetProgramPipelineiv");
	this->GetProgramResourceIndex = (PROC_glGetProgramResourceIndex)LoadMethod(PREFIX "glGetProgramResourceIndex");
	this->GetProgramResourceLocation = (PROC_glGetProgramResourceLocation)LoadMethod(PREFIX "glGetProgramResourceLocation");
	this->GetProgramResourceLocationIndex = (PROC_glGetProgramResourceLocationIndex)LoadMethod(PREFIX "glGetProgramResourceLocationIndex");
	this->GetProgramResourceName = (PROC_glGetProgramResourceName)LoadMethod(PREFIX "glGetProgramResourceName");
	this->GetProgramResourceiv = (PROC_glGetProgramResourceiv)LoadMethod(PREFIX "glGetProgramResourceiv");
	this->GetProgramStageiv = (PROC_glGetProgramStageiv)LoadMethod(PREFIX "glGetProgramStageiv");
	this->GetProgramiv = (PROC_glGetProgramiv)LoadMethod(PREFIX "glGetProgramiv");
	this->GetQueryBufferObjecti64v = (PROC_glGetQueryBufferObjecti64v)LoadMethod(PREFIX "glGetQueryBufferObjecti64v");
	this->GetQueryBufferObjectiv = (PROC_glGetQueryBufferObjectiv)LoadMethod(PREFIX "glGetQueryBufferObjectiv");
	this->GetQueryBufferObjectui64v = (PROC_glGetQueryBufferObjectui64v)LoadMethod(PREFIX "glGetQueryBufferObjectui64v");
	this->GetQueryBufferObjectuiv = (PROC_glGetQueryBufferObjectuiv)LoadMethod(PREFIX "glGetQueryBufferObjectuiv");
	this->GetQueryIndexediv = (PROC_glGetQueryIndexediv)LoadMethod(PREFIX "glGetQueryIndexediv");
	this->GetQueryObjecti64v = (PROC_glGetQueryObjecti64v)LoadMethod(PREFIX "glGetQueryObjecti64v");
	this->GetQueryObjectiv = (PROC_glGetQueryObjectiv)LoadMethod(PREFIX "glGetQueryObjectiv");
	this->GetQueryObjectui64v = (PROC_glGetQueryObjectui64v)LoadMethod(PREFIX "glGetQueryObjectui64v");
	this->GetQueryObjectuiv = (PROC_glGetQueryObjectuiv)LoadMethod(PREFIX "glGetQueryObjectuiv");
	this->GetQueryiv = (PROC_glGetQueryiv)LoadMethod(PREFIX "glGetQueryiv");
	this->GetRenderbufferParameteriv = (PROC_glGetRenderbufferParameteriv)LoadMethod(PREFIX "glGetRenderbufferParameteriv");
	this->GetSamplerParameterIiv = (PROC_glGetSamplerParameterIiv)LoadMethod(PREFIX "glGetSamplerParameterIiv");
	this->GetSamplerParameterIuiv = (PROC_glGetSamplerParameterIuiv)LoadMethod(PREFIX "glGetSamplerParameterIuiv");
	this->GetSamplerParameterfv = (PROC_glGetSamplerParameterfv)LoadMethod(PREFIX "glGetSamplerParameterfv");
	this->GetSamplerParameteriv = (PROC_glGetSamplerParameteriv)LoadMethod(PREFIX "glGetSamplerParameteriv");
	this->GetShaderInfoLog = (PROC_glGetShaderInfoLog)LoadMethod(PREFIX "glGetShaderInfoLog");
	this->GetShaderPrecisionFormat = (PROC_glGetShaderPrecisionFormat)LoadMethod(PREFIX "glGetShaderPrecisionFormat");
	this->GetShaderSource = (PROC_glGetShaderSource)LoadMethod(PREFIX "glGetShaderSource");
	this->GetShaderiv = (PROC_glGetShaderiv)LoadMethod(PREFIX "glGetShaderiv");
	this->GetString = (PROC_glGetString)LoadMethod(PREFIX "glGetString");
	this->GetStringi = (PROC_glGetStringi)LoadMethod(PREFIX "glGetStringi");
	this->GetSubroutineIndex = (PROC_glGetSubroutineIndex)LoadMethod(PREFIX "glGetSubroutineIndex");
	this->GetSubroutineUniformLocation = (PROC_glGetSubroutineUniformLocation)LoadMethod(PREFIX "glGetSubroutineUniformLocation");
	this->GetSynciv = (PROC_glGetSynciv)LoadMethod(PREFIX "glGetSynciv");
	this->GetTexImage = (PROC_glGetTexImage)LoadMethod(PREFIX "glGetTexImage");
	this->GetTexLevelParameterfv = (PROC_glGetTexLevelParameterfv)LoadMethod(PREFIX "glGetTexLevelParameterfv");
	this->GetTexLevelParameteriv = (PROC_glGetTexLevelParameteriv)LoadMethod(PREFIX "glGetTexLevelParameteriv");
	this->GetTexParameterIiv = (PROC_glGetTexParameterIiv)LoadMethod(PREFIX "glGetTexParameterIiv");
	this->GetTexParameterIuiv = (PROC_glGetTexParameterIuiv)LoadMethod(PREFIX "glGetTexParameterIuiv");
	this->GetTexParameterfv = (PROC_glGetTexParameterfv)LoadMethod(PREFIX "glGetTexParameterfv");
	this->GetTexParameteriv = (PROC_glGetTexParameteriv)LoadMethod(PREFIX "glGetTexParameteriv");
	this->GetTextureImage = (PROC_glGetTextureImage)LoadMethod(PREFIX "glGetTextureImage");
	this->GetTextureLevelParameterfv = (PROC_glGetTextureLevelParameterfv)LoadMethod(PREFIX "glGetTextureLevelParameterfv");
	this->GetTextureLevelParameteriv = (PROC_glGetTextureLevelParameteriv)LoadMethod(PREFIX "glGetTextureLevelParameteriv");
	this->GetTextureParameterIiv = (PROC_glGetTextureParameterIiv)LoadMethod(PREFIX "glGetTextureParameterIiv");
	this->GetTextureParameterIuiv = (PROC_glGetTextureParameterIuiv)LoadMethod(PREFIX "glGetTextureParameterIuiv");
	this->GetTextureParameterfv = (PROC_glGetTextureParameterfv)LoadMethod(PREFIX "glGetTextureParameterfv");
	this->GetTextureParameteriv = (PROC_glGetTextureParameteriv)LoadMethod(PREFIX "glGetTextureParameteriv");
	this->GetTextureSubImage = (PROC_glGetTextureSubImage)LoadMethod(PREFIX "glGetTextureSubImage");
	this->GetTransformFeedbackVarying = (PROC_glGetTransformFeedbackVarying)LoadMethod(PREFIX "glGetTransformFeedbackVarying");
	this->GetTransformFeedbacki64_v = (PROC_glGetTransformFeedbacki64_v)LoadMethod(PREFIX "glGetTransformFeedbacki64_v");
	this->GetTransformFeedbacki_v = (PROC_glGetTransformFeedbacki_v)LoadMethod(PREFIX "glGetTransformFeedbacki_v");
	this->GetTransformFeedbackiv = (PROC_glGetTransformFeedbackiv)LoadMethod(PREFIX "glGetTransformFeedbackiv");
	this->GetUniformBlockIndex = (PROC_glGetUniformBlockIndex)LoadMethod(PREFIX "glGetUniformBlockIndex");
	this->GetUniformIndices = (PROC_glGetUniformIndices)LoadMethod(PREFIX "glGetUniformIndices");
	this->GetUniformLocation = (PROC_glGetUniformLocation)LoadMethod(PREFIX "glGetUniformLocation");
	this->GetUniformSubroutineuiv = (PROC_glGetUniformSubroutineuiv)LoadMethod(PREFIX "glGetUniformSubroutineuiv");
	this->GetUniformdv = (PROC_glGetUniformdv)LoadMethod(PREFIX "glGetUniformdv");
	this->GetUniformfv = (PROC_glGetUniformfv)LoadMethod(PREFIX "glGetUniformfv");
	this->GetUniformiv = (PROC_glGetUniformiv)LoadMethod(PREFIX "glGetUniformiv");
	this->GetUniformuiv = (PROC_glGetUniformuiv)LoadMethod(PREFIX "glGetUniformuiv");
	this->GetVertexArrayIndexed64iv = (PROC_glGetVertexArrayIndexed64iv)LoadMethod(PREFIX "glGetVertexArrayIndexed64iv");
	this->GetVertexArrayIndexediv = (PROC_glGetVertexArrayIndexediv)LoadMethod(PREFIX "glGetVertexArrayIndexediv");
	this->GetVertexArrayiv = (PROC_glGetVertexArrayiv)LoadMethod(PREFIX "glGetVertexArrayiv");
	this->GetVertexAttribIiv = (PROC_glGetVertexAttribIiv)LoadMethod(PREFIX "glGetVertexAttribIiv");
	this->GetVertexAttribIuiv = (PROC_glGetVertexAttribIuiv)LoadMethod(PREFIX "glGetVertexAttribIuiv");
	this->GetVertexAttribLdv = (PROC_glGetVertexAttribLdv)LoadMethod(PREFIX "glGetVertexAttribLdv");
	this->GetVertexAttribPointerv = (PROC_glGetVertexAttribPointerv)LoadMethod(PREFIX "glGetVertexAttribPointerv");
	this->GetVertexAttribdv = (PROC_glGetVertexAttribdv)LoadMethod(PREFIX "glGetVertexAttribdv");
	this->GetVertexAttribfv = (PROC_glGetVertexAttribfv)LoadMethod(PREFIX "glGetVertexAttribfv");
	this->GetVertexAttribiv = (PROC_glGetVertexAttribiv)LoadMethod(PREFIX "glGetVertexAttribiv");
	this->GetnCompressedTexImage = (PROC_glGetnCompressedTexImage)LoadMethod(PREFIX "glGetnCompressedTexImage");
	this->GetnTexImage = (PROC_glGetnTexImage)LoadMethod(PREFIX "glGetnTexImage");
	this->GetnUniformdv = (PROC_glGetnUniformdv)LoadMethod(PREFIX "glGetnUniformdv");
	this->GetnUniformfv = (PROC_glGetnUniformfv)LoadMethod(PREFIX "glGetnUniformfv");
	this->GetnUniformiv = (PROC_glGetnUniformiv)LoadMethod(PREFIX "glGetnUniformiv");
	this->GetnUniformuiv = (PROC_glGetnUniformuiv)LoadMethod(PREFIX "glGetnUniformuiv");
	this->Hint = (PROC_glHint)LoadMethod(PREFIX "glHint");
	this->InvalidateBufferData = (PROC_glInvalidateBufferData)LoadMethod(PREFIX "glInvalidateBufferData");
	this->InvalidateBufferSubData = (PROC_glInvalidateBufferSubData)LoadMethod(PREFIX "glInvalidateBufferSubData");
	this->InvalidateFramebuffer = (PROC_glInvalidateFramebuffer)LoadMethod(PREFIX "glInvalidateFramebuffer");
	this->InvalidateNamedFramebufferData = (PROC_glInvalidateNamedFramebufferData)LoadMethod(PREFIX "glInvalidateNamedFramebufferData");
	this->InvalidateNamedFramebufferSubData = (PROC_glInvalidateNamedFramebufferSubData)LoadMethod(PREFIX "glInvalidateNamedFramebufferSubData");
	this->InvalidateSubFramebuffer = (PROC_glInvalidateSubFramebuffer)LoadMethod(PREFIX "glInvalidateSubFramebuffer");
	this->InvalidateTexImage = (PROC_glInvalidateTexImage)LoadMethod(PREFIX "glInvalidateTexImage");
	this->InvalidateTexSubImage = (PROC_glInvalidateTexSubImage)LoadMethod(PREFIX "glInvalidateTexSubImage");
	this->IsBuffer = (PROC_glIsBuffer)LoadMethod(PREFIX "glIsBuffer");
	this->IsEnabled = (PROC_glIsEnabled)LoadMethod(PREFIX "glIsEnabled");
	this->IsEnabledi = (PROC_glIsEnabledi)LoadMethod(PREFIX "glIsEnabledi");
	this->IsFramebuffer = (PROC_glIsFramebuffer)LoadMethod(PREFIX "glIsFramebuffer");
	this->IsProgram = (PROC_glIsProgram)LoadMethod(PREFIX "glIsProgram");
	this->IsProgramPipeline = (PROC_glIsProgramPipeline)LoadMethod(PREFIX "glIsProgramPipeline");
	this->IsQuery = (PROC_glIsQuery)LoadMethod(PREFIX "glIsQuery");
	this->IsRenderbuffer = (PROC_glIsRenderbuffer)LoadMethod(PREFIX "glIsRenderbuffer");
	this->IsSampler = (PROC_glIsSampler)LoadMethod(PREFIX "glIsSampler");
	this->IsShader = (PROC_glIsShader)LoadMethod(PREFIX "glIsShader");
	this->IsSync = (PROC_glIsSync)LoadMethod(PREFIX "glIsSync");
	this->IsTexture = (PROC_glIsTexture)LoadMethod(PREFIX "glIsTexture");
	this->IsTransformFeedback = (PROC_glIsTransformFeedback)LoadMethod(PREFIX "glIsTransformFeedback");
	this->IsVertexArray = (PROC_glIsVertexArray)LoadMethod(PREFIX "glIsVertexArray");
	this->LineWidth = (PROC_glLineWidth)LoadMethod(PREFIX "glLineWidth");
	this->LinkProgram = (PROC_glLinkProgram)LoadMethod(PREFIX "glLinkProgram");
	this->LogicOp = (PROC_glLogicOp)LoadMethod(PREFIX "glLogicOp");
	this->MapBuffer = (PROC_glMapBuffer)LoadMethod(PREFIX "glMapBuffer");
	this->MapBufferRange = (PROC_glMapBufferRange)LoadMethod(PREFIX "glMapBufferRange");
	this->MapNamedBuffer = (PROC_glMapNamedBuffer)LoadMethod(PREFIX "glMapNamedBuffer");
	this->MapNamedBufferRange = (PROC_glMapNamedBufferRange)LoadMethod(PREFIX "glMapNamedBufferRange");
	this->MemoryBarrier = (PROC_glMemoryBarrier)LoadMethod(PREFIX "glMemoryBarrier");
	this->MemoryBarrierByRegion = (PROC_glMemoryBarrierByRegion)LoadMethod(PREFIX "glMemoryBarrierByRegion");
	this->MinSampleShading = (PROC_glMinSampleShading)LoadMethod(PREFIX "glMinSampleShading");
	this->MultiDrawArrays = (PROC_glMultiDrawArrays)LoadMethod(PREFIX "glMultiDrawArrays");
	this->MultiDrawArraysIndirect = (PROC_glMultiDrawArraysIndirect)LoadMethod(PREFIX "glMultiDrawArraysIndirect");
	this->MultiDrawElements = (PROC_glMultiDrawElements)LoadMethod(PREFIX "glMultiDrawElements");
	this->MultiDrawElementsBaseVertex = (PROC_glMultiDrawElementsBaseVertex)LoadMethod(PREFIX "glMultiDrawElementsBaseVertex");
	this->MultiDrawElementsIndirect = (PROC_glMultiDrawElementsIndirect)LoadMethod(PREFIX "glMultiDrawElementsIndirect");
	this->NamedBufferData = (PROC_glNamedBufferData)LoadMethod(PREFIX "glNamedBufferData");
	this->NamedBufferStorage = (PROC_glNamedBufferStorage)LoadMethod(PREFIX "glNamedBufferStorage");
	this->NamedBufferSubData = (PROC_glNamedBufferSubData)LoadMethod(PREFIX "glNamedBufferSubData");
	this->NamedFramebufferDrawBuffer = (PROC_glNamedFramebufferDrawBuffer)LoadMethod(PREFIX "glNamedFramebufferDrawBuffer");
	this->NamedFramebufferDrawBuffers = (PROC_glNamedFramebufferDrawBuffers)LoadMethod(PREFIX "glNamedFramebufferDrawBuffers");
	this->NamedFramebufferParameteri = (PROC_glNamedFramebufferParameteri)LoadMethod(PREFIX "glNamedFramebufferParameteri");
	this->NamedFramebufferReadBuffer = (PROC_glNamedFramebufferReadBuffer)LoadMethod(PREFIX "glNamedFramebufferReadBuffer");
	this->NamedFramebufferRenderbuffer = (PROC_glNamedFramebufferRenderbuffer)LoadMethod(PREFIX "glNamedFramebufferRenderbuffer");
	this->NamedFramebufferTexture = (PROC_glNamedFramebufferTexture)LoadMethod(PREFIX "glNamedFramebufferTexture");
	this->NamedFramebufferTextureLayer = (PROC_glNamedFramebufferTextureLayer)LoadMethod(PREFIX "glNamedFramebufferTextureLayer");
	this->NamedRenderbufferStorage = (PROC_glNamedRenderbufferStorage)LoadMethod(PREFIX "glNamedRenderbufferStorage");
	this->NamedRenderbufferStorageMultisample = (PROC_glNamedRenderbufferStorageMultisample)LoadMethod(PREFIX "glNamedRenderbufferStorageMultisample");
	this->ObjectLabel = (PROC_glObjectLabel)LoadMethod(PREFIX "glObjectLabel");
	this->ObjectPtrLabel = (PROC_glObjectPtrLabel)LoadMethod(PREFIX "glObjectPtrLabel");
	this->PatchParameterfv = (PROC_glPatchParameterfv)LoadMethod(PREFIX "glPatchParameterfv");
	this->PatchParameteri = (PROC_glPatchParameteri)LoadMethod(PREFIX "glPatchParameteri");
	this->PauseTransformFeedback = (PROC_glPauseTransformFeedback)LoadMethod(PREFIX "glPauseTransformFeedback");
	this->PixelStoref = (PROC_glPixelStoref)LoadMethod(PREFIX "glPixelStoref");
	this->PixelStorei = (PROC_glPixelStorei)LoadMethod(PREFIX "glPixelStorei");
	this->PointParameterf = (PROC_glPointParameterf)LoadMethod(PREFIX "glPointParameterf");
	this->PointParameterfv = (PROC_glPointParameterfv)LoadMethod(PREFIX "glPointParameterfv");
	this->PointParameteri = (PROC_glPointParameteri)LoadMethod(PREFIX "glPointParameteri");
	this->PointParameteriv = (PROC_glPointParameteriv)LoadMethod(PREFIX "glPointParameteriv");
	this->PointSize = (PROC_glPointSize)LoadMethod(PREFIX "glPointSize");
	this->PolygonMode = (PROC_glPolygonMode)LoadMethod(PREFIX "glPolygonMode");
	this->PolygonOffset = (PROC_glPolygonOffset)LoadMethod(PREFIX "glPolygonOffset");
	this->PopDebugGroup = (PROC_glPopDebugGroup)LoadMethod(PREFIX "glPopDebugGroup");
	this->PrimitiveRestartIndex = (PROC_glPrimitiveRestartIndex)LoadMethod(PREFIX "glPrimitiveRestartIndex");
	this->ProgramBinary = (PROC_glProgramBinary)LoadMethod(PREFIX "glProgramBinary");
	this->ProgramParameteri = (PROC_glProgramParameteri)LoadMethod(PREFIX "glProgramParameteri");
	this->ProgramUniform1d = (PROC_glProgramUniform1d)LoadMethod(PREFIX "glProgramUniform1d");
	this->ProgramUniform1dv = (PROC_glProgramUniform1dv)LoadMethod(PREFIX "glProgramUniform1dv");
	this->ProgramUniform1f = (PROC_glProgramUniform1f)LoadMethod(PREFIX "glProgramUniform1f");
	this->ProgramUniform1fv = (PROC_glProgramUniform1fv)LoadMethod(PREFIX "glProgramUniform1fv");
	this->ProgramUniform1i = (PROC_glProgramUniform1i)LoadMethod(PREFIX "glProgramUniform1i");
	this->ProgramUniform1iv = (PROC_glProgramUniform1iv)LoadMethod(PREFIX "glProgramUniform1iv");
	this->ProgramUniform1ui = (PROC_glProgramUniform1ui)LoadMethod(PREFIX "glProgramUniform1ui");
	this->ProgramUniform1uiv = (PROC_glProgramUniform1uiv)LoadMethod(PREFIX "glProgramUniform1uiv");
	this->ProgramUniform2d = (PROC_glProgramUniform2d)LoadMethod(PREFIX "glProgramUniform2d");
	this->ProgramUniform2dv = (PROC_glProgramUniform2dv)LoadMethod(PREFIX "glProgramUniform2dv");
	this->ProgramUniform2f = (PROC_glProgramUniform2f)LoadMethod(PREFIX "glProgramUniform2f");
	this->ProgramUniform2fv = (PROC_glProgramUniform2fv)LoadMethod(PREFIX "glProgramUniform2fv");
	this->ProgramUniform2i = (PROC_glProgramUniform2i)LoadMethod(PREFIX "glProgramUniform2i");
	this->ProgramUniform2iv = (PROC_glProgramUniform2iv)LoadMethod(PREFIX "glProgramUniform2iv");
	this->ProgramUniform2ui = (PROC_glProgramUniform2ui)LoadMethod(PREFIX "glProgramUniform2ui");
	this->ProgramUniform2uiv = (PROC_glProgramUniform2uiv)LoadMethod(PREFIX "glProgramUniform2uiv");
	this->ProgramUniform3d = (PROC_glProgramUniform3d)LoadMethod(PREFIX "glProgramUniform3d");
	this->ProgramUniform3dv = (PROC_glProgramUniform3dv)LoadMethod(PREFIX "glProgramUniform3dv");
	this->ProgramUniform3f = (PROC_glProgramUniform3f)LoadMethod(PREFIX "glProgramUniform3f");
	this->ProgramUniform3fv = (PROC_glProgramUniform3fv)LoadMethod(PREFIX "glProgramUniform3fv");
	this->ProgramUniform3i = (PROC_glProgramUniform3i)LoadMethod(PREFIX "glProgramUniform3i");
	this->ProgramUniform3iv = (PROC_glProgramUniform3iv)LoadMethod(PREFIX "glProgramUniform3iv");
	this->ProgramUniform3ui = (PROC_glProgramUniform3ui)LoadMethod(PREFIX "glProgramUniform3ui");
	this->ProgramUniform3uiv = (PROC_glProgramUniform3uiv)LoadMethod(PREFIX "glProgramUniform3uiv");
	this->ProgramUniform4d = (PROC_glProgramUniform4d)LoadMethod(PREFIX "glProgramUniform4d");
	this->ProgramUniform4dv = (PROC_glProgramUniform4dv)LoadMethod(PREFIX "glProgramUniform4dv");
	this->ProgramUniform4f = (PROC_glProgramUniform4f)LoadMethod(PREFIX "glProgramUniform4f");
	this->ProgramUniform4fv = (PROC_glProgramUniform4fv)LoadMethod(PREFIX "glProgramUniform4fv");
	this->ProgramUniform4i = (PROC_glProgramUniform4i)LoadMethod(PREFIX "glProgramUniform4i");
	this->ProgramUniform4iv = (PROC_glProgramUniform4iv)LoadMethod(PREFIX "glProgramUniform4iv");
	this->ProgramUniform4ui = (PROC_glProgramUniform4ui)LoadMethod(PREFIX "glProgramUniform4ui");
	this->ProgramUniform4uiv = (PROC_glProgramUniform4uiv)LoadMethod(PREFIX "glProgramUniform4uiv");
	this->ProgramUniformMatrix2dv = (PROC_glProgramUniformMatrix2dv)LoadMethod(PREFIX "glProgramUniformMatrix2dv");
	this->ProgramUniformMatrix2fv = (PROC_glProgramUniformMatrix2fv)LoadMethod(PREFIX "glProgramUniformMatrix2fv");
	this->ProgramUniformMatrix2x3dv = (PROC_glProgramUniformMatrix2x3dv)LoadMethod(PREFIX "glProgramUniformMatrix2x3dv");
	this->ProgramUniformMatrix2x3fv = (PROC_glProgramUniformMatrix2x3fv)LoadMethod(PREFIX "glProgramUniformMatrix2x3fv");
	this->ProgramUniformMatrix2x4dv = (PROC_glProgramUniformMatrix2x4dv)LoadMethod(PREFIX "glProgramUniformMatrix2x4dv");
	this->ProgramUniformMatrix2x4fv = (PROC_glProgramUniformMatrix2x4fv)LoadMethod(PREFIX "glProgramUniformMatrix2x4fv");
	this->ProgramUniformMatrix3dv = (PROC_glProgramUniformMatrix3dv)LoadMethod(PREFIX "glProgramUniformMatrix3dv");
	this->ProgramUniformMatrix3fv = (PROC_glProgramUniformMatrix3fv)LoadMethod(PREFIX "glProgramUniformMatrix3fv");
	this->ProgramUniformMatrix3x2dv = (PROC_glProgramUniformMatrix3x2dv)LoadMethod(PREFIX "glProgramUniformMatrix3x2dv");
	this->ProgramUniformMatrix3x2fv = (PROC_glProgramUniformMatrix3x2fv)LoadMethod(PREFIX "glProgramUniformMatrix3x2fv");
	this->ProgramUniformMatrix3x4dv = (PROC_glProgramUniformMatrix3x4dv)LoadMethod(PREFIX "glProgramUniformMatrix3x4dv");
	this->ProgramUniformMatrix3x4fv = (PROC_glProgramUniformMatrix3x4fv)LoadMethod(PREFIX "glProgramUniformMatrix3x4fv");
	this->ProgramUniformMatrix4dv = (PROC_glProgramUniformMatrix4dv)LoadMethod(PREFIX "glProgramUniformMatrix4dv");
	this->ProgramUniformMatrix4fv = (PROC_glProgramUniformMatrix4fv)LoadMethod(PREFIX "glProgramUniformMatrix4fv");
	this->ProgramUniformMatrix4x2dv = (PROC_glProgramUniformMatrix4x2dv)LoadMethod(PREFIX "glProgramUniformMatrix4x2dv");
	this->ProgramUniformMatrix4x2fv = (PROC_glProgramUniformMatrix4x2fv)LoadMethod(PREFIX "glProgramUniformMatrix4x2fv");
	this->ProgramUniformMatrix4x3dv = (PROC_glProgramUniformMatrix4x3dv)LoadMethod(PREFIX "glProgramUniformMatrix4x3dv");
	this->ProgramUniformMatrix4x3fv = (PROC_glProgramUniformMatrix4x3fv)LoadMethod(PREFIX "glProgramUniformMatrix4x3fv");
	this->ProvokingVertex = (PROC_glProvokingVertex)LoadMethod(PREFIX "glProvokingVertex");
	this->PushDebugGroup = (PROC_glPushDebugGroup)LoadMethod(PREFIX "glPushDebugGroup");
	this->QueryCounter = (PROC_glQueryCounter)LoadMethod(PREFIX "glQueryCounter");
	this->ReadBuffer = (PROC_glReadBuffer)LoadMethod(PREFIX "glReadBuffer");
	this->ReadPixels = (PROC_glReadPixels)LoadMethod(PREFIX "glReadPixels");
	this->ReadnPixels = (PROC_glReadnPixels)LoadMethod(PREFIX "glReadnPixels");
	this->ReleaseShaderCompiler = (PROC_glReleaseShaderCompiler)LoadMethod(PREFIX "glReleaseShaderCompiler");
	this->RenderbufferStorage = (PROC_glRenderbufferStorage)LoadMethod(PREFIX "glRenderbufferStorage");
	this->RenderbufferStorageMultisample = (PROC_glRenderbufferStorageMultisample)LoadMethod(PREFIX "glRenderbufferStorageMultisample");
	this->ResumeTransformFeedback = (PROC_glResumeTransformFeedback)LoadMethod(PREFIX "glResumeTransformFeedback");
	this->SampleCoverage = (PROC_glSampleCoverage)LoadMethod(PREFIX "glSampleCoverage");
	this->SampleMaski = (PROC_glSampleMaski)LoadMethod(PREFIX "glSampleMaski");
	this->SamplerParameterIiv = (PROC_glSamplerParameterIiv)LoadMethod(PREFIX "glSamplerParameterIiv");
	this->SamplerParameterIuiv = (PROC_glSamplerParameterIuiv)LoadMethod(PREFIX "glSamplerParameterIuiv");
	this->SamplerParameterf = (PROC_glSamplerParameterf)LoadMethod(PREFIX "glSamplerParameterf");
	this->SamplerParameterfv = (PROC_glSamplerParameterfv)LoadMethod(PREFIX "glSamplerParameterfv");
	this->SamplerParameteri = (PROC_glSamplerParameteri)LoadMethod(PREFIX "glSamplerParameteri");
	this->SamplerParameteriv = (PROC_glSamplerParameteriv)LoadMethod(PREFIX "glSamplerParameteriv");
	this->Scissor = (PROC_glScissor)LoadMethod(PREFIX "glScissor");
	this->ScissorArrayv = (PROC_glScissorArrayv)LoadMethod(PREFIX "glScissorArrayv");
	this->ScissorIndexed = (PROC_glScissorIndexed)LoadMethod(PREFIX "glScissorIndexed");
	this->ScissorIndexedv = (PROC_glScissorIndexedv)LoadMethod(PREFIX "glScissorIndexedv");
	this->ShaderBinary = (PROC_glShaderBinary)LoadMethod(PREFIX "glShaderBinary");
	this->ShaderSource = (PROC_glShaderSource)LoadMethod(PREFIX "glShaderSource");
	this->ShaderStorageBlockBinding = (PROC_glShaderStorageBlockBinding)LoadMethod(PREFIX "glShaderStorageBlockBinding");
	this->StencilFunc = (PROC_glStencilFunc)LoadMethod(PREFIX "glStencilFunc");
	this->StencilFuncSeparate = (PROC_glStencilFuncSeparate)LoadMethod(PREFIX "glStencilFuncSeparate");
	this->StencilMask = (PROC_glStencilMask)LoadMethod(PREFIX "glStencilMask");
	this->StencilMaskSeparate = (PROC_glStencilMaskSeparate)LoadMethod(PREFIX "glStencilMaskSeparate");
	this->StencilOp = (PROC_glStencilOp)LoadMethod(PREFIX "glStencilOp");
	this->StencilOpSeparate = (PROC_glStencilOpSeparate)LoadMethod(PREFIX "glStencilOpSeparate");
	this->TexBuffer = (PROC_glTexBuffer)LoadMethod(PREFIX "glTexBuffer");
	this->TexBufferRange = (PROC_glTexBufferRange)LoadMethod(PREFIX "glTexBufferRange");
	this->TexImage1D = (PROC_glTexImage1D)LoadMethod(PREFIX "glTexImage1D");
	this->TexImage2D = (PROC_glTexImage2D)LoadMethod(PREFIX "glTexImage2D");
	this->TexImage2DMultisample = (PROC_glTexImage2DMultisample)LoadMethod(PREFIX "glTexImage2DMultisample");
	this->TexImage3D = (PROC_glTexImage3D)LoadMethod(PREFIX "glTexImage3D");
	this->TexImage3DMultisample = (PROC_glTexImage3DMultisample)LoadMethod(PREFIX "glTexImage3DMultisample");
	this->TexParameterIiv = (PROC_glTexParameterIiv)LoadMethod(PREFIX "glTexParameterIiv");
	this->TexParameterIuiv = (PROC_glTexParameterIuiv)LoadMethod(PREFIX "glTexParameterIuiv");
	this->TexParameterf = (PROC_glTexParameterf)LoadMethod(PREFIX "glTexParameterf");
	this->TexParameterfv = (PROC_glTexParameterfv)LoadMethod(PREFIX "glTexParameterfv");
	this->TexParameteri = (PROC_glTexParameteri)LoadMethod(PREFIX "glTexParameteri");
	this->TexParameteriv = (PROC_glTexParameteriv)LoadMethod(PREFIX "glTexParameteriv");
	this->TexStorage1D = (PROC_glTexStorage1D)LoadMethod(PREFIX "glTexStorage1D");
	this->TexStorage2D = (PROC_glTexStorage2D)LoadMethod(PREFIX "glTexStorage2D");
	this->TexStorage2DMultisample = (PROC_glTexStorage2DMultisample)LoadMethod(PREFIX "glTexStorage2DMultisample");
	this->TexStorage3D = (PROC_glTexStorage3D)LoadMethod(PREFIX "glTexStorage3D");
	this->TexStorage3DMultisample = (PROC_glTexStorage3DMultisample)LoadMethod(PREFIX "glTexStorage3DMultisample");
	this->TexSubImage1D = (PROC_glTexSubImage1D)LoadMethod(PREFIX "glTexSubImage1D");
	this->TexSubImage2D = (PROC_glTexSubImage2D)LoadMethod(PREFIX "glTexSubImage2D");
	this->TexSubImage3D = (PROC_glTexSubImage3D)LoadMethod(PREFIX "glTexSubImage3D");
	this->TextureBarrier = (PROC_glTextureBarrier)LoadMethod(PREFIX "glTextureBarrier");
	this->TextureBuffer = (PROC_glTextureBuffer)LoadMethod(PREFIX "glTextureBuffer");
	this->TextureBufferRange = (PROC_glTextureBufferRange)LoadMethod(PREFIX "glTextureBufferRange");
	this->TextureParameterIiv = (PROC_glTextureParameterIiv)LoadMethod(PREFIX "glTextureParameterIiv");
	this->TextureParameterIuiv = (PROC_glTextureParameterIuiv)LoadMethod(PREFIX "glTextureParameterIuiv");
	this->TextureParameterf = (PROC_glTextureParameterf)LoadMethod(PREFIX "glTextureParameterf");
	this->TextureParameterfv = (PROC_glTextureParameterfv)LoadMethod(PREFIX "glTextureParameterfv");
	this->TextureParameteri = (PROC_glTextureParameteri)LoadMethod(PREFIX "glTextureParameteri");
	this->TextureParameteriv = (PROC_glTextureParameteriv)LoadMethod(PREFIX "glTextureParameteriv");
	this->TextureStorage1D = (PROC_glTextureStorage1D)LoadMethod(PREFIX "glTextureStorage1D");
	this->TextureStorage2D = (PROC_glTextureStorage2D)LoadMethod(PREFIX "glTextureStorage2D");
	this->TextureStorage2DMultisample = (PROC_glTextureStorage2DMultisample)LoadMethod(PREFIX "glTextureStorage2DMultisample");
	this->TextureStorage3D = (PROC_glTextureStorage3D)LoadMethod(PREFIX "glTextureStorage3D");
	this->TextureStorage3DMultisample = (PROC_glTextureStorage3DMultisample)LoadMethod(PREFIX "glTextureStorage3DMultisample");
	this->TextureSubImage1D = (PROC_glTextureSubImage1D)LoadMethod(PREFIX "glTextureSubImage1D");
	this->TextureSubImage2D = (PROC_glTextureSubImage2D)LoadMethod(PREFIX "glTextureSubImage2D");
	this->TextureSubImage3D = (PROC_glTextureSubImage3D)LoadMethod(PREFIX "glTextureSubImage3D");
	this->TextureView = (PROC_glTextureView)LoadMethod(PREFIX "glTextureView");
	this->TransformFeedbackBufferBase = (PROC_glTransformFeedbackBufferBase)LoadMethod(PREFIX "glTransformFeedbackBufferBase");
	this->TransformFeedbackBufferRange = (PROC_glTransformFeedbackBufferRange)LoadMethod(PREFIX "glTransformFeedbackBufferRange");
	this->TransformFeedbackVaryings = (PROC_glTransformFeedbackVaryings)LoadMethod(PREFIX "glTransformFeedbackVaryings");
	this->Uniform1d = (PROC_glUniform1d)LoadMethod(PREFIX "glUniform1d");
	this->Uniform1dv = (PROC_glUniform1dv)LoadMethod(PREFIX "glUniform1dv");
	this->Uniform1f = (PROC_glUniform1f)LoadMethod(PREFIX "glUniform1f");
	this->Uniform1fv = (PROC_glUniform1fv)LoadMethod(PREFIX "glUniform1fv");
	this->Uniform1i = (PROC_glUniform1i)LoadMethod(PREFIX "glUniform1i");
	this->Uniform1iv = (PROC_glUniform1iv)LoadMethod(PREFIX "glUniform1iv");
	this->Uniform1ui = (PROC_glUniform1ui)LoadMethod(PREFIX "glUniform1ui");
	this->Uniform1uiv = (PROC_glUniform1uiv)LoadMethod(PREFIX "glUniform1uiv");
	this->Uniform2d = (PROC_glUniform2d)LoadMethod(PREFIX "glUniform2d");
	this->Uniform2dv = (PROC_glUniform2dv)LoadMethod(PREFIX "glUniform2dv");
	this->Uniform2f = (PROC_glUniform2f)LoadMethod(PREFIX "glUniform2f");
	this->Uniform2fv = (PROC_glUniform2fv)LoadMethod(PREFIX "glUniform2fv");
	this->Uniform2i = (PROC_glUniform2i)LoadMethod(PREFIX "glUniform2i");
	this->Uniform2iv = (PROC_glUniform2iv)LoadMethod(PREFIX "glUniform2iv");
	this->Uniform2ui = (PROC_glUniform2ui)LoadMethod(PREFIX "glUniform2ui");
	this->Uniform2uiv = (PROC_glUniform2uiv)LoadMethod(PREFIX "glUniform2uiv");
	this->Uniform3d = (PROC_glUniform3d)LoadMethod(PREFIX "glUniform3d");
	this->Uniform3dv = (PROC_glUniform3dv)LoadMethod(PREFIX "glUniform3dv");
	this->Uniform3f = (PROC_glUniform3f)LoadMethod(PREFIX "glUniform3f");
	this->Uniform3fv = (PROC_glUniform3fv)LoadMethod(PREFIX "glUniform3fv");
	this->Uniform3i = (PROC_glUniform3i)LoadMethod(PREFIX "glUniform3i");
	this->Uniform3iv = (PROC_glUniform3iv)LoadMethod(PREFIX "glUniform3iv");
	this->Uniform3ui = (PROC_glUniform3ui)LoadMethod(PREFIX "glUniform3ui");
	this->Uniform3uiv = (PROC_glUniform3uiv)LoadMethod(PREFIX "glUniform3uiv");
	this->Uniform4d = (PROC_glUniform4d)LoadMethod(PREFIX "glUniform4d");
	this->Uniform4dv = (PROC_glUniform4dv)LoadMethod(PREFIX "glUniform4dv");
	this->Uniform4f = (PROC_glUniform4f)LoadMethod(PREFIX "glUniform4f");
	this->Uniform4fv = (PROC_glUniform4fv)LoadMethod(PREFIX "glUniform4fv");
	this->Uniform4i = (PROC_glUniform4i)LoadMethod(PREFIX "glUniform4i");
	this->Uniform4iv = (PROC_glUniform4iv)LoadMethod(PREFIX "glUniform4iv");
	this->Uniform4ui = (PROC_glUniform4ui)LoadMethod(PREFIX "glUniform4ui");
	this->Uniform4uiv = (PROC_glUniform4uiv)LoadMethod(PREFIX "glUniform4uiv");
	this->UniformBlockBinding = (PROC_glUniformBlockBinding)LoadMethod(PREFIX "glUniformBlockBinding");
	this->UniformMatrix2dv = (PROC_glUniformMatrix2dv)LoadMethod(PREFIX "glUniformMatrix2dv");
	this->UniformMatrix2fv = (PROC_glUniformMatrix2fv)LoadMethod(PREFIX "glUniformMatrix2fv");
	this->UniformMatrix2x3dv = (PROC_glUniformMatrix2x3dv)LoadMethod(PREFIX "glUniformMatrix2x3dv");
	this->UniformMatrix2x3fv = (PROC_glUniformMatrix2x3fv)LoadMethod(PREFIX "glUniformMatrix2x3fv");
	this->UniformMatrix2x4dv = (PROC_glUniformMatrix2x4dv)LoadMethod(PREFIX "glUniformMatrix2x4dv");
	this->UniformMatrix2x4fv = (PROC_glUniformMatrix2x4fv)LoadMethod(PREFIX "glUniformMatrix2x4fv");
	this->UniformMatrix3dv = (PROC_glUniformMatrix3dv)LoadMethod(PREFIX "glUniformMatrix3dv");
	this->UniformMatrix3fv = (PROC_glUniformMatrix3fv)LoadMethod(PREFIX "glUniformMatrix3fv");
	this->UniformMatrix3x2dv = (PROC_glUniformMatrix3x2dv)LoadMethod(PREFIX "glUniformMatrix3x2dv");
	this->UniformMatrix3x2fv = (PROC_glUniformMatrix3x2fv)LoadMethod(PREFIX "glUniformMatrix3x2fv");
	this->UniformMatrix3x4dv = (PROC_glUniformMatrix3x4dv)LoadMethod(PREFIX "glUniformMatrix3x4dv");
	this->UniformMatrix3x4fv = (PROC_glUniformMatrix3x4fv)LoadMethod(PREFIX "glUniformMatrix3x4fv");
	this->UniformMatrix4dv = (PROC_glUniformMatrix4dv)LoadMethod(PREFIX "glUniformMatrix4dv");
	this->UniformMatrix4fv = (PROC_glUniformMatrix4fv)LoadMethod(PREFIX "glUniformMatrix4fv");
	this->UniformMatrix4x2dv = (PROC_glUniformMatrix4x2dv)LoadMethod(PREFIX "glUniformMatrix4x2dv");
	this->UniformMatrix4x2fv = (PROC_glUniformMatrix4x2fv)LoadMethod(PREFIX "glUniformMatrix4x2fv");
	this->UniformMatrix4x3dv = (PROC_glUniformMatrix4x3dv)LoadMethod(PREFIX "glUniformMatrix4x3dv");
	this->UniformMatrix4x3fv = (PROC_glUniformMatrix4x3fv)LoadMethod(PREFIX "glUniformMatrix4x3fv");
	this->UniformSubroutinesuiv = (PROC_glUniformSubroutinesuiv)LoadMethod(PREFIX "glUniformSubroutinesuiv");
	this->UnmapBuffer = (PROC_glUnmapBuffer)LoadMethod(PREFIX "glUnmapBuffer");
	this->UnmapNamedBuffer = (PROC_glUnmapNamedBuffer)LoadMethod(PREFIX "glUnmapNamedBuffer");
	this->UseProgram = (PROC_glUseProgram)LoadMethod(PREFIX "glUseProgram");
	this->UseProgramStages = (PROC_glUseProgramStages)LoadMethod(PREFIX "glUseProgramStages");
	this->ValidateProgram = (PROC_glValidateProgram)LoadMethod(PREFIX "glValidateProgram");
	this->ValidateProgramPipeline = (PROC_glValidateProgramPipeline)LoadMethod(PREFIX "glValidateProgramPipeline");
	this->VertexArrayAttribBinding = (PROC_glVertexArrayAttribBinding)LoadMethod(PREFIX "glVertexArrayAttribBinding");
	this->VertexArrayAttribFormat = (PROC_glVertexArrayAttribFormat)LoadMethod(PREFIX "glVertexArrayAttribFormat");
	this->VertexArrayAttribIFormat = (PROC_glVertexArrayAttribIFormat)LoadMethod(PREFIX "glVertexArrayAttribIFormat");
	this->VertexArrayAttribLFormat = (PROC_glVertexArrayAttribLFormat)LoadMethod(PREFIX "glVertexArrayAttribLFormat");
	this->VertexArrayBindingDivisor = (PROC_glVertexArrayBindingDivisor)LoadMethod(PREFIX "glVertexArrayBindingDivisor");
	this->VertexArrayElementBuffer = (PROC_glVertexArrayElementBuffer)LoadMethod(PREFIX "glVertexArrayElementBuffer");
	this->VertexArrayVertexBuffer = (PROC_glVertexArrayVertexBuffer)LoadMethod(PREFIX "glVertexArrayVertexBuffer");
	this->VertexArrayVertexBuffers = (PROC_glVertexArrayVertexBuffers)LoadMethod(PREFIX "glVertexArrayVertexBuffers");
	this->VertexAttrib1d = (PROC_glVertexAttrib1d)LoadMethod(PREFIX "glVertexAttrib1d");
	this->VertexAttrib1dv = (PROC_glVertexAttrib1dv)LoadMethod(PREFIX "glVertexAttrib1dv");
	this->VertexAttrib1f = (PROC_glVertexAttrib1f)LoadMethod(PREFIX "glVertexAttrib1f");
	this->VertexAttrib1fv = (PROC_glVertexAttrib1fv)LoadMethod(PREFIX "glVertexAttrib1fv");
	this->VertexAttrib1s = (PROC_glVertexAttrib1s)LoadMethod(PREFIX "glVertexAttrib1s");
	this->VertexAttrib1sv = (PROC_glVertexAttrib1sv)LoadMethod(PREFIX "glVertexAttrib1sv");
	this->VertexAttrib2d = (PROC_glVertexAttrib2d)LoadMethod(PREFIX "glVertexAttrib2d");
	this->VertexAttrib2dv = (PROC_glVertexAttrib2dv)LoadMethod(PREFIX "glVertexAttrib2dv");
	this->VertexAttrib2f = (PROC_glVertexAttrib2f)LoadMethod(PREFIX "glVertexAttrib2f");
	this->VertexAttrib2fv = (PROC_glVertexAttrib2fv)LoadMethod(PREFIX "glVertexAttrib2fv");
	this->VertexAttrib2s = (PROC_glVertexAttrib2s)LoadMethod(PREFIX "glVertexAttrib2s");
	this->VertexAttrib2sv = (PROC_glVertexAttrib2sv)LoadMethod(PREFIX "glVertexAttrib2sv");
	this->VertexAttrib3d = (PROC_glVertexAttrib3d)LoadMethod(PREFIX "glVertexAttrib3d");
	this->VertexAttrib3dv = (PROC_glVertexAttrib3dv)LoadMethod(PREFIX "glVertexAttrib3dv");
	this->VertexAttrib3f = (PROC_glVertexAttrib3f)LoadMethod(PREFIX "glVertexAttrib3f");
	this->VertexAttrib3fv = (PROC_glVertexAttrib3fv)LoadMethod(PREFIX "glVertexAttrib3fv");
	this->VertexAttrib3s = (PROC_glVertexAttrib3s)LoadMethod(PREFIX "glVertexAttrib3s");
	this->VertexAttrib3sv = (PROC_glVertexAttrib3sv)LoadMethod(PREFIX "glVertexAttrib3sv");
	this->VertexAttrib4Nbv = (PROC_glVertexAttrib4Nbv)LoadMethod(PREFIX "glVertexAttrib4Nbv");
	this->VertexAttrib4Niv = (PROC_glVertexAttrib4Niv)LoadMethod(PREFIX "glVertexAttrib4Niv");
	this->VertexAttrib4Nsv = (PROC_glVertexAttrib4Nsv)LoadMethod(PREFIX "glVertexAttrib4Nsv");
	this->VertexAttrib4Nub = (PROC_glVertexAttrib4Nub)LoadMethod(PREFIX "glVertexAttrib4Nub");
	this->VertexAttrib4Nubv = (PROC_glVertexAttrib4Nubv)LoadMethod(PREFIX "glVertexAttrib4Nubv");
	this->VertexAttrib4Nuiv = (PROC_glVertexAttrib4Nuiv)LoadMethod(PREFIX "glVertexAttrib4Nuiv");
	this->VertexAttrib4Nusv = (PROC_glVertexAttrib4Nusv)LoadMethod(PREFIX "glVertexAttrib4Nusv");
	this->VertexAttrib4bv = (PROC_glVertexAttrib4bv)LoadMethod(PREFIX "glVertexAttrib4bv");
	this->VertexAttrib4d = (PROC_glVertexAttrib4d)LoadMethod(PREFIX "glVertexAttrib4d");
	this->VertexAttrib4dv = (PROC_glVertexAttrib4dv)LoadMethod(PREFIX "glVertexAttrib4dv");
	this->VertexAttrib4f = (PROC_glVertexAttrib4f)LoadMethod(PREFIX "glVertexAttrib4f");
	this->VertexAttrib4fv = (PROC_glVertexAttrib4fv)LoadMethod(PREFIX "glVertexAttrib4fv");
	this->VertexAttrib4iv = (PROC_glVertexAttrib4iv)LoadMethod(PREFIX "glVertexAttrib4iv");
	this->VertexAttrib4s = (PROC_glVertexAttrib4s)LoadMethod(PREFIX "glVertexAttrib4s");
	this->VertexAttrib4sv = (PROC_glVertexAttrib4sv)LoadMethod(PREFIX "glVertexAttrib4sv");
	this->VertexAttrib4ubv = (PROC_glVertexAttrib4ubv)LoadMethod(PREFIX "glVertexAttrib4ubv");
	this->VertexAttrib4uiv = (PROC_glVertexAttrib4uiv)LoadMethod(PREFIX "glVertexAttrib4uiv");
	this->VertexAttrib4usv = (PROC_glVertexAttrib4usv)LoadMethod(PREFIX "glVertexAttrib4usv");
	this->VertexAttribBinding = (PROC_glVertexAttribBinding)LoadMethod(PREFIX "glVertexAttribBinding");
	this->VertexAttribDivisor = (PROC_glVertexAttribDivisor)LoadMethod(PREFIX "glVertexAttribDivisor");
	this->VertexAttribFormat = (PROC_glVertexAttribFormat)LoadMethod(PREFIX "glVertexAttribFormat");
	this->VertexAttribI1i = (PROC_glVertexAttribI1i)LoadMethod(PREFIX "glVertexAttribI1i");
	this->VertexAttribI1iv = (PROC_glVertexAttribI1iv)LoadMethod(PREFIX "glVertexAttribI1iv");
	this->VertexAttribI1ui = (PROC_glVertexAttribI1ui)LoadMethod(PREFIX "glVertexAttribI1ui");
	this->VertexAttribI1uiv = (PROC_glVertexAttribI1uiv)LoadMethod(PREFIX "glVertexAttribI1uiv");
	this->VertexAttribI2i = (PROC_glVertexAttribI2i)LoadMethod(PREFIX "glVertexAttribI2i");
	this->VertexAttribI2iv = (PROC_glVertexAttribI2iv)LoadMethod(PREFIX "glVertexAttribI2iv");
	this->VertexAttribI2ui = (PROC_glVertexAttribI2ui)LoadMethod(PREFIX "glVertexAttribI2ui");
	this->VertexAttribI2uiv = (PROC_glVertexAttribI2uiv)LoadMethod(PREFIX "glVertexAttribI2uiv");
	this->VertexAttribI3i = (PROC_glVertexAttribI3i)LoadMethod(PREFIX "glVertexAttribI3i");
	this->VertexAttribI3iv = (PROC_glVertexAttribI3iv)LoadMethod(PREFIX "glVertexAttribI3iv");
	this->VertexAttribI3ui = (PROC_glVertexAttribI3ui)LoadMethod(PREFIX "glVertexAttribI3ui");
	this->VertexAttribI3uiv = (PROC_glVertexAttribI3uiv)LoadMethod(PREFIX "glVertexAttribI3uiv");
	this->VertexAttribI4bv = (PROC_glVertexAttribI4bv)LoadMethod(PREFIX "glVertexAttribI4bv");
	this->VertexAttribI4i = (PROC_glVertexAttribI4i)LoadMethod(PREFIX "glVertexAttribI4i");
	this->VertexAttribI4iv = (PROC_glVertexAttribI4iv)LoadMethod(PREFIX "glVertexAttribI4iv");
	this->VertexAttribI4sv = (PROC_glVertexAttribI4sv)LoadMethod(PREFIX "glVertexAttribI4sv");
	this->VertexAttribI4ubv = (PROC_glVertexAttribI4ubv)LoadMethod(PREFIX "glVertexAttribI4ubv");
	this->VertexAttribI4ui = (PROC_glVertexAttribI4ui)LoadMethod(PREFIX "glVertexAttribI4ui");
	this->VertexAttribI4uiv = (PROC_glVertexAttribI4uiv)LoadMethod(PREFIX "glVertexAttribI4uiv");
	this->VertexAttribI4usv = (PROC_glVertexAttribI4usv)LoadMethod(PREFIX "glVertexAttribI4usv");
	this->VertexAttribIFormat = (PROC_glVertexAttribIFormat)LoadMethod(PREFIX "glVertexAttribIFormat");
	this->VertexAttribIPointer = (PROC_glVertexAttribIPointer)LoadMethod(PREFIX "glVertexAttribIPointer");
	this->VertexAttribL1d = (PROC_glVertexAttribL1d)LoadMethod(PREFIX "glVertexAttribL1d");
	this->VertexAttribL1dv = (PROC_glVertexAttribL1dv)LoadMethod(PREFIX "glVertexAttribL1dv");
	this->VertexAttribL2d = (PROC_glVertexAttribL2d)LoadMethod(PREFIX "glVertexAttribL2d");
	this->VertexAttribL2dv = (PROC_glVertexAttribL2dv)LoadMethod(PREFIX "glVertexAttribL2dv");
	this->VertexAttribL3d = (PROC_glVertexAttribL3d)LoadMethod(PREFIX "glVertexAttribL3d");
	this->VertexAttribL3dv = (PROC_glVertexAttribL3dv)LoadMethod(PREFIX "glVertexAttribL3dv");
	this->VertexAttribL4d = (PROC_glVertexAttribL4d)LoadMethod(PREFIX "glVertexAttribL4d");
	this->VertexAttribL4dv = (PROC_glVertexAttribL4dv)LoadMethod(PREFIX "glVertexAttribL4dv");
	this->VertexAttribLFormat = (PROC_glVertexAttribLFormat)LoadMethod(PREFIX "glVertexAttribLFormat");
	this->VertexAttribLPointer = (PROC_glVertexAttribLPointer)LoadMethod(PREFIX "glVertexAttribLPointer");
	this->VertexAttribP1ui = (PROC_glVertexAttribP1ui)LoadMethod(PREFIX "glVertexAttribP1ui");
	this->VertexAttribP1uiv = (PROC_glVertexAttribP1uiv)LoadMethod(PREFIX "glVertexAttribP1uiv");
	this->VertexAttribP2ui = (PROC_glVertexAttribP2ui)LoadMethod(PREFIX "glVertexAttribP2ui");
	this->VertexAttribP2uiv = (PROC_glVertexAttribP2uiv)LoadMethod(PREFIX "glVertexAttribP2uiv");
	this->VertexAttribP3ui = (PROC_glVertexAttribP3ui)LoadMethod(PREFIX "glVertexAttribP3ui");
	this->VertexAttribP3uiv = (PROC_glVertexAttribP3uiv)LoadMethod(PREFIX "glVertexAttribP3uiv");
	this->VertexAttribP4ui = (PROC_glVertexAttribP4ui)LoadMethod(PREFIX "glVertexAttribP4ui");
	this->VertexAttribP4uiv = (PROC_glVertexAttribP4uiv)LoadMethod(PREFIX "glVertexAttribP4uiv");
	this->VertexAttribPointer = (PROC_glVertexAttribPointer)LoadMethod(PREFIX "glVertexAttribPointer");
	this->VertexBindingDivisor = (PROC_glVertexBindingDivisor)LoadMethod(PREFIX "glVertexBindingDivisor");
	this->Viewport = (PROC_glViewport)LoadMethod(PREFIX "glViewport");
	this->ViewportArrayv = (PROC_glViewportArrayv)LoadMethod(PREFIX "glViewportArrayv");
	this->ViewportIndexedf = (PROC_glViewportIndexedf)LoadMethod(PREFIX "glViewportIndexedf");
	this->ViewportIndexedfv = (PROC_glViewportIndexedfv)LoadMethod(PREFIX "glViewportIndexedfv");

	// Fake implementation

	if (INVALID_METHOD(this->GetProgramStageiv)) {
		this->GetProgramStageiv = FakeGetProgramStageiv;
	}

	// GetActiveSubroutineName
	// GetActiveSubroutineUniformName
	// GetSubroutineIndex
	// GetSubroutineUniformLocation

	// Must have

	PyObject * loading_log = PyUnicode_FromStringAndSize(0, 0);

	if (INVALID_METHOD(this->ActiveTexture)) {
		AddToLog(&loading_log, "glActiveTexture");
	}

	if (INVALID_METHOD(this->AttachShader)) {
		AddToLog(&loading_log, "glAttachShader");
	}

	if (INVALID_METHOD(this->BeginTransformFeedback)) {
		AddToLog(&loading_log, "glBeginTransformFeedback");
	}

	if (INVALID_METHOD(this->BindBuffer)) {
		AddToLog(&loading_log, "glBindBuffer");
	}

	if (INVALID_METHOD(this->BindBufferBase)) {
		AddToLog(&loading_log, "glBindBufferBase");
	}

	if (INVALID_METHOD(this->BindFramebuffer)) {
		AddToLog(&loading_log, "glBindFramebuffer");
	}

	if (INVALID_METHOD(this->BindRenderbuffer)) {
		AddToLog(&loading_log, "glBindRenderbuffer");
	}

	if (INVALID_METHOD(this->BindTexture)) {
		AddToLog(&loading_log, "glBindTexture");
	}

	if (INVALID_METHOD(this->BindVertexArray)) {
		AddToLog(&loading_log, "glBindVertexArray");
	}

	if (INVALID_METHOD(this->BlendFunc)) {
		AddToLog(&loading_log, "glBlendFunc");
	}

	if (INVALID_METHOD(this->BufferData)) {
		AddToLog(&loading_log, "glBufferData");
	}

	if (INVALID_METHOD(this->BufferSubData)) {
		AddToLog(&loading_log, "glBufferSubData");
	}

	if (INVALID_METHOD(this->CheckFramebufferStatus)) {
		AddToLog(&loading_log, "glCheckFramebufferStatus");
	}

	if (INVALID_METHOD(this->Clear)) {
		AddToLog(&loading_log, "glClear");
	}

	if (INVALID_METHOD(this->ClearColor)) {
		AddToLog(&loading_log, "glClearColor");
	}

	if (INVALID_METHOD(this->CompileShader)) {
		AddToLog(&loading_log, "glCompileShader");
	}

	if (INVALID_METHOD(this->CopyBufferSubData)) {
		AddToLog(&loading_log, "glCopyBufferSubData");
	}

	if (INVALID_METHOD(this->CreateProgram)) {
		AddToLog(&loading_log, "glCreateProgram");
	}

	if (INVALID_METHOD(this->CreateShader)) {
		AddToLog(&loading_log, "glCreateShader");
	}

	if (INVALID_METHOD(this->DeleteBuffers)) {
		AddToLog(&loading_log, "glDeleteBuffers");
	}

	if (INVALID_METHOD(this->DeleteFramebuffers)) {
		AddToLog(&loading_log, "glDeleteFramebuffers");
	}

	if (INVALID_METHOD(this->DeleteProgram)) {
		AddToLog(&loading_log, "glDeleteProgram");
	}

	if (INVALID_METHOD(this->DeleteRenderbuffers)) {
		AddToLog(&loading_log, "glDeleteRenderbuffers");
	}

	if (INVALID_METHOD(this->DeleteShader)) {
		AddToLog(&loading_log, "glDeleteShader");
	}

	if (INVALID_METHOD(this->DeleteTextures)) {
		AddToLog(&loading_log, "glDeleteTextures");
	}

	if (INVALID_METHOD(this->DeleteVertexArrays)) {
		AddToLog(&loading_log, "glDeleteVertexArrays");
	}

	if (INVALID_METHOD(this->Disable)) {
		AddToLog(&loading_log, "glDisable");
	}

	if (INVALID_METHOD(this->DisableVertexAttribArray)) {
		AddToLog(&loading_log, "glDisableVertexAttribArray");
	}

	if (INVALID_METHOD(this->DrawArraysInstanced)) {
		AddToLog(&loading_log, "glDrawArraysInstanced");
	}

	if (INVALID_METHOD(this->DrawElementsInstanced)) {
		AddToLog(&loading_log, "glDrawElementsInstanced");
	}

	if (INVALID_METHOD(this->Enable)) {
		AddToLog(&loading_log, "glEnable");
	}

	if (INVALID_METHOD(this->EnableVertexAttribArray)) {
		AddToLog(&loading_log, "glEnableVertexAttribArray");
	}

	if (INVALID_METHOD(this->EndTransformFeedback)) {
		AddToLog(&loading_log, "glEndTransformFeedback");
	}

	if (INVALID_METHOD(this->Finish)) {
		AddToLog(&loading_log, "glFinish");
	}

	if (INVALID_METHOD(this->Flush)) {
		AddToLog(&loading_log, "glFlush");
	}

	if (INVALID_METHOD(this->FramebufferRenderbuffer)) {
		AddToLog(&loading_log, "glFramebufferRenderbuffer");
	}

	if (INVALID_METHOD(this->FramebufferTexture2D)) {
		AddToLog(&loading_log, "glFramebufferTexture2D");
	}

	if (INVALID_METHOD(this->GenBuffers)) {
		AddToLog(&loading_log, "glGenBuffers");
	}

	if (INVALID_METHOD(this->GenFramebuffers)) {
		AddToLog(&loading_log, "glGenFramebuffers");
	}

	if (INVALID_METHOD(this->GenRenderbuffers)) {
		AddToLog(&loading_log, "glGenRenderbuffers");
	}

	if (INVALID_METHOD(this->GenTextures)) {
		AddToLog(&loading_log, "glGenTextures");
	}

	if (INVALID_METHOD(this->GenVertexArrays)) {
		AddToLog(&loading_log, "glGenVertexArrays");
	}

	if (INVALID_METHOD(this->GetActiveAttrib)) {
		AddToLog(&loading_log, "glGetActiveAttrib");
	}

	if (INVALID_METHOD(this->GetActiveUniform)) {
		AddToLog(&loading_log, "glGetActiveUniform");
	}

	if (INVALID_METHOD(this->GetActiveUniformBlockiv)) {
		AddToLog(&loading_log, "glGetActiveUniformBlockiv");
	}

	if (INVALID_METHOD(this->GetActiveUniformBlockName)) {
		AddToLog(&loading_log, "glGetActiveUniformBlockName");
	}

	if (INVALID_METHOD(this->GetAttribLocation)) {
		AddToLog(&loading_log, "glGetAttribLocation");
	}

	if (INVALID_METHOD(this->GetFloatv)) {
		AddToLog(&loading_log, "glGetFloatv");
	}

	if (INVALID_METHOD(this->GetIntegerv)) {
		AddToLog(&loading_log, "glGetIntegerv");
	}

	if (INVALID_METHOD(this->GetProgramInfoLog)) {
		AddToLog(&loading_log, "glGetProgramInfoLog");
	}

	if (INVALID_METHOD(this->GetProgramiv)) {
		AddToLog(&loading_log, "glGetProgramiv");
	}

	if (INVALID_METHOD(this->GetShaderInfoLog)) {
		AddToLog(&loading_log, "glGetShaderInfoLog");
	}

	if (INVALID_METHOD(this->GetShaderiv)) {
		AddToLog(&loading_log, "glGetShaderiv");
	}

	if (INVALID_METHOD(this->GetString)) {
		AddToLog(&loading_log, "glGetString");
	}

	if (INVALID_METHOD(this->GetTransformFeedbackVarying)) {
		AddToLog(&loading_log, "glGetTransformFeedbackVarying");
	}

	if (INVALID_METHOD(this->GetUniformdv)) {
		AddToLog(&loading_log, "glGetUniformdv");
	}

	if (INVALID_METHOD(this->GetUniformfv)) {
		AddToLog(&loading_log, "glGetUniformfv");
	}

	if (INVALID_METHOD(this->GetUniformiv)) {
		AddToLog(&loading_log, "glGetUniformiv");
	}

	if (INVALID_METHOD(this->GetUniformLocation)) {
		AddToLog(&loading_log, "glGetUniformLocation");
	}

	if (INVALID_METHOD(this->GetUniformuiv)) {
		AddToLog(&loading_log, "glGetUniformuiv");
	}

	if (INVALID_METHOD(this->GetVertexAttribiv)) {
		AddToLog(&loading_log, "glGetVertexAttribiv");
	}

	if (INVALID_METHOD(this->LineWidth)) {
		AddToLog(&loading_log, "glLineWidth");
	}

	if (INVALID_METHOD(this->LinkProgram)) {
		AddToLog(&loading_log, "glLinkProgram");
	}

	if (INVALID_METHOD(this->MapBufferRange)) {
		AddToLog(&loading_log, "glMapBufferRange");
	}

	if (INVALID_METHOD(this->PointSize)) {
		AddToLog(&loading_log, "glPointSize");
	}

	if (INVALID_METHOD(this->PrimitiveRestartIndex)) {
		AddToLog(&loading_log, "glPrimitiveRestartIndex");
	}

	if (INVALID_METHOD(this->ProgramUniform1dv)) {
		AddToLog(&loading_log, "glProgramUniform1dv");
	}

	if (INVALID_METHOD(this->ProgramUniform1fv)) {
		AddToLog(&loading_log, "glProgramUniform1fv");
	}

	if (INVALID_METHOD(this->ProgramUniform1iv)) {
		AddToLog(&loading_log, "glProgramUniform1iv");
	}

	if (INVALID_METHOD(this->ProgramUniform1uiv)) {
		AddToLog(&loading_log, "glProgramUniform1uiv");
	}

	if (INVALID_METHOD(this->ProgramUniform2dv)) {
		AddToLog(&loading_log, "glProgramUniform2dv");
	}

	if (INVALID_METHOD(this->ProgramUniform2fv)) {
		AddToLog(&loading_log, "glProgramUniform2fv");
	}

	if (INVALID_METHOD(this->ProgramUniform2iv)) {
		AddToLog(&loading_log, "glProgramUniform2iv");
	}

	if (INVALID_METHOD(this->ProgramUniform2uiv)) {
		AddToLog(&loading_log, "glProgramUniform2uiv");
	}

	if (INVALID_METHOD(this->ProgramUniform3dv)) {
		AddToLog(&loading_log, "glProgramUniform3dv");
	}

	if (INVALID_METHOD(this->ProgramUniform3fv)) {
		AddToLog(&loading_log, "glProgramUniform3fv");
	}

	if (INVALID_METHOD(this->ProgramUniform3iv)) {
		AddToLog(&loading_log, "glProgramUniform3iv");
	}

	if (INVALID_METHOD(this->ProgramUniform3uiv)) {
		AddToLog(&loading_log, "glProgramUniform3uiv");
	}

	if (INVALID_METHOD(this->ProgramUniform4dv)) {
		AddToLog(&loading_log, "glProgramUniform4dv");
	}

	if (INVALID_METHOD(this->ProgramUniform4fv)) {
		AddToLog(&loading_log, "glProgramUniform4fv");
	}

	if (INVALID_METHOD(this->ProgramUniform4iv)) {
		AddToLog(&loading_log, "glProgramUniform4iv");
	}

	if (INVALID_METHOD(this->ProgramUniform4uiv)) {
		AddToLog(&loading_log, "glProgramUniform4uiv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix2dv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix2dv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix2fv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix2fv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix2x3dv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix2x3dv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix2x3fv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix2x3fv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix2x4dv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix2x4dv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix2x4fv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix2x4fv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix3dv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix3dv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix3fv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix3fv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix3x2dv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix3x2dv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix3x2fv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix3x2fv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix3x4dv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix3x4dv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix3x4fv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix3x4fv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix4dv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix4dv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix4fv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix4fv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix4x2dv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix4x2dv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix4x2fv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix4x2fv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix4x3dv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix4x3dv");
	}

	if (INVALID_METHOD(this->ProgramUniformMatrix4x3fv)) {
		AddToLog(&loading_log, "glProgramUniformMatrix4x3fv");
	}

	if (INVALID_METHOD(this->ReadPixels)) {
		AddToLog(&loading_log, "glReadPixels");
	}

	if (INVALID_METHOD(this->RenderbufferStorage)) {
		AddToLog(&loading_log, "glRenderbufferStorage");
	}

	if (INVALID_METHOD(this->ShaderSource)) {
		AddToLog(&loading_log, "glShaderSource");
	}

	if (INVALID_METHOD(this->TexImage2D)) {
		AddToLog(&loading_log, "glTexImage2D");
	}

	if (INVALID_METHOD(this->TexParameteri)) {
		AddToLog(&loading_log, "glTexParameteri");
	}

	if (INVALID_METHOD(this->TexSubImage2D)) {
		AddToLog(&loading_log, "glTexSubImage2D");
	}

	if (INVALID_METHOD(this->TransformFeedbackVaryings)) {
		AddToLog(&loading_log, "glTransformFeedbackVaryings");
	}

	if (INVALID_METHOD(this->UnmapBuffer)) {
		AddToLog(&loading_log, "glUnmapBuffer");
	}

	if (INVALID_METHOD(this->UseProgram)) {
		AddToLog(&loading_log, "glUseProgram");
	}

	if (INVALID_METHOD(this->VertexAttribDivisor)) {
		AddToLog(&loading_log, "glVertexAttribDivisor");
	}

	if (INVALID_METHOD(this->VertexAttribIPointer)) {
		AddToLog(&loading_log, "glVertexAttribIPointer");
	}

	// if (INVALID_METHOD(this->VertexAttribLPointer)) {
	// 	AddToLog(&loading_log, "glVertexAttribLPointer");
	// }

	if (INVALID_METHOD(this->VertexAttribPointer)) {
		AddToLog(&loading_log, "glVertexAttribPointer");
	}

	if (INVALID_METHOD(this->Viewport)) {
		AddToLog(&loading_log, "glViewport");
	}

	if (PyUnicode_GET_SIZE(loading_log) != 0) {
		PyObject * message = PyUnicode_FromFormat("Error loading some OpenGL functions:\n");
		PyUnicode_Append(&message, loading_log);
		Py_DECREF(loading_log);

		MGLError_Set(message);
		return false;
	}

	Py_DECREF(loading_log);
	return true;
}
