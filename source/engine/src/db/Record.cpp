/**
 * whery: Record.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/Record.h"

namespace whery {

//#################### CONSTRUCTORS ####################

Record::Record(char *location, const RecordManipulator& manipulator)
:	m_location(location), m_manipulator(manipulator)
{}

Record::Record(const RecordManipulator& manipulator)
:	m_location(NULL), m_manipulator(manipulator)
{}

//#################### PUBLIC INHERITED METHODS ####################

unsigned int Record::arity() const
{
	return m_manipulator.arity();
}

Field Record::field(unsigned int i) const
{
	return m_manipulator.field(m_location, i);
}

//#################### PUBLIC METHODS ####################

void Record::copy_from(const FieldTuple& source) const
{
	if(source.arity() != arity())
	{
		throw std::invalid_argument("It is not possible to copy from a tuple whose arity differs from this record.");
	}

	// Copy the individual fields across. If the field types are not compatible, an exception will be thrown.
	for(size_t i = 0; i < arity(); ++i)
	{
		field(i).set_from(source.field(i));
	}
}

const char *Record::location() const
{
	return m_location;
}

unsigned int Record::size() const
{
	return m_manipulator.size();
}

}
