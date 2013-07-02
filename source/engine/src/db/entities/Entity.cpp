/**
 * whery: Entity.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/entities/Entity.h"

namespace whery {

//#################### CONSTRUCTORS ####################

Entity::Entity()
:	m_id()
{}

//#################### DESTRUCTOR ####################

Entity::~Entity()
{}

//#################### PUBLIC METHODS ####################

const uuid& Entity::id() const
{
	return m_id;
}

}
