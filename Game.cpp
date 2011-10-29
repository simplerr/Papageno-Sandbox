#include <Windows.h>
#include <crtdbg.h>
#include <time.h>

#include "Game.h"
#include "Runnable.h"
#include "Graphics.h"
#include "DirectInput.h"
#include "World.h"
#include "RigidBody.h"
#include <fstream>
#include "Help.h"
#include "About.h"
#include "Menu.h"

// Set the globals
Runnable*			gGame = 0;
IDirect3DDevice9*	gd3dDevice = 0;
Graphics*			gGraphics = 0;
DirectInput*		gDInput = 0;

using namespace Flex;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	//InitAllocCheck(ACOutput_XML); // ACOutput_XML

	// Enable run-time memory check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif
	
	Game game(hInstance, "Riptide Physics", 1200, 800, D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING);
	gGame = &game;
	
	DirectInput di(gGame->getMainWnd(), DISCL_NONEXCLUSIVE|DISCL_FOREGROUND, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);
	gDInput = &di;
	
	gGame->init();

	// Run the game
	return gGame->run();
}

Game::Game(HINSTANCE hInstance, std::string caption, int width, int height, D3DDEVTYPE devType, DWORD requestedVP)
	:Runnable(hInstance, caption, width, height, devType, requestedVP)
{
	gGraphics = new Graphics();
	mCamera = new Camera(DIMENSION_3D, 0, 0, getScreenWidth(), getScreenHeight());
	mCamera->setPosition(0, 0, 500);
	mCamera->setTarget(0, 0, 0);
	mCamera->update();
	mGfxStats = new GfxStats();

	mCurrentState = new MainMenu();
	mCurrentState->connect(&Game::changeState, this);
}
	
Game::~Game()
{
	delete mCamera;
	delete mGfxStats;
	delete mCurrentState;
}

void Game::init()
{
	// This code sets texture filters, which helps to smooth out distortions
	// when you scale a texture.  
	HR(gd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
	HR(gd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
	HR(gd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR));

	//HR(gd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT));

	// This line of code disables Direct3D lighting
	//HR(gd3dDevice->SetRenderState(D3DRS_LIGHTING, false));
	
	// The following code specifies an alpha test and reference value.
	//HR(gd3dDevice->SetRenderState(D3DRS_ALPHAREF, 10));
	//HR(gd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER));

	gd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	gd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	gd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	gd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	gd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	// Indicates that we are using 2D texture coordinates.
	HR(gd3dDevice->SetTextureStageState(
		0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2));

	// No lightning in order for the colors to show
	gd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void Game::changeState(State newState)
{
	delete mCurrentState;
	
	if(newState == SANDBOX)	
		mCurrentState = new Sandbox();
	else if(newState == MENU)
		mCurrentState = new MainMenu();
	else if(newState == ABOUT)
		mCurrentState = new About();
	else if(newState == HELP)
		mCurrentState = new Help();
	
	mCurrentState->connect(&Game::changeState, this);
}

void Game::update(float t, float dt)
{
	mCamera->moveTarget(gDInput->mouseDX(), gDInput->mouseDY(), 0);
	mGfxStats->update(dt);

	mCurrentState->update(dt);

	// Some effects need to get get updated
	gGraphics->update(dt);
}
	
void Game::draw()
{
	HR(gd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));

	// Clears the backbuffer and depth buffer
	HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0));	// 0xffffffff

	HR(gd3dDevice->BeginScene());

	// Rendering of all sprites
	// Even for none sprites this means that everthing gets drawn in screen coordinates
	gGraphics->getSpriteHandler()->Begin(D3DXSPRITE_ALPHABLEND);

	// FPS hud
	mGfxStats->display();

	mCurrentState->draw();

	//gGraphics->drawHollowCircle(mBackgroundTexture, 400, 300, 200, 100);

	gGraphics->getSpriteHandler()->Flush();
	gGraphics->getSpriteHandler()->End();

	// Rendering af all textured quads
	gDInput->drawCursorPos();

	HR(gd3dDevice->EndScene());

	// Present the backbuffer
	HR(gd3dDevice->Present(0, 0, 0, 0));
}