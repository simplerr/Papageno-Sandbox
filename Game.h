#ifndef GAME_H
#define GAME_H

#include <Windows.h>
#include "d3dUtil.h"
#include "Runnable.h"
#include "Camera.h"
#include "GfxStats.h"
#include "RigidBody.h"
#include "MouseBox.h"
#include "Sandbox.h"
#include "MainMenu.h"
#include "Enums.h"

using namespace Flex;

class World;

class Game : public Runnable
{
public:
	Game(HINSTANCE hInstance, std::string caption, int width, int height, D3DDEVTYPE devType, DWORD requestedVP);
	~Game();

	void init();
	void update(float t, float dt);
	void draw();
	void changeState(State newState);
private:
	Camera* mCamera;
	Sandbox* mSandbox;
	GfxStats* mGfxStats;
	GameState* mCurrentState;
};

#endif