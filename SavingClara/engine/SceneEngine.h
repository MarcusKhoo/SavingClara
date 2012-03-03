/*==============================================
 * Scene Manager class for GDEV Engine
 *
 * Written by Mark Gossage
 *==============================================*/
#pragma once
#include "GameEngine.h"
#include <d3dx9tex.h>
#include <vector>

class CScene;
class CGameComponent;

class CSceneEngine: public CGameEngine
{
public:
	CSceneEngine(int width=640,int height=480,bool fullscreen=true);

	virtual bool Startup();
	virtual void Update(float dt);
	virtual void Draw(float dt);
	virtual void Shutdown();

	/** Adds a new Scene as the topmost scene.
	\param pScene the new scene (MUST be DMA) as it will be deleted once the scene is finished
	*/
	void AddScene(CScene* pScene);
	/// Removes a scene \internal
	bool RemoveScene(CScene* pScene);
	ID3DXSprite* GetSprite(){return mpSprite;}	///< shared spritebatch
	ID3DXFont* GetDefaultFont(){return mpFont;}	///< default font

	/// covers the whole screen with a colour tint (usually alpha).
	/// must be called within BeginScene()..EndScene()
	void DrawColourTint(D3DCOLOR col);

	/// gets a list of scenes (readonly) bottom..top
	const std::vector<CScene*>& GetScenes(){return mScenes;}

	/// Adds a component to the engine:
	void AddComponent(CGameComponent* pComp);
	/** Looks for a given component in the engine.
	\code
	// to find an XYZ component
	XYZ* ptr=GetEngine()->FindComponent<XYZ>();
	if (ptr!=NULL)
		// found it
	else
		// it seems to be missing?
	\endcode
	*/
	template<typename T>
	T* FindComponent()
	{
		T* ptr=NULL;
		for(unsigned i=0;i<mComponents.size();i++)
		{
			ptr=dynamic_cast<T*>(mComponents[i]);
			if (ptr!=NULL)	break;
		}
		return ptr;
	}
private:
	std::vector<CGameComponent*> mComponents;	///< list of components
	std::vector<CScene*> mScenes;	///< list of scenes (bottom..top)
	ID3DXSprite* mpSprite;	///< spritebatch
	ID3DXFont* mpFont;	///< default font
	IDirect3DTexture9* mpBlankTex;	///<  an empty texture for fading purposes
	int mDWidth,mDHeight;	///< desired width & height
	bool mDFullScreen;	///< desired full screen
	int mStarted;		///< if startup has been called
};

/**
*/
class CScene
{
public:
	friend class CSceneEngine;	// allows CSceneEngine to access private data 
	typedef enum{FADE_IN,ACTIVE,FADE_OUT,DEAD} State;	// the scene State 'type'
	CScene();
	virtual void Enter(){}
	virtual void Update(float dt);	///< must be called to make transitions work
	virtual void Draw(float dt){}
	virtual void Leave(){}
	virtual ~CScene(){}
public:
	void ExitScene();	// makes the scene exit
	CSceneEngine* GetEngine(){return mpEngine;}	///< get the engine
	IDirect3DDevice9* GetDevice(){return mpEngine->GetDevice();}	///< get the device
	ID3DXSprite* GetSprite(){return mpEngine->GetSprite();}	///< shared spritebatch
	ID3DXFont* GetFont(){return mpEngine->GetDefaultFont();}	///< default font
	bool IsPopup(){return mPopup;}
	State GetState(){return mState;}
	bool IsMain(){return GetState()==ACTIVE && IsTopMost();}	///< returns if this scene is the active & topmost 
	bool IsTopMost();	///< returns true if this scene is the topmost scene, handy for deciding if to act
	float GetTransPos(){return mTransPos;}///< returns transition %age, 0 is active, 1 is transitioned on/off
	/** displays a colour fade over the screen.
	\param col the desired colour (default black)
	if this function is called every draw cycle (between the BeginScene() & EndScene()),
	it will automatically produce a nice fade effect as the scene transitions.
	Does nothing if Scene if GetState==ACTIVE.
	*/
	void DrawTransitionFade(D3DXCOLOR col=D3DXCOLOR(D3DCOLOR_XRGB(0,0,0)));
protected:
	float mTransOnTime,mTransOffTime;	///< on/off transtion times
	bool mPopup;	///< if popup (non popup covers all other scenes)
private:
	float mTransPos;
	State mState;
	CSceneEngine* mpEngine;
private:
	CScene(const CScene&);	/// NO COPYING
	void operator=(const CScene&);	/// NO NO COPYING
};
