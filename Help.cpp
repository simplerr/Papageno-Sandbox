#include "Help.h"
#include "GameState.h"
#include "ScrollBox.h"

Help::Help()
{
	mBkgdTexture = gGraphics->loadTexture("imgs\\help_bkgd.bmp");
	mHeader = gGraphics->loadTexture("imgs\\help_header.bmp");
	mScrollBox = new ScrollBox(600, 400, 800, 600);
	mScrollBox->setFadeLen(20.0f);
	mScrollBox->addItem("imgs\\help.bmp", 700, 2000);
	/*mScrollBox->addItem("fisk_erik.bmp", 500, 200);
	mScrollBox->addItem("fisk_erik.bmp", 500, 200);
	mScrollBox->addItem("help_page.bmp", 300, 100);
	mScrollBox->addItem("fisk_erik.bmp", 500, 200);
	mScrollBox->addItem("help_page.bmp", 300, 100);
	mScrollBox->addItem("fisk_erik.bmp", 500, 200);*/
}

Help::~Help()
{
	ReleaseCOM(mBkgdTexture);
	ReleaseCOM(mHelpTexture);
	ReleaseCOM(mHeader);
	delete mScrollBox;
}

void Help::update(float dt)
{
	mScrollBox->update(dt);

	if(gDInput->keyPressed(DIK_ESCAPE))
		callback(MENU);
}

void Help::draw()	
{
	// Background
	gGraphics->drawGlowTexture(mBkgdTexture, 600, 400, 1200, 800, 1000, Vector(600, 400));

	gGraphics->drawTexture(mHeader, 600, 60, 300, 100);

	mScrollBox->draw();
}