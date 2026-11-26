enum {
    GL_BYTE = 0x1400,
    GL_CLAMP_TO_EDGE = 0x812F,
    GL_FRONT_AND_BACK = 0x0408,
    GL_LEQUAL = 0x0203,
    GL_LINEAR_MIPMAP_LINEAR = 0x2703,
    GL_LINEAR_MIPMAP_NEAREST = 0x2701,
    GL_MIRRORED_REPEAT = 0x8370,
    GL_NEAREST_MIPMAP_LINEAR = 0x2702,
    GL_NEAREST_MIPMAP_NEAREST = 0x2700,
    GL_NEVER = 0x0200,
    GL_REPEAT = 0x2901,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z = 0x851A,
};

enum {
    VK_ACCESS_MEMORY_READ_BIT = 0x8000,
    VK_ACCESS_MEMORY_WRITE_BIT = 0x10000,
    VK_ATTACHMENT_LOAD_OP_LOAD = 0,
    VK_ATTACHMENT_STORE_OP_STORE = 0,
    VK_BLEND_FACTOR_CONSTANT_ALPHA = 12,
    VK_BLEND_FACTOR_CONSTANT_COLOR = 10,
    VK_BLEND_FACTOR_DST_ALPHA = 8,
    VK_BLEND_FACTOR_DST_COLOR = 4,
    VK_BLEND_FACTOR_ONE = 1,
    VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA = 13,
    VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR = 11,
    VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA = 9,
    VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR = 5,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA = 7,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR = 3,
    VK_BLEND_FACTOR_SRC_ALPHA = 6,
    VK_BLEND_FACTOR_SRC_ALPHA_SATURATE = 14,
    VK_BLEND_FACTOR_SRC_COLOR = 2,
    VK_BLEND_FACTOR_ZERO = 0,
    VK_BLEND_OP_ADD = 0,
    VK_BLEND_OP_MAX = 4,
    VK_BLEND_OP_MIN = 3,
    VK_BLEND_OP_REVERSE_SUBTRACT = 2,
    VK_BLEND_OP_SUBTRACT = 1,
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT = 0x40,
    VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT = 0x100,
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT = 0x20,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT = 0x2,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT = 0x1,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT = 0x10,
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT = 0x80,
    VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT = 0x2,
    VK_CULL_MODE_BACK_BIT = 0x2,
    VK_CULL_MODE_FRONT_AND_BACK = 0x3,
    VK_CULL_MODE_FRONT_BIT = 0x1,
    VK_CULL_MODE_NONE = 0,
    VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR = 0x1,
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
    VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
    VK_DYNAMIC_STATE_SCISSOR = 1,
    VK_DYNAMIC_STATE_VIEWPORT = 0,
    VK_FILTER_LINEAR = 1,
    VK_FILTER_NEAREST = 0,
    VK_FORMAT_A2B10G10R10_UNORM_PACK32 = 64,
    VK_FORMAT_B8G8R8A8_UNORM = 44,
    VK_FORMAT_D16_UNORM = 124,
    VK_FORMAT_D24_UNORM_S8_UINT = 129,
    VK_FORMAT_D32_SFLOAT = 126,
    VK_FORMAT_D32_SFLOAT_S8_UINT = 130,
    VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT = 0x80,
    VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT = 0x100,
    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT = 0x200,
    VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT = 0x1,
    VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT = 0x2,
    VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT = 0x1000,
    VK_FORMAT_R16G16B16A16_SFLOAT = 97,
    VK_FORMAT_R16G16B16A16_SINT = 96,
    VK_FORMAT_R16G16B16A16_SNORM = 92,
    VK_FORMAT_R16G16B16A16_UINT = 95,
    VK_FORMAT_R16G16B16A16_UNORM = 91,
    VK_FORMAT_R16G16_SFLOAT = 83,
    VK_FORMAT_R16G16_SINT = 82,
    VK_FORMAT_R16G16_SNORM = 78,
    VK_FORMAT_R16G16_UINT = 81,
    VK_FORMAT_R16G16_UNORM = 77,
    VK_FORMAT_R16_SFLOAT = 76,
    VK_FORMAT_R16_SINT = 75,
    VK_FORMAT_R16_UINT = 74,
    VK_FORMAT_R32G32B32A32_SFLOAT = 109,
    VK_FORMAT_R32G32B32A32_SINT = 108,
    VK_FORMAT_R32G32B32A32_UINT = 107,
    VK_FORMAT_R32G32B32_SFLOAT = 106,
    VK_FORMAT_R32G32B32_SINT = 105,
    VK_FORMAT_R32G32B32_UINT = 104,
    VK_FORMAT_R32G32_SFLOAT = 103,
    VK_FORMAT_R32G32_SINT = 102,
    VK_FORMAT_R32G32_UINT = 101,
    VK_FORMAT_R32_SFLOAT = 100,
    VK_FORMAT_R32_SINT = 99,
    VK_FORMAT_R32_UINT = 98,
    VK_FORMAT_R8G8B8A8_SINT = 42,
    VK_FORMAT_R8G8B8A8_SNORM = 38,
    VK_FORMAT_R8G8B8A8_UINT = 41,
    VK_FORMAT_R8G8B8A8_UNORM = 37,
    VK_FORMAT_R8G8_SINT = 21,
    VK_FORMAT_R8G8_SNORM = 17,
    VK_FORMAT_R8G8_UINT = 20,
    VK_FORMAT_R8G8_UNORM = 16,
    VK_FORMAT_R8_SINT = 14,
    VK_FORMAT_R8_SNORM = 10,
    VK_FORMAT_R8_UINT = 13,
    VK_FORMAT_R8_UNORM = 9,
    VK_FORMAT_UNDEFINED = 0,
    VK_FRONT_FACE_CLOCKWISE = 1,
    VK_IMAGE_ASPECT_COLOR_BIT = 0x1,
    VK_IMAGE_ASPECT_DEPTH_BIT = 0x2,
    VK_IMAGE_ASPECT_STENCIL_BIT = 0x4,
    VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT = 0x10,
    VK_IMAGE_LAYOUT_GENERAL = 1,
    VK_IMAGE_LAYOUT_UNDEFINED = 0,
    VK_IMAGE_TYPE_2D = 1,
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT = 0x10,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT = 0x20,
    VK_IMAGE_USAGE_SAMPLED_BIT = 0x4,
    VK_IMAGE_USAGE_STORAGE_BIT = 0x8,
    VK_IMAGE_USAGE_TRANSFER_DST_BIT = 0x2,
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT = 0x1,
    VK_IMAGE_VIEW_TYPE_2D = 1,
    VK_IMAGE_VIEW_TYPE_2D_ARRAY = 5,
    VK_IMAGE_VIEW_TYPE_CUBE = 3,
    VK_INDEX_TYPE_UINT16 = 0,
    VK_INDEX_TYPE_UINT32 = 1,
    VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR = 0x1,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT = 0x1,
    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT = 0x4,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT = 0x2,
    VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU = 2,
    VK_PIPELINE_BIND_POINT_COMPUTE = 1,
    VK_PIPELINE_BIND_POINT_GRAPHICS = 0,
    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT = 0x10000,
    VK_PRIMITIVE_TOPOLOGY_LINE_LIST = 1,
    VK_PRIMITIVE_TOPOLOGY_LINE_STRIP = 2,
    VK_PRIMITIVE_TOPOLOGY_POINT_LIST = 0,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN = 5,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 4,
    VK_QUEUE_COMPUTE_BIT = 0x2,
    VK_QUEUE_GRAPHICS_BIT = 0x1,
    VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE = 2,
    VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT = 1,
    VK_SAMPLER_ADDRESS_MODE_REPEAT = 0,
    VK_SAMPLER_MIPMAP_MODE_LINEAR = 1,
    VK_SAMPLER_MIPMAP_MODE_NEAREST = 0,
    VK_SHADER_STAGE_ALL = 0x7FFFFFFF,
    VK_SHADER_STAGE_COMPUTE_BIT = 0x20,
    VK_SHADER_STAGE_FRAGMENT_BIT = 0x10,
    VK_SHADER_STAGE_VERTEX_BIT = 0x1,
    VK_STENCIL_OP_DECREMENT_AND_CLAMP = 4,
    VK_STENCIL_OP_DECREMENT_AND_WRAP = 7,
    VK_STENCIL_OP_INCREMENT_AND_CLAMP = 3,
    VK_STENCIL_OP_INCREMENT_AND_WRAP = 6,
    VK_STENCIL_OP_INVERT = 5,
    VK_STENCIL_OP_KEEP = 0,
    VK_STENCIL_OP_REPLACE = 2,
    VK_STENCIL_OP_ZERO = 1,
    VK_STRUCTURE_TYPE_APPLICATION_INFO = 0,
    VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO = 12,
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO = 40,
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO = 42,
    VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO = 39,
    VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO = 29,
    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO = 32,
    VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO = 3,
    VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO = 2,
    VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO = 28,
    VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO = 14,
    VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER = 45,
    VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO = 15,
    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO = 1,
    VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO = 5,
    VK_STRUCTURE_TYPE_MEMORY_BARRIER = 46,
    VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT = 1000355000,
    VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 = 1000059000,
    VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES = 53,
    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO = 26,
    VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO = 25,
    VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO = 27,
    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO = 20,
    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO = 30,
    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO = 24,
    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO = 23,
    VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO = 1000044002,
    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO = 18,
    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO = 19,
    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_DEPTH_CLIP_CONTROL_CREATE_INFO_EXT = 1000355001,
    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO = 22,
    VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO = 1000044001,
    VK_STRUCTURE_TYPE_RENDERING_INFO = 1000044000,
    VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO = 31,
    VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO = 16,
    VK_STRUCTURE_TYPE_SUBMIT_INFO = 4,
    VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET = 35,
    VK_VERTEX_INPUT_RATE_INSTANCE = 1,
    VK_VERTEX_INPUT_RATE_VERTEX = 0,
};

static const uint32_t VK_API_VERSION_1_3 = 0x403000;
static const uint32_t VK_QUEUE_FAMILY_IGNORED = 0xFFFFFFFF;
static const uint64_t VK_WHOLE_SIZE = 0xFFFFFFFFFFFFFFFF;

typedef struct VkApplicationInfo {
    uint32_t sType;
    const void * pNext;
    const char * pApplicationName;
    uint32_t applicationVersion;
    const char * pEngineName;
    uint32_t engineVersion;
    uint32_t apiVersion;
} VkApplicationInfo;

typedef struct VkInstanceCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    const VkApplicationInfo * pApplicationInfo;
    uint32_t enabledLayerCount;
    const char * const * ppEnabledLayerNames;
    uint32_t enabledExtensionCount;
    const char * const * ppEnabledExtensionNames;
} VkInstanceCreateInfo;

typedef struct VkExtent3D {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
} VkExtent3D;

typedef struct VkQueueFamilyProperties {
    uint32_t queueFlags;
    uint32_t queueCount;
    uint32_t timestampValidBits;
    VkExtent3D minImageTransferGranularity;
} VkQueueFamilyProperties;

typedef struct VkMemoryType {
    uint32_t propertyFlags;
    uint32_t heapIndex;
} VkMemoryType;

typedef struct VkMemoryHeap {
    uint64_t size;
    uint32_t flags;
} VkMemoryHeap;

typedef struct VkPhysicalDeviceMemoryProperties {
    uint32_t memoryTypeCount;
    VkMemoryType memoryTypes[32];
    uint32_t memoryHeapCount;
    VkMemoryHeap memoryHeaps[16];
} VkPhysicalDeviceMemoryProperties;

typedef struct VkPhysicalDeviceFeatures2 {
    uint32_t sType;
    void * pNext;
    uint32_t features[55];
} VkPhysicalDeviceFeatures2;

typedef struct VkPhysicalDeviceVulkan13Features {
    uint32_t sType;
    void * pNext;
    uint32_t robustImageAccess;
    uint32_t inlineUniformBlock;
    uint32_t descriptorBindingInlineUniformBlockUpdateAfterBind;
    uint32_t pipelineCreationCacheControl;
    uint32_t privateData;
    uint32_t shaderDemoteToHelperInvocation;
    uint32_t shaderTerminateInvocation;
    uint32_t subgroupSizeControl;
    uint32_t computeFullSubgroups;
    uint32_t synchronization2;
    uint32_t textureCompressionASTC_HDR;
    uint32_t shaderZeroInitializeWorkgroupMemory;
    uint32_t dynamicRendering;
    uint32_t shaderIntegerDotProduct;
    uint32_t maintenance4;
} VkPhysicalDeviceVulkan13Features;

typedef struct VkPhysicalDeviceDepthClipControlFeaturesEXT {
    uint32_t sType;
    void * pNext;
    uint32_t depthClipControl;
} VkPhysicalDeviceDepthClipControlFeaturesEXT;

typedef struct VkExtensionProperties {
    char extensionName[256];
    uint32_t specVersion;
} VkExtensionProperties;

typedef struct VkDeviceQueueCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t queueFamilyIndex;
    uint32_t queueCount;
    const float * pQueuePriorities;
} VkDeviceQueueCreateInfo;

typedef struct VkDeviceCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t queueCreateInfoCount;
    const VkDeviceQueueCreateInfo * pQueueCreateInfos;
    uint32_t enabledLayerCount;
    const char * const * ppEnabledLayerNames;
    uint32_t enabledExtensionCount;
    const char * const * ppEnabledExtensionNames;
    const void * pEnabledFeatures;
} VkDeviceCreateInfo;

typedef struct VkCommandPoolCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t queueFamilyIndex;
} VkCommandPoolCreateInfo;

typedef struct VkCommandBufferAllocateInfo {
    uint32_t sType;
    const void * pNext;
    uint64_t commandPool;
    uint32_t level;
    uint32_t commandBufferCount;
} VkCommandBufferAllocateInfo;

typedef struct VkCommandBufferBeginInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    const void * pInheritanceInfo;
} VkCommandBufferBeginInfo;

typedef struct VkSubmitInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t waitSemaphoreCount;
    const uint64_t * pWaitSemaphores;
    const uint32_t * pWaitDstStageMask;
    uint32_t commandBufferCount;
    void * const * pCommandBuffers;
    uint32_t signalSemaphoreCount;
    const uint64_t * pSignalSemaphores;
} VkSubmitInfo;

typedef struct VkBufferCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint64_t size;
    uint32_t usage;
    uint32_t sharingMode;
    uint32_t queueFamilyIndexCount;
    const uint32_t * pQueueFamilyIndices;
} VkBufferCreateInfo;

typedef struct VkMemoryRequirements {
    uint64_t size;
    uint64_t alignment;
    uint32_t memoryTypeBits;
} VkMemoryRequirements;

typedef struct VkMemoryAllocateInfo {
    uint32_t sType;
    const void * pNext;
    uint64_t allocationSize;
    uint32_t memoryTypeIndex;
} VkMemoryAllocateInfo;

typedef struct VkImageCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t imageType;
    uint32_t format;
    VkExtent3D extent;
    uint32_t mipLevels;
    uint32_t arrayLayers;
    uint32_t samples;
    uint32_t tiling;
    uint32_t usage;
    uint32_t sharingMode;
    uint32_t queueFamilyIndexCount;
    const uint32_t * pQueueFamilyIndices;
    uint32_t initialLayout;
} VkImageCreateInfo;

typedef struct VkComponentMapping {
    uint32_t r;
    uint32_t g;
    uint32_t b;
    uint32_t a;
} VkComponentMapping;

typedef struct VkImageSubresourceRange {
    uint32_t aspectMask;
    uint32_t baseMipLevel;
    uint32_t levelCount;
    uint32_t baseArrayLayer;
    uint32_t layerCount;
} VkImageSubresourceRange;

typedef struct VkImageViewCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint64_t image;
    uint32_t viewType;
    uint32_t format;
    VkComponentMapping components;
    VkImageSubresourceRange subresourceRange;
} VkImageViewCreateInfo;

typedef struct VkSamplerCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t magFilter;
    uint32_t minFilter;
    uint32_t mipmapMode;
    uint32_t addressModeU;
    uint32_t addressModeV;
    uint32_t addressModeW;
    float mipLodBias;
    uint32_t anisotropyEnable;
    float maxAnisotropy;
    uint32_t compareEnable;
    uint32_t compareOp;
    float minLod;
    float maxLod;
    uint32_t borderColor;
    uint32_t unnormalizedCoordinates;
} VkSamplerCreateInfo;

typedef struct VkShaderModuleCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    size_t codeSize;
    const uint32_t * pCode;
} VkShaderModuleCreateInfo;

typedef struct VkDescriptorSetLayoutBinding {
    uint32_t binding;
    uint32_t descriptorType;
    uint32_t descriptorCount;
    uint32_t stageFlags;
    const uint64_t * pImmutableSamplers;
} VkDescriptorSetLayoutBinding;

typedef struct VkDescriptorSetLayoutCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t bindingCount;
    const VkDescriptorSetLayoutBinding * pBindings;
} VkDescriptorSetLayoutCreateInfo;

typedef struct VkPipelineLayoutCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t setLayoutCount;
    const uint64_t * pSetLayouts;
    uint32_t pushConstantRangeCount;
    const void * pPushConstantRanges;
} VkPipelineLayoutCreateInfo;

typedef struct VkPipelineShaderStageCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t stage;
    uint64_t module;
    const char * pName;
    const void * pSpecializationInfo;
} VkPipelineShaderStageCreateInfo;

typedef struct VkVertexInputBindingDescription {
    uint32_t binding;
    uint32_t stride;
    uint32_t inputRate;
} VkVertexInputBindingDescription;

typedef struct VkVertexInputAttributeDescription {
    uint32_t location;
    uint32_t binding;
    uint32_t format;
    uint32_t offset;
} VkVertexInputAttributeDescription;

typedef struct VkPipelineVertexInputStateCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t vertexBindingDescriptionCount;
    const VkVertexInputBindingDescription * pVertexBindingDescriptions;
    uint32_t vertexAttributeDescriptionCount;
    const VkVertexInputAttributeDescription * pVertexAttributeDescriptions;
} VkPipelineVertexInputStateCreateInfo;

typedef struct VkPipelineInputAssemblyStateCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t topology;
    uint32_t primitiveRestartEnable;
} VkPipelineInputAssemblyStateCreateInfo;

typedef struct VkViewport {
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;
} VkViewport;

typedef struct VkOffset2D {
    int32_t x;
    int32_t y;
} VkOffset2D;

typedef struct VkExtent2D {
    uint32_t width;
    uint32_t height;
} VkExtent2D;

typedef struct VkRect2D {
    VkOffset2D offset;
    VkExtent2D extent;
} VkRect2D;

typedef struct VkPipelineViewportStateCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t viewportCount;
    const VkViewport * pViewports;
    uint32_t scissorCount;
    const VkRect2D * pScissors;
} VkPipelineViewportStateCreateInfo;

typedef struct VkPipelineViewportDepthClipControlCreateInfoEXT {
    uint32_t sType;
    const void * pNext;
    uint32_t negativeOneToOne;
} VkPipelineViewportDepthClipControlCreateInfoEXT;

typedef struct VkPipelineRasterizationStateCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t depthClampEnable;
    uint32_t rasterizerDiscardEnable;
    uint32_t polygonMode;
    uint32_t cullMode;
    uint32_t frontFace;
    uint32_t depthBiasEnable;
    float depthBiasConstantFactor;
    float depthBiasClamp;
    float depthBiasSlopeFactor;
    float lineWidth;
} VkPipelineRasterizationStateCreateInfo;

typedef struct VkPipelineMultisampleStateCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t rasterizationSamples;
    uint32_t sampleShadingEnable;
    float minSampleShading;
    const uint32_t * pSampleMask;
    uint32_t alphaToCoverageEnable;
    uint32_t alphaToOneEnable;
} VkPipelineMultisampleStateCreateInfo;

typedef struct VkStencilOpState {
    uint32_t failOp;
    uint32_t passOp;
    uint32_t depthFailOp;
    uint32_t compareOp;
    uint32_t compareMask;
    uint32_t writeMask;
    uint32_t reference;
} VkStencilOpState;

typedef struct VkPipelineDepthStencilStateCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t depthTestEnable;
    uint32_t depthWriteEnable;
    uint32_t depthCompareOp;
    uint32_t depthBoundsTestEnable;
    uint32_t stencilTestEnable;
    VkStencilOpState front;
    VkStencilOpState back;
    float minDepthBounds;
    float maxDepthBounds;
} VkPipelineDepthStencilStateCreateInfo;

typedef struct VkPipelineColorBlendAttachmentState {
    uint32_t blendEnable;
    uint32_t srcColorBlendFactor;
    uint32_t dstColorBlendFactor;
    uint32_t colorBlendOp;
    uint32_t srcAlphaBlendFactor;
    uint32_t dstAlphaBlendFactor;
    uint32_t alphaBlendOp;
    uint32_t colorWriteMask;
} VkPipelineColorBlendAttachmentState;

typedef struct VkPipelineColorBlendStateCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t logicOpEnable;
    uint32_t logicOp;
    uint32_t attachmentCount;
    const VkPipelineColorBlendAttachmentState * pAttachments;
    float blendConstants[4];
} VkPipelineColorBlendStateCreateInfo;

typedef struct VkPipelineDynamicStateCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t dynamicStateCount;
    const uint32_t * pDynamicStates;
} VkPipelineDynamicStateCreateInfo;

typedef struct VkPipelineRenderingCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t viewMask;
    uint32_t colorAttachmentCount;
    const uint32_t * pColorAttachmentFormats;
    uint32_t depthAttachmentFormat;
    uint32_t stencilAttachmentFormat;
} VkPipelineRenderingCreateInfo;

typedef struct VkGraphicsPipelineCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    uint32_t stageCount;
    const VkPipelineShaderStageCreateInfo * pStages;
    const VkPipelineVertexInputStateCreateInfo * pVertexInputState;
    const VkPipelineInputAssemblyStateCreateInfo * pInputAssemblyState;
    const void * pTessellationState;
    const VkPipelineViewportStateCreateInfo * pViewportState;
    const VkPipelineRasterizationStateCreateInfo * pRasterizationState;
    const VkPipelineMultisampleStateCreateInfo * pMultisampleState;
    const VkPipelineDepthStencilStateCreateInfo * pDepthStencilState;
    const VkPipelineColorBlendStateCreateInfo * pColorBlendState;
    const VkPipelineDynamicStateCreateInfo * pDynamicState;
    uint64_t layout;
    uint64_t renderPass;
    uint32_t subpass;
    uint64_t basePipelineHandle;
    int32_t basePipelineIndex;
} VkGraphicsPipelineCreateInfo;

typedef struct VkComputePipelineCreateInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    VkPipelineShaderStageCreateInfo stage;
    uint64_t layout;
    uint64_t basePipelineHandle;
    int32_t basePipelineIndex;
} VkComputePipelineCreateInfo;

typedef struct VkMemoryBarrier {
    uint32_t sType;
    const void * pNext;
    uint32_t srcAccessMask;
    uint32_t dstAccessMask;
} VkMemoryBarrier;

typedef struct VkImageMemoryBarrier {
    uint32_t sType;
    const void * pNext;
    uint32_t srcAccessMask;
    uint32_t dstAccessMask;
    uint32_t oldLayout;
    uint32_t newLayout;
    uint32_t srcQueueFamilyIndex;
    uint32_t dstQueueFamilyIndex;
    uint64_t image;
    VkImageSubresourceRange subresourceRange;
} VkImageMemoryBarrier;

typedef struct VkBufferCopy {
    uint64_t srcOffset;
    uint64_t dstOffset;
    uint64_t size;
} VkBufferCopy;

typedef struct VkImageSubresourceLayers {
    uint32_t aspectMask;
    uint32_t mipLevel;
    uint32_t baseArrayLayer;
    uint32_t layerCount;
} VkImageSubresourceLayers;

typedef struct VkOffset3D {
    int32_t x;
    int32_t y;
    int32_t z;
} VkOffset3D;

typedef struct VkBufferImageCopy {
    uint64_t bufferOffset;
    uint32_t bufferRowLength;
    uint32_t bufferImageHeight;
    VkImageSubresourceLayers imageSubresource;
    VkOffset3D imageOffset;
    VkExtent3D imageExtent;
} VkBufferImageCopy;

typedef struct VkImageBlit {
    VkImageSubresourceLayers srcSubresource;
    VkOffset3D srcOffsets[2];
    VkImageSubresourceLayers dstSubresource;
    VkOffset3D dstOffsets[2];
} VkImageBlit;

typedef struct VkImageResolve {
    VkImageSubresourceLayers srcSubresource;
    VkOffset3D srcOffset;
    VkImageSubresourceLayers dstSubresource;
    VkOffset3D dstOffset;
    VkExtent3D extent;
} VkImageResolve;

typedef union VkClearColorValue {
    float float32[4];
    int32_t int32[4];
    uint32_t uint32[4];
} VkClearColorValue;

typedef struct VkClearDepthStencilValue {
    float depth;
    uint32_t stencil;
} VkClearDepthStencilValue;

typedef union VkClearValue {
    VkClearColorValue color;
    VkClearDepthStencilValue depthStencil;
} VkClearValue;

typedef struct VkRenderingAttachmentInfo {
    uint32_t sType;
    const void * pNext;
    uint64_t imageView;
    uint32_t imageLayout;
    uint32_t resolveMode;
    uint64_t resolveImageView;
    uint32_t resolveImageLayout;
    uint32_t loadOp;
    uint32_t storeOp;
    VkClearValue clearValue;
} VkRenderingAttachmentInfo;

typedef struct VkRenderingInfo {
    uint32_t sType;
    const void * pNext;
    uint32_t flags;
    VkRect2D renderArea;
    uint32_t layerCount;
    uint32_t viewMask;
    uint32_t colorAttachmentCount;
    const VkRenderingAttachmentInfo * pColorAttachments;
    const VkRenderingAttachmentInfo * pDepthAttachment;
    const VkRenderingAttachmentInfo * pStencilAttachment;
} VkRenderingInfo;

typedef struct VkDescriptorBufferInfo {
    uint64_t buffer;
    uint64_t offset;
    uint64_t range;
} VkDescriptorBufferInfo;

typedef struct VkDescriptorImageInfo {
    uint64_t sampler;
    uint64_t imageView;
    uint32_t imageLayout;
} VkDescriptorImageInfo;

typedef struct VkWriteDescriptorSet {
    uint32_t sType;
    const void * pNext;
    uint64_t dstSet;
    uint32_t dstBinding;
    uint32_t dstArrayElement;
    uint32_t descriptorCount;
    uint32_t descriptorType;
    const VkDescriptorImageInfo * pImageInfo;
    const VkDescriptorBufferInfo * pBufferInfo;
    const uint64_t * pTexelBufferView;
} VkWriteDescriptorSet;

typedef struct VkFormatProperties {
    uint32_t linearTilingFeatures;
    uint32_t optimalTilingFeatures;
    uint32_t bufferFeatures;
} VkFormatProperties;

typedef struct VkPhysicalDeviceProperties {
    uint32_t apiVersion;
    uint32_t driverVersion;
    uint32_t vendorID;
    uint32_t deviceID;
    uint32_t deviceType;
    char deviceName[256];
    uint8_t pipelineCacheUUID[16];
    uint64_t limitsAndSparseProperties[66];
} VkPhysicalDeviceProperties;

static void * (GLAPI * vkGetInstanceProcAddr)(void * instance, const char * name);
static void * (GLAPI * vkGetDeviceProcAddr)(void * device, const char * name);
static int32_t (GLAPI * vkCreateInstance)(const VkInstanceCreateInfo * info, const void * allocator, void ** instance);
static int32_t (GLAPI * vkEnumeratePhysicalDevices)(void * instance, uint32_t * count, void ** devices);
static void (GLAPI * vkGetPhysicalDeviceProperties)(void * physical, VkPhysicalDeviceProperties * properties);
static void (GLAPI * vkGetPhysicalDeviceQueueFamilyProperties)(void * physical, uint32_t * count, VkQueueFamilyProperties * properties);
static void (GLAPI * vkGetPhysicalDeviceMemoryProperties)(void * physical, VkPhysicalDeviceMemoryProperties * properties);
static void (GLAPI * vkGetPhysicalDeviceFeatures2)(void * physical, VkPhysicalDeviceFeatures2 * features);
static void (GLAPI * vkGetPhysicalDeviceFormatProperties)(void * physical, uint32_t format, VkFormatProperties * properties);
static int32_t (GLAPI * vkEnumerateDeviceExtensionProperties)(void * physical, const char * layer, uint32_t * count, VkExtensionProperties * properties);
static int32_t (GLAPI * vkCreateDevice)(void * physical, const VkDeviceCreateInfo * info, const void * allocator, void ** device);
static void (GLAPI * vkGetDeviceQueue)(void * device, uint32_t family, uint32_t index, void ** queue);
static int32_t (GLAPI * vkCreateCommandPool)(void * device, const VkCommandPoolCreateInfo * info, const void * allocator, uint64_t * pool);
static int32_t (GLAPI * vkAllocateCommandBuffers)(void * device, const VkCommandBufferAllocateInfo * info, void ** commands);
static int32_t (GLAPI * vkBeginCommandBuffer)(void * command, const VkCommandBufferBeginInfo * info);
static int32_t (GLAPI * vkEndCommandBuffer)(void * command);
static int32_t (GLAPI * vkResetCommandBuffer)(void * command, uint32_t flags);
static int32_t (GLAPI * vkQueueSubmit)(void * queue, uint32_t count, const VkSubmitInfo * submits, uint64_t fence);
static int32_t (GLAPI * vkQueueWaitIdle)(void * queue);
static int32_t (GLAPI * vkCreateBuffer)(void * device, const VkBufferCreateInfo * info, const void * allocator, uint64_t * buffer);
static void (GLAPI * vkDestroyBuffer)(void * device, uint64_t buffer, const void * allocator);
static void (GLAPI * vkGetBufferMemoryRequirements)(void * device, uint64_t buffer, VkMemoryRequirements * requirements);
static int32_t (GLAPI * vkAllocateMemory)(void * device, const VkMemoryAllocateInfo * info, const void * allocator, uint64_t * memory);
static void (GLAPI * vkFreeMemory)(void * device, uint64_t memory, const void * allocator);
static int32_t (GLAPI * vkBindBufferMemory)(void * device, uint64_t buffer, uint64_t memory, uint64_t offset);
static int32_t (GLAPI * vkMapMemory)(void * device, uint64_t memory, uint64_t offset, uint64_t size, uint32_t flags, void ** data);
static int32_t (GLAPI * vkCreateImage)(void * device, const VkImageCreateInfo * info, const void * allocator, uint64_t * image);
static void (GLAPI * vkDestroyImage)(void * device, uint64_t image, const void * allocator);
static void (GLAPI * vkGetImageMemoryRequirements)(void * device, uint64_t image, VkMemoryRequirements * requirements);
static int32_t (GLAPI * vkBindImageMemory)(void * device, uint64_t image, uint64_t memory, uint64_t offset);
static int32_t (GLAPI * vkCreateImageView)(void * device, const VkImageViewCreateInfo * info, const void * allocator, uint64_t * view);
static void (GLAPI * vkDestroyImageView)(void * device, uint64_t view, const void * allocator);
static int32_t (GLAPI * vkCreateSampler)(void * device, const VkSamplerCreateInfo * info, const void * allocator, uint64_t * sampler);
static void (GLAPI * vkDestroySampler)(void * device, uint64_t sampler, const void * allocator);
static int32_t (GLAPI * vkCreateShaderModule)(void * device, const VkShaderModuleCreateInfo * info, const void * allocator, uint64_t * module);
static void (GLAPI * vkDestroyShaderModule)(void * device, uint64_t module, const void * allocator);
static int32_t (GLAPI * vkCreateDescriptorSetLayout)(void * device, const VkDescriptorSetLayoutCreateInfo * info, const void * allocator, uint64_t * layout);
static int32_t (GLAPI * vkCreatePipelineLayout)(void * device, const VkPipelineLayoutCreateInfo * info, const void * allocator, uint64_t * layout);
static int32_t (GLAPI * vkCreateGraphicsPipelines)(void * device, uint64_t cache, uint32_t count, const VkGraphicsPipelineCreateInfo * infos, const void * allocator, uint64_t * pipelines);
static int32_t (GLAPI * vkCreateComputePipelines)(void * device, uint64_t cache, uint32_t count, const VkComputePipelineCreateInfo * infos, const void * allocator, uint64_t * pipelines);
static void (GLAPI * vkDestroyPipeline)(void * device, uint64_t pipeline, const void * allocator);
static void (GLAPI * vkCmdPipelineBarrier)(void * command, uint32_t src_stages, uint32_t dst_stages, uint32_t flags, uint32_t memory_count, const VkMemoryBarrier * memory, uint32_t buffer_count, const void * buffers, uint32_t image_count, const VkImageMemoryBarrier * images);
static void (GLAPI * vkCmdCopyBuffer)(void * command, uint64_t src, uint64_t dst, uint32_t count, const VkBufferCopy * regions);
static void (GLAPI * vkCmdCopyBufferToImage)(void * command, uint64_t src, uint64_t dst, uint32_t layout, uint32_t count, const VkBufferImageCopy * regions);
static void (GLAPI * vkCmdCopyImageToBuffer)(void * command, uint64_t src, uint32_t layout, uint64_t dst, uint32_t count, const VkBufferImageCopy * regions);
static void (GLAPI * vkCmdBlitImage)(void * command, uint64_t src, uint32_t src_layout, uint64_t dst, uint32_t dst_layout, uint32_t count, const VkImageBlit * regions, uint32_t filter);
static void (GLAPI * vkCmdResolveImage)(void * command, uint64_t src, uint32_t src_layout, uint64_t dst, uint32_t dst_layout, uint32_t count, const VkImageResolve * regions);
static void (GLAPI * vkCmdClearColorImage)(void * command, uint64_t image, uint32_t layout, const VkClearColorValue * color, uint32_t count, const VkImageSubresourceRange * ranges);
static void (GLAPI * vkCmdClearDepthStencilImage)(void * command, uint64_t image, uint32_t layout, const VkClearDepthStencilValue * value, uint32_t count, const VkImageSubresourceRange * ranges);
static void (GLAPI * vkCmdBeginRendering)(void * command, const VkRenderingInfo * info);
static void (GLAPI * vkCmdEndRendering)(void * command);
static void (GLAPI * vkCmdBindPipeline)(void * command, uint32_t bind_point, uint64_t pipeline);
static void (GLAPI * vkCmdSetViewport)(void * command, uint32_t first, uint32_t count, const VkViewport * viewports);
static void (GLAPI * vkCmdSetScissor)(void * command, uint32_t first, uint32_t count, const VkRect2D * scissors);
static void (GLAPI * vkCmdBindVertexBuffers)(void * command, uint32_t first, uint32_t count, const uint64_t * buffers, const uint64_t * offsets);
static void (GLAPI * vkCmdBindIndexBuffer)(void * command, uint64_t buffer, uint64_t offset, uint32_t type);
static void (GLAPI * vkCmdDraw)(void * command, uint32_t vertices, uint32_t instances, uint32_t first_vertex, uint32_t first_instance);
static void (GLAPI * vkCmdDrawIndexed)(void * command, uint32_t indices, uint32_t instances, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance);
static void (GLAPI * vkCmdDrawIndirect)(void * command, uint64_t buffer, uint64_t offset, uint32_t count, uint32_t stride);
static void (GLAPI * vkCmdDrawIndexedIndirect)(void * command, uint64_t buffer, uint64_t offset, uint32_t count, uint32_t stride);
static void (GLAPI * vkCmdDispatch)(void * command, uint32_t x, uint32_t y, uint32_t z);
static void (GLAPI * vkCmdPushDescriptorSetKHR)(void * command, uint32_t bind_point, uint64_t layout, uint32_t set, uint32_t count, const VkWriteDescriptorSet * writes);

