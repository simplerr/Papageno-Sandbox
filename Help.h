#ifndef HELP_H
#define HELP_H

#include "Graphics.h"
#include "DirectInput.h"
#include "GameState.h"
class ScrollBox;

class Help	: public GameState
{
public:
	Help();
	~Help();

	void update(float dt);
	void draw();
private:
	// Help texture etc...
	IDirect3DTexture9*	mBkgdTexture;
	IDirect3DTexture9*	mHelpTexture;
	IDirect3DTexture9*	mHeader;
	ScrollBox* mScrollBox;
};

#endif