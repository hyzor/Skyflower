#include <iostream>

#include "graphics/graphics.h"
#include "sound/sound.h"

#include "shared/util.h"

int main(int argc, const char *argv[])
{
	std::cout << "Hello Skyflower!\n";

	FinGrafikFunktion();
	FinLjudFunktion();

	uint64_t memory = GetMemoryUsage();
	printf("Skyflower is using %.1fMiB of memory!\n", (float)memory / (1024 * 1024));

	while (1)
	{
		// Trevlig mainloop.
	}

	return 0;
}
