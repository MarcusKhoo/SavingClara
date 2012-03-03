
#pragma once
#include "Enemy.h"
#include "ConsoleOutput.h"
#include <sstream>

static bool alerted;  //  if the ball hits the enemy

Enemy::Enemy()
{
	damage = false;
	counter = 1;
	timer = 0;
	alerted = false;
	attacking = false;
	_state = LOOKING;  //  enemies start rotating and looking around their surroundings
	//myNum = enemyNum; // enable to test single enemy
	//enemyNum++; // enable to test single enemy
}

void Enemy::Update(CMeshNode* _player,float dt)
{
	float damageMin = 0.0f;
	float damageMax = 10.0f;
	float yaw = GetHpr().x;
	D3DXVECTOR3 pPos = _player->GetPos();
	D3DXVECTOR3 myPos = GetPos();
	float delta=GetDirection(pPos-myPos);
	NormalizeRotation(this);
	switch(_state)
	{
	case PATROLLING:  //  enemies atart walking around
		attacking = false;
		if (rand()%1000 < 10)  //  percentage chance to stop walking and start looking around
		{
			_state = LOOKING;
			direction = randi(2);
		}
		Move(D3DXVECTOR3(0,0,D2R(1*SPEED)));  //  walking
		if (GetDistance(_player->GetPos(),mPos)<=ATTACK_DISTANCE)  //  within attack distance
		{
			_state = ATTACKING;
		}
		if (GetDistance(_player->GetPos(),mPos)<=DETECT_DISTANCE)  //  within radius
		{
			_state = CHASING;
		}
		break;
	case LOOKING:  //  enemies start looking around
		attacking = false;
		if(direction)
			Yaw(D2R(-1*ROTSPEED));
		else
			Yaw(D2R(1*ROTSPEED));
		if (rand()%1000 < 10)
			_state = PATROLLING;
		if (GetDistance(_player->GetPos(),mPos)<=ATTACK_DISTANCE)  //  within attack distance
		{
			_state = ATTACKING;
		}
		if (GetDistance(_player->GetPos(),mPos)<=DETECT_DISTANCE)  //  within radius
		{
			_state = CHASING;
		}
		break;
	case CHASING:  //  enemy chases you
		attacking = true;
		if (GetDistance(_player->GetPos(),mPos)<=ATTACK_DISTANCE)  //  within attack distance
		{
			_state = ATTACKING;
		}
		if (GetDistance(_player->GetPos(),mPos)>DETECT_DISTANCE)  //  out of radius
		{
			_state = PATROLLING;
		}
		PlayerInSight(yaw,delta);
		break;
	case ATTACKING:  //  enemy attacks you
		attacking = true;
		if (GetDistance(_player->GetPos(),mPos)>ATTACK_DISTANCE)  //  out of attacking distance
		{
			_state = CHASING;
		}
		if (GetDistance(_player->GetPos(),mPos)>DETECT_DISTANCE)  //  out of radius
		{
			_state = PATROLLING;
		}
		RotateTowardsTarget(yaw,delta);
		if(counter > 1)
		{
			counter -= 1.0f;
			_player->Damage(randf(damageMin,damageMax));
			damage = true;
		}
		counter += dt;
		//std::cout << "life: " << _player->mLife << std::endl;
		break;
	case ALERTED:
		if (GetDistance(_player->GetPos(),mPos)<=ATTACK_DISTANCE)  //  within attack distance
		{
			_state = ATTACKING;
			alerted = false;
			timer = 0;
		}
		RotateTowardsTarget(yaw,delta);
		Move(D3DXVECTOR3(0,0,D2R(1*SPEED)));  //  walking
		if(timer >= 10)
		{
			alerted = false;
			_state = PATROLLING;
			timer = 0;
		}
		timer += dt;
		break;
	}
	if(alerted)  //  if enemy is hit by attack
	{
		if (GetDistance(_player->GetPos(),mPos)<=HEARING_DISTANCE)  //  within detection distance
		{
			_state = ALERTED;
		}
	}
}

//  to make the enemy rotate and face you
void Enemy::RotateTowardsTarget(float dira,float dirb)
{
	if(NormalizeAngle(dira-dirb) > 0)
		Yaw(D2R(-1*ROTSPEED));
	else if(NormalizeAngle(dira-dirb) < 0)
		Yaw(D2R(1*ROTSPEED));
}

//  to check if the player can be seen
void Enemy::PlayerInSight(float yaw, float delta)
{
	//if(myNum == 210) // enable to test single enemy
	//{ // enable to test single enemy
	//float delta = atan2(pPos.x - myPos.x,pPos.z - myPos.z);
	if(GetDeltaDirection(yaw , delta) < D2R(ENEMYSIGHT))  //  field of vision of the enemy(angle of enemy can see you)
	{
		if(!(GetDeltaDirection(yaw , delta) < D2R(ROTSPEED*1.5)))  //  threshold to prevent enemy from vibrating while walking
		{
			RotateTowardsTarget(yaw,delta);
		}
	}
	Move(D3DXVECTOR3(0,0,D2R(1*SPEED)));  //  walking
	//std::cout << "delta: " << delta << "    yaw: " << yaw << std::endl;
	//} // enable to test single enemy
}

//  when u shoot, enemy hear u shooting and chase u
void Enemy::Alerted()
{
	alerted = true;
}

//  when ur attack hits the enemy, he comes chase u
void Enemy::Alerted(CMeshNode* _player)
{
	if (GetDistance(_player->GetPos(),mPos)<=DETECTION_DISTANCE)
	{
		_state = ALERTED;
	}
}

//  returns value of whether the enemy is attacking you or not
bool Enemy::IsAttacking()
{
	return attacking;
}

void DeleteEnemy(std::vector<Enemy*>& e)
{
	for(int i = (int)e.size()-1; i>=0; i--)
	{
		delete e[i];
	}
	e.clear();
}

void DrawEnemy(const std::vector<Enemy*>& e,CMeshNode* mPlayer)
{
	for(int i=0; i<e.size(); i++)
	{
		if(e[i]->IsAlive())
		{
			if(CollisionSphereSphere(e[i]->GetPos(),1.0f,mPlayer->GetPos(),50.0f))
				if(GetDeltaDirection(mPlayer->GetHpr().x,GetDirection(e[i]->GetPos()-mPlayer->GetPos()))<D2R(40))
					e[i]->Draw();
		}
	}
}

void DeleteDeadEnemy(std::vector<Enemy*>& e)
{
	// this is a bit tricky, so here it is in full
	for(int i=(int)e.size()-1;i>=0;i--)
	{
		if (e[i]->IsAlive()==false)
		{
			delete e[i];	// deletes the object
			// removed item at index 'i', by shifting down the items
			e.erase(e.begin()+i);
		}
	}
}

bool Enemy::Dmg()
{
	bool a = damage;
	damage = false;
	return a;
}