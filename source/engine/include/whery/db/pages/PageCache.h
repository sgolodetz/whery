/**
 * whery: PageCache.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_PAGECACHE
#define H_WHERY_PAGECACHE

#include <map>

#include <boost/thread/mutex.hpp>

#include "InMemorySortedPage.h"

namespace whery {

//#################### FORWARD DECLARATIONS ####################
class PagePersister;

//#################### TYPEDEFS ####################
typedef boost::shared_ptr<const PagePersister> PagePersister_CPtr;

//#################### HELPER CLASSES ####################

/**
\brief An instance of this class represents the ID of a page in a page cache.
*/
class PageCacheID
{
private:
	int m_id;

	PageCacheID(int id)
	: m_id(id)
	{}

public:
	bool operator<(const PageCacheID& rhs) const
	{
		return m_id < rhs.m_id;
	}

	friend class PageCache;
};

//#################### MAIN CLASS ####################

/**
\brief An instance of this class represents a cache of sorted pages.

Pages in the cache will be kept in memory where possible. If there are too many pages for
them all to be kept in memory simultaneously, some of the pages that are persistable will
be swapped out to the hard disk and later reloaded as necessary. It is possible to "pin"
individual pages to prevent them from being swapped out to disk.
*/
class PageCache
{
	//#################### TYPEDEFS ####################
private:
	typedef std::pair<InMemorySortedPage_Ptr,PagePersister_CPtr> Entry;

	//#################### PRIVATE VARIABLES ####################
private:
	/**
	The maximum total number of bytes we want to use for pages in the cache
	(a soft limit in practice).
	*/
	unsigned int m_maxBytes;

	/** A mutex used to serialise accesses to the cache. */
	mutable boost::mutex m_mutex;

	/**
	A map from persisters to IDs of pages in the cache. This is used to ensure that
	multiple pages in the cache cannot share the same persistence target on disk.
	*/
	// TODO

	/**
	The pages in the cache that are currently pinned (these will be kept in memory
	for as long as they remain pinned). Note that not every pinned page has to have
	a persister; pages without a persister are ineligible for swapping out to disk
	and, as such, cannot be unpinned.
	*/
	std::map<PageCacheID,Entry> m_pinnedPages;

	/**
	A priority queue of the pages in the cache that are currently unpinned
	(these are eligible to be swapped out to disk at any point).
	*/
	// TODO

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a page cache that attempts to swap pages out to disk so as to keep
	the overall memory usage for cached pages below the specified number of bytes.
	This may not always be possible if too many pages are pinned, so the cache
	will simply make a best attempt rather than providing a guaranteed memory
	limit (there's probably more memory available, we're just trying not to use it).

	\param maxBytes	The maximum total number of bytes we want to use for pages in
					the cache (a soft limit in practice).
	*/
	explicit PageCache(unsigned int maxBytes);

	//#################### PUBLIC METHODS ####################
public:
	/**
	Adds a non-persistable page to the cache.

	\param page	The page to add.
	\return		An ID that can be used to retrieve the page from the cache.
	*/
	PageCacheID add_page(const InMemorySortedPage_Ptr& page);

	/**
	Adds a persistable page that has already been loaded from disk to the cache.
	If a page corresponding to the specified persister is already in the cache,
	then an exception will be thrown (this prevents there from being multiple
	pages in the cache that are persisted from/to the same place on disk, which
	would clearly be unfortunate).

	\param page						The page to add.
	\param persister				The persister that can be used to load/save the page from/to disk.
	\return							An ID that can be used to retrieve the page from the cache.
	\throw std::invalid_argument	If a page corresponding to the specified persister is already in the cache.
	*/
	PageCacheID add_page(const InMemorySortedPage_Ptr& page, const PagePersister_CPtr& persister);

	/**
	Adds a persistable page that has not yet been loaded from disk to the cache.
	Note that this method does not eagerly load the page - it will be loaded as
	and when it is actually required. If a page corresponding to the specified
	persister is already in the cache, the ID of that will be returned instead
	of adding a new cache entry (this prevents there from being multiple pages
	in the cache that are persisted from/to the same place on disk, which would
	clearly be unfortunate).

	\param persister	The persister that can be used to load/save the page from/to disk.
	\return				An ID that can be used to retrieve the page from the cache.
	*/
	PageCacheID add_page(const PagePersister_CPtr& persister);

	/**
	Flushes the specified persistable page to disk, provided it is currently in memory.

	\param id						The ID of the page to flush.
	\throw std::invalid_argument	If the ID refers to a non-persistable page.
	*/
	void flush_page(const PageCacheID& id);

	/**
	Returns whether or not the specified page is persistable.

	\param id	The ID of the page to check.
	\return		true, if the specified page is persistable, or false otherwise.
	*/
	bool is_persistable(const PageCacheID& id) const;

	/**
	Returns whether or not the specified page is currently pinned.

	\param id	The ID of the page to check.
	\return		true, if the specified page is currently pinned, or false otherwise.
	*/
	bool is_pinned(const PageCacheID& id) const;

	/**
	"Pins" the specified persistable page, preventing it from being swapped out to disk.
	If the page is not currently in memory, it will be eagerly loaded.

	\param id						The ID of the page to pin.
	\throw std::invalid_argument	If the ID refers to a non-persistable page.
	*/
	void pin_page(const PageCacheID& id);

	/**
	Removes the specified page from the cache, if it is present. By default, persistable
	pages that are in memory will be flushed to disk before removal from the cache.

	\param id			The ID of the page to remove.
	\param flushToDisk	Whether or not to flush the page to disk before removal if possible.
	*/
	void remove_page(const PageCacheID& id, bool flushToDisk = true);

	/**
	Retrieves the page with the specified ID from the cache. This may involve loading the
	page from disk if it is not already in memory, which may in turn cause unpinned pages
	that are already in memory to be swapped out.

	\param id						The ID of the page to retrieve.
	\return							The retrieved page.
	\throw std::invalid_argument	If the ID does not refer to a page that is in the cache.
	*/
	InMemorySortedPage_Ptr retrieve_page(const PageCacheID& id) const;

	/**
	"Unpins" the specified persistable page, making it eligible to be swapped out to disk.

	\param id						The ID of the page to unpin.
	\throw std::invalid_argument	If the ID refers to a non-persistable page.
	*/
	void unpin_page(const PageCacheID& id);
};

}

#endif
