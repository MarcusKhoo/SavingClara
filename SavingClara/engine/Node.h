/*==============================================
 * GameNodes, MeshNode & CameraNodes for GDEV engine
 *
 * Written by <YOUR NAME>
 *==============================================*/
#pragma once

#include <vector>
#include <d3dx9.h>
#include "XMesh.h"

/** The CNode class is the basic (position & orientation) class.
It provides basic movement capabilities & little else.
Its derived classes are the main things to use:

However if you write code to manipulate the CNode class,
it will work equally well for Mesh's & Camera's.
This is one of the key reasons for this base class.
*/
class CNode
{
public:	// data is public, though not often used
	D3DXVECTOR3 mPos;	///< position
	D3DXVECTOR3 mOldPos;
	D3DXVECTOR3 mHpr;	///< turning (yaw/heading, pitch, roll in radians)
public:

	CNode(const D3DXVECTOR3& pos=D3DXVECTOR3(0,0,0),
				const D3DXVECTOR3& hpr=D3DXVECTOR3(0,0,0));

	// accessor
	const D3DXVECTOR3& GetPos(){return mPos;}
	const D3DXVECTOR3& GetOldPos(){return mOldPos;}
	void SetPos(const D3DXVECTOR3& pos){mPos=pos;}
	void SetOldPos(const D3DXVECTOR3& oldPos){mOldPos = oldPos;}
	void SetPos(float x, float y, float z){mPos=D3DXVECTOR3(x,y,z);}
	/** orientation: Yaw, Pitch & Roll in Radians.
	Because the angles are stored as 3 euler angles, it is not
	possible to perform airplane (cockpit style) manuvers.
	\par eg. 
	if you roll 90 degrees counter clockwise, then pitch up
	in an aircraft you would be turning left. However in the node's case
	if you roll 90 degrees counter clockwise, then pitch up, you just pitch up.
	*/
	const D3DXVECTOR3& GetHpr(){return mHpr;}
	void SetHpr(const D3DXVECTOR3& hpr){mHpr=hpr;}
	void SetHpr(float h, float p, float r){mHpr=D3DXVECTOR3(h,p,r);}

	/// Sets this nodes position & orientation to match the other
	void Match(const CNode& other);

	/** returns a vector rotated by the Nodes orientation.
	This is very useful for determining directions & similar.
	\param vec the vector to be rotated
	\returns the rotated vector
	EG.
	\code
	CNode node;
	...
	// to determine what is 'forward' direction WRT the node.
	D3DXVECTOR3 forward=node.RotateVector(D3DXVECTOR3(0,0,1));
	// to get a suitable position which is behind the node
	// (OffsetPos would be better)
	D3DXVECTOR3 behindNode=node.RotateVector(D3DXVECTOR3(0,10,-20))+
							node.GetPos();
	\endcode
	\see OffsetPos to get the direction+ the current position
	*/
	D3DXVECTOR3 RotateVector(const D3DXVECTOR3& vec);

	/** returns a position offset from the Node, based upon its orientation.
	This is very useful for getting a positon infront of behind the node.
	\param offset the amount to be offset by
	\returns the offset position
	EG.
	\code
	CNode node;
	...
	// to get a suitable position which is behind the node
	D3DXVECTOR3 behindNode=node.OffsetPos(D3DXVECTOR3(0,10,-20));
	\endcode
	\see RotateVector to get just the direction
	*/
	D3DXVECTOR3 OffsetPos(const D3DXVECTOR3& offset);

	/// \defgroup NodeMove Movement code for CNode,CMeshNode & CCamera
	/// @{
	/** Moves the node by a certain amount in its current direction.
	If you want to have simple movement of the object, without considering orientation.
	You can just use the Position() function.
	\code
	// this code moves the object but does not consider orientation
	D3DXVECTOR3 delta(0,0,0);
	if (KeyDown(VK_LEFT))
		delta.x+=1.0f
	if (KeyDown(VK_RIGHT))
		delta.x-=1.0f
	// etc,etc
	node.SetPos(node.GetPos()+delta*SPEED);
	\endcode
	However the above code does not consider orientation of the node.
	The Move() code does consider orientation.
	\code
	// this code moves the object considering orientation
	D3DXVECTOR3 dir(0,0,0);
	if (KeyDown('A'))
		dir.x-=1.0f
	if (KeyDown('D'))
		dir.x+=1.0f
	// etc,etc
	node.Move(dir*SPEED); // move it accordingly
	\endcode

	\note For the coordinates, it assumes forwards is +Z & right is +X (in DX, not in 3DSMax)
	(so if your models are not facing that way, it might look strange).
	\todo get someone who uses max to find out which way the coordinates look
	*/
	void Move(const D3DXVECTOR3& delta);
	/// Special version of Move() which does not change the Y axis.
	/// \see Move() for full details
	void MoveGround(const D3DXVECTOR3& delta);
	/// Yaws the by the amount of radians, this is turning right
	void Yaw(float amount){mHpr.x+=amount;}
	/// Pitches the by the amount of radians, this is turning down
	void Pitch(float amount){mHpr.y+=amount;}
	/// Rolls the by the amount of radians, this is counter clockwise (left down,right up)
	void Roll(float amount){mHpr.z+=amount;}
	/// Turns (yaw,pitch,roll) the the specfied amount
	void Turn(const D3DXVECTOR3& turn){mHpr+=turn;}

