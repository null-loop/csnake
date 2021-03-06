
#include "../Headers/snake.h"
#include "../Headers/grid.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static SnakeTraits* __globalDefaultTraits;

void __shrink_snake(Snake* snake, Grid* grid)
{

	if (snake->length > 1) {
		GridPos* tail = snake_get_tail(snake);
		grid_set(grid, snake->last_tail_pos, Space);
		snake->last_tail_pos->x = tail->x;
		snake->last_tail_pos->y = tail->y;
		snake->length--;
	}
}

int __grow_snake(Snake* snake, Grid* grid)
{
	if (snake->length == 0)
	{
		GridPos* head_pos = grid_find_random_free_pos(grid);

		snake->last_tail_pos = (GridPos*)malloc(sizeof(GridPos));
		snake->last_tail_pos->x = -1;
		snake->last_tail_pos->y = -1;
		snake->last_head_pos = (GridPos*)malloc(sizeof(GridPos));
		snake->last_head_pos->x = -1;
		snake->last_head_pos->y = -1;
		snake->length = 1;


		snake->bits[0].x = head_pos->x;
		snake->bits[0].y = head_pos->y;
		grid_set(grid, head_pos, SnakeHead);
	}
	else
	{
		if (snake->last_tail_pos->x == -1 && snake->last_tail_pos->y == -1) return false;

		GridPos* new_pos = (GridPos*)malloc(sizeof(GridPos));

		if (new_pos == NULL) return false;

		new_pos->x = snake->last_tail_pos->x;
		new_pos->y = snake->last_tail_pos->y;

		int e = grid_get(grid, new_pos);

		if (e == Food)
		{
			free(new_pos);
			return false;
		}

		snake->bits[snake->length].x = new_pos->x;
		snake->bits[snake->length].y = new_pos->y;
		grid_set(grid, new_pos, SnakeBody);
		snake->length++;
		free(new_pos);
	}

	return true;
}


Snake* snake_birth_from_parent(Grid* grid, Snake* parent, GameStats* stats)
{
	Snake* snake = snake_birth_from_traits(grid, parent->traits, stats);

	if (snake != NULL)
	{
		parent->stats->snakes_born++;
	}

	return snake;
}

static SnakeTraits* __mutate_traits(SnakeTraits* traits)
{
	SnakeTraits* nTraits = (SnakeTraits*)malloc(sizeof(SnakeTraits));

	memcpy(nTraits, traits, sizeof(SnakeTraits));

	// TODO:perform mutation on nTraits

	return nTraits;
}

Snake* snake_birth_from_traits(Grid* grid, SnakeTraits* parentTraits, GameStats* stats)
{
	Snake* snake = (Snake*)malloc(sizeof(Snake));

	snake->alive = true;
	snake->bits = (GridPos*)malloc(sizeof(GridPos)*parentTraits->max_length);
	snake->length = 0;
	snake->stats = stats_snake_new();
	snake->traits = __mutate_traits(parentTraits);
	snake->weight = parentTraits->child_birth_weight;
	snake->time_to_birth = snake->traits->time_to_birth;

	if (!__grow_snake(snake, grid))
	{
		snake_free(snake);
		return NULL;
	}

	return snake;
}

static void __update_derived_traits(SnakeTraits* traits)
{
	traits->child_birth_weight = traits->weight_per_growth;
	traits->max_weight = traits->weight_per_growth * traits->max_length;
}

SnakeTraits* snake_traits_default()
{
	if (__globalDefaultTraits != NULL)
	{
		return __globalDefaultTraits;
	}

	__globalDefaultTraits = (SnakeTraits*)malloc(sizeof(SnakeTraits));
	__globalDefaultTraits->start_length = 1;
	__globalDefaultTraits->max_length = 15;
	__globalDefaultTraits->look_ahead_distance = 32;
	__globalDefaultTraits->momentum_score = 30;
	__globalDefaultTraits->default_score = 20;
	__globalDefaultTraits->future_collision_penalty = 5;
	__globalDefaultTraits->food_score = 40;
	__globalDefaultTraits->food_weight = 40;
	__globalDefaultTraits->weight_per_growth = 40;
	__globalDefaultTraits->move_weight_cost = 1;
	__globalDefaultTraits->birth_weight_cost = 200;
	__globalDefaultTraits->birth_weight_threshold = 400;
	__globalDefaultTraits->time_to_birth = 100;

	__update_derived_traits(__globalDefaultTraits);

	return __globalDefaultTraits;
}

