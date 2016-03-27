#pragma once

typedef struct SnakeStats SnakeStats;
typedef struct GameStats GameStats;
typedef struct GlobalStats GlobalStats;

struct SnakeStats {
	long snakes_born;
	long food_eaten;
	long moves_made;
	long score;
};

struct GameStats
{
	long snakes_born;
	long food_eaten;
	long moves_made;
	long score;
	long snakes_died;
};

struct GlobalStats {
	long snakes_born;
	long food_eaten;
	long moves_made;
	long score;
	long snakes_died;

	long games_played;
};


GameStats* stats_game_new();
GlobalStats* stats_global_new();
SnakeStats* stats_snake_new();

void stats_game_free(GameStats*);
void stats_global_free(GlobalStats*);
void stats_snake_free(SnakeStats*);

void stats_game_over(GameStats*, GlobalStats*);
