#include <iostream>
#include <vector>

#include "VulkanFunctions.hpp"

int main() {
	
	/*
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
	return true;

	/**
	* load vulkan functions
	*/
	cook::loadVulkanFunction(vulkan_library);
	/**
	* load vulkan global functions
	*/
	cook::loadVulkanGlobalFunction();

	/**
	* check available instance extensions
	*/
	uint32_t extensions_count;
	VK_CHK(vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr));
	std::vector<VkExtensionProperties> available_extensions(extensions_count);
	VK_CHK(vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, available_extensions.data()));

}