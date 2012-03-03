/*==============================================
 * Terrain class for GDEV Engine
 *
 * Author: Frank Luna (C) All Rights Reserved
 * (Subsiquently heavily modified by GDEV team)
 *
 *==============================================*/
#include "Terrain.h"
#include "Fail.h"
#include <fstream>
#include <cmath>

//
// Colors
//
const D3DXCOLOR      WHITE( D3DCOLOR_XRGB(255, 255, 255) );
const D3DXCOLOR   DARKGRAY(D3DCOLOR_XRGB(200, 200, 200) );
const D3DXCOLOR   LIGHTGRAY(D3DCOLOR_XRGB(225, 225, 225) );
const D3DXCOLOR      BLACK( D3DCOLOR_XRGB(  0,   0,   0) );
const D3DXCOLOR        RED( D3DCOLOR_XRGB(255,   0,   0) );
const D3DXCOLOR      GREEN( D3DCOLOR_XRGB(  0, 255,   0) );
const D3DXCOLOR       BLUE( D3DCOLOR_XRGB(  0,   0, 255) );
const D3DXCOLOR     YELLOW( D3DCOLOR_XRGB(255, 255,   0) );
const D3DXCOLOR       CYAN( D3DCOLOR_XRGB(  0, 255, 255) );
const D3DXCOLOR    MAGENTA( D3DCOLOR_XRGB(255,   0, 255) );

const D3DXCOLOR BEACH_SAND( D3DCOLOR_XRGB(255, 249, 157) );
const D3DXCOLOR DESERT_SAND( D3DCOLOR_XRGB(250, 205, 135) );

const D3DXCOLOR LIGHTGREEN( D3DCOLOR_XRGB( 60, 184, 120) );
const D3DXCOLOR  PUREGREEN( D3DCOLOR_XRGB(  0, 166,  81) );
const D3DXCOLOR  DARKGREEN( D3DCOLOR_XRGB(  0, 114,  54) );


const D3DXCOLOR LIGHT_YELLOW_GREEN( D3DCOLOR_XRGB(124, 197, 118) );
const D3DXCOLOR  PURE_YELLOW_GREEN( D3DCOLOR_XRGB( 57, 181,  74) );
const D3DXCOLOR  DARK_YELLOW_GREEN( D3DCOLOR_XRGB( 25, 123,  48) );

const D3DXCOLOR LIGHTBROWN(D3DCOLOR_XRGB(198, 156, 109));
const D3DXCOLOR DARKBROWN( D3DCOLOR_XRGB(115, 100,  87));

const DWORD CTerrain::STerrainVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;

//
// Interpolation helper function 
//
float Lerp(float a, float b, float t)
{
	return a - (a*t) + (b*t);
}

CTerrain::CTerrain(IDirect3DDevice9* pDevice,const char* heightMapFileName, 
		int cellSpacing,float heightScale, const char* objectsMapFileName)
:	mpDevice(pDevice),
	mCellSpacing(cellSpacing),
	mHeightScale(heightScale),
	mpTexture(NULL)
{

	// compute the vertices
	if( !ReadHeightFile(heightMapFileName) )
	{
		FAIL(heightMapFileName,"Cannot load heightmap");
		return;
	}
	// read trees position
	if( !ReadObjectsFile(objectsMapFileName) )
	{
		FAIL(objectsMapFileName, "Cannot load bmp");
	}
	// compute the vertices
	if( !ComputeVertices() )
	{
		FAIL("ComputeVertices");
		return;
	}

	// compute the indices
	if( !ComputeIndices() )
	{
		FAIL("ComputeIndices");
		return;
	}
}


CTerrain::~CTerrain()
{
	if( mpVertexBuffer )
	{
		mpVertexBuffer->Release( );
		mpVertexBuffer = NULL;
	}

	if( mpIndexBuffer )
	{
		mpIndexBuffer->Release( );
		mpIndexBuffer = NULL;
	}

	if( mpTexture )
	{
		mpTexture->Release( );
		mpTexture = NULL;
	}
}

