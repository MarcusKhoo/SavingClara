/*==============================================
 * JoystickComponent class, handles Joysticks via DirectInput
 *
 * Written by <YOUR NAME>
 *==============================================*/
#include "JoystickComponent.h"
#include "SceneEngine.h"
#include "Fail.h"
// include the DirectX Library files
#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )

CJoystickComponent::CJoystickComponent()
:mpDirectInput(NULL)
{
	mpDirectInput=NULL;
}

void CJoystickComponent::Startup()
{
	HRESULT	hr; // Used for checking DX function reports.
	// create the DirectInput object
	//TODO(create the DirectInput object);
	HINSTANCE hwnd;
	hr= DirectInput8Create(GetEngine()->GetInstance(), 
							DIRECTINPUT_VERSION, 
							IID_IDirectInput8, 
							(void**) & mpDirectInput, 
							NULL);

	if( FAILED(hr) )
	{
		FAIL("DirectInput8Create() - FAILED", "CInputManager");
		Shutdown();
		return;
	}
	// lets go looking for joysticks:
	std::vector<GUID> GUIDs;	// the id's of the joysticks

    // Look for all the Joysticks, we can use for this sample program.
	// we will need to pass in CJoystickComponent::EnumJoysticksCallback as the callback function
	//TODO(Look for Joysticks);
	hr=mpDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, 
									CJoystickComponent::EnumJoysticksCallback,&(GUIDs), 
									DIEDFL_ATTACHEDONLY);

	if (FAILED(hr) || GUIDs.empty())	// no joysticks
		return;

	CJoyData joy;
	ZeroMemory(&joy,sizeof(joy));
	// create & setup the joysticks:
	for(unsigned i=0;i<GUIDs.size();i++)
	{
		joy.mpJoystick=CreateJoystick(GUIDs[i],GetEngine()->GetWindow());
		if (joy.mpJoystick)
			mJoyData.push_back(joy);
	}
}

void CJoystickComponent::Shutdown()
{
	// drop all the joysticks
	for(unsigned i=0;i<mJoyData.size();i++)
	{
		if (mJoyData[i].mpJoystick)
		{
			mJoyData[i].mpJoystick->Unacquire();	// stop acquiring
			mJoyData[i].mpJoystick->Release();
		}
	}
	mJoyData.clear();

	if (mpDirectInput)	mpDirectInput->Release();
	mpDirectInput=NULL;
}

void CJoystickComponent::Update(float dt)
{
	for(unsigned i=0;i<mJoyData.size();i++)
	{
		mJoyData[i].mPrevState=mJoyData[i].mCurrState; // copy old:
		bool ok=GetJoystickState(mJoyData[i].mpJoystick,mJoyData[i].mCurrState);
		mJoyData[i].mIsActive=ok;
	}
}

bool CJoystickComponent::IsJoyActive(int joyIdx)
{
	if (joyIdx<0 || joyIdx>=(int)mJoyData.size())	return false;	// out of range
	return mJoyData[joyIdx].mIsActive;
}

LPDIRECTINPUTDEVICE8 CJoystickComponent::CreateJoystick(GUID guid,HWND hwnd)
{
	HRESULT	hr; // Used for checking DX function reports.
	LPDIRECTINPUTDEVICE8 pDev;
	//TODO(Create the joystick device);
	hr=mpDirectInput->CreateDevice(guid, &pDev, NULL);

	if (FAILED(hr))	return NULL;	// error
    // Set the data format to "simple Joystick" - a predefined data format 
    //
    // A data format specifies which controls on a device we are interested in,
    // and how they should be reported. This tells DInput that we will be
    // passing a DIJOYSTATE structure to IDirectInputDevice::GetDeviceState().
	//TODO(Set the data format to "simple Joystick");
	hr=pDev->SetDataFormat(&c_dfDIJoystick);

	if(FAILED(hr))
	{
		pDev->Release();
        return NULL;
	}
	// Set the cooperative level to let DInput know how this device should
    // interact with the system and with other DInput applications.
	// same settings as the mouse
	//TODO(Set the cooperative level);
	 hr=pDev->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
    if(FAILED(hr))
	{
		pDev->Release();
        return NULL;
	}

    // Enumerate the Joystick objects. The callback function enabled user
    // interface elements for objects that are found, and sets the min/max
    // values property for discovered axes.
	//TODO(Enumerate the Joystick object);
	hr=pDev->EnumObjects(CJoystickComponent::EnumObjectsCallback, pDev, DIDFT_ALL);
	// you will need to pass in the CInputManager::EnumObjectsCallback callback fn
	if(FAILED(hr))
	{
		pDev->Release();
        return NULL;
	}
	// all ok
	return pDev;
}
BOOL CJoystickComponent::EnumJoysticksCallback( const DIDEVICEINSTANCE* diDeviceInstance, VOID* usr )
{
	// we know that 'usr' is really a pointer to GUID's
	std::vector<GUID>* pGUIDs=(std::vector<GUID>*)(usr);
	pGUIDs->push_back(diDeviceInstance->guidInstance);	// add the instance
	return DIENUM_CONTINUE;
}

