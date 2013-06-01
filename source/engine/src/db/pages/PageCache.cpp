/**
 * whery: PageCache.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/pages/PageCache.h"

#include <boost/thread/lock_guard.hpp>

namespace whery {

//#################### CONSTRUCTORS ####################

PageCache::PageCache(unsigned int maxBytes)
:	m_maxBytes(maxBytes)
{}

//#################### PUBLIC METHODS ####################

PageCacheID PageCache::add_page(const InMemorySortedPage_Ptr& page)
{
	boost::lock_guard<boost::mutex> guard(m_mutex);

	// TODO: Implement an ID allocator.
	PageCacheID id(23);
	m_pinnedPages.insert(std::make_pair(id, std::make_pair(page, PagePersister_CPtr())));
	return id;
}

bool PageCache::is_pinned(const PageCacheID& id) const
{
	boost::lock_guard<boost::mutex> guard(m_mutex);
	return m_pinnedPages.find(id) != m_pinnedPages.end();
}

InMemorySortedPage_Ptr PageCache::retrieve_page(const PageCacheID& id) const
{
	boost::lock_guard<boost::mutex> guard(m_mutex);

	std::map<PageCacheID,Entry>::const_iterator it = m_pinnedPages.find(id);
	if(it != m_pinnedPages.end())
	{
		return it->second.first;
	}

	// TODO
	throw std::exception("Support for unpinned pages is not yet implemented");
}

}
