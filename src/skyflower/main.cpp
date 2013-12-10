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

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
int main(int argc, const char *argv[])
{
	Application app;


	// Graphics engine
	//GraphicsEngine* gEngine = CreateGraphicsEngine();

	//if (!gEngine->Init(app.m_window->m_window))
	//	return 0;

	//std::cout << "GraphicsEngine started\n";
	//gEngine->Run();


	/*CollisionInstance* ci = Collision::GetInstance()->CreateCollisionInstance("TestBlock", Vec3());
	if (ci->Test(r) > 0)
		std::cout << "Model collision! " << ci->Test(r) << std::endl;
	Collision::GetInstance()->DeleteCollisionInstance(ci);*/









	app.Start();


	//delete singleton
	delete Collision::GetInstance();
	delete ScriptHandler::GetInstance();

	return 0;
}
