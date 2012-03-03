/*==============================================
 * utility functions for GDEV Engine
 *
 * Written by <YOUR NAME HERE>
 *
 *==============================================*/
#pragma once
#ifndef _CRT_SECURE_NO_DEPRECATE 
#define _CRT_SECURE_NO_DEPRECATE // remove a VS2005 warning
#endif
#ifndef _CRT_SECURE_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS // remove a VS2005 warning
#endif
#pragma warning(disable : 4996 4018)

// memory debugging
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <string.h>
#include <d3dx9.h>
// Assorted useful game utils

/** Code to report memory leaks.
Simply add this code at the beginning of Winmain
\code
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine, int nCmdShow)
{
	ENABLE_LEAK_DETECTION();	// add here

	MyGame eng;
	return eng.Go(hInstance);	// runs everything
}
\endcode
At the end it will report memory leaks.
It does not report COM leaks (DirectX stuff), just normal C++ stuff.
*/
#define ENABLE_LEAK_DETECTION()\
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );\
	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG|_CRTDBG_MODE_WNDW );


/** Auto deletion/releasing macros.
Taken from DxUtils.h  Copyright (c) Microsoft Corporation. All rights reserved

What you do is at the shutdown, when you want to delete an object use:
\code
  //delete mPointer; // dont do this
  SAFE_DELETE(mPointer); // do this instead

  // or if is a COM object (any of the DirectX objects)
  //mpObject->Release(); // dont do this
  SAFE_RELEASE(mpObject); // do this instead
\endcode
What it does is delete the object and set its pointer to NULL.
That way if you try to do it again, it will not double delete.
*/
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
/// as per SAFE_DELETE() but for an array
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
/// as per SAFE_DELETE() but for a COM (DirectX) object
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

/// converts degrees to radians
#define D2R(x) D3DXToRadian(x)
/// converts radians to degrees
#define R2D(x) D3DXToDegree(x)

/// returns a random number between 0..1 (inclusive)
float randf();
/// returns a random number between a..b (inclusive)
float randf(float a,float b);
/// returns a random integer between 0..n-1 (inclusive)
int randi(int n);
/// returns a random integer between a..b (inclusive)
int randi(int a,int b);


inline float NormalizeAngle(float ang)
{
	if (ang>D3DX_PI)	ang-=D3DX_PI*2;
	if (ang<-D3DX_PI)	ang+=D3DX_PI*2;
	return ang;
}
inline float GetDirection(D3DXVECTOR3 dir)
{
	return atan2(dir.x,dir.z);
}
inline float GetDeltaDirection(float dira, float dirb)
{
	return fabs(NormalizeAngle(dira-dirb));
}
/** setup camera FOV 
\param pd3dDevice the d3d
\param fov the field of view (radians)
\param aspect ratio (useally 640./480.)
\param nearDist,farDist the near & far distances
*/
void CameraInit(LPDIRECT3DDEVICE9 pd3dDevice,float fov=D3DX_PI/4, float aspect=1.333f,float nearDist=1.f,float farDist=1000.f);

/** setup a camera lookat
\param pd3dDevice the d3d
\param pos where the camera is
\param tgt wherethe camera looks at
\param up the up vector
*/
void CameraLookAt(LPDIRECT3DDEVICE9 pd3dDevice,D3DXVECTOR3 pos,D3DXVECTOR3 tgt,D3DXVECTOR3 up=D3DXVECTOR3(0.0f, 1.0f, 0.0f));

/** Sets a bunch of default D3D options to get the D3D into a usable state
*/
void SetupDefaultD3DOptions(LPDIRECT3DDEVICE9 pd3dDevice, bool lights=true);

/// The identity Matrix
const D3DXMATRIX IDENTITY_MAT=*D3DXMatrixIdentity(&D3DXMATRIX());

/// resets the world matrix to identity
void ResetWorldMatrix(LPDIRECT3DDEVICE9 pDev);

/** Sets up a linear fog effect.
\param colour the colour of the fog
\param start the distance at which the fog begins
\param end the distance at which the fog is full
Objects nearer than 'begin' will look normal, 
Objects further than 'end' will be hidden by fog
Object in between will be partially obscured
*/
void SetupLinearFog(LPDIRECT3DDEVICE9 pd3dDevice, D3DCOLOR colour, float start,float end, DWORD Mode, float Density, bool UseRange);

