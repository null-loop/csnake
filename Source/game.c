#include "../Headers/game.h"
#include "../Headers/grid.h"
#include "../Headers/loop.h"
#include "../Headers/snake.h"
#include "../Headers/food.h"
#include <stdlib.h>
#include <stdbool.h>

static unsigned int const MOVE_SCORE = 1;
static unsigned int const ATE_SCORE = 10;
static unsigned int const GREW_SCORE = 10;
static unsigned int const GAVE_BIRTH_SCORE = 100;

Game* game_new(unsigned short maxFood, unsigned short worldSize, SnakeTraits* defaultTraits)
{
	Game* game = (Game*)malloc(sizeof(Game));

	game->default_traits = defaultTraits;

	game->max_food = maxFood;
	game->world_size = worldSize;

	game->grid = grid_new(worldSize);
	game->snakes = loop_new();
	game->stats = stats_game_new();

	game->top_score = 0;
	game->top_scoring_snake = NULL;
	game->top_scoring_traits = (SnakeTraits*)malloc(sizeof(SnakeTraits));
	game->average_traits = (SnakeTraits*)malloc(sizeof(SnakeTraits));

	return game;
}

void game_free(Game* game)
{
	free(game->stats);
	grid_free(game->grid);
	snakes_free(game->snakes);
	free(game);
}

struct Move {
	short dX;
	short dY;
};

struct ScoredMove {
	struct Move move;
	short score;
};

static int __score_move(Snake* snake, Game* game, struct Move move, int score)
{
	unsigned int moves = 0;
	GridPos current;
	GridPos* head = snake_get_head(snake);
	current.x = head->x + move.dX;
	current.y = head->y + move.dY;

	while (moves < snake->traits->look_ahead_distance)
	{
		int content = grid_get(game->grid, &current);

		if (moves == 0 && (content == SnakeBody || content == SnakeHead || content == OffGrid || content == Obstruction))
		{
			score = -10;
			return score;
		}
		else
		{
			if (content == SnakeBody || content == SnakeHead || content == OffGrid || content == Obstruction)
			{
				int p = snake->traits->future_collision_penalty / (moves + 1);
				score -= p;
				return score;
			}
			else if (content == Food)
			{
				int f = snake->traits->food_score;
				score += f;
			}
		}

		current.x = current.x + move.dX;
		current.y = current.y + move.dY;

		moves++;
	}

	return score;
}

static struct Move __get_best_move(Snake* snake, Game* game)
{
	GridPos* head = snake_get_head(snake);
	int lposX = head->x, lposY = head->y;
	if (snake->last_head_pos->x != -1 && snake->last_head_pos->y != -1)
	{
		lposX = snake->last_head_pos->x;
		lposY = snake->last_head_pos->y;
	}
	int dx = head->x - lposX;
	int dy = head->y - lposY;
	int momentum_score = snake->traits->momentum_score;
	int default_score = snake->traits->default_score;

	struct ScoredMove moves[4];
	struct ScoredMove bestMove;

	moves[0].move.dX = 1;
	moves[0].move.dY = 0;
	moves[0].score = __score_move(snake, game, moves[0].move, (dx == 1 && dy == 0) ? momentum_score : default_score);

	moves[1].move.dX = 0;
	moves[1].move.dY = 1;
	moves[1].score = __score_move(snake, game, moves[1].move, (dx == 0 && dy == 1) ? momentum_score : default_score);

	moves[2].move.dX = -1;
	moves[2].move.dY = 0;
	moves[2].score = __score_move(snake, game, moves[2].move, (dx == -1 && dy == 0) ? momentum_score : default_score);

	moves[3].move.dX = 0;
	moves[3].move.dY = -1;
	moves[3].score = __score_move(snake, game, moves[3].move, (dx == 0 && dy == -1) ? momentum_score : default_score);

	bestMove = moves[0];
	if (moves[1].score > bestMove.score) bestMove = moves[1];
	if (moves[2].score > bestMove.score) bestMove = moves[2];
	if (moves[3].score > bestMove.score) bestMove = moves[3];

	if (bestMove.score < 0)
	{
		bestMove.move.dX = 0;
		bestMove.move.dY = 0;
	}

	return bestMove.move;
}

