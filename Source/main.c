
#include "../Headers/snake.h"
#include "../Headers/game.h"
#include "../Headers/loop.h"
#include "../Headers/food.h"
#include "../Headers/display.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static Game* __game;
static GlobalStats* __global;
static int __display_count = 0;
static const int DISPLAY_STATS_COUNT_MOD = 100000;
static const int DISPLAY = true;

int __birth_snakes(int snakes)
{
	int birthed = 0;
	while (__game->snakes->count < snakes)
	{
		Snake* snake = snake_birth_from_traits(__game->grid, __game->default_traits, __game->stats);
		if (snake != NULL)
		{
			loop_add(__game->snakes, snake);
			birthed++;
		}
		else
		{
			break;
		}
	}
	return birthed;
}

LoopStepResult __game_turn(void* node)
{
	LoopStepResult result = game_turn(node, __game);

	Snake* snake = (Snake*)node;

	if (snake->stats->score > __game->top_score && __game->top_scoring_snake != snake)
	{
		__game->top_scoring_snake = snake;
		__game->top_score = snake->stats->score;

		// copy traits over the top of the best one
		memcpy(__game->top_scoring_traits, snake->traits, sizeof(SnakeTraits));
	}

	if (__display_count % DISPLAY_STATS_COUNT_MOD == 0)
	{
		display_stats(__game->stats, __global);
		__display_count = 0;

		game_average_traits(__game);
		display_average_traits(__game->average_traits);
		if (__game->top_scoring_traits != NULL)
		{
			display_top_scorer_traits(__game->top_score, __game->top_scoring_traits);
		}
		if (DISPLAY == false)
		{
			// TODO:force a redraw of the whole grid!
		}
	}

	if (result.next_step == RemoveNode)
	{
		if (__game->top_scoring_snake == snake)
		{
			__game->top_scoring_snake = NULL;
		}
		snake_free(snake);
	}

	display_flush_pixels(DISPLAY);

	__display_count++;

	return result;
}

void __draw_h_obstruction(Grid* grid)
{
	// draw a capital H obstruction
}

int main(void)
{
	unsigned short maxFood = 256;
	//TODO: why can't we make this bigger?
	unsigned short worldSize = 118;
	unsigned short startSnakes = 20;

	srand((unsigned int)time(0));
	display_init(worldSize, DISPLAY);

	__global = stats_global_new();

	SnakeTraits* defaultTraits = snake_traits_default();

	display_default_traits(defaultTraits);

	while (true)
	{
		__game = game_new(maxFood, worldSize, defaultTraits);

		__draw_h_obstruction(__game->grid);

		// setup food
		food_fill(__game);

		// setup starting snakes
		__birth_snakes(startSnakes);

		display_flush_pixels();

		// run the main loop
		loop_run(__game->snakes, &__game_turn);

		// game's ended, adjust global stats
		stats_game_over(__game->stats, __global);

		// final tidy of output
		display_flush_pixels();
		display_reset();
		display_stats(__game->stats, __global);

		// TODO:update the default traits with the best from this game

		// clean up
		game_free(__game);
	}


}
