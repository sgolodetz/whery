/**
 * whery: EntityCache.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_ENTITYCACHE
#define H_WHERY_ENTITYCACHE

namespace whery {

/**
\brief An instance of this class represents a cache of database entities.

Entities in the cache will be kept in memory where possible. If there are too many entities for
them all to be kept in memory simultaneously, some of the entities will be swapped out to disk
and later reloaded as necessary (assuming that not all of the entities that are in memory are
currently in use).
*/
class EntityCache
{
	// TODO
};

}

#endif
