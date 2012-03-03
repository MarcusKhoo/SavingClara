
#pragma once
#include "Boss.h"
#include "ConsoleOutput.h"
#include <sstream>

Boss::Boss()
{
	counter = 1.0f;
	_state = IDLE;
	_dmgMultiplier = 1.0f;
	shoot = false;
}

void Boss::Update(CMeshNode* _player,float dt)
{
	float yaw = GetHpr().x;
	D3DXVECTOR3 pPos = _player->GetPos();
	D3DXVECTOR3 myPos = GetPos();
	float delta=GetDirection(pPos-myPos);
	NormalizeRotation(this);
	switch(_state)
	{
	case IDLE:
		d = GetDirection(D3DXVECTOR3(-165,0,125)-myPos);
		if(GetDistance(D3DXVECTOR3(-165,0,125),myPos)>=SPEED)
		{
			RotateTowardsTarget(yaw,d);
			Move(D3DXVECTOR3(0,0,D2R(1*SPEED)));  //  walking
		}
		if (GetDistance(_player->GetPos(),mPos)<=ATTACK_DISTANCE)  //  within attack distance
		{
			_state = ATTACKING;
		}
		if (GetDistance(_player->GetPos(),mPos)<=DETECT_DISTANCE)  //  within radius
		{
			_state = CHASING;
		}
		break;
	case CHASING:
		if (GetDistance(_player->GetPos(),mPos)<=ATTACK_DISTANCE)  //  within attack distance
		{
			_state = ATTACKING;
		}
		if (GetDistance(_player->GetPos(),mPos)>DETECT_DISTANCE)  //  out of radius
		{
			_state = IDLE;
		}
		PlayerInSight(yaw,delta);
		break;
	case ATTACKING:
		if (GetDistance(_player->GetPos(),mPos)>ATTACK_DISTANCE)  //  out of attacking distance
		{
			_state = CHASING;
		}
		if (GetDistance(_player->GetPos(),mPos)>DETECT_DISTANCE)  //  out of radius
		{
			_state = IDLE;
		}
		RotateTowardsTarget(yaw,delta);
		if(counter > 1)
		{
			counter -= 1.0f;
			shoot = true;
		}
		counter += dt;
		break;
	}
}

//  to make the enemy rotate and face you
void Boss::RotateTowardsTarget(float dira,float dirb)
{
	if(NormalizeAngle(dira-dirb) > 0)
		Yaw(D2R(-1*ROTSPEED));
	else if(NormalizeAngle(dira-dirb) < 0)
		Yaw(D2R(1*ROTSPEED));
}

//  to check if the player can be seen
void Boss::PlayerInSight(float yaw, float delta)
{
	//if(myNum == 210) // enable to test single enemy
	//{ // enable to test single enemy
	//float delta = atan2(pPos.x - myPos.x,pPos.z - myPos.z);
	if(!(GetDeltaDirection(yaw , delta) < D2R(ROTSPEED*1.5)))  //  threshold to prevent enemy from vibrating while walking
	{
		RotateTowardsTarget(yaw,delta);
	}
	Move(D3DXVECTOR3(0,0,D2R(1*SPEED)));  //  walking
	//std::cout << "delta: " << delta << "    yaw: " << yaw << std::endl;
	//} // enable to test single enemy
}

//  return whether the boss is shooting or not
bool Boss::Shoot()
{
	bool a = shoot;
	shoot = false;
	return a;
}

//  amount of damage the boss deals to the player if the shot hits
float Boss::Dmg()
{
	float damageMin = 1.0f;
	float damageMax = 50.0f;
	return _dmgMultiplier * randf(damageMin,damageMax);
}

//  set the percentage of damage the boss deals
void Boss::SetDmgMultiplier(float dmgMultiplier)
{
	_dmgMultiplier = dmgMultiplier;
}