/*==============================================
 * Shot class
 *
 * Written by <YOUR NAME HERE>
 *
 *==============================================*/
#pragma once
#include "Node.h"

/// extra class for shot
/// the life is the number of miliseconds the shot will live for
/// its a simple way to ensure they they don't exist forever
class CShot: public CMeshNode
{
public:
	D3DXVECTOR3 mVel;	///< velocity
	virtual void Update(float dt)
	{
		mPos+=mVel*dt;	// move it
		mLife-=(int)(1000*dt);	// decrease life
	}
};