static void __snake_free(void* node)
{
	snake_free((Snake*)node);
}

void snakes_free(Loop* snakesLoop)
{
	loop_clear(snakesLoop, &__snake_free);
	free(snakesLoop);
}


void snake_free(Snake* snake)
{
	free(snake->last_head_pos);
	free(snake->last_tail_pos);
	free(snake->stats);
	free(snake->traits);
	free(snake->bits);
	free(snake);
}

void __move_snake_bit(GridPos* c, GridPos* s)
{
	GridPos v = *c;

	c->x = s->x;
	c->y = s->y;

	s->x = v.x;
	s->y = v.y;
}

void __move_snake_bits(Snake* snake, int moveX, int moveY)
{
	GridPos p;
	p.x = snake_get_head(snake)->x + moveX;
	p.y = snake_get_head(snake)->y + moveY;

	for (int i = 0; i < snake->length; i++)
	{
		__move_snake_bit(&(snake->bits[i]), &p);
	}
}

GridPos* snake_get_head(Snake* snake)
{
	return &(snake->bits[0]);
}

GridPos* snake_get_tail(Snake* snake)
{
	return &(snake->bits[snake->length - 1]);
}

MoveResult snake_perform_move(Snake* snake, Grid* grid, GameStats* stats, short dX, short dY)
{
	MoveResult result;
	result.ate = false;
	result.died = false;
	result.gave_birth = false;
	result.grew = false;
	result.child = NULL;
	GridPos* head = snake_get_head(snake);
	GridPos* tail = snake_get_tail(snake);

	snake->last_head_pos->x = head->x;
	snake->last_head_pos->y = head->y;

	snake->last_tail_pos->x = tail->x;
	snake->last_tail_pos->y = tail->y;

	// move the snake bits along.
	__move_snake_bits(snake, dX, dY);

	// get the new head pos
	// remember if there's food there
	GridContents headContent = grid_get(grid, head);

	// set new head pos in grid
	grid_set(grid, head, SnakeHead);

	// set new body pos for the next bit
	if (snake->length > 1)
	{
		GridPos* bodyPos = &(snake->bits[1]);
		grid_set(grid, bodyPos, SnakeBody);
	}

	// apply eating & growing
	if (headContent == Food)
	{
		result.ate = true;
		snake->weight += snake->traits->food_weight;
		if (snake->weight > snake->traits->max_weight)
		{
			snake->weight = snake->traits->max_weight;
		}

		if (snake->time_to_birth > 0)
		{
			snake->time_to_birth--;
		}
		else
		{
			if (snake->weight > snake->traits->birth_weight_threshold)
			{
				// give birth!
				snake->weight -= snake->traits->birth_weight_cost;
				snake->time_to_birth = snake->traits->time_to_birth;

				result.gave_birth = true;
				result.child = snake_birth_from_parent(grid, snake, stats);
			}
		}
	}
	else {
		// remove movement cost weight
		if (snake->weight > snake->traits->move_weight_cost)
		{
			snake->weight -= snake->traits->move_weight_cost;
		}
		else {
			snake->weight = 0;
		}
	}

	float weightLength = (float)snake->weight / snake->traits->weight_per_growth;
	float diff = weightLength - snake->length;

	if (diff >= 1)
	{
		__grow_snake(snake, grid);
		result.grew = true;
	}
	else if (diff <= -1)
	{
		__shrink_snake(snake, grid);
	}

	// check if we've died of starvation
	if (snake->weight == 0)
	{
		result.died = true;
	}

	if (!result.grew)
	{
		// unset last tail pos in grid if we didn't grow
		grid_set(grid, snake->last_tail_pos, Space);
	}

	return result;
}
