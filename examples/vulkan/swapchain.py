import ctypes

import moderngl
import pygame

u32 = ctypes.c_uint32
u64 = ctypes.c_uint64
i32 = ctypes.c_int32
ptr = ctypes.c_void_p


class XlibSurfaceCreateInfo(ctypes.Structure):
    _fields_ = [('sType', u32), ('pNext', ptr), ('flags', u32), ('dpy', ptr), ('window', ctypes.c_ulong)]


class WaylandSurfaceCreateInfo(ctypes.Structure):
    _fields_ = [('sType', u32), ('pNext', ptr), ('flags', u32), ('display', ptr), ('surface', ptr)]


class SurfaceCapabilities(ctypes.Structure):
    _fields_ = [
        ('minImageCount', u32),
        ('maxImageCount', u32),
        ('currentExtent', u32 * 2),
        ('minImageExtent', u32 * 2),
        ('maxImageExtent', u32 * 2),
        ('maxImageArrayLayers', u32),
        ('supportedTransforms', u32),
        ('currentTransform', u32),
        ('supportedCompositeAlpha', u32),
        ('supportedUsageFlags', u32),
    ]


class SurfaceFormat(ctypes.Structure):
    _fields_ = [('format', u32), ('colorSpace', u32)]


class SwapchainCreateInfo(ctypes.Structure):
    _fields_ = [
        ('sType', u32),
        ('pNext', ptr),
        ('flags', u32),
        ('surface', u64),
        ('minImageCount', u32),
        ('imageFormat', u32),
        ('imageColorSpace', u32),
        ('imageExtent', u32 * 2),
        ('imageArrayLayers', u32),
        ('imageUsage', u32),
        ('imageSharingMode', u32),
        ('queueFamilyIndexCount', u32),
        ('pQueueFamilyIndices', ptr),
        ('preTransform', u32),
        ('compositeAlpha', u32),
        ('presentMode', u32),
        ('clipped', u32),
        ('oldSwapchain', u64),
    ]


class CommandPoolCreateInfo(ctypes.Structure):
    _fields_ = [('sType', u32), ('pNext', ptr), ('flags', u32), ('queueFamilyIndex', u32)]


class CommandBufferAllocateInfo(ctypes.Structure):
    _fields_ = [('sType', u32), ('pNext', ptr), ('commandPool', u64), ('level', u32), ('commandBufferCount', u32)]


class CommandBufferBeginInfo(ctypes.Structure):
    _fields_ = [('sType', u32), ('pNext', ptr), ('flags', u32), ('pInheritanceInfo', ptr)]


class FenceCreateInfo(ctypes.Structure):
    _fields_ = [('sType', u32), ('pNext', ptr), ('flags', u32)]


class SubresourceRange(ctypes.Structure):
    _fields_ = [('aspectMask', u32), ('baseMipLevel', u32), ('levelCount', u32), ('baseArrayLayer', u32), ('layerCount', u32)]


class SubresourceLayers(ctypes.Structure):
    _fields_ = [('aspectMask', u32), ('mipLevel', u32), ('baseArrayLayer', u32), ('layerCount', u32)]


class ImageMemoryBarrier(ctypes.Structure):
    _fields_ = [
        ('sType', u32),
        ('pNext', ptr),
        ('srcAccessMask', u32),
        ('dstAccessMask', u32),
        ('oldLayout', u32),
        ('newLayout', u32),
        ('srcQueueFamilyIndex', u32),
        ('dstQueueFamilyIndex', u32),
        ('image', u64),
        ('subresourceRange', SubresourceRange),
    ]


class ImageBlit(ctypes.Structure):
    _fields_ = [('srcSubresource', SubresourceLayers), ('srcOffsets', i32 * 6), ('dstSubresource', SubresourceLayers), ('dstOffsets', i32 * 6)]


class SubmitInfo(ctypes.Structure):
    _fields_ = [
        ('sType', u32),
        ('pNext', ptr),
        ('waitSemaphoreCount', u32),
        ('pWaitSemaphores', ptr),
        ('pWaitDstStageMask', ptr),
        ('commandBufferCount', u32),
        ('pCommandBuffers', ctypes.POINTER(ptr)),
        ('signalSemaphoreCount', u32),
        ('pSignalSemaphores', ptr),
    ]


class PresentInfo(ctypes.Structure):
    _fields_ = [
        ('sType', u32),
        ('pNext', ptr),
        ('waitSemaphoreCount', u32),
        ('pWaitSemaphores', ptr),
        ('swapchainCount', u32),
        ('pSwapchains', ctypes.POINTER(u64)),
        ('pImageIndices', ctypes.POINTER(u32)),
        ('pResults', ptr),
    ]