typedef struct VkglTable {
    char * items;
    uint32_t count;
    uint32_t capacity;
} VkglTable;

typedef struct VkglBuffer {
    uint64_t buffer;
    uint64_t memory;
    char * ptr;
    int64_t size;
} VkglBuffer;

typedef struct VkglImage {
    uint64_t image;
    uint64_t memory;
    uint64_t view;
    uint32_t target;
    uint32_t format;
    uint32_t aspect;
    uint32_t features;
    int32_t width;
    int32_t height;
    int32_t layers;
    int32_t levels;
    int32_t samples;
    int32_t pixel_size;
    int32_t max_level;
} VkglImage;

typedef struct VkglSampler {
    uint64_t sampler;
    uint32_t min_filter;
    uint32_t mag_filter;
    uint32_t wrap[3];
    uint32_t compare_mode;
    uint32_t compare_func;
    float min_lod;
    float max_lod;
    float lod_bias;
    float anisotropy;
} VkglSampler;

typedef struct VkglShader {
    uint32_t type;
    uint32_t * code;
    int64_t size;
    int32_t valid;
} VkglShader;

typedef struct VkglProgram {
    uint32_t shaders[2];
    uint64_t vertex;
    uint64_t fragment;
    uint64_t compute;
    uint64_t compute_layouts[8];
    uint64_t compute_pipelines[8];
    int32_t compute_count;
    int32_t linked;
} VkglProgram;

typedef struct VkglAttrib {
    uint32_t buffer;
    uint32_t format;
    uint32_t stride;
    uint32_t rate;
    uint64_t offset;
    int32_t enabled;
} VkglAttrib;

typedef struct VkglVertexArray {
    VkglAttrib attribs[16];
    uint32_t index_buffer;
} VkglVertexArray;

typedef struct VkglAttachment {
    uint32_t image;
    int32_t level;
    int32_t layer;
    uint64_t view;
} VkglAttachment;

typedef struct VkglFramebuffer {
    VkglAttachment color[8];
    VkglAttachment depth;
    int32_t color_count;
    int32_t read;
} VkglFramebuffer;

typedef struct VkglRange {
    uint32_t buffer;
    uint64_t offset;
    uint64_t size;
} VkglRange;

typedef struct VkglState {
    uint32_t depth_test;
    uint32_t depth_write;
    uint32_t depth_func;
    uint32_t cull;
    uint32_t cull_face;
    uint32_t blend;
    uint32_t blend_func[4];
    uint32_t blend_equation[2];
    uint32_t stencil;
    uint32_t stencil_func[2];
    uint32_t stencil_ref[2];
    uint32_t stencil_read_mask[2];
    uint32_t stencil_fail[2];
    uint32_t stencil_depth_fail[2];
    uint32_t stencil_depth_pass[2];
    uint32_t stencil_write_mask;
    uint32_t polygon_offset;
    float polygon_offset_factor;
    float polygon_offset_units;
} VkglState;

typedef struct VkglPipelineKey {
    uint32_t program;
    uint32_t topology;
    uint32_t samples;
    uint32_t color_count;
    uint32_t color_formats[8];
    uint32_t depth_format;
    uint32_t depth_aspect;
    uint32_t layout[2];
    uint32_t attrib_formats[16];
    uint32_t attrib_strides[16];
    uint32_t attrib_rates[16];
    VkglState state;
} VkglPipelineKey;

typedef struct VkglStorageImage {
    uint32_t image;
    int32_t level;
    int32_t layer;
    int32_t layered;
    uint64_t view;
} VkglStorageImage;

typedef struct VkglLayout {
    uint64_t mask;
    uint64_t set_layout;
    uint64_t pipeline_layout;
} VkglLayout;

typedef struct VkglPipeline {
    VkglPipelineKey key;
    uint64_t pipeline;
} VkglPipeline;

static struct {
    void * instance;
    void * physical;
    void * device;
    void * queue;
    void * command;
    uint32_t family;
    uint64_t pool;
    VkglLayout layouts[64];
    int32_t layout_count;
    uint64_t * garbage;
    int32_t garbage_count;
    uint64_t default_sampler;
    VkPhysicalDeviceMemoryProperties memory;
    char renderer[256];
    int32_t depth_clip_control;
    uint32_t depth_stencil_format;
    int32_t dirty;
    int64_t submits;
    VkglBuffer staging;
    int64_t staging_offset;
    VkglTable buffers;
    VkglTable images;
    VkglTable samplers;
    VkglTable shaders;
    VkglTable programs;
    VkglTable vertex_arrays;
    VkglTable framebuffers;
    VkglPipeline * pipelines;
    int32_t pipeline_count;
    uint64_t current_pipeline;
    VkglState state;
    uint32_t array_buffer;
    uint32_t copy_read_buffer;
    uint32_t copy_write_buffer;
    uint32_t pixel_pack_buffer;
    uint32_t pixel_unpack_buffer;
    uint32_t uniform_buffer;
    uint32_t storage_buffer;
    uint32_t indirect_buffer;
    VkglRange uniform_buffers[8];
    VkglRange storage_buffers[8];
    VkglStorageImage storage_images[8];
    uint32_t active_unit;
    uint32_t textures[32];
    uint32_t unit_samplers[32];
    uint32_t program;
    uint32_t vertex_array;
    uint32_t draw_framebuffer;
    uint32_t read_framebuffer;
    uint32_t renderbuffer;
    uint32_t rendering;
    uint32_t rendering_size[2];
    int32_t viewport[4];
    int32_t scissor_test;
    int32_t scissor[4];
} vkgl;

#define VKGL_BUFFER(name) (&((VkglBuffer *)vkgl.buffers.items)[name])
#define VKGL_IMAGE(name) (&((VkglImage *)vkgl.images.items)[name])
#define VKGL_SAMPLER(name) (&((VkglSampler *)vkgl.samplers.items)[name])
#define VKGL_SHADER(name) (&((VkglShader *)vkgl.shaders.items)[name])
#define VKGL_PROGRAM(name) (&((VkglProgram *)vkgl.programs.items)[name])
#define VKGL_VERTEX_ARRAY(name) (&((VkglVertexArray *)vkgl.vertex_arrays.items)[name])
#define VKGL_FRAMEBUFFER(name) (&((VkglFramebuffer *)vkgl.framebuffers.items)[name])

static const uint32_t vkgl_image_formats[][3] = {
    {0x8229, VK_FORMAT_R8_UNORM, 1},
    {0x822B, VK_FORMAT_R8G8_UNORM, 2},
    {0x8058, VK_FORMAT_R8G8B8A8_UNORM, 4},
    {0x8F94, VK_FORMAT_R8_SNORM, 1},
    {0x8F95, VK_FORMAT_R8G8_SNORM, 2},
    {0x8F97, VK_FORMAT_R8G8B8A8_SNORM, 4},
    {0x8232, VK_FORMAT_R8_UINT, 1},
    {0x8238, VK_FORMAT_R8G8_UINT, 2},
    {0x8D7C, VK_FORMAT_R8G8B8A8_UINT, 4},
    {0x8234, VK_FORMAT_R16_UINT, 2},
    {0x823A, VK_FORMAT_R16G16_UINT, 4},
    {0x8D76, VK_FORMAT_R16G16B16A16_UINT, 8},
    {0x8236, VK_FORMAT_R32_UINT, 4},
    {0x823C, VK_FORMAT_R32G32_UINT, 8},
    {0x8D70, VK_FORMAT_R32G32B32A32_UINT, 16},
    {0x8231, VK_FORMAT_R8_SINT, 1},
    {0x8237, VK_FORMAT_R8G8_SINT, 2},
    {0x8D8E, VK_FORMAT_R8G8B8A8_SINT, 4},
    {0x8233, VK_FORMAT_R16_SINT, 2},
    {0x8239, VK_FORMAT_R16G16_SINT, 4},
    {0x8D88, VK_FORMAT_R16G16B16A16_SINT, 8},
    {0x8235, VK_FORMAT_R32_SINT, 4},
    {0x823B, VK_FORMAT_R32G32_SINT, 8},
    {0x8D82, VK_FORMAT_R32G32B32A32_SINT, 16},
    {0x822D, VK_FORMAT_R16_SFLOAT, 2},
    {0x822F, VK_FORMAT_R16G16_SFLOAT, 4},
    {0x881A, VK_FORMAT_R16G16B16A16_SFLOAT, 8},
    {0x822E, VK_FORMAT_R32_SFLOAT, 4},
    {0x8230, VK_FORMAT_R32G32_SFLOAT, 8},
    {0x8814, VK_FORMAT_R32G32B32A32_SFLOAT, 16},
    {0x8059, VK_FORMAT_A2B10G10R10_UNORM_PACK32, 4},
    {0x81A5, VK_FORMAT_D16_UNORM, 2},
    {0x81A6, VK_FORMAT_D32_SFLOAT, 4},
    {0x88F0, VK_FORMAT_D24_UNORM_S8_UINT, 4},
    {0x8CAC, VK_FORMAT_D32_SFLOAT, 4},
    {0},
};

static const uint32_t vkgl_vertex_formats[][4] = {
    {0x1401, 2, 0, VK_FORMAT_R8G8_UNORM},
    {0x1401, 4, 0, VK_FORMAT_R8G8B8A8_UNORM},
    {0x1401, 2, 1, VK_FORMAT_R8G8_UINT},
    {0x1401, 4, 1, VK_FORMAT_R8G8B8A8_UINT},
    {0x1400, 2, 0, VK_FORMAT_R8G8_SNORM},
    {0x1400, 4, 0, VK_FORMAT_R8G8B8A8_SNORM},
    {0x1400, 2, 1, VK_FORMAT_R8G8_SINT},
    {0x1400, 4, 1, VK_FORMAT_R8G8B8A8_SINT},
    {0x1403, 2, 0, VK_FORMAT_R16G16_UNORM},
    {0x1403, 4, 0, VK_FORMAT_R16G16B16A16_UNORM},
    {0x1403, 2, 1, VK_FORMAT_R16G16_UINT},
    {0x1403, 4, 1, VK_FORMAT_R16G16B16A16_UINT},
    {0x1402, 2, 0, VK_FORMAT_R16G16_SNORM},
    {0x1402, 4, 0, VK_FORMAT_R16G16B16A16_SNORM},
    {0x1402, 2, 1, VK_FORMAT_R16G16_SINT},
    {0x1402, 4, 1, VK_FORMAT_R16G16B16A16_SINT},
    {0x140B, 2, 0, VK_FORMAT_R16G16_SFLOAT},
    {0x140B, 4, 0, VK_FORMAT_R16G16B16A16_SFLOAT},
    {0x1406, 1, 0, VK_FORMAT_R32_SFLOAT},
    {0x1406, 2, 0, VK_FORMAT_R32G32_SFLOAT},
    {0x1406, 3, 0, VK_FORMAT_R32G32B32_SFLOAT},
    {0x1406, 4, 0, VK_FORMAT_R32G32B32A32_SFLOAT},
    {0x1405, 1, 1, VK_FORMAT_R32_UINT},
    {0x1405, 2, 1, VK_FORMAT_R32G32_UINT},
    {0x1405, 3, 1, VK_FORMAT_R32G32B32_UINT},
    {0x1405, 4, 1, VK_FORMAT_R32G32B32A32_UINT},
    {0x1404, 1, 1, VK_FORMAT_R32_SINT},
    {0x1404, 2, 1, VK_FORMAT_R32G32_SINT},
    {0x1404, 3, 1, VK_FORMAT_R32G32B32_SINT},
    {0x1404, 4, 1, VK_FORMAT_R32G32B32A32_SINT},
    {0},
};

static const uint32_t vkgl_topologies[] = {
    VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
    VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
};

static uint32_t vkgl_new(VkglTable * table, uint32_t size) {
    if (table->count == table->capacity) {
        table->capacity = table->capacity ? table->capacity * 2 : 64;
        table->items = (char *)PyMem_Realloc(table->items, (size_t)table->capacity * size);
    }
    memset(table->items + (size_t)table->count * size, 0, size);
    return table->count++;
}

static uint32_t vkgl_memory_type(uint32_t bits, uint32_t flags) {
    for (uint32_t i = 0; i < vkgl.memory.memoryTypeCount; ++i) {
        if ((bits & (1u << i)) && (vkgl.memory.memoryTypes[i].propertyFlags & flags) == flags) {
            return i;
        }
    }
    return vkgl_memory_type(bits, 0);
}

static void vkgl_barrier(void) {
    VkMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
    };
    vkCmdPipelineBarrier(vkgl.command, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 1, &barrier, 0, NULL, 0, NULL);
}

static void vkgl_outside(void) {
    if (vkgl.rendering) {
        vkCmdEndRendering(vkgl.command);
        vkgl.rendering = 0;
    }
    vkgl_barrier();
    vkgl.dirty = 1;
}

static void vkgl_flush(void) {
    if (vkgl.dirty) {
        vkgl_outside();
        vkEndCommandBuffer(vkgl.command);
        VkSubmitInfo submit = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &vkgl.command,
        };
        vkQueueSubmit(vkgl.queue, 1, &submit, 0);
        vkQueueWaitIdle(vkgl.queue);
        vkResetCommandBuffer(vkgl.command, 0);
        VkCommandBufferBeginInfo begin = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        vkBeginCommandBuffer(vkgl.command, &begin);
        vkgl.current_pipeline = 0;
        vkgl.dirty = 0;
    }
    for (int32_t i = 0; i < vkgl.garbage_count; ++i) {
        vkDestroyImageView(vkgl.device, vkgl.garbage[i], NULL);
    }
    vkgl.garbage_count = 0;
    vkgl.staging_offset = 0;
    vkgl.submits += 1;
}

static void vkgl_create_buffer(VkglBuffer * buffer, int64_t size) {
    VkBufferCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = (uint64_t)size,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
    };
    vkCreateBuffer(vkgl.device, &info, NULL, &buffer->buffer);
    VkMemoryRequirements requirements = {0};
    vkGetBufferMemoryRequirements(vkgl.device, buffer->buffer, &requirements);
    VkMemoryAllocateInfo allocate = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = requirements.size,
        .memoryTypeIndex = vkgl_memory_type(requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
    };
    vkAllocateMemory(vkgl.device, &allocate, NULL, &buffer->memory);
    vkBindBufferMemory(vkgl.device, buffer->buffer, buffer->memory, 0);
    vkMapMemory(vkgl.device, buffer->memory, 0, VK_WHOLE_SIZE, 0, (void **)&buffer->ptr);
    buffer->size = size;
}

static void vkgl_destroy_buffer(VkglBuffer * buffer) {
    if (buffer->buffer) {
        vkDestroyBuffer(vkgl.device, buffer->buffer, NULL);
        vkFreeMemory(vkgl.device, buffer->memory, NULL);
    }
    memset(buffer, 0, sizeof(VkglBuffer));
}

static int64_t vkgl_staging(const void * data, int64_t size) {
    int64_t offset = (vkgl.staging_offset + 15) & ~15;
    if (offset + size > vkgl.staging.size) {
        vkgl_flush();
        offset = 0;
        if (size > vkgl.staging.size) {
            int64_t capacity = vkgl.staging.size * 2 > size ? vkgl.staging.size * 2 : size;
            vkgl_destroy_buffer(&vkgl.staging);
            vkgl_create_buffer(&vkgl.staging, capacity);
        }
    }
    memcpy(vkgl.staging.ptr + offset, data, (size_t)size);
    vkgl.staging_offset = offset + size;
    return offset;
}

static uint32_t * vkgl_buffer_target(uint32_t target) {
    switch (target) {
        case GL_ARRAY_BUFFER: return &vkgl.array_buffer;
        case GL_ELEMENT_ARRAY_BUFFER: return &VKGL_VERTEX_ARRAY(vkgl.vertex_array)->index_buffer;
        case GL_COPY_READ_BUFFER: return &vkgl.copy_read_buffer;
        case GL_COPY_WRITE_BUFFER: return &vkgl.copy_write_buffer;
        case GL_PIXEL_PACK_BUFFER: return &vkgl.pixel_pack_buffer;
        case GL_PIXEL_UNPACK_BUFFER: return &vkgl.pixel_unpack_buffer;
        case GL_UNIFORM_BUFFER: return &vkgl.uniform_buffer;
        case GL_SHADER_STORAGE_BUFFER: return &vkgl.storage_buffer;
        case GL_DRAW_INDIRECT_BUFFER: return &vkgl.indirect_buffer;
    }
    return &vkgl.copy_write_buffer;
}

static VkglBuffer * vkgl_bound_buffer(uint32_t target) {
    return VKGL_BUFFER(*vkgl_buffer_target(target));
}

static VkglImage * vkgl_bound_texture(void) {
    return VKGL_IMAGE(vkgl.textures[vkgl.active_unit]);
}

