#ifndef ABOUT_H
#define ABOUT_H

#include "Graphics.h"
#include "DirectInput.h"
#include "GameState.h"
#include "ScrollBox.h"

class About	: public GameState
{
public:
	About();
	~About();

	void update(float dt);
	void draw();
private:
	// About texture etc...
	IDirect3DTexture9*	mBkgdTexture;
	IDirect3DTexture9*	mHeader;
	ScrollBox*			mScrollBox;
};

#endif