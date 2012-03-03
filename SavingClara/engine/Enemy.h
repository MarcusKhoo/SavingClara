#pragma once
#include "Node.h"
#include "Collision.h"
#include "GameUtils.h"

class Enemy: public CMeshNode
{
	//static int enemyNum = 0; //enable to test single enemy
	enum state {CHASING, LOOKING, ATTACKING, PATROLLING, ALERTED};
	static const int DETECT_DISTANCE = 10;  //  radius when the enemy is in sight
	static const int ATTACK_DISTANCE = 2;  //  radius when the enemy is in attack range
	static const int HEARING_DISTANCE = 15;  //  radius when the enemy can hear you shooting
	static const int DETECTION_DISTANCE = 20;  //  radius when the enemy can detect you when your attack hits them
	static const int ROTSPEED = 3.0f;
	static const int SPEED = 4.0f;

	//angle which the enemy can see. value is in degrees NOT RADIANS!!!
	//when setting this value, take note that the value is 2 times.
	//eg. if u set ENEMYSIGHT = 35; the enemy can see 70degrees. 35 towards the left and 35 towards the right.
	static const int ENEMYSIGHT = 35;
	int timer;
	float counter;
public:
	Enemy();
	void Update(CMeshNode* _player,float dt);
	state GetState(){return _state;}
	void Alerted();
	bool IsAttacking();
	void Alerted(CMeshNode* _player);
	bool Dmg();
private:
	bool direction;
	void PlayerInSight(float yaw, float delta);
	void RotateTowardsTarget(float dira,float dirb);
	state _state;
	bool attacking;
	bool damage;
	//int myNum; //enable to test single enemy
};

void DeleteEnemy(std::vector<Enemy*>& e);
void DrawEnemy(const std::vector<Enemy*>& e,CMeshNode* mPlayer);
void DeleteDeadEnemy(std::vector<Enemy*>& e);