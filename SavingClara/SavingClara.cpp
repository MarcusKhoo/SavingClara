#include "SavingClara.h"

/* Gameplay Scene*/
void GameScene::Enter()
{
	mpJoy=GetEngine()->FindComponent<CJoystickComponent>();

	mpSound = GetEngine()->FindComponent<CSoundComponent>();
	mpSound->StopCue("menu_01");
	mpSound->PlayCue("town");
	// camera
	SetupDefaultD3DOptions(GetDevice(), true);
	mCamera.SetAspect(GetEngine()->GetAspectRatio());
	mCamera.SetNearFar(0.01f,100000.0f);

	gameFont = CreateD3DFont(GetDevice(), "Segoe UI", 24, false);

	// crosshair
	mpCrosshairTex=LoadSpriteTex(GetDevice(),"media/Textures/crosshair.png");

	// terrain
	mpTerrain=new CTerrain(GetDevice(),"media/Terrains/heightmap.bmp",5,0.5,"media/Terrains/trees.bmp");
	mpTerrain->LoadTexture("media/Terrains/terrain_texture.png");

	// models
	mpMarcusMesh=new CXMesh(GetDevice(),"media/Models/marcus.X");
	mpClaraMesh = new CXMesh(GetDevice(), "media/Models/clara.X");
	mpMarkMesh = new CXMesh(GetDevice(), "media/Models/mark.X");
	mpFireballMesh = new CXMesh(GetDevice(), "media/Models/fireball.X");
	mpIcicleMesh = new CXMesh(GetDevice(), "media/Models/icicle02.X");
	mpMagicballMesh = new CXMesh(GetDevice(), "media/Models/magicball.X");
	mpEnemyWMesh = new CXMesh(GetDevice(), "media/Models/monsterWeak.X");
	mpEnemySMesh = new CXMesh(GetDevice(), "media/Models/monsterStrong.X");
	mpTreeMesh = new CXMesh(GetDevice(), "media/Models/tree.X");
	mpWandMesh = new CXMesh(GetDevice(), "media/Models/wand.X");
	mpHandMesh = new CXMesh(GetDevice(), "media/Models/hand.X");
	mpSkyBoxMesh = new CXMesh(GetDevice(), "media/Models/skyBox.X");
	mpHutMesh = new CXMesh(GetDevice(), "media/Models/hut.X");
	mpJinMesh = new CXMesh(GetDevice(), "media/Models/jin.X");
	mMarcus.node.Init(mpMarcusMesh);
	// the particles
	mpFire=new CParticleSystem();
	mpIce = new CParticleSystem();
	mpWandIce = new CParticleSystem();
	mpCurrSkillParticles = new CParticleSystem();
	mpIceCollide = new CExplosion();
	mpSnow = new CPrecipitation();

	// make sure its on the ground
	//mMarcus.node.SetPos(-100,mpTerrain->GetPointOnGround(D3DXVECTOR3(-165,0,125)).y,125);
	mMarcus.node.Init(mpMarcusMesh);
	//mMarcus.node.SetPos(200,mpTerrain->GetPointOnGround(mMarcus.node.GetPos()).y,-190);
	mMarcus.node.SetPos(-165,mpTerrain->GetPointOnGround(mMarcus.node.GetPos()).y,125);
	mMarcus.node.SetHpr(D2R(-60),0,0);
	mMarcus.state = mMarcus.GROUND;
	mMarcus.node.mLife = 100;
	// wand
	mWand.Init(mpWandMesh, mMarcus.node.OffsetPos(D3DXVECTOR3(0.07f,-0.1f,0.1f)),D3DXVECTOR3(D2R(-10),D2R(20),0));
	// hand
	mHand.Init(mpHandMesh, mMarcus.node.OffsetPos(D3DXVECTOR3(-0.07f, 0.2f, 0.5f)));
	// current skill
	mCurrSkill.Init(mpMagicballMesh, mMarcus.node.GetPos());
	mCurrSkill.mScale = 0.02f;

	// npcs
	mMark.Init(mpMarkMesh, D3DXVECTOR3(190,0,-190));
	mClara.Init(mpClaraMesh, D3DXVECTOR3(-165,mpTerrain->GetHeight(-165,125),125));

	magicballStartTime = 0;
	icicleStartTime = 0;
	SetupLinearFog(GetDevice(), D3DCOLOR_XRGB(200,200,200), 0.0, 200, D3DFOG_LINEAR, 0.01f, true);

	weaponToggle = 1;

	icicle_charge = 0;

	for(int x =  -50; x<50; x+= 10)
	{
		for(float z=-50;z<50;z+= 10)
		{
			Enemy* enemy = new Enemy();
			D3DXVECTOR3 pos(x, mpTerrain->GetHeight(x,z), z);
			enemy->Init(mpEnemySMesh, pos);
			enemy->mLife = 100;
			mEnemies.push_back(enemy);
		}
	}

	D3DXVECTOR3 pos(-165,mpTerrain->GetHeight(-165,125),125);
	mJin.Init(mpJinMesh,pos);
	mJin.mLife = 1000;

	ZeroMemory(&emitter, sizeof(emitter));

	// setup particles
	InitParticles();

	mTransOnTime=2;		// slow fade in
	mTransOffTime=2;	// slow fade out

	CGameWindow::ClearKeyPress();
}
void GameScene::Update(float dt)
{
	CScene::Update(dt);	// MUST CALL BASE CLASS'S Update
	
	// Pause if I'm not the topmost screen
	if (IsTopMost()==false)	return;

	// Pause if I'm fading out
	if (GetState()==FADE_OUT)
	{
		mpSound->StopCategory("Music");
		mpSound->StopCategory("SFX");
		return;
	}

	D3DXVECTOR3 oldPos = mMarcus.node.GetPos();

	//sound
	UpdateBackgroundMusic();

	// update player
	UpdatePlayer(dt);

	// update npcs
	mMark.Update(&mMarcus.node, true);
	mClara.Update(&mMarcus.node, false);

	// first person view
	mCamera.Match(mMarcus.node);

	mCurrSkill.SetPos(mMarcus.node.OffsetPos(D3DXVECTOR3(-0.09f,-0.05f,0.2f)));
	mCurrSkill.Yaw(D2R(45*dt));

	mWand.SetPos(mMarcus.node.OffsetPos(D3DXVECTOR3(0.07f,-0.1f,0.1f)));
	mWand.SetHpr(mCamera.GetHpr());

	mHand.SetPos(mMarcus.node.OffsetPos(D3DXVECTOR3(-0.07f, -0.1f, 0.1f)));
	mHand.SetHpr(mCamera.GetHpr());

	//check collisions
	CheckCollisions(oldPos);

	// move shots
	UpdateMeshNodes(mMagicball, dt);
	UpdateMeshNodes(mIcicles, dt);
	UpdateMeshNodes(mFireball, dt);
	for(int i=(int)mEnemies.size()-1;i>=0;i--)
	{
		mEnemies[i]->Update(&mMarcus.node,dt);
		if(mEnemies[i]->Dmg())
			mMarcus.lastHitTime=0;
		if (mEnemies[i]->mLife <= 0)
		{
			if (activeQuest)
			{
				monstersSlain++;
				if (monstersSlain >= 20)
				{
					questCompleted = true;
					monstersSlain = 20;
				}
			}
		}
	}

	if(mJin.IsAlive())
	{
		mJin.Update(&mMarcus.node,dt);
		NormalizeRotation(&mMarcus.node);
		if (wearArmour)
			mJin.SetDmgMultiplier(0.5f);
		if(mJin.Shoot())
		{
			CShot* shot = new CShot();

			D3DXVECTOR3 dir = mJin.RotateVector(D3DXVECTOR3(0,0,1));

			// setup the magicball
			shot->Init(mpFireballMesh, mJin.GetPos(), mJin.GetHpr());
			shot->mPos = mJin.OffsetPos(D3DXVECTOR3(0.2f,0.2f,0.0f));
			shot->mVel = dir*SHOT_VEL;
			shot->mScale = 0.3f;
			shot->mLife = 3000;

			// play sound
			pCue = mpSound->Play3DCue("fireball_cast");
			vec_pCue.push_back(pCue);
			emitter.Position = shot->GetPos();
			vec_emitter.push_back(emitter);

			mFireball.push_back(shot);
		}
	}
	else
		defeatedJin = true;

	UpdateParticles(dt, mMagicball, mIcicles, mFireball);

	// update 3D sound
	FillListener(mCamera, listener);
	FillEmitter(vec_emitter);
	mpSound->Apply3D(vec_pCue, listener, vec_emitter);

	// delete dead meshes
	DeleteDeadMeshNodes(mMagicball);
	DeleteDeadMeshNodes(mIcicles);
	DeleteDeadMeshNodes(mFireball);
	DeleteDeadEnemy(mEnemies);
}
void GameScene::UpdatePlayer(float dt)
{
	// basic movement
	HandleInput(dt);

	// first person view
	mCamera.Match(mMarcus.node);

	// health regeneration
	if (mMarcus.node.mLife <= 0)
	{
		ExitScene();
		GetEngine()->AddScene(new GameOver());
	}
	else if (mMarcus.node.mLife < 100)
	{
		mMarcus.lastHitTime+=dt;
		if(mMarcus.node.mLife < 70 && mMarcus.node.mLife >= 40)
		{
			if(!mpSound->IsCuePlaying("heartbeat_slow"))
			{
				mpSound->StopCategory("Default");
				mpSound->PlayCue("heartbeat_slow");
			}
		}
		else if(mMarcus.node.mLife < 40)
		{
			if(!mpSound->IsCuePlaying("heartbeat_fast"))
			{
				mpSound->StopCategory("Default");
				mpSound->PlayCue("heartbeat_fast");
			}
		}
		if (mMarcus.lastHitTime > HEALTH_COOLDOWN)
			mMarcus.node.mLife += 0.1f;
	}
	else
	{
		mpSound->StopCategory("Default");
		mMarcus.node.mLife = 100;
		mMarcus.lastHitTime = 0;
	}
}
void GameScene::HandleInput(float dt)
{
	D3DXVECTOR3 move,turn;

	// check to see if there is a joystick:
	if (mpJoy->IsJoyActive(0))
	{
		// read joystick:
		move=mpJoy->GetJoystickPtr(0)->GetVector(1,0,-2);
		turn=mpJoy->GetJoystickPtr(0)->GetVector(4,5,0);

		DIJOYSTATE state = mpJoy->GetJoystick(0).GetState();
		DIJOYSTATE prevState = mpJoy->GetJoystick(0).GetPrevState();

		/************************************************
		*	state.rgbButtons							*
		*	0	=	A				1	=	B			*
		*	2	=	X				3	=	Y			*
		*	4	=	L Bumper		5	=	R Bumper	*
		*	6	=	Back			7	=	Start		*
		*	8	=	L Stick			9	=	R Stick		*
		************************************************/

		if (state.rgbButtons[6] != 0 && prevState.rgbButtons[6] == 0)
			ConfirmExit();

		if (state.rgbButtons[1] != 0 && prevState.rgbButtons[1] == 0)
			PlayerJump();

		if (state.rgbButtons[4] != 0 && prevState.rgbButtons[4] == 0)
			ChangeSpell(1);
		else if (state.rgbButtons[5] != 0 && prevState.rgbButtons[5] == 0)
			ChangeSpell(2);

		if (state.rgbButtons[2] != 0 && prevState.rgbButtons[2] == 0)
			TalkToNPC();

		switch (weaponToggle)
		{
		case 1:
			if (state.rgbButtons[0] != 0 && prevState.rgbButtons[0] == 0)
				CastSpell(false);
			break;
		case 2:
			if (state.rgbButtons[0] != 0)
				CastSpell(true);
			else
				CastSpell(false);
			break;
		}
	}
	else
	{
		// read keyboard instead:
		turn=GetMouseTurnVector(45, true);
		move=GetKeyboardVector(WSAD_KEYS);

		if (CGameWindow::KeyPress(VK_ESCAPE))
			ConfirmExit();

		if (CGameWindow::KeyPress(VK_SPACE))
			PlayerJump();

		if (CGameWindow::KeyPress('1'))
			ChangeSpell(1);
		else if (CGameWindow::KeyPress('2'))
			ChangeSpell(2);

		if (CGameWindow::KeyPress('E'))
			TalkToNPC();

		switch (weaponToggle)
		{
		case 1:
			if (CGameWindow::KeyPress(VK_LBUTTON))
				CastSpell(false);
			break;
		case 2:
			if (CGameWindow::KeyDown(VK_LBUTTON))
				CastSpell(true);
			else
				CastSpell(false);
			break;
		}
	}

	if(CGameWindow::KeyPress('B'))
		mMarcus.node.SetPos(200,mpTerrain->GetPointOnGround(mMarcus.node.GetPos()).y,-190);

	// do the movement
	const float TURN_SPEED=D2R(90);
	const float SPEED=5.0f;
	mMarcus.node.Turn(turn*dt*TURN_SPEED);
	mMarcus.node.MoveGround(move*dt*SPEED);

	// player can only rotate up and down 45 degrees
	if (mMarcus.node.GetHpr().y <= -D3DXToRadian(45))
		mMarcus.node.mHpr.y = -D3DXToRadian(45);
	else if (mMarcus.node.GetHpr().y >= D3DXToRadian(45))
		mMarcus.node.mHpr.y = D3DXToRadian(45);

	// do the jumping
	D3DXVECTOR3 rate = D3DXVECTOR3(0,9.81,0);
	switch	(mMarcus.state)
	{
	case mMarcus.JUMP:
		mMarcus.node.SetPos(mMarcus.node.GetPos()+rate*dt);
		if (mMarcus.node.GetPos().y > mMarcus.jumpHeight)
			mMarcus.state = mMarcus.FALL;
		break;
	case mMarcus.FALL:
		mMarcus.node.SetPos(mMarcus.node.GetPos()-rate*dt);
		if (mMarcus.node.GetPos().y <= mpTerrain->GetPointOnGround(mMarcus.node.GetPos(),1.0f).y)
			mMarcus.state = mMarcus.GROUND;
		break;
	case mMarcus.GROUND:
		mMarcus.node.SetPos(mpTerrain->GetPointOnGround(mMarcus.node.GetPos(),1.0f));
		break;
	}

	switch (weaponToggle)
	{
	case 1:
		if (clock()-magicballStartTime > MAGICBALL_COOLDOWN && MB_COOLDOWN)
			MB_COOLDOWN = false;
		break;
	case 2:
		if (clock()-icicleStartTime > ICICLE_COOLDOWN && ICE_COOLDOWN)
			ICE_COOLDOWN = false;
		break;
	}
}
void GameScene::ConfirmExit()
{
	CGameWindow::ClearKeyPress();
	CMessageBoxScene* pMsg=new CMessageBoxScene("Do you want to exit?");
	pMsg->OnOkCloseScene(this);
	GetEngine()->AddScene(pMsg);
}
void GameScene::PlayerJump()
{
	CGameWindow::ClearKeyPress();
	if (mMarcus.state == mMarcus.GROUND)
	{
		mMarcus.state = mMarcus.JUMP;
		mMarcus.jumpHeight = mMarcus.node.GetPos().y + 2;
	}
}
void GameScene::ChangeSpell(int spell)
{
	CGameWindow::ClearKeyPress();
	weaponToggle = spell;
	switch (spell)
	{
	case 1:
		mCurrSkill.Init(mpMagicballMesh, mMarcus.node.GetPos());
		mCurrSkill.mScale = 0.03f;
		break;
	case 2:
		mCurrSkill.Init(mpIcicleMesh, mMarcus.node.GetPos());
		mCurrSkill.mScale = 0.02f;
		break;
	}
}
void GameScene::CastSpell(bool charge)
{
	D3DXVECTOR3 dir = mMarcus.node.RotateVector(D3DXVECTOR3(0,0,1));

	switch (weaponToggle)
	{
	case 1:
		if (clock()-magicballStartTime > MAGICBALL_COOLDOWN)
		{
			CShot* shot = new CShot();

			// setup the magicball
			shot->Init(mpMagicballMesh, mMarcus.node.GetPos(), mMarcus.node.GetHpr());
			shot->mPos = mMarcus.node.OffsetPos(D3DXVECTOR3(0.2f,0,2.0f));
			shot->mVel = dir*SHOT_VEL;
			shot->mScale = 0.3f;
			shot->mLife = 3000;

			// play sound
			pCue = mpSound->Play3DCue("frost_cast_01");
			vec_pCue.push_back(pCue);
			emitter.Position = shot->GetPos();
			vec_emitter.push_back(emitter);

			mMagicball.push_back(shot);
			magicballStartTime = clock();
			MB_COOLDOWN = true;
			mpCurrSkillParticles->GetSettings().Size = 0.0f;
		}
		break;
	case 2:
		if (clock()-icicleStartTime > ICICLE_COOLDOWN)
		{
			if (charge)
			{
				if(icicle_charge < FULLY_CHARGED)
					icicle_charge += 1;
			}
			else if (icicle_charge > 0)
			{
				CShot* shot = new CShot();

				// setup the icicle
				shot->Init(mpIcicleMesh, mMarcus.node.GetPos(), mMarcus.node.GetHpr());
				shot->mPos = mMarcus.node.OffsetPos(D3DXVECTOR3(0.2f,0,2.0f));
				shot->mVel = dir*SHOT_VEL;
				shot->mScale = 0.3f + (icicle_charge/100.0f);
				shot->mLife = 2000;
				mIcicles.push_back(shot);
				icicleDamage = 20+icicle_charge;
				icicle_charge = 0;
				icicleStartTime = clock();

				// play sound
				pCue = mpSound->Play3DCue("frost_cast_02");
				vec_pCue.push_back(pCue);
				emitter.Position = shot->GetPos();
				vec_emitter.push_back(emitter);
				ICE_COOLDOWN = true;
				mpCurrSkillParticles->GetSettings().Size = 0.0f;
			}
		}
		break;
	}

	mEnemies[0]->Alerted();
}
void GameScene::TalkToNPC()
{
	if (mMark.canTalk)
	{
		GetEngine()->AddScene(new QuestScene());
	}
	else if(mClara.canTalk)
	{
		if (defeatedJin)
		{
			ExitScene();
			GetEngine()->AddScene(new GameOver());
		}
	}
}
void GameScene::CheckCollisions(D3DXVECTOR3 oldPos)
{
	// check collision
	for(int sh=0;sh<mMagicball.size();sh++)
	{
		for(int en=0;en<mEnemies.size();en++)
		{
			// if it occurs, destroy the shot and do 50 damage to the enemy
			if(CollisionMeshNode(mMagicball[sh], mEnemies[en],1.1f))
			{
				mpIceCollide->Explode(mMagicball[sh]->GetPos(),
					D3DCOLOR_XRGB(0,0,255),
					D3DCOLOR_XRGB(50,50,255),
					CParticleSystem::GetRandomFloat(1.5f,2.0f));

				//sound
				pCue = mpSound->Play3DCue("frost_hit_01");
				vec_pCue.push_back(pCue);
				emitter.Position = mMagicball[sh]->GetPos();
				vec_emitter.push_back(emitter);

				mMagicball[sh]->Destroy();
				mEnemies[en]->Alerted(&mMarcus.node);
				mEnemies[en]->Damage(randi(20,30));
				break;
			}
		}
		if(mMagicball[sh]->GetPos().y < mpTerrain->GetHeight(mMagicball[sh]->GetPos().x, mMagicball[sh]->GetPos().z))
		{
			mpIceCollide->Explode(mMagicball[sh]->GetPos(),
				D3DCOLOR_XRGB(0,0,255),
				D3DCOLOR_XRGB(50,50,255),
				CParticleSystem::GetRandomFloat(1.5f,2.0f));

			// sound
			pCue = mpSound->Play3DCue("frost_hit_01");
			vec_pCue.push_back(pCue);
			emitter.Position = mMagicball[sh]->GetPos();
			vec_emitter.push_back(emitter);

			mMagicball[sh]->Destroy();
			break;
		}
		
		if(mJin.IsAlive())
		{
			if(CollisionMeshNode(mMagicball[sh], &mJin,1.5f))
			{
				mpIceCollide->Explode(mMagicball[sh]->GetPos(),
					D3DCOLOR_XRGB(0,0,255),
					D3DCOLOR_XRGB(50,50,255),
					CParticleSystem::GetRandomFloat(1.5f,2.0f));

				// sound
				pCue = mpSound->Play3DCue("frost_hit_01");
				vec_pCue.push_back(pCue);
				emitter.Position = mMagicball[sh]->GetPos();
				vec_emitter.push_back(emitter);

				mJin.Damage(randi(20,30));
				mMagicball[sh]->Destroy();
				break;
			}
		}
	}

	for(int sh=0;sh<mIcicles.size();sh++)
	{
		for(int en=0;en<mEnemies.size();en++)
		{
			// if it occurs, destroy the shot and do 50 damage to the enemy
			if(CollisionMeshNode(mIcicles[sh], mEnemies[en],1.1f))
			{
				mpIceCollide->Explode(mIcicles[sh]->GetPos(),
					D3DCOLOR_XRGB(0,157,157),
					D3DCOLOR_XRGB(0,0,0),
					CParticleSystem::GetRandomFloat(1.5f+(icicleDamage/20.0f),2.0f+(icicleDamage/20.0f)),
					icicleDamage*20);

				// sound
				pCue = mpSound->Play3DCue("frost_hit_02");
				vec_pCue.push_back(pCue);
				emitter.Position = mIcicles[sh]->GetPos();
				vec_emitter.push_back(emitter);

				mIcicles[sh]->Destroy();
				mEnemies[en]->Alerted(&mMarcus.node);
				mEnemies[en]->Damage(icicleDamage);
				break;
			}
		}
		if(mIcicles[sh]->GetPos().y < mpTerrain->GetHeight(mIcicles[sh]->GetPos().x, mIcicles[sh]->GetPos().z))
		{
			mpIceCollide->Explode(mIcicles[sh]->GetPos(),
				D3DCOLOR_XRGB(0,157,157),
				D3DCOLOR_XRGB(0,0,0),
				CParticleSystem::GetRandomFloat(1.5f,2.0f));

			// sound
			pCue = mpSound->Play3DCue("frost_hit_02");
			vec_pCue.push_back(pCue);
			emitter.Position = mIcicles[sh]->GetPos();
			vec_emitter.push_back(emitter);

			mIcicles[sh]->Destroy();
			break;
		}

		else if(CollisionMeshNode(mIcicles[sh], &mJin, 1.5f))
		{
			mpIceCollide->Explode(mIcicles[sh]->GetPos(),
				D3DCOLOR_XRGB(0,157,157),
				D3DCOLOR_XRGB(0,0,0),
				CParticleSystem::GetRandomFloat(1.5f,2.0f));

			// sound
			pCue = mpSound->Play3DCue("frost_hit_02");
			vec_pCue.push_back(pCue);
			emitter.Position = mIcicles[sh]->GetPos();
			vec_emitter.push_back(emitter);
			mJin.Damage(icicleDamage);
			mIcicles[sh]->Destroy();
			break;
		}
	}

	for(int i = 0; i<mpTerrain->treesPosition.size(); i++)
	{
		if(CollisionSphereSphere(D3DXVECTOR3(mpTerrain->treesPosition[i].x,
			mpTerrain->GetHeight(mpTerrain->treesPosition[i].x, mpTerrain->treesPosition[i].y),
			mpTerrain->treesPosition[i].y),2.0f,mMarcus.node.GetPos(),1.0f))
			mMarcus.node.SetPos(oldPos);
	}

	for(int i = 0; i<mpTerrain->hutsPosition.size(); i++)
	{
		if(CollisionSphereSphere(D3DXVECTOR3(mpTerrain->hutsPosition[i].x,
			mpTerrain->GetHeight(mpTerrain->hutsPosition[i].x, mpTerrain->hutsPosition[i].y),
			mpTerrain->hutsPosition[i].y),2.0f,mMarcus.node.GetPos(),1.0f))
			mMarcus.node.SetPos(oldPos);
	}

	for(int i = mFireball.size()-1;i>=0;i--)
	{
		if(CollisionMeshNode(mFireball[i], &mMarcus.node, 1.5f))
		{
			mpSound->PlayCue("fireball_hit");
			mMarcus.node.Damage(mJin.Dmg());
			mFireball[i]->Destroy();
			break;
		}
	}
}
void GameScene::Draw(float dt)
{
	mCamera.SetMatrixes(GetDevice());
	GetDevice()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, WHITE_COL * 0.9f , 1.0f, 0 );
	GetDevice()->BeginScene();

	// draw terrain
	mpTerrain->Draw(IDENTITY_MAT,false);

	// draw trees
	for(int i = 0; i<mpTerrain->treesPosition.size(); i++)
	{
		if(CollisionSphereSphere(D3DXVECTOR3(mpTerrain->treesPosition[i].x,
			mpTerrain->GetHeight(mpTerrain->treesPosition[i].x, mpTerrain->treesPosition[i].y),
			mpTerrain->treesPosition[i].y),1.0f,mMarcus.node.GetPos(),100.0f))
		{
			if(GetDeltaDirection(mMarcus.node.GetHpr().x , GetDirection(D3DXVECTOR3(mpTerrain->treesPosition[i].x,
				mpTerrain->GetHeight(mpTerrain->treesPosition[i].x, mpTerrain->treesPosition[i].y),
				mpTerrain->treesPosition[i].y)-mMarcus.node.GetPos())) < D2R(40))  //  field of vision of the enemy(angle of enemy can see you)
			{
				mpTreeMesh->Draw(D3DXVECTOR3(mpTerrain->treesPosition[i].x,
					mpTerrain->GetHeight(mpTerrain->treesPosition[i].x, mpTerrain->treesPosition[i].y),
					mpTerrain->treesPosition[i].y),8,0);
			}

		}
	}
	// draw huts
	for(int i = 0; i<mpTerrain->hutsPosition.size(); i++)
	{
		mpHutMesh->Draw(D3DXVECTOR3(mpTerrain->hutsPosition[i].x,
			mpTerrain->GetHeight(mpTerrain->hutsPosition[i].x, mpTerrain->hutsPosition[i].y),
			mpTerrain->hutsPosition[i].y), 2, 0);
	}
	// draw wand and current skill
	mWand.Draw();
	mHand.Draw();
	mCurrSkill.Draw();

	// draw boss
	if(mJin.IsAlive())
		mJin.Draw();

	// draw npcs
	mMark.Draw();
	mClara.Draw();

	// draw shots
	DrawMeshNodes(mMagicball);
	DrawMeshNodes(mIcicles);
	DrawMeshNodes(mFireball);

	// draw skybox
	mpSkyBoxMesh->Draw(mMarcus.node.GetPos());

	// draw enemies
	for(unsigned i=0;i<mEnemies.size();i++)
		mEnemies[i]->DrawBounds(GetDevice());
	DrawEnemy(mEnemies,&mMarcus.node);

	DrawParticles();

