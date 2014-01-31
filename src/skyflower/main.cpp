#include <crtdbg.h>
#include <cstdlib>
#include <ctime>

#include "Application.h"

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
int main(int argc, const char *argv[])
{
	// Check for memory leaks for debug builds
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(571766);
#endif

	srand((unsigned int)time(NULL));

	Application app;
	app.Start();

	return 0;
}
