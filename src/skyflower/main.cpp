#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <ctime>

#include "Application.h"

// Must be included last!
#include "shared/debug.h"

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
int main(int argc, const char *argv[])
{
	// Check for memory leaks in debug builds.
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	srand((unsigned int)time(NULL));

	Application app;
	app.Start();

	return 0;
}
