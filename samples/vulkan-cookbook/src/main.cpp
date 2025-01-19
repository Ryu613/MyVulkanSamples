#include "Recipes.hpp"

#ifndef RUN_RECIPE_NAME
#define RUN_RECIPE_NAME
#endif

#define _DO_RECIPE(function, name ) cook::##function##name()

#define DO_RECIPE( function, name ) _DO_RECIPE(function, name)

int main() {
	if (!DO_RECIPE(doRecipe, RUN_RECIPE_NAME)) {
		std::cout << "failed cooking" << std::endl;
	}
	DO_RECIPE(washDishes, RUN_RECIPE_NAME);
	return 0;
}
