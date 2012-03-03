/*==============================================
 * font functions for directX
 *
 * Written by <YOUR NAME HERE>
 *==============================================*/
#pragma once
#include <d3dx9.h>
#include <string>
/** \file FontUtils.h Font Drawing Utilities for GDEV engine.
The code here is designed to be able to draw text on the screen.
Its enough for good GUI's, but not heavily optimised.

The font code uses any font installed on the PC.
If you wish, you can use a custom TTF font.
Provided you put the font with in the same directory os the executable
(or the project file if running in the IDE)

The font code can be used with a spritebatch (see CreateSpriteBatch()), which will
help the code to draw a bit faster.

\par References:
- Frank Luna: gives information on 3 ways to draw fonts, refer to the LPD3DXFONT information
- XNA game studio gave some idea's on the best way to write the functions.

*/

/** Creates and returns a D3DXFont object
\param name the font name
\param height the size (could well be its point)
\param bold if you want it bold

\note remember to Release() the font after use
*/
ID3DXFont* CreateD3DFont(LPDIRECT3DDEVICE9 pDevice,const char* name="Arial",int height=16,bool bold=false);

/** Draws a string in the point x,y in specified colour
This function can be called outside the the D3DDevice::Begin() End()
(unlike most of the other functions in this file)
\param pFont the font
\param text the text to print
\param x,y is the top left of the string
*/
void DrawD3DFont(ID3DXFont* pFont,const char* text,int x,int y,D3DCOLOR col=0xFF000000);

/// alternative version of DrawD3DFont which uses strings
void DrawD3DFont(ID3DXFont* pFont,const std::string& text,int x,int y,D3DCOLOR col=0xFF000000);

/** Draws a string in the point x,y in specified colour
\param pFont the font
\param [optional] pSpriteBat the spritebatch (can be NULL)
\param text the text to print
\param x,y is the top left of the string
\param the text colour
\param flags should be a combination of require flags to pass in
	eg DT_TOP|DT_LEFT|DT_NOCLIP for top left
	eg DT_TOP|DT_CENTER|DT_NOCLIP for top centre
	eg DT_TOP|DT_RIGHT|DT_NOCLIP for top right
	(you must have the DT_NOCLIP to get it working properly)
*/
void DrawD3DFontEx(ID3DXFont* pFont,ID3DXSprite* pSpriteBat,
				   const char* text,int x,int y,
				   D3DCOLOR col=0xFF000000,
				   DWORD flags=DT_TOP|DT_LEFT|DT_NOCLIP);

/** Most flexible text drawing routine: supports scale & rotation.
This code must be called between a pSpriteBat->Begin(D3DXSPRITE_ALPHABLEND)
and pSpriteBat->End()
\param pFont the font
\param pSpriteBat the spritebatch
  \warning MUST be a valid LPD3DXSPRITE, it cannot be NULL
\param text the text to print
\param pos its location
	\note if the values are not integer it will blend slightly
\param col the color to use
\param the offset for the font (often the centre of the sprite)
\param scale the amount to scale the font by
\param rotate rotation (in radians)
*/
void DrawD3DFontEx(ID3DXFont* pFont,ID3DXSprite* pSpriteBat,
				   const char* text,const D3DXVECTOR2& pos,
				   D3DCOLOR col,
				   const D3DXVECTOR2& origin=D3DXVECTOR2(0,0),
				   float scale=1,float rotate=0);

/** Gets the size for a chunk of text.
\note D3DXVECTOR2 is just a pair of x,y floats
*/
D3DXVECTOR2 GetTextSize(ID3DXFont* pFont,const char* text);
