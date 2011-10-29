#include <tchar.h>

#include "Graphics.h"
#include "Vertex.h"

Graphics::Graphics()
{
	// Makes all vertexes ready to use
	InitAllVertexDeclarations();

	mVB_texture = NULL;
	mVB_Rect	= NULL;

	// Maximum is 16 vertices
	if(FAILED(gd3dDevice->CreateVertexBuffer(sizeof(TextureVertex) *16, D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &mVB_texture, NULL)))
		MessageBox(0, "Error creating TextureVertex buffer", 0, 0);

	if(FAILED(gd3dDevice->CreateVertexBuffer(sizeof(RectVertex) *16, D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &mVB_Rect, NULL)))
		MessageBox(0, "Error creating RectVertex buffer", 0, 0);

	// Load the font
	loadFont();

	// Create the sprite handler
	HRESULT result = D3DXCreateSprite(gd3dDevice, &mSpriteHandler);
	if(result != D3D_OK)
		showMsg("Error creating sprite handler");

	checkDeviceCaps();

	// Build all the effects
	buildAllFX();
}

Graphics::~Graphics()
{
	ReleaseCOM(mVB_texture);
	ReleaseCOM(mVB_Rect);
	ReleaseCOM(mFont);
}

IDirect3DTexture9* Graphics::loadTexture(const char *fileName)
{
	IDirect3DTexture9 *texture;		// Texture to return
	D3DXIMAGE_INFO SrcInfo;			// Optional

	// Use a magenta colourkey
	D3DCOLOR colorkey = 0xFFFF00FF;

	// Load texture from file image
	if (FAILED(D3DXCreateTextureFromFileEx (gd3dDevice, fileName, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 0, 
        D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 
        colorkey, &SrcInfo, NULL, &texture)))
	{
		char buffer[256];
		sprintf(buffer, "Error loading texture: %s", fileName);
		showMsg(buffer);
		return NULL;
	}

	return texture;
}

IDirect3DTexture9* Graphics::loadTexture(string filename)
{
	return loadTexture(filename.c_str());
}

void Graphics::loadFont(void)
{
	D3DXFONT_DESC fontDesc;
	fontDesc.Height          = 18;
    fontDesc.Width           = 8;
    fontDesc.Weight          = 0;
    fontDesc.MipLevels       = 1;
    fontDesc.Italic          = false;
    fontDesc.CharSet         = DEFAULT_CHARSET;
    fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
    fontDesc.Quality         = 2;
    fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
	 _tcscpy(fontDesc.FaceName, _T("Bitstream Vera Sans Mono"));
    //_tcscpy(fontDesc.FaceName, _T("Arial")); Bitstream Vera Sans Mono

	HR(D3DXCreateFontIndirect(gd3dDevice, &fontDesc, &mFont));
}

bool Graphics::checkDeviceCaps()
{
	D3DCAPS9 caps;
	HR(gd3dDevice->GetDeviceCaps(&caps));

	// Check for vertex shader support
	if(caps.VertexShaderVersion < D3DVS_VERSION(2, 0))
		return false;
	
	// Check pixel shader support
	if(caps.PixelShaderVersion < D3DPS_VERSION(2, 0))
		return false;

	return true;
}

void Graphics::drawText(char *text, int x, int y, D3DCOLOR textColor)
{
	RECT pos = {x, y, 0, 0};
	HR(mFont->DrawText(0, text, -1, &pos, DT_NOCLIP, textColor));
}

LPD3DXSPRITE Graphics::getSpriteHandler()
{
	return mSpriteHandler;
}

void Graphics::buildAllFX()
{
	mGlowFX.build();
	mHollowFX.build();
	mBorderFX.build();
	mScrollFX.build();
}

void Graphics::update(float dt)
{
	mGlowFX.update(dt);
}

void Graphics::drawGlowTexture(IDirect3DTexture9* texture, float x, float y, int width, int height, float glowRadius, Vector glowSpot, float rotation)
{
	mGlowFX.draw(texture, x, y, width, height, glowRadius, glowSpot, rotation);
}

void Graphics::drawHollowCircle(IDirect3DTexture9* texture, float x, float y, float radius, float hollowRadius, float rotation)
{
	mHollowFX.draw(texture, x, y, radius, hollowRadius, rotation);
}

