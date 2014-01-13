#include <cstdio>
#include <iostream>
#include <sstream>
#include <ctime>
#include <Windows.h>

//#include "graphics/GraphicsEngine.h"
#include "shared/platform.h"
#include "physics/Physics.h"
#include "Application.h"
#include "ScriptHandler.h"
#include "LevelHandler.h"

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
int main(int argc, const char *argv[])
{
	// Check for memory leaks for debug builds
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Application app;

	/*CollisionInstance* ci = Collision::GetInstance()->CreateCollisionInstance("TestBlock", Vec3());
	if (ci->Test(r) > 0)
		std::cout << "Model collision! " << ci->Test(r) << std::endl;
	Collision::GetInstance()->DeleteCollisionInstance(ci);*/

	/*PotentialField pf;
	pf.CreateField(200, 5, Vec3(10, 10, 0));
	pf.CreateField(500, 3, Vec3(6, 10, 0));
	pf.CreateField(-20, 100, Vec3(0, 0, 0));
	Vec3 pos = Vec3(10, 20, 0);
	for (int i = 0; i < 30; i++)
	{
		system("CLS");

		pos += pf.GetDir(pos);

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				if ((int)pos.X == x && (int)pos.Y == y)
					std::cout << "#";
				else if (pf.GetWeight(Vec3(x, y))>0)
					std::cout << "+";
				else
					std::cout << "-";
			}
			std::cout << std::endl;
		}
	}*/

	/*
	PotentialField pf;
	pf.CreateField(200, 5, Vec3(10, 10, 0));
	pf.CreateField(500, 3, Vec3(6, 10, 0));
	pf.CreateField(-20, 100, Vec3(10, 0, 0));
	Vec3 pos = Vec3(10, 20, 0);
	for (int i = 0; i < 30; i++)
	{
		system("CLS");

		pos += pf.GetDir(pos);

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				if ((int)pos.X == x && (int)pos.Y == y)
					std::cout << "#";
				else if (pf.GetWeight(Vec3(x, y))>0)
					std::cout << "+";
				else
					std::cout << "-";
			}
			std::cout << std::endl;
		}
	}
	*/

	app.Start();


	//delete singletons
	//delete levelHandler;
	delete Collision::GetInstance();

	return 0;
}
