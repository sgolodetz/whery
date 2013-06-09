/**
 * whery: CachedSortedPage.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/pages/CachedSortedPage.h"

namespace whery {

//#################### CONSTRUCTORS ####################

CachedSortedPage::CachedSortedPage(const PageCacheID& id, const PageCache_CPtr& cache)
:	m_cache(cache), m_id(id)
{}

//#################### PUBLIC INHERITED METHODS ####################

void CachedSortedPage::add_tuple(const Tuple& tuple)
{
	page()->add_tuple(tuple);
}

SortedPage::TupleSetCIter CachedSortedPage::begin() const
{
	return page()->begin();
}

unsigned int CachedSortedPage::buffer_size() const
{
	return page()->buffer_size();
}

void CachedSortedPage::clear()
{
	page()->clear();
}

void CachedSortedPage::delete_tuple(const BackedTuple& tuple)
{
	page()->delete_tuple(tuple);
}

unsigned int CachedSortedPage::empty_tuple_count() const
{
	return page()->empty_tuple_count();
}

SortedPage::TupleSetCIter CachedSortedPage::end() const
{
	return page()->end();
}

SortedPage::EqualRangeResult CachedSortedPage::equal_range(const RangeKey& key) const
{
	return page()->equal_range(key);
}

SortedPage::EqualRangeResult CachedSortedPage::equal_range(const ValueKey& key) const
{
	return page()->equal_range(key);
}

const std::vector<const FieldManipulator*>& CachedSortedPage::field_manipulators() const
{
	return page()->field_manipulators();
}

SortedPage::TupleSetCIter CachedSortedPage::find(const ValueKey& key) const
{
	return page()->find(key);
}

SortedPage::TupleSetCIter CachedSortedPage::lower_bound(const RangeKey& key) const
{
	return page()->lower_bound(key);
}

SortedPage::TupleSetCIter CachedSortedPage::lower_bound(const ValueKey& key) const
{
	return page()->lower_bound(key);
}

unsigned int CachedSortedPage::max_tuple_count() const
{
	return page()->max_tuple_count();
}

double CachedSortedPage::percentage_full() const
{
	return page()->percentage_full();
}

SortedPage::TupleSetCRIter CachedSortedPage::rbegin() const
{
	return page()->rbegin();
}

SortedPage::TupleSetCRIter CachedSortedPage::rend() const
{
	return page()->rend();
}

unsigned int CachedSortedPage::tuple_count() const
{
	return page()->tuple_count();
}

SortedPage::TupleSetCIter CachedSortedPage::upper_bound(const RangeKey& key) const
{
	return page()->upper_bound(key);
}

SortedPage::TupleSetCIter CachedSortedPage::upper_bound(const ValueKey& key) const
{
	return page()->upper_bound(key);
}

//#################### PRIVATE METHODS ####################

InMemorySortedPage_Ptr CachedSortedPage::page() const
{
	return m_cache->retrieve_page(m_id);
}

}
