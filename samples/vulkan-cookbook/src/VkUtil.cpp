#include "VkUtil.hpp"

#include <algorithm>
#include <iterator>

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
}