FUNCTIONS = {
    'vkCreateXlibSurfaceKHR': (i32, ptr, ptr, ptr, ctypes.POINTER(u64)),
    'vkCreateWaylandSurfaceKHR': (i32, ptr, ptr, ptr, ctypes.POINTER(u64)),
    'vkGetPhysicalDeviceSurfaceSupportKHR': (i32, ptr, u32, u64, ctypes.POINTER(u32)),
    'vkGetPhysicalDeviceSurfaceCapabilitiesKHR': (i32, ptr, u64, ctypes.POINTER(SurfaceCapabilities)),
    'vkGetPhysicalDeviceSurfaceFormatsKHR': (i32, ptr, u64, ctypes.POINTER(u32), ptr),
    'vkCreateSwapchainKHR': (i32, ptr, ctypes.POINTER(SwapchainCreateInfo), ptr, ctypes.POINTER(u64)),
    'vkGetSwapchainImagesKHR': (i32, ptr, u64, ctypes.POINTER(u32), ptr),
    'vkAcquireNextImageKHR': (i32, ptr, u64, u64, u64, u64, ctypes.POINTER(u32)),
    'vkQueuePresentKHR': (i32, ptr, ctypes.POINTER(PresentInfo)),
    'vkCreateCommandPool': (i32, ptr, ctypes.POINTER(CommandPoolCreateInfo), ptr, ctypes.POINTER(u64)),
    'vkAllocateCommandBuffers': (i32, ptr, ctypes.POINTER(CommandBufferAllocateInfo), ctypes.POINTER(ptr)),
    'vkBeginCommandBuffer': (i32, ptr, ctypes.POINTER(CommandBufferBeginInfo)),
    'vkEndCommandBuffer': (i32, ptr),
    'vkCmdPipelineBarrier': (None, ptr, u32, u32, u32, u32, ptr, u32, ptr, u32, ptr),
    'vkCmdBlitImage': (None, ptr, u64, u32, u64, u32, u32, ctypes.POINTER(ImageBlit), u32),
    'vkQueueSubmit': (i32, ptr, u32, ctypes.POINTER(SubmitInfo), u64),
    'vkQueueWaitIdle': (i32, ptr),
    'vkCreateFence': (i32, ptr, ctypes.POINTER(FenceCreateInfo), ptr, ctypes.POINTER(u64)),
    'vkWaitForFences': (i32, ptr, u32, ctypes.POINTER(u64), u32, u64),
    'vkResetFences': (i32, ptr, u32, ctypes.POINTER(u64)),
}


def instance_extensions():
    surface = 'VK_KHR_wayland_surface' if pygame.display.get_driver() == 'wayland' else 'VK_KHR_xlib_surface'
    return ['VK_KHR_surface', surface]


def capsule(info, name):
    get_pointer = ctypes.pythonapi.PyCapsule_GetPointer
    get_pointer.restype = ptr
    get_pointer.argtypes = [ctypes.py_object, ctypes.c_char_p]
    return get_pointer(info[name], name.encode())


