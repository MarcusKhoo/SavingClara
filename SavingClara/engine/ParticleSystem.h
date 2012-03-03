/*==============================================
 * Particle System
 *
 * Author: Frank Luna (C) All Rights Reserved
 * (Subsiquently modified by GDEV team)
 *
 * Desc: A generic Particle system and some specialist systems
 *
 *==============================================*/
#pragma once

#include <d3dx9.h>
#include <deque>

/// This is the rendering type
/// used it the vertex buffered
struct SPointSprite
{
    D3DXVECTOR3 position;
    D3DCOLOR color;
};
const DWORD D3DFVF_POINTSPRITE=(D3DFVF_XYZ|D3DFVF_DIFFUSE);

/// this is the individual particles
struct SAttribute
{
	SAttribute()
	{
		mLifeTime = 0.0f;
		mAge      = 0.0f;
		mIsAlive  = true;
	}

	D3DXVECTOR3 mPosition;     
	D3DXVECTOR3 mVelocity;     
	float       mLifeTime;     // how long the particle lives for before dying  
	float       mAge;          // current age of the particle  
	D3DXCOLOR   mColor;        // current color of the particle   
	D3DXCOLOR   mColorFade;    // how the color fades with respect to time
	bool        mIsAlive;    
};

/** The settings for the particle system.
You will need to fill this class in & use it to describe your particles
*/
struct SParticleSetting
{
	unsigned MaxParticles;	// max number of particles in the system
	D3DXCOLOR StartColor,EndColor;	// start & end colors
	float Size;	// size of the particle
	float LifeTime;	// lifetime of the particle
	DWORD SourceBlend,DestBlend;	// source & destination blend values

	SParticleSetting()	// constructor with default values
	{
		MaxParticles=100;
		StartColor=EndColor=D3DCOLOR_XRGB(255,0,0);
		Size=LifeTime=1;
		SourceBlend=DestBlend=D3DBLEND_ONE;
	}
};


/** This is the particle system class which holds all the rendering code
as well as all the particles.
*/
class CParticleSystem
{
public:
	CParticleSystem();
	virtual ~CParticleSystem();

	virtual bool Init( IDirect3DDevice9* inDevice, const char* inTextureFilename,
					const SParticleSetting& settings);
	virtual void Clear(){mParticles.clear();}

	virtual void AddParticle(D3DXVECTOR3 pos,D3DXVECTOR3 vel=D3DXVECTOR3(0,0,0));

	virtual void AddParticle(D3DXVECTOR3 pos,D3DXVECTOR3 vel,D3DXCOLOR startCol,D3DXCOLOR endCol, float lifeTime);

	virtual void Update( float inDeltaTime );

	virtual void Draw(const D3DXMATRIX& world);		///< call this to draw all particles

	bool IsEmpty();	///< returns true if there are no particles
	bool IsDead();	///< returns true if there are no live particles
	std::deque<SAttribute>* GetParticles(){return &mParticles;}
	SParticleSetting& GetSettings(){return mSettings;}

	/// removes the dead particles, keeping the count down.
	virtual void RemoveDeadParticles();

	// helpers for deling with resetting of the screen
	void OnLostDevice();	// called just before reset device
	void OnResetDevice();	// called just after reset device


	/// \defgroup Rand Random number generators
	/// @{
	/// returns a random float in the range inLowBound..inHighBound
	static float GetRandomFloat( float inLowBound, float inHighBound );
	/// returns a random D3DXVECTOR3 in the range inMin..inMax
	static D3DXVECTOR3 GetRandomVector( const D3DXVECTOR3& inMin, const D3DXVECTOR3& inMax );
	/// returns a random D3DXVECTOR3 which is normalised to length 1
	static D3DXVECTOR3 GetRandomDirection();
	/// adds a variation of the provided angle to a direction
	static D3DXVECTOR3 VaryDirection(const D3DXVECTOR3& vec, float angRad);
	/// returns a random colour
	static D3DCOLOR GetRandomColour();
	/// }@
protected:
	virtual void PreDraw();	///< \internal DO NOT CALL
	virtual void PostDraw();	///< \internal DO NOT CALL

protected:
//public:	// hack for particle editor...
	IDirect3DDevice9*       mpDevice;
	SParticleSetting		mSettings;
	IDirect3DTexture9*      mpTexture;
	IDirect3DVertexBuffer9* mpVertexBuffer;
	std::deque<SAttribute>   mParticles;

	//
	// Following data elements used for rendering the p-system efficiently
	//
	DWORD mpVertexBufferSize;		// size of vertex buffer
	DWORD mpVertexBufferOffset;		// offset in vertex buffer to lock   
};

/** Special version of CParticleSystem for Precipitation (rain,snow).
Works similar to a normal particle system but with the following changes.
+ you must provide a fall direction (normally down) & a variation
+ you don't need to provide the other particle settings if you don't want to
+ you don't need to call AddParticle, as Update will do this for you.
+ you should SetCentre() onto the camera for best effect.

*/
class CPrecipitation : public CParticleSystem
{
	D3DXVECTOR3 mCentre;	// change this value to move the centre of the precipication
	D3DXVECTOR3 mDimension;	// change this to control the size of the precipication box
	D3DXVECTOR3 mEmitDirection;	// direction of emmision
	float mEmitDirVar;	// variation in the emmision direction	
public:
	/** Constructor.
	\param dimensions how large the Precipitation box is.
	Rather than have a vast area, instead the precipication is to be in a small area localised on the camera.
	*/
	CPrecipitation( D3DXVECTOR3 dimensions=D3DXVECTOR3(10,10,10));
	/** Special init function.
	*/
	bool Init(IDirect3DDevice9* inDevice, char* inTextureFilename,D3DXVECTOR3 dir=D3DXVECTOR3(0,-5,0),float variation=D3DX_PI/180*10);
	/// creates new particles & moves them
	void Update( float inDeltaTime );
	/// updates the Precipitation box centre.
	/// call this before Update() for preference
	void SetCentre(D3DXVECTOR3 cent){mCentre=cent;}
	/// sets the emittion direction
	void SetEmitDirection(D3DXVECTOR3 dir,float var){mEmitDirection=dir; mEmitDirVar=var;}
};

class CExplosion : public CParticleSystem
{
public:
	// init with standard settings
	bool Init( IDirect3DDevice9* inDevice, char* inTextureFilename);
	/// makes a multicoloured explosion at a certain location
	void Explode(const D3DXVECTOR3& pos,float speed=10,int number=500);
	/// makes a coloured explosion at a certain location
	void Explode(const D3DXVECTOR3& pos,D3DXCOLOR startCol,D3DXCOLOR endCol=D3DXCOLOR(0,0,0,1),float speed=10,int number=500);
};