	/// @}
	/** makes the node look at the a point in space.
	This code sets the pitch & yaw of the node, and sets roll=0
	\param tgt the POSITION to look at
	\note will do nothing if the target point is at the nodes location
	(you cannot look at yourself)
	\see SetLookDirection for information on setting the DIRECTION to look
	*/
	void LookAt(const D3DXVECTOR3& tgt);
	/** makes the node look at another node (or mesh-node)
	\param tgt the CNode to look at
	Eg.
	\code
	CNode target;
	CNode seeker;
	...
	seeker.LookAt(target); // seeker will now face the target
	\endcode
	This code sets the pitch & yaw of the node, and sets roll=0
	\note will do nothing if the target point is at the nodes location
	(you cannot look at yourself)
	\see SetLookDirection for information on setting the DIRECTION to look
	*/
	void LookAt(const CNode& tgt);
	/** makes the node look in a given direction.
	Only sets the Yaw and Pitch values, sets Roll to Zero.
	\param dir the DIRECTION to look in (x axis, y axis or otherwise)
		This need not be normalised, but is should not be (0,0,0).
		If it is zero, then the code will ignore the request (its meaningless to look nowhere)
	\code
	CNode node;
	// look in the Z direction (forward)
	node.SetLookDirection(D3DXVECTOR3(0,0,1));
	// look in the X direction (right)
	node.SetLookDirection(D3DXVECTOR3(1,0,0));
	// look in the Y direction (up)
	node.SetLookDirection(D3DXVECTOR3(0,1,0));
	\endcode
	\see LookAt for information on looking AT a position 
	*/
	void SetLookDirection(const D3DXVECTOR3& dir);
private:
	CNode(const CNode&); // no copying
	void operator=(const CNode&); // no copying
};

/** CMeshNode is the basic actor class with a mesh & life.
It is designed to be derived from and to have its Update() function
overridden.
*/
class CMeshNode: public CNode
{
public:	// all are public for easy access
	CXMesh* mpMesh;	///< the X model
	float mScale;	///< assuming uniform scale
	float mLife;		///< life level of object if <=0 its to be removed
	D3DXVECTOR3 mDirection;
public:
	virtual ~CMeshNode(){}	// must have the virtual destructor
	/// Constructor
	/// \see Init() for the parameters
	CMeshNode(CXMesh* pMesh=NULL,const D3DXVECTOR3& pos=D3DXVECTOR3(0,0,0),
				const D3DXVECTOR3& turn=D3DXVECTOR3(0,0,0),
				float scale=1,int life=100);

    /** Initalise the object.
	\param pMesh the mesh to use (will not be delete'd at destruction)
	\param pos the position on the object
	\param turn the orientation
	\param scale the uniform scale of the object.
		(non uniform scaling is not permitted as it make collision more complex)

	\param life the life level(hp) of the object.
		Once the object has <1 life is considered 'dead' and will not be drawn/updated (usually)
	*/
	void Init(CXMesh* pMesh,const D3DXVECTOR3& pos=D3DXVECTOR3(0,0,0),
				const D3DXVECTOR3& turn=D3DXVECTOR3(0,0,0),
				float scale=1,int life=100);
	/// returns true if the object is alive
	bool IsAlive(){return mLife>0;}
	/// damages the target (decreased the life)
	void Damage(float dam);
	/// kills the object (sets life=0)
	void Destroy();
	/** Returns the bounding radius of the object.
	This is based on the mesh radius and the models scale.
	Unless the object is sphere shaped it will appear a little large */
	float GetBoundingRadius();
    /** updates/moves the object
    virtual function which can be overridden for derived classes
	\param dt the time (in seconds since last update)
	\note This function should not normally be called on a dead object.
	  But it does not check to see if the object is alive (caller is expected to do that)
	*/
	virtual void Update(float dt){}
    /** draws the object.
    virtual function which can be overridden for derived classes
	\note This function should not normally be called on a dead object.
	  But it does not check to see if the object is alive (caller is expected to do that)
	*/
	virtual void Draw();
	/** Draws the objects bounding sphere.
	\see CollisionMeshNode() for information on the factor
	*/
	void DrawBounds(IDirect3DDevice9* pDev,float factor=0.75f);

};

//////////////////////////////////////////////////////////////////////////

