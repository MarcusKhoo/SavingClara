/*==============================================
 * GameWindow class for GDEV engine
 *
 * Written by Marcus Khoo
 *==============================================*/
#include "GameWindow.h"
#include "Fail.h"
#include <windowsx.h>

#pragma comment(lib, "winmm.lib") // instruction to the linker

// engine version
const char* CGameWindow::VERSION="GDEV Engine v3.0";

const DWORD STYLE_GAMEWINDOW=(WS_CAPTION|WS_OVERLAPPED |WS_SYSMENU);
// or use WS_OVERLAPPEDWINDOW

const DWORD STYLE_GAMEFULL=(WS_POPUP);
///*WS_EX_TOPMOST |*/ WS_POPUP | WS_SYSMENU | WS_VISIBLE;

/// there are hidden global variables for storing the mouse position
POINT gMousePos={0,0},gOldMousePos={0,0};;
int gMouseZ=0;

LARGE_INTEGER CGameWindow::mTimerFreq;	// static variable

/*********************************************************************
* WndProc
*********************************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_MOUSEMOVE:	// capture mouse input information
			gMousePos.x = GET_X_LPARAM(lParam); 
			gMousePos.y = GET_Y_LPARAM(lParam);
			break;
		case WM_MOUSEWHEEL:	// capture mouse wheel information
			gMousePos.x = GET_X_LPARAM(lParam); 
			gMousePos.y = GET_Y_LPARAM(lParam);
			gMouseZ = GET_WHEEL_DELTA_WPARAM(wParam);
			break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

CGameWindow::CGameWindow()
{
	mHWnd=0;
	// init the high performance timer
	QueryPerformanceFrequency(&mTimerFreq);
}
CGameWindow::~CGameWindow()
{
	// nothing to do
}

bool CGameWindow::OpenWin(HINSTANCE hInstance,int width,int height,bool fullscreen)
{
	mInstance=hInstance;
	// set the rect size
	mSize.left=mSize.top=0;
	mSize.bottom=height;
	mSize.right=width;

	// adjust to include the titlebar etc.
	AdjustWindowSize(width,height,STYLE_GAMEWINDOW);

	WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW; // the class style
	wcex.lpfnWndProc = (WNDPROC)WndProc; // the window procedure callback
	wcex.cbClsExtra = 0; // extra bytes to allocate for this class
	wcex.cbWndExtra = 0; // extra bytes to allocate for this instance
	wcex.hInstance = hInstance; // handle to the application instance5
	wcex.hIcon = 0; // icon to associate with the application
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);// the default cursor
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); // the background color
	wcex.lpszMenuName = NULL; // the resource name for the menu
	wcex.lpszClassName = "GDEV Engine V1.0"; // the class name being created
	wcex.hIconSm = 0; // the handle to the small icon
	RegisterClassEx(&wcex);

	// if the application is to be fullscreen then set the window styles to
	// either STYLE_GAMEFULL or STYLE_GAMEWINDOW
	// (defined at top of file)
	DWORD      dwWindowStyle;			// this holds the window style
	if (fullscreen)
		dwWindowStyle = STYLE_GAMEFULL;
	else
		dwWindowStyle = STYLE_GAMEWINDOW;

	// create the window
	mHWnd = CreateWindow("GDEV Engine V1.0", 
						 "GDEV Engine V1.0", 
						 dwWindowStyle, // the type
						 CW_USEDEFAULT, CW_USEDEFAULT, // the position
						 width, height, // the size (640X480 + a bit for the border)
						 NULL, NULL, hInstance, NULL);

	if (!mHWnd)
		return false;

	ShowWindow(mHWnd, SW_RESTORE);
	UpdateWindow(mHWnd);
	// force window to top
	SetWindowPos(mHWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED );
	return true;	
}

void CGameWindow::AdjustWindowSize(int& w,int& h,DWORD style)
{
	RECT r={0,0,w,h};
	AdjustWindowRect(&r,style,false);
	w=r.right-r.left;
	h=r.bottom-r.top;
}

void CGameWindow::CloseWin()
{
	// this seems ok for now
	SendMessage(mHWnd,WM_CLOSE,0,0);
	// or use:
	// PostQuitMessage(0);
}

int CGameWindow::Go(HINSTANCE hInstance)
{
	// Main message loop:
    MSG msg;
    ZeroMemory( &msg, sizeof(msg) );

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
		
	return (int) msg.wParam;
}

float CGameWindow::GetTime()
{
	LARGE_INTEGER timer;
	QueryPerformanceCounter(&timer);
	return (float)timer.QuadPart / mTimerFreq.QuadPart;
}

void CGameWindow::SetTitle(const char* name)
{
	SetWindowText(mHWnd,name);
}

bool CGameWindow::KeyDown(int vk)
{
	return ((GetAsyncKeyState(vk)& 0x8000)!=0);
}

bool CGameWindow::KeyPress(int vk)
{
	return ((GetAsyncKeyState(vk)& 0x01)!=0);
}

void CGameWindow::ClearKeyPress()
{
	for(int i=0;i<255;i++)
		GetAsyncKeyState(i);
}

POINT CGameWindow::GetMouseDelta()
{
	POINT p={gMousePos.x-gOldMousePos.x,gMousePos.y-gOldMousePos.y};
	return p;
}

POINT CGameWindow::GetMousePos()
{
	return gMousePos;
}

int CGameWindow::GetMouseWheel()
{
	return gMouseZ;
}
void CGameWindow::UpdateInput()
{
	gMouseZ=0;
	gOldMousePos=gMousePos;
}
	
void CGameWindow::SetMousePos(int x, int y)
{
	gMousePos.x=x;	gMousePos.y=y;	// update the mouse pos variable for delta tracking
	//gOldMousePos=gMousePos;
	// this is a bit messy as we have to include the size of the window & borders
	HWND wnd=GetForegroundWindow();
	RECT r;
	::GetWindowRect(wnd,&r);
	x+=r.left; y+=r.top;	//apply window offset
	r.left=0;	r.top=0;
	AdjustWindowRect(&r,GetWindowLong(wnd,GWL_STYLE),false);
	x-=r.left;	y-=r.top;	// apply window style offset
	SetCursorPos(x,y);
}
