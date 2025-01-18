#pragma once

#include "vulkan/vulkan.h"
#include <cassert>
#include <iostream>

#if defined _WIN32
    #include <windows.h>
    #define LoadFunction GetProcAddress
    #define LIBRARY_TYPE HMODULE
#elif defined _linux
    #define LoadFunction dlsym
#define LIBRARY_TYPE void*
#endif

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
// can be used for functions which should be exported from vulkan.h
// just declares, the definitions are in VulkanFunctions.cpp respectively
#define EXPORTED_VULKAN_FUNCTION( name ) extern PFN_##name name;
// these macros are in different scope
#define GLOBAL_LEVEL_VULKAN_FUNCTION( name ) extern PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION( name ) extern PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension ) extern PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION( name ) extern PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension ) extern PFN_##name name;

#include "ListOfVulkanFunctions.inl"
    
    bool loadVulkanFunction(const LIBRARY_TYPE& vulkan_library);

    bool loadVulkanGlobalFunction();
    
} // namespace cook