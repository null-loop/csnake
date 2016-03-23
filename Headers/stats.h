#pragma once

typedef struct SnakeStats SnakeStats;
typedef struct GameStats GameStats;
typedef struct GlobalStats GlobalStats;

struct SnakeStats {
	unsigned int snakes_born;
	unsigned int food_eaten;
	unsigned int moves_made;
	unsigned int score;
};

struct GameStats
{
	unsigned int snakes_born;
	unsigned int food_eaten;
	unsigned int moves_made;
	unsigned int score;
	unsigned int snakes_died;
};

struct GlobalStats {
	unsigned long snakes_born;
	unsigned long food_eaten;
	unsigned long moves_made;
	unsigned long score;
	unsigned int snakes_died;

	unsigned int games_played;
};


GameStats* stats_game_new();
GlobalStats* stats_global_new();
SnakeStats* stats_snake_new();

void stats_game_free(GameStats*);
void stats_global_free(GlobalStats*);
void stats_snake_free(SnakeStats*);

void stats_game_over(GameStats*, GlobalStats*);
