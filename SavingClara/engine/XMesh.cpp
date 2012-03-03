/*==============================================
 * Improved X Mesh loader for GDEV Engine
 *
 * First version by Mark Gossage
 *==============================================*/
#pragma once
#include <string>
#include "XMesh.h"
#include "Fail.h"

CXMesh::CXMesh(LPDIRECT3DDEVICE9 pDev,const char * name)
{
	mpDev=pDev;
	mpMesh=0;

	LoadMesh(name);
	TidyMesh();	///< optimisation & repairs
	ComputeCollisionInfo();	///< computes the bounding sphere
}

CXMesh::~CXMesh()
{
	mpMesh->Release();
	mMats.clear();
	mTextures.clear();
}

bool CXMesh::LoadMesh(const char* name)
{
	// buffer to hold materials
	// variable to hold the return code
	HRESULT hr;
	// variable to hold the loaded mesh
	LPD3DXMESH pMeshSysMem;
	// buffer to hold the adjacency data
    // step 1: load the mesh
	LPD3DXBUFFER ppAdjacencyBuffer;
	// buffer to hold materials
	LPD3DXBUFFER pD3DXMtrlBuffer;
	DWORD numberOfMaterials = 0;

	// Load the mesh from the disk
	hr = D3DXLoadMeshFromX (name,
							D3DXMESH_SYSTEMMEM,
							mpDev,
							&ppAdjacencyBuffer,
							&pD3DXMtrlBuffer,
							NULL,
							&numberOfMaterials,
							&mpMesh);

	//HRESULT hr = D3DXLoadMeshFromX(name, D3DXMESH_SYSTEMMEM, mpDev, NULL, &pD3DXMtrlBuffer, NULL, &numMaterials, &mpMesh);

	if(FAILED(hr))
	{
		return false;
	}
    // step 2: resize & fill the materials vector
    //  note: you need to set mMats[i].Ambient=mMats[i].Diffuse;

	D3DXMATERIAL* matMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	mMats.resize(numberOfMaterials); // make the vector the right size
	for(unsigned i = 0; i<numberOfMaterials; i++)
	{
		//Copy the material
		mMats[i] = matMaterials[i].MatD3D;
		//Set the ambient color for the material (D3DX does not do this)
		mMats[i].Ambient = mMats[i].Diffuse;
	}

	mTextures.resize(numberOfMaterials);
	for(unsigned i = 0; i<numberOfMaterials; i++)
	{
		if(matMaterials[i].pTextureFilename == NULL)
		{
			//no texture
			mTextures[i] = NULL;
		}
		else
		{
			//there is texture
			std::string s = "media/Models/";
			s+= matMaterials[i].pTextureFilename;
			IDirect3DTexture9* pTex;
			if (FAILED(D3DXCreateTextureFromFile(mpDev,s.c_str(),&pTex )))
			{
				FAIL(name,"Cannot find the mesh's texture");
			}
			mTextures[i] = pTex;
		}
	}
	ppAdjacencyBuffer->Release();
	pD3DXMtrlBuffer->Release();
	return true;
}

void CXMesh::TidyMesh()	///< optimisation & repairs
{
	if (mpMesh==NULL)	return;	// sanity

	// Does the mesh have a D3DFVF_NORMAL in its vertex format?
	if( !( mpMesh->GetFVF( ) & D3DFVF_NORMAL ) )
	{
		// No, so clone a new mesh and add D3DFVF_NORMAL to its vertex format:
		ID3DXMesh* theTempMesh = NULL;
		mpMesh->CloneMeshFVF( D3DXMESH_MANAGED, 
								mpMesh->GetFVF( ) | D3DFVF_NORMAL, // add normals to FVF here
								mpDev, 
								&theTempMesh );
		// Compute the normals using the mesh; this will insert normals into the mesh.
		D3DXComputeNormals( theTempMesh, NULL );
		// Dispose of old mesh
		mpMesh->Release( );
		// Reassign our pointer to the new generated mesh with normals.
		mpMesh = theTempMesh;
	}
}

void CXMesh::ComputeCollisionInfo()	///< computes the bounding sphere
{
	if (mpMesh==NULL)	return;	// sanity
	D3DXVECTOR3* pVertices=NULL;

	if (FAILED(mpMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pVertices)))
	{
		mRadius=1.0f;	// a guess?
		return;
	}

	mRadius=0;
	D3DXVECTOR3 centre;
	D3DXComputeBoundingSphere(pVertices,	// the vertices
						mpMesh->GetNumVertices(),	// number of them 
						D3DXGetFVFVertexSize(mpMesh->GetFVF()),	// the struct size
						&centre,	// centre
						&mRadius);	// radius
	mpMesh->UnlockVertexBuffer();
}

void CXMesh::Draw()
{
	for(unsigned i = 0; i<mMats.size(); i++)
	{
		//set material
		mpDev->SetMaterial(&mMats[i]);
		mpDev->SetTexture(0, mTextures[i]);
		mpMesh->DrawSubset(i);
	}
}

void CXMesh::Draw(const D3DXMATRIX& mat)	// renders at matrix
{
	// hint: set the matrix, then call Draw();
	mpDev->SetTransform(D3DTS_WORLD, &mat);
	Draw();
}
void CXMesh::Draw(const D3DXVECTOR3& pos,float scale, float yaw)	// at some position
{
	// hint: create the matrix, then call Draw(D3DXMATRIX& mat);
	D3DXMATRIX world, trans, scaling, rot;
	D3DXMatrixTranslation(&trans, pos.x, pos.y,pos.z);
	D3DXMatrixScaling(&scaling,scale,scale,scale);
	D3DXMatrixRotationYawPitchRoll(&rot, yaw,0,0);
	world = scaling * rot * trans;
	Draw(world);
}
