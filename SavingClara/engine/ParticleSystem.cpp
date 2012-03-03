/*==============================================
 * Particle System
 *
 * Author: Frank Luna (C) All Rights Reserved
 * (Subsiquently modified by GDEV team)
 *
 * Desc: A generic Particle system and some specialist systems
 *
 *==============================================*/
#include "ParticleSystem.h"
#include "Fail.h"

// inline function that converts a float to a DWORD value
inline DWORD FLOAT_TO_DWORD( FLOAT f ) { return *((DWORD*)&f); }

CParticleSystem::CParticleSystem() :
	mpDevice( NULL ),
	mpVertexBuffer( NULL ),
	mpTexture( NULL )
{
}

CParticleSystem::~CParticleSystem()
{
	if( mpVertexBuffer )
	{
		mpVertexBuffer->Release( );
		mpVertexBuffer = NULL;
	}

	if( mpTexture )
	{
		mpTexture->Release( );
		mpTexture = NULL;
	}
}

bool CParticleSystem::Init( IDirect3DDevice9* inDevice, const char* inTextureFilename,
							const SParticleSetting& settings)
{
	mSettings=settings;
	mpVertexBufferSize=mSettings.MaxParticles;

	mpDevice = inDevice; // save a ptr to the device

	HRESULT hr = 0;

	//TODO("Create the vertex buffer for storing particles");
	hr = mpDevice->CreateVertexBuffer(mpVertexBufferSize * sizeof(SPointSprite),
							D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS,
							D3DFVF_POINTSPRITE, D3DPOOL_DEFAULT, 
							&mpVertexBuffer, NULL);//, you will need to fill mpVertexBuffer
	
	if( FAILED( hr ) )
	{
		FAIL( "CreateVertexBuffer() - FAILED", "CParticleSystem");
		return false;
	}

	//TODO("load the texture");
	// texture
	hr=D3DXCreateTextureFromFile(mpDevice, inTextureFilename, &mpTexture);

	if( FAILED( hr ) )
	{
		FAIL( inTextureFilename,"D3DXCreateTextureFromFile() - FAILED");
		return false;
	}

	// resize ready for the capacity
	mParticles.resize(mSettings.MaxParticles);
	mParticles.clear();

	return true;
}

void CParticleSystem::AddParticle(D3DXVECTOR3 pos,D3DXVECTOR3 vel)
{
	AddParticle(pos,vel,mSettings.StartColor,mSettings.EndColor,mSettings.LifeTime);
}

void CParticleSystem::AddParticle(D3DXVECTOR3 pos,D3DXVECTOR3 vel,D3DXCOLOR startCol,D3DXCOLOR endCol,float lifeTime)
{
	if (mParticles.size()>=mSettings.MaxParticles)	// it full, remove oldest
		mParticles.pop_front();

	SAttribute attrib;
//	attrib.mAcceleration=D3DXVECTOR3(0,0,0);
	attrib.mAge=0;
	attrib.mIsAlive=true;
	attrib.mLifeTime=lifeTime;
	attrib.mPosition=pos;
	attrib.mVelocity=vel;
	attrib.mColor=startCol;
	attrib.mColorFade=(endCol-startCol)/mSettings.LifeTime;

	mParticles.push_back( attrib );
}

void CParticleSystem::Update( float inDeltaTime )
{
	std::deque<SAttribute>::iterator theIterator = mParticles.begin( );
	std::deque<SAttribute>::iterator theEnd = mParticles.end( );
	for( ; theIterator != theEnd; ++theIterator )
	{
		// only do those alive
		if (theIterator->mIsAlive)
		{
			// default behaviour: move, change colour, age, die if needed
//			theIterator->mVelocity += theIterator->mAcceleration * inDeltaTime;
			theIterator->mPosition += theIterator->mVelocity * inDeltaTime;
			theIterator->mColor+= theIterator->mColorFade * inDeltaTime;
			theIterator->mAge+=inDeltaTime;
			if (theIterator->mAge > theIterator->mLifeTime)
				theIterator->mIsAlive=false;
		}
	}
	RemoveDeadParticles();	// get rid of the corpses
}

