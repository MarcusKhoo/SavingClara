#pragma once
#include "Node.h"
#include "Collision.h"
#include "GameUtils.h"

class Boss: public CMeshNode
{
	//static int enemyNum = 0; //enable to test single enemy
	enum state {CHASING, ATTACKING, IDLE};
	static const int DETECT_DISTANCE = 20;  //  radius when the enemy is in sight
	static const int ATTACK_DISTANCE = 10;  //  radius when the enemy is in attack range
	static const int ROTSPEED = 3.0f;
	static const int SPEED = 3.0f;

	float counter;
public:
	Boss();
	void Update(CMeshNode* _player,float dt);
	state GetState(){return _state;}
	bool Shoot();
	float Dmg();
	void SetDmgMultiplier(float dmgMultiplier);
private:
	void PlayerInSight(float yaw, float delta);
	void RotateTowardsTarget(float dira,float dirb);
	state _state;
	bool shoot;
	float _dmgMultiplier;
	float d;
	//int myNum; //enable to test single enemy
};