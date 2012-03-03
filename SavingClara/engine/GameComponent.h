/*==============================================
 * GameComponent class
 *
 * Written by Mark Gossage
 *==============================================*/
#pragma once

class CSceneEngine;	// pre decl

/** Base class for Game components.
These component are held by the CSceneEngine
and are available between any of the Scene's.
They can contain any code and can will be called every game loop
(before the calling to CScene::Update() )
They also have Startup(),Shutdown() called at the relevant moments.
There is no draw function, but it could be added if needed.
*/
class CGameComponent
{
public:
	CGameComponent(){}
	virtual ~CGameComponent(){}
	virtual void Startup(){}
	virtual void Update(float dt){}
	virtual void Shutdown(){}

	CSceneEngine* GetEngine(){return mpEngine;}
private:
	friend class CSceneEngine;	// allows CSceneEngine to access private data 
	CSceneEngine* mpEngine;
private:
	CGameComponent(const CGameComponent&);	/// NO COPYING
	void operator=(const CGameComponent&);	/// NO NO COPYING
};

