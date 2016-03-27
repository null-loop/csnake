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

	GridPos* bits;
	int length;

	int alive;
	int weight;
	int time_to_birth;
	GridPos* last_tail_pos;
	GridPos* last_head_pos;
};

struct MoveResult
{
	int died;
	int gave_birth;
	int ate;
	int grew;
	Snake* child;
};

struct SnakeTraits {
	int start_length;
	int max_length;
	int max_weight;
	int look_ahead_distance;
	int momentum_score;
	int default_score;
	int future_collision_penalty;
	int food_score;
	int food_weight;
	int weight_per_growth;
	int move_weight_cost;
	int birth_weight_cost;
	int child_birth_weight;
	int time_to_birth;
	int birth_weight_threshold;
};

GridPos* snake_get_head(Snake* snake);
GridPos* snake_get_tail(Snake* snake);

Snake* snake_birth_from_traits(Grid* grid, SnakeTraits* parentTraits, GameStats* stats);
Snake* snake_birth_from_parent(Grid* grid, Snake* parent, GameStats* stats);

SnakeTraits* snake_traits_default();

void snakes_free(Loop* snakesLoop);
void snake_free(Snake* snake);

MoveResult snake_perform_move(Snake* snake, Grid* grid, GameStats* stats, int dX, int dY);
