#include "../Headers/food.h"
#include <stdlib.h>

food_add(Game* game)
{
	GridPos* foodPos = grid_find_random_free_pos(game->grid);

	grid_set(game->grid, foodPos, Food);

	free(foodPos);
}

food_fill(Game* game)
{
	for (int i = 0; i < game->max_food; i++)
	{
		food_add(game);
	}
}