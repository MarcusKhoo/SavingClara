/*==============================================
* utility functions for GDEV Engine
*
* Written by <YOUR NAME HERE>
*
*==============================================*/

#include "GameUtils.h"
#include "Fail.h"


void CameraInit(LPDIRECT3DDEVICE9 pd3dDevice,float fov, float aspect,float nearDist,float farDist)
{
	/*TODO(CameraInit: refer to Wendy Jones chapter 5);*/
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, fov, aspect, nearDist, farDist);
	// Apply the matProj matrix to the projection stage of the pipeline
	pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

void CameraLookAt(LPDIRECT3DDEVICE9 pd3dDevice,D3DXVECTOR3 pos,D3DXVECTOR3 tgt,D3DXVECTOR3 up)
{
	//TODO(CameraLookAt: refer to Wendy Jones chapter 5)
	D3DXMATRIX matView;
	D3DXMatrixLookAtLH(&matView, &pos, &tgt, &up);
	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
}

void SetupDefaultD3DOptions(LPDIRECT3DDEVICE9 pDev, bool lights) 
{
	/* TODO: refer to Wendy Jones chapter 5
	enable z buffering
	set sampler state for min,mag & mip to linear (read directX help on SetSamplerState for this)
	setup camera @ 0,1,-5 looking at 0,0,0
	if lights: 
	turn on lighting
	D3DRS_SPECULARENABLE=true
	D3DRS_NORMALIZENORMALS=true
	set ambient to 20,20,20
	add at least one directional light (white colour)
	else:
	turn off lighting
	(+anything else that seems reasonable)
	*/
	pDev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	pDev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pDev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pDev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	CameraInit(pDev);
	CameraLookAt(pDev, D3DXVECTOR3(0,1,-5), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,1,0));

	if(lights)
	{
		pDev->SetRenderState(D3DRS_LIGHTING,true);
		pDev->SetRenderState(D3DRS_SPECULARENABLE, true);
		pDev->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		pDev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(20,20,20));
		pDev->SetLight(0,&InitDirectionalLight(D3DXVECTOR3(-1,-1,0), D3DXCOLOR(D3DCOLOR_XRGB(200,200,200))));
		pDev->LightEnable(0,true);
	}
	else
	{
		pDev->SetRenderState(D3DRS_LIGHTING,false);
	}

}


D3DLIGHT9 InitDirectionalLight(const D3DXVECTOR3& direction, const D3DXCOLOR& colour)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	/* TODO: refer to http://toymaker.info/Games/html/lighting.html
	the light should have:
	* no ambient
	* diffuse 'colour' 
	* white specular
	* should be facing in 'direction'
	you should also return the light*/
	light.Type       = D3DLIGHT_DIRECTIONAL;
	light.Specular = D3DXCOLOR(D3DCOLOR_XRGB(255,255,255));
	light.Ambient = D3DXCOLOR(D3DCOLOR_XRGB(0,0,0));
	light.Diffuse = colour;
	light.Range      = 1000.0f;

	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &direction );

	return light;
}

D3DLIGHT9 InitPointLight(const D3DXVECTOR3& position,const D3DXCOLOR& colour)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	//TODO(InitPointLight);
	/* TODO: refer to http://toymaker.info/Games/html/lighting.html
	the light should have:
	* no ambient
	* diffuse 'colour' 
	* white specular
	* should be at 'position'
	* range set to something large
	* attenuation should be 1,0,0
	you should also return the light*/
	light.Type    = D3DLIGHT_POINT;
	light.Ambient = D3DXCOLOR(D3DCOLOR_XRGB(0,0,0));
	light.Specular = D3DXCOLOR(D3DCOLOR_XRGB(255,255,255));
	light.Diffuse = colour;

	light.Position = position;

	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Range=2000.0f;

	return light;
}