/** \defgroup MeshNodeFn Mesh node functions.
One of the most common ideas in computer games is the idea of groups of objects.
For example:
\code
// THIS IS NOT REAL C++ CODE, DO NOT COPY
ObjectGroup Enemies;
ObjectGroup Bullets;
Object Player;
...
// move player about
...
if (KeyPress(SPACE))
	Bullets.Add(new Bullet(....));
Enemies.MoveAll(); // moves all the enemies
Bullets.MoveAll(); // moves all the bullets
// add the collision code 
...
\endcode

In the GDEV engine, this is achieved by using a std::vector<CMeshNode*> as the 'ObjectGroup'

The vector is then filled with pointers to dynamically allocated CMeshNode (or derived classes)

There are then functions UpdateMeshNodes() & DrawMeshNodes() to move/draw the nodes accordingly.

There is also a function DeleteMeshNodes() which will can be used to delete all the object at the end of the game.

\see CollisionFn for more details on using MeshNode vector's in the context of collisions.
@{
*/

/** Updated/Moves all living members of the group.
This code can be though of as: (foreach living object: call Update)
\param nodes the vector of CMeshNodes
\param dt time since last update
*/
void UpdateMeshNodes(const std::vector<CMeshNode*>& nodes,float dt);

/** Draws all living members of the group.
This code can be though of as: (foreach living object: call Draw)
\param nodes the vector of CMeshNodes
*/
void DrawMeshNodes(const std::vector<CMeshNode*>& nodes);

/** Draws the bounds of all living members of the group.
This code can be though of as: (foreach living object: call Draw)
\param nodes the vector of CMeshNodes
*/
void DrawMeshNodeBounds(const std::vector<CMeshNode*>& nodes,IDirect3DDevice9* pDev);

/** Deletes every member of the vector & then clears the vecotr.
This code can be though of as: (foreach living object: call delete obj; vecotr.clear())
\param nodes the vector of CMeshNodes
*/
void DeleteMeshNodes(std::vector<CMeshNode*>& vec);

/** Finds and deletes all the dead CMeshNode's in the vector.
This is a rather clever function to solve a common problem in many programming languages.
<i>That is removing an element from a collection while searching through the collection.</i>

Take the following code:
\code
// BAD CODE DO NOT EVER USE
for(int i=0;i<vec.size();i++)
{
  if (...)
    delete vec[i]; // BAD CODE
}
\endcode
The issue is this: most collections don't like you adding/removing things while looping through.

To overcome this: the life flag on the CMeshNode's is used.
If you want to get rid of a node (for any reason), just set its life to zero.
When you then call this function it will safely delete the objects out and reorganise the vector to remove the corpse.

\code
// GOOD CODE: YOU CAN USE THIS
for(int i=0;i<vec.size();i++)
{
  if (...)
    vec[i]->Destroy(); // mark it as dead
}
// once all access to the vector is finished we can delete:
DeleteDeadMeshNodes(vec); // remove dead ones

\warning DO NOT call this function inside the loop.
	The whole point of this code is to mark the objects for destruction first.
	Then once the looping is finished, _then_ delete them out. 
\endcode
*/
void DeleteDeadMeshNodes(std::vector<CMeshNode*>& vec);

void NormalizeRotation(CMeshNode* nodes);

///@}

//////////////////////////////////////////////////////////////////////////

/** The CCameraNode is a node for holding camera infomation.
Like all nodes it can be moved about & made to look at other nodes.
It also has all the code to setup view & projection matrixes.

This class is good for First & Third person views as well as for overhead & fixed viewpoints.
Like the CNode class: its not really suitable for Cockpit views.

As well as the Camera position & orientation, it also has near & far planes as well as 
field of view and aspect ratio.

The best thing about this class is that being derived from the CNode class, 
anything you could be with a CNode (or CMeshNode), you can also do with a CCameraNode.

Eg. Its one line of code to do a first person camera
\code
camera.Match(player); // camera has the same position & orientation as player
\endcode
And its not to hard to do other camera nodes.
*/
class CCameraNode: public CNode
{
protected:
	float mFov;	///< field of view in radians
	float mAspect;	///< aspect ratio
	float mNear,mFar;	///< near & far clipping
public:
	/// Constuctor: see Init for the parameters
	CCameraNode(const D3DXVECTOR3& pos=D3DXVECTOR3(0,0,0),
				const D3DXVECTOR3& turn=D3DXVECTOR3(0,0,0),
				float fov=D3DX_PI/4, float aspect=1.333f,float nearDist=1.f,float farDist=1000.f);
	/** Sets camera parameters.
	\param pos,turn as per the node
	\param fov field of view in radians: PI/4 (45 degrees) is the normal
	\param aspect aspect ratio: for PC's is usually 4/3 (1.333) or 16/9 (1.777)
	\param near,far near & far clipping distance
	*/
	void Init(const D3DXVECTOR3& pos=D3DXVECTOR3(0,0,0),
				const D3DXVECTOR3& turn=D3DXVECTOR3(0,0,0),
				float fov=D3DX_PI/4, float aspect=1.333f,float nearDist=1.f,float farDist=1000.f);

