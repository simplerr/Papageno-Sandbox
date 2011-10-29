#ifndef SCROLLBOX_H
#define SCROLLBOX_H

#include <string>
#include <vector>
#include "d3dUtil.h"
#include "Vector.h"
#include "Enums.h"
using namespace std;

enum ScrollType
{
	SCROLLED,
	MOVED
};

class ScrollItem
{
public:
	ScrollItem(string textureSource, float x, float y, int width, int height);
	~ScrollItem();

	void scroll(float dy);

	Vector getPos();
	int getWidth();
	int getHeight();

	IDirect3DTexture9* getTexture();
	
private:
	IDirect3DTexture9* mTexture;
	Vector mPos;
	int mWidth;
	int mHeight;
};

class ScrollBox
{
public:
	ScrollBox(float x, float y, int width, int height);	// :TODO: Automaticly calculate height
	~ScrollBox();

	void update(float dt);
	void draw();

	void addItem(string textureSource, int width, int height);
	void scroll(float dy, ScrollType type = MOVED);

	void setFadeLen(float fadeLen);
private:
	vector<ScrollItem> mItemList;
	Vector mPos;
	int mWidth;
	int mHeight;
	int mBorder;
	float mSpacing;
	float mSpeed;
	float mFadeLen;
	float mViewDist;
	bool mScrolling;

	IDirect3DTexture9* mScrollRectTexture;
	IDirect3DTexture9* mScrollButtontexture;
	Rect mScrollRect;
	Rect mScrollButton;
};

#endif