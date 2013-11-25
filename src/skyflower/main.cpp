#include "shared/platform.h"

#include "physics/Box.h"
#include "Application.h"
#include <stdio.h>
#include <iostream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	Application app;

	FILE* cout = nullptr;
	if (AllocConsole())
	{
		freopen_s(&cout, "CONOUT$", "w", stdout);
		SetConsoleTitle("Debug Console");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_BLUE);
	}
	std::cout << "Debug!" << std::endl;


	Box b1 = Box(10,0,0,10,10,10);
	Box b2 = Box(19, 9, -9, 10, 10, 10);
	if(b1.Test(b2))
		std::cout << "collision!" << std::endl;
	

	app.Start();

	//stäng debug output
	if(cout)
		fclose(cout);

	return 0;
}