bool Graphics::drawTexturedShape(Shape shape, IDirect3DTexture9 *texture, Rect* srcRect, bool flipped)
{
	// can't go outside buffer size
	if(shape.pointList.size() > 16)
		return false;

	gd3dDevice->SetVertexDeclaration(TextureVertex::Decl);
	gd3dDevice->SetStreamSource(0, mVB_texture, 0, sizeof(TextureVertex));	

	Rect sourceRect;
	if(srcRect != NULL)
		 sourceRect = *srcRect;

	// Normalize to image coordinates
	if(srcRect != NULL)	{
		D3DXVECTOR3 center;
		D3DSURFACE_DESC desc;
		texture->GetLevelDesc(NULL, &desc);
		sourceRect.left /= desc.Width;
		sourceRect.right /= desc.Width;
		sourceRect.top /= desc.Height;
		sourceRect.bottom /= desc.Height;
	}
	else	{
		sourceRect.left = 0;
		sourceRect.right = 1;
		sourceRect.top = 0;
		sourceRect.bottom = 1;
	}

	if(flipped)	{
		if(sourceRect.left != 0)
			sourceRect.bottom = sourceRect.bottom;

		float tmp = sourceRect.left;
		sourceRect.left = sourceRect.right;
		sourceRect.right = tmp;
	}

	TextureVertex *vertices = 0;
	mVB_texture->Lock(0, 0, (void**)&vertices, 0);

	vertices[0].pos.x = shape.pointList[0].x + shape.getOrigin().x;
	vertices[0].pos.y = shape.pointList[0].y + shape.getOrigin().y;
	vertices[0].pos.z = 0.0f;
	vertices[0].tex0.x = sourceRect.left;
	vertices[0].tex0.y = sourceRect.top;

	vertices[1].pos.x = shape.pointList[1].x + shape.getOrigin().x;
	vertices[1].pos.y = shape.pointList[1].y + shape.getOrigin().y;
	vertices[1].pos.z = 0.0f;
	vertices[1].tex0.x = sourceRect.left;
	vertices[1].tex0.y = sourceRect.bottom;

	vertices[2].pos.x = shape.pointList[2].x + shape.getOrigin().x;
	vertices[2].pos.y = shape.pointList[2].y + shape.getOrigin().y;
	vertices[2].pos.z = 0.0f;
	vertices[2].tex0.x = sourceRect.right;
	vertices[2].tex0.y = sourceRect.bottom;

	vertices[3].pos.x = shape.pointList[3].x + shape.getOrigin().x;
	vertices[3].pos.y = shape.pointList[3].y + shape.getOrigin().y;
	vertices[3].pos.z = 0.0f;
	vertices[3].tex0.x = sourceRect.right;
	vertices[3].tex0.y = sourceRect.top;

	// Unlock the vertex buffer
	mVB_texture->Unlock();

	// Set texture
	gd3dDevice->SetTexture (0, texture);

	// Draw image
	gd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	gd3dDevice->SetTexture (0, NULL);
}

void Graphics::drawRect(float x, float y, int width, int height, bool solid, D3DCOLOR fillColor)
{
	// Setup the vertexbuffer and its declaration
	gd3dDevice->SetStreamSource(0, mVB_Rect, 0, sizeof(RectVertex));
	gd3dDevice->SetVertexDeclaration(RectVertex::Decl);	  

	Rect drawRect;
	drawRect.left = x-(width/2);
	drawRect.right = x+(width/2);
	drawRect.top = y-(height/2);
	drawRect.bottom = y+(height/2);

	RectVertex *vertices = 0;

	HR(mVB_Rect->Lock(0, 0, (void**)&vertices, 0));

	vertices[0].color = fillColor;
	vertices[0].pos.x = (float) drawRect.left;
	vertices[0].pos.y = (float) drawRect.top;
	vertices[0].pos.z = 0;

	vertices[1].color = fillColor;
	vertices[1].pos.x = (float) drawRect.right;
	vertices[1].pos.y = (float) drawRect.top;
	vertices[1].pos.z = 0; 

	vertices[2].color = fillColor;
	vertices[2].pos.x = (float) drawRect.right;
	vertices[2].pos.y = (float) drawRect.bottom;
	vertices[2].pos.z = 0; 

	vertices[3].color = fillColor;
	vertices[3].pos.x = (float) drawRect.left;
	vertices[3].pos.y = (float) drawRect.bottom;
	vertices[3].pos.z = 0;

	// Unlock the vertex buffer
	HR(mVB_Rect->Unlock());

	// Draw content in buffer
	if(solid)
		gd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	else
		gd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		  
	gd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);				
}

