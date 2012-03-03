/*==============================================
 * Quick'n'Dirty drawing routines
 *
 * Written by Mark Gossage
 *==============================================*/
#include "QDraw.h"
#include "GameUtils.h"


/// Draws a line between two points.
void QDrawLine(IDirect3DDevice9* pDev,const D3DXVECTOR3& p1,const D3DXVECTOR3& p2,D3DCOLOR col)
{
	pDev->SetTransform(D3DTS_WORLD,&IDENTITY_MAT);	// clear matrix
	pDev->SetTexture(0,NULL);	// no texture
	pDev->SetFVF(D3DFVF_XYZ);
	pDev->SetMaterial(&InitMtrl(BLACK_COL,BLACK_COL,BLACK_COL,col,0));	// set colour
	D3DXVECTOR3 points[2]={p1,p2};
	pDev->DrawPrimitiveUP(D3DPT_LINESTRIP,1,points,sizeof(D3DXVECTOR3));
}

/// Draws a sphere of a given radius at a given place.
/// Very good for bounding spheres
void QDrawSphere(IDirect3DDevice9* pDev,const D3DXVECTOR3& pos,float radius,D3DCOLOR col)
{
	const int STEPS=32;
	D3DXVECTOR3 points[STEPS+1];
	pDev->SetMaterial(&InitMtrl(BLACK_COL,BLACK_COL,BLACK_COL,col,0));	// set colour
	pDev->SetTexture(0,NULL);	// no texture
	pDev->SetFVF(D3DFVF_XYZ);

	// xz loop
	for(int i=0;i<STEPS+1;i++)
	{
		float ang=D3DX_PI*2*i/STEPS;
		points[i].x=sin(ang);
		points[i].y=0;
		points[i].z=cos(ang);
	}
	D3DXMATRIX trans,rot,scale;
	D3DXMatrixTranslation(&trans,pos.x,pos.y,pos.z);
	D3DXMatrixScaling(&scale,radius,radius,radius);
	D3DXMatrixRotationX(&rot,D2R(0));
	pDev->SetTransform(D3DTS_WORLD,&(scale*rot*trans));	
	pDev->DrawPrimitiveUP(D3DPT_LINESTRIP,         //PrimitiveType
							 STEPS,              //PrimitiveCount
							 points,            //pVertexStreamZeroData
							 sizeof(D3DXVECTOR3));   //VertexStreamZeroStride
	D3DXMatrixRotationX(&rot,D2R(90));
	pDev->SetTransform(D3DTS_WORLD,&(scale*rot*trans));	
	pDev->DrawPrimitiveUP(D3DPT_LINESTRIP,         //PrimitiveType
							 STEPS,              //PrimitiveCount
							 points,            //pVertexStreamZeroData
							 sizeof(D3DXVECTOR3));   //VertexStreamZeroStride
	D3DXMatrixRotationZ(&rot,D2R(90));
	pDev->SetTransform(D3DTS_WORLD,&(scale*rot*trans));	
	pDev->DrawPrimitiveUP(D3DPT_LINESTRIP,         //PrimitiveType
							 STEPS,              //PrimitiveCount
							 points,            //pVertexStreamZeroData
							 sizeof(D3DXVECTOR3));   //VertexStreamZeroStride
}

/// Draws an Axis using the current world matrix.
/// \note This is the only function that uses the world matrixes value
void QDrawAxisCurrent(IDirect3DDevice9* pDev,float size)
{
	D3DXVECTOR3 points[2];
	pDev->SetTexture(0,NULL);	// no texture
	pDev->SetFVF(D3DFVF_XYZ);
	pDev->SetMaterial(&InitMtrl(BLACK_COL,BLACK_COL,BLACK_COL,RED_COL,0));	// set colour
	points[0]=D3DXVECTOR3(0,0,0);
	points[1]=D3DXVECTOR3(size,0,0);
	pDev->DrawPrimitiveUP(D3DPT_LINESTRIP,1,points,sizeof(D3DXVECTOR3));
	pDev->SetMaterial(&InitMtrl(BLACK_COL,BLACK_COL,BLACK_COL,GREEN_COL,0));	// set colour
	points[1]=D3DXVECTOR3(0,size,0);
	pDev->DrawPrimitiveUP(D3DPT_LINESTRIP,1,points,sizeof(D3DXVECTOR3));
	pDev->SetMaterial(&InitMtrl(BLACK_COL,BLACK_COL,BLACK_COL,BLUE_COL,0));	// set colour
	points[1]=D3DXVECTOR3(0,0,size);
	pDev->DrawPrimitiveUP(D3DPT_LINESTRIP,1,points,sizeof(D3DXVECTOR3));
}

