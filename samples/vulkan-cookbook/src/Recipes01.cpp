#include "Recipes.hpp"

namespace cook {
	bool doRecipeChapter1() {

		/**
		* load vulkan library
		*
		* in modern way, generally this should be done by volk or vulkan-hpp
		* (volk & vulkan-hpp has been included in Vulkan SDK)
		* but now we are manually load library and functions
		*
		* @see Volk <vulkan/volk.h>
		* @see Vulkan-Hpp <vulkan/vulkan.hpp>
		*/

		// if cross-platorm support needed, use macro to determine which library to load;
		// only support windows platform for now
		HMODULE vulkan_library = LoadLibrary("vulkan-1.dll");
		if (vulkan_library == nullptr) {
			std::cout << " could not connect with a Vulkan Runtime Library" << std::endl;
			return false;
		}

		/**
		* load vulkan functions
		*/
		if (!cook::loadVulkanFunction(vulkan_library)) {
			return false;
		}
		/**
		* load vulkan global functions
		*/
		if (!cook::loadVulkanGlobalFunction()) {
			return false;
		}

		/**
		* check available instance extensions
		*/
		uint32_t extensions_count = 0;
		VK_CHK(vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr));
		std::vector<VkExtensionProperties> available_extensions(extensions_count);
		VK_CHK(vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, available_extensions.data()));

		/**
		* create vulkan instance
		*/
		std::vector<char const*> desired_extensions;
		// enable debug util extensions is available
		// VK_EXT_debug_utils extension provide more info combined with validation layers
#if defined(VK_DEBUG_ENABLED)
		bool has_debug_extensions = false;
		for (const auto& eachExt : available_extensions) {
			// chars start with VK_EXT_DEBUG_UTILS_EXTENSION_NAME regarded as support debug extensions
			if (strncmp(eachExt.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME, strlen(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) == 0) {
				has_debug_extensions = true;
				break;
			}
		}
		if (has_debug_extensions) {
			desired_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		else {
			std::cout << "debug extension not supported, aborted." << std::endl;
		}
#endif

		// validate desired extensions all contained in available extensions
		if (!util::validateExtensions(desired_extensions, available_extensions)) {
			std::cout << "extensions not all supported" << std::endl;
		}
		// set parameters of application and vulkan instance
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
		VkInstance instance;
		VK_CHK(vkCreateInstance(&instance_create_info, nullptr, &instance));

		/**
		* load instance-level functions
		*/
		if (!cook::loadVulkanInstanceLevelFunction(instance)) {
			return false;
		}
		// instance-level functions which have extensions dependencies
		if (!cook::loadVulkanInstanceLevelExtensionFunction(instance, desired_extensions)) {
			return false;
		}

		/**
		* enumerating available physical devices
		*/
		uint32_t devices_count = 0;
		VK_CHK(vkEnumeratePhysicalDevices(instance, &devices_count, nullptr));
		std::vector<VkPhysicalDevice> available_devices(devices_count);
		VK_CHK(vkEnumeratePhysicalDevices(instance, &devices_count, available_devices.data()));

		uint32_t queue_family_index = -1;
		VkPhysicalDevice chosen_physical_device;
		// now can be remain empty desired specifications temporarily
		// use multiViewport for example
		VkPhysicalDeviceFeatures desired_features{
			.multiViewport = 1
		};
		std::vector<const char*> desired_device_extensions;
		for (const auto& physical_device : available_devices) {

			/**
			* check available device extensions
			*/
			uint32_t extensions_device_count = 0;
			std::vector<VkExtensionProperties> available_device_extensions;
			// get number and extensions of physical device
			VK_CHK(vkEnumerateDeviceExtensionProperties(physical_device,
				nullptr, &extensions_device_count, nullptr));
			available_device_extensions.resize(extensions_device_count);
			VK_CHK(vkEnumerateDeviceExtensionProperties(physical_device,
				nullptr, &extensions_device_count, available_device_extensions.data()));

			/**
			* get physical device features and properties and choose one of them
			*/
			// additional device specification, geometry/tessellation shaders, depth clamp,bias, etc.
			VkPhysicalDeviceFeatures device_features;
			vkGetPhysicalDeviceFeatures(physical_device, &device_features);
			// device name, driver version, vulkan api version, type(integrated or discrete), etc.
			VkPhysicalDeviceProperties device_properties;
			vkGetPhysicalDeviceProperties(physical_device, &device_properties);
			// validate desired extensions all contained in available extensions
			if (!util::validateExtensions(desired_device_extensions, available_device_extensions)) {
				std::cout << "extensions not all supported" << std::endl;
				continue;
			}
			// validate features sufficiency
			if (desired_features.multiViewport != device_features.multiViewport) {
				std::cout << "device features not supported! " << std::endl;
				continue;
			}

			/**
			* check available queue families and their properties
			*/
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
			// important properties: operation type (graphics, compute, transfer, sparse),queue numbers, timestamps etc.

			/**
			* select index of queue family which sufficient our desired capabilities
			*/

			// use | to represent multi capabilities;
			VkQueueFlags desired_capabilities = VK_QUEUE_GRAPHICS_BIT;
			for (uint32_t index = 0; index < queue_families_count; ++index) {
				if (queue_families[index].queueCount > 0) {
					if ((queue_families[index].queueFlags & desired_capabilities) != 0) {
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

		/**
		* create logical device
		*/

		struct QueueInfo {
			// chosen family index
			uint32_t familyIndex;
			// each queue's priority, size() means number of queues
			std::vector<float> priorities;
		};
		// actually we can choose multiple queue families with multiple queues, now we only use one for simplify the process
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
			.enabledExtensionCount = static_cast<uint32_t>(desired_device_extensions.size()),
			.ppEnabledExtensionNames = desired_device_extensions.data(),
			.pEnabledFeatures = &desired_features
		};

		VkDevice logical_device;
		VK_CHK(vkCreateDevice(chosen_physical_device, &device_create_info, nullptr, &logical_device));

		return true;
	}

	void washDishesChapter1() {

	}
}