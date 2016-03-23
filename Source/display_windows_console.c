#include "../Headers/display.h"
#include "../Headers/queue.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>

typedef struct PixelUpdate PixelUpdate;

struct PixelUpdate
{
	unsigned short x;
	unsigned short y;
	int new_state;
};

static char* DIVIDER					  = "-----------------------------------------";

static char* FOOD_EATEN_LABEL		      = "Food Eaten             : %16d";
static char* MOVES_MADE_LABEL		      = "Moves Made             : %16d";
static char* SNAKES_BORN_LABEL		      = "Snakes Born            : %16d";
static char* SNAKES_DIED_LABEL		      = "Snakes Died            : %16d";
static char* SNAKES_ALIVE_LABEL		      = "Snakes Alive           : %16d";
static char* GAMES_PLAYED_LABEL		      = "Games Played           : %16d";
static char* SCORE_LABEL                  = "Score                  : %16d";

static char* START_LENGTH_LABEL		      = "Start Length           : %16d";
static char* MAX_LENGTH_LABEL		      = "Max. Length            : %16d";
static char* MAX_WEIGHT_LABEL		      = "Max. Weight            : %16d";
static char* LOOK_AHEAD_LABEL		      = "Look Ahead             : %16d";
static char* MOMENTUM_SCORE_LABEL	      = "Momentum Score         : %16d";
static char* DEFAULT_SCORE_LABEL	      = "Default Score          : %16d";
static char* COLLISION_PENALTY_LABEL      = "Collision Penalty      : %16d";
static char* FOOD_SCORE_LABEL		      = "Food Score             : %16d";
static char* FOOD_WEIGHT_LABEL		      = "Food Weight            : %16d";
static char* WEIGHT_PER_GROWTH_LABEL      = "Weight Per Growth      : %16d";
static char* MOVE_WEIGHT_COST_LABEL       = "Move Weight Cost       : %16d";
static char* BIRTH_WEIGHT_COST_LABEL      = "Birth Weight Cost      : %16d";
static char* CHILD_BIRTH_WEIGHT_LABEL     = "Child Birth Weight     : %16d";
static char* TIME_TO_BIRTH_LABEL          = "Time To Birth          : %16d";
static char* BIRTH_WEIGHT_THRESHOLD_LABEL = "Birth Weight Threshold : %16d";

static const int STATS_COLUMN_WIDTH = 35;
static const int STATS_ROW_OFFSET = 1;
static const int STATS_COLUMN_OFFSET = 120;

static int GRID_ROW_OFFSET = 3;
static const int GRID_COLUMN_OFFSET = 1;
static const int GRID_ELEMENT_SIZE_COLUMNS = 1;
static const int GRID_ELEMENT_SIZE_ROWS = 1;

static CONSOLE_CURSOR_INFO __cursor_info;
static CONSOLE_SCREEN_BUFFER_INFO __console_info;
static HANDLE __console_out;
static DWORD __cell_count;
static COORD __home_coords = { 0,0 };

static Queue* __pixel_updates;
static unsigned short __world_size;

void __init_console_cursor()
{
	__cursor_info.bVisible = false;
	__cursor_info.dwSize = 0;
	SetConsoleCursorInfo(__console_out, &__cursor_info);
}

void __init_console()
{
	// get the handle
	__console_out = GetStdHandle(STD_OUTPUT_HANDLE);
	if (__console_out == INVALID_HANDLE_VALUE) return;

	// set the title
	SetConsoleTitle(TEXT("CSnake"));

	// compute cell count
	if (!GetConsoleScreenBufferInfo(__console_out, &__console_info)) return;
	__cell_count = __console_info.dwSize.X * __console_info.dwSize.Y;

	// init the cursor
	__init_console_cursor();
}

void __clear_console()
{
	DWORD count;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(__console_out, (TCHAR) ' ', __cell_count, __home_coords, &count)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(__console_out, __console_info.wAttributes, __cell_count, __home_coords, &count)) return;

	SetConsoleCursorPosition(__console_out, __home_coords);
}

void display_init(unsigned short worldSize)
{
	// work out the screen size we need for the world size
	GRID_ROW_OFFSET = 1;

	__pixel_updates = queue_new();
	__init_console();
	__clear_console();
}

void display_reset() {
	__clear_console();
}

void __null_pixel_out(PixelUpdate* pixel)
{
	free(pixel);
}

void __console_pixel_out(PixelUpdate* pixel)
{
	COORD c;
	c.X = GRID_COLUMN_OFFSET + (pixel->x * (GRID_ELEMENT_SIZE_COLUMNS));
	c.Y = GRID_ROW_OFFSET + (pixel->y * (GRID_ELEMENT_SIZE_ROWS));

	SetConsoleCursorPosition(__console_out, c);

	switch (pixel->new_state)
	{
		case 1:
			printf(".");
			break;
		case 2:
			printf("#");
			break;
		case 9:
			printf("@");
			break;
		case 10:
			printf("'");
			break;
		default:
			printf(" ");
			break;
	}
	free(pixel);
}

void display_pixel(unsigned short x, unsigned short y, int newState)
{
	PixelUpdate* update = (PixelUpdate*)malloc(sizeof(PixelUpdate));

	update->x = x;
	update->y = y;
	update->new_state = newState;

	queue_enqueue(__pixel_updates, update);
}

