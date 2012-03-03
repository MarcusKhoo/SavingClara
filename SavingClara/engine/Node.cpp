/*==============================================
 * GameNodes, MeshNode & CameraNodes for GDEV engine
 *
 * Written by Marcus Khoo
 *==============================================*/

#include "Node.h"
#include "Collision.h"
#include "QDraw.h"
#include "Shot.h"
#include "Fail.h"

CNode::CNode(const D3DXVECTOR3& pos,const D3DXVECTOR3& hpr)
:mPos(pos),mHpr(hpr)
{}

void CNode::Match(const CNode& other)
{
	mPos=other.mPos;
	mHpr=other.mHpr;
}

D3DXVECTOR3 CNode::RotateVector(const D3DXVECTOR3& vec)
{
	// rotate vec by the nodes rotation matrix, then return it
	D3DXMATRIX rot;
	D3DXMatrixRotationYawPitchRoll(&rot, mHpr.x, mHpr.y, mHpr.z);

	D3DXVECTOR3 rvec;	// rotated vec
	D3DXVec3TransformCoord(&rvec, &vec, &rot);
	return rvec;
}

D3DXVECTOR3 CNode::OffsetPos(const D3DXVECTOR3& offset)
{
	return mPos + RotateVector(offset);
}
void CNode::Move(const D3DXVECTOR3& vec)
{
	// use RotateVector to change the mPos
	//mPos += vec;
	mPos += RotateVector(vec);
}

void CNode::MoveGround(const D3DXVECTOR3& delta)
{
	// use RotateVector to change the mPos
	mPos.x += RotateVector(delta).x;
	mPos.z += RotateVector(delta).z;
	// but make sure you don't change Y value
}

void CNode::LookAt(const D3DXVECTOR3& tgt)
{
	SetLookDirection(tgt-mPos);
}

void CNode::LookAt(const CNode& tgt)
{
	SetLookDirection(tgt.mPos-mPos);
}

void CNode::SetLookDirection(const D3DXVECTOR3& dir)
{
	float len=D3DXVec3Length(&dir);
	if (len<0.001)	return;
	float yaw, pitch;
	yaw = atan2(dir.x, dir.z);
	pitch = -asin(dir.y/len);

	mHpr = D3DXVECTOR3(yaw,pitch,0);
	// you will need to use an atan2() and an asin()
	// (look them up in your C++ reference)
}

CMeshNode::CMeshNode(CXMesh* pMesh,const D3DXVECTOR3& pos,
				const D3DXVECTOR3& turn,float scale,int life)
{
	Init(pMesh,pos,turn,scale,life);
}
void CMeshNode::Init(CXMesh* pMesh,const D3DXVECTOR3& pos,
				const D3DXVECTOR3& turn,float scale,int life)
{
	mpMesh=pMesh;
	mScale=scale;
	mLife=life;
	mPos=pos;
	mHpr=turn;
}

void CMeshNode::Damage(float dam){mLife-=dam;}

void CMeshNode::Destroy(){mLife=0;}

float CMeshNode::GetBoundingRadius(){return mpMesh->GetRadius() * mScale;}

void CMeshNode::Draw()
{
	D3DXMATRIX trans, scale, rot, world;
	D3DXMatrixTranslation(&trans, mPos.x, mPos.y, mPos.z);
	D3DXMatrixScaling(&scale, mScale, mScale, mScale);
	D3DXMatrixRotationYawPitchRoll(&rot, mHpr.x, mHpr.y, mHpr.z);
	world = scale * rot * trans;
	mpMesh->Draw(world);
	// setup the matrixes and multiply them together
	// then call the mesh's draw function
}

void CMeshNode::DrawBounds(IDirect3DDevice9* pDev,float factor)
{
	QDrawSphere(pDev,mPos,GetBoundingRadius()*factor);
}


CCameraNode::CCameraNode(const D3DXVECTOR3& pos,const D3DXVECTOR3& turn,
			float fov, float aspect,float nearDist,float farDist)
{
	Init(pos,turn,fov,aspect,nearDist,farDist);
}
void CCameraNode::Init(const D3DXVECTOR3& pos,const D3DXVECTOR3& turn,
			float fov, float aspect,float nearDist,float farDist)
{
	mPos=pos;
	mHpr=turn;
	mFov=fov;
	mAspect=aspect;
	mNear=nearDist;
	mFar=farDist;
}

void CCameraNode::SetProjection(float fov,float aspect,float nearDist,float farDist)
{
	mFov=fov;
	mAspect=aspect;
	mNear=nearDist;
	mFar=farDist;
}

void CCameraNode::SetFov(float fov)
{
	mFov=fov;
}
void CCameraNode::SetAspect(float aspect)
{
	mAspect=aspect;
}
void CCameraNode::SetNearFar(float nearDist,float farDist)
{
	mNear=nearDist;
	mFar=farDist;
}

void CCameraNode::SetMatrixes(LPDIRECT3DDEVICE9 pDev)
{
	// get the camera's view & projection matrixes
	D3DXMATRIX proj,view;
	GetProjectionMatrix(proj);
	GetViewMatrix(view);
	
	// then set them into the device
	pDev->SetTransform(D3DTS_PROJECTION, &proj);
	pDev->SetTransform(D3DTS_VIEW, &view);

}

void CCameraNode::GetProjectionMatrix( D3DXMATRIX& outMatrix )
{
    D3DXMatrixPerspectiveFovLH(&outMatrix, mFov, mAspect,mNear,mFar);
}

