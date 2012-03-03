/*==============================================
 * Terrain class for GDEV Engine
 *
 * Author: Frank Luna (C) All Rights Reserved
 * (Subsiquently heavily modified by GDEV team)
 *
 *==============================================*/

#pragma once

#include <string>
#include <vector>
#include <d3dx9.h>


class CTerrain
{
public:
	std::vector<D3DXVECTOR2> treesPosition;
	std::vector<D3DXVECTOR2> hutsPosition;
	/** Creates the terrain object.
	\param pDevice the direct 3d device
	\param heightMapFileName name of the heightmap file 
	\note texture should be a normal texture format (bmp,png,jpg) and a power of 2 size
		the file should be greyscale (it uses the blue channel for its information)
	\param cellSpacing the scaling factor in the XZ direction
	\param heightScale the scaling factor in the Y direction

	After creating the texture, you must call EITHER LoadTexture() or GenerateTexture()
	(but not both).
	*/
	CTerrain(
		IDirect3DDevice9* pDevice,
		const char* heightMapFileName, 
		int cellSpacing,    // space between cells
		float heightScale,
		const char* objectsMapFileName);   

	~CTerrain();///< destructor

	/// \internal loads the file
	bool  ReadObjectsFile(const char* fileName);
	/** Method fills the top surface of a texture procedurally.  Then
	lights the top surface.  Finally, it fills the other mipmap
	surfaces based on the top surface data using D3DXFilterTexture.
	\param directionToLight direction to the light, (0,1,0) gives mid-day light.
	*/
	bool GenerateTexture( const D3DXVECTOR3& directionToLight );
	/// Loads a texture from a file
	bool LoadTexture( const char* fileName );
	/** Draws the terrain.
	\param worldMatrix the world matrix to draw at
	\param drawTriangles whether to overlay a wireframe triangles over it
	*/
	bool Draw(const D3DXMATRIX& worldMatrix, bool drawTriangles=false);

	int GetTerrainWidth(){return mWidth;}	///< returns the scaled size of the terrain
	int GetTerrainDepth(){return mDepth;}	///< returns the scaled size of the terrain
	/// returns the height of the terrain, given the x&z coordinates.
	float GetHeight( float x, float z);
	/// Returns a point on the ground (with a specified offset)
	D3DXVECTOR3 GetPointOnGround(D3DXVECTOR3 pos, float offset=0);
	/// returns if a given point is above the ground
	bool IsPointAboveGround(const D3DXVECTOR3& pos,float offset=0);
	/** Computes the orientation of an object on the ground.
	Its very complex and not 100% tested, but might be useful.
	*/
	D3DXVECTOR3 GetOrientationOnGround(float x,float z,float headingRad,float radius=1);
private:
	IDirect3DDevice9*       mpDevice;
	IDirect3DTexture9*      mpTexture;
	IDirect3DVertexBuffer9* mpVertexBuffer;
	IDirect3DIndexBuffer9*  mpIndexBuffer;

	int mNumVerticesPerRow;
	int mNumVerticesPerCol;
	int mCellSpacing;

	int mNumCellsPerRow;
	int mNumCellsPerCol;
	int mWidth;
	int mDepth;
	int mNumOfVertices;
	int mNumOfTriangles;

	float mHeightScale;

	std::vector<float> mHeightMap;

	/// \internal loads the file
	bool  ReadHeightFile(const char* fileName );
	/// \internal gets the heightmap entry (not real height)
	float  GetHeightMapEntry( int inRow, int inCol);
	/// \internal computes the vertex buffers
	bool  ComputeVertices();
	/// \internal computes the index buffer
	bool  ComputeIndices();
	/// \internal performs the lighting computation on the terrain
	bool  LightTerrain( const D3DXVECTOR3& inDirectionToLight );
	/// \internal helper to compute the lighting factor
	float ComputeShading( int inCellRow, int inCellCol, const D3DXVECTOR3& inDirectionToLight );

	/// the internal vertex buffer format
	struct STerrainVertex
	{
		STerrainVertex(){}
		STerrainVertex( float x, float y, float z, float u, float v )
		{
			_x = x; _y = y; _z = z; _u = u; _v = v;
		}
		float _x, _y, _z;
		float _u, _v;

		static const DWORD FVF;
	};
};
