#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <crtdbg.h>
#include <string>

#include "d3dUtil.h"
#include "Shape.h"
#include "Effects.h"

/// Wrapper with drawing functions
class Graphics
{
public:
	Graphics();
	~Graphics();

	/// Draws a solid or wireframed rectangle
	void drawRect(float x, float y, int width, int height, bool solid = true, D3DCOLOR fillColor = D3DCOLOR_ARGB( 255, 255 , 000, 000 ));	//:TODO: Fix z and solid!
	void drawRect(Rect rect, bool solid = true, D3DCOLOR fillColor = D3DCOLOR_ARGB( 155, 155, 000, 000 ));	
	
	/// Draws a textured rectangle - origin is top left
	void drawTexture(IDirect3DTexture9 *texture, float x, float y, int width, int height, D3DCOLOR vertexColour = 0xFFFFFFFF, float rotate = 0.0f);
	void drawTexture(IDirect3DTexture9 *texture, Rect rect, D3DCOLOR vertexColour = 0xFFFFFFFF, float rotate = 0.0f);

	/// Draws a part from texture atlas
	void drawAnimation(IDirect3DTexture9* texture, float x, float y, int width, int height, Rect *srcRect = NULL, bool flipped = false);

	/// Draws a textured shape, supports animation
	bool drawTexturedShape(Shape shape, IDirect3DTexture9 *texture, Rect *srcRect = NULL, bool flipped = false);

	/// Draws text using ID3DXFont
	void drawText(char *text, int x, int y, D3DCOLOR textColor = D3DCOLOR_ARGB(255,0,0,0));

	/// Draws a thin line
	void drawLine(Vector start, Vector end, D3DCOLOR color = D3DCOLOR_ARGB(255, 0, 0, 0));

	/// Draws a hollow circle
	void drawHollowCircle(IDirect3DTexture9* texture, float x, float y, float radius, float hollowRadius, float rotation = 0);

	/// Draws a rounded texture
	void drawCurvedTexture(IDirect3DTexture9* texture, float x, float y, int width, int height, float rounding, float rotation = 0);

	void drawGlowSpot(IDirect3DTexture9* texture, float x, float y, float radius);

	void drawGlowTexture(IDirect3DTexture9* texture, float x, float y, int width, int height, float glowRadius, Vector glowSpot, float rotation = 0);

	void drawScrollFX(IDirect3DTexture9* texture, float x, float y, int width, int height, float offset, float viewDistance, float fadeLen = 20.0f);

	// Shouldn't be a texture...
	void drawBorderTexture(IDirect3DTexture9* texture, float x, float y, int width, int height, float border, float rotation = 0);

	/// Loads and returns the specified texture
	IDirect3DTexture9* loadTexture(const char *filename);
	IDirect3DTexture9* loadTexture(string filename);

	/// Build all effects
	void buildAllFX();

	/// Returns the sprite handler
	LPD3DXSPRITE getSpriteHandler();

	/// Check device caps
	bool checkDeviceCaps();

	/// Some effects need to get updates
	void update(float dt);

	void loadFont(void);
private:
	IDirect3DVertexBuffer9* mVB_texture;
	IDirect3DVertexBuffer9* mVB_Rect;
	ID3DXFont* mFont;

	LPD3DXSPRITE mSpriteHandler;

	GlowFX			mGlowFX;
	HollowCircleFX	mHollowFX;
	BorderFX		mBorderFX;
	ScrollFX		mScrollFX;
};	// Class

extern Graphics* gGraphics;

#endif