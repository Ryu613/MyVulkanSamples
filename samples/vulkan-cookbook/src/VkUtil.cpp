#include "VkUtil.hpp"

#include <algorithm>
#include <iterator>
#include <iostream>

namespace util {
	using namespace std;
	unordered_set<string> filterIntersected(vector<string> vector1, vector<string> vector2) {
		sort(vector1.begin(), vector1.end());
		sort(vector2.begin(), vector2.end());

		vector<string> result;
		set_intersection(vector1.begin(), vector1.end(),
			vector2.begin(), vector2.end(),
			back_inserter(result));
		return unordered_set<string>(result.begin(), result.end());
	}

	bool validateExtensions(std::vector<const char*> required, std::vector<VkExtensionProperties> availables) {
		bool eachExtFinded = false;
		for (const auto& rExt : required) {
			eachExtFinded = false;
			for (const auto& aExt : availables) {
				if (strcmp(rExt, aExt.extensionName) == 0) {
					eachExtFinded = true;
					break;
				}
			}
			if (!eachExtFinded) {
				std::cout << "extension: " << rExt << " not supported! " << std::endl;
				return false;
			}
		}
		return true;
	}
}