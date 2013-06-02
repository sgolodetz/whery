/**
 * whery: IDAllocator.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/util/IDAllocator.h"

#include <stdexcept>

namespace whery {

//#################### PUBLIC METHODS ####################

int IDAllocator::allocate()
{
	int n;

	if(!m_free.empty())
	{
		n = *m_free.begin();
		m_free.erase(m_free.begin());
	}
	else
	{
		n = static_cast<int>(m_used.size());
	}

	m_used.insert(n);
	return n;
}

void IDAllocator::deallocate(int n)
{
	std::set<int>::iterator it = m_used.find(n);
	if(it == m_used.end())
	{
		throw std::invalid_argument("The specified ID is not currently in use.");
	}

	// Note: If we get here, m_used is guaranteed to be non-empty (as it contains n).
	if(n == *m_used.rbegin())
	{
		// We're removing the last element of the used set.
		m_used.erase(it);
		m_free.erase(m_free.upper_bound(max_used()), m_free.end());
	}
	else
	{
		m_used.erase(it);
		m_free.insert(n);
	}
}

void IDAllocator::reset()
{
	m_free.clear();
	m_used.clear();
}

//#################### PRIVATE METHODS ####################

int IDAllocator::max_used() const
{
	if(m_used.empty()) return -1;
	else return *m_used.rbegin();
}

}