#if DEBUG
	stringstream sout;
	sout << "Player pos: " << mMarcus.node.GetPos();
	sout << "Player hpr: " << mMarcus.node.GetHpr();
	sout << "\nCharge value: " << icicle_charge; 
	sout << "\nNo of enemies: " << mEnemies.size();
	sout << "\nIcicle blast damage: " << icicleDamage;
	sout << "\nCurrent time: " << clock();
	sout << "\nClock: " << clock();
	sout << "\n DT: " << dt;
	sout << "\n FPS: " << GetEngine()->GetFps();
	sout << "\n Health: " << mMarcus.node.mLife;
	sout << "\n Boss Health: " << mJin.mLife;
	DrawD3DFont(gameFont, sout.str().c_str(), 20,20, RED_COL);
#endif
	if (activeQuest)
	{
		stringstream ss;
		ss << "Quest:" << endl;
		if (!questCompleted)
			ss << "Monsters Slain: " << monstersSlain << "/20";
		else
			ss << "Monsters Slain: Completed!";
		DrawD3DFont(gameFont, ss.str().c_str(), 10, 10, BLACK_COL);
	}

	// Draw crosshair
	GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
	DrawSprite(GetSprite(),mpCrosshairTex,GetEngine()->GetWindowRect().right/2-4,GetEngine()->GetWindowRect().bottom/2-4);
	GetSprite()->End();

	// Fills screen with red which represents damage received by player
	GetEngine()->DrawColourTint(D3DXCOLOR(0.4,0,0,(100-mMarcus.node.mLife)/100));

	DrawTransitionFade();	// fade, just add it before the EndScene
	GetDevice()->EndScene();
	// note: no need to call Present, that is done in the CSceneEngine
}
void GameScene::InitParticles()
{
	SParticleSetting settings;
	settings.Size = 0.5f;
	settings.LifeTime = 1; // 2secs
	settings.StartColor=D3DCOLOR_XRGB(252,77,18);	// start bright
	settings.EndColor=WHITE_COL;

	// additive blend
	settings.SourceBlend=D3DBLEND_SRCALPHA;
	settings.DestBlend = D3DBLEND_INVSRCALPHA;
	settings.MaxParticles=500;
	mpFire->Init(GetDevice(),"media/Particles/fire.png",settings);

	settings.StartColor = D3DCOLOR_XRGB(0,0,255);
	mpIce->Init(GetDevice(),"media/Particles/smoke.png",settings);

	settings.Size = 0.05f;
	settings.LifeTime = 0.3f;
	mpWandIce->Init(GetDevice(), "media/Particles/smoke.png", settings);

	settings.Size = 0.05f;
	settings.LifeTime = 0.7f;
	mpCurrSkillParticles->Init(GetDevice(), "media/Particles/smoke.png", settings);

	mpIceCollide->Init(GetDevice(), "media/Particles/flare.bmp");

	mpSnow->Init(GetDevice(), "media/Particles/snowball.bmp", D3DXVECTOR3(CParticleSystem::GetRandomFloat(0.7f,1.0f), -3, 0));
}