void CParticleSystem::PreDraw()
{
	mpDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	mpDevice->SetRenderState(D3DRS_LIGHTING, false);

	//TODO(enable blending and set the blend type);
	mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	mpDevice-> SetRenderState(D3DRS_SRCBLEND, mSettings.SourceBlend);
	mpDevice-> SetRenderState(D3DRS_DESTBLEND, mSettings.DestBlend);
	
	//TODO(turn on pointsprites);
	mpDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
    //TODO(enable scaling);
	mpDevice->SetRenderState(D3DRS_POINTSCALEENABLE, true);
    //TODO(set size of the sprite);
	mpDevice->SetRenderState(D3DRS_POINTSIZE, FLOAT_TO_DWORD(mSettings.Size));
	// the point size to use when the vertex does not include this information
	//TODO(set the minimum size of the point)
    mpDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FLOAT_TO_DWORD(0.0f));
    // these three renderstates control the scaling of the pointsprite
	//TODO(set the pointscale_ attributes);
	mpDevice->SetRenderState(D3DRS_POINTSCALE_A, FLOAT_TO_DWORD(0.0f));
	mpDevice->SetRenderState(D3DRS_POINTSCALE_B, FLOAT_TO_DWORD(0.0f));
	mpDevice->SetRenderState(D3DRS_POINTSCALE_C, FLOAT_TO_DWORD(1.0f));
}

/**
*	Reset any states we may have altered for point sprites to be rendered.
*/
void CParticleSystem::PostDraw()
{
    //TODO(CParticleSystem::PostDraw());
	mpDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	mpDevice->SetRenderState(D3DRS_LIGHTING, true);
	mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	mpDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
}

