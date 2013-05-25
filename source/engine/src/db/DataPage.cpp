/**
 * whery: DataPage.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/DataPage.h"

#include <cassert>

#include "whery/db/FieldTupleComparator.h"
#include "whery/db/RecordProjection.h"
#include "whery/db/ValueKey.h"

namespace whery {

DataPage::DataPage(const std::vector<const FieldManipulator*>& fieldManipulators, unsigned int bufferSize)
:	m_buffer(bufferSize), m_recordManipulator(fieldManipulators)
{}

Record DataPage::add_record()
{
	if(record_count() >= max_records())
	{
		throw std::out_of_range("It is not possible to add an additional record to a full data page.");
	}

	if(!m_freeList.empty())
	{
		Record record = m_freeList.back();
		m_freeList.pop_back();
		m_records.insert(std::make_pair(record.location(), record));
		return record;
	}
	else
	{
		char *location = &m_buffer[0] + record_count() * m_recordManipulator.size();
		Record record(location, m_recordManipulator);
		m_records.insert(std::make_pair(location, record));
		return record;
	}
}

void DataPage::delete_record(const Record& record)
{
	m_records.erase(record.location());
	m_freeList.push_back(record);
}

unsigned int DataPage::empty_records() const
{
	return max_records() - record_count();
}

const std::vector<const FieldManipulator*>& DataPage::field_manipulators() const
{
	return m_recordManipulator.field_manipulators();
}

unsigned int DataPage::max_records() const
{
	return m_buffer.size() / m_recordManipulator.size();
}

double DataPage::percentage_full() const
{
	return record_count() * 100.0 / max_records();
}

unsigned int DataPage::record_count() const
{
	return m_records.size();
}

std::vector<Record> DataPage::records() const
{
	std::vector<Record> result;
	result.reserve(m_records.size());
	for(std::map<const char*,Record>::const_iterator it = m_records.begin(), iend = m_records.end(); it != iend; ++it)
	{
		result.push_back(it->second);
	}
	return result;
}

std::vector<Record> DataPage::records_by_range(const RangeKey& key) const
{
	// TODO
	throw 23;
}

std::vector<Record> DataPage::records_by_value(const ValueKey& key) const
{
	// Make the comparator.
	std::vector<std::pair<unsigned int,SortDirection> > comparisonFields;
	comparisonFields.reserve(key.arity());
	for(size_t i = 0; i < key.arity(); ++i) comparisonFields.push_back(std::make_pair(i, ASC));
	FieldTupleComparator comparator(comparisonFields);

	// Filter the records for those whose projection on the key's field indices equals the key.
	std::vector<Record> results;
	results.reserve(m_records.size());
	for(std::map<const char*,Record>::const_iterator it = m_records.begin(), iend = m_records.end(); it != iend; ++it)
	{
		const Record& record = it->second;
		RecordProjection projection(record, key.field_indices());
		if(comparator.compare(projection, key) == 0)
		{
			results.push_back(record);
		}
	}

	return results;
}

unsigned int DataPage::size() const
{
	return m_buffer.size();
}

}