void Graphics::drawRect(Rect Rect, bool solid, D3DCOLOR fillColor)
{
	// Setup the vertexbuffer and its declaration
	gd3dDevice->SetStreamSource(0, mVB_Rect, 0, sizeof(RectVertex));
	gd3dDevice->SetVertexDeclaration(RectVertex::Decl);	  

	RectVertex *vertices = 0;

	HR(mVB_Rect->Lock(0, 0, (void**)&vertices, 0));

	vertices[0].color = fillColor;
	vertices[0].pos.x = (float) Rect.left;
	vertices[0].pos.y = (float) Rect.top;
	vertices[0].pos.z = 0;

	vertices[1].color = fillColor;
	vertices[1].pos.x = (float) Rect.right;
	vertices[1].pos.y = (float) Rect.top;
	vertices[1].pos.z = 0; 

	vertices[2].color = fillColor;
	vertices[2].pos.x = (float) Rect.right;
	vertices[2].pos.y = (float) Rect.bottom;
	vertices[2].pos.z = 0; 

	vertices[3].color = fillColor;
	vertices[3].pos.x = (float) Rect.left;
	vertices[3].pos.y = (float) Rect.bottom;
	vertices[3].pos.z = 0;

	// Unlock the vertex buffer
	HR(mVB_Rect->Unlock());

	// Draw content in buffer
	if(solid)
		gd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	else
		gd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		  
	gd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);			

	gd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void Graphics::drawTexture(IDirect3DTexture9 *texture, float x, float y, int width, int height, D3DCOLOR vertexColour, float rotate)
{
	Rect drawRect;
	drawRect.left = x - width/2;
	drawRect.right = x + width/2;
	drawRect.top = y - height/2;
	drawRect.bottom = y + height/2;
	
	gd3dDevice->SetVertexDeclaration(TextureVertex::Decl);
	gd3dDevice->SetStreamSource(0, mVB_texture, 0, sizeof(TextureVertex));	

	TextureVertex *vertices = 0;
	mVB_texture->Lock(0, 0, (void**)&vertices, 0);

	vertices[0].pos.z = 0;
	vertices[0].tex0.x = 0.0f;
	vertices[0].tex0.y = 0.0f;
	vertices[1].pos.z = 0;
	vertices[1].tex0.x = 1.0f;
	vertices[1].tex0.y = 0.0f;
	vertices[2].pos.z = 0;
	vertices[2].tex0.x = 1.0f;
	vertices[2].tex0.y = 1.0f;
	vertices[3].pos.z = 0;
	vertices[3].tex0.x = 0.0f;
	vertices[3].tex0.y = 1.0f;

	if(rotate != 0)
	{
		Rect rOrigin; 
		//Translate destination rect to be centered on the origin
		rOrigin.top = drawRect.top - y;
		rOrigin.bottom = drawRect.bottom - y;
		rOrigin.left = drawRect.left - x;
		rOrigin.right = drawRect.right - x; 
		//Rotate vertices about the origin 
		vertices[0].pos.x = rOrigin.left * cosf(rotate) - rOrigin.top * sinf(rotate); 
		vertices[0].pos.y = rOrigin.left * sinf(rotate) + rOrigin.top * cosf(rotate); 
		vertices[1].pos.x = rOrigin.right * cosf(rotate) - rOrigin.top * sinf(rotate);
		vertices[1].pos.y = rOrigin.right * sinf(rotate) + rOrigin.top * cosf(rotate); 
		vertices[2].pos.x = rOrigin.right * cosf(rotate) - rOrigin.bottom * sinf(rotate);
		vertices[2].pos.y = rOrigin.right * sinf(rotate) + rOrigin.bottom * cosf(rotate); 
		vertices[3].pos.x = rOrigin.left * cosf(rotate) - rOrigin.bottom * sinf(rotate);
		vertices[3].pos.y = rOrigin.left * sinf(rotate) + rOrigin.bottom * cosf(rotate);
		
		//Translate vertices to proper position 
		vertices[0].pos.x += x; 
		vertices[0].pos.y += y; 
		vertices[1].pos.x += x; 
		vertices[1].pos.y += y; 
		vertices[2].pos.x += x;
		vertices[2].pos.y += y;
		vertices[3].pos.x += x;
		vertices[3].pos.y += y;
	}
	else
	{
		vertices[0].pos.x = (float) drawRect.left;
		vertices[0].pos.y = (float) drawRect.top;

		vertices[1].pos.x = (float) drawRect.right;
		vertices[1].pos.y = (float) drawRect.top;

		vertices[2].pos.x = (float) drawRect.right;
		vertices[2].pos.y = (float) drawRect.bottom;

		vertices[3].pos.x = (float) drawRect.left;
		vertices[3].pos.y = (float) drawRect.bottom;
	}

	// Unlock the vertex buffer
	mVB_texture->Unlock();

	// Set texture
	gd3dDevice->SetTexture (0, texture);

	// Draw content in buffer
	gd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	gd3dDevice->SetTexture (0, NULL);
}

