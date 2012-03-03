/*==============================================
 * JoystickComponent class, handles Joysticks via DirectInput
 *
 * Written by <YOUR NAME>
 *==============================================*/
#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <d3dx9.h>
#include <vector>
#include "GameComponent.h"

class CJoystickComponent;
/** The class which holds the actual joystick state.
Also includes functions to access it.
*/
class CJoyData
{
public:
	// Constants used for POV controls (D-keys) on the joystick
	enum{	POV_LEFT	= 27000,
			POV_RIGHT	= 9000,
			POV_UP		= 0,
			POV_DOWN	= 18000};

	bool IsActive(){return mIsActive;}	///< returns if the joystick is active
	const DIJOYSTATE& GetState(){return mCurrState;}	///< returns the joystick state
	const DIJOYSTATE& GetPrevState(){return mPrevState;}	///< returns the previous state
	/// returns true if joystick button is down now (max 32 buttons)
	bool ButtonDown(int buttId);
	/// returns true if mouse button is down now, but wasn't last cycle (max 32 buttons)
	bool ButtonPress(int buttId);

	/// gets the joysticks xyz(normalised -1.0f..+1.0f)
	D3DXVECTOR3 GetXYZ();

	/// Calls GetAxis() to make a Vector3
	D3DXVECTOR3 GetVector(int axisX,int axisY,int axisZ);
	/// helper which returns the axis value (0 if not valid/present)
	/// \param axis 1:x,2:y,3:z, etc. values like 0 will give a zero return
	/// a negative value will give the inverted value, eg -1 is inverted X axis
	float GetAxis(int axis);
private:
	friend CJoystickComponent;	// CJoystickComponent is allowed access to private data
	LPDIRECTINPUTDEVICE8 mpJoystick;	///< pointer to the joystick
	bool mIsActive;	///< whether its active
	DIJOYSTATE mCurrState;	///< current data of the joystick
	DIJOYSTATE mPrevState;	///< previous data from joystick
};

/**

\par References:
- Direct Input
 - Wendy Jones: is a very good book on this area
 - http://en.wikipedia.org/wiki/DirectInput (gives an overview)
 - http://msdn.microsoft.com/en-us/library/ee418273%28VS.85%29.aspx
 - http://wiki.directxers.com/index.php?title=DirectX_Input
- Microsoft itself does not recomend DirectInput for Keyboard/Mouse
 - http://msdn.microsoft.com/en-us/library/ee416842%28VS.85%29.aspx
 - http://msdn.microsoft.com/en-us/library/ee418864%28VS.85%29.aspx (WM_INPUT: not used)
 - http://msdn.microsoft.com/en-us/library/ms645536%28VS.85%29.aspx (Raw input: num used)
- Action Mapping (considered, but not used)
 - http://www.sunlightd.com/Archive/Windows/DirectX/Input.aspx
 - http://doc.51windows.net/Directx9_SDK/?url=/Directx9_SDK/dir_3.htm

*/
class CJoystickComponent: public CGameComponent
{
public:
	/// returns the number of joysticks registered in the manager.
	/// Joystick may not be active.
	int GetJoyCount(){return mJoyData.size();}
	/// returns is a given joystick is active.
	/// if its a invalid joystick it will return false
	bool IsJoyActive(int joyIdx=0);
	/// gets the joystick data
	CJoyData& GetJoystick(int joyIdx=0);
	/// pointer version of GetJoystick()
	CJoyData* GetJoystickPtr(int joyIdx=0);

	CJoystickComponent();	///< constructor
	void Startup();			///< set up everything
	void Update(float dt);	///< must be called once/cycle
	void Shutdown();		///< closes DirectInput and releases memory	
protected:	// internal functions
	/// creates up a single joystick
	LPDIRECTINPUTDEVICE8 CreateJoystick(GUID guid,HWND hwnd);
	/// gets the joysticks state
	bool GetJoystickState(LPDIRECTINPUTDEVICE8 pJoy, DIJOYSTATE& state);
	/// callback called when directinput is looking for joysticks
	static BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* diDeviceInstance, VOID* usr );
	/// callback called when directinput is enumerating the joysticks
	static BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* diDeviceObjInstance, VOID* usr );
protected: // data
	LPDIRECTINPUT8 mpDirectInput;	///< the pointer to our DirectInput interface
	std::vector<CJoyData> mJoyData;	///< all the joysticks data
};