bool CTerrain::ReadHeightFile(const char* fileName)
{
	LPDIRECT3DTEXTURE9 pTex=0;
	// load
	//TODO("Load the heightmap texture");
	if (FAILED(D3DXCreateTextureFromFileEx(mpDevice,fileName,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DX_DEFAULT,0,D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL,
		&pTex)))
		return false;

	// get size
	D3DSURFACE_DESC desc;
	pTex->GetLevelDesc(0,&desc);
	mNumVerticesPerRow=desc.Width;
	mNumVerticesPerCol=desc.Height;
	// set variables:
	mNumCellsPerRow= mNumVerticesPerRow - 1;
	mNumCellsPerCol= mNumVerticesPerCol - 1;
	mWidth=mNumCellsPerRow * mCellSpacing;
	mDepth=mNumCellsPerCol * mCellSpacing;
	mNumOfVertices=mNumVerticesPerRow * mNumVerticesPerCol;
	mNumOfTriangles=mNumCellsPerRow * mNumCellsPerCol * 2;
	// extract the heightmap information from the texture:
	D3DLOCKED_RECT lockRect;
	pTex->LockRect(0,&lockRect,0,D3DLOCK_READONLY);	// lock it:
	BYTE* pData=(BYTE*)lockRect.pBits;
	for(int h=0;h<mNumVerticesPerCol;h++)
	{
		for(int w=0;w<mNumVerticesPerRow;w++)
		{
			BYTE val=pData[h*lockRect.Pitch+w*4];
			mHeightMap.push_back(val*mHeightScale);
		}
	}
	pTex->UnlockRect(0);	// unlock it once finished
	// dispose of the texture we have all the info:
	pTex->Release();
	return true;
}

bool CTerrain::ReadObjectsFile(const char* fileName )
{
	LPDIRECT3DTEXTURE9 pTex=0;
	// load
	//TODO("Load the heightmap texture");
	if (FAILED(D3DXCreateTextureFromFileEx(mpDevice,fileName,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DX_DEFAULT,0,D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL,
		&pTex)))
		return false;

	// get size
	D3DSURFACE_DESC desc;
	pTex->GetLevelDesc(0,&desc);
	mNumVerticesPerRow=desc.Width;
	mNumVerticesPerCol=desc.Height;
	// set variables:
	mNumCellsPerRow= mNumVerticesPerRow - 1;
	mNumCellsPerCol= mNumVerticesPerCol - 1;
	mWidth=mNumCellsPerRow * mCellSpacing;
	mDepth=mNumCellsPerCol * mCellSpacing;
	mNumOfVertices=mNumVerticesPerRow * mNumVerticesPerCol;
	mNumOfTriangles=mNumCellsPerRow * mNumCellsPerCol * 2;
	// extract the heightmap information from the texture:
	D3DLOCKED_RECT lockRect;
	pTex->LockRect(0,&lockRect,0,D3DLOCK_READONLY);	// lock it:
	BYTE* pData=(BYTE*)lockRect.pBits;
	for(int h=0;h<mNumVerticesPerCol;h++)
	{
		for(int w=0;w<mNumVerticesPerRow;w++)
		{
			BYTE val=pData[h*lockRect.Pitch+w*4];
			mHeightMap.push_back(val*mHeightScale);
			float x=-mWidth/2.0f+w*mCellSpacing;
			float z=mDepth/2.0f-h*mCellSpacing;
			if(val == 255)
				treesPosition.push_back(D3DXVECTOR2(x,z));
			if(val == 100)
				hutsPosition.push_back(D3DXVECTOR2(x,z)); 
		}
	}
	pTex->UnlockRect(0);	// unlock it once finished
	// dispose of the texture we have all the info:
	pTex->Release();
	return true;
}

float CTerrain::GetHeightMapEntry(int inRow, int inCol)
{
    //TODO(CTerrain::GetHeightMapEntry);
	return mHeightMap[inRow*mNumVerticesPerRow+inCol];
}

bool CTerrain::ComputeVertices()
{
	HRESULT hr = 0;

	hr = mpDevice->CreateVertexBuffer(
		mNumOfVertices * sizeof(STerrainVertex),
		D3DUSAGE_WRITEONLY,
		STerrainVertex::FVF,
		D3DPOOL_MANAGED,
		&mpVertexBuffer,
		NULL );

	if(FAILED(hr))
		return false;

	STerrainVertex* v = NULL;
	mpVertexBuffer->Lock( NULL, NULL, (void**)&v, NULL );

    // BAD HACK: fixme
	int index=0;
	for(int i = 0; i<mNumVerticesPerCol; i++)
	{
		for(int j = 0; j<mNumVerticesPerRow; j++)
		{
			// compute the correct index into 
			// the vertex buffer and heightmap
			// based on where we are in the nested loop.
			int index = i * mNumVerticesPerRow + j;
			float x=-mWidth/2.0f+j*mCellSpacing;
			float z=mDepth/2.0f-i*mCellSpacing;

			v[index] = STerrainVertex(
			x, static_cast<float>( mHeightMap[ index ] ), z,
			static_cast<float>( j ) / mNumVerticesPerRow,
			static_cast<float>( i ) / mNumVerticesPerCol);

		}
	}
    // END HACK:

	mpVertexBuffer->Unlock();

	return true;
}

