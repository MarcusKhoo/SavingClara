/*==============================================
 * Quick'n'Dirty drawing routines
 *
 * Written by Mark Gossage
 *==============================================*/
#pragma once
#include <d3d9.h>
#include <d3dx9.h>

/** \file QDraw.h Quick'n'Dirty drawing routines.
This is very simple geometry drawing routines. Its designed for guildlines or information.
Not for a serious graphics, or for production code.
If you want to do this properly, use a vertex buffer.

However its quite handy for drawing lines of sight or bounding spheres.

\note ALL code turns on lighting 
\note ALL code (Except QDrawAxisCurrent) will change the World matrix.

*/

/// Draws a line between two points.
void QDrawLine(IDirect3DDevice9* pDev,const D3DXVECTOR3& p1,const D3DXVECTOR3& p2,D3DCOLOR col=D3DCOLOR_XRGB(255,255,0));

/// Draws a sphere of a given radius at a given place.
/// Very good for bounding spheres
void QDrawSphere(IDirect3DDevice9* pDev,const D3DXVECTOR3& pos,float radius,D3DCOLOR col=D3DCOLOR_XRGB(255,255,0));

/// Draws a cube of a given width at a given place.
void QDrawBox(IDirect3DDevice9* pDev,const D3DXVECTOR3& pos,float size,D3DCOLOR col=D3DCOLOR_XRGB(255,255,0));

/// Draws a cube of a given size at a given place.
void QDrawBox(IDirect3DDevice9* pDev,const D3DXVECTOR3& pos,const D3DXVECTOR3& size,D3DCOLOR col=D3DCOLOR_XRGB(255,255,0));


/// Draws an Axis using the current world matrix.
/// \note This is the only function that uses the world matrixes value without changing it
void QDrawAxisCurrent(IDirect3DDevice9* pDev,float size=1);


/// Draws an Axis at the specified point in space.
void QDrawAxis(IDirect3DDevice9* pDev,float size=1,const D3DXVECTOR3& pos=D3DXVECTOR3(0,0,0));