	float GetFov(){return mFov;}	///< gets the field of view
	float GetAspect(){return mAspect;}	///< gets the aspect ratio
	float GetNear(){return mNear;}	///< gets the near clip
	float GetFar(){return mFar;}	///< gets the far clip
	/// Sets camera parameters.
	/// \see Init() for parameters
	void SetProjection(float fov,float aspect,float nearDist,float farDist);
	void SetFov(float fov);	///< sets field of view
	void SetAspect(float aspect);	///< sets aspect ratio
	void SetNearFar(float nearDist,float farDist);	///< sets near & far clip distance

	/** Sets the view & project matrix.
	\param pDev pointerto the device which requires the matrixes to be set
	*/
	void SetMatrixes(LPDIRECT3DDEVICE9 pDev);
	/// Gets the view matrix.
	/// this takes a bit of time to compute, so try not to call it needlessly.
	/// \param [out] outMatrix The matrix to be filled
	void GetViewMatrix( D3DXMATRIX& outMatrix );
	/// Gets the projection matrix.
	/// this takes a bit of time to compute, so try not to call it needlessly.
	/// \param [out] outMatrix The matrix to be filled
	void GetProjectionMatrix( D3DXMATRIX& outMatrix );

};

/** \defgroup CollisionFn Node & Mesh Collision functions.
There are two sets of collision routines in the Node code:
- Mesh Collisions which consider the size of the mesh
- Node Collisions which assume the node is point size

Most of the time you want to collide mesh vs mesh. Therefore you will want to use CollisionMeshNode()

However, sometimes you might want to collide things which do not have a mesh (such as the camera).
In which case you must 'estimate' the object size for yourself. You can just say zero, 
but there are times when this will not work (eg. if you have two nodes of size zero, then they will 
not collide if they are 0.00001 units apart). Usually its a good idea to assume size of one unit, or
if its a camera use the camera's near distance.

For example:
\code

\endcode

@{
*/

/** Returns if two CMeshNode's collide with each other.
\param p1,p2 the CMeshNode's
\param [optional]factor a factor controlling the sensitivity.
	Because the bounding spheres can be a little large (especially on long thin objects),
	this factor can be used to scale down the bounding sphere to make it less sensitive.
	A factor of 1 is normal size and a factor of 0 cannot be hit.
\returns if a collision occurs
\note for collisions when one object is moving at a high speed object (like a bullet), 
	you may wish to consider CollisionShotMeshNode()
*/
bool CollisionMeshNode(CMeshNode* p1, CMeshNode* p2,float factor=0.75f);

/** If a fast moving object passes through a meshnode.
This is a variant of CollisionMeshNode() which should be used if the velocity of one of the objects (the shot) is
a lot higher than its size. Therefore it might pass through its target. What would happen is one frame its before its target
then the next frame its move through it.
\param pShot the shot CMeshNode
\param shotVel its velocity (cannot be zero, obviously)
\param pTarget the target of collision
\param [optional]factor a factor controlling the sensitivity. See CollisionMeshNode for details.
\returns if a collision occurs
*/
bool CollisionShotMeshNode(CMeshNode* pShot,const D3DXVECTOR3& shotVel, CMeshNode* pTarget,float factor=0.75f);

/** If a fast moving object passes through a meshnode.
This is a variant of CollisionMeshNode() which should be used if the velocity of one of the objects (the shot) is
a lot higher than its size. Therefore it might pass through its target. What would happen is one frame its before its target
then the next frame its move through it.
\param shotPos position of the shot
\param shotVel velocity of the shot (cannot be zero, obviously)
\param pTarget the target of collision
\param [optional]factor a factor controlling the sensitivity. See CollisionMeshNode for details.
\returns if a collision occurs
*/
bool CollisionShotMeshNode(const D3DXVECTOR3& shotPos,const D3DXVECTOR3& shotVel, CMeshNode* pTarget,float factor=0.75f);

/** Returns if two nodes are near each other.
Since Nodes do not have a size, you must provide the estimated node size for collision
\param p1,p2 the nodes
\param dist the distance they need to be appear
\return if they are within the distance
*/
bool CollisionNodeNode(CNode* p1,CNode* p2,float dist);

/** Returns if a node is near a point.
Since Nodes do not have a size, you must provide the estimated node size for collision
\param p1 the node
\param pos the other point
\param dist the distance they need to be appear
\return if they are within the distance
*/
bool CollisionNodePoint(CNode* p1,const D3DXVECTOR3& pos,float dist);
///@}
