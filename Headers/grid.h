#pragma once

typedef struct Grid Grid;
typedef struct GridRow GridRow;
typedef struct GridPos GridPos;
typedef enum GridContents GridContents;

struct Grid {
	int size;
	GridContents** cells;
};

struct GridPos {
	int x;
	int y;
};

enum GridContents {
	OffGrid = -1,
	Space = 0,
	Food = 1,
	Obstruction = 2,
	SnakeHead = 9,
	SnakeBody = 10
};

void grid_each(Grid* grid, void(*e)(GridPos* p, GridContents c));

Grid* grid_new(int gridSize);
void grid_free(Grid* grid);

#define grid_get(g,p) p.x < 0 || p.x >= g->size || p.y < 0 || p.y >= g->size ? OffGrid : g->cells[p.x][p.y]

void grid_set(Grid* grid, GridPos* pos, GridContents value);

GridPos* grid_find_random_free_pos(Grid* grid);