bool CTerrain::ComputeIndices()
{
	HRESULT hr = 0;

	hr = mpDevice->CreateIndexBuffer(
		mNumOfTriangles * 3 * sizeof( WORD ), // 3 indices per triangle
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&mpIndexBuffer,
		NULL );

	if(FAILED(hr))
		return false;

	WORD* theIndices = 0;
	mpIndexBuffer->Lock( NULL, NULL, (void**)&theIndices, NULL );

	// index to start of a group of 6 indices that describe the
	// two triangles that make up a quad
	int theBaseIndex = 0;

	for(int i = 0; i<mNumCellsPerCol; i++)
	{
		for(int j = 0; j<mNumCellsPerRow; j++)
		{
			theIndices[theBaseIndex]     =   i   * mNumVerticesPerRow + j;
			theIndices[theBaseIndex + 1] =   i   * mNumVerticesPerRow + j + 1;
			theIndices[theBaseIndex + 2] = (i+1) * mNumVerticesPerRow + j;

			theIndices[theBaseIndex + 3] = (i+1) * mNumVerticesPerRow + j;
			theIndices[theBaseIndex + 4] =   i   * mNumVerticesPerRow + j + 1;
			theIndices[theBaseIndex + 5] = (i+1) * mNumVerticesPerRow + j + 1;

			// next quad
			theBaseIndex += 6;

		}
	}
	//TODO("CTerrain::ComputeIndices()");
	// loop through and compute the triangles of each quad

    
    
	mpIndexBuffer->Unlock();

	return true;
}

bool CTerrain::LoadTexture( const char* fileName )
{
	if (mpTexture!=NULL)
	{
		FAIL("Texture already created","CTerrain::LoadTexture");
		return false;
	}
	HRESULT hr = 0;

	hr = D3DXCreateTextureFromFile(mpDevice,fileName,&mpTexture );

	if( FAILED(hr) )
		return false;

	return true;
}

bool CTerrain::GenerateTexture( const D3DXVECTOR3& inDirectionToLight )
{
	if (mpTexture!=NULL)
	{
		FAIL("Texture already created","CTerrain::GenerateTexture");
		return false;
	}
	// Method fills the top surface of a texture procedurally.  Then
	// lights the top surface.  Finally, it fills the other mipmap
	// surfaces based on the top surface data using D3DXFilterTexture.

	HRESULT hr = 0;

	// texel for each quad cell
	int theTexWidth  = mNumCellsPerRow;
	int theTexHeight = mNumCellsPerCol;

	// create an empty texture
    //TODO(CTerrain::GenerateTexture);
    // you need to create the texture of size theTexWidth x theTexHeight
	//hr = ???
	hr = D3DXCreateTexture( mpDevice,
			theTexWidth, theTexHeight,
			0, // create a complete mipmap chain
			0, // usage
			D3DFMT_X8R8G8B8,// 32 bit XRGB format
			D3DPOOL_MANAGED, &mpTexture);


	if(FAILED(hr))
		return false;

	D3DSURFACE_DESC theTextureDesc; 
	mpTexture->GetLevelDesc(0 /*level*/, &theTextureDesc);

	// make sure we got the requested format because our code 
	// that fills the texture is hard coded to a 32 bit pixel depth.
	if( theTextureDesc.Format != D3DFMT_X8R8G8B8 )
		return false;
		
	D3DLOCKED_RECT theLockedRect;
	mpTexture->LockRect(0/*lock top surface*/, &theLockedRect, 
		0 /* lock entire tex*/, 0/*flags*/);         

	DWORD* theImageData = (DWORD*)theLockedRect.pBits;
	for(int i = 0; i < theTexHeight; i++)
	{
		for(int j = 0; j < theTexWidth; j++)
		{
			D3DXCOLOR c;

			// get height of upper left vertex of quad.
			float theHeight = (float)GetHeightMapEntry(i, j) / mHeightScale;

			c = D3DXCOLOR( 1.0f, 0.0f, 1.0f, 1.0f );	// PINK!!
            // TODO: I'm sure you can do something better than this
            // have a look at the available colours at the top of the file
            // an choose some.
            // eg.
			if( theHeight > 212.5f )
				c = WHITE;
			else if( theHeight > 130.0f )
				c = DARKBROWN;
			else if(theHeight > 80.0f)
				c = DARKGRAY;
			else if(theHeight > 37.5f)
				c = LIGHTGRAY;
			else
				c = LIGHTGRAY;

			// fill locked data, note we divide the pitch by four because the
			// pitch is given in bytes and there are 4 bytes per DWORD.
			theImageData[i * theLockedRect.Pitch / 4 + j] = (D3DCOLOR)c;
		}
	}

	mpTexture->UnlockRect( NULL );

    // TODO: make a call to LightTerrain() here

	if(!LightTerrain( inDirectionToLight ) )
	{
		FAIL( "LightTerrain()");
		return false;
	}

	
	hr = D3DXFilterTexture(
		mpTexture,
		0, // default palette
		0, // use top level as source level
		D3DX_DEFAULT ); // default filter

	if(FAILED(hr))
	{
		FAIL( "D3DXFilterTexture() ");
		return false;
	}

	return true;
}

