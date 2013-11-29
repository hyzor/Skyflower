#include "shared/platform.h"

#include "physics/Collision.h"
#include "physics/Physics.h"
#include "Application.h"
#include <stdio.h>
#include <iostream>
#include <sstream>

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

	Physics p;
	float dt; //represents delta time for the gameloop

	Vec3 pos(0.0f, 0.0f, 0.0f); //represents the position of an entity in the world
	Vec3 velo(0.0f, 0.0f, 0.0f); //same, but velocity

	std::ostringstream ss;
	std::string x, y, z;
	std::string xVelo, yVelo, zVelo;
	std::string iter;

	ss << pos.X;
	x = ss.str();

	ss.str("");
	ss << pos.Y;
	y = ss.str();

	ss.str("");
	ss << pos.Z;
	z = ss.str();

	ss.str("");
	ss << velo.X;
	xVelo = ss.str();

	ss.str("");
	ss << velo.Y;
	yVelo = ss.str();

	ss.str("");
	ss << velo.Z;
	zVelo = ss.str();

	std::cout << "Position_0 - X: " + x + " Y: " + y + " Z: " + z + " \n";
	std::cout << "Velocity_0 - X: " + xVelo + " Y: " + yVelo + " Z: " + zVelo + " \n";

	dt = 0.1f;
	p.update(dt);

	for (int i = 0; i < 100; i++)
	{
		//TO be done: do collisioncheck here and decide whether or not to call Physics::addGravityCalc(), or do collisioncheck inside physics class
		p.update(dt); //unnecessary in this example, but should be called before any calculations
		p.addGravityCalc(pos, velo);

		ss.str("");
		ss << pos.X;
		x = ss.str();

		ss.str("");
		ss << pos.Y;
		y = ss.str();

		ss.str("");
		ss << pos.Z;
		z = ss.str();

		ss.str("");
		ss << velo.X;
		xVelo = ss.str();

		ss.str("");
		ss << velo.Y;
		yVelo = ss.str();

		ss.str("");
		ss << velo.Z;
		zVelo = ss.str();

		ss.str("");
		ss << (i + 1);
		iter = ss.str();


		std::cout << "Position_" + iter + " - X: " + x + " Y: " + y + " Z: " + z + " \n";
		std::cout << "Velocity_" + iter + " - X: " + x + " Y: " + y + " Z: " + z + " \n";
	}

	

	

	app.Start();


	//delete singleton
	delete Collision::GetInstance();

	//stäng debug output
	if(cout)
		fclose(cout);
	return 0;
}
