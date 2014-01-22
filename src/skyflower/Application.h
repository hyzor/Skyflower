#ifndef SKYFLOWER_APPLICATION
#define SKYFLOWER_APPLICATION

#include "Window.h"
#include "InputHandler.h"
#include "graphics/GraphicsEngine.h"
#include "shared/util.h"
#include "PotentialField.h"
#include "physics/Collision.h"
#include "Sound/SoundEngine.h"
#include "EntityManager.h"
#include "LevelHandler.h"
#include "Menu.h"
#include "GUI.h"

using namespace Cistron;

class Application : WindowListener, InputListener
{
public:
	enum GameState
	{
		menu,
		game,
		loading
	};

	Application();
	virtual ~Application();

	void Start();

public: // WindowListener
	void OnWindowShouldClose();
	void OnWindowResized(unsigned int width, unsigned int height);
	void OnWindowResizeEnd();
	void OnWindowActivate();
	void OnWindowDeactivate();

public: // InputListener
	void OnMouseMove(int deltaX, int deltaY);
	void OnMouseButtonDown(enum MouseButton button);
	void OnMouseButtonUp(enum MouseButton button);
	void OnMouseWheel(int delta);

	void OnKeyDown(unsigned short key);
	void OnKeyUp(unsigned short key);


private:
	void updateGame(float dt, Movement* playerMove);
	void updateMenu(float dt);
	void updateLoading(float dt);
	void changeGameState(GameState newState);

	void setBackgroundMusicList(const std::vector<std::string> &musicList);

private:
	double m_oldTime;

	Window *m_window;
	InputHandler *m_inputHandler;
	GraphicsEngine *m_graphicsEngine;
	SoundEngine *m_soundEngine;
	EntityManager* entityManager;
	CameraController *camera;
	GUI *m_GUI;
	bool m_quit;
	GameState gameState;
	Menu *m_menu;

	bool m_showCharts;
	int m_frameChartID;
	int m_memChartID;

	std::vector<std::string> m_backgroundMusicMenu;
	std::vector<std::string> m_backgroundMusicGame;

	SoundSource *m_backgroundMusic;
	size_t m_backgroundMusicIndex;
	const std::vector<std::string> *m_backgroundMusicList;
};

#endif