bool CTerrain::LightTerrain(const D3DXVECTOR3& inDirectionToLight )
{
	HRESULT hr = 0;

	D3DSURFACE_DESC theTextureDesc; 
	mpTexture->GetLevelDesc(0 /*level*/, &theTextureDesc);

	// make sure we got the requested format because our code that fills the
	// texture is hard coded to a 32 bit pixel depth.
	if( theTextureDesc.Format != D3DFMT_X8R8G8B8 )
		return false;
		
	D3DLOCKED_RECT theLockedRect;
	mpTexture->LockRect(
		0,          // lock top surface level in mipmap chain
		&theLockedRect,// pointer to receive locked data
		0,          // lock entire texture image
		0);         // no lock flags specified

	DWORD* theImageData = (DWORD*)theLockedRect.pBits;
	for( unsigned int i = 0; i < theTextureDesc.Height; ++i )
	{
		for( unsigned int j = 0; j < theTextureDesc.Width; ++j )
		{
			// index into texture, note we use the pitch and divide by 
			// four since the pitch is given in bytes and there are 
			// 4 bytes per DWORD.
			int theIndex = i * theLockedRect.Pitch / 4 + j;

			// get current color of quad
			D3DXCOLOR theCurrentColor( theImageData[ theIndex ] );

			// shade current quad
			theCurrentColor *= ComputeShading(i, j, inDirectionToLight);;

			// save shaded color
			theImageData[theIndex] = (D3DCOLOR) theCurrentColor;
		}
	}

	mpTexture->UnlockRect(0);

	return true;
}

float CTerrain::ComputeShading( int inCellRow, int inCellCol, const D3DXVECTOR3& inDirectionToLight )
{
	// get heights of three vertices on the quad
	float theHeightA = static_cast<float> ( GetHeightMapEntry( inCellRow,   inCellCol ) );
	float theHeightB = static_cast<float> ( GetHeightMapEntry( inCellRow,   inCellCol+1 ) );
	float theHeightC = static_cast<float> ( GetHeightMapEntry( inCellRow+1, inCellCol ) );

	// build two vectors on the quad
	D3DXVECTOR3 u( static_cast<float>(mCellSpacing), theHeightB - theHeightA, 0.0f);
	D3DXVECTOR3 v( 0.0f,         theHeightC - theHeightA, static_cast<float>(-mCellSpacing) );

	// find the normal by taking the cross product of two
	// vectors on the quad.
	D3DXVECTOR3 theNormal;
	D3DXVec3Cross( &theNormal, &u, &v );
	D3DXVec3Normalize( &theNormal, &theNormal );

	float theCosine = D3DXVec3Dot( &theNormal, &inDirectionToLight );

	if( theCosine < 0.0f )
		theCosine = 0.0f;

	return theCosine;
}

