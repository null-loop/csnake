#pragma once

#include "grid.h"
#include "loop.h"
#include "stats.h"
#include "snake.h"

typedef struct Game Game;


struct Game
{
	SnakeTraits* default_traits;
	Grid* grid;
	Loop* snakes;
	GameStats* stats;
	unsigned short max_food;
	unsigned short world_size;

	// the snake may be dead - so assign the scores AND a copy of traits
	unsigned int top_score;
	Snake* top_scoring_snake;
	SnakeTraits* top_scoring_traits;
	SnakeTraits* average_traits;
};

// create grid, list of snakes, stats object and then game
Game* game_new(unsigned short maxFood, unsigned short worldSize, SnakeTraits* defaultTraits);

void game_free(Game* game);

LoopStepResult game_turn(void* node, Game* game);

void game_average_traits(Game* game);