void Graphics::drawTexture(IDirect3DTexture9 *texture, Rect rect, D3DCOLOR vertexColour, float rotate)
{
	float width = rect.right - rect.left;
	float height = rect.bottom - rect.top;
	float x = rect.left + width/2;
	float y = rect.top + height/2;
	Rect drawRect = rect;
	
	gd3dDevice->SetVertexDeclaration(TextureVertex::Decl);
	gd3dDevice->SetStreamSource(0, mVB_texture, 0, sizeof(TextureVertex));	

	TextureVertex *vertices = 0;
	mVB_texture->Lock(0, 0, (void**)&vertices, 0);

	vertices[0].pos.z = 0;
	vertices[0].tex0.x = 0.0f;
	vertices[0].tex0.y = 0.0f;
	vertices[1].pos.z = 0;
	vertices[1].tex0.x = 1.0f;
	vertices[1].tex0.y = 0.0f;
	vertices[2].pos.z = 0;
	vertices[2].tex0.x = 1.0f;
	vertices[2].tex0.y = 1.0f;
	vertices[3].pos.z = 0;
	vertices[3].tex0.x = 0.0f;
	vertices[3].tex0.y = 1.0f;

	if(rotate != 0)
	{
		Rect rOrigin; 
		//Translate destination rect to be centered on the origin
		rOrigin.top = drawRect.top - y;
		rOrigin.bottom = drawRect.bottom - y;
		rOrigin.left = drawRect.left - x;
		rOrigin.right = drawRect.right - x; 
		//Rotate vertices about the origin 
		vertices[0].pos.x = rOrigin.left * cosf(rotate) - rOrigin.top * sinf(rotate); 
		vertices[0].pos.y = rOrigin.left * sinf(rotate) + rOrigin.top * cosf(rotate); 
		vertices[1].pos.x = rOrigin.right * cosf(rotate) - rOrigin.top * sinf(rotate);
		vertices[1].pos.y = rOrigin.right * sinf(rotate) + rOrigin.top * cosf(rotate); 
		vertices[2].pos.x = rOrigin.right * cosf(rotate) - rOrigin.bottom * sinf(rotate);
		vertices[2].pos.y = rOrigin.right * sinf(rotate) + rOrigin.bottom * cosf(rotate); 
		vertices[3].pos.x = rOrigin.left * cosf(rotate) - rOrigin.bottom * sinf(rotate);
		vertices[3].pos.y = rOrigin.left * sinf(rotate) + rOrigin.bottom * cosf(rotate);
		
		//Translate vertices to proper position 
		vertices[0].pos.x += x; 
		vertices[0].pos.y += y; 
		vertices[1].pos.x += x; 
		vertices[1].pos.y += y; 
		vertices[2].pos.x += x;
		vertices[2].pos.y += y;
		vertices[3].pos.x += x;
		vertices[3].pos.y += y;
	}
	else
	{
		vertices[0].pos.x = (float) drawRect.left;
		vertices[0].pos.y = (float) drawRect.top;

		vertices[1].pos.x = (float) drawRect.right;
		vertices[1].pos.y = (float) drawRect.top;

		vertices[2].pos.x = (float) drawRect.right;
		vertices[2].pos.y = (float) drawRect.bottom;

		vertices[3].pos.x = (float) drawRect.left;
		vertices[3].pos.y = (float) drawRect.bottom;
	}

	// Unlock the vertex buffer
	mVB_texture->Unlock();

	// Set texture
	gd3dDevice->SetTexture (0, texture);

	// Draw content in buffer
	gd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	gd3dDevice->SetTexture (0, NULL);
}

