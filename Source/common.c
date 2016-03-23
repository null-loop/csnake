#include <stdlib.h>
#include "../Headers/common.h"

int get_random_int(int maxExclusive)
{
	return rand() % maxExclusive;
}