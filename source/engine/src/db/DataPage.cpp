/**
 * whery: DataPage.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/DataPage.h"

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

std::vector<Record> DataPage::records_by_value(
	const std::vector<unsigned int>& projectedFields,
	const FieldTuple& key) const
{
	// TODO
	throw 23;
}

unsigned int DataPage::size() const
{
	return m_buffer.size();
}

}
