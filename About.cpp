#include "About.h"
#include "GameState.h"

About::About() 
{
	mBkgdTexture = gGraphics->loadTexture("imgs\\about_bkgd.bmp");
	mHeader = gGraphics->loadTexture("imgs\\about_header.bmp");
	mScrollBox = new ScrollBox(600, 400, 800, 600);
	mScrollBox->addItem("imgs\\about.bmp", 700, 600);
}

About::~About()
{
	ReleaseCOM(mBkgdTexture);
	ReleaseCOM(mHeader);
	delete mScrollBox;
}

void About::update(float dt)
{
	mScrollBox->update(dt);

	if(gDInput->keyPressed(DIK_ESCAPE))
		callback(MENU);
}

void About::draw()	
{
	// Background
	gGraphics->drawGlowTexture(mBkgdTexture, 600, 400, 1200, 800, 1000, Vector(600, 400));

	mScrollBox->draw();

	gGraphics->drawTexture(mHeader, 600, 60, 300, 100);
}