/*==============================================
 * Improved X Mesh loader for GDEV Engine
 *
 * First version by Mark Gossage
 *==============================================*/
#pragma once
#include <d3d9.h>
#include <d3dx9tex.h>
#include <vector>

/** X Mesh loading & drawing class.
If there was an important class in the engine, this is it.

It was designed to be very easy to use. One line of code to load the mesh,
one line of code (usually) to draw it.
\code
CXMesh* pMesh=new CXMesh(pD3dDevice, "media/tiger.x");	// load
...
pD3dDevice->Begin();
...
// one line of code to draw
pMesh->Draw(D3DXVECTOR3(0,0.7f,0));	// draw it at the specified location
...
pD3dDevice->End();
\endcode

\todo Support for animation 
\todo have a base class for CXMesh & the animation
\todo support for transforming the base mesh

\par References:
- Frank Luna (first version of the code)
- Wendy Jones has reasonable information on the steps for loading a mesh
- Collision information
 - http://www.toymaker.info/Games/html/collisions.html
*/
class CXMesh
{
public:
	/** constructor, loads the model.
	\param pDev pointer to the D3D Device
	\param name the filename of the X file (should be within the media directory)
	If there is a problem loading the mesh or textures, the function will FAIL()
	*/
	CXMesh(LPDIRECT3DDEVICE9 pDev,const char * name);
	~CXMesh();
	/// Draw the mesh using the current world, view & project matrixes.
	void Draw();
	/** Draws the model with the supplied world matrix.
	\param world the world matrix
	\note does not reset the world matrix after drawing
	*/
	void Draw(const D3DXMATRIX& world);	// renders at matrix
	/** Draws the model at the specified location and size.
	\param pos the location to draw at
	\param [optional] scale the size to draw
	\note does not reset the world matrix after drawing
	*/
	void Draw(const D3DXVECTOR3& pos,float scale=1.0f, float rotate = 0.0f);	// at some position
	float GetRadius(){return mRadius;}	///< gets the model size
	ID3DXMesh* GetMesh(){return mpMesh;}	///< accessor for the Mesh
	int GetNumMaterial(){return mMats.size();}	///< accessor for the number of materials
	/// accessor for the Textures.
	/// does not check for valid range of values
	LPDIRECT3DTEXTURE9 GetTexture(int id){return mTextures[id];}
	/// accessor for materials.
	/// does not check for valid range of values
	const D3DMATERIAL9& GetMaterial(int id){return mMats[id];}
private:	// internal fns
	bool LoadMesh(const char* name);	///< does the loading
	void TidyMesh();	///< optimisation & repairs
	void ComputeCollisionInfo();	///< computes the bounding sphere
private:
	LPDIRECT3DDEVICE9 mpDev;	// the device
	ID3DXMesh* mpMesh;	// the mesh
	std::vector<D3DMATERIAL9> mMats;	// array of materials
	std::vector<LPDIRECT3DTEXTURE9>  mTextures;	// array of texture pointers
	float mRadius;	///< radius of bounding sphere
};
