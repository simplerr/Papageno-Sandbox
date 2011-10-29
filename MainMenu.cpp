#include "MainMenu.h"
#include "Vector.h"
#include "Graphics.h"

MainMenu::MainMenu()
{
	mMenu = new Menu("MainMenu", MOUSE, HOR, 1);
	mMenu->setMenuBackground("none", 600, 500, 256, 512);
	mMenu->addMenuItem("sandbox", "imgs\\buttons\\sandbox_button.bmp", "imgs\\buttons\\sandbox_button_glow.bmp");
	mMenu->addMenuItem("about", "imgs\\buttons\\about_button.bmp", "imgs\\buttons\\about_button_glow.bmp");
	mMenu->addMenuItem("help", "imgs\\buttons\\help_button.bmp", "imgs\\buttons\\help_button_glow.bmp");
	
	mMenu->buildMenu2(256, 128);
	mMenu->connect(&MainMenu::menuHandler, this);

	mBkgdTexture = gGraphics->loadTexture("imgs\\pink_big_bkgd.bmp");
}
	
MainMenu::~MainMenu()
{
	ReleaseCOM(mBkgdTexture);
	delete mMenu;
}

void MainMenu::update(float dt)
{
	mMenu->update(gDInput->getCursorPos().x, gDInput->getCursorPos().y);
}
	
void MainMenu::draw()
{
	// Background texture
	gGraphics->drawGlowTexture(mBkgdTexture, 600, 400, 1200, 800, 1000, Vector(600, 400));

	// Menu
	mMenu->draw();
}

bool MainMenu::menuHandler(std::string name)
{
	if(name == "sandbox")
	{
		callback(SANDBOX);
	}
	else if(name == "about")
	{
		callback(ABOUT);
	}
	else if(name == "help")
	{
		callback(HELP);
	}

	return true;
}