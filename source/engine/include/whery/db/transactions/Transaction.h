/**
 * whery: Transaction.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_TRANSACTION
#define H_WHERY_TRANSACTION

#include "whery/db/entities/EntityCache.h"

namespace whery {

/**
\brief An instance of this class tracks a program that is run against the database.

In practical terms, transaction objects are passed around and used to mediate between
higher-level code and the entity cache, keeping track of which entities are accessed
in the course of executing the program and whether they are modified.
*/
class Transaction
{
	//#################### PUBLIC METHODS ####################
public:
	template <typename T>
	boost::shared_ptr<T> acquire_exclusive_entity(const EntityCacheID& id, const boost::shared_ptr<T>& /* dummy */)
	{
		// TODO
		throw 23;
	}

	template <typename T>
	boost::shared_ptr<const T> acquire_shared_entity(const EntityCacheID& id, const boost::shared_ptr<const T>& /* dummy */)
	{
		// TODO
		throw 23;
	}
};

}

#endif
