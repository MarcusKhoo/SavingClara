/*==============================================
 * GameEngine class for GDEV Engine
 *
 * Written by <YOUR NAME HERE>
 *==============================================*/
#pragma once

#include "GameWindow.h"	// base class
// include directx9
#include <d3d9.h>

/** The CGameEngine class provides a wrapper around D3D and Win32.

All games/application just derive from this class and override the following functions:
- Startup() this is all the setting up
- Update() this contains all the game logic
- Draw() the drawing routines
- Shutdown() disposing of all the resources

To write a game, all you have to do is derive a class from CGameEngine, 
and then call the Go() function in your programs WinMain.

eg.
\code
#include "GameEngine.h"

class MyGame: public CGameEngine
{
public:
	bool Startup();
	void Update(float dt);
	void Draw(float dt);
	void Shutdown();
	// add the all variables here
};

bool MyGame::Startup()
{
	bool fullscreen=true;
	if (!StartupWinD3D(640,480,fullscreen))
		return false;	// error
	...
}
void MyGame::Update(float dt)
{
	...
}
void MyGame::Draw(float dt)
{
	...
}
void MyGame::Shutdown()
{
	...
	ShutdownD3D();
}

int WINAPI WinMain(HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance, 
				   LPTSTR lpCmdLine, int nCmdShow)
{
	MyGame game;
	return game.Go(hInstance);	// runs everything
}
\endcode

*/
class CGameEngine: public CGameWindow
{
public:
	/// Constructor.
	/// Don't put any code in here, except inital values, put it inside Startup()
	CGameEngine();
	/// Destructor.
	/// Don't put any code in here, put it inside Shutdown()
	virtual ~CGameEngine();
	/** main game engine loop.
	Basically what this does is call Startup,Update,Render & Shutdown in that order.
	All your code should be added into one of these functions.
	You shouldn't need to modify the Go function, but just in case...
	*/
	virtual int Go(HINSTANCE hInstance);
	/// gets the frame rate
	float GetFps(){return mFPS;}
	/// Shows the frame rate on the title of the window.
	/// this function must be called every cycle
	void ShowFrameRate(const char* title=CGameWindow::VERSION);
	/// returns the D3D object (not used too much)
	IDirect3D9* GetD3D(){return mpD3D;}
	/// returns the D3D Device (this is the function you need)
	IDirect3DDevice9* GetDevice(){return mpDevice;}
protected:	// internal functions
	/** The user startup function, All your startup code should go here.
	The first could of lines of code should be calling StartupD3D()
	(if D3D is not setup by the end of this function the application will FAIL).

	\return whether everything went ok.
		If your code returns a false, Shutdown() will be immidately called and the app will end.

	default behaviour is 640x480 window (but you will want to override this)
	*/
	virtual bool Startup();
	/** The user update function, all game logic goes here.
	\param dt the number of second elapsed since the last time this function was called (usually 1/60 seconds)
		If you want the absolute time use CGameWindow::GetTime()
	*/
	virtual void Update(float dt);
	/** The user draw function, all display goes here.
	\param dt the number of second elapsed since the last time this function was called (usually 1/60 seconds)

	The code you have should include the normal Clear, BeginScene, EndScene, Present flow that you find in Direct3D applications.
	*/
	virtual void Draw(float dt);
	/** the user shutdown function, all shutdown code goes here.
	Default behaviour calls ShutdownD3D().
	Your code should SAFE_DELETE or SAFE_RELEASE all the objects required and then call ShutdownD3D();
	*/
	virtual void Shutdown();
	/** starts D3D at the required mode.
	\param width,height the size of screen required.
	\note will support wide screen requests
	\param fullscreen for choosing windowed or full screen mode.
	\return if able to setup D3D
	\note this code will support 'silly' screen modes (eg 1024x128) when in windowed mode 
		but will not work when requesting this in full screen mode
	\warning you must have an open window before you call this function
	*/
	bool StartupD3D(int width,int height,bool fullscreen);
	/// ends D3D & shuts down all the D3D code
	void ShutdownD3D();
	/// Opens a window and starts D3D in a single call.
	/// parameters are the same as CGameWindow::Open() and CGameEngine::StartupD3D()
	bool StartupWinD3D(int width,int height,bool fullscreen=false);
	/** Called when the D3DDevice have been lost.
	If you want you program to survive an Alt-Tab when in full screen mode, 
	you MUST override this function.
	Its default behaviour is to print a neat message and exit.

	When you Alt-Tab from a DX fullscreen app or change screen mode,
	DX will clear the graphics memory, and you will lose textures etc.
	When your application is switched back to you will need to reset/reload all the lost things.
	This is performed in this function.

	What this function should be is call Reset on mpDevice and then 
	restore/reset/reload all graphics object which have been affected.
	\code
    ... // neatly close any assets which need it
	ResetDevice(); // reset the device
    ... // neatly re-initialise any assets which need it
    ... // setup any default D3D settings
		// eg camera, lights & other options
	*/
	virtual void OnLostDevice();

	/// returns if the app is full screen
	bool IsFullScreen(){return !mD3Dpp.Windowed;}
	/// sets the app to full screen or windowed.
	/// \warn only works if OnLostDevice() has been overridden properly
	void SetFullScreen(bool fullscreen);
	/// toggle full screen mode.
	/// \warn only works if OnLostDevice() has been overridden properly
	void ToggleFullScreen();
protected:
	/// performs the check to see if device is lost & calls OnLostDevice()
	bool CheckLostDevice();
	/// Resets the D3D device.
	/// should only be called within OnLostDevice()
	/// \warn if some component have not be closed properly this will FAIL()
	void ResetDevice();	
	D3DPRESENT_PARAMETERS	mD3Dpp;				///< the Direct3D presentation parameters
	LPDIRECT3D9             mpD3D;				///< the Direct3D Object
	LPDIRECT3DDEVICE9       mpDevice;			///< the Direct3D Device
	float mFPS;	///< used for FPS computation
};
