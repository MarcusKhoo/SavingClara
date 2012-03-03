/*==============================================
 * MessageBox Scene for SceneManager
 *
 * Original code from 
 *	http://creators.xna.com/en-US/samples/gamestatemanagement
 * Ported from XNA to GDev engine by Mark Gossage
 * Hacked by Ong Jin Wen to accept controller input
 *==============================================*/
#include "MessageBoxScene.h"

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

CMessageBoxScene::CMessageBoxScene(const char* message,bool addUsage,const char* bgName)
{
	mText=message;
	if (addUsage)
		mText+="\nEnter, A = Yes"
				"\nEsc, B = No";
	mBgName=bgName;
	mCallback=NULL;
	mCallbackUsr=NULL;
}
void CMessageBoxScene::Enter()
{
	mpJoy=GetEngine()->FindComponent<CJoystickComponent>();
	mPopup=true;
	mTransOnTime=mTransOffTime=0.2f;
	mpTexture=LoadSpriteTex(GetDevice(),mBgName.c_str());
}
void CMessageBoxScene::Update(float dt)
{
	CScene::Update(dt);	// NEED THIS FIRST
	if (mpJoy->IsJoyActive(0))
	{
		DIJOYSTATE state = mpJoy->GetJoystick(0).GetState();
		DIJOYSTATE prevState = mpJoy->GetJoystick(0).GetPrevState();

		if (state.rgbButtons[0] != 0 && prevState.rgbButtons[0] == 0)
		{
			if (mCallback)
				mCallback(true,mCallbackUsr);
			ExitScene();
		}
		else if (state.rgbButtons[1] != 0 && prevState.rgbButtons[1] == 0)
		{
			if (mCallback)
				mCallback(false,mCallbackUsr);
			ExitScene();
		}
	}
	else
	{
		if (CGameWindow::KeyPress(VK_RETURN))
		{
			if (mCallback)
				mCallback(true,mCallbackUsr);
			ExitScene();
		}
		if (CGameWindow::KeyPress(VK_ESCAPE))
		{
			if (mCallback)
				mCallback(false,mCallbackUsr);
			ExitScene();
		}
	}
}
void CMessageBoxScene::Draw(float dt)
{
	LPD3DXSPRITE pSprite=GetSprite();
	LPD3DXFONT pFont=GetFont();

	// Darken down any other Scenes that were drawn beneath the popup.
	float alpha=1-GetTransPos();	
	GetEngine()->DrawColourTint(D3DXCOLOR(0,0,0,alpha*2/3));

	// compute the sizes
	RECT scr=GetEngine()->GetWindowRect();
	D3DXVECTOR2 scrSize((float)scr.right,(float)scr.bottom);
	D3DXVECTOR2 textSize=GetTextSize(pFont,mText.c_str());
	D3DXVECTOR2 textPos=(scrSize-textSize)/2;

	const int VPAD=16,HPAD=32;	// padding
	RECT bg;
	bg.left=(int)textPos.x - HPAD;
	bg.top=(int)textPos.y - VPAD;
	bg.right=bg.left+(int)textSize.x + HPAD * 2;
	bg.bottom=bg.top+(int)textSize.y + VPAD * 2;

	D3DCOLOR col=D3DCOLOR_ARGB((int)(255*alpha),255,255,255);
	pSprite->Begin(D3DXSPRITE_ALPHABLEND);	// must have ALPHABLEND or font looks awful
	// stretch the 8x8 background into its area
	DrawSprite(pSprite,mpTexture,bg,col);
	// add text (using the sprite batch)
	DrawD3DFontEx(pFont,pSprite,mText.c_str(),(int)textPos.x,(int)textPos.y,
					col);
	pSprite->End();
}
void CMessageBoxScene::Leave()
{
	SAFE_RELEASE(mpTexture);
}

void CMessageBoxScene::OnOkCloseScene(CScene* pTgt)
{
	SetCallback(ExitSceneOnOkCb,pTgt);
}

void CMessageBoxScene::OnOkExitGame()
{
	SetCallback(CloseWinOnOkCb,this);
}

void CMessageBoxScene::SetCallback(Callback fn, void* usr)
{
	mCallback=fn;
	mCallbackUsr=usr;
}

void CMessageBoxScene::ExitSceneOnOkCb(int result,void* usr)
{
	if (result)	// ok
	{
		CScene* self=(CScene*)(usr);
		self->ExitScene();	// ending
	}
}

void CMessageBoxScene::CloseWinOnOkCb(int result,void* usr)
{
	if (result)	//ok
	{
		CScene* self=(CScene*)(usr);
		self->GetEngine()->CloseWin();	// ending everything
	}
}
