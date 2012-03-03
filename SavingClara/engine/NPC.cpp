#pragma once

#include "NPC.h"

NPC::NPC()
{
	canTalk = false;
}

void NPC::Update(CMeshNode* player, bool lookAt)
{
	D3DXVECTOR3 pos = player->GetPos();

	if (lookAt)
		LookAt(D3DXVECTOR3(pos.x, 0, pos.z));

	if (mPos.x - 2 < pos.x && mPos.x + 2 > pos.x &&
		mPos.z - 2 < pos.z && mPos.z + 2 > pos.z)
		canTalk = true;
	else
		canTalk = false;
}
