/*==============================================
 * 2D Sprite for directX
 *
 * Written by Mark Gossage
 *==============================================*/
#pragma once
#include <d3d9.h>
/** \file SpriteUtils.h Sprite Drawing Utilities for GDEV engine.
The code here is designed to be able to draw sprites on the screen.
Its enough for good GUI's, but not really a full 2D sprite game.

A sprite is any bitmap image (bmp,jpg,tga,png), but not psd.
The images are draw with transparency (if required) and can be stretched to any size.
(Rotation/flipping is not currently supported, but it could be added if needed)

\par Important.
Many older graphics cards (such as the built in ones) require that 
textures width and height MUST be a multiple of 2 (2,4,8,16,32,64,128,256,512,1024,etc).
Therefore if a texture in not a power of two, it will be quietly scaled to the next size up 
(so a 640x480 will be quietly be scaled to 1024x512).

This can cause an image to look fine on one PC but messed up upon another
(usually with scale incorrect, or part of the image off the screen)
The easiest way to deal with to deal with this is to keep all textures of asuitable size
(just add a black/transparent border around them).

Alternatively, just scale to the inage to the correct size. 
For things like background images this is easy as you can use CGameWindow::GetWindowRect() to get the size.

\par References:
- http://www.toymaker.info/Games/html/sprites.html is a good reference on this
- XNA Game Studio provided some of the inspiration on how to design the functions.

*/

/** Creates a sprite batch drawing object.
In order to do any drawing, a SpriteBatch must be created and a texture loaded.
Within the Device->BeginScene()..EndScene(), there must then be a SpriteBatch->Begin()..End()
\code
mpd3dDevice->BeginScene();	// begin scene
mpSpriteBatch->Begin(D3DXSPRITE_ALPHABLEND);	// begin sprites
DrawSprite(mpSpriteBatch,mpSprite1,100,100);	// the sprite
mpSpriteBatch->End();	// end sprites
mpd3dDevice->EndScene();	// end scene
\endcode
\param pDev the D3D device
\returns a pointer to the SpriteBatch
\note once finished with: this must be Released()
*/
ID3DXSprite* CreateSpriteBatch(LPDIRECT3DDEVICE9 pDev);

/** loads a texture into memory.
Supports most common image formats (bmp,jpg,tga,png), but not psd.
It the format supports transparency (png), then the sprite will be transparent.
\note Certain older graphics cards require that The textures width and height MUST be a multiple of 2 
(2,4,8,16,32,64,128,etc).
Therefore if a texture in not a power of two, it will be quietly scaled to the next size up 
(so a 640x480 will be quietly be scaled to 1024x512)

\returns a pointer to the texture (will FAIL() if unable to load it)
\note once finished with: this must be Released()
*/
IDirect3DTexture9* LoadSpriteTex(LPDIRECT3DDEVICE9 pDev,const char *filename);

/** Loads a image into the Creates surface and loads an image onto it.
Supports most common image formats (bmp,jpg,tga,png), but not psd
\param transColour the colour which is to be made transparent
\note Certain older graphics cards require that The textures width and height MUST be a multiple of 2 
(2,4,8,16,32,64,128,etc).
Therefore if a textue in not a power of two, it will be quietly scaled to the next size up 
(so a 640x480 will be quietly be scaled to 1024x512)

\returns a pointer to the texture (will FAIL() if unable to load it)
\note once finished with: this must be Released()
*/
IDirect3DTexture9* LoadSpriteTexEx(LPDIRECT3DDEVICE9 pDev,const char *filename,D3DXCOLOR transcolour);

/** gets a rectangle of the sprites size.
This is the size of the texture in memory, not the original size of the bitmap.
\param pTex the sprite texture
\param[out] width,height the size of the sprite
*/
void GetSpriteSize(IDirect3DTexture9* pTex,int& width,int& height);

