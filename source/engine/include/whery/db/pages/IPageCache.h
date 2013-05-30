/**
 * whery: IPageCache.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_IPAGECACHE
#define H_WHERY_IPAGECACHE

#include "SortedPage.h"

namespace whery {

//#################### FORWARD DECLARATIONS ####################
class PageCacheID;

/**
\brief An instance of a class deriving from this one represents a cache of sorted pages.

Pages in the cache will be kept in memory where possible. If there are too many pages
for them all to be kept in memory simultaneously, some of them will be swapped out to
the hard disk and reloaded as necessary.
*/
class IPageCache
{
	//#################### DESTRUCTOR ####################
public:
	/**
	Destroys the page cache.
	*/
	virtual ~IPageCache() {}

	//#################### PUBLIC ABSTRACT METHODS ####################
public:
	/**
	Retrieves the page with the specified ID from the cache.

	\param id						The ID of the page to retrieve.
	\return							The retrieved page.
	\throw std::invalid_argument	If the ID does not refer to a page that is in the cache.
	*/
	virtual SortedPage_Ptr page(const PageCacheID& id) = 0;
};

}

#endif