float CTerrain::GetHeight( float inX, float inZ )
{
	// Translate on xz-plane by the transformation that takes
	// the terrain START point to the origin.
	inX = ( static_cast<float>( mWidth ) / 2.0f) + inX;
	inZ = ( static_cast<float>( mDepth ) / 2.0f) - inZ;

	// Scale down by the transformation that makes the 
	// cellspacing equal to one.  This is given by 
	// 1 / cellspacing since; cellspacing * 1 / cellspacing = 1.
	inX /= static_cast<float>( mCellSpacing );
	inZ /= static_cast<float>( mCellSpacing );

	// From now on, we will interpret our positive z-axis as
	// going in the 'down' direction, rather than the 'up' direction.
	// This allows to extract the row and column simply by 'flooring'
	// x and z:

	int col = (int)::floorf(inX);
	int row = (int)::floorf(inZ);

	// get the heights of the quad we're in:
	// 
    //  A   B
    //  *---*
    //  | / |
    //  *---*  
    //  C   D

	float A = (float)GetHeightMapEntry( row,   col );
	float B = (float)GetHeightMapEntry( row,   col+1 );
	float C = (float)GetHeightMapEntry( row+1, col );
	float D = (float)GetHeightMapEntry( row+1, col+1 );

	//
	// Find the triangle we are in:
	//

	// Translate by the transformation that takes the upper-left
	// corner of the cell we are in to the origin.  Recall that our 
	// cellspacing was nomalized to 1.  Thus we have a unit square
	// at the origin of our +x -> 'right' and +z -> 'down' system.
	float dx = inX - col;
	float dz = inZ - row;

	// Note the below compuations of u and v are unneccessary, we really
	// only need the height, but we compute the entire vector to emphasis
	// the books discussion.
	float theHeight = 0.0f;
	if(dz < 1.0f - dx)  // upper triangle ABC
	{
		float uy = B - A; // A->B
		float vy = C - A; // A->C

		// Linearly interpolate on each vector.  The height is the vertex
		// height the vectors u and v originate from {A}, plus the heights
		// found by interpolating on each vector u and v.
		theHeight = A + Lerp(0.0f, uy, dx) + Lerp(0.0f, vy, dz);
	}
	else // lower triangle DCB
	{
		float uy = C - D; // D->C
		float vy = B - D; // D->B

		// Linearly interpolate on each vector.  The height is the vertex
		// height the vectors u and v originate from {D}, plus the heights
		// found by interpolating on each vector u and v.
		theHeight = D + Lerp(0.0f, uy, 1.0f - dx) + Lerp(0.0f, vy, 1.0f - dz);
	}

	return theHeight;
}

bool CTerrain::Draw(const D3DXMATRIX& inWorldMatrix, bool inDrawTriangles )
{
	HRESULT hr = 0;

	// Set texture filters.
	//mpDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//mpDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//mpDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	//mpDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	//mpDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	//mpDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	//mpDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	mpDevice->SetTransform( D3DTS_WORLD, &inWorldMatrix );

	mpDevice->SetStreamSource( 0, mpVertexBuffer, 0, sizeof( STerrainVertex ) );
	mpDevice->SetFVF( STerrainVertex::FVF );
	mpDevice->SetIndices( mpIndexBuffer );
	
	mpDevice->SetTexture( 0, mpTexture );

	// turn off lighting since we're lighting it ourselves
	mpDevice->SetRenderState( D3DRS_LIGHTING, false );

	hr = mpDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST,
		0,
		0,
		mNumOfVertices,
		0,
		mNumOfTriangles );

	mpDevice->SetRenderState( D3DRS_LIGHTING, true );

	if( inDrawTriangles )
	{
		mpDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		hr = mpDevice->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			0,
			mNumOfVertices,
			0,
			mNumOfTriangles );

		mpDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	}

	if( FAILED(hr) )
		return false;

	return true;
}

D3DXVECTOR3 CTerrain::GetPointOnGround(D3DXVECTOR3 pos, float offset)
{
	if((pos.x < -GetTerrainWidth()/2))
		pos.x = -GetTerrainWidth()/2;
	else if(pos.x > GetTerrainWidth()/2)
		pos.x = GetTerrainWidth()/2;
	if(pos.z < -GetTerrainDepth()/2)
		pos.z = -GetTerrainDepth()/2;
	else if(pos.z > GetTerrainDepth()/2)
		pos.z = GetTerrainDepth()/2;

    pos.y = GetHeight(pos.x,pos.z) + offset;
	return pos;
}
bool CTerrain::IsPointAboveGround(const D3DXVECTOR3& pos,float offset)
{
	if(pos.y + offset > GetHeight(pos.x+offset,pos.z+offset))
	{
		return true;
	}
	
    //TODO(CTerrain::IsPointAboveGround);  
	return false;
}

D3DXVECTOR3 CTerrain::GetOrientationOnGround(float x,float z,float headingRad,float radius)
{
	D3DXVECTOR3 centre(x,GetHeight(x,z),z);	// this the centre
	float cosHR=cos(headingRad)*radius;
	float sinHR=sin(headingRad)*radius;
	D3DXVECTOR3 front(x+sinHR,0,z+cosHR);	// in front
	front.y=GetHeight(front.x,front.z);
	D3DXVECTOR3 right(x+cosHR,0,z+sinHR);	// to right
	right.y=GetHeight(right.x,right.z);
	D3DXVECTOR3 orient;	//result
	orient.x=headingRad;	// heading unchanged
	orient.y=-atan((front.y-centre.y)/radius);
	orient.z=atan((right.y-centre.y)/radius);
	return orient;
}