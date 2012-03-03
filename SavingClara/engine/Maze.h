/*==============================================
 * Crate Maze class for GDEV engine
 *
 * Written by <YOUR NAME HERE>
 *==============================================*/
#pragma once

#include <vector>
#include <string>
#include <d3dx9math.h>	// D3DXVECTOR3
#include "XMesh.h"	// the mesh class

/** The CMaze class provides a simple 2D maze and basic collision detection.
*/
class CMaze
{
public:
    /** Constructor.
    This code should store the block model, but not much else.
	If you want lots of models, add it here
    */
	CMaze(CXMesh* pBlock, CXMesh* hBlock);
    /** init function,loads the maze.
	If you wish, you may call init a second time to reset the maze
	*/
	bool Init(const char* name);


    void Draw();  /// draws the maze

	/// gets the cell value (or \0 if off the map)
	char GetCell(D3DXVECTOR3 pos);

    /* returns if a given 3D point is clear
	\param pos the point to check (y value not considered)
	\return if the point is clear
	\note if the point is off the map, the results are undefined
	*/
    bool IsClear(D3DXVECTOR3 pos);
	/** returns if an area is clear.
	It might be a circle or square testing area
	\param pos the point to check (y value not considered)
	\param radius object radius
	\return if the point is clear
	\note if the point is off the map, the results are undefined
	*/
	bool IsClear(D3DXVECTOR3 pos,float radius);

	bool IsTouchingHot(D3DXVECTOR3 pos,float radius);
	/** returns a position which will slide along the walls
	\param oldPos a position for which IsClear() is true
	\param newPos the desired positon fo move to (may or may not pass IsClear)
	\param radius the radius parameter for IsClear()
	\returns a point for which IsClear will be valid
	*/
	D3DXVECTOR3 WallSlide(D3DXVECTOR3 oldPos,D3DXVECTOR3 newPos,float radius);
private:
	std::vector<std::string> mMaze;	// the data
    CXMesh* mpBlock;
	CXMesh* hotBlock;
};
