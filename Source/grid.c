#include "../Headers/grid.h"
#include "../Headers/display.h"
#include "../Headers/common.h"
#include <stdlib.h>

void grid_each(Grid* grid, void(*e)(GridPos* p, GridContents c))
{
	GridPos pos;
	GridPos* apos = &pos;

	for (int x = 0; x < grid->size; x++)
	{
		for (int y = 0; y < grid->size; y++)
		{
			pos.x = x;
			pos.y = y;
			e(apos, grid_get(grid, pos));
		}
	}
}

Grid* grid_new(int gridSize)
{
	Grid* grid = (Grid*)malloc(sizeof(Grid));

	if (grid == NULL) return NULL;

	grid->size = gridSize;
	grid->cells = (GridContents**)malloc(sizeof(GridContents*)*gridSize);
	
	if (grid->cells == NULL) return NULL;
	for (int i = 0; i < gridSize; i++)
	{
		grid->cells[i] = (GridContents*)malloc(sizeof(GridContents)*gridSize);
		if (grid->cells[i] == NULL) return NULL;

		for (int j = 0; j < gridSize; j++)
		{
			grid->cells[i][j] = Space;
		}
	}

	return grid;
}

void grid_free(Grid* grid)
{
	for (int i = 0; i < grid->size; i++)
	{
		free(grid->cells[i]);
	}

	free(grid->cells);
	free(grid);
}


unsigned short __colour(GridContents content)
{
	return content;
}

void __get_random_grid_pos(Grid* grid, GridPos* pos)
{
	int x = get_random_int(grid->size);
	int y = get_random_int(grid->size);

	pos->x = x;
	pos->y = y;
}

void grid_set(Grid* grid, GridPos* pos, GridContents value)
{
	grid->cells[pos->x][pos->y] = value;

	display_pixel(pos->x, pos->y, __colour(value));
}

GridPos* grid_find_random_free_pos(Grid* grid)
{
	GridPos* pos = (GridPos*)malloc(sizeof(GridPos));

	__get_random_grid_pos(grid, pos);

	while (grid_get(grid, (*pos)) != Space)
	{
		__get_random_grid_pos(grid, pos);
	}

	return pos;
}