void GameScene::UpdateParticles(float dt, vector<CMeshNode*>& mpMagicBall, vector<CMeshNode*>& mpIcicles, vector<CMeshNode*>& mpFireBall)
{
	D3DXVECTOR3 dir = mMarcus.node.RotateVector(D3DXVECTOR3(0,0,1));

	if(MB_COOLDOWN)
		mpCurrSkillParticles->GetSettings().Size += 0.005f;
	else if(ICE_COOLDOWN)
		mpCurrSkillParticles->GetSettings().Size += 0.0005f;
	else
	{
		mpCurrSkillParticles->GetSettings().Size = 0.05f;
		mpCurrSkillParticles->GetSettings().StartColor = D3DCOLOR_XRGB(0,0,255);
	}

	for(int i = 0; i<(int)(50*dt); i++)
	{
		D3DXVECTOR3 vel = CParticleSystem::VaryDirection(D3DXVECTOR3(0,0.1f,0), D2R(10));
		vel *= CParticleSystem::GetRandomFloat(0.2f,1.0f);
		D3DXVECTOR3 pos = mCurrSkill.GetPos();
		mpCurrSkillParticles->AddParticle(pos, vel);
	}
	mpCurrSkillParticles->Update(dt);

	if(icicle_charge > 0)
	{
		mpWandIce->GetSettings().Size += 0.0001f;
		mpWandIce->GetSettings().LifeTime += 0.02f;
	}
	else if(icicle_charge == 0)
	{
		mpWandIce->GetSettings().Size = 0.05;
		mpWandIce->GetSettings().LifeTime = 0.3f;
	}

	for(int k = 0; k<(int)(icicle_charge+(50*dt)); k++)
	{
		D3DXVECTOR3 vel=CParticleSystem::VaryDirection(D3DXVECTOR3(0,0.1f,0),D2R(20));
		vel*=CParticleSystem::GetRandomFloat(0.2f,1.0f);
		D3DXVECTOR3 pos = mWand.OffsetPos(D3DXVECTOR3(-0.025f,0.095f,0.04f));
		mpWandIce->AddParticle(pos, vel);
	}
	mpWandIce->Update(dt);

	for(int i = 0; i<mpMagicBall.size(); i++)
	{
		for(int k = 0; k<(int)(100*dt); k++)
		{
			D3DXVECTOR3 vel = CParticleSystem::VaryDirection(-dir, D2R(20));
			vel *= SHOT_VEL * 0.2f;
			mpIce->AddParticle(mpMagicBall[i]->GetPos(), vel);
		}
	}

	for(int i = 0; i<mpIcicles.size(); i++)
	{
		for(int k = 0; k<(int)(200*dt); k++)
		{
			D3DXVECTOR3 vel = CParticleSystem::VaryDirection(-dir, D2R(20));
			vel *= SHOT_VEL * 0.2f;
			mpIce->AddParticle(mpIcicles[i]->GetPos(), vel);
		}
	}

	D3DXVECTOR3 boss_dir = mJin.RotateVector(D3DXVECTOR3(0,0,1));

	//for jin's attack
	for(int i = 0; i<mpFireBall.size(); i++)
	{
		for(int k = 0; k<(int)(100*dt); k++)
		{
			D3DXVECTOR3 vel = CParticleSystem::VaryDirection(-boss_dir, D2R(20));
			vel *= SHOT_VEL * 0.2f;
			mpFire->AddParticle(mpFireBall[i]->GetPos(), vel);
		}
	}

	if(mJin.IsAlive())
	{
		//for jin's wand
		for(int i = 0; i<(int)(35*dt); i++)
		{
			D3DXVECTOR3 vel=CParticleSystem::VaryDirection(D3DXVECTOR3(0,0.1f,0),D2R(20));
			vel*=CParticleSystem::GetRandomFloat(0.2f,1.0f);
			D3DXVECTOR3 pos = mJin.OffsetPos(D3DXVECTOR3(0.18f,0.3f,0.35f));
			mpFire->AddParticle(pos, vel);
		}
	}


	mpFire->Update(dt);
	mpIce->Update(dt);
	mpIceCollide->Update(dt);
	mpSnow->SetCentre(mCamera.GetPos());
	mpSnow->Update(dt);
}
void GameScene::DrawParticles()
{
	mpFire->Draw(IDENTITY_MAT);
	mpIce->Draw(IDENTITY_MAT);
	mpIceCollide->Draw(IDENTITY_MAT);
	mpSnow->Draw(IDENTITY_MAT);
	mpWandIce->Draw(IDENTITY_MAT);
	mpCurrSkillParticles->Draw(IDENTITY_MAT);
}
void GameScene::Leave()
{
	SAFE_DELETE(mpEnemySMesh);
	SAFE_DELETE(mpTerrain);
	SAFE_DELETE(mpMarcusMesh);
	SAFE_DELETE(mpClaraMesh);
	SAFE_DELETE(mpMarkMesh);
	SAFE_DELETE(mpFireballMesh);
	SAFE_DELETE(mpHandMesh);
	SAFE_DELETE(mpFire);
	SAFE_DELETE(mpIce);
	SAFE_DELETE(mpIcicleMesh);
	SAFE_DELETE(mpMagicballMesh);
	SAFE_DELETE(mpEnemyWMesh);
	SAFE_DELETE(mpWandMesh);
	SAFE_DELETE(mpIceCollide);
	SAFE_DELETE(mpTreeMesh);
	SAFE_DELETE(mpSkyBoxMesh);
	SAFE_DELETE(mpHutMesh);
	SAFE_DELETE(mpCurrSkillParticles);
	SAFE_DELETE(mpSnow);
	SAFE_DELETE(mpWandIce);
	SAFE_DELETE(mpJinMesh);

	DeleteMeshNodes(mFireball);
	DeleteMeshNodes(mMagicball);
	DeleteMeshNodes(mIcicles);

	DeleteEnemy(mEnemies);

	for( int i = (int)vec_pCue.size()-1; i>=0; i--)
	{
		vec_pCue[i] = NULL;
		delete vec_pCue[i];
	}
	vec_pCue.clear();
	mpSound->StopCategory("Default");
	mpSound->PlayCue("menu_01");
}
void GameScene::UpdateBackgroundMusic()
{
	//check if player in town
	if((mMarcus.node.GetPos().x > 155 && mMarcus.node.GetPos().x < 235) &&
		(mMarcus.node.GetPos().z > -196 && mMarcus.node.GetPos().z <225))
		inTown = true;
	else
		inTown = false;

	if(inTown)
	{
		if(!mpSound->IsCuePlaying("town"))
		{
			mpSound->StopCategory("Music");
			mpSound->PlayCue("town");
		}
		return;
	}

	else if(!inTown && notAttacking == mEnemies.size() && !mpSound->IsCuePlaying("quiet") && !fightingBoss)
	{
		mpSound->StopCategory("Music");
		mpSound->PlayCue("quiet");
		return;
	}

	// check if all enemies are not attacking and chasing
	notAttacking = 0;
	for(int i = 0; i<mEnemies.size(); i++)
	{
		if(!mEnemies[i]->IsAttacking())
			notAttacking += 1;
	}

	// check if enemies are engaging the player
	for(int i = 0 ; i<mEnemies.size(); i++)
	{
		//battle *= mEnemies[i]->GetState();
		if((mEnemies[i]->IsAttacking() || mJin.GetState() == 0 || mJin.GetState() == 1) &&
			!mpSound->IsCuePlaying("battle_02") && !defeatedJin)
		{
			fightingBoss = true;
			mpSound->StopCategory("Music");
			mpSound->PlayCue("battle_02");
			return;
		}
		else if((mJin.GetState() != 0 && mJin.GetState() != 1) || defeatedJin)
			fightingBoss = false;

	}


	//check if the player is outside town
	//check if the player is inside town
	//check if the player is in combat with boss
}
void GameScene::FillListener(CNode& node, X3DAUDIO_LISTENER& listener)
{
	//ZeroMemory(&listener, sizeof(listener));
	listener.Position = node.GetPos();
	listener.OrientFront = node.RotateVector(D3DXVECTOR3(0,0,1));
	listener.OrientTop = node.RotateVector(D3DXVECTOR3(0,1,0));
	listener.Velocity = D3DXVECTOR3(0,0,0);
}
void GameScene::FillEmitter(vector<X3DAUDIO_EMITTER>& emitter)
{
	for(int i = 0 ;i<emitter.size();i++)
	{
		emitter[i].OrientFront = D3DXVECTOR3(0,0,1);
		emitter[i].OrientTop = D3DXVECTOR3(0,1,0);
	}
}

