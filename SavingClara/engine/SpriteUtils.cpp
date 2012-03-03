/*==============================================
 * 2D Sprite for directX
 *
 * Written by Mark Gossage
 *==============================================*/
#include <d3dx9tex.h>
#include "SpriteUtils.h"
#include "Fail.h"

const D3DXMATRIX IDENTITY_MAT=*D3DXMatrixIdentity(&D3DXMATRIX());


LPD3DXSPRITE CreateSpriteBatch(LPDIRECT3DDEVICE9 pDev)
{
	LPD3DXSPRITE pSprite=0;
	D3DXCreateSprite(pDev,&pSprite);
	return pSprite;
}

LPDIRECT3DTEXTURE9 LoadSpriteTex(LPDIRECT3DDEVICE9 pDev,const char *filename)
{
	LPDIRECT3DTEXTURE9 pTex=0;
	if (FAILED(D3DXCreateTextureFromFileEx(pDev,filename,D3DX_DEFAULT_NONPOW2,D3DX_DEFAULT_NONPOW2,
								D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,
								D3DX_DEFAULT,D3DX_DEFAULT,
								0,NULL,NULL,&pTex)))
	{
		FAIL(filename,"Cannot load texture");
		return NULL;
	}
	return pTex;
}

LPDIRECT3DTEXTURE9 LoadSpriteTexEx(LPDIRECT3DDEVICE9 pDev,const char *filename,D3DXCOLOR transcolour)
{
	LPDIRECT3DTEXTURE9 pTex=0;
	if (FAILED(D3DXCreateTextureFromFileEx(pDev,filename,D3DX_DEFAULT_NONPOW2,D3DX_DEFAULT_NONPOW2,
								D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,
								D3DX_DEFAULT,D3DX_DEFAULT,
								transcolour,NULL,NULL,&pTex)))
	{
		FAIL(filename,"Cannot load texture");
	}
	return pTex;
}

void GetSpriteSize(LPDIRECT3DTEXTURE9 pTex,int& width,int& height)
{
	D3DSURFACE_DESC desc;
	pTex->GetLevelDesc(0,&desc);
	width=desc.Width;
	height=desc.Height;
}

void DrawSprite(LPD3DXSPRITE pSpriteBat,LPDIRECT3DTEXTURE9 pSpriteTex,
					 int x, int y,D3DCOLOR col)
{
	pSpriteBat->Draw(pSpriteTex,0,0,&D3DXVECTOR3((float)x,(float)y,0),col);
}

void DrawSprite(ID3DXSprite* pSpriteBat,IDirect3DTexture9* pSpriteTex,
					 const POINT& p,D3DCOLOR col)
{
	pSpriteBat->Draw(pSpriteTex,0,0,&D3DXVECTOR3((float)p.x,(float)p.y,0),col);
}

void DrawSprite(LPD3DXSPRITE pSpriteBat,LPDIRECT3DTEXTURE9 pSpriteTex,
					 int x, int y, int w, int h,D3DCOLOR col)
{
	RECT to={x,y,x+w,y+h};
	DrawSprite(pSpriteBat,pSpriteTex,to,col);
}

void DrawSprite(LPD3DXSPRITE pSpriteBat,LPDIRECT3DTEXTURE9 pSpriteTex,
					 const RECT& to,D3DCOLOR col)
{
	int w,h;
	GetSpriteSize(pSpriteTex,w,h);
	float sx=float(to.right-to.left)/w;
	float sy=float(to.bottom-to.top)/h;
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale,sx,sy,1);
	pSpriteBat->SetTransform(&scale);
	pSpriteBat->Draw(pSpriteTex,NULL,NULL,
				&D3DXVECTOR3(to.left/sx,to.top/sy,0),col);
	pSpriteBat->SetTransform(&IDENTITY_MAT);	// reset the matrix
}

void ResetRenderState(IDirect3DDevice9* pDev)
{
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	pDev->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	pDev->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
	pDev->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
}

bool InRect(int x, int y,const RECT& r)
{
	if (x>=r.left && x<=r.right && y>=r.top && y<=r.bottom)
		return true;
	return false;
}

bool InArea(int x, int y,int ax,int ay,int w, int h)
{
	if (x>=ax && x<=ax+w && y>=ay && y<=ay+h)
		return true;
	return false;
}

bool InSprite(const POINT& p,const POINT& s,IDirect3DTexture9* pTex)
{
	int w,h;
	GetSpriteSize(pTex,w,h);
	return InArea(p.x,p.y,s.x,s.y,w,h);
}

bool InSprite(int x,int y,int sx,int sy,LPDIRECT3DTEXTURE9 pTex)
{
	int w,h;
	GetSpriteSize(pTex,w,h);
	return InArea(x,y,sx,sy,w,h);
}

bool IntersectRect(const RECT& r1,const RECT& r2)
{
	RECT t;
	return IntersectRect(&t,&r1,&r2)==TRUE;
}

bool IntersectSprite(int x1,int y1,LPDIRECT3DTEXTURE9 pTex1,
				   int x2,int y2,LPDIRECT3DTEXTURE9 pTex2)
{
	int w,h;
	GetSpriteSize(pTex1,w,h);
	RECT r1={x1,y1,x1+w,y1+h};
	GetSpriteSize(pTex2,w,h);
	RECT r2={x2,y2,x2+w,y2+h};
	return IntersectRect(r1,r2);
}
