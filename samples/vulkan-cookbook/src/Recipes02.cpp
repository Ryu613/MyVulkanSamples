#include <Windows.h>
#include "Recipes.hpp"
#define VK_USE_PLATFORM_WIN32_KHR
#define VOLK_IMPLEMENTATION
#define VK_NO_PROTOTYPES
#include "volk.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <cassert>

#define VK_CHK(func)                                                                   \
  {                                                                                    \
    const VkResult result = func;                                                      \
    if (result != VK_SUCCESS) {                                                        \
      std::cerr << "Error calling function " << #func << " at " << __FILE__ << ":"     \
                << __LINE__ << std::endl;                                              \
      assert(false);                                                                   \
    }                                                                                  \
  }

namespace cook {
	struct WindowParameters {
		HINSTANCE HInstance;
		HWND HWnd;
	};

	bool doRecipeChapter2() {
		// use volk to init vulkan library instead of copying CH1's code
		if (volkInitialize()) {
			throw std::runtime_error("volk init failed");
		}
		std::vector<char const*> desired_extensions{ 
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME
		};
		VkApplicationInfo app_info{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "MyVulkanSamples",
			.applicationVersion = VK_MAKE_VERSION(1,3,250),
			.pEngineName = "MyVulkanSamples",
			.engineVersion = VK_MAKE_VERSION(1,0,0),
			.apiVersion = VK_MAKE_VERSION(1,0,0)
		};
		VkInstanceCreateInfo instance_create_info{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.pApplicationInfo = &app_info,
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = nullptr,
			.enabledExtensionCount = static_cast<uint32_t>(desired_extensions.size()),
			.ppEnabledExtensionNames = desired_extensions.data()
		};
		// create vulkan instance
		VkInstance instance = VK_NULL_HANDLE;
		VK_CHK(vkCreateInstance(&instance_create_info, nullptr, &instance));

		volkLoadInstance(instance);

		// register windows class first!
		WindowParameters wParam = {};
		WNDCLASSEX wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = DefWindowProc; // 默认窗口过程
		wc.hInstance = wParam.HInstance;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.lpszClassName = "VulkanCookbook"; // 窗口类名

		if (!RegisterClassEx(&wc)) {
			std::cerr << "Failed to register window class!" << std::endl;
			return false;
		}
		// create windows native window
		wParam.HInstance = GetModuleHandle(nullptr);
		std::string name("Vulkan - cookbook");
		HWND nativeWindow = CreateWindow("VulkanCookbook", name.c_str(), WS_OVERLAPPEDWINDOW, 0, 0, 1024, 768, nullptr, nullptr, wParam.HInstance, nullptr);
		wParam.HWnd = nativeWindow;
		if (!wParam.HWnd) {
			std::cout << "window create failed" << std::endl;
			return false;
		}
		VkWin32SurfaceCreateInfoKHR surface_create_info {
			.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.hinstance = wParam.HInstance,
			.hwnd = wParam.HWnd
		};
		VkSurfaceKHR presentation_surface = VK_NULL_HANDLE;
		VK_CHK(vkCreateWin32SurfaceKHR(instance, &surface_create_info, nullptr, &presentation_surface))
		// copy from repipes01 and modified to choose physical device which support surface presentation
		uint32_t devices_count = 0;
		VK_CHK(vkEnumeratePhysicalDevices(instance, &devices_count, nullptr));
		std::vector<VkPhysicalDevice> available_devices(devices_count);
		VK_CHK(vkEnumeratePhysicalDevices(instance, &devices_count, available_devices.data()));

		uint32_t queue_family_index = -1;
		VkPhysicalDevice chosen_physical_device;
		for (const auto& physical_device : available_devices) {
			uint32_t queue_families_count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, nullptr);
			if (queue_families_count == 0) {
				std::cout << "couldn't get the number of queue families. " << std::endl;
				return false;
			}
			std::vector<VkQueueFamilyProperties> queue_families(queue_families_count);
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, queue_families.data());
			if (queue_families_count == 0) {
				std::cout << "couldn't get properties of queue families. " << std::endl;
				return false;
			}
			VkQueueFlags desired_capabilities = VK_QUEUE_GRAPHICS_BIT;
			for (uint32_t index = 0; index < queue_families_count; ++index) {
				if (queue_families[index].queueCount > 0) {
					VkBool32 supports_present;
					vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, index, presentation_surface, &supports_present);
					if ((queue_families[index].queueFlags & desired_capabilities) && supports_present) {
						queue_family_index = index;
						break;
					}
				}
			}
			// choose first sufficient physical device
			if (queue_family_index == -1) {
				// skip this insuficient physical device;
				continue;
			}
			// thoose this device
			chosen_physical_device = physical_device;
		}

		if (queue_family_index == -1) {
			std::cout << "no sufficient physical device !" << std::endl;
			return false;
		}
		std::vector<const char*> required_device_extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		struct QueueInfo {
			// chosen family index
			uint32_t familyIndex;
			// each queue's priority, size() means number of queues
			std::vector<float> priorities;
		};
		QueueInfo queue_info{
			.familyIndex = queue_family_index,
			.priorities = {1.f}
		};
		VkDeviceQueueCreateInfo queue_create_info{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueFamilyIndex = queue_info.familyIndex,
			.queueCount = static_cast<uint32_t>(queue_info.priorities.size()),
			.pQueuePriorities = queue_info.priorities.data()
		};
		VkDeviceCreateInfo device_create_info{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &queue_create_info,
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = nullptr,
			.enabledExtensionCount = static_cast<uint32_t>(required_device_extensions.size()),
			.ppEnabledExtensionNames = required_device_extensions.data()
		};

		VkDevice logical_device;
		VK_CHK(vkCreateDevice(chosen_physical_device, &device_create_info, nullptr, &logical_device));
		volkLoadDevice(logical_device);

		/**
		* surface present_mode fifo(VK_PRESENT_MODE_FIFO_KHR) must be supported by all implementations
		* omitted
		*/

		/**
		* getting capabilities of presentation surface
		* - min/max allowed number of swapchain images
		* - min/max, current extent of a presentation surface
		* - supported image transformations
		* - max number of supported image layers
		* - supported usages
		* - supported compositions of a surface's alpha value
		*/
		VkSurfaceCapabilitiesKHR surface_capabilities;
		VK_CHK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(chosen_physical_device, presentation_surface, &surface_capabilities))

		/**
		* select a number of swapchain images
		*/
		uint32_t number_of_images = surface_capabilities.minImageCount + 1;
		// clamp imageCount
		if (surface_capabilities.maxImageCount > 0 && surface_capabilities.maxImageCount < number_of_images) {
			// limited max allowed number of created images
			number_of_images = surface_capabilities.maxImageCount;
		}

		/**
		* chose size of swapchain images
		*/
		VkExtent2D size_of_images = {};
		if (surface_capabilities.currentExtent.width == 0xFFFFFFFF) {
			// means size of images determines size of window
			size_of_images.width = std::clamp(
				(uint32_t)1024,
				surface_capabilities.minImageExtent.width,
				surface_capabilities.maxImageExtent.width);
			size_of_images.height = std::clamp(
				(uint32_t)768,
				surface_capabilities.minImageExtent.height,
				surface_capabilities.maxImageExtent.height);
		}
		else {
			size_of_images = surface_capabilities.currentExtent;
		}

		/**
		* select usage of swapchain images
		* default is VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
		* omitted
		*/
		
		/**
		* select swapchain images' transformation
		* VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR by default
		* omitted
		*/

		/**
		* select image format and colorspace
		* VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
		* omitted
		*/

		/**
		* create a swapchain
		*/
		VkSwapchainCreateInfoKHR swapchain_create_info{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.surface = presentation_surface,
			.minImageCount = number_of_images,
			.imageFormat = VK_FORMAT_R8G8B8A8_UNORM,
			.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
			.imageExtent = size_of_images,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = VK_PRESENT_MODE_FIFO_KHR,
			.clipped = VK_TRUE,
			.oldSwapchain = nullptr
		};
		VkSwapchainKHR swapchain;
		VK_CHK(vkCreateSwapchainKHR(logical_device, &swapchain_create_info, nullptr, &swapchain))
		// then destroy old swapchain
		return true;
	}
}