/**
 * whery: BTreeDataPage.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/btrees/BTreeDataPage.h"

#include <cassert>
#include <stdexcept>

#include "whery/db/base/RangeKey.h"
#include "whery/db/base/TupleComparator.h"
#include "whery/db/base/TupleProjection.h"
#include "whery/db/base/ValueKey.h"

namespace whery {

//#################### CONSTRUCTORS ####################

BTreeDataPage::BTreeDataPage(const std::vector<const FieldManipulator*>& fieldManipulators, unsigned int bufferSize)
:	m_buffer(bufferSize), m_tupleManipulator(fieldManipulators)
{}

BTreeDataPage::BTreeDataPage(unsigned int bufferSize, const TupleManipulator& tupleManipulator)
:	m_buffer(bufferSize), m_tupleManipulator(tupleManipulator)
{}

//#################### PUBLIC METHODS ####################

BackedTuple BTreeDataPage::add_tuple()
{
	if(tuple_count() >= max_tuples())
	{
		throw std::out_of_range("It is not possible to add an additional tuple to a full data page.");
	}

	if(!m_freeList.empty())
	{
		BackedTuple tuple = m_freeList.back();
		m_freeList.pop_back();
		m_tuples.insert(std::make_pair(tuple.location(), tuple));
		return tuple;
	}
	else
	{
		char *location = &m_buffer[0] + tuple_count() * m_tupleManipulator.size();
		BackedTuple tuple(location, m_tupleManipulator);
		m_tuples.insert(std::make_pair(location, tuple));
		return tuple;
	}
}

void BTreeDataPage::delete_tuple(const BackedTuple& tuple)
{
	// TODO: Consider not putting the tuple on the free list if it's the last one in the map.
	m_tuples.erase(tuple.location());
	m_freeList.push_back(tuple);
}

unsigned int BTreeDataPage::empty_tuples() const
{
	return max_tuples() - tuple_count();
}

const std::vector<const FieldManipulator*>& BTreeDataPage::field_manipulators() const
{
	return m_tupleManipulator.field_manipulators();
}

unsigned int BTreeDataPage::max_tuples() const
{
	return m_buffer.size() / m_tupleManipulator.size();
}

double BTreeDataPage::percentage_full() const
{
	return tuple_count() * 100.0 / max_tuples();
}

unsigned int BTreeDataPage::tuple_count() const
{
	return m_tuples.size();
}

std::vector<BackedTuple> BTreeDataPage::tuples() const
{
	std::vector<BackedTuple> result;
	result.reserve(m_tuples.size());
	for(std::map<const char*,BackedTuple>::const_iterator it = m_tuples.begin(), iend = m_tuples.end(); it != iend; ++it)
	{
		result.push_back(it->second);
	}
	return result;
}

std::vector<BackedTuple> BTreeDataPage::tuples_by_range(const RangeKey& key) const
{
	std::vector<BackedTuple> results;
	results.reserve(m_tuples.size());

	TupleComparator comparator = TupleComparator::make_default(key.arity());

	// Filter the tuples for those whose projection on the key's field indices
	// falls within the range specified by the key.
	for(std::map<const char*,BackedTuple>::const_iterator it = m_tuples.begin(), iend = m_tuples.end(); it != iend; ++it)
	{
		const BackedTuple& tuple = it->second;
		TupleProjection projection(tuple, key.field_indices());

		// Check whether the tuple is excluded by the low end of the range.
		if(key.has_low_endpoint())
		{
			int comp = comparator.compare(projection, key.low_value());
			if((key.low_kind() == CLOSED && comp < 0) ||
			   (key.low_kind() == OPEN && comp <= 0))
			{
				continue;
			}
		}

		// Check whether the tuple is excluded by the high end of the range.
		if(key.has_high_endpoint())
		{
			int comp = comparator.compare(projection, key.high_value());
			if((key.high_kind() == CLOSED && comp > 0) ||
			   (key.high_kind() == OPEN && comp >= 0))
			{
				continue;
			}
		}

		// If we get here, the tuple was not excluded by either the low
		// or high end of the range, so we add it to the results.
		results.push_back(tuple);
	}

	return results;
}

std::vector<BackedTuple> BTreeDataPage::tuples_by_value(const ValueKey& key) const
{
	std::vector<BackedTuple> results;
	results.reserve(m_tuples.size());

	TupleComparator comparator = TupleComparator::make_default(key.arity());

	// Filter the tuples for those whose projection on the key's field indices equals the key.
	for(std::map<const char*,BackedTuple>::const_iterator it = m_tuples.begin(), iend = m_tuples.end(); it != iend; ++it)
	{
		const BackedTuple& tuple = it->second;
		TupleProjection projection(tuple, key.field_indices());
		if(comparator.compare(projection, key) == 0)
		{
			results.push_back(tuple);
		}
	}

	return results;
}

unsigned int BTreeDataPage::size() const
{
	return m_buffer.size();
}

}
