#include "../Headers/stats.h"
#include "../Headers/snake.h"
#include <stdlib.h>



GameStats* stats_game_new()
{
	GameStats* stats = (GameStats*)malloc(sizeof(GameStats));

	stats->food_eaten = 0;
	stats->moves_made = 0;
	stats->score = 0;
	stats->snakes_born = 0;
	stats->snakes_died = 0;

	return stats;
}

GlobalStats* stats_global_new()
{
	GlobalStats* stats = (GlobalStats*)malloc(sizeof(GlobalStats));
	
	stats->food_eaten = 0;
	stats->games_played = 0;
	stats->moves_made = 0;
	stats->score = 0;
	stats->snakes_born = 0;
	stats->snakes_died = 0;

	return stats;
}

SnakeStats* stats_snake_new()
{
	SnakeStats* stats = (SnakeStats*)malloc(sizeof(SnakeStats));

	stats->food_eaten = 0;
	stats->moves_made = 0;
	stats->score = 0;
	stats->snakes_born = 0;

	return stats;
}

void stats_game_free(GameStats* stats)
{
	free(stats);
}

void stats_snake_free(SnakeStats* stats)
{
	free(stats);
}

void stats_global_free(GlobalStats* stats)
{
	free(stats);
}

void stats_game_over(GameStats* gameStats, GlobalStats* globalStats)
{
	globalStats->food_eaten += gameStats->food_eaten;
	globalStats->moves_made += gameStats->moves_made;
	globalStats->snakes_born += gameStats->snakes_born;
	globalStats->snakes_died += gameStats->snakes_died;
	globalStats->score += gameStats->score;
	globalStats->games_played ++;
}

