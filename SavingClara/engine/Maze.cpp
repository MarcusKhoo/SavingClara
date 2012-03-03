/*==============================================
* Crate Maze class for GDEV engine
*
* Written by Marcus Khoo
*==============================================*/
#include <fstream>  // file IO
#include <algorithm>  // for reverse
#include "Maze.h"
#include "Fail.h"
#include "GameUtils.h"
#include "ConsoleOutput.h"
using namespace std;

CMaze::CMaze(CXMesh* pBlock, CXMesh* hBlock)
{
	mpBlock=pBlock;
	hotBlock = hBlock;
}

bool CMaze::Init(const char* name)
{
	mMaze.clear();
	// this is standard file reading code using C++
	// if you have not seen this before, you might want to study it
	ifstream in(name);	// the file
	if (!in)	FAIL(name,"Unable to load maze file");
	while(in.eof()==false)	// until the end of file
	{
		string s;
		getline(in,s);	// gets the line
		if (s.length()>1)	// skip empty lines
			mMaze.push_back(s);	// add to the list
	}
	// this chunk of code will reverse the order of the maze, making it look as the screen
	std::reverse(mMaze.begin(),mMaze.end());
	return true;
}

void CMaze::Draw()
{
	for(int z = 0; z < mMaze.size(); z++)
	{
		for(int x = 0; x < mMaze[z].size(); x++)
		{
			if(mMaze[z][x] == '#')
			{
				mpBlock->Draw(D3DXVECTOR3(x,0,z));
				
			}
			else if(mMaze[z][x] == '%')
			{
				hotBlock->Draw(D3DXVECTOR3(x,0,z));
			}
		}
	}
	//		if (....)
	//			mpBlock->Draw(....)
	// PS: display should be on the XZ plane, not XY
	// you should swap the XY around to make it look right
}

char CMaze::GetCell(D3DXVECTOR3 pos)
{
	// Hint: convert the x,z to integer
	// NOTE: this is floating point values, you will need to round nearest number
	// look up the functi	on floor() as this will help you
	// check if its outside the size of the array & return '\0'
	// otherwise return the data
	// remember the XY are reverse
	int x, z;

	x = floor(pos.x+0.5);
	z = floor(pos.z+0.5);

	//if(x > mMaze.
	if(z >= mMaze.size() || x >= mMaze[0].size() || z < 0 || x < 0)
	{
		return '\0';
	}
	else
	{
		return mMaze[z][x];
	}

}
bool CMaze::IsClear(D3DXVECTOR3 pos)
{
	// this is easy, just call GetCell
	// then see if its clear
	if(GetCell(pos) == '.')
	{
		return true;
	}
	else
	{
		return false;
	}

}

bool CMaze::IsClear(D3DXVECTOR3 pos,float radius)
{
	if(IsClear(pos) && IsClear(D3DXVECTOR3(pos.x + radius,pos.y,pos.z))  &&
		IsClear(D3DXVECTOR3(pos.x - radius,pos.y,pos.z)) && IsClear(D3DXVECTOR3(pos.x,pos.y,pos.z +radius)) &&
		IsClear(D3DXVECTOR3(pos.x, pos.y,pos.z-radius)) && IsClear(D3DXVECTOR3(pos.x+radius,pos.y,pos.z+radius)) &&
		IsClear(D3DXVECTOR3(pos.x-radius,pos.y,pos.z-radius)) && IsClear(D3DXVECTOR3(pos.x+radius,pos.y,pos.z-radius)) &&
		IsClear(D3DXVECTOR3(pos.x-radius,pos.y,pos.z+radius)))
		return true;
	else
		return false;
}

bool CMaze::IsTouchingHot(D3DXVECTOR3 pos,float radius)
{
	if (GetCell(pos)=='%')	return true;
	if (GetCell(pos+D3DXVECTOR3(radius,0,0))=='%')	return true;
	if (GetCell(pos+D3DXVECTOR3(0,0,radius))=='%')	return true;
	if (GetCell(pos+D3DXVECTOR3(-radius,0,0))=='%')	return true;
	if (GetCell(pos+D3DXVECTOR3(0,0,-radius))=='%')	return true;

	return false;
}

D3DXVECTOR3 CMaze::WallSlide(D3DXVECTOR3 oldPos,D3DXVECTOR3 newPos,float radius)
{

	if(IsClear(newPos, radius))
	{
		return newPos;
	}
	else
	{
		if(IsClear(D3DXVECTOR3(newPos.x, 0, oldPos.z), radius))
		{
			oldPos.x = newPos.x;
		}
		else if(IsClear(D3DXVECTOR3(oldPos.x, 0, newPos.z), radius))
		{
			oldPos.z = newPos.z;
		}
		return oldPos;
	}


}