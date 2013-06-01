/**
 * whery: InMemorySortedPage.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/pages/InMemorySortedPage.h"

#include <cassert>
#include <stdexcept>

#include "whery/db/base/RangeKey.h"
#include "whery/db/base/TupleProjection.h"
#include "whery/db/base/ValueKey.h"

namespace whery {

//#################### CONSTRUCTORS ####################

InMemorySortedPage::InMemorySortedPage(const std::vector<const FieldManipulator*>& fieldManipulators, unsigned int bufferSize)
:	m_buffer(new std::vector<char>(bufferSize)),
	m_tupleManipulator(fieldManipulators)
{}

InMemorySortedPage::InMemorySortedPage(unsigned int bufferSize, const TupleManipulator& tupleManipulator)
:	m_buffer(new std::vector<char>(bufferSize)),
	m_tupleManipulator(tupleManipulator)
{}

//#################### PUBLIC METHODS ####################

void InMemorySortedPage::add_tuple(const Tuple& tuple)
{
	if(tuple_count() >= max_tuple_count())
	{
		throw std::out_of_range("It is not possible to add an additional tuple to a full page.");
	}

	if(!m_freeList.empty())
	{
		BackedTuple backedTuple(m_freeList.back(), m_tupleManipulator);
		m_freeList.pop_back();
		backedTuple.copy_from(tuple);
		backedTuple.make_read_only();
		m_tuples.insert(backedTuple);
	}
	else
	{
		char *location = &(*m_buffer)[0] + tuple_count() * m_tupleManipulator.size();
		BackedTuple backedTuple(location, m_tupleManipulator);
		backedTuple.copy_from(tuple);
		backedTuple.make_read_only();
		m_tuples.insert(backedTuple);
	}
}

SortedPage::TupleSetCIter InMemorySortedPage::begin() const
{
	return m_tuples.begin();
}

unsigned int InMemorySortedPage::buffer_size() const
{
	return m_buffer->size();
}

void InMemorySortedPage::delete_tuple(const BackedTuple& tuple)
{
	TupleSet::iterator it = m_tuples.find(tuple);
	if(it != m_tuples.end())
	{
		m_freeList.push_back(const_cast<char*>(it->location()));
		m_tuples.erase(it);
	}
}

unsigned int InMemorySortedPage::empty_tuple_count() const
{
	return max_tuple_count() - tuple_count();
}

SortedPage::TupleSetCIter InMemorySortedPage::end() const
{
	return m_tuples.end();
}

SortedPage::EqualRangeResult InMemorySortedPage::equal_range(const RangeKey& key) const
{
	return std::make_pair(lower_bound(key), upper_bound(key));
}

SortedPage::EqualRangeResult InMemorySortedPage::equal_range(const ValueKey& key) const
{
	return std::make_pair(lower_bound(key), upper_bound(key));
}

const std::vector<const FieldManipulator*>& InMemorySortedPage::field_manipulators() const
{
	return m_tupleManipulator.field_manipulators();
}

SortedPage::TupleSetCIter InMemorySortedPage::lower_bound(const RangeKey& key) const
{
	if(key.has_low_endpoint())
	{
		TupleSetCIter it = m_tuples.lower_bound(key.low_value());
		if(key.low_kind() == OPEN)
		{
			PrefixTupleComparator comp;
			while(it != m_tuples.end() && comp.compare(*it, key.low_value()) == 0) ++it;
		}
		return it;
	}
	else return m_tuples.begin();
}

SortedPage::TupleSetCIter InMemorySortedPage::lower_bound(const ValueKey& key) const
{
	return m_tuples.lower_bound(key);
}

unsigned int InMemorySortedPage::max_tuple_count() const
{
	return buffer_size() / m_tupleManipulator.size();
}

double InMemorySortedPage::percentage_full() const
{
	return tuple_count() * 100.0 / max_tuple_count();
}

unsigned int InMemorySortedPage::tuple_count() const
{
	return m_tuples.size();
}

SortedPage::TupleSetCIter InMemorySortedPage::upper_bound(const RangeKey& key) const
{
	if(key.has_high_endpoint())
	{
		TupleSetCIter it = m_tuples.upper_bound(key.high_value());
		if(key.high_kind() == OPEN)
		{
			PrefixTupleComparator comp;
			TupleSet::const_reverse_iterator rit(it);
			while(rit != m_tuples.rend() && comp.compare(*rit, key.high_value()) == 0) ++rit;
			it = rit.base();
		}
		return it;
	}
	else return m_tuples.end();
}

SortedPage::TupleSetCIter InMemorySortedPage::upper_bound(const ValueKey& key) const
{
	return m_tuples.upper_bound(key);
}

}
