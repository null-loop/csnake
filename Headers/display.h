#pragma once

#include "stats.h"
#include "snake.h"

void display_init(int worldSize, int displayActive);

void display_reset();

void display_pixel(unsigned short x, unsigned short y, int new_state);
void display_flush_pixels();

void display_stats(GameStats* stats, GlobalStats* globalStats);
void display_average_traits(SnakeTraits* traits);
void display_default_traits(SnakeTraits* traits);
void display_top_scorer_traits(int score, SnakeTraits* traits);