/* Quest Screen */
void QuestScene::Enter()
{
	mpJoy=GetEngine()->FindComponent<CJoystickComponent>();
	mpSound=GetEngine()->FindComponent<CSoundComponent>();
	mpSound->StopCategory("Voice");
	mpSound->PlayCue("npc_greetings");

	// load the media
	mpQuestTex[0]=LoadSpriteTex(GetDevice(),"media/Textures/questMain.png");
	mpQuestTex[1]=LoadSpriteTex(GetDevice(),"media/Textures/questAdvice.png");
	if (questCompleted)
		mpQuestTex[2]=LoadSpriteTex(GetDevice(),"media/Textures/questFinish.png");
	else
		mpQuestTex[2]=LoadSpriteTex(GetDevice(),"media/Textures/questInfo.png");
	// Advise
	mpButtonTex[0]=LoadSpriteTex(GetDevice(),"media/Textures/adviceBtn.png");
	// Quest
	mpButtonTex[1]=LoadSpriteTex(GetDevice(),"media/Textures/questBtn.png");
	// Shop
	mpButtonTex[2]=LoadSpriteTex(GetDevice(),"media/Textures/shopBtn.png");
	// Leave
	mpButtonTex[3]=LoadSpriteTex(GetDevice(),"media/Textures/leaveBtn.png");
	// Accept
	mpButtonTex[4]=LoadSpriteTex(GetDevice(),"media/Textures/acceptBtn.png");
	// Cancel
	mpButtonTex[5]=LoadSpriteTex(GetDevice(),"media/Textures/cancelBtn.png");

	mpCursorTex=LoadSpriteTex(GetDevice(),"media/Textures/cursor.png");

	inAdvice = false;
	inQuest = false;

	btnSelected = 0;

	CGameWindow::ClearKeyPress();
}
void QuestScene::Update(float dt)
{
	// MUST CALL BASE CLASS'S Update
	CScene::Update(dt);	

	// Pause if I'm not the topmost screen
	if (IsTopMost()==false)	return;	

	POINT mouse=CGameWindow::GetMousePos();

	if (mpJoy->IsJoyActive(0))
	{
		useMouse = false;

		DIJOYSTATE state=mpJoy->GetJoystick(0).GetState();
		DIJOYSTATE prevState=mpJoy->GetJoystick(0).GetPrevState();

		/****************************
		*	0		↑	4500	↗	*
		*	9000	→	13500	↘	*
		*	18000	↓	22500	↙	*
		*	27000	←	31500	↖	*
		****************************/

		int sensitive = 750;

		if (!inQuest && !inAdvice)
		{
			if (state.lX <= -sensitive && prevState.lX > -sensitive ||
				state.rgdwPOV[0] == 27000 && prevState.rgdwPOV[0] != 27000)
			{
				mpSound->PlayCue("UI_Clicks");
				if (btnSelected == 2)
					btnSelected = 0;
				else if (btnSelected == 3)
					btnSelected = 1;
			}
			else if (state.lX >= sensitive && prevState.lX < sensitive || 
				state.rgdwPOV[0] == 9000 && prevState.rgdwPOV[0] != 9000)
			{
				mpSound->PlayCue("UI_Clicks");
				if (btnSelected == 0)
					btnSelected = 2;
				else if (btnSelected == 1)
					btnSelected = 3;
			}
			else if (state.lY <= -sensitive && prevState.lY > -sensitive ||
				state.rgdwPOV[0] == 0 && prevState.rgdwPOV[0] != 0)
			{
				mpSound->PlayCue("UI_Clicks");
				if (btnSelected == 1)
					btnSelected = 0;
				else if (btnSelected == 3)
					btnSelected = 2;
			}
			else if (state.lY >= sensitive && prevState.lY < sensitive ||
				state.rgdwPOV[0] == 18000 && prevState.rgdwPOV[0] != 18000)
			{
				mpSound->PlayCue("UI_Clicks");
				if (btnSelected == 0)
					btnSelected = 1;
				else if (btnSelected == 2)
					btnSelected = 3;
			}
		}
		else
		{
			if (state.lX <= -sensitive && prevState.lX > -sensitive ||
				state.rgdwPOV[0] == 27000 && prevState.rgdwPOV[0] != 27000)
			{
				mpSound->PlayCue("UI_Clicks");
				btnSelected--;
			}
			else if (state.lX >= sensitive && prevState.lX < sensitive || 
				state.rgdwPOV[0] == 9000 && prevState.rgdwPOV[0] != 9000)
			{
				mpSound->PlayCue("UI_Clicks");
				btnSelected++;
			}
		}

		if (state.rgbButtons[0] != 0 && prevState.rgbButtons[0] == 0)
		{
			if (inAdvice)
			{
				if(InSprite(mouse,CANCEL_BUT,mpButtonTex[5]))
					PressCancel();
			}
			else if (inQuest)
			{
				if (!questCompleted)
				{
					if (InSprite(mouse,ACCEPT_BUT,mpButtonTex[4]))
						PressAccept();
				}

				if(InSprite(mouse,CANCEL_BUT,mpButtonTex[5]))
					PressCancel();
			}
			else
			{
				if (InSprite(mouse,ADVISE_BUT,mpButtonTex[0]))
					PressAdvise();
				else if(InSprite(mouse,QUEST_BUT,mpButtonTex[1]))
					PressQuest();
				else if(InSprite(mouse,SHOP_BUT,mpButtonTex[2]))
					PressShop();
				else if(InSprite(mouse,LEAVE_BUT,mpButtonTex[3]))
					PressLeave();
			}
		}

		if (inAdvice)
		{
			if (btnSelected != 5)
				btnSelected = 5;
		}
		else if (inQuest)
		{
			if (btnSelected > 5)
				btnSelected = 4;
			else if (btnSelected < 4)
				btnSelected = 5;
		}
		else
		{
			if (btnSelected > 3)
				btnSelected = 0;
			else if (btnSelected < 0)
				btnSelected = 3;
		}

		switch (btnSelected)
		{
		case 0:
			GetEngine()->SetMousePos(ADVISE_BUT.x+75,ADVISE_BUT.y+25);
			break;
		case 1:
			GetEngine()->SetMousePos(QUEST_BUT.x+75,QUEST_BUT.y+25);
			break;
		case 2:
			GetEngine()->SetMousePos(SHOP_BUT.x+75,SHOP_BUT.y+25);
			break;
		case 3:
			GetEngine()->SetMousePos(LEAVE_BUT.x+75,LEAVE_BUT.y+25);
			break;
		case 4:
			GetEngine()->SetMousePos(ACCEPT_BUT.x+75,ACCEPT_BUT.y+25);
			break;
		case 5:
			GetEngine()->SetMousePos(CANCEL_BUT.x+75,CANCEL_BUT.y+25);
			break;
		}
	}
	else
	{
		useMouse = true;

		if (CGameWindow::KeyPress(VK_LBUTTON))
		{
			if (inAdvice)
			{
				if(InSprite(mouse,CANCEL_BUT,mpButtonTex[5]))
					PressCancel();
			}
			else if (inQuest)
			{
				if (!questCompleted)
				{
					if (InSprite(mouse,ACCEPT_BUT,mpButtonTex[4]))
						PressAccept();
				}

				if(InSprite(mouse,CANCEL_BUT,mpButtonTex[5]))
					PressCancel();
			}
			else
			{
				if (InSprite(mouse,ADVISE_BUT,mpButtonTex[0]))
					PressAdvise();
				else if(InSprite(mouse,QUEST_BUT,mpButtonTex[1]))
					PressQuest();
				else if(InSprite(mouse,SHOP_BUT,mpButtonTex[2]))
					PressShop();
				else if(InSprite(mouse,LEAVE_BUT,mpButtonTex[3]))
					PressLeave();
			}
		}
	}
}
void QuestScene::PressAdvise()
{
	inAdvice = true;
	mpSound->StopCategory("Voice");
	mpSound->PlayCue("npc_advice");
}
void QuestScene::PressQuest()
{
	if (!activeQuest && !questCompleted)
	{
		mpSound->StopCategory("Voice");
		mpSound->PlayCue("npc_quest");
		inQuest = true;
	}
	else if (questCompleted)
	{
		activeQuest = false;
		wearArmour = true;
		inQuest = true;
	}
	else
	{
		mpSound->StopCategory("Voice");
		mpSound->PlayCue("npc_quest_yes");
	}
}
void QuestScene::PressShop()
{
	mpSound->StopCategory("Voice");
	mpSound->PlayCue("npc_shop");
}
void QuestScene::PressLeave()
{
	mpSound->StopCategory("Voice");
	mpSound->PlayCue("npc_leave");
	ExitScene();
}
void QuestScene::PressAccept()
{
	mpSound->StopCategory("Voice");
	mpSound->PlayCue("npc_quest_yes");
	activeQuest = true;
	inQuest = false;
	inAdvice = false;
}
void QuestScene::PressCancel()
{
	mpSound->StopCategory("Voice");
	mpSound->PlayCue("npc_quest_no");
	inQuest = false;
	inAdvice = false;
}
void QuestScene::Draw(float dt)
{
	GetDevice()->BeginScene();
	GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);

	POINT mouse = CGameWindow::GetMousePos();

	RECT r=GetEngine()->GetWindowRect();
	D3DCOLOR inactive = WHITE_COL*0.5f;

	if (inAdvice)
	{
		DrawSprite(GetSprite(),mpQuestTex[1], r);

		if (InSprite(mouse,CANCEL_BUT,mpButtonTex[5]))
			DrawSprite(GetSprite(),mpButtonTex[5],CANCEL_BUT);
		else
			DrawSprite(GetSprite(),mpButtonTex[5],CANCEL_BUT,inactive);
	}
	else if (inQuest)
	{
		DrawSprite(GetSprite(),mpQuestTex[2], r);

		if (!questCompleted)
		{
			if (InSprite(mouse,ACCEPT_BUT,mpButtonTex[4]))
				DrawSprite(GetSprite(),mpButtonTex[4],ACCEPT_BUT);
			else
				DrawSprite(GetSprite(),mpButtonTex[4],ACCEPT_BUT,inactive);
		}

		if (InSprite(mouse,CANCEL_BUT,mpButtonTex[5]))
			DrawSprite(GetSprite(),mpButtonTex[5],CANCEL_BUT);
		else
			DrawSprite(GetSprite(),mpButtonTex[5],CANCEL_BUT,inactive);
	}
	else
	{
		DrawSprite(GetSprite(),mpQuestTex[0], r);

		if (InSprite(mouse,ADVISE_BUT,mpButtonTex[0]))
			DrawSprite(GetSprite(),mpButtonTex[0],ADVISE_BUT);
		else
			DrawSprite(GetSprite(),mpButtonTex[0],ADVISE_BUT,inactive);

		if (InSprite(mouse,QUEST_BUT,mpButtonTex[1]))
			DrawSprite(GetSprite(),mpButtonTex[1],QUEST_BUT);
		else
			DrawSprite(GetSprite(),mpButtonTex[1],QUEST_BUT,inactive);

		if (InSprite(mouse,SHOP_BUT,mpButtonTex[2]))
			DrawSprite(GetSprite(),mpButtonTex[2],SHOP_BUT);
		else
			DrawSprite(GetSprite(),mpButtonTex[2],SHOP_BUT,inactive);

		if (InSprite(mouse,LEAVE_BUT,mpButtonTex[3]))
			DrawSprite(GetSprite(),mpButtonTex[3],LEAVE_BUT);
		else
			DrawSprite(GetSprite(),mpButtonTex[3],LEAVE_BUT,inactive);
	}

	if (useMouse)
		DrawSprite(GetSprite(),mpCursorTex,mouse,WHITE_COL);

	GetSprite()->End();
	GetDevice()->EndScene();
}
void QuestScene::Leave()
{
	for(int i = 0; i < 3; i++)
		SAFE_RELEASE(mpQuestTex[i]);
	for(int i = 0; i < 6; i++)
		SAFE_RELEASE(mpButtonTex[i]);
	SAFE_RELEASE(mpCursorTex);

	CGameWindow::ClearKeyPress();
}

