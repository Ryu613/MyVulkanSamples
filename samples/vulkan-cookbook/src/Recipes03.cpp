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
	/**
	* command buffers and synchronization
	*/
	bool doRecipeChapter3() {
		// reuse ch2's code, create device
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
		uint32_t queue_family_index = -1;
		VkPhysicalDevice chosen_physical_device;
		uint32_t devices_count = 0;
		VK_CHK(vkEnumeratePhysicalDevices(instance, &devices_count, nullptr));
		std::vector<VkPhysicalDevice> available_devices(devices_count);
		VK_CHK(vkEnumeratePhysicalDevices(instance, &devices_count, available_devices.data()));
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
					if (queue_families[index].queueFlags & desired_capabilities) {
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
			break;
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
		* create command pool
		* 
		* can create separate command pools for each thread
		*/
		VkCommandPoolCreateInfo command_pool_create_info{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = queue_family_index
		};
		VkCommandPool command_pool;
		VK_CHK(vkCreateCommandPool(logical_device, &command_pool_create_info, nullptr, &command_pool));

		/**
		* command buffer allocate
		*/
		VkCommandBufferAllocateInfo command_buffer_allocate_info{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = command_pool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1
		};
		std::vector<VkCommandBuffer> command_buffers(1);
		VK_CHK(vkAllocateCommandBuffers(logical_device, &command_buffer_allocate_info, command_buffers.data()));
		
		/**
		* begin command buffer recording
		*/
		VkCommandBuffer command_buffer = command_buffers[0];
		// only use once and then reset
		VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		// if secondary buffer, should initialize
		VkCommandBufferInheritanceInfo* secondary_command_buffer_info = nullptr;
		VkCommandBufferBeginInfo command_buffer_begin_info{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = usage,
			.pInheritanceInfo = secondary_command_buffer_info,
		};
		VK_CHK(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));

		/**
		* stop command buffer recording
		*/
		VK_CHK(vkEndCommandBuffer(command_buffer));

		/**
		* reset command buffer
		*/
		usage = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		VkCommandBufferResetFlags release_resources = VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT;
		VK_CHK(vkResetCommandBuffer(command_buffer, release_resources));

		/**
		* create semaphore
		*/
		VkSemaphoreCreateInfo semaphore_create_info{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0
		};
		VkSemaphore semaphore;
		VK_CHK(vkCreateSemaphore(logical_device, &semaphore_create_info, nullptr, &semaphore));

		/**
		* create fence
		*/
		VkFenceCreateInfo fence_create_info{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0u, // unsignaled
		};
		VkFence fence;
		VK_CHK(vkCreateFence(logical_device, &fence_create_info, nullptr, &fence));
		/**
		* waiting for fences
		*/
		std::vector<VkFence> fences;
		// wait for all fences in "fences" are signaled, otherwise one of them signaled
		VkBool32 wait_for_all = VK_TRUE;
		// in nanosec equal to 5sec
		uint64_t timeout = 5000000000;
		fences.push_back(fence);
 		VkResult result = vkWaitForFences(logical_device, static_cast<uint32_t>(fences.size()), fences.data(), wait_for_all, timeout);
		if (result) {
			std::cout << "Waiting on fence failed." << std::endl;
			return false;
		}

		/**
		* reset fence(can batch reset in vector)
		*/
		VK_CHK(vkResetFences(logical_device, static_cast<uint32_t>(fences.size()), &fences[0]));

		/**
		* submit command buffers to a queue
		*/
		struct WaitSemaphoreInfo {
			VkSemaphore Semaphore;
			VkPipelineStageFlags WaitingStage;
		};
		VkQueue queue;
		vkGetDeviceQueue(logical_device, queue_family_index, 0u, &queue);
		// commands should wait for semaphores to be signaled
		std::vector<VkSemaphore> wait_semaphore_handles;
		// queue should wait at which pipeline stages
		std::vector<VkPipelineStageFlags> wait_semaphore_stages;
		// command buffers which should be submitted to the selected queue
		std::vector<VkCommandBuffer> command_buffers;
		// store all semaphores' handles which should be signaled in that command buffers finished
		std::vector<VkSemaphore> signal_semaphores;
		VkSubmitInfo submit_info{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,
			.waitSemaphoreCount = static_cast<uint32_t>(wait_semaphore_handles.size()),
			.pWaitSemaphores = wait_semaphore_handles.data(),
			.pWaitDstStageMask = wait_semaphore_stages.data(),
			.commandBufferCount = static_cast<uint32_t>(command_buffers.size()),
			.pCommandBuffers = command_buffers.data(),
			.signalSemaphoreCount = static_cast<uint32_t>(signal_semaphores.size()),
			.pSignalSemaphores = signal_semaphores.data()
		};
		VK_CHK(vkQueueSubmit(queue, 1, &submit_info, fence));

		/**
		* Sync two command buffers
		*/
		VkQueue& first_queue = queue;
		return true;
	}
}