/**
*	The Draw method works by filling a section of the vertex buffer with data,
*	then we Render that section.  While that section is rendering we lock a new
*	section and begin to fill that section.  Once that sections filled we Render it.
*	This process continues until all the particles have been drawn.  The benefit
*	of this method is that we keep the video card and the CPU busy.  
*/
void CParticleSystem::Draw(const D3DXMATRIX& world)
{
	if( !mParticles.empty() )
	{
		mpDevice->SetTransform(D3DTS_WORLD,&world);
		// set Render states
		PreDraw();
		
		mpDevice->SetTexture( 0, mpTexture );
		mpDevice->SetFVF( D3DFVF_POINTSPRITE );
		mpDevice->SetStreamSource( 0, mpVertexBuffer, 0, sizeof( SPointSprite ) );

		// Render batches one by one

		// start at beginning if we're at the end of the vb
		if( mpVertexBufferOffset >= mpVertexBufferSize )
			mpVertexBufferOffset = 0;

		SPointSprite* theParticleVertex = NULL;

		mpVertexBuffer->Lock(	mpVertexBufferOffset * sizeof( SPointSprite ),
								mpVertexBufferSize * sizeof( SPointSprite ),
								(void**)&theParticleVertex,
								mpVertexBufferOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD theNumParticlesInBatch = 0;

		//
		// Until all particles have been rendered.
		//
		std::deque<SAttribute>::iterator theIterator = mParticles.begin();
		for( ; theIterator != mParticles.end(); ++theIterator )
		{
			if( theIterator->mIsAlive )
			{
				// Copy a batch of the living particles to the
				// next vertex buffer segment
				theParticleVertex->position = theIterator->mPosition;
				theParticleVertex->color    = (D3DCOLOR) theIterator->mColor;
				++theParticleVertex; // next element;

				++theNumParticlesInBatch; //increase batch counter

				// if this batch full?
				if( theNumParticlesInBatch == mpVertexBufferSize ) 
				{
					// Draw the last batch of particles that was
					// copied to the vertex buffer. 
					mpVertexBuffer->Unlock();

					mpDevice->DrawPrimitive(	D3DPT_POINTLIST,
												mpVertexBufferOffset,
												mpVertexBufferSize);

					// While that batch is drawing, start filling the
					// next batch with particles.

					// move the offset to the start of the next batch
					mpVertexBufferOffset += mpVertexBufferSize; 

					// don't offset into memory thats outside the vb's range.
					// If we're at the end, start at the beginning.
					if( mpVertexBufferOffset >= mpVertexBufferSize ) 
						mpVertexBufferOffset = 0;       

					mpVertexBuffer->Lock(	mpVertexBufferOffset * sizeof( SPointSprite ),
											mpVertexBufferSize * sizeof( SPointSprite ),
											(void**)&theParticleVertex,
											mpVertexBufferOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD );

					theNumParticlesInBatch = 0; // Reset for new batch
				}	
			}
		}

		mpVertexBuffer->Unlock();

		// its possible that the LAST batch being filled never 
		// got rendered because the condition 
		// (theNumParticlesInBatch == mpVertexBufferSize) would not have
		// been satisfied.  We draw the last partially filled batch now.
		
		if( theNumParticlesInBatch )
		{
			mpDevice->DrawPrimitive( D3DPT_POINTLIST,
										mpVertexBufferOffset,
										theNumParticlesInBatch );
		}

		// next block
		mpVertexBufferOffset += mpVertexBufferSize; 

		// Reset Render states
		PostDraw();
	}
}

bool CParticleSystem::IsEmpty()
{
	return mParticles.empty();
}

bool CParticleSystem::IsDead()
{
	std::deque<SAttribute>::iterator theIterator = mParticles.begin( );
	std::deque<SAttribute>::iterator theEnd = mParticles.end( );
	for( ; theIterator != theEnd; ++theIterator )
	{
		// is there at least one living particle?  If yes,
		// the system is not dead.
		if( theIterator->mIsAlive )
			return false;
	}
	// no living particles found, the system must be dead.
	return true;
}

void CParticleSystem::RemoveDeadParticles()
{
	// remove all corpses at the front:
	while(mParticles.empty()==false && mParticles.front().mIsAlive==false)
		mParticles.pop_front();
}
void CParticleSystem::OnLostDevice()
{
	// we must release the VB to the device can reset
	mpVertexBuffer->Release();
}
void CParticleSystem::OnResetDevice()
{
	// recreate the device
	if (FAILED(mpDevice->CreateVertexBuffer(	mpVertexBufferSize * sizeof(SPointSprite),
											D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
											D3DFVF_POINTSPRITE,
											D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
											&mpVertexBuffer,
											0 )))
		FAIL( "CreateVertexBuffer() - FAILED", "CParticleSystem");
}


/**
*	Return random float in [inLowBound, inHighBound] interval.
*	@return random float between inLowBound and inHighBound
*/
float CParticleSystem::GetRandomFloat( float inLowBound, float inHighBound )
{
	// get random float in [0, 1] interval
	float theRandomFloat = (rand() % 10000) * 0.0001f; 

	// return float in [inLowBound, inHighBound] interval. 
	return ( theRandomFloat * ( inHighBound - inLowBound ) ) + inLowBound; 
}

/**
*	Returns a random vector in the bounds specified by inMin and inMax.
*	@return random vector between inMin and inMax
*/
D3DXVECTOR3 CParticleSystem::GetRandomVector( const D3DXVECTOR3& inMin, const D3DXVECTOR3& inMax )
{
	return D3DXVECTOR3(GetRandomFloat( inMin.x, inMax.x ),
							GetRandomFloat( inMin.y, inMax.y ),
							GetRandomFloat( inMin.z, inMax.z ));
}
D3DXVECTOR3 CParticleSystem::GetRandomDirection()
{
	D3DXVECTOR3 v(GetRandomFloat(-1,1),GetRandomFloat(-1,1),GetRandomFloat(-1,1));
	D3DXVec3Normalize(&v,&v);
	return v;
}

D3DXVECTOR3 CParticleSystem::VaryDirection(const D3DXVECTOR3& vec, float angRad)
{
	D3DXMATRIX rot;
	D3DXMatrixRotationYawPitchRoll(&rot,GetRandomFloat(-angRad,angRad),GetRandomFloat(-angRad,angRad),GetRandomFloat(-angRad,angRad));
	D3DXVECTOR3 result;
	D3DXVec3TransformCoord(&result,&vec,&rot);
	return result;
}

D3DCOLOR CParticleSystem::GetRandomColour()
{
	return D3DCOLOR_XRGB(rand()%256,rand()%256,rand()%256);
}

//*****************************************************************************
// CPrecipitation System
//***************

CPrecipitation::CPrecipitation( D3DXVECTOR3 dimensions)
{
	mCentre=D3DXVECTOR3(0,0,0);
	mDimension=dimensions;
	SetEmitDirection(D3DXVECTOR3(0,-5,0),D3DX_PI/180*10);
}

bool CPrecipitation::Init( IDirect3DDevice9* inDevice, char* inTextureFilename,D3DXVECTOR3 dir,float variation)
{
	SetEmitDirection(dir,variation);
	SParticleSetting setting;
	setting.LifeTime=FLT_MAX;	// particle life is forever
	setting.Size=0.1f;
	setting.StartColor=D3DCOLOR_XRGB(255,255,255);	// start bright
	setting.EndColor=D3DCOLOR_XRGB(0,0,0);	// cool to black
	setting.SourceBlend=D3DBLEND_SRCALPHA;
	setting.DestBlend=D3DBLEND_SRCALPHA;
	setting.MaxParticles=5000;
	
	return CParticleSystem::Init(inDevice,inTextureFilename,setting);
}

void CPrecipitation::Update( float inDeltaTime )
{
	// could call the base class, but not bothering:

	// compute limits:
	D3DXVECTOR3 minA=mCentre-mDimension/2;
	D3DXVECTOR3 maxA=mCentre+mDimension/2;
	// add new particles:
	while(mParticles.size()< mSettings.MaxParticles)
	{
		// add them
        //TODO("Add particle");
		D3DXVECTOR3 pos = D3DXVECTOR3(GetRandomVector(minA, maxA));
		D3DXVECTOR3 vel = mEmitDirection * mEmitDirVar;
		AddParticle(pos,vel);
        // particle should be between minA and maxA for location
        // and should be moving in direction mEmitDirection
        // but the direction should vary a bit (mEmitDirVar);
	}
	// move all:
	std::deque<SAttribute>::iterator theIterator = mParticles.begin();
	std::deque<SAttribute>::iterator theEnd = mParticles.end();
	for( ; theIterator != theEnd; ++theIterator )
	{
		theIterator->mPosition += theIterator->mVelocity * inDeltaTime;
		// wrap:
		if (theIterator->mPosition.x<minA.x)
			theIterator->mPosition.x+=mDimension.x;
		if (theIterator->mPosition.x>maxA.x)
			theIterator->mPosition.x-=mDimension.x;
		if (theIterator->mPosition.y<minA.y)
			theIterator->mPosition.y+=mDimension.y;
		if (theIterator->mPosition.y>maxA.y)
			theIterator->mPosition.y-=mDimension.y;
		if (theIterator->mPosition.z<minA.z)
			theIterator->mPosition.z+=mDimension.z;
		if (theIterator->mPosition.z>maxA.z)
			theIterator->mPosition.z-=mDimension.z;
	}

}

//*****************************************************************************
// Explosion System
//********************
bool CExplosion::Init( IDirect3DDevice9* inDevice, char* inTextureFilename)
{
	SParticleSetting setting;
	setting.Size=0.25f;
	setting.LifeTime=1.0f;
	setting.SourceBlend=D3DBLEND_ONE;
	setting.DestBlend=D3DBLEND_ONE;
	setting.MaxParticles=1000;
	return CParticleSystem::Init(inDevice,inTextureFilename,setting);
}
void CExplosion::Explode(const D3DXVECTOR3& pos,float speed,int number)
{
	//TODO(CExplosion::Explode);
	// AddParticle doesn't allow for random colour
	// so we do it all ourselves

	// TODO: add in 'number' particles
	// with a random start colour & endCol
	// & a random direction
	for(int i = 0 ; i<number; i++)
	{
		D3DXCOLOR endCol= GetRandomColour();
		D3DXCOLOR startCol = GetRandomColour();
		D3DXVECTOR3 dir = speed * GetRandomDirection();
		AddParticle(pos, dir, startCol,endCol,mSettings.LifeTime);
	}
}

void CExplosion::Explode(const D3DXVECTOR3& pos,D3DXCOLOR startCol,D3DXCOLOR endCol,float speed,int number)
{
	//TODO(CExplosion::Explode);
	
	for(int i = 0 ; i<number; i++)
	{
		AddParticle(pos, speed*GetRandomDirection(), startCol,endCol,mSettings.LifeTime);
	}
	// TODO: add in 'number' particles
	// with a random direction 
	// & the specified colours
}