BOOL CJoystickComponent::EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* diDeviceObjInstance, VOID* usr)
{
	// we know that 'usr' is really the joystick's pointer
	LPDIRECTINPUTDEVICE8 pDev=(LPDIRECTINPUTDEVICE8)(usr);
    if( diDeviceObjInstance->dwType & DIDFT_AXIS )
    {
        DIPROPRANGE diPropertyRange; 
        diPropertyRange.diph.dwSize       = sizeof(DIPROPRANGE); 
        diPropertyRange.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        diPropertyRange.diph.dwHow        = DIPH_BYID; 
        diPropertyRange.diph.dwObj        = diDeviceObjInstance->dwType; // Specify the enumerated axis
		//TODO("set max & min ranges for the axis, should be +-1000");
		diPropertyRange.lMin			  = -1000;
		diPropertyRange.lMax			  = +1000;
        // Set the range for the axis
        if( FAILED( pDev->SetProperty( DIPROP_RANGE, &diPropertyRange.diph ) ) ) 
            return DIENUM_STOP;

		// while we are here: set up a small dead zone:
		DIPROPDWORD diPropDead;
		diPropDead.diph.dwSize=sizeof(DIPROPDWORD);
		diPropDead.diph.dwHeaderSize=sizeof(DIPROPHEADER);
		diPropDead.diph.dwHow=DIPH_BYID;
		diPropDead.diph.dwObj=diDeviceObjInstance->dwType; // Specify the enumerated axis
		diPropDead.dwData=(DWORD)(0.10f*10000);	// 10%
        if( FAILED( pDev->SetProperty( DIPROP_DEADZONE, &diPropDead.diph ) ) ) 
            return DIENUM_STOP;

    }
    return DIENUM_CONTINUE;
}

bool CJoystickComponent::GetJoystickState(LPDIRECTINPUTDEVICE8 pJoy, DIJOYSTATE& state)
{
	HRESULT hr;
    // Poll the device to read the current state
	hr=pJoy->Poll();
	if (FAILED(hr))
	{
        // DInput is telling us that the input stream has been
        // interrupted. We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done. We
        // just re-acquire and try again.
		do
		{
			hr=pJoy->Acquire();	// re-acquire
		}while(hr==DIERR_INPUTLOST);	// while its still missing
	}
    // Get the input's device state
	//TODO(Get joystick data);
	hr=pJoy->GetDeviceState(sizeof(DIJOYSTATE), &state);
	// you will need to get the data into outData
    if( FAILED( hr) )
        return false; // The device should have been acquired during the Poll()
	return true;	// all ok
}

CJoyData& CJoystickComponent::GetJoystick(int joyIdx)
{
	if (joyIdx<0 || joyIdx>=(int)mJoyData.size())
		FAIL("Bad Joystick ID","GetJoystick");
	return mJoyData[joyIdx];
}

CJoyData* CJoystickComponent::GetJoystickPtr(int joyIdx)
{
	if (joyIdx<0 || joyIdx>=(int)mJoyData.size())
		FAIL("Bad Joystick ID","GetJoystickPtr");
	return &mJoyData[joyIdx];
}

bool CJoyData::ButtonDown(int buttId)
{
	return (mCurrState.rgbButtons[buttId] & 0x80)!=0;
}
bool CJoyData::ButtonPress(int buttId)
{
	return (mCurrState.rgbButtons[buttId] & 0x80)!=0 &&	// currently down
			(mPrevState.rgbButtons[buttId] & 0x80)==0;	// not prev down
}

D3DXVECTOR3 CJoyData::GetXYZ()
{
	return D3DXVECTOR3(mCurrState.lX/1000.0f,
						mCurrState.lY/1000.0f,
						mCurrState.lZ/1000.0f);
}

D3DXVECTOR3 CJoyData::GetVector(int axisX,int axisY,int axisZ)
{
	return D3DXVECTOR3(GetAxis(axisX),GetAxis(axisY),GetAxis(axisZ));
}

float CJoyData::GetAxis(int axis)
{
	if (axis==1)	return mCurrState.lX/1000.0f;
	if (axis==-1)	return -mCurrState.lX/1000.0f;
	if (axis==2)	return mCurrState.lY/1000.0f;
	if (axis==-2)	return -mCurrState.lY/1000.0f;
	if (axis==3)	return mCurrState.lZ/1000.0f;
	if (axis==-3)	return -mCurrState.lZ/1000.0f;
	if (axis==4)	return mCurrState.lRx/1000.0f;
	if (axis==-4)	return -mCurrState.lRx/1000.0f;
	if (axis==5)	return mCurrState.lRy/1000.0f;
	if (axis==-5)	return -mCurrState.lRy/1000.0f;
	if (axis==6)	return mCurrState.lRz/1000.0f;
	if (axis==-6)	return -mCurrState.lRz/1000.0f;
	return 0;
}