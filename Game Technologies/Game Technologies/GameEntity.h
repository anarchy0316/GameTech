/******************************************************************************
Class:GameEntity
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk> and YOU!
Description: This is the base class for all of the objects in your game - the
player character, enemies, pickups etc would all be derived classes of the 
GameEntity. 

This class 'wraps up' all of the communication between the subsystems of your
game - it has a renderer component (The SceneNode you are used to using), a
'physics' component (The PhysicsNode you've been introduced to in this code
download), and eventually you'll add a 'sound' component, in the short sound
workshop in semester 2. 

The sub systems handle the updating of the 'sub components' of a GameEntity,
so in the Update function of this class will just be the 'gameplay' specific
type functionality (has the player picked up some health / has he shot an enemy
and so forth). 

The sub components of the GameEntity are accessible through 'Getter' functions,
but note that they return references, rather than the pointer type they have
been initialised to. This is to better encapsulate the data components, and to
prevent the components being deleted / new'd on top of - this should be only
allowable 'inside' the class that owns the sub components (ie 'this')

In order to 'hook' into the sub systems of the game, there are two functions to
add and remove the SceneNode / PhysicsNode from the Renderer and PhysicsSystem.
By doing this, the GameEntity has control over when it will be processed or
visible.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../nclgl/SceneNode.h"
#include "PhysicsNode.h"

class GameEntity	{
public:
	friend class GameClass;

	GameEntity(void);
	GameEntity(SceneNode* s, PhysicsNode* p);
	virtual ~GameEntity(void);

	virtual void	Update(float msec);

	SceneNode&		GetRenderNode()		{ return *renderNode;}
	PhysicsNode&	GetPhysicsNode()	{ return *physicsNode;}

	void			ConnectToSystems();
	void			DisconnectFromSystems();

protected:
	SceneNode*		renderNode;
	PhysicsNode*	physicsNode;
};

