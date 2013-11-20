#include <iostream>

#include "graphics/graphics.h"
#include "physics/physics.h"
#include "sound/sound.h"

#include "shared/util.h"

int main(int argc, const char *argv[])
{
	std::cout << "Hello Skyflower!\n";

	FinGrafikFunktion();
	FinFysikFunktion();
	FinLjudFunktion();

	uint64_t memory = GetMemoryUsage();
	printf("Skyflower is using %.1fMiB of memory!\n", (float)memory / (1024 * 1024));

	float oldTime = GetTime();
	float time, timeElapsed;

	while(1)
	{
		time = GetTime();
		timeElapsed = time - oldTime;
		oldTime = time;

		//printf("%f\n", timeElapsed);
	}

	return 0;
}
