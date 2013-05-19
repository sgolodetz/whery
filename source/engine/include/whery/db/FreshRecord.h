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

	\param fieldManipulators	A non-empty array of manipulators for the fields in the record.
	*/
	explicit FreshRecord(const std::vector<const FieldManipulator*>& fieldManipulators);

	/**
	Constructs a fresh record backed by a buffer of the correct size.

	\param manipulator	The manipulator used to interact with the buffer.
	*/
	explicit FreshRecord(const RecordManipulator& manipulator);
};

}

#endif
