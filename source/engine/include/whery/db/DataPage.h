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

	/** A free list of records that have been deleted - these can be reallocated by add_record(). */
	std::vector<Record> m_freeList;

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
	\param bufferSize				The size (in bytes) to use for the page's memory buffer.
	\throw std::invalid_argument	If fieldManipulators is empty.
	*/
	DataPage(const std::vector<const FieldManipulator*>& fieldManipulators, unsigned int bufferSize);

	//#################### COPY CONSTRUCTOR & ASSIGNMENT OPERATOR ####################
private:
	/**
	Private and unimplemented for now - the compiler-generated defaults
	would be inadequate, because the copied records map would refer to
	the buffer on the original data page.
	*/
	DataPage(const DataPage&);
	DataPage& operator=(const DataPage&);

	//#################### PUBLIC METHODS ####################
public:
	/**
	Adds a record to the page, provided there is enough space to do so.

	\return						A Record object referring to the newly-added record, if the add succeeds.
	\throw std::out_of_range	If there is not enough space on the page to add a record.
	*/
	Record add_record();

	/**
	Deletes the specified record from the page, if it is present. The Record object
	is invalid after the deletion and should no longer be used by the calling code.

	\param record	A Record object referring to the record to delete.
	*/
	void delete_record(const Record& record);

	/**
	Gets the number of additional records that can fit on the page.

	\return	The number of additional records that can fit on the page.
	*/
	unsigned int empty_records() const;

	/**
	Gets the manipulators for the fields in records on the page.

	\return	The manipulators for the fields in records on the page.
	*/
	const std::vector<const FieldManipulator*>& field_manipulators() const;

	/**
	Gets the maximum number of records that can be stored on the page.

	\return	The maximum number of records that can be stored on the page.
	*/
	unsigned int max_records() const;

	/**
	Gets the percentage of the page's buffer that currently contains records.

	\return	The percentage of the page's buffer that currently contains records (in the range 0-100).
	*/
	double percentage_full() const;

	/**
	Gets the number of records that are currently stored on the page.

	\return	The number of records that are currently stored on the page.
	*/
	unsigned int record_count() const;

	/**
	Gets an array of Record objects that refer to the records on the page.

	\return	An array of Record objects that refer to the records on the page.
	*/
	std::vector<Record> records() const;

	std::vector<Record> records_by_range(
		const std::vector<unsigned int>& projectedFields,
		const FieldTuple& lowerBound,
		const FieldTuple& upperBound
	) const;

	/**
	Performs a value-based lookup to find the records on the page that match
	the specified key after projecting them on the specified fields.

	\param projectedFields	The fields on which to project the records on the page.
	\param key				The key against which to match the record projections.
	*/
	std::vector<Record> records_by_value(
		const std::vector<unsigned int>& projectedFields,
		const FieldTuple& key
	) const;

	/**
	Gets the size (in bytes) of the page's buffer.

	\return	The size (in bytes) of the page's buffer.
	*/
	unsigned int size() const;
};

}

#endif
