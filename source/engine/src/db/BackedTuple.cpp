/**
 * whery: BackedTuple.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/BackedTuple.h"

namespace whery {

//#################### CONSTRUCTORS ####################

BackedTuple::BackedTuple(char *location, const TupleManipulator& manipulator)
:	m_location(location), m_manipulator(manipulator)
{}

BackedTuple::BackedTuple(const TupleManipulator& manipulator)
:	m_location(NULL), m_manipulator(manipulator)
{}

//#################### PUBLIC INHERITED METHODS ####################

unsigned int BackedTuple::arity() const
{
	return m_manipulator.arity();
}

Field BackedTuple::field(unsigned int i) const
{
	return m_manipulator.field(m_location, i);
}

//#################### PUBLIC METHODS ####################

void BackedTuple::copy_from(const Tuple& source) const
{
	if(source.arity() != arity())
	{
		throw std::invalid_argument("It is not possible to copy from a tuple whose arity differs from this one.");
	}

	// Copy the individual fields across. If the field types are not compatible, an exception will be thrown.
	for(size_t i = 0; i < arity(); ++i)
	{
		field(i).set_from(source.field(i));
	}
}

const char *BackedTuple::location() const
{
	return m_location;
}

unsigned int BackedTuple::size() const
{
	return m_manipulator.size();
}

}
