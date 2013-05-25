/**
 * whery: DataPage.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/DataPage.h"

#include <cassert>

#include "whery/db/FieldTupleComparator.h"
#include "whery/db/RecordProjection.h"

namespace whery {

DataPage::DataPage(const std::vector<const FieldManipulator*>& fieldManipulators)
:	m_recordManipulator(fieldManipulators)
{
	// TODO: Determine the size properly.
	m_buffer.resize(1024);
}

Record DataPage::add_record()
{
	if(record_count() >= max_records())
	{
		throw std::out_of_range("It is not possible to add an additional record to a full data page.");
	}

	char *location = &m_buffer[0] + record_count() * m_recordManipulator.size();
	Record record(location, m_recordManipulator);
	m_records.insert(std::make_pair(location, record));
	return record;
}

void DataPage::delete_record(const Record& record)
{
	// If the record is not actually in the page, early out.
	if(m_records.find(record.location()) == m_records.end()) return;

	// Copy the last record over the top of this one.
	Record lastRecord(&m_buffer[0] + (record_count() - 1) * m_recordManipulator.size(), m_recordManipulator);
	record.copy_from(lastRecord);

	// Remove the last record from the map.
	m_records.erase(lastRecord.location());
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

std::vector<Record> DataPage::records_by_range(
		const std::vector<unsigned int>& projectedFields,
		const FieldTuple& lowerBound,
		const FieldTuple& upperBound) const
{
	// TODO
	throw 23;
}

std::vector<Record> DataPage::records_by_value(
	const std::vector<unsigned int>& projectedFields,
	const FieldTuple& key) const
{
	assert(projectedFields.size() == key.arity());

	// Make the comparator.
	std::vector<std::pair<unsigned int,SortDirection> > fieldIndices;
	fieldIndices.reserve(key.arity());
	for(size_t i = 0; i < key.arity(); ++i) fieldIndices.push_back(std::make_pair(i, ASC));
	FieldTupleComparator comparator(fieldIndices);

	// Filter the records for those whose projection equals the key.
	std::vector<Record> results;
	results.reserve(m_records.size());
	for(std::map<const char*,Record>::const_iterator it = m_records.begin(), iend = m_records.end(); it != iend; ++it)
	{
		const Record& record = it->second;
		RecordProjection projection(record, projectedFields);
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
