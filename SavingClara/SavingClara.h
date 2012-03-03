/*==============================================
* Assignment 2
*
* Written by Ong Jin Wen
*			 Marcus Khoo
*			 Clarence Lam
*
*			 DIT/FT/2A/07
*==============================================*/

#pragma once
#include <sstream>

#include "ConsoleOutput.h"
#include "FontUtils.h"
#include "GameUtils.h"
#include "JoystickComponent.h"
#include "MessageBoxScene.h"
#include "Node.h"
#include "ParticleSystem.h"
#include "SpriteUtils.h"
#include "SoundComponent.h"
#include "Terrain.h"
#include "Shot.h"
#include "Enemy.h"
#include "NPC.h"
#include "Boss.h"
#include <ctime>

using namespace std;

bool useMouse = true;

bool wearArmour = false;
bool activeQuest = false;
bool questCompleted = false;
short monstersSlain = 0;

bool defeatedJin;

float bgVolume = 1;
bool MB_COOLDOWN = false;
bool ICE_COOLDOWN = false;
const float SHOT_VEL = 10;
bool fightingBoss = false;
const int FULLY_CHARGED = 70;
const int MAGICBALL_COOLDOWN = 20;
const int ICICLE_COOLDOWN = 200;
const short HEALTH_COOLDOWN = 60;

class GameScene: public CScene
{
	CJoystickComponent* mpJoy;

	// sound
	CSoundComponent* mpSound;
	X3DAUDIO_LISTENER listener;
	vector<X3DAUDIO_EMITTER> vec_emitter;
	X3DAUDIO_EMITTER emitter;
	vector<IXACT3Cue*> vec_pCue;
	IXACT3Cue* pCue;

	// particles
	CParticleSystem* mpFire;
	CParticleSystem* mpIce;
	CParticleSystem* mpWandIce;
	CParticleSystem* mpCurrSkillParticles;
	CExplosion* mpIceCollide;
	CPrecipitation* mpSnow;

	CTerrain* mpTerrain;
	CCameraNode mCamera;

	// Models
	CXMesh* mpMarcusMesh;
	CXMesh* mpMarkMesh;
	CXMesh* mpClaraMesh;
	CXMesh* mpFireballMesh;
	CXMesh* mpMagicballMesh;
	CXMesh* mpIcicleMesh;
	CXMesh* mpEnemyWMesh;
	CXMesh* mpEnemySMesh;
	CXMesh* mpTreeMesh;
	CXMesh* mpWandMesh;
	CXMesh* mpSkyBoxMesh;
	CXMesh* mpHutMesh;
	CXMesh* mpHandMesh;
	CXMesh* mpJinMesh;

	CMeshNode mCurrSkill;
	CMeshNode mWand, mHand;
	vector<CMeshNode*> mMagicball;
	vector<CMeshNode*> mIcicles;
	vector<CMeshNode*> mFireball;
	vector<Enemy*> mEnemies;
	Boss mJin;
	NPC mMark;
	NPC mClara;

	IDirect3DTexture9* mpCrosshairTex;
	ID3DXFont* gameFont;

	// switch skills
	int weaponToggle;
	float icicle_charge;
	float icicleDamage;

	bool inTown;
	int notAttacking;

	clock_t magicballStartTime;
	clock_t icicleStartTime;

	struct Player
	{
		CMeshNode node;
		float lastHitTime;
		float jumpHeight;

		enum STATE {GROUND, JUMP, FALL};
		STATE state;

	} mMarcus;

	void Enter();
	void Update(float dt);
	void UpdatePlayer(float dt);
	void HandleInput(float dt);
	void ConfirmExit();
	void PlayerJump();
	void ChangeSpell(int spell);
	void CastSpell(bool charge);
	void TalkToNPC();
	void Draw(float dt);
	void InitParticles();
	void UpdateParticles(float dt, vector<CMeshNode*>& mpMagicBall, vector<CMeshNode*>& mpIcicles, vector<CMeshNode*>& mpFireBall);
	void UpdateBackgroundMusic();
	void DrawParticles();
	void CheckCollisions(D3DXVECTOR3 oldPos);
	void FillListener(CNode& node, X3DAUDIO_LISTENER& listener);
	void FillEmitter(vector<X3DAUDIO_EMITTER>& emitter);
	void Leave();
};

const POINT ADVISE_BUT={615,420},
			QUEST_BUT={615,470},
			SHOP_BUT={735,420},
			LEAVE_BUT={735,470},
			ACCEPT_BUT={490,495},
			CANCEL_BUT={640,495};

class QuestScene: public CScene
{
	CJoystickComponent* mpJoy;

	IDirect3DTexture9* mpQuestTex[3];
	IDirect3DTexture9* mpButtonTex[6];
	IDirect3DTexture9* mpCursorTex;

	CSoundComponent* mpSound;

	bool inAdvice;
	bool inQuest;

	int btnSelected;

	void Enter();
	void Update(float dt);
	void PressAdvise();
	void PressQuest();
	void PressShop();
	void PressLeave();
	void PressAccept();
	void PressCancel();
	void Draw(float dt);
	void Leave();
};

POINT OVER_BUT={540,550};
class GameOver: public CScene
{
	CJoystickComponent* mpJoy;

	CSoundComponent* mpSound;

	IDirect3DTexture9* mpGameOverTex;
	IDirect3DTexture9* mpButtonTex;
	IDirect3DTexture9* mpCursorTex;

	void Enter();
	void Update(float dt);
	void Draw(float dt);
	void Leave();
};

class PrefaceScene: public CScene
{
	CJoystickComponent* mpJoy;

	IDirect3DTexture9* mpPrefaceTex;
	ID3DXFont* mpFont;

	void Enter();
	void Update(float dt);
	void Draw(float dt);
	void Leave();
};

const POINT BACK_BUT={1010,550};

class HelpScene: public CScene
{
	CJoystickComponent* mpJoy;
	CSoundComponent* mpSound;

	IDirect3DTexture9* mpHelpTex;
	IDirect3DTexture9* mpButtonTex;
	IDirect3DTexture9* mpCursorTex;

	void Enter();
	void Update(float dt);
	void Draw(float dt);
	void Leave();
};

class CreditsScene : public CScene
{
	CJoystickComponent* mpJoy;
	CSoundComponent* mpSound;

	IDirect3DTexture9* mpCreditsTex;
	IDirect3DTexture9* mpButtonTex;
	IDirect3DTexture9* mpCursorTex;

	void Enter();
	void Update(float dt);
	void Draw(float dt);
	void Leave();
};

const POINT PLAY_BUT={540,295},
			HELP_BUT={123,395},
			CREDITS_BUT={939,395},
			EXIT_BUT={540,445};

class MenuScene: public CScene
{
	CJoystickComponent* mpJoy;
	CSoundComponent* mpSound;

	IDirect3DTexture9* mpMenuTex;
	IDirect3DTexture9* mpButtonTex[4];
	IDirect3DTexture9* mpCursorTex;

	int btnSelected;

	void Enter();
	void Update(float dt);
	void Draw(float dt);
	void Leave();
};