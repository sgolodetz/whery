/**
 * whery: EntityCache.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_ENTITYCACHE
#define H_WHERY_ENTITYCACHE

#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace whery {

//#################### FORWARD DECLARATIONS ####################
class EntityCacheID;

/**
\brief An instance of this class represents a cache of database entities.

Entities in the cache will be kept in memory where possible. If there are too many entities for
them all to be kept in memory simultaneously, some of the entities will be swapped out to disk
and later reloaded as necessary (assuming that not all of the entities that are in memory are
currently in use).
*/
class EntityCache
{
	//#################### TYPEDEFS ####################
public:
	typedef boost::shared_lock<boost::shared_mutex> ReadLock;
	typedef boost::shared_ptr<const ReadLock> ReadLock_CPtr;
	typedef boost::lock_guard<boost::shared_mutex> WriteLock;
	typedef boost::shared_ptr<const WriteLock> WriteLock_CPtr;

	//#################### PUBLIC METHODS ####################
public:
	template <typename T>
	boost::shared_ptr<T> acquire_exclusive_entity(const EntityCacheID& id, WriteLock_CPtr& lock)
	{
		// TODO
		throw 23;
	}

	template <typename T>
	boost::shared_ptr<const T> acquire_shared_entity(const EntityCacheID& id, ReadLock_CPtr& lock)
	{
		// TODO
		throw 23;
	}
};

}

#endif