/* Game Over */
void GameOver::Enter()
{
	mTransOnTime=2;		// slow fade in
	mTransOffTime=2;	// slow fade out

	mpJoy=GetEngine()->FindComponent<CJoystickComponent>();
	mpSound = GetEngine()->FindComponent<CSoundComponent>();
	mpSound->StopCategory("Music");
	mpSound->StopCategory("SFX");
	if (defeatedJin)
	{
		mpSound->PlayCue("victory");
		mpGameOverTex=LoadSpriteTex(GetDevice(),"media/Textures/WinScreen.jpg");
	}
	else
	{
		mpSound->PlayCue("lose");
		mpGameOverTex=LoadSpriteTex(GetDevice(),"media/Textures/DeathScreen.jpg");
	}
	mpButtonTex=LoadSpriteTex(GetDevice(),"media/Textures/backBtn.jpg");
	mpCursorTex=LoadSpriteTex(GetDevice(),"media/Textures/cursor.png");

	CGameWindow::ClearKeyPress();
}
void GameOver::Update(float dt)
{
	CScene::Update(dt);	// MUST CALL BASE CLASS'S Update

	// Pause if I'm not the topmost screen
	if (IsTopMost()==false)	return;
	if (GetState()==FADE_OUT)
	{
		mpSound->StopCue("lose");
		mpSound->StopCue("victory");
		return;
	}

	if (mpJoy->IsJoyActive(0))
	{
		useMouse = false;

		GetEngine()->SetMousePos(OVER_BUT.x, OVER_BUT.y);

		DIJOYSTATE state=mpJoy->GetJoystick(0).GetState();
		DIJOYSTATE prevState=mpJoy->GetJoystick(0).GetPrevState();

		if (state.rgbButtons[1] != 0 && prevState.rgbButtons[1] == 0)
			ExitScene();
	}
	else
	{
		useMouse = true;

		if (CGameWindow::KeyPress(VK_LBUTTON))
		{
			POINT mouse=CGameWindow::GetMousePos();
			if (InSprite(mouse,OVER_BUT,mpButtonTex))
				ExitScene();
		}
	}
}
void GameOver::Draw(float dt)
{
	GetDevice()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, BLACK_COL, 1.0f, 0 );
	GetDevice()->BeginScene();
	GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);

	DrawSprite(GetSprite(), mpGameOverTex, GetEngine()->GetWindowRect());

	POINT mouse=CGameWindow::GetMousePos();
	if (InSprite(mouse,OVER_BUT,mpButtonTex))
		DrawSprite(GetSprite(),mpButtonTex,OVER_BUT,WHITE_COL);
	else
		DrawSprite(GetSprite(),mpButtonTex,OVER_BUT,SKY_COL*0.75f);

	if (useMouse)
		DrawSprite(GetSprite(), mpCursorTex, GetEngine()->GetMousePos());

	GetSprite()->End();
	DrawTransitionFade();	// fade, just add it before the EndScene
	GetDevice()->EndScene();
}
void GameOver::Leave()
{
	SAFE_RELEASE(mpGameOverTex);
	SAFE_RELEASE(mpButtonTex);
	SAFE_RELEASE(mpCursorTex);
	CGameWindow::ClearKeyPress();
}