////////////////////////////////////////////////////////////////////////
// the following code is takens from Frank Luna's book
// Introduction to 3D Game Programming with DirectX 9.0
// Frank Luna (C) All Rights Reserved
// some have been slightly modified by Mark Gossage (mainly pointers => references)

//
// Colors
//
const D3DXCOLOR      WHITE_COL( D3DCOLOR_XRGB(255, 255, 255) );
const D3DXCOLOR      BLACK_COL( D3DCOLOR_XRGB(  0,   0,   0) );
const D3DXCOLOR        RED_COL( D3DCOLOR_XRGB(255,   0,   0) );
const D3DXCOLOR      GREEN_COL( D3DCOLOR_XRGB(  0, 255,   0) );
const D3DXCOLOR       BLUE_COL( D3DCOLOR_XRGB(  0,   0, 255) );
const D3DXCOLOR     YELLOW_COL( D3DCOLOR_XRGB(255, 255,   0) );
const D3DXCOLOR       CYAN_COL( D3DCOLOR_XRGB(  0, 255, 255) );
const D3DXCOLOR    MAGENTA_COL( D3DCOLOR_XRGB(255,   0, 255) );
const D3DXCOLOR    SKY_COL( D3DCOLOR_XRGB(64,128, 255) );
const D3DXCOLOR    CORNFLOWER_BLUE_COL( D3DCOLOR_XRGB(100, 149, 237) );

/// creates a D3DMATERIAL9 material
/// taken from Frank Luna's book
D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);

const D3DMATERIAL9 WHITE_MTRL  = InitMtrl(WHITE_COL, WHITE_COL, WHITE_COL, BLACK_COL, 2.0f);
const D3DMATERIAL9 RED_MTRL    = InitMtrl(RED_COL, RED_COL, RED_COL, BLACK_COL, 2.0f);
const D3DMATERIAL9 GREEN_MTRL  = InitMtrl(GREEN_COL, GREEN_COL, GREEN_COL, BLACK_COL, 2.0f);
const D3DMATERIAL9 BLUE_MTRL   = InitMtrl(BLUE_COL, BLUE_COL, BLUE_COL, BLACK_COL, 2.0f);
const D3DMATERIAL9 YELLOW_MTRL = InitMtrl(YELLOW_COL, YELLOW_COL, YELLOW_COL, BLACK_COL, 2.0f);

//
// Lights
//
D3DLIGHT9 InitDirectionalLight(const D3DXVECTOR3& direction, const D3DXCOLOR& color);
D3DLIGHT9 InitPointLight(const D3DXVECTOR3& position,const D3DXCOLOR& color);
D3DLIGHT9 InitSpotLight(const D3DXVECTOR3& position, const D3DXVECTOR3& direction, const D3DXCOLOR& color);

////////////////////////////////////////////////////////////////////////
// basic movement

/// returns a vector of movement based upon the keys passed in
/// \param keys an array of which keys to use
///		the order is -x,+x,-y,+y,-z+z
///		you can use a zero in the array for not-used
/// \return the D3DXVECTOR3 with the information
D3DXVECTOR3 GetKeyboardVector(const int keys[6]);

// standard arrays to pass into the GetKeyboardVector() function
// note the order, -x,+x,-y,+y,-z+z
// for movement you want z(forward), x(right) & possibly y(up)
const int WSAD_KEYS[]={'A','D',0,0,'S','W'};
const int WSADQE_KEYS[]={'A','D','E','Q','S','W'};
const int CURSOR_KEYS[]={VK_LEFT,VK_RIGHT,0,0,VK_DOWN,VK_UP};
const int CURSOR_PG_KEYS[]={VK_LEFT,VK_RIGHT,VK_PRIOR,VK_NEXT,VK_DOWN,VK_UP};
// for turning you want x(yaw) & y (pitch), but probably not z(roll)
const int CURSOR_HPR_KEYS[]={VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN,0,0};

/// gets mouse movement for rotation or yaw & pitch
/// \param sensitive the sensitiviy of the mouse (divided by the value, 20 is very sentiive, 100 is not)
/// \param resetMouse whethe to reset the mouses position
D3DXVECTOR3 GetMouseTurnVector(float sensitive=50,bool resetMouse=false);

#include <stdio.h>
#include <ostream>
/** additional helper for dealing with D3DVECTOR3()
using an advanced C++ feature known as operator overloading.
*/
inline std::ostream& operator<<(std::ostream& os,const D3DXVECTOR3& v)
{
	char buff[80];
	_snprintf(buff,80,"(%.2f %.2f %.2f)",v.x,v.y,v.z);
	return os<<buff;
}