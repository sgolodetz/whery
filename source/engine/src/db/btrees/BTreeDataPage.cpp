/**
 * whery: BTreeDataPage.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/btrees/BTreeDataPage.h"

#include <cassert>
#include <stdexcept>

#include "whery/db/base/RangeKey.h"
#include "whery/db/base/TupleProjection.h"
#include "whery/db/base/ValueKey.h"

namespace whery {

//#################### CONSTRUCTORS ####################

BTreeDataPage::BTreeDataPage(const std::vector<const FieldManipulator*>& fieldManipulators, unsigned int bufferSize)
:	m_buffer(bufferSize),
	m_tupleManipulator(fieldManipulators)
{}

BTreeDataPage::BTreeDataPage(unsigned int bufferSize, const TupleManipulator& tupleManipulator)
:	m_buffer(bufferSize),
	m_tupleManipulator(tupleManipulator)
{}

//#################### PUBLIC METHODS ####################

void BTreeDataPage::add_tuple(const Tuple& tuple)
{
	if(tuple_count() >= max_tuple_count())
	{
		throw std::out_of_range("It is not possible to add an additional tuple to a full data page.");
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
		char *location = &m_buffer[0] + tuple_count() * m_tupleManipulator.size();
		BackedTuple backedTuple(location, m_tupleManipulator);
		backedTuple.copy_from(tuple);
		backedTuple.make_read_only();
		m_tuples.insert(backedTuple);
	}
}

BTreeDataPage::TupleSetCIter BTreeDataPage::begin() const
{
	return m_tuples.begin();
}

void BTreeDataPage::delete_tuple(const BackedTuple& tuple)
{
	TupleSet::iterator it = m_tuples.find(tuple);
	if(it != m_tuples.end())
	{
		m_freeList.push_back(const_cast<char*>(it->location()));
		m_tuples.erase(it);
	}
}

unsigned int BTreeDataPage::empty_tuple_count() const
{
	return max_tuple_count() - tuple_count();
}

BTreeDataPage::TupleSetCIter BTreeDataPage::end() const
{
	return m_tuples.end();
}

BTreeDataPage::EqualRangeResult BTreeDataPage::equal_range(const RangeKey& key) const
{
	return std::make_pair(lower_bound(key), upper_bound(key));
}

BTreeDataPage::EqualRangeResult BTreeDataPage::equal_range(const ValueKey& key) const
{
	return std::make_pair(lower_bound(key), upper_bound(key));
}

const std::vector<const FieldManipulator*>& BTreeDataPage::field_manipulators() const
{
	return m_tupleManipulator.field_manipulators();
}

BTreeDataPage::TupleSetCIter BTreeDataPage::lower_bound(const RangeKey& key) const
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

BTreeDataPage::TupleSetCIter BTreeDataPage::lower_bound(const ValueKey& key) const
{
	return m_tuples.lower_bound(key);
}

unsigned int BTreeDataPage::max_tuple_count() const
{
	return m_buffer.size() / m_tupleManipulator.size();
}

double BTreeDataPage::percentage_full() const
{
	return tuple_count() * 100.0 / max_tuple_count();
}

unsigned int BTreeDataPage::tuple_count() const
{
	return m_tuples.size();
}

BTreeDataPage::TupleSetCIter BTreeDataPage::upper_bound(const RangeKey& key) const
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

BTreeDataPage::TupleSetCIter BTreeDataPage::upper_bound(const ValueKey& key) const
{
	return m_tuples.upper_bound(key);
}

unsigned int BTreeDataPage::size() const
{
	return m_buffer.size();
}

}