/* Preface Screen */
void PrefaceScene::Enter()
{
	mTransOnTime=2;		// slow fade in
	mTransOffTime=2;	// slow fade out

	mpJoy=GetEngine()->FindComponent<CJoystickComponent>();

	mpPrefaceTex=LoadSpriteTex(GetDevice(),"media/Textures/Prologue.jpg");
	mpFont=CreateD3DFont(GetDevice(),"Arial",24,true);

	CGameWindow::ClearKeyPress();
}
void PrefaceScene::Update(float dt)
{
	CScene::Update(dt);	// MUST CALL BASE CLASS'S Update
	if (IsTopMost()==false)	return;	// Pause if I'm not the topmost screen

	if (mpJoy->IsJoyActive(0))
	{
		DIJOYSTATE state=mpJoy->GetJoystick(0).GetState();
		DIJOYSTATE prevState=mpJoy->GetJoystick(0).GetPrevState();

		if (state.rgbButtons[7] != 0 && prevState.rgbButtons[7] == 0)
		{
			ExitScene();
			GetEngine()->AddScene(new GameScene());
		}
	}
	else
	{
		if (CGameWindow::KeyPress(VK_RETURN))
		{
			ExitScene();
			GetEngine()->AddScene(new GameScene());
		}
	}
}
void PrefaceScene::Draw(float dt)
{
	GetDevice()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, BLACK_COL, 1.0f, 0 );
	GetDevice()->BeginScene();
	GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);

	DrawSprite(GetSprite(), mpPrefaceTex, GetEngine()->GetWindowRect());

	string s = "Press ENTER to continue.";

	if (mpJoy->IsJoyActive(0))
		s = "Press START to continue.";

	DrawD3DFontEx(mpFont,GetSprite(),s.c_str(),D3DXVECTOR2(1000,666),WHITE_COL,D3DXVECTOR2(0,0),1.0f);

	GetSprite()->End();
	DrawTransitionFade();	// fade, just add it before the EndScene
	GetDevice()->EndScene();
}
void PrefaceScene::Leave()
{
	SAFE_RELEASE(mpPrefaceTex);
	SAFE_RELEASE(mpFont);
	CGameWindow::ClearKeyPress();
}