/// Draws an Axis at the specified point in space.
void QDrawAxis(IDirect3DDevice9* pDev,float size,const D3DXVECTOR3& pos)
{
	pDev->SetTransform(D3DTS_WORLD,&IDENTITY_MAT);	// clear matrix
	pDev->SetTexture(0,NULL);	// no texture
	pDev->SetFVF(D3DFVF_XYZ);
	D3DXVECTOR3 points[2];
	points[0]=pos;
	points[1]=pos+D3DXVECTOR3(size,0,0);
	pDev->SetMaterial(&InitMtrl(BLACK_COL,BLACK_COL,BLACK_COL,RED_COL,0));	// set colour
	pDev->DrawPrimitiveUP(D3DPT_LINESTRIP,1,points,sizeof(D3DXVECTOR3));
	points[1]=pos+D3DXVECTOR3(0,size,0);
	pDev->SetMaterial(&InitMtrl(BLACK_COL,BLACK_COL,BLACK_COL,GREEN_COL,0));	// set colour
	pDev->DrawPrimitiveUP(D3DPT_LINESTRIP,1,points,sizeof(D3DXVECTOR3));
	points[1]=pos+D3DXVECTOR3(0,0,size);
	pDev->SetMaterial(&InitMtrl(BLACK_COL,BLACK_COL,BLACK_COL,BLUE_COL,0));	// set colour
	pDev->DrawPrimitiveUP(D3DPT_LINESTRIP,1,points,sizeof(D3DXVECTOR3));
}

void QDrawBox(IDirect3DDevice9* pDev,const D3DXVECTOR3& pos,float size,D3DCOLOR col)
{
	QDrawBox(pDev,pos,D3DXVECTOR3(size,size,size),col);
}
/// Draws a cube of a given size at a given place.
void QDrawBox(IDirect3DDevice9* pDev,const D3DXVECTOR3& pos,const D3DXVECTOR3& size,D3DCOLOR col)
{
	D3DXVECTOR3 points[]={D3DXVECTOR3(-size.x,-size.y,-size.z),D3DXVECTOR3(-size.x,-size.y,size.z),
						D3DXVECTOR3(-size.x,-size.y,size.z),D3DXVECTOR3(size.x,-size.y,size.z),
						D3DXVECTOR3(size.x,-size.y,size.z),D3DXVECTOR3(size.x,-size.y,-size.z),
						D3DXVECTOR3(size.x,-size.y,-size.z),D3DXVECTOR3(-size.x,-size.y,-size.z),
						D3DXVECTOR3(-size.x,-size.y,-size.z),D3DXVECTOR3(-size.x,size.y,-size.z),
						D3DXVECTOR3(-size.x,-size.y,size.z),D3DXVECTOR3(-size.x,size.y,size.z),
						D3DXVECTOR3(size.x,-size.y,-size.z),D3DXVECTOR3(size.x,size.y,-size.z),
						D3DXVECTOR3(size.x,-size.y,size.z),D3DXVECTOR3(size.x,size.y,size.z),
						D3DXVECTOR3(-size.x,size.y,-size.z),D3DXVECTOR3(-size.x,size.y,size.z),
						D3DXVECTOR3(-size.x,size.y,size.z),D3DXVECTOR3(size.x,size.y,size.z),
						D3DXVECTOR3(size.x,size.y,size.z),D3DXVECTOR3(size.x,size.y,-size.z),
						D3DXVECTOR3(size.x,size.y,-size.z),D3DXVECTOR3(-size.x,size.y,-size.z)};
	pDev->SetMaterial(&InitMtrl(BLACK_COL,BLACK_COL,BLACK_COL,col,0));	// set colour
	pDev->SetTexture(0,NULL);	// no texture
	pDev->SetFVF(D3DFVF_XYZ);
	D3DXMATRIX trans;
	D3DXMatrixTranslation(&trans,pos.x,pos.y,pos.z);
	pDev->SetTransform(D3DTS_WORLD,&trans);	
	pDev->DrawPrimitiveUP(D3DPT_LINELIST,12,points,sizeof(D3DXVECTOR3));
}