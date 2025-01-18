#pragma once

#include <unordered_set>
#include <vector>
#include <string>

namespace util {
	/**
	* get intersected elements of two std::vector
	*/
	std::unordered_set<std::string> filterIntersected(std::vector<std::string> vector1, std::vector<std::string> vector2);
}