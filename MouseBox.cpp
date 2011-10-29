#include "MouseBox.h"
#include "enums.h"
#include "Graphics.h"

MouseBox::MouseBox()
{
	// Ctor
	mBox = Rect(0, 0, 0, 0);
	mBoxingActive = false;
}

MouseBox::~MouseBox()
{
	// Dtor
}

void MouseBox::update(float dt)
{
	if(gDInput->mouseButtonPressed(1))
	{
		mBoxingActive = true;
		mCorner = gDInput->getCursorPos();
	}

	if(gDInput->mouseButtonReleased(1) && mBoxingActive)
	{
		mBoxingActive = false;
		callback();
	}

	if(gDInput->mouseDX() != 0 || gDInput->mouseDY() != 0)
	{
		// Update the mouse box, can be done in various ways
		float width = mCorner.x - gDInput->getCursorPos().x;
		float height = mCorner.y - gDInput->getCursorPos().y;
		Vector center = Vector(mCorner.x - width/2, mCorner.y - height/2);

		mBox.left = center.x - width/2;
		mBox.right = center.x + width/2;
		mBox.top = center.y - height/2;
		mBox.bottom = center.y + height/2;
	}
}
	
void MouseBox::draw()
{
	if(mBoxingActive)
		gGraphics->drawRect(mBox, false, D3DCOLOR_ARGB(255, 0, 0, 255));
}