static void vkgl_create_image(VkglImage * image, uint32_t internal_format, uint32_t format, int32_t width, int32_t height, int32_t layers, int32_t levels, int32_t samples, int32_t cube) {
    for (int32_t i = 0; vkgl_image_formats[i][0]; ++i) {
        if (vkgl_image_formats[i][0] == internal_format) {
            image->format = vkgl_image_formats[i][1];
            image->pixel_size = (int32_t)vkgl_image_formats[i][2];
        }
    }

    if (internal_format == 0x88F0) {
        image->format = vkgl.depth_stencil_format;
    }

    if (format == GL_BGRA) {
        image->format = VK_FORMAT_B8G8R8A8_UNORM;
    }

    image->aspect = VK_IMAGE_ASPECT_COLOR_BIT;
    if (image->format == VK_FORMAT_D16_UNORM || image->format == VK_FORMAT_D32_SFLOAT) {
        image->aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    if (image->format == vkgl.depth_stencil_format) {
        image->aspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    VkFormatProperties properties = {0};
    vkGetPhysicalDeviceFormatProperties(vkgl.physical, image->format, &properties);
    image->features = properties.optimalTilingFeatures;

    uint32_t usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    if ((image->features & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) && samples == 1) {
        usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    if ((image->features & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) && samples == 1) {
        usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    }
    if (image->features & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) {
        usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }
    if (image->features & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }

    VkImageCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .flags = cube ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = image->format,
        .extent = {(uint32_t)width, (uint32_t)height, 1},
        .mipLevels = (uint32_t)levels,
        .arrayLayers = (uint32_t)layers,
        .samples = (uint32_t)samples,
        .usage = usage,
    };
    vkCreateImage(vkgl.device, &info, NULL, &image->image);
    VkMemoryRequirements requirements = {0};
    vkGetImageMemoryRequirements(vkgl.device, image->image, &requirements);
    VkMemoryAllocateInfo allocate = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = requirements.size,
        .memoryTypeIndex = vkgl_memory_type(requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    };
    vkAllocateMemory(vkgl.device, &allocate, NULL, &image->memory);
    vkBindImageMemory(vkgl.device, image->image, image->memory, 0);

    image->width = width;
    image->height = height;
    image->layers = layers;
    image->levels = levels;
    image->samples = samples;

    VkImageSubresourceRange range = {image->aspect, 0, (uint32_t)levels, 0, (uint32_t)layers};

    if (usage & VK_IMAGE_USAGE_SAMPLED_BIT) {
        VkImageViewCreateInfo view = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image->image,
            .viewType = cube ? VK_IMAGE_VIEW_TYPE_CUBE : image->target == GL_TEXTURE_2D_ARRAY ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D,
            .format = image->format,
            .subresourceRange = range,
        };
        view.subresourceRange.aspectMask &= ~VK_IMAGE_ASPECT_STENCIL_BIT;
        vkCreateImageView(vkgl.device, &view, NULL, &image->view);
    }

    vkgl_outside();
    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_GENERAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image->image,
        .subresourceRange = range,
    };
    vkCmdPipelineBarrier(vkgl.command, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);
}

static void vkgl_destroy_image(VkglImage * image) {
    if (image->view) {
        vkDestroyImageView(vkgl.device, image->view, NULL);
    }
    if (image->image) {
        vkDestroyImage(vkgl.device, image->image, NULL);
        vkFreeMemory(vkgl.device, image->memory, NULL);
    }
    memset(image, 0, sizeof(VkglImage));
}

static void vkgl_attach(uint32_t target, uint32_t attachment, uint32_t image, int32_t level, int32_t layer) {
    VkglFramebuffer * framebuffer = VKGL_FRAMEBUFFER(target == GL_READ_FRAMEBUFFER ? vkgl.read_framebuffer : vkgl.draw_framebuffer);
    VkglAttachment * slot = attachment == GL_DEPTH_ATTACHMENT || attachment == GL_DEPTH_STENCIL_ATTACHMENT ? &framebuffer->depth : &framebuffer->color[attachment - GL_COLOR_ATTACHMENT0];
    if (slot->view) {
        vkgl_flush();
        vkDestroyImageView(vkgl.device, slot->view, NULL);
    }
    memset(slot, 0, sizeof(VkglAttachment));
    if (!image) {
        return;
    }
    VkglImage * source = VKGL_IMAGE(image);
    slot->image = image;
    slot->level = level;
    slot->layer = layer;
    VkImageViewCreateInfo view = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = source->image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = source->format,
        .subresourceRange = {source->aspect, (uint32_t)level, 1, (uint32_t)layer, 1},
    };
    vkCreateImageView(vkgl.device, &view, NULL, &slot->view);
}

static VkImageSubresourceLayers vkgl_layers(VkglAttachment * attachment, uint32_t aspect) {
    VkImageSubresourceLayers res = {aspect, (uint32_t)attachment->level, (uint32_t)attachment->layer, 1};
    return res;
}

static void vkgl_begin_rendering(void) {
    if (vkgl.rendering == vkgl.draw_framebuffer + 1) {
        return;
    }

    vkgl_outside();

    VkglFramebuffer * framebuffer = VKGL_FRAMEBUFFER(vkgl.draw_framebuffer);
    VkRenderingAttachmentInfo colors[8] = {0};
    VkRenderingAttachmentInfo depth = {0};
    uint32_t width = 0x7FFFFFFF;
    uint32_t height = 0x7FFFFFFF;

    for (int32_t i = 0; i < framebuffer->color_count + 1; ++i) {
        VkglAttachment * attachment = i < framebuffer->color_count ? &framebuffer->color[i] : &framebuffer->depth;
        VkRenderingAttachmentInfo * info = i < framebuffer->color_count ? &colors[i] : &depth;
        if (!attachment->image) {
            continue;
        }
        VkglImage * image = VKGL_IMAGE(attachment->image);
        uint32_t level_width = (uint32_t)image->width >> attachment->level;
        uint32_t level_height = (uint32_t)image->height >> attachment->level;
        width = level_width && level_width < width ? level_width : width;
        height = level_height && level_height < height ? level_height : height;
        info->sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        info->imageView = attachment->view;
        info->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        info->loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        info->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    }

    uint32_t depth_aspect = framebuffer->depth.image ? VKGL_IMAGE(framebuffer->depth.image)->aspect : 0;

    VkRenderingInfo info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = {{0, 0}, {width, height}},
        .layerCount = 1,
        .colorAttachmentCount = (uint32_t)framebuffer->color_count,
        .pColorAttachments = colors,
        .pDepthAttachment = depth_aspect & VK_IMAGE_ASPECT_DEPTH_BIT ? &depth : NULL,
        .pStencilAttachment = depth_aspect & VK_IMAGE_ASPECT_STENCIL_BIT ? &depth : NULL,
    };

    vkCmdBeginRendering(vkgl.command, &info);
    vkgl.rendering = vkgl.draw_framebuffer + 1;
    vkgl.rendering_size[0] = width;
    vkgl.rendering_size[1] = height;
}

static uint32_t vkgl_blend_factor(uint32_t factor) {
    switch (factor) {
        case 0x0000: return VK_BLEND_FACTOR_ZERO;
        case 0x0001: return VK_BLEND_FACTOR_ONE;
        case 0x0300: return VK_BLEND_FACTOR_SRC_COLOR;
        case 0x0301: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case 0x0306: return VK_BLEND_FACTOR_DST_COLOR;
        case 0x0307: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case 0x0302: return VK_BLEND_FACTOR_SRC_ALPHA;
        case 0x0303: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case 0x0304: return VK_BLEND_FACTOR_DST_ALPHA;
        case 0x0305: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case 0x8001: return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case 0x8002: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case 0x8003: return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case 0x8004: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        case 0x0308: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
    }
    return VK_BLEND_FACTOR_ONE;
}

static uint32_t vkgl_blend_op(uint32_t equation) {
    switch (equation) {
        case 0x800A: return VK_BLEND_OP_SUBTRACT;
        case 0x800B: return VK_BLEND_OP_REVERSE_SUBTRACT;
        case 0x8007: return VK_BLEND_OP_MIN;
        case 0x8008: return VK_BLEND_OP_MAX;
    }
    return VK_BLEND_OP_ADD;
}

static uint32_t vkgl_stencil_op(uint32_t op) {
    switch (op) {
        case 0x0000: return VK_STENCIL_OP_ZERO;
        case 0x1E01: return VK_STENCIL_OP_REPLACE;
        case 0x1E02: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case 0x1E03: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case 0x150A: return VK_STENCIL_OP_INVERT;
        case 0x8507: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case 0x8508: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
    }
    return VK_STENCIL_OP_KEEP;
}

static VkStencilOpState vkgl_stencil_state(int32_t face) {
    VkglState * state = &vkgl.state;
    VkStencilOpState res = {
        .failOp = vkgl_stencil_op(state->stencil_fail[face]),
        .passOp = vkgl_stencil_op(state->stencil_depth_pass[face]),
        .depthFailOp = vkgl_stencil_op(state->stencil_depth_fail[face]),
        .compareOp = state->stencil_func[face] - GL_NEVER,
        .compareMask = state->stencil_read_mask[face],
        .writeMask = state->stencil_write_mask,
        .reference = state->stencil_ref[face],
    };
    return res;
}

static uint64_t vkgl_binding_mask(void) {
    uint64_t mask = 0;
    for (uint32_t i = 0; i < 8; ++i) {
        mask |= vkgl.uniform_buffers[i].buffer ? 1ull << i : 0;
        mask |= vkgl.storage_buffers[i].buffer ? 1ull << (8 + i) : 0;
        mask |= vkgl.storage_images[i].view ? 1ull << (32 + i) : 0;
    }
    for (uint32_t i = 0; i < 16; ++i) {
        mask |= VKGL_IMAGE(vkgl.textures[i])->view ? 1ull << (16 + i) : 0;
    }
    return mask;
}

static VkglLayout * vkgl_layout(uint64_t mask) {
    for (int32_t i = 0; i < vkgl.layout_count; ++i) {
        if (vkgl.layouts[i].mask == mask) {
            return &vkgl.layouts[i];
        }
    }

    VkDescriptorSetLayoutBinding bindings[40] = {0};
    uint32_t count = 0;
    for (uint32_t i = 0; i < 40; ++i) {
        if (!(mask & (1ull << i))) {
            continue;
        }
        bindings[count].binding = i < 8 ? i : i < 16 ? 32 + i - 8 : i < 32 ? 64 + i - 16 : 128 + i - 32;
        bindings[count].descriptorType = i < 8 ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : i < 16 ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER : i < 32 ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        bindings[count].descriptorCount = 1;
        bindings[count].stageFlags = VK_SHADER_STAGE_ALL;
        count += 1;
    }

    VkglLayout * layout = &vkgl.layouts[vkgl.layout_count < 64 ? vkgl.layout_count++ : 63];
    layout->mask = mask;

    VkDescriptorSetLayoutCreateInfo set_layout_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR,
        .bindingCount = count,
        .pBindings = bindings,
    };
    vkCreateDescriptorSetLayout(vkgl.device, &set_layout_info, NULL, &layout->set_layout);

    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &layout->set_layout,
    };
    vkCreatePipelineLayout(vkgl.device, &pipeline_layout_info, NULL, &layout->pipeline_layout);
    return layout;
}

static uint64_t vkgl_graphics_pipeline(VkglPipelineKey * key) {
    for (int32_t i = 0; i < vkgl.pipeline_count; ++i) {
        if (!memcmp(&vkgl.pipelines[i].key, key, sizeof(VkglPipelineKey))) {
            return vkgl.pipelines[i].pipeline;
        }
    }

    VkglProgram * program = VKGL_PROGRAM(key->program);
    VkglState * state = &key->state;

    VkPipelineShaderStageCreateInfo stages[2] = {
        {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_VERTEX_BIT, .module = program->vertex, .pName = "main"},
        {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_FRAGMENT_BIT, .module = program->fragment, .pName = "main"},
    };

    VkVertexInputBindingDescription bindings[16] = {0};
    VkVertexInputAttributeDescription attributes[16] = {0};
    uint32_t attribute_count = 0;

    for (uint32_t i = 0; i < 16; ++i) {
        if (!key->attrib_formats[i]) {
            continue;
        }
        VkVertexInputBindingDescription binding = {i, key->attrib_strides[i], key->attrib_rates[i]};
        VkVertexInputAttributeDescription attribute = {i, i, key->attrib_formats[i], 0};
        bindings[attribute_count] = binding;
        attributes[attribute_count] = attribute;
        attribute_count += 1;
    }

    VkPipelineVertexInputStateCreateInfo vertex_input = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = attribute_count,
        .pVertexBindingDescriptions = bindings,
        .vertexAttributeDescriptionCount = attribute_count,
        .pVertexAttributeDescriptions = attributes,
    };

    int32_t strip = key->topology == VK_PRIMITIVE_TOPOLOGY_LINE_STRIP || key->topology == VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP || key->topology == VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = key->topology,
        .primitiveRestartEnable = (uint32_t)strip,
    };

    VkPipelineViewportDepthClipControlCreateInfoEXT depth_clip_control = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_DEPTH_CLIP_CONTROL_CREATE_INFO_EXT,
        .negativeOneToOne = 1,
    };

    VkPipelineViewportStateCreateInfo viewport = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = vkgl.depth_clip_control ? &depth_clip_control : NULL,
        .viewportCount = 1,
        .scissorCount = 1,
    };

    uint32_t cull_mode = VK_CULL_MODE_NONE;
    if (state->cull) {
        cull_mode = state->cull_face == GL_FRONT ? VK_CULL_MODE_FRONT_BIT : state->cull_face == GL_BACK ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_FRONT_AND_BACK;
    }

    VkPipelineRasterizationStateCreateInfo rasterization = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .cullMode = cull_mode,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = state->polygon_offset,
        .depthBiasConstantFactor = state->polygon_offset_units,
        .depthBiasSlopeFactor = state->polygon_offset_factor,
        .lineWidth = 1.0f,
    };

    VkPipelineMultisampleStateCreateInfo multisample = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = key->samples,
    };

    VkPipelineDepthStencilStateCreateInfo depth_stencil = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = state->depth_test,
        .depthWriteEnable = state->depth_test && state->depth_write,
        .depthCompareOp = state->depth_func - GL_NEVER,
        .stencilTestEnable = state->stencil,
        .front = vkgl_stencil_state(0),
        .back = vkgl_stencil_state(1),
    };

    VkPipelineColorBlendAttachmentState blend[8] = {0};
    for (uint32_t i = 0; i < key->color_count; ++i) {
        VkFormatProperties properties = {0};
        vkGetPhysicalDeviceFormatProperties(vkgl.physical, key->color_formats[i], &properties);
        blend[i].blendEnable = state->blend && (properties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT) ? 1 : 0;
        blend[i].srcColorBlendFactor = vkgl_blend_factor(state->blend_func[0]);
        blend[i].dstColorBlendFactor = vkgl_blend_factor(state->blend_func[1]);
        blend[i].colorBlendOp = vkgl_blend_op(state->blend_equation[0]);
        blend[i].srcAlphaBlendFactor = vkgl_blend_factor(state->blend_func[2]);
        blend[i].dstAlphaBlendFactor = vkgl_blend_factor(state->blend_func[3]);
        blend[i].alphaBlendOp = vkgl_blend_op(state->blend_equation[1]);
        blend[i].colorWriteMask = 0xF;
    }

    VkPipelineColorBlendStateCreateInfo color_blend = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = key->color_count,
        .pAttachments = blend,
    };

    uint32_t dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamic = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 2,
        .pDynamicStates = dynamic_states,
    };

    VkPipelineRenderingCreateInfo rendering = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount = key->color_count,
        .pColorAttachmentFormats = key->color_formats,
        .depthAttachmentFormat = key->depth_aspect & VK_IMAGE_ASPECT_DEPTH_BIT ? key->depth_format : VK_FORMAT_UNDEFINED,
        .stencilAttachmentFormat = key->depth_aspect & VK_IMAGE_ASPECT_STENCIL_BIT ? key->depth_format : VK_FORMAT_UNDEFINED,
    };

    VkGraphicsPipelineCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &rendering,
        .stageCount = 2,
        .pStages = stages,
        .pVertexInputState = &vertex_input,
        .pInputAssemblyState = &input_assembly,
        .pViewportState = &viewport,
        .pRasterizationState = &rasterization,
        .pMultisampleState = &multisample,
        .pDepthStencilState = &depth_stencil,
        .pColorBlendState = &color_blend,
        .pDynamicState = &dynamic,
        .layout = vkgl_layout(key->layout[0] | (uint64_t)key->layout[1] << 32)->pipeline_layout,
    };

    uint64_t pipeline = 0;
    vkCreateGraphicsPipelines(vkgl.device, 0, 1, &info, NULL, &pipeline);

    vkgl.pipelines = (VkglPipeline *)PyMem_Realloc(vkgl.pipelines, sizeof(VkglPipeline) * (vkgl.pipeline_count + 1));
    vkgl.pipelines[vkgl.pipeline_count].key = *key;
    vkgl.pipelines[vkgl.pipeline_count].pipeline = pipeline;
    vkgl.pipeline_count += 1;
    return pipeline;
}

static uint64_t vkgl_sampler(uint32_t name) {
    if (!name) {
        return vkgl.default_sampler;
    }

    VkglSampler * sampler = VKGL_SAMPLER(name);
    if (sampler->sampler) {
        return sampler->sampler;
    }

    uint32_t min_filter = sampler->min_filter;
    int32_t mipmaps = min_filter >= GL_NEAREST_MIPMAP_NEAREST;
    uint32_t wraps[3] = {0};

    for (int32_t i = 0; i < 3; ++i) {
        wraps[i] = sampler->wrap[i] == GL_CLAMP_TO_EDGE ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE : sampler->wrap[i] == GL_MIRRORED_REPEAT ? VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT : VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }

    VkSamplerCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = sampler->mag_filter == GL_LINEAR ? VK_FILTER_LINEAR : VK_FILTER_NEAREST,
        .minFilter = min_filter == GL_LINEAR || min_filter == GL_LINEAR_MIPMAP_NEAREST || min_filter == GL_LINEAR_MIPMAP_LINEAR ? VK_FILTER_LINEAR : VK_FILTER_NEAREST,
        .mipmapMode = min_filter == GL_NEAREST_MIPMAP_LINEAR || min_filter == GL_LINEAR_MIPMAP_LINEAR ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST,
        .addressModeU = wraps[0],
        .addressModeV = wraps[1],
        .addressModeW = wraps[2],
        .mipLodBias = sampler->lod_bias,
        .anisotropyEnable = sampler->anisotropy > 1.0f,
        .maxAnisotropy = sampler->anisotropy,
        .compareEnable = sampler->compare_mode == GL_COMPARE_REF_TO_TEXTURE,
        .compareOp = sampler->compare_func - GL_NEVER,
        .minLod = mipmaps ? sampler->min_lod : 0.0f,
        .maxLod = mipmaps ? sampler->max_lod : 0.25f,
    };

    vkCreateSampler(vkgl.device, &info, NULL, &sampler->sampler);
    return sampler->sampler;
}