void display_flush_pixels(int toDisplay)
{
	if (toDisplay == true)
	{
		queue_dequeue_each(__pixel_updates, &__console_pixel_out);
	}
	else
	{
		queue_dequeue_each(__pixel_updates, &__null_pixel_out);
	}
}

COORD __get_console_coord(int row, int col)
{
	COORD c;
	c.Y = STATS_ROW_OFFSET + row;
	c.X = STATS_COLUMN_OFFSET + (STATS_COLUMN_WIDTH * col);
	return c;
}

void __console_stat_out(int row, int col, char* label, long value)
{
	SetConsoleCursorPosition(__console_out, __get_console_coord(row, col));
	printf(label, value);
}

void __console_label(int row, int col, char* label)
{
	SetConsoleCursorPosition(__console_out, __get_console_coord(row,col));
	printf(label);
}

void __console_stats_out(GameStats* stats, GlobalStats* globalStats)
{
	__console_label(0, 0, "All Games");
	__console_label(1, 0, DIVIDER);
	__console_stat_out(2, 0, GAMES_PLAYED_LABEL, globalStats->games_played);
	__console_stat_out(3, 0, FOOD_EATEN_LABEL, globalStats->food_eaten);
	__console_stat_out(4, 0, MOVES_MADE_LABEL, globalStats->moves_made);
	__console_stat_out(5, 0, SNAKES_BORN_LABEL, globalStats->snakes_born);
	__console_stat_out(6, 0, SNAKES_DIED_LABEL, globalStats->snakes_died);
	

	__console_label(8, 0, "This Game");
	__console_label(9, 0, DIVIDER);
	__console_stat_out(10, 0, FOOD_EATEN_LABEL, stats->food_eaten);
	__console_stat_out(11, 0, MOVES_MADE_LABEL, stats->moves_made);
	__console_stat_out(12, 0, SNAKES_BORN_LABEL, stats->snakes_born);
	__console_stat_out(13, 0, SNAKES_DIED_LABEL, stats->snakes_died);
	__console_stat_out(14, 0, SNAKES_ALIVE_LABEL, stats->snakes_born - stats->snakes_died);
}

void __console_traits(int startRow, SnakeTraits* traits)
{
	__console_stat_out(startRow, 0, START_LENGTH_LABEL, traits->start_length);
	__console_stat_out(startRow + 1, 0, MAX_LENGTH_LABEL, traits->max_length);
	__console_stat_out(startRow + 2, 0, MAX_WEIGHT_LABEL, traits->max_weight);
	__console_stat_out(startRow + 3, 0, LOOK_AHEAD_LABEL, traits->look_ahead_distance);
	__console_stat_out(startRow + 4, 0, MOMENTUM_SCORE_LABEL, traits->momentum_score);
	__console_stat_out(startRow + 5, 0, DEFAULT_SCORE_LABEL, traits->default_score);
	__console_stat_out(startRow + 6, 0, COLLISION_PENALTY_LABEL, traits->future_collision_penalty);
	__console_stat_out(startRow + 7, 0, FOOD_SCORE_LABEL, traits->food_score);
	__console_stat_out(startRow + 8, 0, FOOD_WEIGHT_LABEL, traits->food_weight);
	__console_stat_out(startRow + 9, 0, WEIGHT_PER_GROWTH_LABEL, traits->weight_per_growth);
	__console_stat_out(startRow + 10, 0, MOVE_WEIGHT_COST_LABEL, traits->move_weight_cost);
	__console_stat_out(startRow + 11, 0, BIRTH_WEIGHT_COST_LABEL, traits->birth_weight_cost);
	__console_stat_out(startRow + 12, 0, CHILD_BIRTH_WEIGHT_LABEL, traits->child_birth_weight);
	__console_stat_out(startRow + 13, 0, TIME_TO_BIRTH_LABEL, traits->time_to_birth);
	__console_stat_out(startRow + 14, 0, BIRTH_WEIGHT_THRESHOLD_LABEL, traits->birth_weight_threshold);
}

void __console_average_traits(SnakeTraits* traits)
{
	__console_label(16, 0, "Average Traits");
	__console_label(17, 0, DIVIDER);
	__console_traits(18, traits);
}

void __console_top_traits(int score, SnakeTraits* traits)
{
	__console_label(34, 0, "Top Scorers Traits");
	__console_label(35, 0, DIVIDER);
	__console_stat_out(36, 0, SCORE_LABEL, score);
	__console_traits(37, traits);
}

void __console_default_traits(SnakeTraits* traits)
{
	__console_label(53, 0, "Starting Traits");
	__console_label(54, 0, DIVIDER);
	__console_traits(55, traits);
}

void display_stats(GameStats* stats, GlobalStats* globalStats)
{
	__console_stats_out(stats, globalStats);
}

void display_average_traits(SnakeTraits* traits)
{
	__console_average_traits(traits);
}

void display_top_scorer_traits(int score, SnakeTraits* traits)
{
	__console_top_traits(score, traits);
}

void display_default_traits(SnakeTraits* traits)
{
	__console_default_traits(traits);
}