D3DLIGHT9 InitSpotLight(const D3DXVECTOR3& position, const D3DXVECTOR3& direction, const D3DXCOLOR& color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	/* TODO: refer to http://toymaker.info/Games/html/lighting.html
	the light should have:
	* no ambient
	* diffuse 'colour' 
	* white specular
	* should be at 'position' facing in 'direction'
	* range set to something large
	* attenuation should be 1,0,0
	* theta & phi should be 30 degrees & 45 degrees (remember use D2R())
	* falloff should be 1
	you should also return the light*/
	light.Type       = D3DLIGHT_SPOT;
	light.Ambient = D3DXCOLOR(D3DCOLOR_XRGB(0,0,0));
	light.Specular = D3DXCOLOR(D3DCOLOR_XRGB(255,255,255));
	light.Diffuse = color;

	light.Position = position;
	light.Direction = direction;

	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	light.Range=1.0f;
	light.Theta=D2R(30);
	light.Phi=D2R(45);
	light.Falloff=1.0f;


	return light;
}

D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient  = a;
	mtrl.Diffuse  = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power    = p;
	return mtrl;
}

///////////////////////////////////////////////////////////////////////
#include "GameWindow.h"

D3DXVECTOR3 GetKeyboardVector(const int keys[6])
{
	D3DXVECTOR3 result(0,0,0);
	if (keys[0]>0 && CGameWindow::KeyDown(keys[0]))
		result.x--;
	if (keys[1]>0 && CGameWindow::KeyDown(keys[1]))
		result.x++;
	if (keys[2]>0 && CGameWindow::KeyDown(keys[2]))
		result.y--;
	if (keys[3]>0 && CGameWindow::KeyDown(keys[3]))
		result.y++;
	if (keys[4]>0 && CGameWindow::KeyDown(keys[4]))
		result.z--;
	if (keys[5]>0 && CGameWindow::KeyDown(keys[5]))
		result.z++;
	return result;
}

D3DXVECTOR3 GetMouseTurnVector(float sensitive,bool resetMouse)
{
	POINT p=CGameWindow::GetMouseDelta();
	if (resetMouse)	CGameWindow::SetMousePos(300,200);
	D3DXVECTOR3 result(p.x/sensitive,p.y/sensitive,0);	// scale towards -1..1
	if (result.x<-3)	result.x=-3;
	if (result.x>+3)	result.x=+3;
	if (result.y<-3)	result.y=-3;
	if (result.y>+3)	result.y=+3;
	return result;
}

void SetupLinearFog(LPDIRECT3DDEVICE9 pd3dDevice, D3DCOLOR colour, float start,float end, DWORD Mode, float Density, bool UseRange)
{
	float Start = start;     // Linear fog distances.
    float End   = end; 

	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	    // Set the fog color.
    pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, colour);

	 // Set fog parameters.
    if(D3DFOG_LINEAR == Mode)
    {
        pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, Mode);
        pd3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&Start));
        pd3dDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&End));
    }
    else
    {
        pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, Mode);
        pd3dDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&Density));
    }

	// Enable range-based fog if desired (only supported for
    //   vertex fog). For this example, it is assumed that UseRange
    //   is set to a nonzero value only if the driver exposes the 
    //   D3DPRASTERCAPS_FOGRANGE capability.
    // Note: This is slightly more performance intensive
    //   than non-range-based fog.
    if(UseRange)
        pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);


}


void ResetWorldMatrix(LPDIRECT3DDEVICE9 pDev)
{
	pDev->SetTransform(D3DTS_WORLD,&IDENTITY_MAT);
}

//////////////////////////////////////////////////////////////
float randf()
{
	return (float)(rand())/(RAND_MAX-1);
}

float randf(float a,float b)
{
	return ((float)(rand())/(RAND_MAX-1)*(b-a))+a;
}
int randi(int n)
{
	return rand()%n;
}

int randi(int a,int b)
{
	return (rand()%(b-a+1))+a;
}