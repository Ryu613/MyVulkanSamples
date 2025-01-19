#include "VulkanFunctions.hpp"

namespace cook {
	/**
	* definition of headers
	* for example:
	* 
	* extern PFN_vkGetInstanceProcAddr; // from header(declares global variable)
	* PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr; // definitions
	* // call loadVulkanFunction()
	* vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)GetProcAddress(vulkan_library, "vkGetInstanceProcAddr"); 
	* ...
	* 
	* next, export other functions based on platforms by redefine EXPORTED_VULKAN_FUNCTION macro,
	* 
	* 
	*/
	#define EXPORTED_VULKAN_FUNCTION( name ) PFN_##name name;
	
	#define GLOBAL_LEVEL_VULKAN_FUNCTION( name ) PFN_##name name;
	#define INSTANCE_LEVEL_VULKAN_FUNCTION( name ) PFN_##name name;
	#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension ) PFN_##name name;
	#define DEVICE_LEVEL_VULKAN_FUNCTION( name ) PFN_##name name;
	#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension ) PFN_##name name;
	
	#include "ListOfVulkanFunctions.inl"
	
	bool loadVulkanFunction(const LIBRARY_TYPE& vulkan_library) {
		#define EXPORTED_VULKAN_FUNCTION( name ) \
		name = (PFN_##name)LoadFunction( vulkan_library, #name ); \
		if(name == nullptr) { \
			std::cout << "couldn't load exported Vulkan function named: " \
				#name << std::endl; \
			return false; \
		}

		#include "ListOfVulkanFunctions.inl"

		return true;
	}

	bool loadVulkanGlobalFunction() {
		#define GLOBAL_LEVEL_VULKAN_FUNCTION( name ) \
		name = (PFN_##name)vkGetInstanceProcAddr( nullptr, #name ); \
		if(name == nullptr) { \
			std::cout << "couldn't load Vulkan global function named: " \
				#name << std::endl; \
			return false; \
		}

		#include "ListOfVulkanFunctions.inl"

		return true;
	}

	bool loadVulkanInstanceLevelFunction(const VkInstance& instance) {
		#define INSTANCE_LEVEL_VULKAN_FUNCTION(name) \
		name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);\
		if(name == nullptr) { \
			std::cout << "couldn't load Vulkan instance-level function named: " \
				#name << std::endl; \
			return false;\
		}
		#include "ListOfVulkanFunctions.inl"
		return true;
	}

	bool loadVulkanInstanceLevelExtensionFunction(const VkInstance& instance, const std::vector<const char*> enabled_extensions) {
		#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) \
			for(auto& each : enabled_extensions) {\
				if (std::string(each) == std::string(extension)) {\
					name = (PFN_##name)vkGetInstanceProcAddr(instance, #name); \
						if (name == nullptr) {\
							std::cout << "couldn't load Vulkan instance-level function named: " \
							#name << std::endl; \
							return false; \
						}\
				}\
			}
		#include "ListOfVulkanFunctions.inl"
		return true;
	}
	
} // namespace cook