static void vkgl_push_descriptors(uint32_t bind_point, uint64_t pipeline_layout) {
    VkWriteDescriptorSet writes[40] = {0};
    VkDescriptorBufferInfo buffers[16] = {0};
    VkDescriptorImageInfo images[24] = {0};
    uint32_t count = 0;

    for (uint32_t i = 0; i < 16; ++i) {
        VkglRange * range = i < 8 ? &vkgl.uniform_buffers[i] : &vkgl.storage_buffers[i - 8];
        if (!range->buffer) {
            continue;
        }
        buffers[i].buffer = VKGL_BUFFER(range->buffer)->buffer;
        buffers[i].offset = range->offset;
        buffers[i].range = range->size;
        writes[count].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[count].dstBinding = i < 8 ? i : 32 + i - 8;
        writes[count].descriptorCount = 1;
        writes[count].descriptorType = i < 8 ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        writes[count].pBufferInfo = &buffers[i];
        count += 1;
    }

    for (uint32_t i = 0; i < 16; ++i) {
        VkglImage * image = VKGL_IMAGE(vkgl.textures[i]);
        if (!image->view) {
            continue;
        }
        images[i].sampler = vkgl_sampler(vkgl.unit_samplers[i]);
        images[i].imageView = image->view;
        images[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        writes[count].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[count].dstBinding = 64 + i;
        writes[count].descriptorCount = 1;
        writes[count].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[count].pImageInfo = &images[i];
        count += 1;
    }

    for (uint32_t i = 0; i < 8; ++i) {
        VkglStorageImage * unit = &vkgl.storage_images[i];
        if (!unit->view) {
            continue;
        }
        images[16 + i].imageView = unit->view;
        images[16 + i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        writes[count].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[count].dstBinding = 128 + i;
        writes[count].descriptorCount = 1;
        writes[count].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        writes[count].pImageInfo = &images[16 + i];
        count += 1;
    }

    if (count) {
        vkCmdPushDescriptorSetKHR(vkgl.command, bind_point, pipeline_layout, 0, count, writes);
    }
}

static void vkgl_prepare_draw(uint32_t mode) {
    vkgl_begin_rendering();

    VkglFramebuffer * framebuffer = VKGL_FRAMEBUFFER(vkgl.draw_framebuffer);
    VkglVertexArray * vertex_array = VKGL_VERTEX_ARRAY(vkgl.vertex_array);

    VkglPipelineKey key;
    memset(&key, 0, sizeof(key));
    key.program = vkgl.program;
    key.topology = vkgl_topologies[mode];
    key.samples = 1;
    key.color_count = (uint32_t)framebuffer->color_count;
    key.state = vkgl.state;
    if (!key.state.polygon_offset) {
        key.state.polygon_offset_factor = 0.0f;
        key.state.polygon_offset_units = 0.0f;
    }
    uint64_t mask = vkgl_binding_mask();
    key.layout[0] = (uint32_t)mask;
    key.layout[1] = (uint32_t)(mask >> 32);

    for (int32_t i = 0; i < framebuffer->color_count; ++i) {
        VkglImage * image = VKGL_IMAGE(framebuffer->color[i].image);
        key.color_formats[i] = image->format;
        key.samples = (uint32_t)image->samples;
    }

    if (framebuffer->depth.image) {
        VkglImage * image = VKGL_IMAGE(framebuffer->depth.image);
        key.depth_format = image->format;
        key.depth_aspect = image->aspect;
        key.samples = (uint32_t)image->samples;
    }

    for (int32_t i = 0; i < 16; ++i) {
        VkglAttrib * attrib = &vertex_array->attribs[i];
        if (!attrib->enabled) {
            continue;
        }
        key.attrib_formats[i] = attrib->format;
        key.attrib_strides[i] = attrib->stride;
        key.attrib_rates[i] = attrib->rate;
        vkCmdBindVertexBuffers(vkgl.command, (uint32_t)i, 1, &VKGL_BUFFER(attrib->buffer)->buffer, &attrib->offset);
    }

    uint64_t pipeline = vkgl_graphics_pipeline(&key);
    if (vkgl.current_pipeline != pipeline) {
        vkCmdBindPipeline(vkgl.command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkgl.current_pipeline = pipeline;
    }

    VkViewport viewport = {(float)vkgl.viewport[0], (float)vkgl.viewport[1], (float)vkgl.viewport[2], (float)vkgl.viewport[3], 0.0f, 1.0f};
    vkCmdSetViewport(vkgl.command, 0, 1, &viewport);
    int32_t box[4] = {0, 0, (int32_t)vkgl.rendering_size[0], (int32_t)vkgl.rendering_size[1]};
    if (vkgl.scissor_test) {
        box[0] = vkgl.scissor[0] > 0 ? vkgl.scissor[0] : 0;
        box[1] = vkgl.scissor[1] > 0 ? vkgl.scissor[1] : 0;
        box[2] = vkgl.scissor[0] + vkgl.scissor[2] < box[2] ? vkgl.scissor[0] + vkgl.scissor[2] : box[2];
        box[3] = vkgl.scissor[1] + vkgl.scissor[3] < box[3] ? vkgl.scissor[1] + vkgl.scissor[3] : box[3];
        box[2] = box[2] > box[0] ? box[2] - box[0] : 0;
        box[3] = box[3] > box[1] ? box[3] - box[1] : 0;
    }
    VkRect2D scissor = {{box[0], box[1]}, {(uint32_t)box[2], (uint32_t)box[3]}};
    vkCmdSetScissor(vkgl.command, 0, 1, &scissor);
    vkgl_push_descriptors(VK_PIPELINE_BIND_POINT_GRAPHICS, vkgl_layout(mask)->pipeline_layout);
}

static void vkgl_bind_index_buffer(uint32_t type) {
    VkglBuffer * buffer = VKGL_BUFFER(VKGL_VERTEX_ARRAY(vkgl.vertex_array)->index_buffer);
    vkCmdBindIndexBuffer(vkgl.command, buffer->buffer, 0, type == GL_UNSIGNED_INT ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16);
}

static int32_t vkgl_index_size(uint32_t type) {
    return type == GL_UNSIGNED_INT ? 4 : 2;
}

static void GLAPI vkgl_glActiveTexture(uint32_t texture) {
    vkgl.active_unit = texture - GL_TEXTURE0;
}

static void GLAPI vkgl_glAttachShader(uint32_t program, uint32_t shader) {
    VkglProgram * res = VKGL_PROGRAM(program);
    res->shaders[res->shaders[0] ? 1 : 0] = shader;
}

static void GLAPI vkgl_glBindBuffer(uint32_t target, uint32_t buffer) {
    *vkgl_buffer_target(target) = buffer;
}

static void GLAPI vkgl_glBindBufferRange(uint32_t target, uint32_t index, uint32_t buffer, intptr_t offset, intptr_t size) {
    VkglRange * range = target == GL_UNIFORM_BUFFER ? &vkgl.uniform_buffers[index] : &vkgl.storage_buffers[index];
    range->buffer = buffer;
    range->offset = (uint64_t)offset;
    range->size = (uint64_t)size;
    *vkgl_buffer_target(target) = buffer;
}

static void GLAPI vkgl_glBindBufferBase(uint32_t target, uint32_t index, uint32_t buffer) {
    vkgl_glBindBufferRange(target, index, buffer, 0, (intptr_t)VK_WHOLE_SIZE);
}

static void GLAPI vkgl_glBindFramebuffer(uint32_t target, uint32_t framebuffer) {
    if (target != GL_READ_FRAMEBUFFER) {
        vkgl.draw_framebuffer = framebuffer;
    }
    if (target != GL_DRAW_FRAMEBUFFER) {
        vkgl.read_framebuffer = framebuffer;
    }
}

static void GLAPI vkgl_glBindRenderbuffer(uint32_t target, uint32_t renderbuffer) {
    vkgl.renderbuffer = renderbuffer;
}

static void GLAPI vkgl_glBindSampler(uint32_t unit, uint32_t sampler) {
    vkgl.unit_samplers[unit] = sampler;
}

static void GLAPI vkgl_glBindTexture(uint32_t target, uint32_t texture) {
    uint32_t * slot = &vkgl.textures[vkgl.active_unit];
    if (texture) {
        VkglImage * image = VKGL_IMAGE(texture);
        image->target = image->target ? image->target : target;
        *slot = texture;
    } else if (VKGL_IMAGE(*slot)->target == target) {
        *slot = 0;
    }
}

static void GLAPI vkgl_glBindVertexArray(uint32_t array) {
    vkgl.vertex_array = array;
}

static void GLAPI vkgl_glBlendEquationSeparate(uint32_t mode_rgb, uint32_t mode_alpha) {
    vkgl.state.blend_equation[0] = mode_rgb;
    vkgl.state.blend_equation[1] = mode_alpha;
}

static void GLAPI vkgl_glBlendFuncSeparate(uint32_t src_rgb, uint32_t dst_rgb, uint32_t src_alpha, uint32_t dst_alpha) {
    vkgl.state.blend_func[0] = src_rgb;
    vkgl.state.blend_func[1] = dst_rgb;
    vkgl.state.blend_func[2] = src_alpha;
    vkgl.state.blend_func[3] = dst_alpha;
}

static void GLAPI vkgl_glBlitFramebuffer(int32_t src_x0, int32_t src_y0, int32_t src_x1, int32_t src_y1, int32_t dst_x0, int32_t dst_y0, int32_t dst_x1, int32_t dst_y1, uint32_t mask, uint32_t filter) {
    VkglFramebuffer * src = VKGL_FRAMEBUFFER(vkgl.read_framebuffer);
    VkglFramebuffer * dst = VKGL_FRAMEBUFFER(vkgl.draw_framebuffer);
    int32_t color = (mask & GL_COLOR_BUFFER_BIT) != 0;
    int32_t count = color ? dst->color_count : 1;
    VkglAttachment * src_attachment = color ? &src->color[src->read] : &src->depth;
    VkglImage * src_image = VKGL_IMAGE(src_attachment->image);
    uint32_t aspect = color ? VK_IMAGE_ASPECT_COLOR_BIT : src_image->aspect;

    vkgl_outside();

    for (int32_t i = 0; i < count; ++i) {
        VkglAttachment * dst_attachment = color ? &dst->color[i] : &dst->depth;
        VkglImage * dst_image = VKGL_IMAGE(dst_attachment->image);
        if (src_image->samples > 1) {
            VkImageResolve region = {
                vkgl_layers(src_attachment, aspect),
                {src_x0, src_y0, 0},
                vkgl_layers(dst_attachment, aspect),
                {dst_x0, dst_y0, 0},
                {(uint32_t)(src_x1 - src_x0), (uint32_t)(src_y1 - src_y0), 1},
            };
            vkCmdResolveImage(vkgl.command, src_image->image, VK_IMAGE_LAYOUT_GENERAL, dst_image->image, VK_IMAGE_LAYOUT_GENERAL, 1, &region);
            continue;
        }
        VkImageBlit region = {
            vkgl_layers(src_attachment, aspect),
            {{src_x0, src_y0, 0}, {src_x1, src_y1, 1}},
            vkgl_layers(dst_attachment, aspect),
            {{dst_x0, dst_y0, 0}, {dst_x1, dst_y1, 1}},
        };
        vkCmdBlitImage(vkgl.command, src_image->image, VK_IMAGE_LAYOUT_GENERAL, dst_image->image, VK_IMAGE_LAYOUT_GENERAL, 1, &region, filter == GL_LINEAR ? VK_FILTER_LINEAR : VK_FILTER_NEAREST);
    }
}

static void GLAPI vkgl_glBufferSubData(uint32_t target, intptr_t offset, intptr_t size, const void * data) {
    VkglBuffer * buffer = vkgl_bound_buffer(target);
    VkBufferCopy region = {(uint64_t)vkgl_staging(data, size), (uint64_t)offset, (uint64_t)size};
    vkgl_outside();
    vkCmdCopyBuffer(vkgl.command, vkgl.staging.buffer, buffer->buffer, 1, &region);
}

static void GLAPI vkgl_glBufferData(uint32_t target, intptr_t size, const void * data, uint32_t usage) {
    VkglBuffer * buffer = vkgl_bound_buffer(target);
    if (buffer->buffer) {
        vkgl_flush();
        vkgl_destroy_buffer(buffer);
    }
    vkgl_create_buffer(buffer, size);
    if (data) {
        vkgl_glBufferSubData(target, 0, size, data);
    }
}

static uint32_t GLAPI vkgl_glCheckFramebufferStatus(uint32_t target) {
    return GL_FRAMEBUFFER_COMPLETE;
}

static void vkgl_clear(VkglAttachment * attachment, uint32_t aspect, const void * value) {
    VkglImage * image = VKGL_IMAGE(attachment->image);
    VkImageSubresourceRange range = {aspect, (uint32_t)attachment->level, 1, (uint32_t)attachment->layer, 1};
    vkgl_outside();
    if (aspect == VK_IMAGE_ASPECT_COLOR_BIT) {
        vkCmdClearColorImage(vkgl.command, image->image, VK_IMAGE_LAYOUT_GENERAL, (const VkClearColorValue *)value, 1, &range);
    } else {
        vkCmdClearDepthStencilImage(vkgl.command, image->image, VK_IMAGE_LAYOUT_GENERAL, (const VkClearDepthStencilValue *)value, 1, &range);
    }
}

static void GLAPI vkgl_glClearBufferfi(uint32_t buffer, int32_t drawbuffer, float depth, int32_t stencil) {
    VkglFramebuffer * framebuffer = VKGL_FRAMEBUFFER(vkgl.draw_framebuffer);
    VkClearDepthStencilValue value = {depth, (uint32_t)stencil};
    vkgl_clear(&framebuffer->depth, VKGL_IMAGE(framebuffer->depth.image)->aspect, &value);
}

static void GLAPI vkgl_glClearBufferfv(uint32_t buffer, int32_t drawbuffer, const float * value) {
    VkglFramebuffer * framebuffer = VKGL_FRAMEBUFFER(vkgl.draw_framebuffer);
    if (buffer == GL_DEPTH) {
        VkClearDepthStencilValue depth = {value[0], 0};
        vkgl_clear(&framebuffer->depth, VK_IMAGE_ASPECT_DEPTH_BIT, &depth);
        return;
    }
    vkgl_clear(&framebuffer->color[drawbuffer], VK_IMAGE_ASPECT_COLOR_BIT, value);
}

static void GLAPI vkgl_glClearBufferiv(uint32_t buffer, int32_t drawbuffer, const int32_t * value) {
    vkgl_clear(&VKGL_FRAMEBUFFER(vkgl.draw_framebuffer)->color[drawbuffer], VK_IMAGE_ASPECT_COLOR_BIT, value);
}

static void GLAPI vkgl_glClearBufferuiv(uint32_t buffer, int32_t drawbuffer, const uint32_t * value) {
    vkgl_clear(&VKGL_FRAMEBUFFER(vkgl.draw_framebuffer)->color[drawbuffer], VK_IMAGE_ASPECT_COLOR_BIT, value);
}

static void GLAPI vkgl_glCompileShader(uint32_t shader) {
}

static void GLAPI vkgl_glCopyBufferSubData(uint32_t read_target, uint32_t write_target, intptr_t read_offset, intptr_t write_offset, intptr_t size) {
    VkBufferCopy region = {(uint64_t)read_offset, (uint64_t)write_offset, (uint64_t)size};
    vkgl_outside();
    vkCmdCopyBuffer(vkgl.command, vkgl_bound_buffer(read_target)->buffer, vkgl_bound_buffer(write_target)->buffer, 1, &region);
}

static uint32_t GLAPI vkgl_glCreateProgram(void) {
    return vkgl_new(&vkgl.programs, sizeof(VkglProgram));
}

static uint32_t GLAPI vkgl_glCreateShader(uint32_t type) {
    uint32_t shader = vkgl_new(&vkgl.shaders, sizeof(VkglShader));
    VKGL_SHADER(shader)->type = type;
    return shader;
}

static void GLAPI vkgl_glCullFace(uint32_t mode) {
    vkgl.state.cull_face = mode;
}

static void GLAPI vkgl_glDeleteBuffers(int32_t n, const uint32_t * buffers) {
    vkgl_flush();
    for (int32_t i = 0; i < n; ++i) {
        vkgl_destroy_buffer(VKGL_BUFFER(buffers[i]));
    }
}

static void GLAPI vkgl_glDeleteFramebuffers(int32_t n, const uint32_t * framebuffers) {
    vkgl_flush();
    for (int32_t i = 0; i < n; ++i) {
        if (!framebuffers[i]) {
            continue;
        }
        VkglFramebuffer * framebuffer = VKGL_FRAMEBUFFER(framebuffers[i]);
        for (int32_t j = 0; j < 9; ++j) {
            VkglAttachment * attachment = j < 8 ? &framebuffer->color[j] : &framebuffer->depth;
            if (attachment->view) {
                vkDestroyImageView(vkgl.device, attachment->view, NULL);
            }
        }
        memset(framebuffer, 0, sizeof(VkglFramebuffer));
    }
}

static void GLAPI vkgl_glDeleteProgram(uint32_t program) {
    vkgl_flush();
    VkglProgram * res = VKGL_PROGRAM(program);
    int32_t count = 0;
    for (int32_t i = 0; i < vkgl.pipeline_count; ++i) {
        if (vkgl.pipelines[i].key.program == program) {
            vkDestroyPipeline(vkgl.device, vkgl.pipelines[i].pipeline, NULL);
            continue;
        }
        vkgl.pipelines[count++] = vkgl.pipelines[i];
    }
    vkgl.pipeline_count = count;
    for (int32_t i = 0; i < res->compute_count; ++i) {
        vkDestroyPipeline(vkgl.device, res->compute_pipelines[i], NULL);
    }
    vkDestroyShaderModule(vkgl.device, res->vertex, NULL);
    vkDestroyShaderModule(vkgl.device, res->fragment, NULL);
    vkDestroyShaderModule(vkgl.device, res->compute, NULL);
    memset(res, 0, sizeof(VkglProgram));
}

static void GLAPI vkgl_glDeleteTextures(int32_t n, const uint32_t * textures) {
    vkgl_flush();
    for (int32_t i = 0; i < n; ++i) {
        for (int32_t j = 0; j < 8; ++j) {
            VkglStorageImage * unit = &vkgl.storage_images[j];
            if (unit->image == textures[i]) {
                vkDestroyImageView(vkgl.device, unit->view, NULL);
                memset(unit, 0, sizeof(VkglStorageImage));
            }
        }
        vkgl_destroy_image(VKGL_IMAGE(textures[i]));
    }
}

static void GLAPI vkgl_glDeleteSamplers(int32_t n, const uint32_t * samplers) {
    vkgl_flush();
    for (int32_t i = 0; i < n; ++i) {
        VkglSampler * sampler = VKGL_SAMPLER(samplers[i]);
        vkDestroySampler(vkgl.device, sampler->sampler, NULL);
        memset(sampler, 0, sizeof(VkglSampler));
    }
}

static void GLAPI vkgl_glDeleteShader(uint32_t shader) {
    VkglShader * res = VKGL_SHADER(shader);
    PyMem_Free(res->code);
    memset(res, 0, sizeof(VkglShader));
}

static void GLAPI vkgl_glDeleteVertexArrays(int32_t n, const uint32_t * arrays) {
    for (int32_t i = 0; i < n; ++i) {
        memset(VKGL_VERTEX_ARRAY(arrays[i]), 0, sizeof(VkglVertexArray));
    }
}

static void GLAPI vkgl_glDepthFunc(uint32_t func) {
    vkgl.state.depth_func = func;
}

static void GLAPI vkgl_glDepthMask(uint8_t flag) {
    vkgl.state.depth_write = flag;
}

static void GLAPI vkgl_glDetachShader(uint32_t program, uint32_t shader) {
}

static void vkgl_set_capability(uint32_t cap, uint32_t value) {
    switch (cap) {
        case GL_DEPTH_TEST: vkgl.state.depth_test = value; break;
        case GL_CULL_FACE: vkgl.state.cull = value; break;
        case GL_BLEND: vkgl.state.blend = value; break;
        case GL_STENCIL_TEST: vkgl.state.stencil = value; break;
        case GL_POLYGON_OFFSET_FILL: vkgl.state.polygon_offset = value; break;
        case GL_SCISSOR_TEST: vkgl.scissor_test = (int32_t)value; break;
    }
}

static void GLAPI vkgl_glDisable(uint32_t cap) {
    vkgl_set_capability(cap, 0);
}

static void GLAPI vkgl_glEnable(uint32_t cap) {
    vkgl_set_capability(cap, 1);
}

static void GLAPI vkgl_glDrawArraysInstancedBaseInstance(uint32_t mode, int32_t first, int32_t count, int32_t instances, uint32_t base_instance) {
    vkgl_prepare_draw(mode);
    vkCmdDraw(vkgl.command, (uint32_t)count, (uint32_t)instances, (uint32_t)first, base_instance);
}

static void GLAPI vkgl_glDrawArraysInstanced(uint32_t mode, int32_t first, int32_t count, int32_t instances) {
    vkgl_glDrawArraysInstancedBaseInstance(mode, first, count, instances, 0);
}

static void GLAPI vkgl_glDrawElementsInstancedBaseVertexBaseInstance(uint32_t mode, int32_t count, uint32_t type, const void * indices, int32_t instances, int32_t base_vertex, uint32_t base_instance) {
    vkgl_prepare_draw(mode);
    vkgl_bind_index_buffer(type);
    vkCmdDrawIndexed(vkgl.command, (uint32_t)count, (uint32_t)instances, (uint32_t)((intptr_t)indices / vkgl_index_size(type)), base_vertex, base_instance);
}

static void GLAPI vkgl_glDrawElementsInstanced(uint32_t mode, int32_t count, uint32_t type, const void * indices, int32_t instances) {
    vkgl_glDrawElementsInstancedBaseVertexBaseInstance(mode, count, type, indices, instances, 0, 0);
}

static void GLAPI vkgl_glMultiDrawArraysIndirect(uint32_t mode, const void * indirect, int32_t count, int32_t stride) {
    vkgl_prepare_draw(mode);
    vkCmdDrawIndirect(vkgl.command, VKGL_BUFFER(vkgl.indirect_buffer)->buffer, (uint64_t)(intptr_t)indirect, (uint32_t)count, stride ? (uint32_t)stride : 16);
}

static void GLAPI vkgl_glMultiDrawElementsIndirect(uint32_t mode, uint32_t type, const void * indirect, int32_t count, int32_t stride) {
    vkgl_prepare_draw(mode);
    vkgl_bind_index_buffer(type);
    vkCmdDrawIndexedIndirect(vkgl.command, VKGL_BUFFER(vkgl.indirect_buffer)->buffer, (uint64_t)(intptr_t)indirect, (uint32_t)count, stride ? (uint32_t)stride : 20);
}

static void GLAPI vkgl_glDispatchCompute(uint32_t x, uint32_t y, uint32_t z) {
    VkglProgram * program = VKGL_PROGRAM(vkgl.program);
    VkglLayout * layout = vkgl_layout(vkgl_binding_mask());
    int32_t index = 0;
    while (index < program->compute_count && program->compute_layouts[index] != layout->pipeline_layout) {
        index += 1;
    }
    if (index == program->compute_count) {
        index = program->compute_count < 8 ? program->compute_count++ : 7;
        VkComputePipelineCreateInfo info = {
            .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            .stage = {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_COMPUTE_BIT, .module = program->compute, .pName = "main"},
            .layout = layout->pipeline_layout,
        };
        program->compute_layouts[index] = layout->pipeline_layout;
        vkCreateComputePipelines(vkgl.device, 0, 1, &info, NULL, &program->compute_pipelines[index]);
    }
    vkgl_outside();
    vkCmdBindPipeline(vkgl.command, VK_PIPELINE_BIND_POINT_COMPUTE, program->compute_pipelines[index]);
    vkgl_push_descriptors(VK_PIPELINE_BIND_POINT_COMPUTE, layout->pipeline_layout);
    vkCmdDispatch(vkgl.command, x, y, z);
}

static void GLAPI vkgl_glBindImageTexture(uint32_t index, uint32_t texture, int32_t level, uint8_t layered, int32_t layer, uint32_t access, uint32_t format) {
    VkglStorageImage * unit = &vkgl.storage_images[index];
    if (unit->image == texture && unit->level == level && unit->layered == layered && unit->layer == layer) {
        return;
    }
    if (unit->view) {
        vkgl.garbage = (uint64_t *)PyMem_Realloc(vkgl.garbage, sizeof(uint64_t) * (vkgl.garbage_count + 1));
        vkgl.garbage[vkgl.garbage_count++] = unit->view;
    }
    memset(unit, 0, sizeof(VkglStorageImage));
    VkglImage * image = VKGL_IMAGE(texture);
    if (!texture || !(image->features & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT)) {
        return;
    }
    unit->image = texture;
    unit->level = level;
    unit->layer = layer;
    unit->layered = layered;
    uint32_t view_type = image->target == GL_TEXTURE_CUBE_MAP ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    VkImageViewCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image->image,
        .viewType = layered ? view_type : VK_IMAGE_VIEW_TYPE_2D,
        .format = image->format,
        .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, (uint32_t)level, 1, layered ? 0 : (uint32_t)layer, layered ? (uint32_t)image->layers : 1},
    };
    vkCreateImageView(vkgl.device, &info, NULL, &unit->view);
}

static void GLAPI vkgl_glMemoryBarrier(uint32_t barriers) {
    vkgl_outside();
}

static void GLAPI vkgl_glDrawBuffers(int32_t n, const uint32_t * bufs) {
    VkglFramebuffer * framebuffer = VKGL_FRAMEBUFFER(vkgl.draw_framebuffer);
    framebuffer->color_count = 0;
    for (int32_t i = 0; i < n; ++i) {
        framebuffer->color_count += bufs[i] != GL_NONE;
    }
}

static void GLAPI vkgl_glEnableVertexAttribArray(uint32_t index) {
    VKGL_VERTEX_ARRAY(vkgl.vertex_array)->attribs[index].enabled = 1;
}

static void GLAPI vkgl_glFlush(void) {
    vkgl_flush();
}

static void * GLAPI vkgl_glFenceSync(uint32_t condition, uint32_t flags) {
    return (void *)(intptr_t)(vkgl.submits + 1);
}

static uint32_t GLAPI vkgl_glClientWaitSync(void * sync, uint32_t flags, uint64_t timeout) {
    if (vkgl.submits >= (int64_t)(intptr_t)sync) {
        return GL_ALREADY_SIGNALED;
    }
    vkgl_flush();
    return GL_CONDITION_SATISFIED;
}

static void GLAPI vkgl_glDeleteSync(void * sync) {
}

static void GLAPI vkgl_glFramebufferRenderbuffer(uint32_t target, uint32_t attachment, uint32_t renderbuffer_target, uint32_t renderbuffer) {
    vkgl_attach(target, attachment, renderbuffer, 0, 0);
}

static void GLAPI vkgl_glFramebufferTexture2D(uint32_t target, uint32_t attachment, uint32_t texture_target, uint32_t texture, int32_t level) {
    int32_t face = texture_target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && texture_target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    vkgl_attach(target, attachment, texture, level, face ? (int32_t)(texture_target - GL_TEXTURE_CUBE_MAP_POSITIVE_X) : 0);
}

static void GLAPI vkgl_glFramebufferTextureLayer(uint32_t target, uint32_t attachment, uint32_t texture, int32_t level, int32_t layer) {
    vkgl_attach(target, attachment, texture, level, layer);
}

static void GLAPI vkgl_glGenBuffers(int32_t n, uint32_t * buffers) {
    for (int32_t i = 0; i < n; ++i) {
        buffers[i] = vkgl_new(&vkgl.buffers, sizeof(VkglBuffer));
    }
}

static void GLAPI vkgl_glGenFramebuffers(int32_t n, uint32_t * framebuffers) {
    for (int32_t i = 0; i < n; ++i) {
        framebuffers[i] = vkgl_new(&vkgl.framebuffers, sizeof(VkglFramebuffer));
    }
}

static void GLAPI vkgl_glGenTextures(int32_t n, uint32_t * textures) {
    for (int32_t i = 0; i < n; ++i) {
        textures[i] = vkgl_new(&vkgl.images, sizeof(VkglImage));
    }
}

static void GLAPI vkgl_glGenSamplers(int32_t n, uint32_t * samplers) {
    for (int32_t i = 0; i < n; ++i) {
        samplers[i] = vkgl_new(&vkgl.samplers, sizeof(VkglSampler));
        VkglSampler * sampler = VKGL_SAMPLER(samplers[i]);
        sampler->min_filter = GL_NEAREST_MIPMAP_LINEAR;
        sampler->mag_filter = GL_LINEAR;
        sampler->wrap[0] = GL_REPEAT;
        sampler->wrap[1] = GL_REPEAT;
        sampler->wrap[2] = GL_REPEAT;
        sampler->compare_func = GL_LEQUAL;
        sampler->min_lod = -1000.0f;
        sampler->max_lod = 1000.0f;
        sampler->anisotropy = 1.0f;
    }
}

static void GLAPI vkgl_glGenVertexArrays(int32_t n, uint32_t * arrays) {
    for (int32_t i = 0; i < n; ++i) {
        arrays[i] = vkgl_new(&vkgl.vertex_arrays, sizeof(VkglVertexArray));
    }
}

static void GLAPI vkgl_glGenerateMipmap(uint32_t target) {
    VkglImage * image = vkgl_bound_texture();
    int32_t linear = (image->features & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) != 0;
    for (int32_t level = 1; level < image->levels; ++level) {
        int32_t src_width = image->width >> (level - 1) ? image->width >> (level - 1) : 1;
        int32_t src_height = image->height >> (level - 1) ? image->height >> (level - 1) : 1;
        int32_t dst_width = image->width >> level ? image->width >> level : 1;
        int32_t dst_height = image->height >> level ? image->height >> level : 1;
        VkImageBlit region = {
            {VK_IMAGE_ASPECT_COLOR_BIT, (uint32_t)level - 1, 0, (uint32_t)image->layers},
            {{0, 0, 0}, {src_width, src_height, 1}},
            {VK_IMAGE_ASPECT_COLOR_BIT, (uint32_t)level, 0, (uint32_t)image->layers},
            {{0, 0, 0}, {dst_width, dst_height, 1}},
        };
        vkgl_outside();
        vkCmdBlitImage(vkgl.command, image->image, VK_IMAGE_LAYOUT_GENERAL, image->image, VK_IMAGE_LAYOUT_GENERAL, 1, &region, linear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST);
    }
}

static void GLAPI vkgl_glGetActiveUniform(uint32_t program, uint32_t index, int32_t buf_size, int32_t * length, int32_t * size, uint32_t * type, char * name) {
}

static void GLAPI vkgl_glGetActiveUniformBlockName(uint32_t program, uint32_t index, int32_t buf_size, int32_t * length, char * name) {
}

static void GLAPI vkgl_glGetActiveUniformBlockiv(uint32_t program, uint32_t index, uint32_t pname, int32_t * params) {
}

static void GLAPI vkgl_glGetBufferParameteriv(uint32_t target, uint32_t pname, int32_t * params) {
    *params = (int32_t)vkgl_bound_buffer(target)->size;
}

static void GLAPI vkgl_glGetBufferSubData(uint32_t target, intptr_t offset, intptr_t size, void * data) {
    vkgl_flush();
    memcpy(data, vkgl_bound_buffer(target)->ptr + offset, (size_t)size);
}

static void GLAPI vkgl_glGetIntegerv(uint32_t pname, int32_t * data) {
    switch (pname) {
        case GL_VIEWPORT: memcpy(data, vkgl.viewport, sizeof(vkgl.viewport)); return;
        case GL_MAX_VERTEX_ATTRIBS: *data = 16; return;
        case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: *data = 17; return;
        case GL_MAX_UNIFORM_BUFFER_BINDINGS: *data = 8; return;
        case GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS: *data = 8; return;
        case GL_MAX_UNIFORM_BLOCK_SIZE: *data = 0x10000; return;
        case GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT: *data = 256; return;
        case GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT: *data = 256; return;
        case GL_MAX_IMAGE_UNITS: *data = 8; return;
    }
    *data = 0;
}

static void GLAPI vkgl_glGetProgramInfoLog(uint32_t program, int32_t buf_size, int32_t * length, char * log) {
    snprintf(log, (size_t)buf_size, "invalid SPIR-V or missing shader stages");
}

static void GLAPI vkgl_glGetProgramiv(uint32_t program, uint32_t pname, int32_t * params) {
    *params = pname == GL_LINK_STATUS ? VKGL_PROGRAM(program)->linked : 0;
}

static void GLAPI vkgl_glGetProgramInterfaceiv(uint32_t program, uint32_t interface, uint32_t pname, int32_t * params) {
    *params = 0;
}

static void GLAPI vkgl_glGetProgramResourceName(uint32_t program, uint32_t interface, uint32_t index, int32_t size, int32_t * length, char * name) {
}

static uint32_t GLAPI vkgl_glGetProgramResourceIndex(uint32_t program, uint32_t interface, const char * name) {
    return GL_INVALID_INDEX;
}

static void GLAPI vkgl_glGetShaderInfoLog(uint32_t shader, int32_t buf_size, int32_t * length, char * log) {
    snprintf(log, (size_t)buf_size, VKGL_SHADER(shader)->code ? "invalid SPIR-V" : "the vulkan backend needs SPIR-V shaders, pass bytes");
}

static void GLAPI vkgl_glGetShaderiv(uint32_t shader, uint32_t pname, int32_t * params) {
    *params = VKGL_SHADER(shader)->valid;
}

static const uint8_t * GLAPI vkgl_glGetString(uint32_t name) {
    switch (name) {
        case GL_VENDOR: return (const uint8_t *)"Vulkan";
        case GL_RENDERER: return (const uint8_t *)vkgl.renderer;
        case GL_VERSION: return (const uint8_t *)"Vulkan 1.3";
        case GL_SHADING_LANGUAGE_VERSION: return (const uint8_t *)"SPIR-V";
    }
    return (const uint8_t *)"";
}

static uint32_t GLAPI vkgl_glGetUniformBlockIndex(uint32_t program, const char * name) {
    return GL_INVALID_INDEX;
}

static int32_t GLAPI vkgl_glGetUniformLocation(uint32_t program, const char * name) {
    return -1;
}

static void GLAPI vkgl_glGetUniformfv(uint32_t program, int32_t location, float * params) {
}

static void GLAPI vkgl_glGetUniformiv(uint32_t program, int32_t location, int32_t * params) {
}

static void GLAPI vkgl_glGetUniformuiv(uint32_t program, int32_t location, uint32_t * params) {
}

static uint64_t vkgl_shader_module(VkglShader * shader) {
    VkShaderModuleCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = (size_t)shader->size,
        .pCode = shader->code,
    };
    uint64_t module = 0;
    vkCreateShaderModule(vkgl.device, &info, NULL, &module);
    return module;
}