void CCameraNode::GetViewMatrix( D3DXMATRIX& outMatrix )
{
	const D3DXVECTOR3 FORWARD(0,0,1), UP(0,1,0), RIGHT(1,0,0);

	D3DXMATRIX rot;
	D3DXMatrixRotationYawPitchRoll(&rot, mHpr.x, mHpr.y, mHpr.z);

	D3DXVECTOR3 fore, up, right;

	D3DXVec3TransformCoord(&fore, &FORWARD, &rot);
	D3DXVec3TransformCoord(&up, &UP, &rot);
	D3DXVec3TransformCoord(&right, &RIGHT, &rot);

	float theX = -D3DXVec3Dot(&right,&mPos);
	float theY = -D3DXVec3Dot(&up, &mPos);
	float theZ = -D3DXVec3Dot(&fore, &mPos);

	outMatrix( 0, 0 ) = right.x;
	outMatrix( 0, 1 ) = up.x;
	outMatrix( 0, 2 ) = fore.x;
	outMatrix( 0, 3 ) = 0.0f;

	outMatrix( 1, 0 ) = right.y;
	outMatrix( 1, 1 ) = up.y;
	outMatrix( 1, 2 ) = fore.y;
	outMatrix( 1, 3 ) = 0.0f;

	outMatrix( 2, 0 ) = right.z;
	outMatrix( 2, 1 ) = up.z;
	outMatrix( 2, 2 ) = fore.z;
	outMatrix( 2, 3 ) = 0.0f;

	outMatrix( 3, 0 ) = theX;
	outMatrix( 3, 1 ) = theY;
	outMatrix( 3, 2 ) = theZ;
	outMatrix( 3, 3 ) = 1.0f;



	// Refer to the lecture notes
	// get the rotation matrix
}

void DrawMeshNodes(const std::vector<CMeshNode*>& nodes)
{
	for(int i=0; i<nodes.size(); i++)
	{
		if(nodes[i]->IsAlive())
		{
			nodes[i]->Draw();
		}
	}
	// for each node, if its alive, draw it
}

void DrawMeshNodeBounds(const std::vector<CMeshNode*>& nodes,IDirect3DDevice9* pDev)
{
	for(int i=0; i<nodes.size(); i++)
	{
		if(nodes[i]->IsAlive())
		{
			nodes[i]->DrawBounds(pDev);
		}
	}
	// for each node, if its alive, draw its bounds
}

void NormalizeRotation(CMeshNode* nodes)
{
	D3DXVECTOR3 v = nodes->GetHpr();
	if(v.x > D3DX_PI)
		v.x -= D3DX_PI * 2;
	if(v.x < -D3DX_PI)
		v.x += D3DX_PI * 2;
	if(v.y > D3DX_PI)
		v.y -= D3DX_PI * 2;
	if(v.y < -D3DX_PI)
		v.y += D3DX_PI * 2;
	if(v.z > D3DX_PI)
		v.z -= D3DX_PI * 2;
	if(v.z < -D3DX_PI)
		v.z += D3DX_PI * 2;
	nodes->SetHpr(v);
}

void UpdateMeshNodes(const std::vector<CMeshNode*>& nodes,float dt)
{
	for(int i=0; i<nodes.size(); i++)
	{
		if(nodes[i]->IsAlive())
		{
			NormalizeRotation(nodes[i]);
			nodes[i]->Update(dt);
		}
	}
	// for each node, if its alive, update it
}

void DeleteDeadMeshNodes(std::vector<CMeshNode*>& vec)
{
	// this is a bit tricky, so here it is in full
	for(int i=(int)vec.size()-1;i>=0;i--)
	{
		if (vec[i]->IsAlive()==false)
		{
			delete vec[i];	// deletes the object
			// removed item at index 'i', by shifting down the items
			vec.erase(vec.begin()+i);
		}
	}
}

void DeleteMeshNodes(std::vector<CMeshNode*>& vec)
{
	for(int i = (int)vec.size()-1; i>=0; i--)
	{
		delete vec[i];
	}

	vec.clear();
	// for each node, delete it
	// clear the vector, just in case
}


bool CollisionMeshNode(CMeshNode* p1, CMeshNode* p2,float factor)
{
	return CollisionSphereSphere(p1->mPos,p1->GetBoundingRadius()*factor,p2->mPos,p2->GetBoundingRadius()*factor);
}

bool CollisionShotMeshNode(CMeshNode* pShot,const D3DXVECTOR3& shotVel, CMeshNode* pTarget,float factor)
{
	return CollisionPointThroughSphere(pShot->mPos,shotVel,pTarget->mPos,(pShot->GetBoundingRadius()+pTarget->GetBoundingRadius())*factor);
}
bool CollisionShotMeshNode(const D3DXVECTOR3& shotPos,const D3DXVECTOR3& shotVel, CMeshNode* pTarget,float factor)
{
	return CollisionPointThroughSphere(shotPos,shotVel,pTarget->mPos,pTarget->GetBoundingRadius()*factor);
}

bool CollisionNodeNode(CNode* p1,CNode* p2,float dist)
{
	return CollisionPointPoint(p1->mPos,p2->mPos,dist);
}

bool CollisionNodePoint(CNode* p1,const D3DXVECTOR3& pos,float dist)
{
	return CollisionPointPoint(p1->mPos,pos,dist);
}