/**
 * whery: Record.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_RECORD
#define H_WHERY_RECORD

#include "FieldTuple.h"
#include "RecordManipulator.h"

namespace whery {

/**
\brief An instance of this class represents a database record.
*/
class Record : public FieldTuple
{
	//#################### PROTECTED VARIABLES ####################
protected:
	/** The location of the record in memory. */
	char *m_location;

	/** The manipulator used to interact with the memory containing the record. */
	RecordManipulator m_manipulator;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a record that is backed by the memory at location and
	that can be interacted with using the specified manipulator.

	\param location		The location of the record in memory.
	\param manipulator	The manipulator used to interact with the memory containing the record.
	*/
	Record(char *location, const RecordManipulator& manipulator);

protected:
	/**
	Constructs a record that can be interacted with using the specified manipulator.
	The memory location for the record must be set separately. This constructor is
	intended for use by derived classes only.

	\manipulator	The manipulator used to interact with the memory containing the record.
	*/
	explicit Record(const RecordManipulator& manipulator);

	//#################### PUBLIC INHERITED METHODS ####################
public:
	virtual unsigned int arity() const;
	virtual Field field(unsigned int i) const;

	//#################### PUBLIC METHODS ####################
public:
	/**
	Gets the overall size of the record (in bytes).

	\return	The overall size of the record (in bytes).
	*/
	unsigned int size() const;
};

}

#endif