/* Help Screen */
void HelpScene::Enter()
{
	mTransOnTime=2;		// slow fade in
	mTransOffTime=2;	// slow fade out

	mpJoy=GetEngine()->FindComponent<CJoystickComponent>();
	mpSound = GetEngine()->FindComponent<CSoundComponent>();
	mpSound->StopCue("menu_01");
	mpSound->PlayCue("quiet");

	// load the media
	if (mpJoy->IsJoyActive(0))
		mpHelpTex=LoadSpriteTex(GetDevice(),"media/Textures/help2.jpg");
	else
		mpHelpTex=LoadSpriteTex(GetDevice(),"media/Textures/help1.jpg");
	mpButtonTex=LoadSpriteTex(GetDevice(),"media/Textures/backBtn.jpg");
	mpCursorTex = LoadSpriteTex(GetDevice(),"media/Textures/cursor.png");

	CGameWindow::ClearKeyPress();
}
void HelpScene::Update(float dt)
{
	CScene::Update(dt);	// MUST CALL BASE CLASS'S Update

	// Pause if I'm not the topmost screen
	if (IsTopMost()==false)	return;

	// Pause if I'm fading out
	if (GetState()==FADE_OUT)
	{
		mpSound->StopCue("quiet");
		return;
	}

	if (mpJoy->IsJoyActive(0))
	{
		GetEngine()->SetMousePos(BACK_BUT.x, BACK_BUT.y);

		DIJOYSTATE state=mpJoy->GetJoystick(0).GetState();
		DIJOYSTATE prevState=mpJoy->GetJoystick(0).GetPrevState();

		if (state.rgbButtons[1] != 0 && prevState.rgbButtons[1] == 0)
			ExitScene();
	}
	else
	{
		// WARNING: you cannot have two calls to KeyPress for the same key within a function
		// the second call always gives false
		// therefore one KeyPress with many tests within it
		if (CGameWindow::KeyPress(VK_LBUTTON))
		{
			POINT mouse=CGameWindow::GetMousePos();
			if (InSprite(mouse,BACK_BUT,mpButtonTex))	// clicked the button
				ExitScene();
		}

		if (CGameWindow::KeyPress(VK_ESCAPE))
			ExitScene();
	}
}
void HelpScene::Draw(float dt)
{
	GetDevice()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, BLACK_COL, 1.0f, 0 );
	GetDevice()->BeginScene();

	GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);

	DrawSprite(GetSprite(),mpHelpTex,GetEngine()->GetWindowRect(),WHITE_COL);

	// just a normal draw using the spritebatch
	POINT mouse=CGameWindow::GetMousePos();
	if (InSprite(mouse,BACK_BUT,mpButtonTex))
		DrawSprite(GetSprite(),mpButtonTex,BACK_BUT,WHITE_COL);
	else
		DrawSprite(GetSprite(),mpButtonTex,BACK_BUT,SKY_COL*0.75f);

	if (!mpJoy->IsJoyActive(0))
		DrawSprite(GetSprite(), mpCursorTex, GetEngine()->GetMousePos());

	GetSprite()->End();

	DrawTransitionFade();	// fade, just add it before the EndScene
	GetDevice()->EndScene();
	// note: no need to call Present, that is done in the CSceneEngine
}
void HelpScene::Leave()
{
	mpSound->PlayCue("menu_01");
	SAFE_RELEASE(mpHelpTex);
	SAFE_RELEASE(mpButtonTex);
	SAFE_RELEASE(mpCursorTex);

	CGameWindow::ClearKeyPress();
}

void CreditsScene::Enter()
{
	mTransOnTime=2;		// slow fade in
	mTransOffTime=2;	// slow fade out

	mpJoy=GetEngine()->FindComponent<CJoystickComponent>();

	mpSound = GetEngine()->FindComponent<CSoundComponent>();
	mpSound->StopCue("menu_01");
	mpSound->PlayCue("quiet");

	// load the media
	mpCreditsTex=LoadSpriteTex(GetDevice(), "media/Textures/Credits.jpg");
	mpButtonTex=LoadSpriteTex(GetDevice(),"media/Textures/backBtn.jpg");
	mpCursorTex=LoadSpriteTex(GetDevice(),"media/Textures/cursor.png");

	CGameWindow::ClearKeyPress();
}
void CreditsScene::Update(float dt)
{
	CScene::Update(dt);	// MUST CALL BASE CLASS'S Update

	// Pause if I'm not the topmost screen
	if (IsTopMost()==false)	return;

	// Pause if I'm fading out
	if (GetState()==FADE_OUT)
	{
		mpSound->StopCue("quiet");
		return;
	}

	if (mpJoy->IsJoyActive(0))
	{
		GetEngine()->SetMousePos(BACK_BUT.x, BACK_BUT.y);

		DIJOYSTATE state=mpJoy->GetJoystick(0).GetState();
		DIJOYSTATE prevState=mpJoy->GetJoystick(0).GetPrevState();

		if (state.rgbButtons[1] != 0 && prevState.rgbButtons[1] == 0)
			ExitScene();
	}
	else
	{
		// WARNING: you cannot have two calls to KeyPress for the same key within a function
		// the second call always gives false
		// therefore one KeyPress with many tests within it
		if (CGameWindow::KeyPress(VK_LBUTTON))
		{
			POINT mouse=CGameWindow::GetMousePos();
			if (InSprite(mouse,BACK_BUT,mpButtonTex))	// clicked the button
				ExitScene();
		}

		if (CGameWindow::KeyPress(VK_ESCAPE))
			ExitScene();
	}
}
void CreditsScene::Draw(float dt)
{
	GetDevice()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, BLACK_COL, 1.0f, 0 );
	GetDevice()->BeginScene();
	GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);

	DrawSprite(GetSprite(),mpCreditsTex,GetEngine()->GetWindowRect());

	// just a normal draw using the spritebatch
	POINT mouse=CGameWindow::GetMousePos();
	if (InSprite(mouse,BACK_BUT,mpButtonTex))
		DrawSprite(GetSprite(),mpButtonTex,BACK_BUT,WHITE_COL);
	else
		DrawSprite(GetSprite(),mpButtonTex,BACK_BUT,SKY_COL*0.75f);

	if (!mpJoy->IsJoyActive(0))
		DrawSprite(GetSprite(), mpCursorTex, GetEngine()->GetMousePos());

	GetSprite()->End();

	DrawTransitionFade();	// fade, just add it before the EndScene
	GetDevice()->EndScene();
	// note: no need to call Present, that is done in the CSceneEngine
}
void CreditsScene::Leave()
{
	mpSound->PlayCue("menu_01");
	SAFE_RELEASE(mpCreditsTex);
	SAFE_RELEASE(mpButtonTex);
	SAFE_RELEASE(mpCursorTex);

	CGameWindow::ClearKeyPress();
}

