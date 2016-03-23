#include "../Headers/grid.h"
#include "../Headers/display.h"
#include "../Headers/common.h"
#include <stdlib.h>

void grid_each(Grid* this, void(*e)(GridPos* p, GridContents c))
{
	GridPos pos;
	GridPos* apos = &pos;

	for (unsigned int x = 0; x < this->size; x++)
	{
		for (unsigned int y = 0; y < this->size; y++)
		{
			pos.x = x;
			pos.y = y;
			e(apos, grid_get(this, apos));
		}
	}
}

Grid* grid_new(unsigned short gridSize)
{
	Grid* grid = (Grid*)malloc(sizeof(Grid));

	if (grid == NULL) return NULL;

	grid->size = gridSize;
	grid->cells = (GridContents**)malloc(sizeof(GridContents*)*gridSize);
	
	if (grid->cells == NULL) return NULL;
	for (unsigned int i = 0; i < gridSize; i++)
	{
		grid->cells[i] = (GridContents*)malloc(sizeof(GridContents)*gridSize);
		if (grid->cells[i] == NULL) return NULL;

		for (unsigned int j = 0; j < gridSize; j++)
		{
			grid->cells[i][j] = Space;
		}
	}

	return grid;
}

void grid_free(Grid* this)
{
	for (unsigned int i = 0; i < this->size; i++)
	{
		free(this->cells[i]);
	}

	free(this->cells);
	free(this);
}

GridContents grid_get(Grid* this, GridPos* pos)
{
	if (pos->x < 0 || pos->x >= (int)this->size || pos->y < 0 || pos->y >= (int)this->size)
	{
		return OffGrid;
	}
	return this->cells[pos->x][pos->y];
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

void grid_set(Grid* this, GridPos* pos, GridContents value)
{
	this->cells[pos->x][pos->y] = value;

	display_pixel(pos->x, pos->y, __colour(value));
}

GridPos* grid_find_random_free_pos(Grid* grid)
{
	GridPos* pos = (GridPos*)malloc(sizeof(GridPos));

	__get_random_grid_pos(grid, pos);

	while (grid_get(grid, pos) != Space)
	{
		__get_random_grid_pos(grid, pos);
	}

	return pos;
}