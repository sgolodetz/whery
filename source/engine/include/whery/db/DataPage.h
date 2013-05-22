/**
 * whery: DataPage.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_DATAPAGE
#define H_WHERY_DATAPAGE

#include <map>
#include <vector>

#include "Record.h"

namespace whery {

class DataPage
{
private:
	std::vector<char> m_buffer;
	RecordManipulator m_recordManipulator;
	std::map<const char*,Record> m_records;

public:
	explicit DataPage(const std::vector<const FieldManipulator*>& fieldManipulators);

public:
	Record add_record();

	void delete_record(const Record& record);

	unsigned int max_records() const;

	double percentage_full() const;

	unsigned int record_count() const;

	std::vector<Record> records() const;

	std::vector<Record> records_by_value(
		const std::vector<unsigned int>& projectedFields,
		const FieldTuple& key
	) const;

	std::vector<Record> records_by_range(
		const std::vector<unsigned int>& projectedFields,
		const FieldTuple& lowerBound,
		const FieldTuple& upperBound
	) const;

	unsigned int size() const;
};

}

#endif