MoveResult __snake_turn(Snake* snake, Game* game)
{
	MoveResult result;

	result.ate = false;
	result.died = false;
	result.gave_birth = false;
	result.grew = false;
	result.child = NULL;

	struct Move move = __get_best_move(snake, game);

	if (move.dX==0 && move.dY==0)
	{
		result.died = true;
	}
	else
	{
		result = snake_perform_move(snake, game->grid, game->stats, move.dX, move.dY);

		if (result.ate)
		{
			food_add(game);
		}
	}

	return result;
}

LoopStepResult game_turn(void* node, Game* game)
{
	int score = 0;
	Snake* snake = (Snake*)node;
	LoopStepResult r;
	r.next_step = ContinueLoop;
	r.node = NULL;

	MoveResult mr = __snake_turn(snake, game);

	if (mr.died)
	{
		for (int i = 0; i < snake->length; i++)
		{
			grid_set(game->grid, &(snake->bits[i]), Space);
		}
		game->stats->snakes_died++;
		r.next_step = RemoveNode;
	}
	else
	{
		score += MOVE_SCORE;
		if (mr.ate)
		{
			score += ATE_SCORE;
			snake->stats->food_eaten++;
			game->stats->food_eaten++;
			if (mr.grew)
			{
				score += GREW_SCORE;
			}
		}

		if (mr.gave_birth)
		{
			score += GAVE_BIRTH_SCORE;
			snake->stats->snakes_born++;
			game->stats->snakes_born++;

			r.next_step = InsertNode;
			r.node = mr.child;
		}
		snake->stats->score += score;
		snake->stats->moves_made++;
		game->stats->score += score;
		game->stats->moves_made++;
	}

	return r;
}

void __average_traits(void* snakePtr, void* avgPtr)
{
	SnakeTraits* snake = ((Snake*)snakePtr)->traits;
	SnakeTraits* avg = (SnakeTraits*)avgPtr;

	avg->birth_weight_cost += snake->birth_weight_cost;
	avg->birth_weight_threshold += snake->birth_weight_threshold;
	avg->child_birth_weight += snake->child_birth_weight;
	avg->default_score += snake->default_score;
	avg->food_score += snake->food_score;
	avg->food_weight += snake->food_weight;
	avg->future_collision_penalty += snake->future_collision_penalty;
	avg->look_ahead_distance += snake->look_ahead_distance;
	avg->max_length += snake->max_length;
	avg->max_weight += snake->max_weight;
	avg->momentum_score += snake->momentum_score;
	avg->move_weight_cost += snake->move_weight_cost;
	avg->start_length += snake->start_length;
	avg->time_to_birth += snake->time_to_birth;
	avg->weight_per_growth += snake->weight_per_growth;
}

void game_average_traits(Game* game)
{
	SnakeTraits* avg = game->average_traits;
	int snakeCount = game->snakes->count;

	avg->birth_weight_cost = 0;
	avg->birth_weight_threshold = 0;
	avg->child_birth_weight = 0;
	avg->default_score = 0;
	avg->food_score = 0;
	avg->food_weight = 0;
	avg->future_collision_penalty = 0;
	avg->look_ahead_distance = 0;
	avg->max_length = 0;
	avg->max_weight = 0;
	avg->momentum_score = 0;
	avg->move_weight_cost = 0;
	avg->start_length = 0;
	avg->time_to_birth = 0;
	avg->weight_per_growth = 0;

	loop_each_with_state(game->snakes, avg, &__average_traits);

	avg->birth_weight_cost = avg->birth_weight_cost / snakeCount;
	avg->birth_weight_threshold = avg->birth_weight_threshold / snakeCount;
	avg->child_birth_weight = avg->child_birth_weight / snakeCount;
	avg->default_score = avg->default_score / snakeCount;
	avg->food_score = avg->food_score / snakeCount;
	avg->food_weight = avg->food_weight / snakeCount;
	avg->future_collision_penalty = avg->future_collision_penalty / snakeCount;
	avg->look_ahead_distance = avg->look_ahead_distance / snakeCount;
	avg->max_length = avg->max_length / snakeCount;
	avg->max_weight = avg->max_weight / snakeCount;
	avg->momentum_score = avg->momentum_score / snakeCount;
	avg->move_weight_cost = avg->move_weight_cost / snakeCount;
	avg->start_length = avg->start_length / snakeCount;
	avg->time_to_birth = avg->time_to_birth / snakeCount;
	avg->weight_per_growth = avg->weight_per_growth / snakeCount;

}