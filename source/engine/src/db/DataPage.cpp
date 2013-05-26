/**
 * whery: DataPage.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/DataPage.h"

#include <cassert>

#include "whery/db/RangeKey.h"
#include "whery/db/TupleComparator.h"
#include "whery/db/TupleProjection.h"
#include "whery/db/ValueKey.h"

namespace whery {

DataPage::DataPage(const std::vector<const FieldManipulator*>& fieldManipulators, unsigned int bufferSize)
:	m_buffer(bufferSize), m_tupleManipulator(fieldManipulators)
{}

BackedTuple DataPage::add_tuple()
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

void DataPage::delete_tuple(const BackedTuple& tuple)
{
	m_tuples.erase(tuple.location());
	m_freeList.push_back(tuple);
}

unsigned int DataPage::empty_tuples() const
{
	return max_tuples() - tuple_count();
}

const std::vector<const FieldManipulator*>& DataPage::field_manipulators() const
{
	return m_tupleManipulator.field_manipulators();
}

unsigned int DataPage::max_tuples() const
{
	return m_buffer.size() / m_tupleManipulator.size();
}

double DataPage::percentage_full() const
{
	return tuple_count() * 100.0 / max_tuples();
}

unsigned int DataPage::tuple_count() const
{
	return m_tuples.size();
}

std::vector<BackedTuple> DataPage::tuples() const
{
	std::vector<BackedTuple> result;
	result.reserve(m_tuples.size());
	for(std::map<const char*,BackedTuple>::const_iterator it = m_tuples.begin(), iend = m_tuples.end(); it != iend; ++it)
	{
		result.push_back(it->second);
	}
	return result;
}

std::vector<BackedTuple> DataPage::tuples_by_range(const RangeKey& key) const
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
		const RangeEndpoint *lowEndpoint = key.low_endpoint();
		if(lowEndpoint != NULL)
		{
			int comp = comparator.compare(projection, lowEndpoint->value());
			if((lowEndpoint->kind() == CLOSED && comp < 0) ||
			   (lowEndpoint->kind() == OPEN && comp <= 0))
			{
				continue;
			}
		}

		// Check whether the tuple is excluded by the high end of the range.
		const RangeEndpoint *highEndpoint = key.high_endpoint();
		if(highEndpoint != NULL)
		{
			int comp = comparator.compare(projection, highEndpoint->value());
			if((highEndpoint->kind() == CLOSED && comp > 0) ||
			   (highEndpoint->kind() == OPEN && comp >= 0))
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

std::vector<BackedTuple> DataPage::tuples_by_value(const ValueKey& key) const
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

unsigned int DataPage::size() const
{
	return m_buffer.size();
}

}
