#include <iostream>
#include <vector>
#include "VulkanFunctions.hpp"

namespace cook {
	bool app() {
	
		/**
		* load vulkan library
		* 
		* in modern way, in general, this should be done by volk or vulkan-hpp
		* (volk has been included in Vulkan SDK, 
		* while vulkan-hpp can be included using vulkan/vulkan.hh instead of vulkan/vulkan.h)
		* @see Volk
		* @see Vulkan-Hpp
		*/ 

		// if cross-platorm support needed, use macro to determine which library to call;
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
		uint32_t extensions_count;
		VK_CHK(vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr));
		std::vector<VkExtensionProperties> available_extensions(extensions_count);
		VK_CHK(vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, available_extensions.data()));

	}
}

int main() {
	if (!cook::app()) {
		std::cout << "app run error" << std::endl;
	}
	return 0;
}
