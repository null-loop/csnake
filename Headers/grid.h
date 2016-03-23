#pragma once

typedef struct Grid Grid;
typedef struct GridRow GridRow;
typedef struct GridPos GridPos;
typedef enum GridContents GridContents;

struct Grid {
	unsigned short size;
	GridContents** cells;
};

struct GridPos {
	short x;
	short y;
};

enum GridContents {
	OffGrid = -1,
	Space = 0,
	Food = 1,
	Obstruction = 2,
	SnakeHead = 9,
	SnakeBody = 10
};

void grid_each(Grid* this, void(*e)(GridPos* p, GridContents c));

Grid* grid_new(unsigned short gridSize);
void grid_free(Grid* this);

GridContents grid_get(Grid* this, GridPos* pos);
void grid_set(Grid* this, GridPos* pos, GridContents value);

GridPos* grid_find_random_free_pos(Grid* grid);