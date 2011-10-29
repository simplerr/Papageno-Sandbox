#include "ScrollBox.h"
#include "Graphics.h"
#include "DirectInput.h"

ScrollBox::ScrollBox(float x, float y, int width, int height)	// :TODO: Automaticly calculate height
{
	// Set values
	mPos = Vector(x, y);
	mWidth = width;
	mHeight = height;
	mScrollRectTexture = gGraphics->loadTexture("imgs\\scroll_rect.bmp");
	mScrollButtontexture = gGraphics->loadTexture("imgs\\scroll_button.bmp");
	mBorder = 10;
	mSpacing = 20;
	mSpeed = 1;
	mFadeLen = 20.0f;
	mViewDist = height/2;

	int scrollWidth = 20;
	// Calculate scroll rect
	mScrollRect.right = mPos.x + mWidth/2 - mBorder;
	mScrollRect.left = mScrollRect.right - scrollWidth;
	mScrollRect.top = mPos.y - mHeight/2 + mBorder;
	mScrollRect.bottom = mPos.y + mHeight/2 - mBorder;

	int scrollHeight = 40;
	mScrollButton = Rect(mScrollRect.right - scrollWidth/2, mScrollRect.top + scrollHeight/2, (int)scrollWidth, (int)scrollHeight);

	mScrolling = false;
}
	
ScrollBox::~ScrollBox()
{
	ReleaseCOM(mScrollButtontexture);
	ReleaseCOM(mScrollRectTexture);
}

void ScrollBox::update(float dt)
{
	// Special scroll when the scroll rect was pressed and not the button
	bool rectPressed = false;

	// Find out if scrollbutton is moved
	if(gDInput->mouseButtonPressed(0) && mScrollButton.pointInside(gDInput->getCursorPos()))
		mScrolling = true;
	// If the scroll rect was pressed and not the button
	else if(gDInput->mouseButtonPressed(0) && mScrollRect.pointInside(gDInput->getCursorPos()))	{
		// Special case when the top or bottom on the scroll rect was pressed
		if(gDInput->getCursorPos().y < mScrollRect.top + mScrollButton.getHeight()/2)	// If the rect was pressed on a place where the button can't go
			scroll(mScrollRect.top + mScrollButton.getHeight()/2 - mScrollButton.getPos().y);	// Move the button to the top-most position
		else if(gDInput->getCursorPos().y > mScrollRect.bottom - mScrollButton.getHeight()/2)
			scroll(mScrollRect.bottom - mScrollButton.getHeight()/2 - mScrollButton.getPos().y);
		else
			scroll(gDInput->getCursorPos().y - mScrollButton.getPos().y);	// Standard

		mScrolling = true;
		rectPressed = true;
	}

	// Scroll when mouse is down and the rect weren't pressed (special case above)
	if(mScrolling && !rectPressed)	
		scroll(gDInput->mouseDY(), MOVED);

	// If the mouse was scrolled
	if(gDInput->mouseDZ() != 0)
		scroll(gDInput->mouseDZ()/3, SCROLLED);

	// No scrolling when the mouse is not held in (except for mouse scroll scrolling >_>)
	if(gDInput->mouseButtonReleased(0))
		mScrolling = false;
}
	
void ScrollBox::draw()
{
	// Draw the (border), scroll button and the scroll rect to the right
	gGraphics->drawTexture(mScrollRectTexture, mScrollRect);
	gGraphics->drawTexture(mScrollButtontexture, mScrollButton);

	for(int i = 0; i < mItemList.size(); i++)	{
		// Distance from item to the origin (middle)
		float offset = mItemList[i].getPos().y - mPos.y;

		gGraphics->drawScrollFX(mItemList[i].getTexture(), mItemList[i].getPos().x, mItemList[i].getPos().y, mItemList[i].getWidth(), mItemList[i].getHeight(), offset, mViewDist, mFadeLen);
	}
}

void ScrollBox::addItem(string textureSource, int width, int height)
{
	// Calculate the Y coordinate for the new item
	float y = 0;
	if(mItemList.size() != 0)	{
		// Y and height from the last item (the one above) :NOTE: You must add items from up and down
		float prevY = mItemList[mItemList.size()-1].getPos().y;
		float prevHeight = mItemList[mItemList.size()-1].getHeight();
		
		// New Y value
		y = prevY + prevHeight/2 + height/2 + mSpacing;
	}
	else	{
		y = mScrollRect.top + height/2 + 20;
		//y = mPos.y;
		
	}

	ScrollItem item(textureSource, mPos.x, y, width, height);
	mItemList.push_back(item);

	int totalHeight = 20;
	for(int i = 0; i < mItemList.size(); i++)
	{
		totalHeight += mItemList[i].getHeight() + 20;
	}

	if(totalHeight < mScrollRect.getHeight() - mScrollButton.getHeight())
		mSpeed = 0.0f;
	else	{
		totalHeight -= mScrollRect.getHeight();
		mSpeed = totalHeight / (mScrollRect.getHeight() - mScrollButton.getHeight());
	}
}
	
void ScrollBox::scroll(float dy, ScrollType type)
{
	// If the scroll button is moved or if the scroll rect is pressed
	if(type == MOVED)
	{
		// Temporary test rect
		Rect tmp = mScrollButton;
		tmp.move(0, dy);

		// Make sure the scroll button is inside the scroll rect
		if(tmp.top >= mScrollRect.top && tmp.bottom <= mScrollRect.bottom)	{
			mScrollButton.move(0, dy);
			for(int i = 0; i < mItemList.size(); i++)	{
				mItemList[i].scroll(-mSpeed*dy);
			}
		}
		else	{
			// :TODO: setCursorY is broken!
			//gDInput->setCursorY(gDInput->getCursorPos().y - gDInput->mouseDY());
		}
	}
	else if(type == SCROLLED)	// If the mouse scrolls
	{
		// Temporary test rect
		Rect tmp = mScrollButton;
		tmp.move(0, -dy/mSpeed);

		// Make sure the scroll button is inside the scroll rect
		if(tmp.top >= mScrollRect.top && tmp.bottom <= mScrollRect.bottom)	{
			mScrollButton.move(0, -dy/mSpeed);
			for(int i = 0; i < mItemList.size(); i++)	{
				mItemList[i].scroll(dy);
			}
		}
		else	{	// If it's not then find out how far it needs to move in order to be at the max or min position
			if(tmp.top < mScrollRect.top)	{
				float allowed = mScrollRect.top - mScrollButton.top;	
				scroll(allowed);
			}
			else if(tmp.bottom > mScrollRect.bottom)	{
				float allowed = mScrollRect.bottom - mScrollButton.bottom;
				scroll(allowed);
			}
		}
	}
}

void ScrollBox::setFadeLen(float fadeLen)
{
	mFadeLen = fadeLen;
}

ScrollItem::ScrollItem(string textureSource, float x, float y, int width, int height)
{
	mTexture = gGraphics->loadTexture(textureSource);
	mPos = Vector(x, y);
	mWidth = width;
	mHeight = height;
}
	
ScrollItem::~ScrollItem()
{

}

void ScrollItem::scroll(float dy)
{
	mPos.y += dy;
}

Vector ScrollItem::getPos()
{
	return mPos;
}

int ScrollItem::getWidth()
{
	return mWidth;
}
	
int ScrollItem::getHeight()
{
	return mHeight;
}

IDirect3DTexture9* ScrollItem::getTexture()
{
	return mTexture;
}