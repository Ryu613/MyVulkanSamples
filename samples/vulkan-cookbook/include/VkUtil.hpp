#pragma once

#include <unordered_set>
#include <vector>
#include <string>

#include <vulkan/vulkan.h>

namespace util {
	/**
	* get intersected elements of two std::vector
	*/
	std::unordered_set<std::string> filterIntersected(std::vector<std::string> vector1, std::vector<std::string> vector2);
	/**
	* validate extensions
	*/
	bool validateExtensions(std::vector<const char*> vector1, std::vector<VkExtensionProperties> vector2);
}