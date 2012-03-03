/*==============================================
 * Collision detection code
 *
 * Written by Mark Gossage
 *
 *==============================================*/
#pragma once

/** \file Collision.h Collision detection code.
This provides routines for simple collision (using relative positions), spherical collisions
and axis aligned bounding boxes.

\todo Write some more on this.

\note several of the functions are inlined for speed

\par References
- Just about any 3D games book can give you information of point & sphere collisions
- The DirectX SDK gave info on a lot of the routines used.
- http://toymaker.info/Games/html/collisions.html (the ray-mesh collisions)
*/

// include directx9
#include <d3dx9.h>


/** Returns the length of a vector.
Trivial, but might be useful.
\param vec the vector
\return its length
*/
inline float VectorLength(const D3DXVECTOR3& vec)
{
	return D3DXVec3Length(&vec);
}

/** Returns the Distance between two points.
\param p1,p2 the two points in space
\return distnace between the points
*/
inline float GetDistance(const D3DXVECTOR3& p1,const D3DXVECTOR3& p2)
{
	return D3DXVec3Length(&(p1-p2));
}

/** Returns if two points are within a certain distance of each other.

Its useless to do if (p1==p2), because if they are 0.00001 units apart, they will not
be considered equal. Therefore this code has a certain tolerence.

\param p1,p2 the two points in space
\param tolerence the distance the points need to be within to be considered collided
	should not be <=0.
	How close you want to make it depends a lot of the size of objects you are considering.
\return whether they collide
*/
inline bool CollisionPointPoint(const D3DXVECTOR3& p1,const D3DXVECTOR3& p2,float tolerence)
{
	return (D3DXVec3LengthSq(&(p1-p2))<=tolerence*tolerence);
}


/** Returns true if the point p1 in within the sphere (p2,r2).
\param p1 the point
\param p2 the sphere's centre
\param r2 the sphere's radius
\return whether they collide
*/
inline bool CollisionPointSphere(const D3DXVECTOR3& p1,const D3DXVECTOR3& p2,float r2)
{
	return (D3DXVec3LengthSq(&(p1-p2))<=r2*r2);
}

/** Returns true if the sphere (p1,r1) collides with the sphere (p2,r2).

\param p1 sphere 1's centre
\param r1 sphere 1's radius
\param p2 sphere 2's centre
\param r2 sphere 2's radius
*/
inline bool CollisionSphereSphere(const D3DXVECTOR3& p1,float r1, const D3DXVECTOR3& p2,float r2)
{
	return CollisionPointPoint(p1,p2,r1+r2);
}


/** Returns true if the ray (p1,v1) in collides with the sphere (p2,r2).

\param p1 a point on the ray's path
\param v1 direction of the ray
\note v1 does not need to be normalised (length 1), but must not be (0,0,0)
\param p2 sphere 2's centre
\param r2 sphere 2's radius

\note the ray is an infinite ray which extends forward only.
*/
bool CollisionRaySphere(const D3DXVECTOR3& p1, const D3DXVECTOR3& v1, const D3DXVECTOR3& p2, float r2);

/** Returns true if the point (p1,v1) moved through the sphere (p2,r2).

\param p1 the location of the point
\param v1 the vector that the point is moving this cycle
\note v1 does not need to be normalised (length 1), but must not be (0,0,0)
\param p2 sphere 2's centre
\param r2 sphere 2's radius

This code is useful for testing if fast moving small objects passed through
a bounding sphere.

Eg. if a shot is moving at 10 units per cycle, it could easily fly straight through
a 2 unit bounding sphere without triggering a CollisionPointSphere() function.
But it will trigger a CollisionPointThroughSphere()

\note code should be called before moving the point
\code
// assuming that p1,v1 is the point and p2,r2 the bounding sphere.
// before updating the position see if its going to move through
if (CollisionPointThroughSphere(p1,v1,p2,r2))
	// hit
// now go ahead and move the point
p1 += v1; // move the point by v1
\endcode
*/
bool CollisionPointThroughSphere(const D3DXVECTOR3& p1, const D3DXVECTOR3& v1, const D3DXVECTOR3& p2, float r2);

/** Performs a ray to mesh level collision detection.
\param p1 the position the ray is fired from (in global coordinates)
\param v1 the direction the ray is fired (doesn't need to be normalised, but must not be zero)
\param pMesh the target mesh
\param world the world matrix which is used to transform the mesh
\returns whether a collision took place.
\note this is a computationally expensive operation, so it is advisable to use CollisionRaySphere() first 
	to check whether the ray is even near the object.
*/
bool CollisionRayMesh(const D3DXVECTOR3& p1, const D3DXVECTOR3& v1, const LPD3DXBASEMESH pMesh,const D3DXMATRIX& world);

/** Performs a ray to mesh level collision detection.
\param p1 the position the ray is fired from (in global coordinates)
\param v1 the direction the ray is fired (doesn't need to be normalised, but must not be zero)
\param pMesh the target mesh
\param world the world matrix which is used to transform the mesh
\param [out]hitPos the location on the mesh that the hit occured (in global coordiantes)
\returns whether a collision took place.
\note this is a computationally expensive operation, so it is advisable to use CollisionRaySphere() first 
	to check whether the ray is even near the object.
*/
bool CollisionRayMesh(const D3DXVECTOR3& p1, const D3DXVECTOR3& v1, const LPD3DXBASEMESH pMesh,const D3DXMATRIX& world,D3DXVECTOR3& hitPos);

/** Converts the xy of a mouse location into a 3D vector in world coordinates.
\param mx,my the mouse location (in pixels)
\param screenW,screenH the screen size (in pixels)
\param view,proj the view and world matrixes
\returns a direction vector in the direction of the cursor location in world coordinates
\note this code is rather meaningless if you use the mouse to turn the camera,
	instead you should use the look direction of the camera.
*/
D3DXVECTOR3 MouseToWorldVector(int mx,int my,int screenW,int screenH,const D3DXMATRIX& view,const D3DXMATRIX& proj);


//////////////////////////////////////////////////////////////////////////////////////////////////

/** Axis aligned bounding box.
Not currently used, but might be useful to add it
\par References
- http://toymaker.info/Games/html/collisions.html
*/
class CBBox
{
public:
	CBBox();
	CBBox(D3DXVECTOR3 mn,D3DXVECTOR3 mx);
	CBBox(const D3DXVECTOR3 box[2]);

	/// fills the BBox
	void Set(D3DXVECTOR3 mn,D3DXVECTOR3 mx);
	/// fills the BBox
	void Set(const D3DXVECTOR3 box[2]);

	/// transforms the bounding box
	void Transform(const D3DXMATRIX& mat);

	/// returns true if point is within the box
	bool PointWithin(D3DXVECTOR3 pt);
	/// returns true if the box collides
	bool Collide(const CBBox& box);
public:
	D3DXVECTOR3 mMin,mMax;	///< the limits
};

