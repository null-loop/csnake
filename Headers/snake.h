#pragma once

#include "grid.h"
#include "stats.h"
#include "loop.h"

typedef struct Snake Snake;
typedef struct SnakeTraits SnakeTraits;
typedef struct MoveResult MoveResult;

struct Snake {
	SnakeStats* stats;
	SnakeTraits* traits;

	//TODO:Replace with fixed size array of GridPos
	GridPos* bits;
	int length;

	unsigned short alive;
	unsigned short weight;
	unsigned int time_to_birth;
	GridPos* last_tail_pos;
	GridPos* last_head_pos;
};

struct MoveResult
{
	unsigned short died;
	unsigned short gave_birth;
	unsigned short ate;
	unsigned short grew;
	Snake* child;
};

struct SnakeTraits {
	unsigned int start_length;
	unsigned int max_length;
	unsigned int max_weight;
	unsigned int look_ahead_distance;
	unsigned int momentum_score;
	unsigned int default_score;
	unsigned int future_collision_penalty;
	unsigned int food_score;
	unsigned int food_weight;
	unsigned int weight_per_growth;
	unsigned int move_weight_cost;
	unsigned int birth_weight_cost;
	unsigned int child_birth_weight;
	unsigned int time_to_birth;
	unsigned int birth_weight_threshold;
};

GridPos* snake_get_head(Snake* snake);
GridPos* snake_get_tail(Snake* snake);

Snake* snake_birth_from_traits(Grid* grid, SnakeTraits* parentTraits, GameStats* stats);
Snake* snake_birth_from_parent(Grid* grid, Snake* parent, GameStats* stats);

SnakeTraits* snake_traits_default();

void snakes_free(Loop* snakesLoop);
void snake_free(Snake* snake);

MoveResult snake_perform_move(Snake* snake, Grid* grid, GameStats* stats, short dX, short dY);