static void GLAPI vkgl_glLinkProgram(uint32_t program) {
    for (int32_t i = 0; i < 2; ++i) {
        VkglShader * shader = VKGL_SHADER(VKGL_PROGRAM(program)->shaders[i]);
        if (!shader->valid) {
            continue;
        }
        uint64_t module = vkgl_shader_module(shader);
        VkglProgram * res = VKGL_PROGRAM(program);
        switch (shader->type) {
            case GL_VERTEX_SHADER: res->vertex = module; break;
            case GL_FRAGMENT_SHADER: res->fragment = module; break;
            case GL_COMPUTE_SHADER: res->compute = module; break;
        }
    }

    VkglProgram * res = VKGL_PROGRAM(program);
    res->linked = (res->vertex && res->fragment) || res->compute;
}

static void GLAPI vkgl_glPolygonOffset(float factor, float units) {
    vkgl.state.polygon_offset_factor = factor;
    vkgl.state.polygon_offset_units = units;
}

static void GLAPI vkgl_glReadBuffer(uint32_t src) {
    VKGL_FRAMEBUFFER(vkgl.read_framebuffer)->read = src == GL_NONE ? 0 : (int32_t)(src - GL_COLOR_ATTACHMENT0);
}

static void vkgl_copy_image_buffer(int32_t to_image, VkglImage * image, uint32_t level, uint32_t layer, uint32_t layers, int32_t x, int32_t y, int32_t width, int32_t height, const void * pixels) {
    uint32_t row = (uint32_t)((width * image->pixel_size + 3) & ~3);
    VkglBuffer * buffer = vkgl_bound_buffer(to_image ? GL_PIXEL_UNPACK_BUFFER : GL_PIXEL_PACK_BUFFER);
    uint64_t offset = (uint64_t)(intptr_t)pixels;
    if (!buffer->buffer) {
        buffer = &vkgl.staging;
        offset = (uint64_t)vkgl_staging(pixels, (int64_t)row * height * layers);
    }
    VkBufferImageCopy region = {
        offset,
        row / (uint32_t)image->pixel_size,
        0,
        {VK_IMAGE_ASPECT_COLOR_BIT, level, layer, layers},
        {x, y, 0},
        {(uint32_t)width, (uint32_t)height, 1},
    };
    vkgl_outside();
    if (to_image) {
        vkCmdCopyBufferToImage(vkgl.command, buffer->buffer, image->image, VK_IMAGE_LAYOUT_GENERAL, 1, &region);
    } else {
        vkCmdCopyImageToBuffer(vkgl.command, image->image, VK_IMAGE_LAYOUT_GENERAL, buffer->buffer, 1, &region);
    }
}

