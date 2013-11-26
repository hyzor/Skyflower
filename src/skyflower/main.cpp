#include "shared/platform.h"

#include "physics/Collision.h"
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
		std::cout << "box collision!" << std::endl;

	Sphere s1 = Sphere(10, 0, 0, 5);
	Sphere s2 = Sphere(0, 0, 0, 5);
	if (s1.Test(s2))
		std::cout << "sphere collision!" << std::endl;

	Triangle t = Triangle(Vec3(), Vec3(1, 0), Vec3(0, 1));
	Ray r = Ray(Vec3(0.1f,0.1f,-0.5f), Vec3(0,0,1));
	if (t.Test(r) > 0)
		std::cout << "triangle collision! " << t.Test(r) << std::endl;
	

	CollisionInstance ci = Collision::GetInstance()->CreateCollisionInstance("TestBlock.obj", Vec3());
	if(ci.Test(r) > 0)
		std::cout << "Model collision! " << ci.Test(r) << std::endl;

	app.Start();


	//delete singleton
	delete Collision::GetInstance();

	//stäng debug output
	if(cout)
		fclose(cout);
	return 0;
}
