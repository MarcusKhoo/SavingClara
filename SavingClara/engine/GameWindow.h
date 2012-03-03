/*==============================================
 * GameWindow class for GDEV Engine
 *
 * Written by Marcus Khoo
 *==============================================*/
#pragma once

#include <windows.h>
/** The CGameWindow class wrappers the Win32 window.

This class holds all the main functionlaity needed for opening up a window
of the desired size.

However, its not really designed to be used on its own. 
Instead use the CGameEngine class. 

\par References:
- Wendy Jones (or any other DirectX book) for the basic window stuff
- http://winprog.org/tutorial for a much more detailed Windows tutorial
- MSDN for details on capturing all the mouse messages
 - http://msdn.microsoft.com/en-us/library/ms645617%28VS.85%29.aspx mouse wheel code
 - http://msdn.microsoft.com/en-us/library/ms645616%28VS.85%29.aspx mouse move

*/
class CGameWindow
{
private:
	CGameWindow(const CGameWindow&);	/// NO COPYING
	void operator=(const CGameWindow&);	/// NO NO COPYING
public:
	/// Constructor, does effectivly nothing
	CGameWindow();
	/// Destructor, unused
	virtual ~CGameWindow();
	/** opens the window of the desired screen size.
	\param hInstance the instance value passed into WinMain
	\param width,height the desired size of the window
	\param fullscreen whether this should be a fullscreen window.
		if fullscreen is true, the window may not actually cover the whole screen
		but is will not have any of the usual titlebar, etc.
	*/
	bool OpenWin(HINSTANCE hInstance,int width,int height,bool fullscreen=false);
	/// closes the window
	void CloseWin();
	/// runs the window until its closed.
	/// not used, use CGameEngine::Go() instead
	virtual int Go(HINSTANCE hInstance);
	/** returns the window handle.
	This is useful when certain applications require the windows handle 
	in their initalisation.
	*/
	HWND GetWindow(){return mHWnd;}
	/// The instance. Just in case you need it...
	HINSTANCE GetInstance(){return mInstance;}
	/** returns current time (in seconds).
	Note: The time is does not start at zero, it often starts at 
	over one million. Therefore users should collect a start time
	and then make all times relative to that.
	\code
	float startTime=GetTime(); // the starting time
	...
	float now=GetTime()-startTime; // time elapsed since startTime
	\endcode
	*/
	static float GetTime();
	/// Returns a rectangle of the window size.
	/// Useful when you need to make an image be stretched into the window.
	RECT GetWindowRect(){return mSize;}
	/// Returns the aspect ratio for the window.
	float GetAspectRatio(){return (float(mSize.right))/mSize.bottom;}
	/// Sets the window title to anything you want.
	/// \param name the new window name
	void SetTitle(const char* name);

	/** \defgroup WinIo Input routines
	These routine rely on the normal Win32 API's to get the mouse and keyboard input.
	They are all static, so that they can be called from anywhere.
	\code
	if (CGameWindow::KeyPress('A'))
		...
	\endcode
	However if you in in a class dervied from CGameWindow (like you main app),
	you can simply call it directly
	\code
	if (KeyPress('A'))
		...
	\endcode
	\{ */

	/** returns true if a given key is down.
	\param vk the keycode to use: which could be
	- 'A'..'Z','0'..'9' for keypresses
	- VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN for cursor keys
	- VK_NEXT, VK_PRIOR, VK_HOME, VK_END for page down, page up, home, end 
	- VK_ENTER, VK_TAB, VK_ESCAPE, VK_SPACE as expected
	- VK_LBUTTON, VK_MBUTTON, VK_RBUTTON for mouse buttons
	- and many others (see WinUser.h)
	*/
	static bool KeyDown(int vk);
	/** returns true if a given key has been pressed.
	\param vk the keycode to use: see KeyPress() for a list of keycodes
	\warning if you try to call this function twice for the same keycode in the same game loop
	the second call will fail.
	\code
	// BAD code:
	// this condition works ok work ok
	if (KeyPress(VK_LBUTTON) && mouseOverButtonA)
		...
	// the KeyPress will ALWAYS return false
	if (KeyPress(VK_LBUTTON) && mouseOverButtonB)
		...
	\endcode
	You must instead use
	\code
	// correct code:
	if (KeyPress(VK_LBUTTON))
	{
		if (mouseOverButtonA)
			...
		if (mouseOverButtonB)
			...
	}
	\endcode
	*/
	static bool KeyPress(int vk);
	/** Clears the input.
	Because of the way KeyPress works it will return true if the key has been pressed any time
	since the last time it was called for that given key.
	So if the last time you called it was 20 miutes ago, it will return true
	if the key was pressed in the last 20 minutes (oops).

	This code clears the KeyPress 'memory' and stops this effect.
	This should only be called when needed, as if it is called too often it can disrupt the 
	KeyPress code, so that it always returns false!
	*/
	static void ClearKeyPress();

	/** Returns the mouses location in pixels.
	\returns POINT x,y the mouses location, relative to the window
	\note mouse tracking becomes irregular once the mouse if outside of the window
	*/
	static POINT GetMousePos();
	/** Returns amount mouse has moved.
	\returns POINT x,y amount mouse has moved (usually <100 pixels)
	\note mouse tracking becomes irregular once the mouse if outside of the window
	*/
	static POINT GetMouseDelta();
	
	/** returns the mouse wheel value.
	normally this is either -120,0 or +120
	\note calling this function will clear the previous value, 
	so like KeyPress(), only call this once per cycle 
	*/
	static int GetMouseWheel();

	/** Sets the mouses location reletive to the window:
	\param x,y the location to put the mouse
	*/
	static void SetMousePos(int x, int y);

	/// \}
	static const char* VERSION;	///< the version number of the Game Engine
protected:
	/// internal function to update input routines.
	/// must be called once/cycle in the Go() function
	void UpdateInput();
	/// internal function to adjust width & height of window.
	/// Designed to make the adjustments to account for the size of borders & title bar.
	void AdjustWindowSize(int& w,int& h,DWORD style);
	RECT mSize;	///< the window size
	HWND mHWnd;	///< the window handle
	HINSTANCE mInstance;			///< the instance
	static LARGE_INTEGER mTimerFreq;	///< timer frequency information
};