void Graphics::drawAnimation(IDirect3DTexture9* texture, float x, float y, int width, int height, Rect *srcRect, bool flipped)
{
	// Rect size as it weill be drawn on screen
	Rect drawRect;
	drawRect.left = x - width/2;
	drawRect.right = x + width/2;
	drawRect.top = y - height/2;
	drawRect.bottom = y + height/2;
	
	// Normalize to image coordinates
	if(srcRect != NULL)	{
		D3DXVECTOR3 center;
		D3DSURFACE_DESC desc;
		texture->GetLevelDesc(NULL, &desc);
		srcRect->left /= desc.Width;
		srcRect->right /= desc.Width;
		srcRect->top /= desc.Height;
		srcRect->bottom /= desc.Height;
	}
	else	{
		srcRect = new Rect();
		srcRect->left = 0;
		srcRect->right = 1;
		srcRect->top = 0;
		srcRect->bottom = 1;
	}

	// Flip texture?
	if(flipped)	{
		int tmp = srcRect->left;
		srcRect->left = srcRect->right;
		srcRect->right = tmp;
	}

	gd3dDevice->SetVertexDeclaration(TextureVertex::Decl);
	gd3dDevice->SetStreamSource(0, mVB_texture, 0, sizeof(TextureVertex));	

	TextureVertex *vertices = 0;
	mVB_texture->Lock(0, 0, (void**)&vertices, 0);

	vertices[0].pos.x = (float) drawRect.left;
	vertices[0].pos.y = (float) drawRect.top;
	vertices[0].pos.z = 0;
	vertices[0].tex0.x = srcRect->left;
	vertices[0].tex0.y = srcRect->top;

	vertices[1].pos.x = (float) drawRect.right;
	vertices[1].pos.y = (float) drawRect.top;
	vertices[1].pos.z = 0;
	vertices[1].tex0.x = srcRect->right;
	vertices[1].tex0.y = srcRect->top;

	vertices[2].pos.x = (float) drawRect.right;
	vertices[2].pos.y = (float) drawRect.bottom;
	vertices[2].pos.z = 0;
	vertices[2].tex0.x = srcRect->right;
	vertices[2].tex0.y = srcRect->bottom;

	vertices[3].pos.x = (float) drawRect.left;
	vertices[3].pos.y = (float) drawRect.bottom;
	vertices[3].pos.z = 0;
	vertices[3].tex0.x = srcRect->left;
	vertices[3].tex0.y = srcRect->bottom;

	// Unlock the vertex buffer
	mVB_texture->Unlock();

	// Set texture
	gd3dDevice->SetTexture (0, texture);

	// Draw content in buffer
	gd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	gd3dDevice->SetTexture (0, NULL);

	delete srcRect;
}

void Graphics::drawLine(Vector start, Vector end, D3DCOLOR color)
{
	// Setup the vertexbuffer and its declaration
	gd3dDevice->SetStreamSource(0, mVB_Rect, 0, sizeof(RectVertex));
	gd3dDevice->SetVertexDeclaration(RectVertex::Decl);	  

	RectVertex *vertices = 0;

	HR(mVB_Rect->Lock(0, 0, (void**)&vertices, 0));

	vertices[0].color = color;
	vertices[0].pos.x = start.x;
	vertices[0].pos.y = start.y;
	vertices[0].pos.z = 0;

	vertices[1].color = color;
	vertices[1].pos.x = end.x;
	vertices[1].pos.y = end.y;
	vertices[1].pos.z = 0; 

	// Unlock the vertex buffer
	HR(mVB_Rect->Unlock());

	// Draw content in buffer
	gd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		
	//gd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		  
	gd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, 2);			
}

void Graphics::drawBorderTexture(IDirect3DTexture9* texture, float x, float y, int width, int height, float border, float rotation)
{
	mBorderFX.draw(texture, x, y, width, height, border, rotation);
}

void Graphics::drawScrollFX(IDirect3DTexture9* texture, float x, float y, int width, int height, float offset, float viewDistance, float fadeLen)
{
	mScrollFX.draw(texture, x, y, width, height, offset, viewDistance, fadeLen);
}