static void GLAPI vkgl_glReadPixels(int32_t x, int32_t y, int32_t width, int32_t height, uint32_t format, uint32_t type, void * pixels) {
    VkglFramebuffer * framebuffer = VKGL_FRAMEBUFFER(vkgl.read_framebuffer);
    VkglAttachment * attachment = &framebuffer->color[framebuffer->read];
    VkglImage * image = VKGL_IMAGE(attachment->image);
    vkgl_copy_image_buffer(0, image, (uint32_t)attachment->level, (uint32_t)attachment->layer, 1, x, y, width, height, pixels);
}

static void GLAPI vkgl_glRenderbufferStorageMultisample(uint32_t target, int32_t samples, uint32_t internal_format, int32_t width, int32_t height) {
    VkglImage * image = VKGL_IMAGE(vkgl.renderbuffer);
    image->target = GL_RENDERBUFFER;
    vkgl_create_image(image, internal_format, 0, width, height, 1, 1, samples > 1 ? samples : 1, 0);
}

static void GLAPI vkgl_glDeleteRenderbuffers(int32_t n, const uint32_t * renderbuffers) {
    vkgl_glDeleteTextures(n, renderbuffers);
}

static void GLAPI vkgl_glGenRenderbuffers(int32_t n, uint32_t * renderbuffers) {
    vkgl_glGenTextures(n, renderbuffers);
}

static void vkgl_sampler_parameter(uint32_t name, uint32_t pname, float value) {
    VkglSampler * sampler = VKGL_SAMPLER(name);
    if (sampler->sampler) {
        vkgl_flush();
        vkDestroySampler(vkgl.device, sampler->sampler, NULL);
        sampler->sampler = 0;
    }
    switch (pname) {
        case GL_TEXTURE_MIN_FILTER: sampler->min_filter = (uint32_t)value; break;
        case GL_TEXTURE_MAG_FILTER: sampler->mag_filter = (uint32_t)value; break;
        case GL_TEXTURE_WRAP_S: sampler->wrap[0] = (uint32_t)value; break;
        case GL_TEXTURE_WRAP_T: sampler->wrap[1] = (uint32_t)value; break;
        case GL_TEXTURE_WRAP_R: sampler->wrap[2] = (uint32_t)value; break;
        case GL_TEXTURE_COMPARE_MODE: sampler->compare_mode = (uint32_t)value; break;
        case GL_TEXTURE_COMPARE_FUNC: sampler->compare_func = (uint32_t)value; break;
        case GL_TEXTURE_MIN_LOD: sampler->min_lod = value; break;
        case GL_TEXTURE_MAX_LOD: sampler->max_lod = value; break;
        case GL_TEXTURE_LOD_BIAS: sampler->lod_bias = value; break;
        case GL_TEXTURE_MAX_ANISOTROPY: sampler->anisotropy = value; break;
    }
}

static void GLAPI vkgl_glSamplerParameterf(uint32_t sampler, uint32_t pname, float param) {
    vkgl_sampler_parameter(sampler, pname, param);
}

static void GLAPI vkgl_glSamplerParameteri(uint32_t sampler, uint32_t pname, int32_t param) {
    vkgl_sampler_parameter(sampler, pname, (float)param);
}

static void GLAPI vkgl_glShaderSource(uint32_t shader, int32_t count, const char * const * string, const int32_t * length) {
}

static void GLAPI vkgl_glShaderBinary(int32_t count, const uint32_t * shaders, uint32_t format, const void * binary, int32_t length) {
    VkglShader * shader = VKGL_SHADER(shaders[0]);
    shader->code = (uint32_t *)PyMem_Malloc((size_t)length + 4);
    memcpy(shader->code, binary, (size_t)length);
    shader->size = length;
}

static void GLAPI vkgl_glSpecializeShader(uint32_t shader, const char * entry, uint32_t count, const uint32_t * indices, const uint32_t * values) {
    VkglShader * res = VKGL_SHADER(shader);
    res->valid = res->size >= 20 && res->size % 4 == 0 && res->code[0] == 0x07230203;
}

static void GLAPI vkgl_glStencilFuncSeparate(uint32_t face, uint32_t func, int32_t ref, uint32_t mask) {
    for (int32_t i = 0; i < 2; ++i) {
        if (face == GL_FRONT_AND_BACK || face == (i ? GL_BACK : GL_FRONT)) {
            vkgl.state.stencil_func[i] = func;
            vkgl.state.stencil_ref[i] = (uint32_t)ref;
            vkgl.state.stencil_read_mask[i] = mask;
        }
    }
}

static void GLAPI vkgl_glStencilFunc(uint32_t func, int32_t ref, uint32_t mask) {
    vkgl_glStencilFuncSeparate(GL_FRONT_AND_BACK, func, ref, mask);
}

static void GLAPI vkgl_glStencilMask(uint32_t mask) {
    vkgl.state.stencil_write_mask = mask;
}

static void GLAPI vkgl_glStencilOpSeparate(uint32_t face, uint32_t fail, uint32_t depth_fail, uint32_t depth_pass) {
    for (int32_t i = 0; i < 2; ++i) {
        if (face == GL_FRONT_AND_BACK || face == (i ? GL_BACK : GL_FRONT)) {
            vkgl.state.stencil_fail[i] = fail;
            vkgl.state.stencil_depth_fail[i] = depth_fail;
            vkgl.state.stencil_depth_pass[i] = depth_pass;
        }
    }
}

static void GLAPI vkgl_glStencilOp(uint32_t fail, uint32_t depth_fail, uint32_t depth_pass) {
    vkgl_glStencilOpSeparate(GL_FRONT_AND_BACK, fail, depth_fail, depth_pass);
}

static void GLAPI vkgl_glTexImage3D(uint32_t target, int32_t level, int32_t internal_format, int32_t width, int32_t height, int32_t depth, int32_t border, uint32_t format, uint32_t type, const void * pixels) {
    VkglImage * image = vkgl_bound_texture();
    if (image->image) {
        return;
    }
    int32_t cube = target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    vkgl_create_image(image, (uint32_t)internal_format, format, width, height, cube ? 6 : depth, image->max_level + 1, 1, cube);
}

static void GLAPI vkgl_glTexImage2D(uint32_t target, int32_t level, int32_t internal_format, int32_t width, int32_t height, int32_t border, uint32_t format, uint32_t type, const void * pixels) {
    vkgl_glTexImage3D(target, level, internal_format, width, height, 1, border, format, type, pixels);
}

static void GLAPI vkgl_glTexParameteri(uint32_t target, uint32_t pname, int32_t param) {
    if (pname == GL_TEXTURE_MAX_LEVEL) {
        vkgl_bound_texture()->max_level = param;
    }
}

static void GLAPI vkgl_glTexSubImage3D(uint32_t target, int32_t level, int32_t x, int32_t y, int32_t z, int32_t width, int32_t height, int32_t depth, uint32_t format, uint32_t type, const void * pixels) {
    vkgl_copy_image_buffer(1, vkgl_bound_texture(), (uint32_t)level, (uint32_t)z, (uint32_t)depth, x, y, width, height, pixels);
}

static void GLAPI vkgl_glTexSubImage2D(uint32_t target, int32_t level, int32_t x, int32_t y, int32_t width, int32_t height, uint32_t format, uint32_t type, const void * pixels) {
    int32_t face = target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    vkgl_glTexSubImage3D(target, level, x, y, face ? (int32_t)(target - GL_TEXTURE_CUBE_MAP_POSITIVE_X) : 0, width, height, 1, format, type, pixels);
}

static void GLAPI vkgl_glUniform(void) {
}

static void GLAPI vkgl_glUniformBlockBinding(uint32_t program, uint32_t index, uint32_t binding) {
}

static void GLAPI vkgl_glShaderStorageBlockBinding(uint32_t program, uint32_t index, uint32_t binding) {
}

static void GLAPI vkgl_glUseProgram(uint32_t program) {
    vkgl.program = program;
}

static void GLAPI vkgl_glVertexAttribDivisor(uint32_t index, uint32_t divisor) {
    VKGL_VERTEX_ARRAY(vkgl.vertex_array)->attribs[index].rate = divisor ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
}

static void vkgl_vertex_attrib(uint32_t index, int32_t size, uint32_t type, int32_t integer, int32_t stride, const void * pointer) {
    VkglAttrib * attrib = &VKGL_VERTEX_ARRAY(vkgl.vertex_array)->attribs[index];
    attrib->buffer = vkgl.array_buffer;
    attrib->offset = (uint64_t)(intptr_t)pointer;
    for (int32_t i = 0; vkgl_vertex_formats[i][0]; ++i) {
        if (vkgl_vertex_formats[i][0] == type && vkgl_vertex_formats[i][1] == (uint32_t)size && vkgl_vertex_formats[i][2] == (uint32_t)integer) {
            attrib->format = vkgl_vertex_formats[i][3];
        }
    }
    int32_t type_size = type == GL_FLOAT || type == GL_INT || type == GL_UNSIGNED_INT ? 4 : type == GL_UNSIGNED_BYTE || type == GL_BYTE ? 1 : 2;
    attrib->stride = (uint32_t)(stride ? stride : size * type_size);
}

static void GLAPI vkgl_glVertexAttribIPointer(uint32_t index, int32_t size, uint32_t type, int32_t stride, const void * pointer) {
    vkgl_vertex_attrib(index, size, type, 1, stride, pointer);
}

static void GLAPI vkgl_glVertexAttribPointer(uint32_t index, int32_t size, uint32_t type, uint8_t normalized, int32_t stride, const void * pointer) {
    vkgl_vertex_attrib(index, size, type, 0, stride, pointer);
}

static void GLAPI vkgl_glViewport(int32_t x, int32_t y, int32_t width, int32_t height) {
    vkgl.viewport[0] = x;
    vkgl.viewport[1] = y;
    vkgl.viewport[2] = width;
    vkgl.viewport[3] = height;
}

static void GLAPI vkgl_glScissor(int32_t x, int32_t y, int32_t width, int32_t height) {
    vkgl.scissor[0] = x;
    vkgl.scissor[1] = y;
    vkgl.scissor[2] = width;
    vkgl.scissor[3] = height;
}

static int vkgl_has_extension(VkExtensionProperties * extensions, uint32_t count, const char * name) {
    for (uint32_t i = 0; i < count; ++i) {
        if (!strcmp(extensions[i].extensionName, name)) {
            return 1;
        }
    }
    return 0;
}

