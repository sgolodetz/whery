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

/**
\brief An instance of this class represents a page of records for a database relation.

Pages of records are of a fixed size, and as such can hold a maximum number of records.
When they are full, additional pages must be allocated.
*/
class DataPage
{
	//#################### PRIVATE VARIABLES ####################
private:
	/** The memory buffer used by the page to hold the record data. */
	std::vector<char> m_buffer;

	/** The manipulator used to interact with the records in the buffer. */
	RecordManipulator m_recordManipulator;

	/** A map from record locations to the corresponding records. */
	std::map<const char*,Record> m_records;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a data page to contain records whose fields can be manipulated
	by the specified manipulators.

	\param fieldManipulators		A non-empty array of manipulators to be used to manipulate
									the fields of each record on the page.
	\throw std::invalid_argument	If fieldManipulators is empty.
	*/
	explicit DataPage(const std::vector<const FieldManipulator*>& fieldManipulators);

	//#################### PUBLIC METHODS ####################
public:
	Record add_record();

	void delete_record(const Record& record);

	unsigned int max_records() const;

	double percentage_full() const;

	unsigned int record_count() const;

	std::vector<Record> records() const;

	std::vector<Record> records_by_range(
		const std::vector<unsigned int>& projectedFields,
		const FieldTuple& lowerBound,
		const FieldTuple& upperBound
	) const;

	std::vector<Record> records_by_value(
		const std::vector<unsigned int>& projectedFields,
		const FieldTuple& key
	) const;

	unsigned int size() const;
};

}

#endif
