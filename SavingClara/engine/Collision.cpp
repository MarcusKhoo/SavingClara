/*==============================================
 * Collision detection code
 *
 * Written by Mark Gossage
 *
 *==============================================*/

#include "Collision.h"	// header

bool CollisionRaySphere(const D3DXVECTOR3& p1, const D3DXVECTOR3& v1, const D3DXVECTOR3& p2, float r2)
{
	return D3DXSphereBoundProbe(&p2,r2,&p1,&v1)!=FALSE;
}

bool CollisionPointThroughSphere(const D3DXVECTOR3& p1, const D3DXVECTOR3& v1, const D3DXVECTOR3& p2, float r2)
{
	return (D3DXSphereBoundProbe(&p2,r2,&p1,&v1)!=FALSE && 
			D3DXVec3LengthSq(&(p1-p2)) < D3DXVec3LengthSq(&v1));
}


bool CollisionRayMesh(const D3DXVECTOR3& p1, const D3DXVECTOR3& v1, const LPD3DXBASEMESH pMesh,const D3DXMATRIX& world)
{
	// copied DIRECTLY from
	// http://www.toymaker.info/Games/html/picking.html
	// you want to know how it works? Read the site...
 
	// Use inverse of world matrix
	D3DXMATRIX matInverse;
	D3DXMatrixInverse(&matInverse,NULL,&world);

	// Transform ray origin and direction by inv matrix
	D3DXVECTOR3 rayObjOrigin,rayObjDirection;
	D3DXVec3TransformCoord(&rayObjOrigin,&p1,&matInverse);
	D3DXVec3TransformNormal(&rayObjDirection,&v1,&matInverse);
	D3DXVec3Normalize(&rayObjDirection,&rayObjDirection);

	BOOL hasHit=FALSE;
	D3DXIntersect(pMesh, &rayObjOrigin, &rayObjDirection, &hasHit, NULL, NULL, NULL, NULL, NULL, NULL);
	return hasHit!=FALSE;
}

bool CollisionRayMesh(const D3DXVECTOR3& p1, const D3DXVECTOR3& v1, const LPD3DXBASEMESH pMesh,const D3DXMATRIX& world,D3DXVECTOR3& hitPos)
{
	// copied DIRECTLY from
	// http://www.toymaker.info/Games/html/picking.html
	// you want to know how it works? Read the site...

	// Use inverse of world matrix
	D3DXMATRIX matInverse;
	D3DXMatrixInverse(&matInverse,NULL,&world);

	// Transform ray origin and direction by inv matrix
	D3DXVECTOR3 rayObjOrigin,rayObjDirection;
	D3DXVec3TransformCoord(&rayObjOrigin,&p1,&matInverse);
	D3DXVec3TransformNormal(&rayObjDirection,&v1,&matInverse);
	D3DXVec3Normalize(&rayObjDirection,&rayObjDirection);

	BOOL hasHit=FALSE;
	float distanceToCollision;
	D3DXIntersect(pMesh, &rayObjOrigin, &rayObjDirection, &hasHit, NULL, NULL, NULL, &distanceToCollision, NULL, NULL);
	if (hasHit==FALSE)	return false;

	// this seems to be the best estimation of the collision location
	D3DXVECTOR3 vec;
	D3DXVec3Normalize(&vec,&v1);
	hitPos=p1 + (vec*distanceToCollision);
	return true;
}

D3DXVECTOR3 MouseToWorldVector(int mx,int my,int screenW,int screenH,const D3DXMATRIX& view,const D3DXMATRIX& proj)
{
	// copied DIRECTLY from
	// http://www.toymaker.info/Games/html/picking.html
	// you want to know how it works? Read the site...
	D3DXVECTOR3 v;
	v.x =  ( ( ( 2.0f * mx ) / screenW  ) - 1 ) / proj._11;
	v.y = -( ( ( 2.0f * my ) / screenH ) - 1 ) / proj._22;
	v.z =  1.0f;

	D3DXMATRIX m;	// inverse the view matrix
	D3DXMatrixInverse( &m, NULL, &view );

	// Transform the screen space pick ray into 3D space
	return D3DXVECTOR3(v.x*m._11 + v.y*m._21 + v.z*m._31,
						v.x*m._12 + v.y*m._22 + v.z*m._32,
						v.x*m._13 + v.y*m._23 + v.z*m._33);
}



CBBox::CBBox(D3DXVECTOR3 mn,D3DXVECTOR3 mx)
{
	Set(mn,mx);
}

CBBox::CBBox(const D3DXVECTOR3 box[2])
{
	Set(box[0],box[1]);
}

void CBBox::Set(D3DXVECTOR3 mn,D3DXVECTOR3 mx)
{
	mMin=mn;
	mMax=mx;
}

void CBBox::Set(const D3DXVECTOR3 box[2])
{
	mMin=box[0];
	mMax=box[1];
}

void CBBox::Transform(const D3DXMATRIX& mat)
{
	// make a BBox
	D3DXVECTOR3 bbox[]={
		D3DXVECTOR3( mMin.x, mMin.y, mMin.z ), // xyz
		D3DXVECTOR3( mMax.x, mMin.y, mMin.z ), // Xyz
		D3DXVECTOR3( mMin.x, mMax.y, mMin.z ), // xYz
		D3DXVECTOR3( mMax.x, mMax.y, mMin.z ), // XYz
		D3DXVECTOR3( mMin.x, mMin.y, mMax.z ), // xyZ
		D3DXVECTOR3( mMax.x, mMin.y, mMax.z ), // XyZ
		D3DXVECTOR3( mMin.x, mMax.y, mMax.z ), //
		D3DXVECTOR3( mMax.x, mMax.y, mMax.z )  // XYZ
	};
	// transform
	for(int i=0;i<8;i++)
	{
		D3DXVec3TransformCoord( &bbox[i], &bbox[i], &mat );
	}

	// convert to OOBB
	mMin=mMax=bbox[0];
	for (int i=1;i<8;i++)
	{
		if (bbox[i].x < mMin.x) mMin.x=bbox[i].x;
		if (bbox[i].x > mMax.x) mMax.x=bbox[i].x;
		if (bbox[i].y < mMin.y) mMin.y=bbox[i].y;
		if (bbox[i].y > mMax.y) mMax.y=bbox[i].y;
		if (bbox[i].z < mMin.z) mMin.z=bbox[i].z;
		if (bbox[i].z > mMax.z) mMax.z=bbox[i].z;
	}
}

bool CBBox::PointWithin(D3DXVECTOR3 pt)
{
	if( pt.x >= mMin.x && pt.y >= mMin.y && pt.z >= mMin.z &&
		pt.x <= mMax.x && pt.y <= mMax.y && pt.z <= mMax.z )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CBBox::Collide(const CBBox& box)
{
	// http://toymaker.info/Games/html/collisions.html
	// If the max x position of A is less than the min x position of B they do not collide  
	// If the min x position of A is greater than the max x position of B they do not collide  
	// and the same goes for y and z 
	// If none of the checks find that no collision occurred then obviously a collision did occur. 
	// This does make sense - read it slowly :) 
	if (mMax.x<box.mMin.x)	return false;
	if (mMin.x>box.mMax.x)	return false;
	if (mMax.y<box.mMin.y)	return false;
	if (mMin.y>box.mMax.y)	return false;
	if (mMax.z<box.mMin.z)	return false;
	if (mMin.z>box.mMax.z)	return false;
	return true;
}



