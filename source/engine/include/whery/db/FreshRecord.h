/**
 * whery: FreshRecord.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_FRESHRECORD
#define H_WHERY_FRESHRECORD

#include <vector>

#include "Record.h"

namespace whery {

/**
\brief An instance of this class represents a freshly-created record that is backed by its own buffer.
*/
class FreshRecord : public Record
{
	//#################### PRIVATE VARIABLES ####################
private:
	/** The buffer backing the record. */
	std::vector<char> m_buffer;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a fresh record backed by a buffer of the correct size.

	\param fieldManipulators		A non-empty array of manipulators for the fields in the record.
	\throw std::invalid_argument	If fieldManipulators is empty.
	*/
	explicit FreshRecord(const std::vector<const FieldManipulator*>& fieldManipulators);

	/**
	Constructs a fresh record backed by a buffer of the correct size.

	\param fieldManipulators		A non-empty array of manipulators for the fields in the record.
	\param projectedFields			A non-empty array specified a way in which the manipulators should be rearranged.
	\throw std::invalid_argument	If fieldManipulators or projectedFields is empty.
	*/
	FreshRecord(
		const std::vector<const FieldManipulator*>& fieldManipulators,
		const std::vector<unsigned int>& projectedFields
	);

	/**
	Constructs a fresh record backed by a buffer of the correct size.

	\param manipulator	The manipulator used to interact with the buffer.
	*/
	explicit FreshRecord(const RecordManipulator& manipulator);

	//#################### COPY CONSTRUCTOR & ASSIGNMENT OPERATOR ####################
public:
	/**
	Constructs a fresh record that is a copy of rhs.

	\param rhs	The record to copy.
	*/
	FreshRecord(const FreshRecord& rhs);

	/**
	Overwrites the contents of this record with the contents of rhs.

	\param rhs	The record from which to assign.
	*/
	FreshRecord& operator=(const FreshRecord& rhs);
};

}

#endif
