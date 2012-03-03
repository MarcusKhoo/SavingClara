/*==============================================
 * MessageBox Scene for SceneManager
 *
 * Original code from 
 *	http://creators.xna.com/en-US/samples/gamestatemanagement
 * Ported from XNA to GDev engine by Mark Gossage
 * Hacked by Ong Jin Wen to accept controller input
 *==============================================*/
#pragma once

#include <string>
#include "SceneEngine.h"
#include "SpriteUtils.h"
#include "FontUtils.h"
#include "JoystickComponent.h"

/** Message box scene.
Basically its a Scene which pops a message onto the Scene.

Usage:
To use it is easy, but to get the input is harder (you need a callback).
If you just want a popup & go away its simple:

\code
mpSceneMgr=new CSceneManager(this);
mpSceneMgr->Add(new CMessageBoxScene("Hello world!"));
\endcode

*/
class CMessageBoxScene:public CScene
{
public:
	CMessageBoxScene(const char* message,bool addUsage=true,const char* bgName="media/Textures/gradient.png");
	/** Closes the speified window on ok.
	Full usage example (getting a window to close itself):
	\code
	// inside the game Scene's update
	if (CGameWindow::KeyPress(VK_ESPACE))
	{
  	  CMessageBoxScene* pMsg=new CMessageBoxScene("Exit Game?");
	  pMsg->OnOkCloseScene(this); // if uses hits ok, close this
	  GetManager()->Add(pMsg); // give to the manager
	}
	\endcode
	\param pTgt this game Scene to be closed.
	*/
	void OnOkCloseScene(CScene* pTgt);

	/** Closes the whole application if the user presses ok.
	Full usage example (getting the game to close itself):
	\code
	// inside the game Scene's update
	if (CGameWindow::KeyPress(VK_ESPACE))
	{
  	  CMessageBoxScene* pMsg=new CMessageBoxScene("Exit Game?");
	  pMsg->OnOkExitGame(); // if uses hits ok, shutdown all
	  GetManager()->Add(pMsg); // give to the manager
	}
	\endcode
	*/	
	void Enter();
	void Update(float dt);
	void Draw(float dt);
	void Leave();

public:	// callbacks are advanced feature: use with care
	typedef void(*Callback)(int result,void* usr);	// Callback is a fn pointer
	/** The callback system allows the messagebox to call a function (not a member fn).
	<B>ADVANCED FEATURE</B>
	To inform the function whether the user pressed ok or cancel.
	When the ok/cancel is pressed, the callback will be called & the window will close itself.

	For those who don't know thats a normal/static function of the format:
	\code
	void Callback(int result,void* usr); // result=true or false
	\endcode

	Full usage example:
	\code
	mpSceneMgr=new CSceneManager(this);

	CMessageBoxScene* pMsg=new CMessageBoxScene("Hello World!");
	pMsg->SetCallback(GameStateManagementGame::OnCancel,this);
	mpSceneMgr->Add(pMsg);
	...

	void GameStateManagementGame::OnCancel(int result,void* usr)
	{
		if (result==false)	// cancel
		{
			GameStateManagementGame* self=(GameStateManagementGame*)(usr);
			self->CloseWin();	// ending
		}
	}
	\endcode
	*/

	void OnOkExitGame();

	void SetCallback(Callback fn, void* usr);

	/// helper Callback: used to close a Scene on the ok
	/// usr must be a pointer to a Scene
	static void ExitSceneOnOkCb(int result,void* usr);

	/// helper Callback: used to close the CGameWindow on the ok
	/// usr must be a pointer to a Scene (NOT CGameWindow)
	static void CloseWinOnOkCb(int result,void* usr);
private:
	CJoystickComponent* mpJoy;
	Callback mCallback;	// user settable callback
	void* mCallbackUsr;	// user data
	std::string mText;
	std::string mBgName;
	IDirect3DTexture9* mpTexture;
};
