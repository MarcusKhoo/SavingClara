#pragma once
#include "Node.h"
#include "Collision.h"
#include "GameUtils.h"

class NPC: public CMeshNode
{
public:
	NPC();
	void Update(CMeshNode* player, bool lookAt);
	bool canTalk;
private:
};