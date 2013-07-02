/**
 * whery: Entity.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_ENTITY
#define H_WHERY_ENTITY

#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>

namespace whery {

using boost::uuids::uuid;

/**
\brief An instance of a class deriving from this one represents a database entity, e.g. a B+-tree node.

Database entities can be stored in an entity cache and persisted to/from disk as necessary.
*/
class Entity
{
	//#################### PROTECTED VARIABLES ####################
protected:
	/** The unique ID of the entity. */
	uuid m_id;

	//#################### CONSTRUCTORS ####################
protected:
	/**
	Constructs an entity.
	*/
	Entity();

	//#################### DESTRUCTOR ####################
public:
	/**
	Destroys the entity.
	*/
	virtual ~Entity();

	//#################### PUBLIC ABSTRACT METHODS ####################
public:
	/**
	Saves the entity to the specified path on disk.

	\param p	The path to which to save the entity.
	*/
	virtual void save(const boost::filesystem::path& p) const = 0;

	//#################### PUBLIC METHODS ####################
public:
	/**
	Returns the unique ID of the entity.

	\return	The unique ID of the entity.
	*/
	const uuid& id() const;
};

}

#endif
