#ifndef MAINMENU_H
#define MAINMENU_H

#include "Menu.h"
#include "GameState.h"

class MainMenu : public GameState
{
public:
	MainMenu();
	~MainMenu();

	void update(float dt);
	void draw();
	bool menuHandler(std::string name);
private:
	IDirect3DTexture9* mBkgdTexture;
	Menu* mMenu;
};

#endif