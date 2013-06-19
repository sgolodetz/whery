/**
 * whery: Entity.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_ENTITY
#define H_WHERY_ENTITY

namespace whery {

/**
\brief An instance of a class deriving from this one represents a database entity, e.g. a B+-tree node.

Database entities can be stored in an entity cache and persisted to/from disk as necessary.
*/
class Entity
{
	//#################### DESTRUCTOR ####################
public:
	/**
	Destroys the entity.
	*/
	virtual ~Entity();

	//#################### PUBLIC ABSTRACT METHODS ####################
public:
	// TODO: Persist to/from disk.
};

}

#endif
