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
	Copies the fields of the source record across to this record. The two records
	must have matching arities and the individual field types must be compatible.

	\param source					The record from which to copy fields.
	\throw std::bad_cast			If one or more of the individual field types are incompatible.
	\throw std::invalid_argument	If the records do not have matching arities.
	*/
	void copy_from(const Record& source) const;

	/**
	Gets the location of the record in memory.

	\return	The location of the record in memory.
	*/
	const char *location() const;

	/**
	Gets the overall size of the record (in bytes).

	\return	The overall size of the record (in bytes).
	*/
	unsigned int size() const;
};

}

#endif
