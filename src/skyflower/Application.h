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
#include "CutScene.h"

using namespace Cistron;

class Application : WindowListener, InputListener
{
public:
	enum GameState
	{
		menu,
		game,
		loading,
		cutScene
	};

	Application();
	virtual ~Application();

	void Start();

private:

	void updateGame(float dt, float gameTime, Movement* playerMove);
	void updateMenu(float dt);
	void updateLoading(float dt);
	void updateCutScene(float dt);
	void changeGameState(GameState newState);

	void setBackgroundMusicList(const std::vector<std::string> &musicList);

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
	double m_oldTime;
	bool m_quit;
	GameState gameState;

	double mGameTime;
	double mStartTime;

	Window *m_window;
	InputHandler *m_inputHandler;
	GraphicsEngine *m_graphicsEngine;
	SoundEngine *m_soundEngine;
	CameraController *m_camera;
	PhysicsEngine* m_physicsEngine;
	Collision *m_collision;
	PotentialField *m_potentialField;
	ScriptHandler *m_scriptHandler;
	
	EntityManager* m_entityManager;
	GUI *m_GUI;
	Menu *m_menu;
	CutScene *cs;

	bool m_showCharts;
	int m_frameChartID;
	int m_memChartID;

	std::vector<std::string> m_backgroundMusicMenu;
	std::vector<std::string> m_backgroundMusicGame;

	SoundSource *m_backgroundMusic;
	size_t m_backgroundMusicIndex;
	const std::vector<std::string> *m_backgroundMusicList;

	float m_SSAOradius;
	float m_SSAOprojectionFactor;
	float m_SSAObias;
	float m_SSAOcontrast;
	float m_SSAOsigma;
};

#endif