static int vkgl_create_device(int32_t width, int32_t height, const char ** extra_instance, int32_t extra_instance_count, const char ** extra_device, int32_t extra_device_count) {
#ifdef _WIN32
    void * library = (void *)LoadLibraryA("vulkan-1.dll");
    vkGetInstanceProcAddr = library ? (void *)GetProcAddress((HMODULE)library, "vkGetInstanceProcAddr") : NULL;
#elif defined(__APPLE__)
    void * library = dlopen("libvulkan.1.dylib", RTLD_LAZY);
    library = library ? library : dlopen("libMoltenVK.dylib", RTLD_LAZY);
    vkGetInstanceProcAddr = library ? dlsym(library, "vkGetInstanceProcAddr") : NULL;
#else
    void * library = dlopen("libvulkan.so.1", RTLD_LAZY);
    vkGetInstanceProcAddr = library ? dlsym(library, "vkGetInstanceProcAddr") : NULL;
#endif

    if (!vkGetInstanceProcAddr) {
        PyErr_Format(PyExc_RuntimeError, "cannot load the vulkan library");
        return 0;
    }

    vkCreateInstance = vkGetInstanceProcAddr(NULL, "vkCreateInstance");

#ifdef __APPLE__
    const char * instance_extensions[17] = {"VK_KHR_portability_enumeration"};
    uint32_t instance_extension_count = 1;
    uint32_t instance_flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#else
    const char * instance_extensions[17] = {NULL};
    uint32_t instance_extension_count = 0;
    uint32_t instance_flags = 0;
#endif

    for (int32_t i = 0; i < extra_instance_count; ++i) {
        instance_extensions[instance_extension_count++] = extra_instance[i];
    }

    VkApplicationInfo application = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "moderngl",
        .pEngineName = "moderngl",
        .apiVersion = VK_API_VERSION_1_3,
    };

    VkInstanceCreateInfo instance_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .flags = instance_flags,
        .pApplicationInfo = &application,
        .enabledExtensionCount = instance_extension_count,
        .ppEnabledExtensionNames = instance_extensions,
    };

    if (vkCreateInstance(&instance_info, NULL, &vkgl.instance)) {
        PyErr_Format(PyExc_RuntimeError, "cannot create the vulkan instance");
        return 0;
    }

    const struct {
        const char * name;
        void ** ptr;
    } instance_functions[] = {
        {"vkGetDeviceProcAddr", (void **)&vkGetDeviceProcAddr},
        {"vkEnumeratePhysicalDevices", (void **)&vkEnumeratePhysicalDevices},
        {"vkGetPhysicalDeviceProperties", (void **)&vkGetPhysicalDeviceProperties},
        {"vkGetPhysicalDeviceQueueFamilyProperties", (void **)&vkGetPhysicalDeviceQueueFamilyProperties},
        {"vkGetPhysicalDeviceMemoryProperties", (void **)&vkGetPhysicalDeviceMemoryProperties},
        {"vkGetPhysicalDeviceFeatures2", (void **)&vkGetPhysicalDeviceFeatures2},
        {"vkGetPhysicalDeviceFormatProperties", (void **)&vkGetPhysicalDeviceFormatProperties},
        {"vkEnumerateDeviceExtensionProperties", (void **)&vkEnumerateDeviceExtensionProperties},
        {"vkCreateDevice", (void **)&vkCreateDevice},
        {NULL},
    };

    for (int32_t i = 0; instance_functions[i].name; ++i) {
        *instance_functions[i].ptr = vkGetInstanceProcAddr(vkgl.instance, instance_functions[i].name);
    }

    uint32_t physical_count = 8;
    void * physicals[8] = {0};
    vkEnumeratePhysicalDevices(vkgl.instance, &physical_count, physicals);

    VkPhysicalDeviceProperties properties = {0};
    for (uint32_t i = 0; i < physical_count; ++i) {
        VkPhysicalDeviceProperties candidate = {0};
        vkGetPhysicalDeviceProperties(physicals[i], &candidate);
        int better = !vkgl.physical || (candidate.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
        if (candidate.apiVersion >= VK_API_VERSION_1_3 && better) {
            vkgl.physical = physicals[i];
            properties = candidate;
        }
    }

    if (!vkgl.physical) {
        PyErr_Format(PyExc_RuntimeError, "no vulkan 1.3 device found");
        return 0;
    }

    memcpy(vkgl.renderer, properties.deviceName, sizeof(vkgl.renderer));
    vkGetPhysicalDeviceMemoryProperties(vkgl.physical, &vkgl.memory);

    uint32_t family_count = 16;
    VkQueueFamilyProperties families[16] = {0};
    vkGetPhysicalDeviceQueueFamilyProperties(vkgl.physical, &family_count, families);
    uint32_t family = 0;
    while (family < family_count && (families[family].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) != (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) {
        family += 1;
    }

    uint32_t extension_count = 0;
    vkEnumerateDeviceExtensionProperties(vkgl.physical, NULL, &extension_count, NULL);
    VkExtensionProperties * extensions = (VkExtensionProperties *)PyMem_Malloc(sizeof(VkExtensionProperties) * (extension_count + 1));
    vkEnumerateDeviceExtensionProperties(vkgl.physical, NULL, &extension_count, extensions);

    const char * device_extensions[19] = {"VK_KHR_push_descriptor"};
    uint32_t device_extension_count = 1;
    for (int32_t i = 0; i < extra_device_count; ++i) {
        device_extensions[device_extension_count++] = extra_device[i];
    }
    vkgl.depth_clip_control = vkgl_has_extension(extensions, extension_count, "VK_EXT_depth_clip_control");
    if (vkgl.depth_clip_control) {
        device_extensions[device_extension_count++] = "VK_EXT_depth_clip_control";
    }
    if (vkgl_has_extension(extensions, extension_count, "VK_KHR_portability_subset")) {
        device_extensions[device_extension_count++] = "VK_KHR_portability_subset";
    }
    int push_descriptor = vkgl_has_extension(extensions, extension_count, "VK_KHR_push_descriptor");
    PyMem_Free(extensions);

    if (!push_descriptor) {
        PyErr_Format(PyExc_RuntimeError, "the vulkan device has no VK_KHR_push_descriptor");
        return 0;
    }

    VkPhysicalDeviceDepthClipControlFeaturesEXT depth_clip_control = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT};
    VkPhysicalDeviceVulkan13Features vulkan13 = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = vkgl.depth_clip_control ? &depth_clip_control : NULL};
    VkPhysicalDeviceFeatures2 supported = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &vulkan13};
    vkGetPhysicalDeviceFeatures2(vkgl.physical, &supported);

    VkPhysicalDeviceFeatures2 features = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &vulkan13};
    const int32_t wanted[] = {1, 2, 3, 9, 10, 16, 19, 25, 26, 29};
    for (int32_t i = 0; i < 10; ++i) {
        features.features[wanted[i]] = supported.features[wanted[i]];
    }
    memset(&vulkan13.robustImageAccess, 0, sizeof(uint32_t) * 15);
    vulkan13.dynamicRendering = 1;

    float priority = 1.0f;
    VkDeviceQueueCreateInfo queue_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = family,
        .queueCount = 1,
        .pQueuePriorities = &priority,
    };

    VkDeviceCreateInfo device_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_info,
        .enabledExtensionCount = device_extension_count,
        .ppEnabledExtensionNames = device_extensions,
    };

    if (vkCreateDevice(vkgl.physical, &device_info, NULL, &vkgl.device)) {
        PyErr_Format(PyExc_RuntimeError, "cannot create the vulkan device");
        return 0;
    }

    const struct {
        const char * name;
        void ** ptr;
    } device_functions[] = {
        {"vkGetDeviceQueue", (void **)&vkGetDeviceQueue},
        {"vkCreateCommandPool", (void **)&vkCreateCommandPool},
        {"vkAllocateCommandBuffers", (void **)&vkAllocateCommandBuffers},
        {"vkBeginCommandBuffer", (void **)&vkBeginCommandBuffer},
        {"vkEndCommandBuffer", (void **)&vkEndCommandBuffer},
        {"vkResetCommandBuffer", (void **)&vkResetCommandBuffer},
        {"vkQueueSubmit", (void **)&vkQueueSubmit},
        {"vkQueueWaitIdle", (void **)&vkQueueWaitIdle},
        {"vkCreateBuffer", (void **)&vkCreateBuffer},
        {"vkDestroyBuffer", (void **)&vkDestroyBuffer},
        {"vkGetBufferMemoryRequirements", (void **)&vkGetBufferMemoryRequirements},
        {"vkAllocateMemory", (void **)&vkAllocateMemory},
        {"vkFreeMemory", (void **)&vkFreeMemory},
        {"vkBindBufferMemory", (void **)&vkBindBufferMemory},
        {"vkMapMemory", (void **)&vkMapMemory},
        {"vkCreateImage", (void **)&vkCreateImage},
        {"vkDestroyImage", (void **)&vkDestroyImage},
        {"vkGetImageMemoryRequirements", (void **)&vkGetImageMemoryRequirements},
        {"vkBindImageMemory", (void **)&vkBindImageMemory},
        {"vkCreateImageView", (void **)&vkCreateImageView},
        {"vkDestroyImageView", (void **)&vkDestroyImageView},
        {"vkCreateSampler", (void **)&vkCreateSampler},
        {"vkDestroySampler", (void **)&vkDestroySampler},
        {"vkCreateShaderModule", (void **)&vkCreateShaderModule},
        {"vkDestroyShaderModule", (void **)&vkDestroyShaderModule},
        {"vkCreateDescriptorSetLayout", (void **)&vkCreateDescriptorSetLayout},
        {"vkCreatePipelineLayout", (void **)&vkCreatePipelineLayout},
        {"vkCreateGraphicsPipelines", (void **)&vkCreateGraphicsPipelines},
        {"vkCreateComputePipelines", (void **)&vkCreateComputePipelines},
        {"vkDestroyPipeline", (void **)&vkDestroyPipeline},
        {"vkCmdPipelineBarrier", (void **)&vkCmdPipelineBarrier},
        {"vkCmdCopyBuffer", (void **)&vkCmdCopyBuffer},
        {"vkCmdCopyBufferToImage", (void **)&vkCmdCopyBufferToImage},
        {"vkCmdCopyImageToBuffer", (void **)&vkCmdCopyImageToBuffer},
        {"vkCmdBlitImage", (void **)&vkCmdBlitImage},
        {"vkCmdResolveImage", (void **)&vkCmdResolveImage},
        {"vkCmdClearColorImage", (void **)&vkCmdClearColorImage},
        {"vkCmdClearDepthStencilImage", (void **)&vkCmdClearDepthStencilImage},
        {"vkCmdBeginRendering", (void **)&vkCmdBeginRendering},
        {"vkCmdEndRendering", (void **)&vkCmdEndRendering},
        {"vkCmdBindPipeline", (void **)&vkCmdBindPipeline},
        {"vkCmdSetViewport", (void **)&vkCmdSetViewport},
        {"vkCmdSetScissor", (void **)&vkCmdSetScissor},
        {"vkCmdBindVertexBuffers", (void **)&vkCmdBindVertexBuffers},
        {"vkCmdBindIndexBuffer", (void **)&vkCmdBindIndexBuffer},
        {"vkCmdDraw", (void **)&vkCmdDraw},
        {"vkCmdDrawIndexed", (void **)&vkCmdDrawIndexed},
        {"vkCmdDrawIndirect", (void **)&vkCmdDrawIndirect},
        {"vkCmdDrawIndexedIndirect", (void **)&vkCmdDrawIndexedIndirect},
        {"vkCmdDispatch", (void **)&vkCmdDispatch},
        {"vkCmdPushDescriptorSetKHR", (void **)&vkCmdPushDescriptorSetKHR},
        {NULL},
    };

    for (int32_t i = 0; device_functions[i].name; ++i) {
        *device_functions[i].ptr = vkGetDeviceProcAddr(vkgl.device, device_functions[i].name);
    }

    vkGetDeviceQueue(vkgl.device, family, 0, &vkgl.queue);
    vkgl.family = family;

    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = family,
    };
    vkCreateCommandPool(vkgl.device, &pool_info, NULL, &vkgl.pool);

    VkCommandBufferAllocateInfo command_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vkgl.pool,
        .commandBufferCount = 1,
    };
    vkAllocateCommandBuffers(vkgl.device, &command_info, &vkgl.command);

    VkCommandBufferBeginInfo begin = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    vkBeginCommandBuffer(vkgl.command, &begin);

    VkSamplerCreateInfo sampler_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .maxLod = 0.25f,
    };
    vkCreateSampler(vkgl.device, &sampler_info, NULL, &vkgl.default_sampler);

    VkFormatProperties depth_stencil = {0};
    vkGetPhysicalDeviceFormatProperties(vkgl.physical, VK_FORMAT_D24_UNORM_S8_UINT, &depth_stencil);
    int d24s8 = (depth_stencil.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0;
    vkgl.depth_stencil_format = d24s8 ? VK_FORMAT_D24_UNORM_S8_UINT : VK_FORMAT_D32_SFLOAT_S8_UINT;

    vkgl_create_buffer(&vkgl.staging, 0x400000);

    vkgl_new(&vkgl.buffers, sizeof(VkglBuffer));
    vkgl_new(&vkgl.images, sizeof(VkglImage));
    vkgl_new(&vkgl.samplers, sizeof(VkglSampler));
    vkgl_new(&vkgl.shaders, sizeof(VkglShader));
    vkgl_new(&vkgl.programs, sizeof(VkglProgram));
    vkgl_new(&vkgl.vertex_arrays, sizeof(VkglVertexArray));
    vkgl_new(&vkgl.framebuffers, sizeof(VkglFramebuffer));

    uint32_t screen[2] = {0};
    vkgl_glGenTextures(2, screen);
    VKGL_IMAGE(screen[0])->target = GL_RENDERBUFFER;
    VKGL_IMAGE(screen[1])->target = GL_RENDERBUFFER;
    vkgl_create_image(VKGL_IMAGE(screen[0]), 0x8058, 0, width, height, 1, 1, 1, 0);
    vkgl_create_image(VKGL_IMAGE(screen[1]), 0x88F0, 0, width, height, 1, 1, 1, 0);
    vkgl_attach(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, screen[0], 0, 0);
    vkgl_attach(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, screen[1], 0, 0);
    VKGL_FRAMEBUFFER(0)->color_count = 1;
    vkgl_glViewport(0, 0, width, height);
    vkgl_flush();
    return 1;
}

static void GLAPI vkglHandles(uint64_t * handles) {
    handles[0] = (uint64_t)(uintptr_t)vkgl.instance;
    handles[1] = (uint64_t)(uintptr_t)vkgl.physical;
    handles[2] = (uint64_t)(uintptr_t)vkgl.device;
    handles[3] = (uint64_t)(uintptr_t)vkgl.queue;
    handles[4] = vkgl.family;
}

static uint64_t GLAPI vkglImage(uint32_t texture) {
    return VKGL_IMAGE(texture)->image;
}

#define VKGL(name) {#name, (void **)&name, (void *)vkgl_##name}

static void * vkgl_functions(const char * name) {
    const struct {
        const char * name;
        void ** ptr;
        void * function;
    } functions[] = {
        VKGL(glActiveTexture),
        VKGL(glAttachShader),
        VKGL(glBindBuffer),
        VKGL(glBindBufferBase),
        VKGL(glBindBufferRange),
        VKGL(glBindFramebuffer),
        VKGL(glBindImageTexture),
        VKGL(glBindRenderbuffer),
        VKGL(glBindSampler),
        VKGL(glBindTexture),
        VKGL(glBindVertexArray),
        VKGL(glBlendEquationSeparate),
        VKGL(glBlendFuncSeparate),
        VKGL(glBlitFramebuffer),
        VKGL(glBufferData),
        VKGL(glBufferSubData),
        VKGL(glCheckFramebufferStatus),
        VKGL(glClearBufferfi),
        VKGL(glClearBufferfv),
        VKGL(glClearBufferiv),
        VKGL(glClearBufferuiv),
        VKGL(glClientWaitSync),
        VKGL(glCompileShader),
        VKGL(glCopyBufferSubData),
        VKGL(glCreateProgram),
        VKGL(glCreateShader),
        VKGL(glCullFace),
        VKGL(glDeleteBuffers),
        VKGL(glDeleteFramebuffers),
        VKGL(glDeleteProgram),
        VKGL(glDeleteRenderbuffers),
        VKGL(glDeleteSamplers),
        VKGL(glDeleteShader),
        VKGL(glDeleteSync),
        VKGL(glDeleteTextures),
        VKGL(glDeleteVertexArrays),
        VKGL(glDepthFunc),
        VKGL(glDepthMask),
        VKGL(glDetachShader),
        VKGL(glDisable),
        VKGL(glDispatchCompute),
        VKGL(glDrawArraysInstanced),
        VKGL(glDrawArraysInstancedBaseInstance),
        VKGL(glDrawBuffers),
        VKGL(glDrawElementsInstanced),
        VKGL(glDrawElementsInstancedBaseVertexBaseInstance),
        VKGL(glEnable),
        VKGL(glEnableVertexAttribArray),
        VKGL(glFenceSync),
        VKGL(glFlush),
        VKGL(glFramebufferRenderbuffer),
        VKGL(glFramebufferTexture2D),
        VKGL(glFramebufferTextureLayer),
        VKGL(glGenBuffers),
        VKGL(glGenFramebuffers),
        VKGL(glGenRenderbuffers),
        VKGL(glGenSamplers),
        VKGL(glGenTextures),
        VKGL(glGenVertexArrays),
        VKGL(glGenerateMipmap),
        VKGL(glGetActiveUniform),
        VKGL(glGetActiveUniformBlockName),
        VKGL(glGetActiveUniformBlockiv),
        VKGL(glGetBufferParameteriv),
        VKGL(glGetBufferSubData),
        {"glGetFloatv", (void **)&glGetFloatv, NULL},
        {"glGetIntegeri_v", (void **)&glGetIntegeri_v, NULL},
        VKGL(glGetIntegerv),
        VKGL(glGetProgramInfoLog),
        VKGL(glGetProgramInterfaceiv),
        VKGL(glGetProgramResourceIndex),
        VKGL(glGetProgramResourceName),
        VKGL(glGetProgramiv),
        VKGL(glGetShaderInfoLog),
        VKGL(glGetShaderiv),
        VKGL(glGetString),
        VKGL(glGetUniformBlockIndex),
        VKGL(glGetUniformLocation),
        VKGL(glGetUniformfv),
        VKGL(glGetUniformiv),
        VKGL(glGetUniformuiv),
        {"glIsEnabled", (void **)&glIsEnabled, NULL},
        VKGL(glLinkProgram),
        VKGL(glMemoryBarrier),
        VKGL(glMultiDrawArraysIndirect),
        VKGL(glMultiDrawElementsIndirect),
        VKGL(glPolygonOffset),
        VKGL(glReadBuffer),
        VKGL(glReadPixels),
        VKGL(glRenderbufferStorageMultisample),
        VKGL(glSamplerParameterf),
        VKGL(glSamplerParameteri),
        VKGL(glScissor),
        VKGL(glShaderBinary),
        VKGL(glShaderSource),
        VKGL(glShaderStorageBlockBinding),
        VKGL(glSpecializeShader),
        VKGL(glStencilFunc),
        VKGL(glStencilFuncSeparate),
        VKGL(glStencilMask),
        VKGL(glStencilOp),
        VKGL(glStencilOpSeparate),
        VKGL(glTexImage2D),
        VKGL(glTexImage3D),
        VKGL(glTexParameteri),
        VKGL(glTexSubImage2D),
        VKGL(glTexSubImage3D),
        {"glUniform1fv", (void **)&glUniform1fv, NULL},
        {"glUniform1i", (void **)&glUniform1i, (void *)vkgl_glUniform},
        {"glUniform1iv", (void **)&glUniform1iv, NULL},
        {"glUniform1uiv", (void **)&glUniform1uiv, NULL},
        {"glUniform2fv", (void **)&glUniform2fv, NULL},
        {"glUniform2iv", (void **)&glUniform2iv, NULL},
        {"glUniform2uiv", (void **)&glUniform2uiv, NULL},
        {"glUniform3fv", (void **)&glUniform3fv, NULL},
        {"glUniform3iv", (void **)&glUniform3iv, NULL},
        {"glUniform3uiv", (void **)&glUniform3uiv, NULL},
        {"glUniform4fv", (void **)&glUniform4fv, NULL},
        {"glUniform4iv", (void **)&glUniform4iv, NULL},
        {"glUniform4uiv", (void **)&glUniform4uiv, NULL},
        VKGL(glUniformBlockBinding),
        {"glUniformMatrix2fv", (void **)&glUniformMatrix2fv, NULL},
        {"glUniformMatrix2x3fv", (void **)&glUniformMatrix2x3fv, NULL},
        {"glUniformMatrix2x4fv", (void **)&glUniformMatrix2x4fv, NULL},
        {"glUniformMatrix3fv", (void **)&glUniformMatrix3fv, NULL},
        {"glUniformMatrix3x2fv", (void **)&glUniformMatrix3x2fv, NULL},
        {"glUniformMatrix3x4fv", (void **)&glUniformMatrix3x4fv, NULL},
        {"glUniformMatrix4fv", (void **)&glUniformMatrix4fv, NULL},
        {"glUniformMatrix4x2fv", (void **)&glUniformMatrix4x2fv, NULL},
        {"glUniformMatrix4x3fv", (void **)&glUniformMatrix4x3fv, NULL},
        VKGL(glUseProgram),
        VKGL(glVertexAttribDivisor),
        VKGL(glVertexAttribIPointer),
        VKGL(glVertexAttribPointer),
        VKGL(glViewport),
        {"vkGetInstanceProcAddr", NULL, (void *)vkGetInstanceProcAddr},
        {"vkglHandles", NULL, (void *)vkglHandles},
        {"vkglImage", NULL, (void *)vkglImage},
        {NULL},
    };

    for (int32_t i = 0; functions[i].name; ++i) {
        if (!name && functions[i].ptr) {
            *functions[i].ptr = functions[i].function;
        }
        if (name && !strcmp(name, functions[i].name)) {
            return functions[i].function;
        }
    }
    return NULL;
}

#undef VKGL

static int load_vulkan(PyObject * loader) {
    int32_t width = 1;
    int32_t height = 1;
    PyObject * size = PyObject_GetAttrString(loader, "size");
    PyErr_Clear();
    if (size && !PyArg_ParseTuple(size, "ii", &width, &height)) {
        Py_DECREF(size);
        PyErr_Format(PyExc_TypeError, "the loader size must be a tuple of 2 ints");
        return 0;
    }
    Py_XDECREF(size);

    const char * names[2][16] = {{0}};
    int32_t counts[2] = {0};
    PyObject * lists[2] = {PyObject_GetAttrString(loader, "instance_extensions"), NULL};
    PyErr_Clear();
    lists[1] = PyObject_GetAttrString(loader, "device_extensions");
    PyErr_Clear();

    for (int32_t i = 0; i < 2; ++i) {
        PyObject * seq = lists[i] ? PySequence_Fast(lists[i], "") : NULL;
        PyErr_Clear();
        Py_XSETREF(lists[i], seq);
        for (int32_t j = 0; seq && j < PySequence_Fast_GET_SIZE(seq) && j < 16; ++j) {
            PyObject * item = PySequence_Fast_GET_ITEM(seq, j);
            names[i][counts[i]++] = PyUnicode_Check(item) ? PyUnicode_AsUTF8(item) : "";
        }
    }

    int ok = vkgl.device || vkgl_create_device(width, height, names[0], counts[0], names[1], counts[1]);
    Py_XDECREF(lists[0]);
    Py_XDECREF(lists[1]);

    if (!ok) {
        return 0;
    }


    vkgl_functions(NULL);
    Py_CLEAR(gl_loader);
    gl_backend = "vulkan";
    gl_loaded = 1;
    return 1;
}
