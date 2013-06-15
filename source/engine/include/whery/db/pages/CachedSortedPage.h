/**
 * whery: CachedSortedPage.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_CACHEDSORTEDPAGE
#define H_WHERY_CACHEDSORTEDPAGE

#include "PageCache.h"
#include "SortedPage.h"

namespace whery {

/**
\brief An instance of this class represents a sorted page of tuples that is backed by the page cache.

In practical terms, this class exists to hide accesses to the cache behind the normal page interface,
allowing the rest of the code to work with pages without worrying about caching behind the scenes.
(Put another way, this class helps separate the concerns of caching and page interaction.)
*/
class CachedSortedPage : public SortedPage
{
	//#################### PRIVATE VARIABLES ####################
private:
	/** A pointer to the cache with which the underlying page is registered. */
	PageCache_CPtr m_cache;

	/** The ID of the underlying page in the cache. */
	PageCacheID m_id;

	//#################### CONSTRUCTORS ####################
public:
	CachedSortedPage(const PageCacheID& id, const PageCache_CPtr& cache);

	//#################### PUBLIC INHERITED METHODS ####################
public:
	virtual void add_tuple(const Tuple& tuple);
	virtual TupleSetCIter begin() const;
	virtual unsigned int buffer_size() const;
	virtual void clear();
	virtual unsigned int empty_tuple_count() const;
	virtual TupleSetCIter end() const;
	virtual EqualRangeResult equal_range(const RangeKey& key) const;
	virtual EqualRangeResult equal_range(const ValueKey& key) const;
	virtual void erase_tuple(const BackedTuple& key);
	virtual void erase_tuple(const TupleSetCIter& it);
	virtual const std::vector<const FieldManipulator*>& field_manipulators() const;
	virtual TupleSetCIter find(const ValueKey& key) const;
	virtual TupleSetCIter lower_bound(const RangeKey& key) const;
	virtual TupleSetCIter lower_bound(const ValueKey& key) const;
	virtual unsigned int max_tuple_count() const;
	virtual double percentage_full() const;
	virtual TupleSetCRIter rbegin() const;
	virtual TupleSetCRIter rend() const;
	virtual unsigned int tuple_count() const;
	virtual TupleSetCIter upper_bound(const RangeKey& key) const;
	virtual TupleSetCIter upper_bound(const ValueKey& key) const;

	//#################### PRIVATE METHODS ####################
private:
	/**
	Retrieves the underlying page from the cache.

	\return	The underlying page.
	*/
	InMemorySortedPage_Ptr page() const;
};

}

#endif