class Swapchain:
    def __init__(self, ctx, size):
        handles = (u64 * 5)()
        ctypes.CFUNCTYPE(None, ctypes.c_void_p)(moderngl.get_proc_address('vkglHandles'))(handles)
        get_proc = ctypes.CFUNCTYPE(ptr, ptr, ctypes.c_char_p)(moderngl.get_proc_address('vkGetInstanceProcAddr'))
        self.get_image = ctypes.CFUNCTYPE(u64, u32)(moderngl.get_proc_address('vkglImage'))
        self.instance = ptr(handles[0])
        self.physical = ptr(handles[1])
        self.device = ptr(handles[2])
        self.queue = ptr(handles[3])
        queue_family = handles[4]
        self.ctx = ctx
        self.size = size
        self.vk = {}
        for name, (restype, *args) in FUNCTIONS.items():
            address = get_proc(self.instance, name.encode())
            self.vk[name] = ctypes.CFUNCTYPE(restype, *args)(address) if address else None

        vk = self.vk
        info = pygame.display.get_wm_info()
        self.surface = u64(0)
        if pygame.display.get_driver() == 'wayland':
            create = WaylandSurfaceCreateInfo(sType=1000006000, display=capsule(info, 'display'), surface=capsule(info, 'surface'))
            vk['vkCreateWaylandSurfaceKHR'](self.instance, ctypes.byref(create), None, ctypes.byref(self.surface))
        else:
            create = XlibSurfaceCreateInfo(sType=1000004000, dpy=capsule(info, 'display'), window=info['window'])
            vk['vkCreateXlibSurfaceKHR'](self.instance, ctypes.byref(create), None, ctypes.byref(self.surface))

        supported = u32(0)
        vk['vkGetPhysicalDeviceSurfaceSupportKHR'](self.physical, queue_family, self.surface, ctypes.byref(supported))
        if not supported.value:
            raise RuntimeError('the vulkan queue cannot present to this window')

        caps = SurfaceCapabilities()
        vk['vkGetPhysicalDeviceSurfaceCapabilitiesKHR'](self.physical, self.surface, ctypes.byref(caps))
        count = u32(0)
        vk['vkGetPhysicalDeviceSurfaceFormatsKHR'](self.physical, self.surface, ctypes.byref(count), None)
        formats = (SurfaceFormat * count.value)()
        vk['vkGetPhysicalDeviceSurfaceFormatsKHR'](self.physical, self.surface, ctypes.byref(count), formats)
        unorm = [f for f in formats if f.format in (37, 44)]
        surface_format = unorm[0] if unorm else formats[0]

        image_count = caps.minImageCount + 1
        if caps.maxImageCount:
            image_count = min(image_count, caps.maxImageCount)

        create = SwapchainCreateInfo(
            sType=1000001000,
            surface=self.surface,
            minImageCount=image_count,
            imageFormat=surface_format.format,
            imageColorSpace=surface_format.colorSpace,
            imageExtent=(u32 * 2)(*size),
            imageArrayLayers=1,
            imageUsage=0x2,
            preTransform=caps.currentTransform,
            compositeAlpha=1 if caps.supportedCompositeAlpha & 1 else caps.supportedCompositeAlpha & -caps.supportedCompositeAlpha,
            presentMode=2,
            clipped=1,
        )
        self.swapchain = u64(0)
        vk['vkCreateSwapchainKHR'](self.device, ctypes.byref(create), None, ctypes.byref(self.swapchain))

        count = u32(0)
        vk['vkGetSwapchainImagesKHR'](self.device, self.swapchain, ctypes.byref(count), None)
        self.images = (u64 * count.value)()
        vk['vkGetSwapchainImagesKHR'](self.device, self.swapchain, ctypes.byref(count), self.images)

        pool = u64(0)
        create = CommandPoolCreateInfo(sType=39, flags=0x2, queueFamilyIndex=queue_family)
        vk['vkCreateCommandPool'](self.device, ctypes.byref(create), None, ctypes.byref(pool))
        self.command = ptr(0)
        allocate = CommandBufferAllocateInfo(sType=40, commandPool=pool, commandBufferCount=1)
        vk['vkAllocateCommandBuffers'](self.device, ctypes.byref(allocate), ctypes.byref(self.command))
        self.fence = u64(0)
        vk['vkCreateFence'](self.device, ctypes.byref(FenceCreateInfo(sType=8)), None, ctypes.byref(self.fence))

    def barrier(self, image, old_layout, new_layout):
        barrier = ImageMemoryBarrier(
            sType=45,
            srcAccessMask=0x10000,
            dstAccessMask=0x18000,
            oldLayout=old_layout,
            newLayout=new_layout,
            srcQueueFamilyIndex=0xFFFFFFFF,
            dstQueueFamilyIndex=0xFFFFFFFF,
            image=image,
            subresourceRange=SubresourceRange(aspectMask=1, levelCount=1, layerCount=1),
        )
        self.vk['vkCmdPipelineBarrier'](self.command, 0x10000, 0x10000, 0, 0, None, 0, None, 1, ctypes.byref(barrier))

    def present(self, texture):
        vk = self.vk
        self.ctx.end_frame()

        index = u32(0)
        vk['vkAcquireNextImageKHR'](self.device, self.swapchain, 0xFFFFFFFFFFFFFFFF, 0, self.fence, ctypes.byref(index))
        vk['vkWaitForFences'](self.device, 1, ctypes.byref(self.fence), 1, 0xFFFFFFFFFFFFFFFF)
        vk['vkResetFences'](self.device, 1, ctypes.byref(self.fence))
        image = self.images[index.value]

        width, height = self.size
        layers = SubresourceLayers(aspectMask=1, layerCount=1)
        blit = ImageBlit(
            srcSubresource=layers,
            srcOffsets=(i32 * 6)(0, height, 0, width, 0, 1),
            dstSubresource=layers,
            dstOffsets=(i32 * 6)(0, 0, 0, width, height, 1),
        )

        vk['vkBeginCommandBuffer'](self.command, ctypes.byref(CommandBufferBeginInfo(sType=42, flags=1)))
        self.barrier(image, 0, 7)
        vk['vkCmdBlitImage'](self.command, self.get_image(texture.glo), 1, image, 7, 1, ctypes.byref(blit), 0)
        self.barrier(image, 7, 1000001002)
        vk['vkEndCommandBuffer'](self.command)

        commands = (ptr * 1)(self.command)
        vk['vkQueueSubmit'](self.queue, 1, ctypes.byref(SubmitInfo(sType=4, commandBufferCount=1, pCommandBuffers=commands)), 0)
        vk['vkQueueWaitIdle'](self.queue)

        swapchains = (u64 * 1)(self.swapchain)
        indices = (u32 * 1)(index.value)
        vk['vkQueuePresentKHR'](self.queue, ctypes.byref(PresentInfo(sType=1000001001, swapchainCount=1, pSwapchains=swapchains, pImageIndices=indices)))
