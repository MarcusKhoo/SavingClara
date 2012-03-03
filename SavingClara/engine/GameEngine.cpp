/*==============================================
 * GameEngine class for GDEV Engine
 *
 * Written by <YOUR NAME HERE>
 *==============================================*/
#define _CRT_SECURE_NO_DEPRECATE // remove a VS2005 warning
#include "GameEngine.h"
#include "Fail.h"
#include <stdio.h>

// handy trick to auto link the libraries
#pragma comment(lib, "d3d9.lib") // instruction to the linker
#pragma comment(lib, "d3dx9.lib") // instruction to the linker

CGameEngine::CGameEngine()
:CGameWindow()
{
	mpD3D=NULL;
	mpDevice=NULL;
	mFPS=0;
}
CGameEngine::~CGameEngine()
{
}

int CGameEngine::Go(HINSTANCE hInstance)
{
	mInstance=hInstance;
	if (!Startup())
	{
		FAIL("Failure in Startup()", "CGameEngine::Go() ERROR");
		Shutdown();
		return 3;
	}
	if (mpD3D==NULL || mpDevice==NULL)
	{
		FAIL("D3D not created in Startup", "CGameEngine::Go() ERROR");
		Shutdown();
		return 3;
	}
	// main windows loop
	int frameCount=0;
	float frameElapsed=0;
    MSG msg; 
    ZeroMemory( &msg, sizeof(msg) );
	float lastTime=GetTime();
    while( msg.message != WM_QUIT )
    {
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
			TranslateMessage( &msg );
            DispatchMessage( &msg );
        }		
		else
		{
			float currTime=GetTime();
			float dt=currTime-lastTime;	// the diff
			Update(dt);
			// if the device is lost, do not render
			if (CheckLostDevice()==false)
				Draw(dt);
			UpdateInput();
			lastTime=currTime;	// update
			// FPS counter
			frameCount++;
			frameElapsed+=dt;
			if (frameElapsed>=1.0)
			{
				mFPS=(float)frameCount/frameElapsed;
				frameCount=0;
				frameElapsed-=1.0;
			}
            Sleep(0);   // Play nice with multitasking
		} 
    }
	Shutdown();
	return (int) msg.wParam;
}

bool CGameEngine::Startup()
{
	return StartupWinD3D(640,480,false);	// 640x480 windowed
}

void CGameEngine::Update(float dt)
{
	// later...
}

void CGameEngine::Draw(float dt)
{
	// TODO: refer to Wendy Jones chapter 2 or 4

    // Clear the backbuffer to some color
	GetDevice()->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 255,0,0 ), 1.0f, 0 );
	GetDevice()->BeginScene();
	GetDevice()->EndScene();
	// Present the backbuffer contents to the display
	GetDevice()->Present( NULL, NULL, NULL, NULL );
}

void CGameEngine::Shutdown()
{
	ShutdownD3D();
}

bool CGameEngine::StartupD3D(int width,int height,bool fullscreen)
{
	// make sure they are null, just in case...
	mpD3D = NULL;
	mpDevice = NULL;
	// TODO: refer to Wendy Jones chapter 4 and the tutorial

	// create the directX object
    mpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	/*if(mpD3D == NULL)
		return false;*/


	// fill the presentation parameters structure
    ZeroMemory( &mD3Dpp, sizeof(mD3Dpp) );
	mD3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	mD3Dpp.AutoDepthStencilFormat = D3DFMT_D16;
	mD3Dpp.EnableAutoDepthStencil = TRUE;
	mD3Dpp.BackBufferCount  = 1;
	mD3Dpp.BackBufferHeight = height;
	mD3Dpp.BackBufferWidth  = width;
	mD3Dpp.hDeviceWindow = mHWnd;
    //mD3Dpp.SwapEffect = 
	//if( fullscreen ){...}
	if (fullscreen)
	{
		mD3Dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
		mD3Dpp.Windowed = FALSE;
	}
	else
	{
		mD3Dpp.Windowed = TRUE;
	    mD3Dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	}
    //else{...}

	HRESULT hr = mpD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mHWnd,
                             D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                             &mD3Dpp, &mpDevice);
    
	// create a default directx device
	if (FAILED(hr)) 
		return false; //if it failed: stop
	return true;
}

bool CGameEngine::StartupWinD3D(int width,int height,bool fullscreen)
{
	//TODO(CGameEngine::StartupWinD3D);
	if (OpenWin(GetInstance(), width, height, fullscreen) == false) 
		return false;
	if (StartupD3D(width,height,fullscreen) == false)
		return false;
	// TODO: you need to call two functions: 
	// one to open the window & one to start D3D
	return true;
}

void CGameEngine::ShutdownD3D()
{
	// TODO: release all you have used
	if (mpDevice!=NULL)
	{
		mpDevice->Release();
		mpDevice=NULL;
	}
	if (mpD3D!=NULL)
	{
		mpD3D->Release();
		mpD3D=NULL;
	}
}

bool CGameEngine::CheckLostDevice()
{
	// check for switch error
	HRESULT hr=GetDevice()->TestCooperativeLevel();

	if(hr == D3DERR_DEVICELOST) 
	{ 
		//Device is lost and cannot be reset yet
		Sleep(100); // Wait a bit so we don't burn through cycles for no reason
		return true;	// device lost
	}
	else if(hr == D3DERR_DEVICENOTRESET)
	{
		//Lost but we can reset it now
		OnLostDevice();
		return false;	// device is not lost now
	}
	return false;	// no problems
}

void CGameEngine::OnLostDevice()
{
	FAIL(	"This application doesn't support Alt-Tab yet\n"
			"Program will now exit\n",
			"FAIL: Device Lost");
}

void CGameEngine::ShowFrameRate(const char* title)
{
	char buff[256];
	sprintf(buff,"%s @ %.2f fps",title,GetFps());
	SetTitle(buff);
}

void CGameEngine::ResetDevice()	///< resets the device or fails
{
	HRESULT hr=mpDevice->Reset(&mD3Dpp);
	if (hr==D3DERR_INVALIDCALL)
		FAIL("DirectX Components attached to device were not reset properly","ResetDevice FAILED");
}

// these must be the same as the ones in GameWindow or your window will look odd
const DWORD STYLE_GAMEWINDOW=(WS_CAPTION|WS_OVERLAPPED |WS_SYSMENU);
const DWORD STYLE_GAMEFULL=(WS_POPUP);

void CGameEngine::SetFullScreen(bool fullscreen)
{
	mD3Dpp.Windowed=!fullscreen;
	OnLostDevice();
	// get new style for window
	DWORD dwstyle = mD3Dpp.Windowed ? STYLE_GAMEWINDOW:STYLE_GAMEFULL;
	// get new size
	RECT r=GetWindowRect();
	int w=r.right,h=r.bottom;
	AdjustWindowSize(w,h,dwstyle);

	SetWindowLong(GetWindow(), GWL_STYLE, dwstyle);	// change style
	SetWindowPos(GetWindow(), 0, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE );	// change size
	// force to top
	SetWindowPos(GetWindow(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
}

void CGameEngine::ToggleFullScreen()
{
	SetFullScreen(!IsFullScreen());
}