/** Draws the sprite with transparency into the screen.
This code must be within a SpriteBatch->Begin(D3DXSPRITE_ALPHABLEND)..End()
\see CreateSpriteBatch() for example of usage
\param pSpriteBat the spritebatch
\param pSpriteTex the texture
\param x,y the location of the top left corner of the sprite
\param col the colour tint to apply 
	(white means as is, black will be pure black, transparent will draw a completely transparent sprite)
*/
void DrawSprite(ID3DXSprite* pSpriteBat,IDirect3DTexture9* pSpriteTex,
					 int x, int y,
					 D3DCOLOR col=D3DCOLOR_XRGB(255,255,255));

/** Draws the sprite with transparency into the screen.
This code must be within a SpriteBatch->Begin(D3DXSPRITE_ALPHABLEND)..End()
\see CreateSpriteBatch() for example of usage
\param pSpriteBat the spritebatch
\param pSpriteTex the texture
\param x,y the location of the top left corner of the sprite
\param col the colour tint to apply 
	(white means as is, black will be pure black, transparent will draw a completely transparent sprite)
*/
void DrawSprite(ID3DXSprite* pSpriteBat,IDirect3DTexture9* pSpriteTex,
					 const POINT& p,
					 D3DCOLOR col=D3DCOLOR_XRGB(255,255,255));

/** Draws the sprite stretched with transparency into the screen.
This code must be within a SpriteBatch->Begin(D3DXSPRITE_ALPHABLEND)..End()
\see CreateSpriteBatch() for example of usage
\param pSpriteBat the spritebatch
\param pSpriteTex the texture
\param x,y the location of the top left corner of the sprite
\param w,h the width & height of the sprite
\param col the colour timt to apply 
	(white means as is, black will be pure black, transparent will draw a completely transparent sprite)
*/
void DrawSprite(ID3DXSprite* pSpriteBat,IDirect3DTexture9* pSpriteTex,
					 int x, int y, int w, int h,
					 D3DCOLOR col=D3DCOLOR_XRGB(255,255,255));

/** Draws the sprite stretched with transparency into the screen.
This code must be within a SpriteBatch->Begin(D3DXSPRITE_ALPHABLEND)..End()
\see CreateSpriteBatch() for example of usage
\param pSpriteBat the spritebatch
\param pSpriteTex the texture
\param to a rectangle containing the position and size of the sprite
\param col the colour timt to apply 
	(white means as is, black will be pure black, transparent will draw a completely transparent sprite)
*/
void DrawSprite(ID3DXSprite* pSpriteBat,IDirect3DTexture9* pSpriteTex,
					 const RECT& to,
					 D3DCOLOR col=D3DCOLOR_XRGB(255,255,255));

/** Resets the Renderstate after the spritebatch usage.
Sometimes when you use the spritebatch is messes up renderstate
\ref http://blogs.msdn.com/b/shawnhar/archive/2006/11/13/spritebatch-and-renderstates.aspx
This function resets the most likely renderstates
*/
void ResetRenderState(IDirect3DDevice9* pDev);

/// helper fn to make a rectangle
inline RECT MakeRect(int x1, int y1, int x2,int y2)
{
	RECT r={x1,y1,x2,y2};	return r;
}

/// returns true if x,y is in the rectangle.
/// \note RECT is a windows structure with the values left,top,right,bottom
bool InRect(int x, int y,const RECT& r);

/** Returns if a point is in a given area
\param x,y the point
\param ax,ay top left corner of the area
\param w,h width & height of the area
\returns true if the point is in the area (including on the edge)
*/
bool InArea(int x, int y,int ax,int ay,int w, int h);

/// returns true if x,y is over a sprite @ sx,sy
bool InSprite(int x,int y,int sx,int sy,IDirect3DTexture9* pTex);

/// returns true if p is over a sprite @ s
bool InSprite(const POINT& p,const POINT& s,IDirect3DTexture9* pTex);

/// returns true if rect's r1 & r2 intersect
/// could be used for sprite-sprite collision
bool IntersectRect(const RECT& r1,const RECT& r2);

/// returns true if two sprites overlap
bool IntersectSprite(int x1,int y1,IDirect3DTexture9* pTex1,
				   int x2,int y2,IDirect3DTexture9* pTex2);
