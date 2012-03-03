/*==============================================
 * Scene Manager class for GDEV Engine
 *
 * Written by Mark Gossage
 *==============================================*/
#include "GameUtils.h"
#include "GameEngine.h"
#include "Fail.h"
#include "SpriteUtils.h"
#include "FontUtils.h"
#include "SceneEngine.h"
#include "GameComponent.h"

const char* DEFAULT_FONT="Arial";
const int DEFAULT_FONT_SIZE=20;

CScene::CScene()
{
	mTransOnTime=mTransOffTime=0;
	mState=FADE_IN;
	mTransPos=1;	// just starting
}
void CScene::ExitScene()	// makes the scene exit
{
	if (mState==FADE_OUT || mState==DEAD)	return;	// already going
	mState=FADE_OUT;	// fade out
}

void CScene::Update(float dt)
{
	// fadeing
	if (mState==FADE_IN)
	{
		if (mTransOnTime<=0)
			mTransPos=0;	// finished
		else
			mTransPos-=dt/mTransOnTime;

		if (mTransPos<=0)
		{
			mTransPos=0;
			mState=ACTIVE;
		}
	}
	if (mState==FADE_OUT)
	{
		if (mTransOffTime<=0)
			mTransPos=1;	// finished
		else
			mTransPos+=dt/mTransOffTime;

		if (mTransPos>=1)
		{
			mTransPos=1;
			mState=DEAD;	// remove me
		}
	}
}

void CScene::DrawTransitionFade(D3DXCOLOR col)
{
	if (GetState()==ACTIVE)	return;
	col.a=GetTransPos();
	GetEngine()->DrawColourTint(col);
}
bool CScene::IsTopMost()
{
	if (GetEngine()->GetScenes().empty())	return false;	// not the top
	return (GetEngine()->GetScenes().back() == this);	// are we at the back
}


CSceneEngine::CSceneEngine(int width,int height,bool fullscreen)
{
	mDWidth=width;
	mDHeight=height;
	mDFullScreen=fullscreen;
	mStarted=0;
}

bool CSceneEngine::Startup()
{
	if (StartupWinD3D(mDWidth,mDHeight,mDFullScreen)==false)
		return false;
	mpSprite=CreateSpriteBatch(GetDevice());
	mpFont=CreateD3DFont(GetDevice(),DEFAULT_FONT,DEFAULT_FONT_SIZE);
	// create a blank 4x4 texture:
	D3DXCreateTexture(GetDevice(),4,4,0,0,D3DFMT_X8R8G8B8,D3DPOOL_MANAGED,&mpBlankTex);
	D3DLOCKED_RECT theLockedRect;
	mpBlankTex->LockRect(0, &theLockedRect,0,0);      
	memset(theLockedRect.pBits,0xFF,theLockedRect.Pitch*4);	// clear to white
	mpBlankTex->UnlockRect( NULL );

	// start all components
	for(unsigned i=0;i<mComponents.size();i++)
		mComponents[i]->Startup();
	mStarted=1;
	// start all scenes
	for(unsigned i=0;i<mScenes.size();i++)
		mScenes[i]->Enter();
	mStarted=2;
	return true;
}

void CSceneEngine::Update(float dt)
{
	// update components
	for(unsigned i=0;i<mComponents.size();i++)
		mComponents[i]->Update(dt);
	// update everyone
	for(unsigned i=0;i<mScenes.size();i++)
		mScenes[i]->Update(dt);
	// remove any dead screens:
	for(int i=mScenes.size()-1;i>=0;i--)
	{
		if (mScenes[i]->GetState()==CScene::DEAD)
			RemoveScene(mScenes[i]);
	}
	if (mScenes.empty())	CloseWin();
}

void CSceneEngine::Draw(float dt)
{
	GetDevice()->Clear(0,0,D3DCLEAR_TARGET,MAGENTA_COL,0,0);
	// start at the back & find the last NON popup
	int startId=0;
	for(unsigned i=0;i<mScenes.size();i++)
	{
		if (mScenes[i]->IsPopup()==false)
			startId=i;	// note: the index
	}
	// draw from this onwards:
	for(unsigned i=startId;i<mScenes.size();i++)
	{
		mScenes[i]->Draw(dt);
	}
	GetDevice()->Present(0,0,0,0);
}
void CSceneEngine::Shutdown()
{
	while(mScenes.empty()==false)	// remove scenes
	{
		RemoveScene(mScenes.back());
	}
	while(mComponents.empty()==false)	// remove component
	{
		CGameComponent* pComp=mComponents.back();
		mComponents.pop_back();
		pComp->Shutdown();
		delete pComp;
	}
	// shutdown
	SAFE_RELEASE(mpBlankTex);
	SAFE_RELEASE(mpSprite);
	SAFE_RELEASE(mpFont);
	ShutdownD3D();
}

void CSceneEngine::DrawColourTint(D3DCOLOR col)
{
	mpSprite->Begin(D3DXSPRITE_ALPHABLEND);
	DrawSprite(mpSprite,mpBlankTex,GetWindowRect(),col);
	mpSprite->End();
}
void CSceneEngine::AddScene(CScene* pScene)
{
	pScene->mpEngine=this;
	mScenes.push_back(pScene);
	if (mStarted>=2)
		pScene->Enter();
}
bool CSceneEngine::RemoveScene(CScene* pScene)
{
	// find:
	for(unsigned i=0;i<mScenes.size();i++)
	{
		if (mScenes[i]==pScene)
		{
			pScene->Leave();
			delete pScene;
			mScenes.erase(mScenes.begin()+i);
			return true;
		}
	}
	return false;
}

void CSceneEngine::AddComponent(CGameComponent* pComp)
{
	pComp->mpEngine=this;
	mComponents.push_back(pComp);
	if (mStarted>=1)
		pComp->Startup();
}