/* Menu Screen */
void MenuScene::Enter()
{
	mTransOnTime=2;		// slow fade in
	mTransOffTime=2;	// slow fade out

	mpJoy=GetEngine()->FindComponent<CJoystickComponent>();
	mpSound = GetEngine()->FindComponent<CSoundComponent>();
	mpSound->PlayCue("menu_01");

	mpMenuTex=LoadSpriteTex(GetDevice(),"media/Textures/Menu.jpg");
	mpButtonTex[0] = LoadSpriteTex(GetDevice(),"media/Textures/playBtn.jpg");
	mpButtonTex[1] = LoadSpriteTex(GetDevice(),"media/Textures/helpBtn.jpg");
	mpButtonTex[2] = LoadSpriteTex(GetDevice(),"media/Textures/creditsBtn.jpg");
	mpButtonTex[3] = LoadSpriteTex(GetDevice(),"media/Textures/exitBtn.jpg");
	mpCursorTex = LoadSpriteTex(GetDevice(),"media/Textures/cursor.png");

	btnSelected = 0;
}
void MenuScene::Update(float dt)
{
	CScene::Update(dt);	// MUST CALL BASE CLASS'S Update

	// Pause if I'm not the topmost screen
	if (IsTopMost()==false)	return;	

	// Pause if I'm fading out
	if (GetState()==FADE_OUT)
	{
		mpSound->StopCategory("Music");
		return;
	}

	if (mpJoy->IsJoyActive(0))
	{
		useMouse = false;

		DIJOYSTATE state=mpJoy->GetJoystick(0).GetState();
		DIJOYSTATE prevState=mpJoy->GetJoystick(0).GetPrevState();

		/****************************
		*	0		↑	4500	↗	*
		*	9000	→	13500	↘	*
		*	18000	↓	22500	↙	*
		*	27000	←	31500	↖	*
		****************************/

		int sensitive = 750;
		if (state.lY <= -sensitive && prevState.lY > -sensitive ||
			state.rgdwPOV[0] == 0 && prevState.rgdwPOV[0] != 0)
		{
			mpSound->PlayCue("UI_Clicks");
			btnSelected = 0;
		}
		else if (state.lX <= -sensitive && prevState.lX > -sensitive ||
			state.rgdwPOV[0] == 27000 && prevState.rgdwPOV[0] != 27000)
		{
			mpSound->PlayCue("UI_Clicks");
			btnSelected = 1;
		}
		else if (state.lX >= sensitive && prevState.lX < sensitive || 
			state.rgdwPOV[0] == 9000 && prevState.rgdwPOV[0] != 9000)
		{
			mpSound->PlayCue("UI_Clicks");
			btnSelected = 2;
		}
		else if (state.lY >= sensitive && prevState.lY < sensitive ||
			state.rgdwPOV[0] == 18000 && prevState.rgdwPOV[0] != 18000)
		{
			mpSound->PlayCue("UI_Clicks");
			btnSelected = 3;
		}

		if (state.rgbButtons[0] != 0 && prevState.rgbButtons[0] == 0)
		{
			POINT mouse=CGameWindow::GetMousePos();
			if (InSprite(mouse,PLAY_BUT,mpButtonTex[0]))
				GetEngine()->AddScene(new PrefaceScene());
			else if(InSprite(mouse,HELP_BUT,mpButtonTex[1]))
				GetEngine()->AddScene(new HelpScene());
			else if(InSprite(mouse,CREDITS_BUT,mpButtonTex[2]))
				GetEngine()->AddScene(new CreditsScene());
			else if(InSprite(mouse,EXIT_BUT,mpButtonTex[2]))
				ExitScene();
		}
	}
	else
	{
		// keep checking for controller if disconnected
		mpJoy=GetEngine()->FindComponent<CJoystickComponent>();

		// you will need to detect the mouse clicks
		// using InSprite() from SpriteUtils
		// to go to a new scene you will need to 
		// GetEngine()->AddScene(new TheSceneClass());
		// you will also need to call ExitScene() when you want the menu to close
		if (CGameWindow::KeyPress(VK_LBUTTON) || CGameWindow::KeyPress(VK_RETURN))
		{
			POINT mouse=CGameWindow::GetMousePos();
			if (InSprite(mouse,PLAY_BUT,mpButtonTex[0]))
				GetEngine()->AddScene(new PrefaceScene());
			else if(InSprite(mouse,HELP_BUT,mpButtonTex[1]))
				GetEngine()->AddScene(new HelpScene());
			else if(InSprite(mouse,CREDITS_BUT,mpButtonTex[2]))
				GetEngine()->AddScene(new CreditsScene());
			else if(InSprite(mouse,EXIT_BUT,mpButtonTex[3]))
				ExitScene();
		}

		// enable mouse if mouse moves
		if (GetEngine()->GetMouseDelta().x != 0 || GetEngine()->GetMouseDelta().y != 0)
			useMouse = true;

		if (CGameWindow::KeyPress(VK_UP))
		{
			btnSelected=0;
			mpSound->PlayCue("UI_Clicks");
			useMouse = false;
		}
		else if (CGameWindow::KeyPress(VK_LEFT))
		{
			btnSelected=1;
			mpSound->PlayCue("UI_Clicks");
			useMouse = false;
		}
		else if (CGameWindow::KeyPress(VK_RIGHT))
		{
			btnSelected=2;
			mpSound->PlayCue("UI_Clicks");
			useMouse = false;
		}
		else if (CGameWindow::KeyPress(VK_DOWN))
		{
			btnSelected=3;
			mpSound->PlayCue("UI_Clicks");
			useMouse = false;
		}

		if (CGameWindow::KeyPress(VK_ESCAPE))
		{
			mpSound->PlayCue("UI_Clicks");
			ExitScene();
		}
	}

	if (mpJoy->IsJoyActive(0) || !useMouse)
	{
		switch (btnSelected)
		{
		case 0:
			GetEngine()->SetMousePos(PLAY_BUT.x,PLAY_BUT.y);
			break;
		case 1:
			GetEngine()->SetMousePos(HELP_BUT.x,HELP_BUT.y);
			break;
		case 2:
			GetEngine()->SetMousePos(CREDITS_BUT.x,EXIT_BUT.y);
			break;
		case 3:
			GetEngine()->SetMousePos(EXIT_BUT.x,EXIT_BUT.y);
			break;
		}
	}
}
void MenuScene::Draw(float dt)
{
	GetDevice()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, BLACK_COL, 1.0f, 0 );
	GetDevice()->BeginScene();

	// textures must be within BeginScene()..EndScene
	// and within a SpriteBatches Begin..End
	// we need the D3DXSPRITE_ALPHABLEND to enable transparency
	// you need the background and three buttons
	// at PLAY_BUT,HELP_BUT & EXIT_BUT locations
	GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);

	DrawSprite(GetSprite(), mpMenuTex, GetEngine()->GetWindowRect());

	// mouse over the button
	POINT mouse=CGameWindow::GetMousePos();

	// play button
	if (InSprite(mouse,PLAY_BUT,mpButtonTex[0]))
		DrawSprite(GetSprite(),mpButtonTex[0],PLAY_BUT);
	else
		DrawSprite(GetSprite(),mpButtonTex[0],PLAY_BUT, SKY_COL*0.75f);

	// help button
	if(InSprite(mouse,HELP_BUT,mpButtonTex[1]))
		DrawSprite(GetSprite(),mpButtonTex[1],HELP_BUT);
	else 
		DrawSprite(GetSprite(),mpButtonTex[1],HELP_BUT, SKY_COL*0.75f);

	// help button
	if(InSprite(mouse,CREDITS_BUT,mpButtonTex[2]))
		DrawSprite(GetSprite(),mpButtonTex[2],CREDITS_BUT);
	else 
		DrawSprite(GetSprite(),mpButtonTex[2],CREDITS_BUT, SKY_COL*0.75f);

	// exit button
	if(InSprite(mouse,EXIT_BUT,mpButtonTex[3]))
		DrawSprite(GetSprite(),mpButtonTex[3],EXIT_BUT);
	else
		DrawSprite(GetSprite(),mpButtonTex[3],EXIT_BUT, SKY_COL*0.75f);

	if (useMouse)
		DrawSprite(GetSprite(), mpCursorTex, GetEngine()->GetMousePos());

	GetSprite()->End();
	DrawTransitionFade();// fade, just add it before the EndScene
	GetDevice()->EndScene();
	// note: no need to call Present, that is done in the CSceneEngine
}
void MenuScene::Leave()
{
	SAFE_RELEASE(mpMenuTex);
	for (int i=0; i < 4; i++)
		SAFE_RELEASE(mpButtonTex[i]);
	SAFE_RELEASE(mpCursorTex);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	bool fullscreen = false;
	if (MessageBox(NULL, "Enable Fullscreen?", "Fullscreen", MB_YESNO)==IDYES)	fullscreen = true;
	ShowCursor(false);	// hides cursor
	ENABLE_LEAK_DETECTION();	// Enable run-time memory check for debug builds.
	CSceneEngine eng(1280,720, fullscreen);
	eng.AddComponent(new CJoystickComponent());
	eng.AddComponent(new CSoundComponent("media/Audio/Win/GameSound.xgs", "media/Audio/Win/WaveBank.xwb", "media/Audio/Win/SoundBank.xsb"));
	eng.AddScene(new MenuScene());
	return eng.Go(hInstance);	// runs everything
}