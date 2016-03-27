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
	int max_food;
	int world_size;

	int top_score;
	Snake* top_scoring_snake;
	SnakeTraits* top_scoring_traits;
	SnakeTraits* average_traits;
};

Game* game_new(unsigned short maxFood, unsigned short worldSize, SnakeTraits* defaultTraits);

void game_free(Game* game);

LoopStepResult game_turn(void* node, Game* game);

void game_average_traits(Game* game);
