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

unsigned int Record::size() const
{
	return m_manipulator.size();
}

}
